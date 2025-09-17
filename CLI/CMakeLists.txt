cmake_minimum_required(VERSION 3.30)
project(oop1Domaci)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(oop1Domaci
        CLIParser.cpp
        Interpreter.cpp
        main.cpp
        Commands/Echo.cpp
        Commands/Touch.cpp
        StreamHandler.cpp
        Commands/Prompt.cpp
        Commands/Time.cpp
        Commands/Date.cpp
        Commands/Rm.cpp
        Commands/Truncate.cpp
        Commands/Wc.cpp
        Commands/Tr.cpp
        Commands/Head.cpp
        Commands/Batch.cpp)
target_include_directories(oop1Domaci PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/CLI
        ${CMAKE_SOURCE_DIR}/CLI/Commands
)