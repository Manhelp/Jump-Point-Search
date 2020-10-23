# CMake 
cmake_minimum_required(VERSION 2.8)

Project(JumpPointSearch)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall -g -ggdb -DDEBUG -fPIC")
else()
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fPIC -std=c++11 -O3 -Wall")
endif()

set(SolutionDir ${PROJECT_SOURCE_DIR})
include_directories(
	${SolutionDir}
	${SolutionDir}/jps
)

aux_source_directory(. DIR_SRC)
add_subdirectory(jps)

add_executable(jspfinder ${DIR_SRC})

target_link_libraries(jspfinder libjps)
