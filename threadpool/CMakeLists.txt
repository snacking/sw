cmake_minimum_required(VERSION 3.10)

project(threadpool)

include_directories(${PROJECT_SOURCE_DIR}/include)  
  
set(SRC_FILES  
    ${PROJECT_SOURCE_DIR}/src/sw_epoll_core.cc  
    ${PROJECT_SOURCE_DIR}/src/sw_queue_core.cc  
    ${PROJECT_SOURCE_DIR}/src/test.cpp  
)  
  
add_executable(MyExecutable ${SRC_FILES}) 
