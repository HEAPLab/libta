
enable_testing()
find_package(GTest QUIET)

if((DEFINED GTEST_LIBRARY) OR (DEFINED GTEST_MAIN_LIBRARY))

	message(STATUS "Google Test framework found.")
	include_directories(${GTEST_INCLUDE_DIR} "../")

	add_executable(libta-testing
	    test-suite.cpp
		test-distributions.cpp
	)

	target_link_libraries(libta-testing ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
	target_link_libraries(libta-testing ta)

	add_test(NAME libta-testing COMMAND libta-testing)
	add_custom_target(check COMMAND libta-testing)
else()

	message(WARNING "Google Test framework NOT FOUND - Testing not available.")

endif()
