#include <iostream>

#include "CommandLineInterface.h"
#include "BatchProcessor.h"


int main(int argc, char **argv) {
    //Process the command line arguments, or prompt user for new ones if interactive mode is selected:
    CommandLineInterface cli(argc, argv);

    //Start processing all the scanned images that are in the folder given by the user:
    BatchProcessor processor(cli);

    return 0;
}