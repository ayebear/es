cmake_minimum_required (VERSION 2.8.11)
project (ES)

set (ES_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/src/es/componentpool.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/src/es/entityprototypeloader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/es/world.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/es/entity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/es/systemcontainer.cpp
)

set (ES_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cpp)

add_definitions ("-Wall -std=c++14 -O3")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
#add_library (es SHARED ${ES_SOURCE})
add_library (es_s STATIC ${ES_SOURCE})
add_executable (es_tests ${ES_TESTS})
target_link_libraries (es_tests LINK_PUBLIC es_s)
