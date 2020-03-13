#include "gtest/gtest.h"

#include "bscta/libta_math.h"

#include <iostream>
#include <random>

#define GTEST_COUT std::cerr << "[          ] "


TEST(internal_test, test_sqrt_int)
{
    EXPECT_EQ(libta::internalSqrt(4),  2);
    EXPECT_EQ(libta::internalSqrt(16), 4);
    EXPECT_EQ(libta::internalSqrt(64), 8);
}

TEST(internal_test, test_sqrt_long_double)
{
	EXPECT_LE( libta::internalSqrt((long double)3) , 1.74 );
	EXPECT_GE( libta::internalSqrt((long double)3) , 1.72 );
	EXPECT_LE( libta::internalSqrt((long double)10), 3.17 );
	EXPECT_GE( libta::internalSqrt((long double)10), 3.16 );
	EXPECT_LE( libta::internalSqrt((long double)70), 8.37 );
	EXPECT_GE( libta::internalSqrt((long double)70), 8.36 );
}

TEST(internal_test, test_getUnbiasedMean)
{
	std::vector<double> tmp;
	for(int i=0;i<6;i++)tmp.push_back((double)(i+1));
	EXPECT_EQ( libta::getUnbiasedMean(tmp, 0, 6), -2.5 );
	tmp.clear();
	for(int i=10 ; i>0 ; i-- ) tmp.push_back(i);
	EXPECT_EQ( libta::getUnbiasedMean(tmp, 0, 10), 4.5 );
}

TEST(internal_test, test_setExponProbabilyDensityFunction)
{
	std::vector<double> tmp;
	for(int i=0;i<10;i++)tmp.push_back((double)(i));
	double sum = libta::setExponProbabilyDensityFunction(tmp , tmp,(double)1);
	int left = sum > 1.5819000 ;
	int right = sum < 1.5820000 ;
	EXPECT_EQ( (left * right), 1 );
	tmp.clear();
	double var = 0.5;
	for(int i=0;i<20;i++){
		tmp.push_back(var);
		var+=1;
	}
	sum = libta::setExponProbabilyDensityFunction(tmp , tmp,(double)1);
	left = sum > 0.959500 ;
	right = sum < 0.959600 ;
	EXPECT_EQ( (left * right), 1 );
	tmp.clear();

	var = 0.5;
	for(int i=0;i<1000;i++){
		tmp.push_back(var);
		var+=1;
	}
	sum = libta::setExponProbabilyDensityFunction(tmp , tmp,(double)1.5);
	left = sum > 0.912057 ;
	right = sum < 0.912058 ;
	EXPECT_EQ( (left * right), 1 );
}

TEST(internal_test, test_setExponSurvivalFunction)
{
	std::vector<double> tmp;
	double var = 0.5;
	for(int i=0;i<10;i++){
		tmp.push_back((double)i);
		var+=1;
	}
	libta::setExponSurvivalFunction(tmp , tmp,(double)1.5);
	double sum_surv = 0 ;
	for(int i=0;i<10;i++) sum_surv+=tmp[i];
	int left  = sum_surv >= 1.2872;
	int right = sum_surv <= 1.2873;
	EXPECT_EQ( (left * right), 1 );

	tmp.clear();
	var = 1.75 ;
	for(int i=0;i<20;i++){
		tmp.push_back(var);
		var+=1.0505;
	}
	libta::setExponSurvivalFunction(tmp , tmp,(double)1.5);
    sum_surv = 0 ;
	for(int i=0;i<20;i++) sum_surv+=tmp[i];
	left = sum_surv >= 1.26070;
	right = sum_surv <= 1.26080;
	EXPECT_EQ( (left * right), 1 );
}



