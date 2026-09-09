cmake_minimum_required(VERSION 4.0)
project(W65C02S_emulator)

set(CMAKE_CXX_STANDARD 26)

include_directories(.)

add_executable(W65C02S_emulator
        CPU.cpp
        CPU.h
        Instructions.h
        main.cpp
        Memory.cpp
        Memory.h)
