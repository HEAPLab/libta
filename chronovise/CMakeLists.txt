set(SOURCES chronovise.cpp SimpleChronovise.cpp)

include_directories(ta ../)

add_library(ta SHARED ${SOURCES})
add_library(taStatic STATIC ${SOURCES})
set_target_properties(taStatic PROPERTIES OUTPUT_NAME ta)

find_library(CHRONOVISE REQUIRED NAMES chronovise)
find_path(CHRONOVISE_INCLUDE aec.hpp PATH_SUFFIXES include/chronovise include)

message( "Chronovise library: " ${CHRONOVISE} )
message( "Chronovise include path: " ${CHRONOVISE_INCLUDE} )

target_include_directories(ta PUBLIC ${CHRONOVISE_INCLUDE})
target_include_directories(taStatic PUBLIC ${CHRONOVISE_INCLUDE})


target_link_libraries(ta PUBLIC chronovise)
target_link_libraries(taStatic PUBLIC chronovise)

install(TARGETS ta taStatic
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

