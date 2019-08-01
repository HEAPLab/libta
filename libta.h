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

typedef enum  error_code {
    no_error,
    no_nelems_enoght

} error_code_pa;


//
// ------------------------------------ CLASSES ------------------------------------
//


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

    /**
     * @brief An Invalid Response. It has a string message
     *
     */

protected:
    response_type_t resp_type;
};


class ResponseInvalid : public Response {

protected:
    std::string message;
public:

    ResponseInvalid(std::string msg, response_type_t invalidType ):
        Response(invalidType),
        message(msg) { }

    ResponseInvalid& operator=(const ResponseInvalid& src){
      if(&src == this)
            return *this;
      this->message = src.message;
      this->resp_type = src.resp_type;
      return *this;
    }

    std::string getMessage() const {
      return this->message;
    }
};

class ResponseInvalidDistribution : public ResponseInvalid {
    //TODO add meaningful message
public:
    ResponseInvalidDistribution():
        ResponseInvalid("Cannot use this distribution", response_type_t::INVALID_DISTRIBUTION)          {}
};


class ResponseInvalidData : public ResponseInvalid {

public:
    //TODO add meaningful message
    ResponseInvalidData():
        ResponseInvalid("Not Enough points in given data", response_type_t::INVALID_DATA)          {}
};


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

    ResponsePWCET<T> & operator=(const libta::ResponsePWCET<T>& src){
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

public:

    /**
     * @brief The ResponseEVTDistribution class constructor
     * @param dist_type  The subtype of EVT distribution (GEV/GPD)
      */
    EVTDistribution(distribution_type_t dist_type, std::vector<T> _tailValues,
                    std::vector<T> _rank, std::vector<T> _pWCET, std::vector<T> _pWCETlow, std::vector<T> _pWCEThigh )
        : dist_type(dist_type), tailValues(_tailValues),
        rank(_rank), pWCET(_pWCET), pWCETlow(_pWCETlow), pWCEThigh(_pWCEThigh) 
    {
      /*
        std::cout<<"NEW EVT!"<<std::endl;
        for(int i=0; i<pWCET.size(); i++){
          std::cout<<i<<" "<<rank[i]<<" "<<pWCET[i]<<std::endl;
        }
        std::cout<<std::endl;
      */
    }

    EVTDistribution() = delete;

    EVTDistribution<T>& operator=(const EVTDistribution<T>& src){
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
     * @brief  getTheExecutionTime that exceeds with the given probability
     */
    neither::Either<ResponseInvalidDistribution,ResponsePWCET<T> > getWCET(T probability) const {
        //TODO implement
        assert(probability >= 0);
        assert(probability <= 1);
        assert(pWCET.size() == pWCETlow.size());
        assert(pWCET.size() == pWCEThigh.size());
        assert(pWCET.size() > 0);

        int pos = pWCET.size()-1;
        for(int i=pWCET.size()-1; i>=0; i--){
          if(pWCET[i] >= probability)
            break;
          pos = i;
        }
        //ResponsePWCET<T> resp(pWCET[pos],pWCETlow[pos],pWCEThigh[pos]);
        return neither::right(ResponsePWCET<T>(rank[pos]) );
    }

    T getMaxExecutionTime() const {
        return tailValues[0];
    }

    const std::vector<T> getTailValues() const{
      return tailValues;
    }


protected:
    distribution_type_t dist_type;
    std::vector<T> tailValues;
    std::vector<T> rank, pWCET, pWCETlow, pWCEThigh;
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
 * @brief The main class to be inherited and implemented by the Timing Analysis tool.
 *
 * @note This class is abstract and must be inherited to be instanced.
 */
template <typename T>
class DistributionAnalyzer {


     /**
     * @brief The method calculates Pow operation, it uses 'powl' if T is long double, otherwise 'pow'.
     */
      inline T internalPow(T base, T exp){
        return std::is_same<T, long double>::value ? powl(base,exp) : pow(base, exp);
      }


     /**
     * @brief The method calculates Sqrt operation, it uses 'sqrtl' if T is long double, otherwise 'sqrt'.
     */
      inline T internalSqrt(T v){
        return std::is_same<T, long double>::value ? sqrtl(v) : sqrt(v);
      }

     /**
     * @brief The method calculates the mean of the vector's values. The used values of the vector are bounded
     *        by start variable and size variable.
     */
      T getUnbiasedMean(std::vector<T> v, size_t start, size_t size){
          assert(start+size <= v.size());
          assert(start+size-1 >= 0);
          T result=0;
          for(size_t i=start; i<start+size; i++){
              result+=(v[i]-v[start+size-1]);
          }
          return result/size;
      }

     /**
     * @brief The method calculates the Std (Standard deviation) of the vector's values. 
     *        The used values of the vector are bounded by start variable and size.
     */
      T getUnbiasedStdDeviation(std::vector<T> v, size_t start, size_t size){
          assert(start+size <= v.size());
          assert(start+size-1 >= 0);
          T sumOfSquares=0;
          T sumOfValues=0;
          for(size_t i=start; i<start+size; i++){
              const T delta=v[i] - v[start+size-1];
              sumOfValues+= delta;
              sumOfSquares+=(delta*delta);
          }
          return internalSqrt( sumOfSquares/(size-1)-internalPow(sumOfValues,2)/((size-1)*size) );
      }

     /**
     * @brief The method fill the vec vector in this way. The first position will be equal to start value, 
     *        the last position will be equal to end. One of the middle values will be equal the previous
     *        value plus step variable.
     */
      void arange( std::vector<T> & vec , const T start ,  const T end ,  const T step ){
            vec[0]=start;
            for(size_t i = 1; i<vec.size() ; i++){
                vec[i] = vec[i-1]+step ;
            }
      }


     /**
     * @brief The method transform the vector src values to Exponential Probability Density Function and
     *        it saves to 'dest vector'. For the calculation it uses 'rate' value.
     *        The function return sum value, which is equal to the sum of all values of dest vector.
     */
      T setExponProbabilyDensityFunction(std::vector<T> & dest , const std::vector<T> & src ,T rate){
        T sum=0;
        for(size_t i=0;i<dest.size();i++){
            dest[i] = rate * internalPow(M_E,-rate * src[i]) ;
            sum += dest[i];
        }
        return sum;
      }

     /**
     * @brief The method transform the vector src values to Exponential Survival Function and it
     *        saves to 'dest vector'. For the calculation it uses 'rate' value.      
     *        
     */
      void setExponSurvivalFunction(std::vector<T> & dest , const std::vector<T> & src , T rate){
        const T sum= setExponProbabilyDensityFunction(dest , src ,  rate);
        const int size = dest.size();
        { //Scope to limit tmp
          std::vector<T> tmp = dest;
          dest[0]=0;
          for(int i=1;i<size;i++){
              dest[i] =  dest[i-1] + tmp[i-1]/sum;
              //We don't need dest[i-1] anymore. If dest[i-1] is less than 0, then set to zero
              dest[i-1] = 1.0 - dest[i-1];
              if(dest[i-1]< 0){
                  dest[i-1]=0;
              }
          }
        }

        //Correct the value for the last element
        dest[size-1] = 1.0 - dest[size-1];
        if(dest[size-1]< 0){
            dest[size-1]=0;
        }
      }


public:
    /**
     * @brief The main method to be implemented that will perform the analysis.
     */
    //std::shared_ptr< neither::Either<ResponseInvalidData,EVTDistribution<T> > > perform_analysis(std::shared_ptr<Request<T>> req) {
    neither::Either<ResponseInvalidData,std::shared_ptr< EVTDistribution<T> > > perform_analysis(std::shared_ptr<Request<T>> req) {

        const auto &exec_times = req->get_all();

        //Init Required stuff
        const int file_size = exec_times.size();
        const int half_size =floor(file_size/2);
        const int minvalues = 10 ;
        const int rank_length = 1000000;

        //Copy vector and sort it
        auto trace_sorted = exec_times;
        std::sort(trace_sorted.begin(),trace_sorted.end(), std::greater<T>() );
        std::vector<T> cv(half_size-2); 
        std::vector<T> limitabove(half_size-2); 
        std::vector<T> limitbelow(half_size-2);

        //Compute the CV = Coefficient of Variation. It is the variance/mean
        //Need at lest 2 samples to compute the std_deviation
        for(int rejectedSamples = 2; rejectedSamples < half_size; rejectedSamples++){
          int usedSamples=half_size - rejectedSamples;
          T mean = getUnbiasedMean(trace_sorted,0,usedSamples);
          T std_deviation = getUnbiasedStdDeviation(trace_sorted,0,usedSamples);
          cv[rejectedSamples-2]= std_deviation/mean;
        }

        //Compute the region of acceptance for the exponential tail.
        //This is the red cone in a CV-plot
        assert(limitbelow.size() == limitabove.size());
        for(size_t i=0; i<limitbelow.size(); i++){
          T tmp = (1.96/internalSqrt(half_size-i));
          limitabove[i]=1 + tmp;
          limitbelow[i]=1 - tmp;
        }

        assert(limitabove.size() == cv.size());

        int nelems =  0;
        int bestCVPos=cv.size()-1;
        //Find the values below the limitabove starting from the end
        //Also look for the bestcv
        for(size_t i=cv.size()-1; i>=0; i--){
          T upperLimit = 1 + (1.96/internalSqrt(half_size-i));
          if(cv[i]>=upperLimit)
            break;
          nelems++;
          if( abs(cv[bestCVPos] - 1.0) > abs(cv[i] - 1.0) )
            bestCVPos=i;
        }

        //We need at least minvalues samples to estimate the tail.
        //If this property is not satisfied, we're force to fail.
        if(nelems < minvalues)
           return neither::left( ResponseInvalidData() );
           //return std::make_shared< neither::Either<ResponseInvalidData,EVTDistribution<T> > >(ResponseInvalidData() );

        T excessesMean = getUnbiasedMean(trace_sorted,0,nelems);
        std::vector<T> tailValues(trace_sorted.begin(), trace_sorted.begin()+nelems);
        T rate = 1/excessesMean;
        T ratelow = rate * (1 + (1.96/internalSqrt(nelems)));
        T ratehigh = rate *(1 - (1.96/internalSqrt(nelems)));

        //Biggest value in nelems is the MET
        const T rank_end =20 * trace_sorted[0] ;
        const T rank_start = 0 ;
        const T rank_step = (rank_end - rank_start)/ (rank_length-1);
        //Generate values for rank
        std::vector<T> rank(rank_length,0);
        std::vector<T> probCCDF(rank_length,0);
        std::vector<T> probCCDFlow(rank_length,0);
        std::vector<T> probCCDFhigh(rank_length,0);
        arange(rank,rank_start,rank_end,rank_step);
        setExponSurvivalFunction(probCCDF,rank, rate);
        setExponSurvivalFunction(probCCDFlow,rank, ratelow);
        setExponSurvivalFunction(probCCDFhigh,rank, ratehigh);
        //Add mean to all the rank values.
        const T mean = std::accumulate(trace_sorted.begin(), trace_sorted.end(), 0.0)/ trace_sorted.size();
        for( auto &v : rank )  v += mean;

        //TODO!!! already create rank starting from the mean and keep it into 
        // account while generating the survival function.

        //TODO estimate the real type of tail
        //auto neither::EitherPtr = new neither::Either<ResponseInvalidData,EVTDistribution<T> >  (dist);
        return neither::right(std::make_shared <EVTDistribution<T> > (distribution_type_e::EVT_EXPONENTIAL,
            tailValues, rank, probCCDF, probCCDFlow, probCCDFhigh));
        //return std::shared_ptr< neither::Either<ResponseInvalidData,EVTDistribution<T> > >( neither::EitherPtr);
    }

};

};    // libta

#endif // LIBTA_H_
