cmake_minimum_required(VERSION 4.0)
project(paramGenerator)

set(CMAKE_CXX_STANDARD 20)

add_executable(paramGenerator main.cpp
        Point.h
        WaveGen.cpp
        WaveGen.h
        WaveProcessor.cpp
        WaveProcessor.h
        PitchProcessor.cpp
        PitchProcessor.h
)
