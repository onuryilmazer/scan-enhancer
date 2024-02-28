cmake_minimum_required(VERSION 3.9)
project(ENHANCER LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libraries/stb_read_write)
add_subdirectory(libraries/catch2_testing)
find_package(OpenMP REQUIRED)

#main executable
add_executable(enhancer main.cpp createStbImplementations.cpp tempfile.cpp fileIO.cpp)
target_link_libraries(enhancer PRIVATE OpenMP::OpenMP_CXX PRIVATE stb)


#executable for the unit tests:
add_executable(enhancer_tests tests/catch_main.cpp tests/tempfile_tests.cpp tests/fileIO_tests.cpp createStbImplementations.cpp tempfile.cpp fileIO.cpp)
target_link_libraries(enhancer_tests PRIVATE OpenMP::OpenMP_CXX PRIVATE stb PRIVATE catch2)


#copy the testInputs folder into the build directory so the test cases have data to work with.
add_custom_command(TARGET enhancer_tests PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:enhancer_tests>)