
set(SOURCES dummy.cpp)

include_directories(ta ../)

add_library(ta SHARED ${SOURCES})
add_library(taStatic STATIC ${SOURCES})
set_target_properties(taStatic PROPERTIES OUTPUT_NAME ta)
