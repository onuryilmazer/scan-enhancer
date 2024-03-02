#ifndef ENHANCER_USERINTERFACE_H
#define ENHANCER_USERINTERFACE_H

#include <iostream>

class CommandLineInterface {
public:
    CommandLineInterface(int argc, char** argv);

    void parseArguments();

    const std::string& getInputPath();
    const std::string& getOutputDirectory();
    const double getWindowWidth();
    const double getThresholdPercentage();


private:
    //these are the parameters of the main function, they contain the arguments the user started the program with.
    int argc;
    char** argv;

    //Whether if the "interactive mode", in which the program guides the user, is enabled or not
    bool interactive;

    //Path of the input directory, name of the output directory (only name, since it'll be saved inside the input directory).
    std::string inputPath, outputDirectory;

    //Parameters of the adaptive thresholding argument.
    double windowWidth;
    double thresholdPercentage;

    void printHelp();

    void startInteractiveMode();

    bool inputIsValid();
};

#endif //ENHANCER_USERINTERFACE_H
