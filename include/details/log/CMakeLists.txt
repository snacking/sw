cmake_minimum_required(VERSION 3.10)

project(log)

include_directories(${PROJECT_SOURCE_DIR}/include)  
  
set(SRC_FILES  
    ${PROJECT_SOURCE_DIR}/src/sw_log.cc
)