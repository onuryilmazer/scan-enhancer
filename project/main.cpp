#include <iostream>

#include "userinterface.h"

int main(int argc, char **argv) {
    std::cout << "Hello, World!" << std::endl;

    CommandLineInterface interface(argc, argv);

    interface.parseArguments();

    std::cout << interface.getInputPath() << ", " << interface.getOutputDirectory() << ", " << interface.getWindowWidth() << ", " << interface.getThresholdPercentage() << std::endl;

    return 0;
}

