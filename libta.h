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
    EVT_HEAVY_TAIL 

} distribution_type_t;

typedef enum  error_code {
    no_error ,
    no_nelems_enoght

} error_code_pa;


//
// ------------------------------------ CLASSES ------------------------------------ 
//

/**
 * @brief The class returned by the timing analysis tool.
 *
 * @note This class is not abstract but intended to be used with sub-classes only. 
 */
template <class L, class R>
class Either{

private:
    /**
    * @brief Internal Enumeration to know if the state is Left or Right
    */
    enum lr_t { LEFT, RIGHT};
    /**
    * @brief Union that stores the data
    */
    union data_u {
        L left; R right;
    data_u(R r) {right=r;}
    data_u(L l) {left=l;}

    };

    const lr_t type;
    const data_u data;

public:

    Either() = delete;
    Either(const Either<L,R> & src):data(src.data),type(src.type) {}
    Either(R _right): data(_right), type(RIGHT) {}
    Either(L _left): data(_left), type(RIGHT) {}

    bool isLeft() {return type==LEFT; }
    bool isRight() {return type==RIGHT; }

    /** @brief If Either is Right return the stored value, otherwise return defaultVal */
    R getRight( R defaultVal ) { return isRight() ? data.right : defaultVal;  }
    /** @brief If Either is Left return the stored value, otherwise return defaultVal */
    L getLeft( L defaultVal ) { return isLeft() ? data.left : defaultVal;  }

};


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

private:
    const response_type_t resp_type;


};


class ResponseInvalid : public Response {

public:
    const std::string message;

    ResponseInvalid(std::string msg, response_type_t invalidType ): 
        Response(invalidType),
        message(msg){ }
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
    ResponsePWCET(T _pWCET, T _pWCETlow, T _pWCEThigh) : Response(response_type_t::PWCET),
        pWCET(_pWCET),pWCETlow(_pWCETlow), pWCEThigh(_pWCEThigh)
    {

    }

    /**
     * @brief The ResponseWCET class default destructor
      */
    virtual ~ResponsePWCET() = default;


    inline T get_pwcet_value() const {
        return this->pWCET;
    }
    
private:
    const T pWCET, pWCETlow, pWCEThigh;

};


/**
 * @brief The class returned by the timing analysis tool when the output is the EVT distribution
 *
 */
template <typename T>
class EVTDistribution {

public:

    /**
     * @brief The ResponseEVTDistribution class constructor
     * @param dist_type  The subtype of EVT distribution (GEV/GPD)
      */
    EVTDistribution(distribution_type_t dist_type, 
            std::vector<T> rank, std::vector<T> pWCETlow, std::vector<T> pWCEThigh ) 
        : dist_type(dist_type) {
        
    }
    
    virtual ~EVTDistribution() = default;
    
    /**
     * @brief  getTheExecutionTime that exceeds with the given probability
     */
    Either<ResponseInvalidDistribution,ResponsePWCET<T> > getWCET(T probability){
        //TODO implement
        ResponsePWCET<T> resp(0,0,0);
        return Either<ResponseInvalidDistribution,ResponsePWCET<T> > (resp); 
    }


private:
    const distribution_type_t dist_type;
    const std::vector<T> rank, pWCET, pWCETlow, pWCEThigh;
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
    inline int get_size(){
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

public:
    /**
     * @brief The main method to be implemented that will perform the analysis.
     */
    virtual std::shared_ptr< Either<ResponseInvalidData,EVTDistribution<T> > > perform_analysis(std::shared_ptr<Request<T>> req) {

        //TODO implement
        return std::make_shared< Either<ResponseInvalidData,EVTDistribution<T> > >();
       }

};

};    // libta

#endif // LIBTA_H_
