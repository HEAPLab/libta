#include "gtest/gtest.h"

#include "bscta/bscta.h"

#include <iostream>
#include <random>

#define GTEST_COUT std::cerr << "[          ] "



TEST(distribution_test, test_distribution_normal)
{

	const int n_estimation=500;  // number of experiments
	const int n_testing=10000;    // maximum number of stars to distribute

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0,1.0);

    libta::BSCTimingAnalyzer<double> mta;

    std::shared_ptr<libta::Request<double>> req = std::make_shared<libta::Request<double>>();

	for (int i=0; i<n_estimation; i++) {
	    req->add_value(distribution(generator));
	}

    std::shared_ptr<libta::ResponseEVTDistribution> pwcet = std::dynamic_pointer_cast<libta::ResponseEVTDistribution>(mta.perform_analysis(req));
	//pwcet = std::dynamic_pointer_cast<libta::ResponseEVTDistribution>(mta.get_low_gpd());

	// Normal distributed samples should have a xi ~= 0
    EXPECT_GE(pwcet->get_xi(), -0.5);
    EXPECT_LE(pwcet->get_xi(), 0.5);

	std::vector<double> dist_testing;
	for (int i=0; i<n_testing; i++) {
		dist_testing.push_back(distribution(generator));
	}

	std::sort (dist_testing.begin(), dist_testing.end()); 

	double value_at_95_percent = dist_testing[n_testing / 100 * 95];

	double prob_95_quantile = 1 - std::exp(-pwcet->get_sigma() * value_at_95_percent );

    EXPECT_LE(prob_95_quantile, 0.96);
	if (prob_95_quantile < 0.94) {
		GTEST_COUT << "WARNING: 95% of quantile value is lower than 0.94: " << prob_95_quantile << std::endl;
		GTEST_COUT << "WARNING: Resulted distribution may be too pessimistic." << std::endl;
	}
}

TEST(distribution_test, test_distribution_normal_high)
{
	const int n_estimation=500;  // number of experiments

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(50,2.0);

    libta::BSCTimingAnalyzer<double> mta;

    std::shared_ptr<libta::Request<double>> req = std::make_shared<libta::Request<double>>();

	for (int i=0; i<n_estimation; i++) {
	    req->add_value(distribution(generator));
	}

    std::shared_ptr<libta::ResponseEVTDistribution> pwcet = std::dynamic_pointer_cast<libta::ResponseEVTDistribution>(mta.perform_analysis(req));
	std::shared_ptr<libta::ResponseEVTDistribution> pwcet2 = std::dynamic_pointer_cast<libta::ResponseEVTDistribution>(mta.get_high_gpd());

	EXPECT_GE(pwcet->get_sigma(), pwcet2->get_sigma());
}

TEST(distribution_test, test_distribution_normal_low)
{
	const int n_estimation=500;  // number of experiments

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(30,5.0);

    libta::BSCTimingAnalyzer<double> mta;

    std::shared_ptr<libta::Request<double>> req = std::make_shared<libta::Request<double>>();

	for (int i=0; i<n_estimation; i++) {
	    req->add_value(distribution(generator));
	}

    std::shared_ptr<libta::ResponseEVTDistribution> pwcet = std::dynamic_pointer_cast<libta::ResponseEVTDistribution>(mta.perform_analysis(req));
	std::shared_ptr<libta::ResponseEVTDistribution> pwcet2 = std::dynamic_pointer_cast<libta::ResponseEVTDistribution>(mta.get_low_gpd());

	EXPECT_LE(pwcet->get_sigma(), pwcet2->get_sigma());

}
