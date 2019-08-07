/** @file libhwrel.h
 * The main header file for libta.
 *
 * This header contains the main classes to be used for interfacing the resource manager with the
 * underlying software that computes the WCET timing analysis.
 */
#ifndef LIBTA_H_
#define LIBTA_H_

#include <cassert>
#include <memory>
#include <vector>
#include <tuple>
#include <algorithm>

#include <cmath>
#include <numeric>

#include <iostream>

#include "neither/neither.hpp"

#include "libta/libta_math.h"


namespace libta {

//
// ---------------------------------- ENUMERATIONS ----------------------------------
//

/**
 * @brief The enumeration for the response type
 *
 * The TimingAnalyzer returns a general Response class and this
 * value represents with child-class actually is.
 */
typedef enum class response_type_e {
    PWCET,
    INVALID_DATA,
    INVALID_DISTRIBUTION
} response_type_t;

/**
 * @brief The enumeration for type of distribution
 *
 * If the output is a distribution, this datatype is used to identify the distribution class.
 */
typedef enum class distribution_type_e {
    EVT_EXPONENTIAL,
    EVT_LIGHT_TAIL,
    EVT_HEAVY_TAIL,
    EVT_EMPTY

} distribution_type_t;

//
// ------------------------------------ CLASSES ------------------------------------
//

/**
 * @brief A generic Response with a code
 *
 */
class Response {

public:

    /**
     * @brief The Response class constructor
     * @param res_type  The type of response
      */
    Response(response_type_t resp_type) : resp_type(resp_type) {

    }

    /**
     * @brief The Response class default destructor
      */
    virtual ~Response() = default;

    /** @brief Getter for reponse type */
    inline response_type_t get_response_type() const {
        return this->resp_type;
    }

protected:
    response_type_t resp_type;
};

/**
 * @brief An Invalid Response. It has a string message
 *
 */
class ResponseInvalid : public Response {

public:

    ResponseInvalid(std::string msg, response_type_t invalidType ):
        Response(invalidType),
        message(msg) { }

    ResponseInvalid& operator=(const ResponseInvalid& src) {
        if(&src == this)
            return *this;
        this->message = src.message;
        this->resp_type = src.resp_type;
        return *this;
    }

    std::string get_message() const {
        return this->message;
    }
protected:
    std::string message;
};

/**
 * @brief An Invalid Response when trying to produce a distribution
 *
 */
class ResponseInvalidDistribution : public ResponseInvalid {
public:
    //TODO add meaningful message
    ResponseInvalidDistribution():
        ResponseInvalid("Cannot use this distribution", response_type_t::INVALID_DISTRIBUTION){}
};


/**
 * @brief An Invalid Response when trying collect pWCET from a distribution that can't provide it
 *
 */
class ResponseInvalidData : public ResponseInvalid {
public:
    //TODO add meaningful message
    ResponseInvalidData():
        ResponseInvalid("Not Enough points in given data", response_type_t::INVALID_DATA) {}
};


/**
 * @brief A response that contain a pWCET value
 *
 */
template <typename T>
class ResponsePWCET : public Response {

public:

    /**
     * @brief The ResponseWCET class constructor
      */
    ResponsePWCET(T _pWCET) : Response(response_type_t::PWCET),
        pWCET(_pWCET)
    {

    }
    ResponsePWCET() = delete;

    ResponsePWCET(const ResponsePWCET<T> & src):Response(src.get_response_type()),
        pWCET(src.pWCET)
    {}

    ResponsePWCET<T> & operator=(const ResponsePWCET<T>& src) {
        if(&src == this)
            return *this;
        this->pWCET = src.pWCET;
        return *this;
    }

    /**
     * @brief The ResponseWCET class default destructor
      */
    virtual ~ResponsePWCET() = default;

    T pWCET;

};


/**
 * @brief The class returned by the timing analysis tool when the output is the EVT distribution
 *
 */
template <class T>
class EVTDistribution {

private:
   neither::Either<ResponseInvalidDistribution,ResponsePWCET<T> > 
   getPWCETInternal(T probability, const std::vector<T> & target) const {
        assert(probability >= 0);
        assert(probability <= 1);
        assert(target.size() > 0);

        auto positionIterator =std::lower_bound(target.begin(),target.end(),probability, std::greater<T>());
        //Not found value. Need to fall back to last available element
        if(positionIterator==target.end())
          positionIterator--;
        auto offset = positionIterator - target.begin();
        return neither::right(ResponsePWCET<T>(rank[offset]) );

   }

public:

    /**
     * @brief The ResponseEVTDistribution class constructor
     * @param dist_type  The subtype of EVT distribution (GEV/GPD)
      */
    EVTDistribution(distribution_type_t dist_type, std::vector<T> _tailValues,
                    std::vector<T> _rank, std::vector<T> _pWCET, std::vector<T> _pWCETlow, std::vector<T> _pWCEThigh )
        : dist_type(dist_type), tailValues(_tailValues),
          rank(_rank), pWCET(_pWCET), pWCETlow(_pWCETlow), pWCEThigh(_pWCEThigh)
    {    }

