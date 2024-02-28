cmake_minimum_required(VERSION 3.9)
project(stb LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)

add_library(stb INTERFACE stb_image.h stb_image_write.h)
target_include_directories(stb INTERFACE ./)