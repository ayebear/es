cmake_minimum_required(VERSION 3.1.3)
project(ES)

aux_source_directory(src/es ES_SOURCE)
aux_source_directory(tests ES_TESTS)

add_subdirectory(lib/config-file)
include_directories(include lib/config-file)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /Ox")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -O3")
endif(MSVC)

add_library(es SHARED ${ES_SOURCE})
add_library(es_s STATIC ${ES_SOURCE})
add_executable(es_tests ${ES_TESTS})
target_link_libraries(es_tests LINK_PUBLIC es_s cfgfile_s)
configure_file(tests/entities.cfg entities.cfg COPYONLY)

set_property(TARGET es es_s es_tests PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET es es_s es_tests PROPERTY CXX_STANDARD 14)
