cmake_minimum_required(VERSION 3.9)
project(ENHANCER LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libraries/stb_read_write)
add_subdirectory(libraries/catch2_testing)
find_package(OpenMP REQUIRED)

#main executable
add_executable(enhancer main.cpp createStbImplementations.cpp fileIO.cpp userInterface.cpp columnPrinter.cpp)
target_link_libraries(enhancer PRIVATE OpenMP::OpenMP_CXX PRIVATE stb)

#static std library: if we don't use this, and use mingw to compile our program, powershell silently fails when trying to run the executable (but mingw terminal seems to have no problems).
#this is a very simple program and we are already dealing with these obscure and hard-to-debug issues...
target_link_options(enhancer PRIVATE -static-libgcc -static-libstdc++)


#executable for the unit tests:
add_executable(enhancer_tests tests/catch_main.cpp tests/fileIO_tests.cpp createStbImplementations.cpp fileIO.cpp userInterface.cpp columnPrinter.cpp)
target_link_libraries(enhancer_tests PRIVATE OpenMP::OpenMP_CXX PRIVATE stb PRIVATE catch2)


#copy the testInputs folder into the build directory so the test cases have some sample images to work with.
add_custom_command(TARGET enhancer_tests PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:enhancer_tests>)