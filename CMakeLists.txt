cmake_minimum_required(VERSION 4.0)
project(paramGenerator)

set(CMAKE_CXX_STANDARD 20)

add_executable(paramGenerator main.cpp
        struct/Point.h
        generator/WaveGen.cpp
        generator/WaveGen.h
        processor/WaveProcessor.cpp
        processor/WaveProcessor.h
        processor/PitchProcessor.cpp
        processor/PitchProcessor.h
        json/json.hpp
        reasoner/vel_reasoner.cpp
        reasoner/vel_reasoner.h
        struct/note_simple.h
        struct/note_normal.h
        cons.h
        cons.cpp
        reasoner/bre_reasoner.cpp
        reasoner/bre_reasoner.h
        math/soft_diff.h
)
