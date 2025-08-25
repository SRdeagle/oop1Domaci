cmake_minimum_required(VERSION 3.30)
project(oop1Domaci)

set(CMAKE_CXX_STANDARD 20)

add_executable(oop1Domaci
        Command.h
        Interpreter.h
        Parser.cpp
        Echo.h
        Interpreter.cpp
        Parser.h
        Structures.h
        Echo.cpp
        main.cpp
        Touch.cpp
        Time.h)
