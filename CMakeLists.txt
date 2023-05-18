cmake_minimum_required(VERSION 3.13)

# Entire Project Uses the PICO SDK
include(pico_sdk_import.cmake)

# Project Name
project(UAV_FIRMWARE C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Init PICO SDK
pico_sdk_init()

# Add src files
add_subdirectory(src)
