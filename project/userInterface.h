#ifndef ENHANCER_USERINTERFACE_H
#define ENHANCER_USERINTERFACE_H

#include <iostream>
#include <vector>

/* This class handles all communication with the user
 * Our program has two modes of operation:
 * First is the command-line tool mode, this is intended for advanced users or for automated use from within other scripts.
 * The second is the interactive mode: in this mode the program guides the user through all steps and prompts the user to give the required arguments.
 */

class CommandLineInterface {
public:
    CommandLineInterface(int argc, char** argv);

    void parseArguments();

    const std::string& getInputPath();
    const std::string& getOutputDirectory();
    const double getWindowWidth();
    const double getThresholdPercentage();


private:
    //these are the parameters of the main function, they contain the arguments the user passed into our program when they started it.
    int argc;
    char** argv;

    //Path of the input directory, name of the output directory (only name, since it'll be saved inside the input directory).
    std::string inputPath, outputDirectory;

    //Parameters of the adaptive thresholding argument.
    double windowWidth;
    double thresholdPercentage;

    static void printHelp();

    void startInteractiveMode();

    bool inputIsValid(std::string& errorMessages);

    //This is for printing a command-line argument on the left, and its description on the right. It prevents the built-in word wrapping of the console from making things really hard to read.
    static void print_twoColumnsLayout(std::vector<std::string> lines);

    //Returns the console width so you can do the word wrapping accordingly.
    //(Defined with a preprocessor macro that handles different operating systems automatically)
    static int getConsoleSize();

    static void clearScreen();

    //Takes a long text and splits it into multiple lines (while keeping the words intact) according to the given maximum width.
    static std::vector<std::string> wrapText(const std::string& text, int width);
};

#endif //ENHANCER_USERINTERFACE_H
