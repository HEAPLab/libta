#include "SimpleChronovise.hpp"
#include "chronovise.hpp"

#include <iostream>
#include <memory>

#include <iostream>
#include <iomanip>
#include "evt/evtapproach_bm.hpp"
#include "statistical/estimator_mle.hpp"
#include "statistical/test_ks.hpp"
#include "statistical/test_ljung_box.hpp"
#include "input/generator_null.hpp"

#include "utility/oop.hpp"
#include "evt/gpd_distribution.hpp"

namespace libta {

    std::shared_ptr<Response>  ChronoviseTimingAnalyzer::perform_analysis(std::shared_ptr<Request<unsigned long>> req) 
    {
        const auto &exec_times = req->get_all();
        auto times = std::vector<unsigned long>(exec_times);

        SimpleChronovise solver(times);
        solver.run();
        
        (void)exec_times;
        std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::make_shared<ResponseWCET<unsigned long>>();
        rdist = std::make_shared<ResponseEVTDistribution>(libta::distribution_type_t::EVT_GPD_3PARAM);
        rwcet->set_wcet_value(solver.get_pwcet_wcet(0.9999));   

        auto it = std::dynamic_pointer_cast<const chronovise::GPD_Distribution> (*solver.get_estimated_distributions().cbegin());                        
        rdist->set_parameters(it->get_location(),it->get_scale(),it->get_shape());
        return rwcet;
    } 

    std::shared_ptr<Response> ChronoviseTimingAnalyzer::get_ResponseEVTDistribution()
    {
        return rdist;
    }
};    // libta,

// int main() {
//     using namespace libta;

//     ChronoviseTimingAnalyzer cta;

//     std::shared_ptr<Request<unsigned long>> req = std::make_shared<Request<unsigned long>>();

//     static std::random_device random_dev;
//     static std::mt19937 mt(random_dev());
//     static std::normal_distribution<double> distribution(84.0,3.2);
//     for(int i = 0; i < 20000; i++)
//         req->add_value(distribution(mt));

//     std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::dynamic_pointer_cast<ResponseWCET<unsigned long>>(cta.perform_analysis(req));
//     std::shared_ptr<ResponseEVTDistribution> rdist = std::dynamic_pointer_cast<ResponseEVTDistribution>(cta.get_ResponseEVTDistribution());
      
//     std::cout << "WCET Value: " << rwcet->get_wcet_value() << std::endl;
//     std::cout << "mu: " << rdist->get_mu() << " sigma: " << rdist->get_sigma() << " xi: " << rdist->get_xi() << std::endl;

//     return 0;
// }