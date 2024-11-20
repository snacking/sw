cmake_minimum_required(VERSION 3.10)

project(sw)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(DETAILS_DIR ${INCLUDE_DIR}/details)

include_directories(${INCLUDE_DIR})

add_subdirectory(${DETAILS_DIR}/threadpool)
add_subdirectory(${DETAILS_DIR}/json)
add_subdirectory(${DETAILS_DIR}/log)
add_subdirectory(${DETAILS_DIR}/time)

set(LIB_DIR ${CMAKE_BINARY_DIR}/ib)
file(MAKE_DIRECTORY ${LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})