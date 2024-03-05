#include <iostream>
#include <filesystem>
#include <algorithm>

#include "userInterface.h"
#include "fileIO.h"

int main(int argc, char **argv) {
    CommandLineInterface interface(argc, argv);

    interface.parseArguments();

    //debug
    std::cout << interface.getInputPath() << ", " << interface.getOutputDirectory() << ", " << interface.getWindowWidth() << ", " << interface.getThresholdPercentage() << std::endl;

    //iterate through the files in the given input directory
    for (const auto& entry : std::filesystem::directory_iterator(interface.getInputPath())) {
        //get file extension
        std::string fileExtension = entry.path().extension().string();
        //convert the extension to lowercase before comparing it with our known extensions
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), [](unsigned char c){ return std::tolower(c); });

        // Check the extension to see if the file is an image
        if (fileExtension != ".jpg" && fileExtension != ".png" && fileExtension != ".bmp") continue;

        std::cout << "Attempting to process: " << entry.path().filename() << std::endl;

        //Load the image
        EnhancerImage image(entry.path().string());

        //Apply the adaptive thresholding method to make the more readable
        image.applyAdaptiveThresholding(interface.getWindowWidth(), interface.getThresholdPercentage());

        //Save the processed image back to the disk
        std::string newFilename = entry.path().stem().string()+"_binarized.jpg";
        std::filesystem::path newPath(interface.getInputPath());
        newPath = newPath / interface.getOutputDirectory();  //the "/" operator of the filesystem library uses the correct separator acc. to the OS ("/" on linux "\" on windows)


        //create the output directory, if it doesn't already exist
        try {
            if (std::filesystem::create_directory(newPath)) {
                std::cout << "Output directory created: " << newPath << std::endl;
            }
        } catch (std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }

        std::filesystem::path fullPath = newPath / newFilename;

        int result = image.saveImage(fullPath.string(), EnhancerImage::jpg);

        //debug
        std::cout << "Done processing: " << fullPath.string() << ": saving " << (result ? "successful" : "failed") << "\n";

    }

    return 0;
}

