cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(sw_test)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/sw)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)

include_directories(${INCLUDE_DIR})

find_library(SW_LIB NAMES libsw.a PATHS ${LIB_DIR})

file(GLOB TEST_SRCS "${TEST_DIR}/*.cpp")

foreach(src IN LISTS TEST_SRCS)
    get_filename_component(target_name ${src} NAME_WE)
    
    add_executable(${target_name} ${src})
    
    target_link_libraries(${target_name} ${SW_LIB})
endforeach()
