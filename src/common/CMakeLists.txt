add_library(
    common
    common.c
    common.h
)

target_link_libraries(common pico_stdlib hardware_i2c freertos)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
