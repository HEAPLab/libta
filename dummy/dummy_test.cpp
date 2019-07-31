#include "libta.h"

#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>


#define type long double
int main(int argc, char * argv[]) {

    using namespace libta;
    using namespace std;
    using namespace neither;

    auto req = std::make_shared<Request<type>>();

    //FILL REQUEST VALUE
    char * end;
    std::string line;
    std::ifstream myfile (argv[1]);
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
            //std::cout << line << '\n';
            char * ptr = &line[0];
            req->add_value(strtold(ptr, &end));
        }
        myfile.close();
    } else {
        std::cout << "Unable to open file";
    }

    DistributionAnalyzer<type> analyzer;
    auto distEither = analyzer.perform_analysis( req );
    
    std::shared_ptr< EVTDistribution<type> > distribution = distEither.join(
    [](auto errorDistribution) { return std::shared_ptr<EVTDistribution<type> > (
        new EVTDistribution<type>(distribution_type_e::EVT_EMPTY,
        {},{},{},{},{})) ; }, // error-case
    [](auto x) { return x; } // success-case
   );

    std::cout<<"Tail values used: "<<distribution->getTailValues().size()<<std::endl;
    std::cout<<"MET: "<<distribution->getMaxExecutionTime()<<std::endl;

    for (auto prob : {1e-3,1e-6,1e-9,1e-12,1e-15}){
           ResponsePWCET<type> resultWCET =  distribution->getWCET(prob).join(
              [](auto errorPWCET) { return ResponsePWCET<type>(0); }, // error-case
                 [](auto x) { return x; } // success-case
            );
        cout<<prob<<": "<<resultWCET.pWCET<<endl;
    }
    /*
      std::cout << "WCET Value: " << rwcet->get_wcet_value() << std::endl;
      std::cout << "PWCET Value: " << rwcet->get_pwcet_value() << std::endl;
      std::cout << "Micv used: " << rwcet->get_mincv_used() << std::endl;
      std::cout << "Error code : " << rwcet->get_error()  <<std::endl;
    */

}

