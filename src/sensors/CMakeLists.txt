add_library(
    sensors
    imu.c
    imu.h
    gy89/lsm303d.c
    gy89/lsm303d.h
)

target_link_libraries(sensors pico_stdlib hardware_i2c freertos common)
target_include_directories(sensors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
