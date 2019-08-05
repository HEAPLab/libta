#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "libta/libta.h"

TEST_CASE( "Request tests", "[Request]" ) {
    libta::Request<int> req;
    req.add_value(1);
    REQUIRE( *req.begin() == 1 );
}
