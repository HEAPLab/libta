set(SOURCES chronovise.cpp SimpleChronovise.cpp)

include_directories(ta ../)

add_library(ta SHARED ${SOURCES})
add_library(taStatic STATIC ${SOURCES})
set_target_properties(taStatic PROPERTIES OUTPUT_NAME ta)

target_link_libraries(ta PUBLIC chronovise)
target_link_libraries(taStatic PUBLIC chronovise)

#add_executable(chrono ${SOURCES})
#target_link_libraries(chrono PUBLIC chronovise)