cmake_minimum_required(VERSION 3.22)

file(GLOB SERVICES_GENERATED_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Generated/src/*.c")

add_library(services_generated STATIC ${SERVICES_GENERATED_SOURCES})
target_include_directories(services_generated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Generated/include)

target_link_libraries(services_generated PRIVATE mcal_generated)

add_library(services STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Det/src/Det.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Dem/src/Dem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/System/EcuM/src/EcuM.c
    ${CMAKE_CURRENT_SOURCE_DIR}/System/EcuM/src/EcuM_Callout.c
)

target_include_directories(services PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Det/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Dem/include
    ${CMAKE_CURRENT_SOURCE_DIR}/System/EcuM/include
)

target_link_libraries(services PRIVATE common mcal PUBLIC services_generated)