cmake_minimum_required(VERSION 3.13)
project(OS3)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(OS3 main.cpp Barrier.cpp )