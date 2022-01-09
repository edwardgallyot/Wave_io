cmake_minimum_required(VERSION 3.20)
project(Wave_io)

set(CMAKE_CXX_STANDARD 14)

add_executable(Wave_io main.cpp Wave.cpp Wave.h)
