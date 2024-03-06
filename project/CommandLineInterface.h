#ifndef ENHANCER_COMMANDLINEINTERFACE_H
#define ENHANCER_COMMANDLINEINTERFACE_H

#include <iostream>
#include <vector>

/* CommandLineInterface:
 * This class handles all communication with the user.
 * It parses the command line arguments that were given when the program was first executed.
 * The interface has two modes of operation:
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
    const int getNumberOfThreads();
    void setNumberOfThreads(int number);
    void setVerbose(bool mode);

    bool benchmarkMode();

    //This function only prints if the user has set the "verbose" argument to true.
    enum MessageType{Error, Success, Information};
    void printDebugInformation(const std::string& message, MessageType type) const;

private:
    //These are the parameters of the main function, they contain the arguments the user passed into our program when they started it.
    int argc;
    char** argv;

    //Path of the input directory, name of the output directory (only name, since it'll be saved inside the input directory).
    std::string inputPath, outputDirectory;

    //Parameters of the adaptive thresholding argument.
    double windowWidth;
    double thresholdPercentage;

    //OpenMP-related parameters
    int numberOfThreads;

    //Verbose mode: whether if debugging information should be printed
    bool verbose = true;

    //Benchmarking mode:
    bool benchmark = false;

    //Prints the expected syntax when the user provides invalid input:
    static void printHelp();

    void startInteractiveMode();

    bool validateInput(std::string& errorMessages);

    //This is for printing a command-line argument on the left, and its description on the right. It prevents the built-in word wrapping of the console from making things really hard to read.
    static void print_twoColumnsLayout(std::vector<std::string> lines);

    //Returns the console width so you can wrap words accordingly.
    //(Defined with a preprocessor macro that handles different operating systems automatically)
    static int getConsoleSize();

    static void clearScreen();

    //Takes a long text and splits it into multiple lines (while keeping the words intact) according to the given maximum width.
    static std::vector<std::string> wrapText(const std::string& text, int width);
};

#endif //ENHANCER_COMMANDLINEINTERFACE_H
