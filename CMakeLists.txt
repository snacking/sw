cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Using CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_SW_DEBUG_)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(sw)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(LIBRARY_NAME "sw")

include_directories(${INCLUDE_DIR})

file(GLOB SW_SRCS "${SRC_DIR}/*.cc")

add_library(${LIBRARY_NAME}_static STATIC ${SW_SRCS})
add_library(${LIBRARY_NAME}_shared SHARED ${SW_SRCS})

set_target_properties(${LIBRARY_NAME}_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    OUTPUT_NAME ${LIBRARY_NAME}
)

set_target_properties(${LIBRARY_NAME}_shared PROPERTIES
    VERSION 1.0
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
    OUTPUT_NAME ${LIBRARY_NAME}
    INSTALL_RPATH ${LIB_DIR}
)

target_include_directories(${LIBRARY_NAME}_static PUBLIC ${INCLUDE_DIR})
target_include_directories(${LIBRARY_NAME}_shared PUBLIC ${INCLUDE_DIR})

add_subdirectory(test)
