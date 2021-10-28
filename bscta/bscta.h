/** @file bscta.h
 * The main header file for the BSC implementation of libta.
 *
 * This header contains the main classes to be used for interfacing the resource manager with the
 * underlying software that computes the WCET timing analysis.
 */

#ifndef BSCTA_H_
#define BSCTA_H_

#include "libta.h"

namespace libta {

template <typename T>
class BSCTimingAnalyzer : public TimingAnalyzer<T> {

public:

	BSCTimingAnalyzer(int rank_length = 90000) noexcept : rank_length(rank_length) {
	}

	virtual ~BSCTimingAnalyzer() = default;

	virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<T>> req) override;

	virtual std::shared_ptr<Response> get_high_gpd() const noexcept {
		return this->high_gpd;
	}

	virtual std::shared_ptr<Response> get_low_gpd() const noexcept {
		return this->low_gpd;
	}

	virtual T get_high_wcet_at_p(double x) const;
	virtual T get_low_wcet_at_p(double x) const;
	
private:
	const int rank_length;

 	std::shared_ptr<ResponseEVTDistribution> high_gpd;
	std::shared_ptr<ResponseEVTDistribution> low_gpd;

	T get_wcet_at_p(double p, double mu, double sigma, double xi) const;

};


}	// namespace libta

#endif
