cmake_minimum_required(VERSION 2.8)
project(exp_4)

set(CMAKE_CXX_STANDARD 11)
set(TARGET_1 doit)
set(TARGET_2 QShell)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

include_directories(include)
add_executable(${TARGET_1} src/doit.cpp)
add_executable(${TARGET_2} src/QShell.cpp)

