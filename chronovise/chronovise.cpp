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

int main() {
    using namespace libta;

    ChronoviseTimingAnalyzer cta;

    std::shared_ptr<Request<unsigned long>> req = std::make_shared<Request<unsigned long>>();

    static std::random_device random_dev;
    static std::mt19937 mt(random_dev());
    static std::normal_distribution<double> distribution(8.0,1.0);
    for(int i = 0; i < 20000; i++)
        req->add_value(distribution(mt));

    std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::dynamic_pointer_cast<ResponseWCET<unsigned long>>(cta.perform_analysis(req));

    std::cout << "WCET Value: " << rwcet->get_wcet_value() << std::endl;

    return 0;
}