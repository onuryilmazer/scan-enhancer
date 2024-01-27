cmake_minimum_required(VERSION 3.9)
project (catch2 LANGUAGES CXX)

add_library(catch2 INTERFACE catch.hpp)
target_include_directories(catch2 INTERFACE ./)
target_compile_features(catch2 INTERFACE cxx_std_11)