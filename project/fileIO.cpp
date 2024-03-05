#include "fileIO.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include <string>
#include <iostream>
#include <stdexcept>


//Constructor:
EnhancerImage::EnhancerImage(const std::string& path) {
    data = stbi_load(path.c_str(), &width, &height, &nrOfChannels, 0);

    if (!data) {
        std::cerr << "Image failed to load at path: " << path << std::endl;
    }
}

//Destructor:
EnhancerImage::~EnhancerImage() {
    stbi_image_free(data);
}

//Check if the image was loaded correctly:
bool EnhancerImage::imageIsLoaded() {
    return (data == nullptr);
}

//Save image in given format:
bool EnhancerImage::saveImage(const std::string& path, Filetype type) {
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

//Converts image to grayscale, single channel.
bool EnhancerImage::convertToGrayscale() {
    if (nrOfChannels < 3) {
        std::cerr << "Image can't be converted to grayscale (it may be already converted)" << std::endl;
        return false;
    }

    int imageSize = width * height * nrOfChannels;
    int grayscale_imageSize = width * height * 1;   //alpha channel of the original image will be discarded, if it exists.

    auto *grayscale_data = new unsigned char[grayscale_imageSize];

    //Two pointers, one iterates over the original picture, the other one over the new grayscale image.
    for(unsigned char *p = data, *pg = grayscale_data; p != data + imageSize; p += nrOfChannels, pg += 1) {
        //Take the average of RGB values of the original picture's current pixel
        //and assign it to the grayscale picture's current pixel.
        *pg = static_cast<char>( ((*p + *(p + 1) + *(p + 2)) / 3.0) );
    }

    //release the memory used by the original image
    stbi_image_free(data);

    //update image information
    data = grayscale_data;
    nrOfChannels = 1;

    return true;
}


bool EnhancerImage::applyAdaptiveThresholding(double windowSize, double tresholdPercentage) {
    //Adaptive thresholding works on grayscale images: check if the image is suitable first (convert if not)
    if (nrOfChannels > 1) convertToGrayscale();

    int windowSize_pixels = width * windowSize;  //determine window size in pixels

    //Create integral image (sum of brightnesses)
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
    int x1, x2, y1, y2;
    int halfWindow = windowSize_pixels/2;

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

            //will be used to calculate the avg. intensity of the pixels
            int nrOfPixelsInWindow = (x2-x1) * (y2-y1);

            //Calculate the sum of the window
            unsigned long intensitySum = integralImage[y2*width+x2] - integralImage[y1*width+x2] - integralImage[y2*width+x1] + integralImage[y1*width+x1];

            //Check if the current pixel's value is x percent lower than the average (-> black) or not (-> white)
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
