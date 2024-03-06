cmake_minimum_required(VERSION 3.12)
project(ENHANCER LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libraries/stb_read_write)
add_subdirectory(libraries/catch2_testing)
add_subdirectory(libraries/termcolor)
find_package(OpenMP REQUIRED)

#main executable
add_executable(enhancer main.cpp CreateStbImplementations.cpp EnhancerImage.cpp CommandLineInterface.cpp BatchProcessor.cpp)
target_link_libraries(enhancer PRIVATE OpenMP::OpenMP_CXX PRIVATE stb PRIVATE termcolor)

#I know this isn't the preferred way to set flags in modern CMAKE, but the modern methods don't work with MinGW on my system, unless I add this line as well:
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

#extremely weird bug: if you have iverilog installed (e.g. for the "technische informatik" lecture) windows powershell and cmd silently fail when trying to run the executables that are compiled by MinGW
#internet says that linking the standard libraries statically, or removing iverilog from your PATH variables solves the problem.
target_link_options(enhancer PRIVATE -static-libgcc -static-libstdc++)


#executable for the unit tests:
add_executable(enhancer_tests tests/catch_main.cpp tests/fileIO_tests.cpp CreateStbImplementations.cpp EnhancerImage.cpp CommandLineInterface.cpp BatchProcessor.cpp)
target_link_libraries(enhancer_tests PRIVATE OpenMP::OpenMP_CXX PRIVATE stb PRIVATE catch2 PRIVATE termcolor)

target_link_options(enhancer_tests PRIVATE -static-libgcc -static-libstdc++)

#copy the testInputs folder into the build directory so the test cases have some sample images to work with.
add_custom_command(TARGET enhancer_tests PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:enhancer_tests>)


#allows you to run the tests using "ctest" command
enable_testing()
add_test(NAME enhancer_tests COMMAND enhancer_tests)