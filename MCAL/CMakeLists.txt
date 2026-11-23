cmake_minimum_required(VERSION 3.22)

file(GLOB MCAL_GENERATED_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Generated/src/*.c")

add_library(mcal_generated STATIC ${MCAL_GENERATED_SOURCES})
target_include_directories(mcal_generated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Generated/include)

target_link_libraries(mcal_generated PUBLIC common)

add_library(mcal STATIC 
    Mcu/src/Mcu.c
    Dio/src/Dio.c
    Port/src/Port.c
)

target_include_directories(mcal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Mcu/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Dio/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Port/include
)

target_link_libraries(mcal 
    PUBLIC common mcal_generated services
    PRIVATE stm32cubemx
)