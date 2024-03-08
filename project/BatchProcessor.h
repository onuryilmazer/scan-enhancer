#ifndef ENHANCER_BATCHPROCESSOR_H
#define ENHANCER_BATCHPROCESSOR_H

#include "CommandLineInterface.h"


class BatchProcessor {
public:
    BatchProcessor(CommandLineInterface& cli);


    void benchmark_nrOfThreads();
    void benchmark_nrOfPixels();
private:
    void processFolder();
    CommandLineInterface& cli;
};


#endif //ENHANCER_BATCHPROCESSOR_H
