#include "sec.hpp"
#include "SimpleChronovise.hpp"

#include "libta.h"
#include <iostream>
#include <memory>

#include <iostream>
#include <iomanip>
#include "evt/evtapproach_bm.hpp"
#include "statistical/estimator_mle.hpp"
#include "statistical/test_ks.hpp"
#include "statistical/test_ljung_box.hpp"
#include "input/generator_null.hpp"


namespace libta {

    class ChronoviseTimingAnalyzer : public TimingAnalyzer<unsigned long> {

    public:

        virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<unsigned long>> req) {

            const auto &exec_times = req->get_all();
            auto times = std::vector<unsigned long>(exec_times);

            SimpleChronovise solver(times);
            solver.run();

            (void)exec_times;
            std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::make_shared<ResponseWCET<unsigned long>>();
            rwcet->set_wcet_value(solver.get_pwcet_wcet(0.9999));
            return rwcet; 
        }

    };

};    // libta,
