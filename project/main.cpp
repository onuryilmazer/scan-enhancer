#include <iostream>

#include "userInterface.h"

int main(int argc, char **argv) {
    CommandLineInterface interface(argc, argv);

    interface.parseArguments();

    std::cout << interface.getInputPath() << ", " << interface.getOutputDirectory() << ", " << interface.getWindowWidth() << ", " << interface.getThresholdPercentage() << std::endl;

    return 0;
}

