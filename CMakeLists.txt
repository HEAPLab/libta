cmake_minimum_required(VERSION 3.8)
project(libta)

set(CMAKE_CXX_STANDARD 14)


# Check the implementation variable
if(DEFINED IMPLEMENTATION)
	message("Building the ${IMPLEMENTATION} implementation...")
else(DEFINED IMPLEMENTATION)
	message(FATAL_ERROR "You MUST specify the selected implementation via -DIMPLEMENTATION=something")
endif(DEFINED IMPLEMENTATION)

# Check that the implementation directory actually exist
string(TOLOWER ${IMPLEMENTATION} IMP_L)
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${IMP_L})
        message(FATAL_ERROR "Directory ${CMAKE_CURRENT_SOURCE_DIR}/${IMP_L} does not exist.")
endif()


# Include that for the building
add_subdirectory(${IMP_L})
add_subdirectory(test)
add_subdirectory(chronovise/chronovise)