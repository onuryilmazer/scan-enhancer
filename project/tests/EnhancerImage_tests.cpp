#include "catch.hpp"
#include "../EnhancerImage.h"
#include "stb_image.h"
#include <iostream>
#include <filesystem>
#include "../CommandLineInterface.h"
namespace fs = std::filesystem;



TEST_CASE("Try to load a known image", "[correctness]") {
    //File 1: 1562 * 1200 px, 3 channels
    EnhancerImage img("test_input/inputfile1.jpg");

    REQUIRE( (img.width == 1562 && img.height == 1200 && img.nrOfChannels == 3) );
}

TEST_CASE("Try to save an image in different formats", "[correctness]") {
    //File 1: 1562 * 1200 px, 3 channels
    EnhancerImage img("test_input/inputfile1.jpg");

    int savedW, savedH, savedDim;

    img.saveImage("test_input/inputfile1_testsave.jpg", EnhancerImage::Filetype::jpg);
    img.saveImage("test_input/inputfile1_testsave.png", EnhancerImage::Filetype::png);
    img.saveImage("test_input/inputfile1_testsave.bmp", EnhancerImage::Filetype::bmp);

    stbi_info("test_input/inputfile1_testsave.jpg", &savedW, &savedH, &savedDim);
    REQUIRE( (savedW == 1562 && savedH == 1200 && savedDim == 3) );

    stbi_info("test_input/inputfile1_testsave.png", &savedW, &savedH, &savedDim);
    REQUIRE( (savedW == 1562 && savedH == 1200 && savedDim == 3) );

    stbi_info("test_input/inputfile1_testsave.bmp", &savedW, &savedH, &savedDim);
    REQUIRE( (savedW == 1562 && savedH == 1200 && savedDim == 3) );

    std::cout << "Image save test complete. Please also manually inspect the output folder to see if the image/s are free from any visual artifacts." << std::endl;
}

TEST_CASE("Turn image into grayscale, save & load it back to check if everything is correct", "[correctness]") {
    //File 1: 1562 * 1200 px, 3 channels
    EnhancerImage img("test_input/inputfile1.jpg");
    int originalWidth = img.width;
    int originalHeight = img.height;
    CommandLineInterface& cli = cli;
    //convert and save to disk:
    img.convertToGrayscale(cli);
    img.saveImage("test_input/inputfile1_grayscale.jpg", EnhancerImage::Filetype::jpg);

    EnhancerImage img2("test_input/inputfile1_grayscale.jpg");

    REQUIRE( (img2.width == originalWidth && img2.height == originalHeight) );

    std::cout << "Grayscale conversion test complete. Please also manually inspect the output folder to see if the image/s are free from any visual artifacts." << std::endl;
}

TEST_CASE("Apply adaptive thresholding to images", "[correctness]") {
    std::string path = "test_input";
    CommandLineInterface& cli = cli;

    for (const auto & entry : fs::directory_iterator(path)) {
        if (!entry.is_directory() && entry.path().filename().string().find("binarized") == std::string::npos) {
            EnhancerImage img(entry.path().string());
            img.applyAdaptiveThresholding(cli, 0.125, 0.15);
            img.saveImage(entry.path().string() + "_binarized.jpg", EnhancerImage::Filetype::jpg);
        }
    }

    std::cout << "Adaptive thresholding test complete. Please also manually inspect the output folder to see if the image/s are free from any visual artifacts." << std::endl;
}