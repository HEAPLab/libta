#include "simple_hello_world.hpp"

#include "evt/evtapproach_cv.hpp"
#include "statistical/estimator_cv.hpp"
#include "statistical/test_ks.hpp"
#include "statistical/test_ljung_box.hpp"
#include "input/generator_null.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace chronovise;

using exit_code_t = AbstractExecutionContext<unsigned int, double>::exit_code_t;


SimpleHelloWorld::SimpleHelloWorld() noexcept {
    // Nothing to do...
}

exit_code_t SimpleHelloWorld::onSetup() noexcept {

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


    // Let's print some debug information
    this->print_configuration_info();
    this->print_legend();

    // Gather input values from file
    this->use_file_as_source("test_cv_values.txt");

    return AEC_OK;
}

exit_code_t SimpleHelloWorld::onConfigure() noexcept
{
    // Since no input generation is provided, we want to execute the estimation only 1 time.
    if (get_input_iteration() > 0)
        return AEC_OK;

    return AEC_CONTINUE;
}


exit_code_t SimpleHelloWorld::onRun() noexcept {

//    std::cout << get_iteration() << std::endl;

    try {

        // Get a sample from the file specified
        // in onSetup().
        this->add_sample_from_file();

    } catch( std::ios::failure ex) {
        std::cout << std::endl << "INPUT ERROR: "
                  << ex.what() << std::endl;
        return AEC_INPUT_ERROR;
    }


    return AEC_OK;
}

exit_code_t SimpleHelloWorld::onMonitor() noexcept {

    if (get_iteration() >= 10000) {
        return AEC_OK;
    } else {
        return AEC_CONTINUE;
    }
}

exit_code_t SimpleHelloWorld::onRelease() noexcept {

    this->print_evt_info();

    // Print of the results, etc.
    this->print_distributions_summary();

    this->print_wcots();

    std::cout << "pWCET(p=0.999999) is: " << this->get_pwcet_wcet(0.999999) << std::endl;
    std::cout << "pWCET(p=1-10^(-9)) is: " << this->get_pwcet_wcet(1 - std::pow(10., -9.)) << std::endl;
    double prob= this->get_pwcet_probability(16.8);
    std::cout << "pWCET(WCET=16.8) is: " << std::setprecision(16) << prob << std::endl;

    return AEC_OK;
}

