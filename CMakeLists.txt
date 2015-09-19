cmake_minimum_required(VERSION 2.8.11)
project(ES)

aux_source_directory(src/es ES_SOURCE)
aux_source_directory(tests ES_TESTS)

add_subdirectory(lib/ConfigFile)
include_directories(include lib/ConfigFile)

add_definitions("-Wpedantic -Wall -std=c++14 -O3")
add_library(es SHARED ${ES_SOURCE})
add_library(es_s STATIC ${ES_SOURCE})
add_executable(es_tests ${ES_TESTS})
target_link_libraries(es_tests LINK_PUBLIC es_s cfgfile_s)
configure_file(tests/entities.cfg entities.cfg COPYONLY)
