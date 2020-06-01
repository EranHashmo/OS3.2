cmake_minimum_required(VERSION 3.13)
project(.)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(. SampleClient.cpp MapReduceFramework.cpp Barrier.cpp)