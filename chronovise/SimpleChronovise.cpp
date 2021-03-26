#include "SimpleChronovise.hpp"

#include "chronovise/evt/evtapproach_bm.hpp"
#include "chronovise/statistical/estimator_mle.hpp"
#include "chronovise/statistical/test_ks.hpp"
#include "chronovise/statistical/test_ljung_box.hpp"
#include "chronovise/input/generator_null.hpp"

using namespace chronovise;

using exit_code_t = AbstractExecutionContext<unsigned int, double>::exit_code_t;

SimpleChronovise::SimpleChronovise(std::vector<unsigned long> & input) noexcept : data(input) {
    it = data.begin();
}

exit_code_t SimpleChronovise::onSetup() noexcept {

    /* ********** PRE-RUN SECTION  ********** */

    // In this simple example we are not interested in multi-input programs, so let's define a
    // a Null input generator.
    std::unique_ptr<chronovise::NullInputGenerator> null_input_gen(new chronovise::NullInputGenerator());
    this->set_input_source(std::move(null_input_gen));

    /* ********** POST-RUN SECTION ********** */

    // Set LjungBox test as the test to be used after the sampling phase.
    // 1st parameter: significance level
    // 2nd parameter: number of lags for the test 
    this->use_LjungBox_as_sample_test(0.01, 10);

    // Select ENVELOPE as merging technique. The possible values are:
    // ENVELOPE: different inputs provide different pWCETs estimation
    // MERGER:   different inputs generating samples that are merged and only 1 pWCET estimation
    //           is performed.
    this->set_merging_technique(merger_type_e::ENVELOPE);

    // Select the EVT approach to use:
    // - Block Maxima -> It will generate a GEV
    // - PoT          -> It will generate a GPD
    this->use_evt_approach_CV(0.1);
    //this->use_evt_approach_PoT(13.5, 0.1);

    // Select the estimator. Currently the only one available is the Maximum-Likelihood-Estimator
    this->use_estimator_CV();

    // Select the Kolmogorov-Smirnov test as test on the estimated probability distribution for
    // pWCET
//    this->use_KS_as_post_evt_test(0.01);

    // Let's print some debug information
    this->print_configuration_info();
    this->print_legend();

    return AEC_OK;
}

exit_code_t SimpleChronovise::onConfigure() noexcept
{
    // Since no input generation is provided, we want to execute the estimation only 1 time.
    if (get_input_iteration() > 0)
        return AEC_OK;

    return AEC_CONTINUE;
}

exit_code_t SimpleChronovise::onRun() noexcept {
    
    if(it != data.end()){
        this->add_sample(*it);
        it++;
    }

    
    return AEC_OK;
}

exit_code_t SimpleChronovise::onMonitor() noexcept {
    
    if(it == data.end())
        return AEC_GENERIC_ERROR;
    else
        return AEC_SLOTH; //Let chronovise to decide when it's time to stop
}

exit_code_t SimpleChronovise::onRelease() noexcept {

    return AEC_OK;
}

