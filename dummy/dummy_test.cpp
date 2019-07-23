#include "libta.h"

#include <iostream>
#include <memory>

namespace libta {

    class MyTimingAnalyzer : public TimingAnalyzer<unsigned long> {

    public:

        virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<unsigned long>> req) {

            const auto &exec_times = req->get_all();

            // Do something

            (void)exec_times;

            std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::make_shared<ResponseWCET<unsigned long>>();
            rwcet->set_wcet_value(12345);
            return rwcet; 
        }

    };

};    // libta,


int main() {

    using namespace libta;

    MyTimingAnalyzer mta;

    std::shared_ptr<Request<unsigned long>> req = std::make_shared<Request<unsigned long>>();
    req->add_value(10000);
    req->add_value(12000);

    std::shared_ptr<ResponseWCET<unsigned long>> rwcet = std::dynamic_pointer_cast<ResponseWCET<unsigned long>>(mta.perform_analysis(req));

    std::cout << "WCET Value: " << rwcet->get_wcet_value() << std::endl;

}

