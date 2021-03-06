#include "SimpleChronovise.hpp"
#include "chronovise.h"

#include <memory>

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
        
        std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::make_shared<ResponseWCET<unsigned long>>();
        auto rdist = std::make_shared<ResponseEVTDistribution>(libta::distribution_type_t::EVT_GPD_3PARAM);
        rwcet->set_wcet_value(solver.get_pwcet_wcet(0.9999));   

        auto it = std::dynamic_pointer_cast<const chronovise::GPD_Distribution> (*solver.get_estimated_distributions().cbegin());                        
        rdist->set_parameters(it->get_location(),it->get_scale(),it->get_shape());
        
        this->rwcet = rwcet;
        
        return rdist;
    } 

    std::shared_ptr<ResponseWCET<unsigned long>> ChronoviseTimingAnalyzer::get_WCET()
    {
        return rwcet;
    }
}    // libta

