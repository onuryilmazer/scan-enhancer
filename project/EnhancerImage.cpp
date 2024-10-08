#include <string>
#include <iostream>
#include <algorithm>
#include <cstdint> //for termcolor
#include <omp.h>
#include <filesystem>

#include "EnhancerImage.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "termcolor.hpp"

//Constructor:
EnhancerImage::EnhancerImage(const std::string& path) {
    data = stbi_load(path.c_str(), &width, &height, &nrOfChannels, 0);

    if (!data) {
        std::cerr << termcolor::red << "Image failed to load at path: " << path << termcolor::reset << std::endl;
    }
}

std::list<std::string> EnhancerImage::supportedFiletypes = {".jpg", ".png", ".bmp"};

//Destructor:
EnhancerImage::~EnhancerImage() {
    stbi_image_free(data);
}

bool EnhancerImage::extensionIsSupported(std::string extension) {
    //convert the extension to lowercase before comparing it with our known extensions
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });

    // Check the extension to see if the file is an image
    for (const std::string& validExtension : EnhancerImage::supportedFiletypes) {
        if (extension == validExtension) return true;
    }

    return false;
}

//Check if the image was loaded correctly:
bool EnhancerImage::imageIsLoaded() {
    return (data == nullptr);
}

//Save image in given format:
bool EnhancerImage::saveImage(const std::string& path, Filetype type) {
    try {
        std::filesystem::path outputDir(path);
        if (std::filesystem::create_directory(outputDir.parent_path())) {
            std::cout << termcolor::green << "Created the parent directory of the image: " << termcolor::reset << outputDir << std::endl;
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
    }

    int result = 0;

    switch (type) {
        case jpg:
            result = stbi_write_jpg(path.c_str(), width, height, nrOfChannels, data, 100);
            break;

        case png:
            result = stbi_write_png(path.c_str(), width, height, nrOfChannels, data, (width * nrOfChannels));
            break;

        case bmp:
            result = stbi_write_bmp(path.c_str(), width, height, nrOfChannels, data);
            break;
    }

    //From the stb header file:
    //...each function returns 0 on failure and non-0 on success.
    return result;  //0 is implicitly converted to "false"

}

//Converts image to grayscale, single channel in parallel
bool EnhancerImage::convertToGrayscale(int nrOfThreads) {
    if (nrOfChannels < 3) {
        std::cerr << termcolor::red << "Image can't be converted to grayscale (it may already be converted)"
                  << termcolor::reset << std::endl;
        return false;
    }

    int grayscale_imageSize = width * height * 1;   //alpha channel of the original image will be discarded, if it exists.
    auto *grayscale_data = new unsigned char[grayscale_imageSize];

    int PixelsPerThread = (width * height) / nrOfThreads;
    int LeftoverPixels = (width * height) % nrOfThreads;

    omp_set_nested(1);

#pragma omp parallel num_threads(nrOfThreads) firstprivate(PixelsPerThread)
    {
        int threadNum = omp_get_thread_num();
        unsigned char *p = data + (threadNum * PixelsPerThread * nrOfChannels);
        unsigned char *pg = grayscale_data + (threadNum * PixelsPerThread * 1);

        // Assign leftover pixels to master thread
        if (threadNum == 0) {
            PixelsPerThread += LeftoverPixels;
        }

        //Two pointers, one iterates over the original picture, the other one over the new grayscale image.
        for (int i = 0; i < PixelsPerThread; i ++) {
            //Take the average of RGB values of the original picture's current pixel
            //and assign it to the grayscale picture's current pixel.
            *pg = static_cast<char>((*p + *(p+1) + *(p+2)) / nrOfChannels);
            pg++;
            p += nrOfChannels;
        }
    }

// Release the memory used by the original image
    stbi_image_free(data);

// Update image information
    data = grayscale_data;
    nrOfChannels = 1;

    return true;
}


bool EnhancerImage::applyAdaptiveThresholding(int nrOfThreads_grayscaleConversion, double windowSize, double tresholdPercentage) {
    //Adaptive thresholding works on grayscale images: check if the image is suitable first (convert if not)
    if (nrOfChannels > 1) convertToGrayscale(nrOfThreads_grayscaleConversion);

    //Create the integral image (sum of brightness values within a certain area)
    auto *integralImage = new unsigned long [width*height];

    for(int column = 0; column < width; column++) {
        unsigned long columnSum = 0;

        for(int row = 0; row < height; row++) {
            int index = row*width + column;

            columnSum += data[index];
            if (column == 0) {
                integralImage[index] = columnSum;
            }
            else {
                integralImage[index] = columnSum + integralImage[index-1];
            }
        }
    }

    //buffer for the new, binarized image:
    auto binarized = new unsigned char[width * height];

    //window variables
    int windowSize_pixels = static_cast<int>(width * windowSize);   //determine the window size in pixels
    int halfWindow = windowSize_pixels/2;
    int x1, x2, y1, y2;                                             //these temporary variables will hold the coordinates of the four ends of our window.

    //Perform adaptive thresholding
    for(int column = 0; column < width; column++) {
        for(int row = 0; row < height; row++) {

            int index = row*width + column;

            //Calculate the SxS window:
            x1 = column - halfWindow;
            x2 = column + halfWindow;
            y1 = row - halfWindow;
            y2 = row + halfWindow;

            //check for image borders
            if (x1 < 0) x1 = 0;
            if (x2 >= width) x2 = width-1;
            if (y1 < 0) y1 = 0;
            if (y2 >= height) y2 = height-1;

            //this value will be used to calculate the avg. intensity of the pixels
            int nrOfPixelsInWindow = (x2-x1) * (y2-y1);

            //Calculate the sum of the window
            unsigned long intensitySum = integralImage[y2*width+x2] - integralImage[y1*width+x2] - integralImage[y2*width+x1] + integralImage[y1*width+x1];

            //Decide whether the pixel should be white or black
            //Criterium: Whether if the current pixel's brightness value is T percent higher than the average (-> white) or not (-> black)
            bool aboveThreshold = static_cast<unsigned long>(data[index] * static_cast<unsigned long>(nrOfPixelsInWindow)) > static_cast<unsigned long>(intensitySum*(1.0-tresholdPercentage));

            if(aboveThreshold) {
                binarized[index] = 255;
            }
            else {
                binarized[index] = 0;
            }
        }
    }

    //free up dynamically allocated memory
    delete[] integralImage;

    //delete original image and replace it with the binarized version
    stbi_image_free(data);
    data = binarized;

    return true;
}