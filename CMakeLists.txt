cmake_minimum_required(VERSION 3.13)

# Entire Project Uses the PICO SDK
include(pico_sdk_import.cmake)

# Project Name
project(UAV_FIRMWARE C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Init PICO SDK
pico_sdk_init()

# Init FreRTOS
set(PICO_SDK_FREERTOS_SOURCE lib/FreeRTOS)

add_library(freertos
    ${PICO_SDK_FREERTOS_SOURCE}/event_groups.c
    ${PICO_SDK_FREERTOS_SOURCE}/list.c
    ${PICO_SDK_FREERTOS_SOURCE}/queue.c
    ${PICO_SDK_FREERTOS_SOURCE}/stream_buffer.c
    ${PICO_SDK_FREERTOS_SOURCE}/tasks.c
    ${PICO_SDK_FREERTOS_SOURCE}/timers.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/MemMang/heap_3.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos PUBLIC
    .
    ${PICO_SDK_FREERTOS_SOURCE}/include
    ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0
)

# Include Header Files
include_directories(include)

# Inclue the Source Files
# add_subdirectory(freertos)
add_subdirectory(src)
