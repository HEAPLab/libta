/** @file libta.h
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
#include <exception>

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
    WCET_VALUE             /*!< The output is a single WCET value */
} response_type_t;

/**
 * @brief The enumeration for type of distribution
 *
 * If the output is a distribution, this datatype is used to identify the distribution class.
 */
typedef enum class distribution_type_e {
    EVT_GEV,               /**< Generalized Extreme Value distribution */
    EVT_GPD_2PARAM,        /**< Generalized Pareto distribution (2-parameters version) */
    EVT_GPD_3PARAM         /**< Generalized Pareto distribution (3-parameters version) */

} distribution_type_t;

/**
 * @brief The enumeration for the error returned by the analyzer
 *
 */
typedef enum class error_e {
    INVALID_DATA,
    INVALID_DISTRIBUTION
} error_t;

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
    Response(response_type_t resp_type) noexcept : resp_type(resp_type) {

    }

    /**
     * @brief The Response class default destructor
      */
    virtual ~Response() = default;

    /** @brief Getter for reponse type */
    inline response_type_t get_response_type() const noexcept {
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
    typedef std::tuple<double, double, double> parameters_t;

    static constexpr int P_MU    = 0;    /**<  */
    static constexpr int P_SIGMA = 1;    /**<  */
    static constexpr int P_XI    = 2;    /**<  */

    /**
     * @brief The ResponseEVTDistribution class constructor
     * @param dist_type  The subtype of EVT distribution (GEV/GPD)
      */
    ResponseEVTDistribution(distribution_type_t dist_type) noexcept
        : Response(response_type_t::PWCET_DISTRIBUTION),
          dist_type(dist_type) {
        
    }
    
    /**
     * @brief The ResponseEVTDistribution class default destructor
      */
    virtual ~ResponseEVTDistribution() = default;

    /** @brief Setter for distribution parameters */
    void set_parameters(double mu, double sigma, double xi) noexcept {
        set_parameters(std::make_tuple(mu, sigma, xi));
    }

    /** @brief Setter for distribution parameters */
    void set_parameters(parameters_t params) noexcept {
        assert(std::get<P_MU>(params)   >= 0);    // A negative mu has no sense for pWCET purposes
        assert(std::get<P_SIGMA>(params) > 0);    // Must be positive

        this->params = params;
    }

    /** @brief Getter for \mu parameter */
    inline double get_mu()    const noexcept {
        return std::get<P_MU>(params);
    }

    /** @brief Getter for \sg parameter */
    inline double get_sigma() const noexcept {
        return std::get<P_SIGMA>(params);
    }

    /** @brief Getter for \xi parameter */
    inline double get_xi()    const noexcept {
        return std::get<P_XI>(params);
    }

    /** @brief Getter for the paramters tuple */
    inline parameters_t get_parameters() const noexcept {
        return this->params;
    }

    /** @brief Getter for the distribution type */
    inline distribution_type_t get_dist_type() const noexcept {
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
    ResponseWCET() noexcept : Response(response_type_t::WCET_VALUE)
    {

    }

    /**
     * @brief The ResponseWCET class default destructor
      */
    virtual ~ResponseWCET() = default;

    /** @brief Getter for the WCET value */
    inline T get_wcet_value() const {
        return this->wcet_value;
    }

    /** @brief Setter for the WCET value */
    inline void set_wcet_value(T value) {
        this->wcet_value = value;
    }

private:
    T wcet_value;


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
    Request() noexcept {}

    virtual ~Request() = default;

    /** @brief Begin iterator for for-range-loops */
    inline typename std::vector<T>::iterator begin() noexcept {
        return this->execution_times.begin();
    }

    /** @brief Const begin iterator for for-range-loops */
    inline typename std::vector<T>::const_iterator cbegin() const noexcept {
        return this->execution_times.cbegin();
    }

    /** @brief End iterator for for-range-loops */
    inline typename std::vector<T>::iterator end() noexcept {
        return this->execution_times.end();
    }

    /** @brief Const end iterator for for-range-loops */
    inline typename std::vector<T>::const_iterator cend() const noexcept {
        return this->execution_times.cend();
    }

    /** @brief Getter for the whole timing array. Do not try to edit the returned value. */
    inline const std::vector<T> &get_all() const noexcept {
        return this->execution_times;
    }

    /** @brief Add new values to the timing array */
    inline void add_value(const T& time) {
        this->execution_times.push_back(time);
    }

private:

    std::vector<T> execution_times;

};

class TimingAnalyzerError : public std::runtime_error {

public:
	TimingAnalyzerError(const std::string& what_arg, error_t err_type) noexcept
											: std::runtime_error(what_arg), err(err_type) {}

	virtual ~TimingAnalyzerError() = default;

	inline error_t get_type() const noexcept {
		return this->err;
	}

private:
	error_t err;
};

/**
 * @brief The main class to be inherited and implemented by the Timing Analysis tool.
 *
 * @note This class is abstract and must be inherited to be instanced.
 */
template <typename T>
class TimingAnalyzer {

public:

	virtual ~TimingAnalyzer() = default;

    /**
     * @brief The main method to be implemented that will perform the analysis.
     */
    virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<T>> req) = 0;

};

};    // libta

#endif // LIBTA_H_
