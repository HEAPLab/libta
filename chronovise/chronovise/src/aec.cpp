#include "aec.hpp"
#include "global.hpp"
#include "utility/utility_hmi.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>

#define COEFF_INCREASE_SAMPLE_SIZE 0.2

namespace chronovise {

template <typename T_INPUT, typename T_TIME>
void AbstractExecutionContext<T_INPUT,T_TIME>::check_preconditions() const noexcept {

    assert( input_gen     != nullptr && "You must call set_input_source() before run.");
    assert( evt_approach  != nullptr && "You must call set_evt_approach() before run.");
    assert( evt_estimator != nullptr && "You must call set_evt_estimator() before run.");
    assert( merger_tech   != merger_type_t::UNKNOWN && "You must call set_merging_technique() before run.");

}

template <typename T_INPUT, typename T_TIME>
void AbstractExecutionContext<T_INPUT,T_TIME>::run() {

    exit_code_t ret;

    ret = this->onSetup();
    if (ret != AEC_OK) {
        print_error("onSetup() returns error code " + std::to_string(ret));
    }

    // Performs some checks that the user correctly set all member of the AEC class
    // It will fail with a assert-failure in case preconditions not respected.
    this->check_preconditions();

    // Provide to EVT estimator the information of which approach will be used to create the
    // estimator input dataset.
    this->evt_estimator->set_source_evt_approach(&typeid(*this->evt_approach));

    // Configure the minimal number of iterations given by tests.
    this->set_min_iterations();

    // This is the core of the estimation routine and represent the external cycle.
    // Please refer to diagram AD#1
    external_cycle();

    // If the user requeste the trace-merging technique, it means that we didn't perform
    // the estimation inside the cycle but we need to do that here.
    if (merger_tech == merger_type_t::TRACE_MERGE) {
        execute_analysis();
    }

    ret = this->onRelease();
    if (ret != AEC_OK) {
        print_error("onRelease() returns error code " + std::to_string(ret));
    }

    VERB(std::cerr << std::endl);
    
}

template <typename T_INPUT, typename T_TIME>
void AbstractExecutionContext<T_INPUT,T_TIME>::external_cycle() {

    bool require_more_samples = true;

    while (require_more_samples) {
        VERB(std::cerr << std::endl);
        current_input = input_gen->get();

        exit_code_t ret = this->onConfigure();

        switch (ret) {
            case AEC_OK:
                // The use requested to stop the capture
                require_more_samples = false;
                // The safety of the estimation is not guaranteed in this case,
                // since we didn't perform any safety check
                this->safety.set_input_representativity(false);
            break;
            case AEC_SLOTH:
            {
                bool cont = false;

                if (representativity_tests.size() == 0) {
                    throw std::runtime_error("You cannot return AEC_SLOTH without "
                                             "representativity tests.");
                }

                for (auto &test : representativity_tests) {
                    test->run(measures);
                    if (test->is_reject()) {
                        cont = true;
                        break;
                    }
                }

                if (! cont) {
                    require_more_samples = false;
                    this->safety.set_input_representativity(true);
                    break;
                }

            }
            [[fallthrough]];
            case AEC_CONTINUE:
                input_iteration++;
                internal_cycle();
            break;

            default:
                print_error("onConfigure() returns error code " + std::to_string(ret));
            break;
        }
    }
}

template <typename T_INPUT, typename T_TIME>
void AbstractExecutionContext<T_INPUT,T_TIME>::internal_cycle() {

    exit_code_t ret;

    iteration = 0;

    aec_status_t ret_analysis;
    bool keep_going = true;

    while(keep_going) {

        ret = this->onRun();
        if (ret != AEC_OK) {
            print_error("onRun() returns error code " + std::to_string(ret));
        }

        ret = this->onMonitor();

        switch(ret) {
            case AEC_CONTINUE:
            break;
            case AEC_OK:
                keep_going = false;
            break;
            case AEC_SLOTH:

                assert(min_nr_iterations_train > 0 &&
                        "Selected EVT approach does not allow AEC_SLOTH.");
                assert(min_nr_iterations_tests > 0 &&
                        "You cannot use AEC_SLOTH without at least one test.");

                keep_going = iteration < min_nr_iterations_total;
            break;
            default:
                print_error("onMonitor() returns error code " + std::to_string(ret));
            break;
        }

        if (iteration % 10 == 0) VERB(std::cerr << hmi_10_iteration);

        if (! keep_going) {
            // We may be here in two cases:
            // 1 - the onMonitor() returned AEC_OK
            // 2 - the onMonitor() returned AEC_SLOTH and sufficient number of iteration reached
            // Now, we have to continue to get sample only if we are not in AEC_OK case
            // and execute_analysis() failed.

            if (iteration >= min_nr_iterations_total) {
                ret_analysis = execute_analysis();
            } else {
                // In this case the application returned AEC_OK but no sufficient samples are estimated
                ret_analysis = aec_status_t::NO_ENOUGH_SAMPLES;
            }

            if (ret_analysis == aec_status_t::OK) {
                break;
            }

            // If we are here onMonitor() returned AEC_SLOTH
            keep_going = true;

            switch(ret_analysis) {
                case aec_status_t::REJECT_SAMPLE_TEST:
                    VERB(std::cerr << hmi_reject_sample);
                    keep_going = false;
                break;
                case aec_status_t::FAIL_EVT_ESTIMATOR:
                    VERB(std::cerr << hmi_fail_estimator);
                    keep_going = false;
                break;
                case aec_status_t::REJECT_POST_EVT_TEST:
                    VERB(std::cerr << hmi_reject_evt);
                    keep_going = false;
                break;
                case aec_status_t::NO_ENOUGH_SAMPLES:
                    VERB(std::cerr << hmi_no_enough_samples);
                    keep_going = false;
                break;
                default:
                    VERB(std::cerr << hmi_more_sample);
                break;
            }
        }

        iteration++;
    }

    if (ret_analysis == aec_status_t::OK) {
        VERB(std::cerr << hmi_success);
    } else {
        this->safety.set_input_representativity(false);
    }
}

template <typename T_INPUT, typename T_TIME>
aec_status_t AbstractExecutionContext<T_INPUT,T_TIME>::execute_analysis() {

    // Perform the sample tests on the input data
    for (auto &test : sample_tests) {
        test->run(measures);
        if (test->is_reject()) {
            return aec_status_t::REJECT_SAMPLE_TEST;
        }
    }

    // Create a pool set to manage training e test
    MeasuresPoolSet<T_INPUT, T_TIME> mps(this->measures, 1.-samples_test_reserve, current_input);

    // Perform BM or POT based on what the user provided
    this->evt_approach->perform(mps);

    // Now measures represents the old pool of values. We now want to
    // get the new BM or POT pool.
    const auto &measures_to_estimate = this->evt_approach->get_training_pool();
    const auto &measures_test = this->evt_approach->get_test_pool();

    if (measures_to_estimate.size() < this->evt_estimator->get_minimal_sample_size()) {
        min_nr_iterations_total *= (1.+COEFF_INCREASE_SAMPLE_SIZE);
        return aec_status_t::FAIL_EVT_APP_MIN_SAMPLE_SIZE;
    }

    auto lambda_check = [&measures_test](const auto &test) {
        return measures_test.size() < test->get_minimal_sample_size();
    };

    if (std::any_of(post_run_tests.cbegin(), post_run_tests.cend(), lambda_check)) {
        min_nr_iterations_total *= (1.+COEFF_INCREASE_SAMPLE_SIZE);
        return aec_status_t::FAIL_POST_RUN_TEST_SAMPLE_SIZE;
    }

    if (std::any_of(post_evt_tests.cbegin(), post_evt_tests.cend(), lambda_check)) {
        min_nr_iterations_total *= (1.+COEFF_INCREASE_SAMPLE_SIZE);
        return aec_status_t::FAIL_POST_EVT_TEST_SAMPLE_SIZE;
    }

    // We can now estimated the EVT distribution...
    this->evt_estimator->run(measures_to_estimate);

    if (this->evt_estimator->get_status() != estimator_status_t::SUCCESS) {
        // TODO: Handle other result values
        return aec_status_t::FAIL_EVT_ESTIMATOR;
    }

    std::shared_ptr<Distribution> evd = this->evt_estimator->get_result();

    // And then test it...
    for (auto &test : post_evt_tests) {
        test->set_ref_distribution(evd);
        test->run(measures_test);
        if (test->is_reject()) {
            return aec_status_t::REJECT_POST_EVT_TEST;
        }
    }

    ev_dist_estimated.push_back(evd);
    wcots.push(current_input,this->measures.max());
    return aec_status_t::OK;
}

template <typename T_INPUT, typename T_TIME>
void AbstractExecutionContext<T_INPUT,T_TIME>::set_min_iterations() noexcept {

    // Calculate the minimum number of iterations is a bit tricky.

    // First of all, the number of samples for the training set is defined by the EVT approach
    // used. E.g. BM method must guarantee at least 2 blocks.
    this->min_nr_iterations_train = this->evt_approach->get_minimal_sample_size();

    this->min_nr_iterations_tests = 0;

    // The minimal number of tests depends obviously on used tests and if a reliability requirement
    // is provided or not.
    if (safety.has_reliability_requirement()) {

        // Check if the tests have all of them the power estimation routine.
        auto lambda_power = [](const auto &x){return !x->has_power();};
        bool any_without_power =
        std::any_of(    sample_tests.cbegin(),
                sample_tests.cend(),
                lambda_power
        )
        ||
        std::any_of(    post_run_tests.cbegin(),
                post_run_tests.cend(),
                lambda_power
        )
        ||
        std::any_of(    post_evt_tests.cbegin(),
                post_evt_tests.cend(),
                lambda_power
        );

        //  If the power estimation routine is not present for at least one test we have to
        // consider the estimation unsafe
        if (any_without_power) {
            this->safety.set_evt_safe(false);
        } else {
            // Otherwise we can calculate the minimal number of samples from the maximum
            // of them
            unsigned long min_iter = 0;
            float rel_req = safety.get_reliability_requirement();
            auto lambda_max = [&min_iter, rel_req](const auto &test) {
                min_iter = std::max(min_iter, test->get_minimal_sample_size(rel_req));
            };

            std::for_each(sample_tests.cbegin(), sample_tests.cend(), lambda_max); 
            std::for_each(post_run_tests.cbegin(), post_run_tests.cend(), lambda_max); 
            std::for_each(post_evt_tests.cbegin(), post_evt_tests.cend(), lambda_max); 

            this->min_nr_iterations_tests = min_iter;
        }

    }

    // If the reliability is not enable or not all the tests has the power estimation routine,
    // let's use the minimal value to run tests
    if (this->min_nr_iterations_tests == 0) {
        unsigned long min_iter = 0;
        auto lambda_max = [&min_iter](const auto &test) {
            min_iter = std::max(min_iter, test->get_minimal_sample_size());
        };
        std::for_each(sample_tests.cbegin(), sample_tests.cend(), lambda_max); 
        std::for_each(post_run_tests.cbegin(), post_run_tests.cend(), lambda_max); 
        std::for_each(post_evt_tests.cbegin(), post_evt_tests.cend(), lambda_max); 

        this->min_nr_iterations_tests = min_iter;
    }

    // Now: since the minimal number of samples per test may be less than the required for BM
    // (or any other EVT approach), we have to compute it considering the size of the BM output
    this->min_nr_iterations_tests = std::max(this->min_nr_iterations_tests, this->min_nr_iterations_train);

    // At last, the training set and test set is divided by the ratio samples_test_reserve, so
    // let's compute the total number.
    this->min_nr_iterations_total = std::ceil(
                    std::max(this->min_nr_iterations_tests / samples_test_reserve, 
                    this->min_nr_iterations_train / (1 - samples_test_reserve))
                    );

}

template <typename T_INPUT, typename T_TIME>
void AbstractExecutionContext<T_INPUT,T_TIME>::set_evt_approach(std::unique_ptr<EVTApproach<T_INPUT, T_TIME>> evt_approach,
                float samples_test_reserve) noexcept {
    assert(samples_test_reserve >= 0. && samples_test_reserve <= 1.);
    this->evt_approach = std::move(evt_approach);
    this->samples_test_reserve = samples_test_reserve;
}


template <typename T_INPUT, typename T_TIME>
T_TIME AbstractExecutionContext<T_INPUT,T_TIME>::get_pwcet_wcet(double probability) const noexcept {
    if (ev_dist_estimated.size() == 0) {
        return T_TIME(NAN);
    }

    std::list<pWCET<T_TIME>> pwcets;
    for (const auto & evd : ev_dist_estimated) {
        pwcets.push_back(pWCET<T_TIME>(evd));
    }
    return pWCET<T_TIME>::get_cumulative_wcet(pwcets, probability);
}

template <typename T_INPUT, typename T_TIME>
double AbstractExecutionContext<T_INPUT,T_TIME>::get_pwcet_probability(T_TIME wcet) const noexcept {
    if (ev_dist_estimated.size() == 0) {
        return NAN;
    }

    std::list<pWCET<T_TIME>> pwcets;
    for (const auto & evd : ev_dist_estimated) {
        pwcets.push_back(pWCET<T_TIME>(evd));
    }
    return pWCET<T_TIME>::get_cumulative_probability(pwcets, wcet);
}


TEMPLATE_CLASS_IMPLEMENTATION(AbstractExecutionContext);

} // chronovise

