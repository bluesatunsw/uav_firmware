add_library(
    sensors
    imu.c
    imu.h
    gy89/lsm303d.c
    gy89/lsm303d.h
    gy89/l3gd20.c
    gy89/l3gd20.h
    gy89/bmp180.c
    gy89/bmp180.h
)

target_link_libraries(sensors pico_stdlib hardware_i2c freertos common)
target_include_directories(sensors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
