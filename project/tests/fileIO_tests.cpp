#include "catch.hpp"
#include "../fileIO.h"
#include "stb_image.h"
#include <iostream>
#include <filesystem>
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

    std::cout << "Please manually inspect the output folder to see if the image/s are free from any visual artifacts." << std::endl;
}

TEST_CASE("Turn image into grayscale, save & load it back to check if everything is correct", "[correctness]") {
    //File 1: 1562 * 1200 px, 3 channels
    EnhancerImage img("test_input/inputfile1.jpg");
    int originalWidth = img.width;
    int originalHeight = img.height;

    //convert and save to disk:
    img.convertToGrayscale();
    img.saveImage("test_input/inputfile1_grayscale.jpg", EnhancerImage::Filetype::jpg);

    EnhancerImage img2("test_input/inputfile1_grayscale.jpg");

    REQUIRE( (img2.width == originalWidth && img2.height == originalHeight) );

    std::cout << "Please manually inspect the output folder to see if the image/s are free from any visual artifacts." << std::endl;
}

TEST_CASE("Apply adaptive thresholding to images", "[correctness]") {
    std::string path = "test_input";

    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.path().filename().string().find("binarized") == std::string::npos) {  //entry.path().filename().string().find("heft") == 0 &&
            EnhancerImage img(entry.path().string());
            img.applyAdaptiveThresholding(img.width/8, 0.15f);
            img.saveImage(entry.path().string() + "_binarized.jpg", EnhancerImage::Filetype::jpg);
        }
    }

    std::cout << "Please manually inspect the output folder to see if the image/s are free from any visual artifacts." << std::endl;
}