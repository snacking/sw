cmake_minimum_required(VERSION 3.10)

project(log)

set(LOG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LOG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

file(GLOB LOG_SRCS "${LOG_SOURCE_DIR}/*.cc")

set(LIBRARY_NAME "log")

add_library(${LIBRARY_NAME}_static STATIC ${LOG_SRCS})
add_library(${LIBRARY_NAME}_shared SHARED ${LOG_SRCS})

set_target_properties(${LIBRARY_NAME}_static PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_DIRECTORY}
    OUTPUT_NAME ${LIBRARY_NAME}
)

set_target_properties(${LIBRARY_NAME}_shared PROPERTIES
    VERSION 1.0
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIRECTORY}
    OUTPUT_NAME ${LIBRARY_NAME}
    INSTALL_RPATH ${LIBRARY_OUTPUT_DIRECTORY}
)

set(PROPERTIES_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../properties/lib")
set(PROPERTIES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../properties/include")

target_link_libraries(${LIBRARY_NAME}_static PUBLIC ${PROPERTIES_LIB_DIR}/libproperties.a)
target_link_libraries(${LIBRARY_NAME}_shared PUBLIC ${PROPERTIES_LIB_DIR}/libproperties.so)

target_include_directories(${LIBRARY_NAME}_static PUBLIC ${PROPERTIES_INCLUDE_DIR})
target_include_directories(${LIBRARY_NAME}_shared PUBLIC ${PROPERTIES_INCLUDE_DIR})

target_include_directories(${LIBRARY_NAME}_static PUBLIC ${LOG_INCLUDE_DIR})
target_include_directories(${LIBRARY_NAME}_shared PUBLIC ${LOG_INCLUDE_DIR})
