add_subdirectory(common)
add_subdirectory(sensors)

add_executable(firmware
    main.c
    hello_there.c
    hello_there.h
    common
    sensors
)

pico_enable_stdio_usb(firmware 1)
pico_enable_stdio_uart(firmware 0)

pico_add_extra_outputs(firmware)

target_link_libraries(
    firmware
    PUBLIC
        hardware_i2c
        pico_stdlib
        freertos
    PRIVATE
        sensors
        common
)
