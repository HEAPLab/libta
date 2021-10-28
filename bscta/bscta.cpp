#include "bscta.h"
#include "libta_math.h"


#include <algorithm>
#include <numeric>

namespace libta {

template <typename T>
std::shared_ptr<Response> BSCTimingAnalyzer<T>::perform_analysis(std::shared_ptr<Request<T>> req) {

    //Copy in vector and sort it
	auto trace_sorted = req->get_all();

	if(trace_sorted.size() <= 10) {
		throw TimingAnalyzerError("The number of samples is '10' or less in the Request. "
							      "Please get more samples.", error_t::INVALID_DATA);
	}

	std::sort(trace_sorted.begin(),trace_sorted.end(), std::greater<T>() );
	
	//Init Required stuff
	const int file_size = trace_sorted.size();
	const int half_size = floor(file_size/2);
	const int minvalues = 10;

	std::vector<T> cv(half_size-2);
	std::vector<T> limitabove(half_size-2);

	//Compute the CV = Coefficient of Variation. It is the std_dev/mean
	//Need at lest 2 samples to compute the std_deviation
	for(int rejectedSamples = 2; rejectedSamples < half_size; rejectedSamples++) {
		int usedSamples=half_size - rejectedSamples;
		T mean = getUnbiasedMean(trace_sorted,0,usedSamples);
		T std_deviation = getUnbiasedStdDeviation(trace_sorted,0,usedSamples);
		cv[rejectedSamples-2]= std_deviation/mean;
	}

	//Compute the region of acceptance for the exponential tail.
	//This is the red cone in a CV-plot
	//Impose the template enforce the type casting of the input
	//assert(limitbelow.size() == limitabove.size());
	for(size_t i=0; i<limitabove.size(); i++) {
		T tmp = (1.96/internalSqrt<T>(half_size-i));
		limitabove[i]=1 + tmp;
		//limitbelow[i]=1 - tmp;
	}

	assert(limitabove.size() == cv.size());

	int nelems =  0;
	//Find the values below the limitabove starting from the end
	for(size_t i=cv.size()-1; i>=0; i--) {
		        //Impose the template enforce the type casting of the input
		T upperLimit = 1 + (1.96/internalSqrt<T>(half_size-i));
		if(cv[i]>=upperLimit)
		    break;
		nelems++;
	}

    //We need at least minvalues samples to estimate the tail.
    //If this property is not satisfied, we're force to fail.
    if(nelems < minvalues) {
		throw TimingAnalyzerError("Minvalues samples are not satisfied.\nYou need to get more samples.", error_t::INVALID_DATA);
	}

    T excessesMean = getUnbiasedMean(trace_sorted,0,nelems);
    std::vector<T> tailValues(trace_sorted.begin(), trace_sorted.begin()+nelems);
    const T rate = 1/excessesMean;
    //Impose the template enforce the type casting of the input
    const T ratelow = rate * (1 + (1.96/internalSqrt<T>(nelems)));
    const T ratehigh = rate *(1 - (1.96/internalSqrt<T>(nelems)));

    //Biggest value in nelems is the MET
   // const T rank_end =20 * trace_sorted[0] ;

    const T rank_end = 20 * (trace_sorted[0] - trace_sorted[nelems-1]);
    const T rank_start = 0 ;
    const T rank_step = (rank_end - rank_start)/ (rank_length-1);
    //Generate values for rank
    std::vector<T> rank(rank_length,0);
    std::vector<T> probCCDF(rank_length,0);
    std::vector<T> probCCDFlow(rank_length,0);
    std::vector<T> probCCDFhigh(rank_length,0);
    arange(rank,rank_start,rank_step);

    //The most expensive step is this one. Especially if you use long double
    setExponSurvivalFunction(probCCDF,rank, rate);
    setExponSurvivalFunction(probCCDFlow,rank, ratelow);
    setExponSurvivalFunction(probCCDFhigh,rank, ratehigh);
    //Add mean to all the rank values.
    //TODO!!! already create rank starting from the mean and keep it into
    // account while generating the survival function.
    for( auto &v : rank )  v += trace_sorted[nelems-1];

    //TODO estimate the real type of tail


	// TODO add mean value

	this->low_gpd = std::make_shared <ResponseEVTDistribution> (distribution_type_e::EVT_GPD_2PARAM);
	this->low_gpd->set_parameters(0, ratelow, 0);
	this->high_gpd = std::make_shared <ResponseEVTDistribution> (distribution_type_e::EVT_GPD_2PARAM);
	this->high_gpd->set_parameters(0, ratehigh, 0);

	auto gpd = std::make_shared <ResponseEVTDistribution> (distribution_type_e::EVT_GPD_2PARAM);
	gpd->set_parameters(0, rate, 0);
    return gpd;

}

template <typename T>
T BSCTimingAnalyzer<T>::get_wcet_at_p(double x, double mu, double sg, double xi) const {
    if (p <= 0. || p >= 1.) {
        throw std::invalid_argument("The probability value is not valid.");
    }

    double quantile;

    if (xi != 0.) {
        quantile = mu + sg  * (1. - std::pow(1. - p, -xi)) / (-xi);
    } else {
        quantile = mu - sg * std::log(1. - p);
    }

    return quantile;
}

template <typename T>
T BSCTimingAnalyzer<T>::get_high_wcet_at_p(double x) const {
	return BSCTimingAnalyzer<T>::get_wcet_at_p(x, this->high_gpd->get_mu(), this->high_gpd->get_sigma(), this->high_gpd->get_xi()); 
}

template <typename T>
T BSCTimingAnalyzer<T>::get_low_wcet_at_p(double x) const {
	return BSCTimingAnalyzer<T>::get_wcet_at_p(x, this->low_gpd->get_mu(), this->low_gpd->get_sigma(), this->low_gpd->get_xi()); 
}

template class BSCTimingAnalyzer<unsigned int>;
template class BSCTimingAnalyzer<int>;
template class BSCTimingAnalyzer<unsigned long>;
template class BSCTimingAnalyzer<long>;
template class BSCTimingAnalyzer<float>;
template class BSCTimingAnalyzer<double>;
template class BSCTimingAnalyzer<long double>;

} //namespace libta
