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
    PWCET_DISTRIBUTION,    /*!< The output is a statistical distribution */
    WCET_VALUE,            /*!< The output is a single WCET value */
    PWCET
} response_type_t;

/**
 * @brief The enumeration for type of distribution
 *
 * If the output is a distribution, this datatype is used to identify the distribution class.
 */
typedef enum class distribution_type_e {
    EVT_GEV,            /**< Generalized Extreme Value distribution */
    EVT_GPD_2PARAM,        /**< Generalized Pareto distribution (2-parameters version) */
    EVT_GPD_3PARAM        /**< Generalized Pareto distribution (3-parameters version) */

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
    

private:
    const response_type_t resp_type;


};

/**
 * @brief The class returned by the timing analysis tool when the output is the EVT distribution
 *
 */
class ResponseEVTDistribution : public Response {

public:

    /**
     * @brief The type for EVT distribution parameters
     *
     * GEV distribution has three parameters \mu, \sigma and \xi. The same for the 3-parameters
     * version of GPD. In case of 2-parameters version GPD the first value (\mu) is not present and
     * ignored. 
     */
    typedef std::tuple<float, float, float> parameters_t;

    static constexpr int P_MU    = 0;    /**<  */
    static constexpr int P_SIGMA = 1;    /**<  */
    static constexpr int P_XI    = 2;    /**<  */

    /**
     * @brief The ResponseEVTDistribution class constructor
     * @param dist_type  The subtype of EVT distribution (GEV/GPD)
      */
    ResponseEVTDistribution(distribution_type_t dist_type) 
        : Response(response_type_t::PWCET_DISTRIBUTION),
          dist_type(dist_type) {
        
    }
    
    /**
     * @brief The ResponseEVTDistribution class default destructor
      */
    virtual ~ResponseEVTDistribution() = default;

    /** @brief Setter for distribution parameters */
    void set_parameters(float mu, float sigma, float xi) {
        set_parameters(std::make_tuple(mu, sigma, xi));
    }

    /** @brief Setter for distribution parameters */
    void set_parameters(parameters_t params) {
        assert(std::get<P_MU>(params)   > 0);    // A negative mu has no sense for pWCET purposes
        assert(std::get<P_SIGMA>(params) > 0);    // Cannot be negative

        this->params = params;
    }

    /** @brief Getter for \mu parameter */
    inline float get_mu()    const {
        return std::get<P_MU>(params);
    }

    /** @brief Getter for \sg parameter */
    inline float get_sigma() const {
        return std::get<P_SIGMA>(params);
    }

    /** @brief Getter for \xi parameter */
    inline float get_xi()    const {
        return std::get<P_XI>(params);
    }

    /** @brief Getter for the paramters tuple */
    inline parameters_t get_parameters() const {
        return this->params;
    }

    /** @brief Getter for the distribution type */
    inline distribution_type_t get_dist_type() const {
        return this->dist_type;
    }

private:
    const distribution_type_t dist_type;
    parameters_t params;

};

/**
 * @brief The class returned by the timing analysis tool when the output is a WCET value. The
 *        class is a template class because the WCET can be a float, an integer, etc. depending on
 *        the context.
 *
 */
template <typename T>
class ResponseWCET : public Response {

public:

    /**
     * @brief The ResponseWCET class constructor
      */
    ResponseWCET() : Response(response_type_t::WCET_VALUE)
    {

    }

    /**
     * @brief The ResponseWCET class default destructor
      */
    virtual ~ResponseWCET() = default;

    /** @brief Getter for the WCET value */
    inline T get_wcet_value() const {
        return this-> wcet_value;
    }

    /** @brief Setter for the WCET value */
    inline void set_wcet_value(T value) {
        this->wcet_value = value;
    }

private:
    T wcet_value;


};

template <typename T>
class ResponsePWCET : public Response {

public:

    /**
     * @brief The ResponseWCET class constructor
      */
    ResponsePWCET() : Response(response_type_t::PWCET)
    {

    }

    /**
     * @brief The ResponseWCET class default destructor
      */
    virtual ~ResponsePWCET() = default;


    /** @brief Getter for the WCET value */
    inline T get_wcet_value() const {
        return this-> wcet_value;
    }
    /** @brief Setter for the WCET value */
    inline void set_wcet_value(T value) {
        this->wcet_value = value;
    }


    inline T get_pwcet_value() const {
        return this-> pwcet_value;
    }
    /** @brief Setter for the WCET value */
    inline void set_pwcet_value(T value) {
        this->pwcet_value = value;
    }



    inline void add_value_rank(const T& value) {
        this->rank.push_back(value);
    }
    inline void add_value_probCCDF(const T& value) {
        this->probCCDF.push_back(value);
    }
    inline void add_value_probCCDFhigh(const T& value) {
        this->probCCDFhigh.push_back(value);
    }
    inline void add_value_probCCDFlow(const T& value) {
        this->probCCDFlow.push_back(value);
    }



    inline void set_mincv_used(const int value){
        this->mincv_used = value;
    }
    inline int get_mincv_used(){
      return this->mincv_used;
    }

    inline void set_error(const  error_code_pa value){
        this->error = value;
    }
    inline error_code_pa get_error(){
        return this->error;
    }

private:


    T wcet_value;
    T pwcet_value;
    std::vector<T> rank;
    std::vector<T> probCCDF;
    std::vector<T> probCCDFhigh;
    std::vector<T> probCCDFlow;

    int mincv_used;
    error_code_pa error;

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
class TimingAnalyzer {

public:
    /**
     * @brief The main method to be implemented that will perform the analysis.
     */
    virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<T>> req) = 0 ;

};

};    // libta

#endif // LIBTA_H_
