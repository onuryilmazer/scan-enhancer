#ifndef ENHANCER_ENHANCERIMAGE_H
#define ENHANCER_ENHANCERIMAGE_H

#include <string>
#include <list>
#include "CommandLineInterface.h"

/*
 * This class defines a custom type called "EnhancerImage" that stores the image data we read from the disk
 * and it also has some additional core functionality related to those images - like converting images into grayscale etc.
 * */

class EnhancerImage {
public:
    int width, height, nrOfChannels;

    //Constructor:
    EnhancerImage(const std::string& path);

    //Destructor:
    ~EnhancerImage();

    enum Filetype {jpg, png, bmp};

    static bool extensionIsSupported(std::string extension);

    bool imageIsLoaded();

    bool saveImage(const std::string& path, Filetype type);

    bool convertToGrayscale(CommandLineInterface& cli);

    bool applyAdaptiveThresholding(CommandLineInterface& cli, double windowSize, double tresholdPercentage);

private:
    unsigned char* data;

    static std::list<std::string> supportedFiletypes;
};

#endif //ENHANCER_ENHANCERIMAGE_H
