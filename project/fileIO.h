#ifndef ENHANCER_FILEIO_H
#define ENHANCER_FILEIO_H

#include <string>

class EnhancerImage {
public:
    int width, height, nrOfChannels;

    //Constructor:
    EnhancerImage(const std::string& path);

    //Destructor:
    ~EnhancerImage();

    enum Filetype {jpg, png, bmp};

    bool imageIsLoaded();

    bool saveImage(const std::string& path, Filetype type);

    bool convertToGrayscale();

    bool applyAdaptiveThresholding(int windowSize, float tresholdPercentage);

private:
    unsigned char* data;
};


#endif //ENHANCER_FILEIO_H
