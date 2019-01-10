cmake_minimum_required(VERSION 3.13)
project(ECVRPTW)

set(CMAKE_CXX_STANDARD 14)

add_executable(ECVRPTW main.cpp Instance.cpp Instance.h Node.cpp Node.h)