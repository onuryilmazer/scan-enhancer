#include <iostream>
#include <filesystem>
#include <omp.h>
#include <fstream>

#include "BatchProcessor.h"
#include "CommandLineInterface.h"
#include "termcolor.hpp"
#include "EnhancerImage.h"

BatchProcessor::BatchProcessor(CommandLineInterface& cli) : cli(cli) {
    //run the benchmarks or start processing the files from the folder, depending on the mode the user choose.
    if (cli.benchmarkMode()) {
        benchmark_nrOfThreads();
    }
    else {
        processFolder(BatchProcessor::OperationType::AdaptiveThresholding);
    }
};

void BatchProcessor::processFolder(BatchProcessor::OperationType type) {
    //create the output directory, if it doesn't already exist
    try {
        std::filesystem::path outputDir(cli.getInputPath());
        outputDir = outputDir / cli.getOutputDirectory();
        if (std::filesystem::create_directory(outputDir)) {
            std::cout << termcolor::green << "Created the output directory: " << termcolor::reset << outputDir << std::endl;
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
    }


    //OpenMP can't automatically iterate over a directory_iterator, so we collect the paths in a vector first:
    std::vector<std::filesystem::path> files;
    for(const auto& entry : std::filesystem::directory_iterator(cli.getInputPath())) {
        //get the file extension
        std::string fileExtension = entry.path().extension().string();

        //add the file to the vector if it has a supported extension
        if (EnhancerImage::extensionIsSupported(fileExtension)) files.push_back(entry.path());
    }

    double startingTime = omp_get_wtime();
    int processed = 0;

    //iterate through the files in the given input directory
    //we use the dynamic schedule, because the workload is not balanced; images can have greatly different sizes / resolutions
#pragma omp parallel for schedule(dynamic, 1) num_threads(cli.getNumberOfThreads_adaptiveThresholding())
    for (int i = 0; i < files.size(); i++) {
        std::filesystem::path entry = files[i];

        //Load the image
        EnhancerImage image(entry.string());

        std::string newFilename;

        switch (type) {
            case OperationType::AdaptiveThresholding:
                //Apply the adaptive thresholding method to make the image more readable
                image.applyAdaptiveThresholding(cli.getNumberOfThreads_grayscaleConversion(), cli.getWindowWidth(), cli.getThresholdPercentage());
                newFilename = entry.stem().string()+"_binarized.jpg";
                break;

            case OperationType::GrayscaleConversion:
                image.convertToGrayscale(cli.getNumberOfThreads_grayscaleConversion());
                newFilename = entry.stem().string()+"_grayscale.jpg";
                break;
        }

        //Save the processed image back to the disk
        std::filesystem::path newPath(cli.getInputPath());
        newPath = newPath / cli.getOutputDirectory() / newFilename;  //the "/" operator of the filesystem library uses the correct separator acc. to the OS ("/" on linux "\" on windows)
        int result = image.saveImage(newPath.string(), EnhancerImage::jpg);

        //debugging information (the function only prints if the user hasn't set the --verbose flag to false)
        //we print in a critical section, to make sure that the output is correctly displayed.
        //printing takes very little time compared to processing so the critical section should not slow down the overall program too much.
#pragma omp critical
        {
            cli.printDebugInformation(std::to_string(++processed) + " / " + std::to_string(files.size()) + " ", CommandLineInterface::MessageType::Information);
            if (result) cli.printDebugInformation(newPath.filename().string() + " has been saved successfully.\n", CommandLineInterface::MessageType::Success);
            else cli.printDebugInformation(newPath.filename().string() + " could not be saved.\n", CommandLineInterface::MessageType::Error);
        }
    }

    double runtime = omp_get_wtime() - startingTime;

    std::cout << "Finished processing in " << runtime << " seconds" << std::endl;
}


void BatchProcessor::benchmark_nrOfThreads() {
    std::ofstream csvFile_grayscale{"threads_benchmark_grayscaleconversion.csv"};
    std::ofstream csvFile_adaptive{"threads_benchmark_adaptivethresholding.csv"};
    std::ofstream csvFile_grayscaleandadaptive{"threads_benchmark_allparallelized.csv"};

    csvFile_grayscale << "number_of_threads, runtime_in_seconds\n";
    csvFile_adaptive << "number_of_threads, runtime_in_seconds\n";
    csvFile_grayscaleandadaptive << "number_of_threads, runtime_in_seconds\n";

    int nrOfThreads_max = omp_get_num_procs() * 2;     //we use up to 2 times the amount of the logical cores, to show the performance effects.
    std::string originalOutputDirectory = cli.getOutputDirectory();

    //Surpress output (we will be running the benchmarks many times)
    cli.setVerbose(false);

    std::cout << termcolor::green << "Starting benchmark 1: Parallelized grayscale conversion only" << termcolor::reset << std::endl;
    cli.setOutputDirectory(originalOutputDirectory + "_parallelGrayscale_benchmark");
    cli.setNumberOfThreads_adaptiveThresholding(1);  //we want the files to be processed one-by-one in this benchmark
    for(int i = 1; i < nrOfThreads_max; i++) {
        cli.setNumberOfThreads_grayscaleConversion(i);
        double startingTime_grayscale = omp_get_wtime();
        processFolder(OperationType::GrayscaleConversion);
        double runtime_grayscale = omp_get_wtime() - startingTime_grayscale;

        csvFile_grayscale << i << ", " << runtime_grayscale << "\n";
    }

    std::cout << termcolor::green << "Starting benchmark 2: Parallelized adaptive thresholding and single-core grayscale conversion" << termcolor::reset << std::endl;
    cli.setOutputDirectory(originalOutputDirectory + "_parallelAdaptiveSequentialGrayscale_benchmark");
    cli.setNumberOfThreads_grayscaleConversion(1);
    for(int i = 1; i < nrOfThreads_max; i++) {
        cli.setNumberOfThreads_adaptiveThresholding(i);

        double startingTime = omp_get_wtime();
        processFolder(OperationType::AdaptiveThresholding);
        double runtime = omp_get_wtime() - startingTime;

        csvFile_adaptive << i << ", " << runtime << "\n";
    }

    std::cout << termcolor::green << "Starting benchmark 3: Parallelized adaptive thresholding and parallelized grayscale conversion" << termcolor::reset << std::endl;
    cli.setOutputDirectory(originalOutputDirectory + "_parallelAdaptiveParallelGrayscale_benchmark");
    for(int i = 1; i < nrOfThreads_max; i++) {
        cli.setNumberOfThreads_adaptiveThresholding(i);
        cli.setNumberOfThreads_grayscaleConversion(i);

        double startingTime = omp_get_wtime();
        processFolder(OperationType::AdaptiveThresholding);
        double runtime = omp_get_wtime() - startingTime;

        csvFile_grayscaleandadaptive << i << ", " << runtime << "\n";
    }

    cli.setOutputDirectory(originalOutputDirectory);

    std::cout << termcolor::green << "Benchmarks are completed" << termcolor::reset << std::endl;
}