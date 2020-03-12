
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(runUnitTests
    test-suite.cpp
)

target_link_libraries(runUnitTests ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
