#include "libta/libta.h"

#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>


/**
 * Double is a good balance between accuracy and performance.
 * the smallest double denormal value is around  4.94e-324 .
 * In single float the smaller value (denormal) is 2−149 ≈ 1.4e−45
 * Performance between double and float are comparable also in this case
**/
#define type  double 

using namespace libta;
using namespace std;
using namespace neither;


void getDistributionInformation(std::shared_ptr<EVTDistribution<type> > distribution){

    std::cout<<"Tail values used: "<<distribution->getTailValues().size()<<std::endl;
    std::cout<<"MET: "<<distribution->getMaxExecutionTime()<<std::endl;

    const std::vector<type> probs ={1e-3,1e-6,1e-9,1e-12,1e-15};

    for (auto prob : probs) {
        distribution->getPWCET(prob).join(
        [](auto errorResponse) {
            cout<<"Can't get pWCET value. Reason: "<<errorResponse.get_message()<<endl;
        }, // error-case
        [prob](auto result) {
            cout<<prob<<": "<<result.pWCET<<endl;    // success-case
        }
                                          );   
    }
    cout<<"Confidence intervals (95%)"<<endl;
     for (auto prob : probs) {
        string p_low=distribution->getPWCETLow(prob).join(
        [](auto errorResponse) {
            return "Can't get pWCETlow value. Reason: "+errorResponse.get_message();
        }, // error-case
        [](auto result) {return std::to_string(result.pWCET);}    // success-case
        );   
        string p_high=distribution->getPWCETHigh(prob).join(
        [](auto errorResponse) {
            return "Can't get pWCEThigh value. Reason: "+errorResponse.get_message();
        }, // error-case
        [](auto result) {return std::to_string(result.pWCET);}    // success-case
        );
        cout<<prob<<": "<<p_low<<" "<<p_high<<endl;   
    }

}

int main(int argc, char * argv[]) {

    //Fill the request with the values from a file
    auto req = std::make_shared<Request<type>>();
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
    auto distEither = analyzer.estimate_distribution( req );


    distEither.join(
    [](auto errorDistribution) {
        cout<<"Cant't estimate distribution. Reason: "<<errorDistribution.get_message()<<endl;
    }, // error-case
    [](auto x){
        getDistributionInformation(x); }   // success-case
            );
    

} //end main

