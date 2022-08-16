cmake_minimum_required(VERSION 3.16)
project(Corsa CXX)

set(CMAKE_CXX_STANDARD 20)
set(STEAMWORKS_SDK "sdk" CACHE STRING "The path to the \"sdk\" folder from Steamworks")

add_library(Corsa SHARED src/Corsa.cpp)
target_include_directories(Corsa SYSTEM PRIVATE ${STEAMWORKS_SDK}/public)
