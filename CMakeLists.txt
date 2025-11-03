cmake_minimum_required(VERSION 3.15)
project(hydro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "src/*.cpp")

add_executable(hydro ${SOURCES})
