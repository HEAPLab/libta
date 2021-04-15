#ifndef LIBTA_CHRONOVISE_H_
#define LIBTA_CHRONOVISE_H_

#include "libta.h"

namespace libta {

class ChronoviseTimingAnalyzer : public TimingAnalyzer<unsigned long> {
    public:
        virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<unsigned long>> req);
        std::shared_ptr<ResponseWCET<unsigned long>> get_WCET();

    private:
        std::shared_ptr<ResponseWCET<unsigned long>> rwcet;

};

}	// namespace libta

#endif
