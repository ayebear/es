cmake_minimum_required (VERSION 2.8.11)
project (ES)

set (ES_SOURCE
	src/es/componentpool.cpp
	src/es/core.cpp
	src/es/entity.cpp
	src/es/world.cpp
	src/es/systemcontainer.cpp
	src/es/entityprototypeloader.cpp
)

set (ES_TESTS tests/tests.cpp tests/tests2.cpp)

add_subdirectory (lib/ConfigFile)
add_definitions ("-Wall -std=c++14 -O3")
include_directories (include lib/ConfigFile)
#add_library (es SHARED ${ES_SOURCE})
add_library (es_s STATIC ${ES_SOURCE})
add_executable (es_tests ${ES_TESTS})
target_link_libraries (es_tests LINK_PUBLIC es_s cfgfile_s)
configure_file(tests/entities.cfg entities.cfg COPYONLY)
