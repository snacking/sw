cmake_minimum_required(VERSION 3.10)

project(threadpool)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories(${PROJECT_SOURCE_DIR}/include)  
  
set(SRC_FILES  
    ${PROJECT_SOURCE_DIR}/src/sw_epoll_core.cc  
    ${PROJECT_SOURCE_DIR}/src/sw_queue_core.cc
    ${PROJECT_SOURCE_DIR}/src/sw_thread.cc
    ${PROJECT_SOURCE_DIR}/tests/test.cpp  
)  
  
add_executable(benchmark ${SRC_FILES}) 
