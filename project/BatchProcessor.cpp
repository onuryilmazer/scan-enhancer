#include <iostream>
#include <filesystem>
#include <omp.h>
#include <fstream>

#include "BatchProcessor.h"
#include "CommandLineInterface.h"
#include "termcolor.hpp"
#include "EnhancerImage.h"

BatchProcessor::BatchProcessor(CommandLineInterface& cli) : cli(cli) {
    if (cli.benchmarkMode()) {
        benchmark_nrOfThreads();
    }
    else {
        processFolder();
    }
};

void BatchProcessor::processFolder() {
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
    omp_set_num_threads(cli.getNumberOfThreads());
    int processed = 0;

    //iterate through the files in the given input directory
#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < files.size(); i++) {
        std::filesystem::path entry = files[i];

        //Load the image
        EnhancerImage image(entry.string());

        //Apply the adaptive thresholding method to make the more readable
        image.applyAdaptiveThresholding(cli.getNumberOfThreads(), cli.getWindowWidth(), cli.getThresholdPercentage());

        //Save the processed image back to the disk
        std::string newFilename = entry.stem().string()+"_binarized.jpg";
        std::filesystem::path newPath(cli.getInputPath());
        newPath = newPath / cli.getOutputDirectory() / newFilename;  //the "/" operator of the filesystem library uses the correct separator acc. to the OS ("/" on linux "\" on windows)

        int result = image.saveImage(newPath.string(), EnhancerImage::jpg);

        //debug
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
    std::ofstream csvFile{"threads_benchmark.csv"};
    std::ofstream csvFile_grayscale{"threads_benchmark_grayscale.csv"};
    csvFile << "number_of_threads, runtime_in_seconds\n";
    csvFile_grayscale << "number_of_threads, runtime_in_seconds\n";


    int nrOfThreads_original = cli.getNumberOfThreads();
    int nrOfThreads_max = omp_get_num_procs() * 2;

    cli.setVerbose(false);

    std::cout << "Starting benchmark" << std::endl;

    for(int i = 1; i < nrOfThreads_max; i++) {
        cli.setNumberOfThreads(i);

        double startingTime = omp_get_wtime();
        processFolder();
        double runtime = omp_get_wtime() - startingTime;

        csvFile << i << ", " << runtime << "\n";

        EnhancerImage BenchmarkImage("project/benchmarks/grayscale_BM");

        double startingTime_grayscale = omp_get_wtime();
        BenchmarkImage.convertToGrayscale(i);
        double runtime_grayscale = omp_get_wtime() - startingTime_grayscale;

        csvFile_grayscale << i << ", " << runtime_grayscale << "\n";

    }

    std::cout << "Benchmark completed" << std::endl;

}