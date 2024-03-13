#ifndef ENHANCER_BATCHPROCESSOR_H
#define ENHANCER_BATCHPROCESSOR_H

#include "CommandLineInterface.h"

/*
    This class takes a CommandLineInterface instance in its constructor, and uses the user inputs
    that are stored in this CommandLineInterface instance; it loads the pictures from the inputPath
    given by the user and processes them, then saves them inside the outputDirectory provided by the user, 
    one by one.
*/

class BatchProcessor {
public:
    BatchProcessor(CommandLineInterface& cli);


    void benchmark_nrOfThreads();

private:
    enum OperationType {GrayscaleConversion, AdaptiveThresholding};
    void processFolder(BatchProcessor::OperationType type);
    CommandLineInterface& cli;
};


#endif //ENHANCER_BATCHPROCESSOR_H
