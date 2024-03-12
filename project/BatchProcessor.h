#ifndef ENHANCER_BATCHPROCESSOR_H
#define ENHANCER_BATCHPROCESSOR_H

#include "CommandLineInterface.h"


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
