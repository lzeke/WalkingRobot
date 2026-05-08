cmake_minimum_required(VERSION 3.10.0)
project(walker VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

file(GLOB SOURCES "src/*.cpp")

add_executable(swalker ${SOURCES})

include_directories(
    ./bno055_cpp/ext/serial/include
    ./src
    ./include
    )

add_library(serial
    bno055_cpp/ext/serial/src/serial.cc
    bno055_cpp/ext/serial/src/impl/unix.cc
    bno055_cpp/ext/serial/src/impl/list_ports/list_ports_linux.cc
    )

add_library(bno_lib
    bno055_cpp/src/BNO055.cpp
    bno055_cpp/src/BNO055.h
    bno055_cpp/src/BNO055DataDefinition.h
    bno055_cpp/src/BNO055CommandDefinitions.h
    )

target_include_directories(swalker PUBLIC ./include /usr/local/include/PiPCA9685)

target_link_libraries(swalker PUBLIC gpiod PiPCA9685 i2c serial bno_lib)