    EVTDistribution() = delete;

    EVTDistribution<T>& operator=(const EVTDistribution<T>& src) {
        if(&src == this)
            return *this;
        dist_type = src.dist_type;
        tailValues = src.tailValues;
        pWCET = src.pWCET;
        pWCETlow = src.pWCETlow;
        pWCEThigh = src.pWCEThigh;
        return *this;
    }

    virtual ~EVTDistribution() = default;

    /**
     * @brief  get the execution time that exceeds with the given probability
     */
    neither::Either<ResponseInvalidDistribution,ResponsePWCET<T> > getPWCET(T probability) const {
        return getPWCETInternal(probability, pWCET);
    }

    /**
     * @brief  get the execution time that exceeds with the given probability with risky assumption
     */
    neither::Either<ResponseInvalidDistribution,ResponsePWCET<T> > getPWCETLow(T probability) const {
        return getPWCETInternal(probability, pWCETlow);
    }

     /**
     * @brief  get the execution time that exceeds with the given probability with safe assumption
     */
    neither::Either<ResponseInvalidDistribution,ResponsePWCET<T> > getPWCETHigh(T probability) const {
        return getPWCETInternal(probability, pWCEThigh);
    }

    /**
     * @brief  get the collected maximum execution from the real values
     */
    T getMaxExecutionTime() const {
        return tailValues[0];
    }

    /**
     * @brief  get the real values used for the tail estimation
     */
    const std::vector<T> getTailValues() const {
        return tailValues;
    }


protected:
    distribution_type_t dist_type;  /*!< Type of the estimated distribution */
    std::vector<T> tailValues;      /*!< real values of the tail */
    std::vector<T> rank;            /*!< execution time associated to pWCET */
    std::vector<T> pWCET;           /*!< pWCET (probability) */
    std::vector<T> pWCETlow;        /*!< lower bound of pWCET */
    std::vector<T> pWCEThigh;       /*!< upper bound of pWCET */
};



/**
 * @brief The class representing the objects sent to the timing analysis tool
 *
 */
template <typename T>
class Request {

public:

    /**
     * @brief The ResponseWCET class constructor
      */
    Request() {}

    /** @brief Begin iterator for for-range-loops */
    inline typename std::vector<T>::iterator begin() {
        return this->execution_times.begin();
    }

    /** @brief Const begin iterator for for-range-loops */
    inline typename std::vector<T>::const_iterator cbegin() const {
        return this->execution_times.cbegin();
    }

    /** @brief End iterator for for-range-loops */
    inline typename std::vector<T>::iterator end() {
        return this->execution_times.end();
    }

    /** @brief Const end iterator for for-range-loops */
    inline typename std::vector<T>::const_iterator cend() const {
        return this->execution_times.cend();
    }

    /** @brief Getter for the whole timing array. Do not try to edit the returned value. */
    inline const std::vector<T> &get_all() const {
        return this->execution_times;
    }

    /** @brief Add new values to the timing array */
    inline void add_value(const T& time) {
        this->execution_times.push_back(time);
    }
    inline int get_size() {
        return this->execution_times.size();
    }

private:

    std::vector<T> execution_times;
};

/**
 * @brief Analize a set of execution times to produce an EVT distribution.
 *
 */
template <typename T>
class DistributionAnalyzer {

public:
    /**
     * @brief Estimates the distribution from the given request
     *
     * @note changing rank_length to a to big value may produce wrong values for small probabilities
     *       (1e-15). For double, do not exceed 1 Million
     */
    neither::Either<ResponseInvalidData,std::shared_ptr< EVTDistribution<T> > > estimate_distribution(std::shared_ptr<Request<T>> req, const int rank_length = 90000) {

        //Copy in vector and sort it
        //std::vector<T> trace_sorted(req->cbegin(),req->cend());
        auto trace_sorted = req->get_all();
        std::sort(trace_sorted.begin(),trace_sorted.end(), std::greater<T>() );

        //Init Required stuff
        const int file_size = trace_sorted.size();
        const int half_size = floor(file_size/2);
        static const int minvalues = 10 ;

        std::vector<T> cv(half_size-2);
        std::vector<T> limitabove(half_size-2);
        //std::vector<T> limitbelow(half_size-2);

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
        if(nelems < minvalues)
            return neither::left( ResponseInvalidData() );

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
        const T mean = std::accumulate(trace_sorted.begin(), trace_sorted.end(), 0.0)/ trace_sorted.size();
        for( auto &v : rank )  v += mean;

        //TODO estimate the real type of tail
        return neither::right(std::make_shared <EVTDistribution<T> > (distribution_type_e::EVT_EXPONENTIAL,
                              tailValues, rank, probCCDF, probCCDFlow, probCCDFhigh));
    }

};

};    // libta

#endif // LIBTA_H_
