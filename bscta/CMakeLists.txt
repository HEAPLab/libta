
set(SOURCES bscta.cpp)

include_directories(ta ../)

add_library(ta SHARED ${SOURCES})
add_library(taStatic STATIC ${SOURCES})
set_target_properties(taStatic PROPERTIES OUTPUT_NAME ta)


install(TARGETS ta taStatic
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

