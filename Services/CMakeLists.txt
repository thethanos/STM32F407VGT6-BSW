cmake_minimum_required(VERSION 3.22)

add_library(services STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Det/src/Det.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Dem/src/Dem.c
)

target_include_directories(services PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Det/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Dem/include
)

target_link_libraries(services PRIVATE common)