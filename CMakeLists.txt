cmake_minimum_required(VERSION 3.3)
project(Debugger)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "home/savelyev/ClionProjects/Debugger")
add_executable(Debugger ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(Debugger dyninstAPI.so)