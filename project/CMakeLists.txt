cmake_minimum_required(VERSION 3.9)
project(ENHANCER LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libraries/stb_read_write)
add_subdirectory(libraries/catch2_testing)
find_package(OpenMP REQUIRED)

#main executable
add_executable(enhancer main.cpp createStbImplementations.cpp tempfile.cpp)
target_link_libraries(enhancer PRIVATE OpenMP::OpenMP_CXX PRIVATE stb)


#executable with the unit tests:
add_executable(enhancer_tests tests/catch_main.cpp tests/tempfile_tests.cpp createStbImplementations.cpp tempfile.cpp)
target_link_libraries(enhancer_tests PUBLIC OpenMP::OpenMP_CXX PUBLIC stb PUBLIC catch2)