cmake_minimum_required (VERSION 2.8.11)
project (ConfigFile)

set (CFG_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/configfile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/configoption.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/strlib.cpp
)

add_definitions ("-Wall -std=c++14 -O3")
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
add_library (cfgfile_s STATIC ${CFG_SOURCE})
