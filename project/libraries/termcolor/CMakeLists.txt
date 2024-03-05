cmake_minimum_required(VERSION 3.9)
project(termcolor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(termcolor INTERFACE termcolor.hpp)
target_include_directories(termcolor INTERFACE ./)