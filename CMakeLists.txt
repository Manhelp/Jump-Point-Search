# cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/work/Jump-Point-Search/bin ..
# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/work/Jump-Point-Search/bin ..

# CMake
cmake_minimum_required(VERSION 3.0)

Project(jpsfinder)

# 运行目录

set(SOLUTION_DIR ${PROJECT_SOURCE_DIR}/src)

set(THIRD_LIB_DIR "-ldl -lrt -lpthread")

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99 -fPIC -g -O2 -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++2a -g -O2 -Wall -fcoroutines")
else()
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99 -O0 -Wall -g -ggdb -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -O0 -Wall -g -ggdb -fPIC -fcoroutines")
endif()


MESSAGE(STATUS " CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})

MESSAGE(STATUS " 运行目录 = " ${CMAKE_INSTALL_PREFIX})
MESSAGE(STATUS " 工程目录 = " ${SOLUTION_DIR})

include_directories(
    ${SOLUTION_DIR}
    ${SOLUTION_DIR}/coroutine
    ${SOLUTION_DIR}/jps
    ${SOLUTION_DIR}/lua
)

add_subdirectory(${SOLUTION_DIR}/coroutine)
add_subdirectory(${SOLUTION_DIR}/jps)
add_subdirectory(${SOLUTION_DIR}/lua)

aux_source_directory(src SRC_DIR)

add_executable(jpsfinder ${SRC_DIR})
target_link_libraries(jpsfinder coroutine jps lua ${THIRD_LIB_DIR})


install(TARGETS jpsfinder DESTINATION ${CMAKE_INSTALL_PREFIX})