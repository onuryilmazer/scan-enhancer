#include <iostream>
#include <filesystem>
#include <algorithm>
#include <list>

#include "CommandLineInterface.h"
#include "EnhancerImage.h"
#include "BatchProcessor.h"
#include "termcolor.hpp"


void processFolder(CommandLineInterface& cli);

int main(int argc, char **argv) {
    //Process the command line arguments, or prompt user for new ones if interactive mode is selected:
    CommandLineInterface cli(argc, argv);

    //Start processing all the scans from the given folder
    BatchProcessor processor(cli);

    return 0;
}