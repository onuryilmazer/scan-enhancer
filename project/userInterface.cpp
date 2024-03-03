#include <iostream>
#include <cstring>
#include <vector>
#include <filesystem>  //requires C++17
#include <string>
#include <sstream>

#include "userInterface.h"
#include "columnPrinter.h"

/*
 * This class handles all communication with the user
 * Our program has two modes:
 * First is the command-line tool mode, this is intended for advanced users or for automated use from within other scripts.
 * The second is the interactive mode: in this mode the program guides the user through all steps and prompts the user to give the required arguments. *
 */
CommandLineInterface::CommandLineInterface(int argc, char** argv): argc(argc), argv(argv), windowWidth(0.125), thresholdPercentage(0.15) {}

void CommandLineInterface::printHelp() {
    std::cout << "Usage syntax: \n";
    ColumnPrinter::print_twoColumnsLayout("--interactive:", "This starts the program in the interactive mode, where you will be guided to provide all the necessary information.");
    ColumnPrinter::print_twoColumnsLayout("-i <path> OR --inputPath <path>:", "Path of the folder that contains your scanned images.");
    ColumnPrinter::print_twoColumnsLayout("-o <name> OR --outputDirectory <name>:", "Name of the folder in which your enhanced images are going to be saved in.");
    ColumnPrinter::print_twoColumnsLayout("-w <val> OR --windowWidth <val>:", "The window width that will be used in the adaptive thresholding method. This is a number between 0-1 and it is defined in terms of the width of the image, e.g. 0.125 (one-eighth).");
    ColumnPrinter::print_twoColumnsLayout("-t <val> OR --thresholdPercentage <val>:", "The threshold percentage that will be used in the adaptive thresholding method. This is a number between 0-1 and it is defined as a percentage, e.g. 0.15 (fifteen percent).");
    ColumnPrinter::print_twoColumnsLayout("-h OR --help:", "Show help.");

    exit(1);
}

void CommandLineInterface::parseArguments() {
    //VERSION 2:
    //We are no longer using "getopt" to parse arguments, because it only works on linux and not on windows; Powershell and CMD produce no output when running the executable.
    //Getopt still works when using MinGW or git bash but that isn't ideal for portability.

    std::string errorMessages;
    bool readingErrors = false;

    for(int i = 1; i < argc; i++) {
        std::string arg = argv[i];  //get the current argument.

        if (arg == "-h" || arg == "--help") {
            printHelp();
            exit(0);
        }
        else if (arg == "--interactive") {
            startInteractiveMode();
            return;
        }
        else if (arg == "-i" || arg == "--inputPath") {
            if (i + 1 < argc) {
                inputPath = argv[++i];
            }
        }
        else if (arg == "-o" || arg == "--outputDirectory") {
            if (i + 1 < argc) {
                outputDirectory = argv[++i];
            }
        }
        else if (arg == "-w" || arg == "--windowWidth") {
            if (i + 1 < argc) {
                std::istringstream numberstream(argv[++i]);
                if (!(numberstream >> windowWidth)) {
                    errorMessages += "Invalid windowWidth argument.\n";
                    readingErrors = true;
                }
            }
        }
        else if (arg == "-t" || arg == "--thresholdPercentage") {
            if (i + 1 < argc) {
                std::istringstream numberstream(argv[++i]);
                if (!(numberstream >> thresholdPercentage)) {
                    errorMessages += "Invalid thresholdPercentage argument.\n";
                    readingErrors = true;
                }
            }
        }
        else {
            std::string type = arg.substr(0,1) == "-" ? "argument: " : "value: ";
            errorMessages += "Unknown " + type + arg + "\n";
            readingErrors = true;
        }
    }

    if(readingErrors || !inputIsValid(errorMessages)) {
        std::cout << "Invalid, missing or unknown arguments are detected:\n" << errorMessages << "\n\n";
        printHelp();
        exit(1);  //redundant, as printHelp does it anyway, but it shows intention.
    }
}

bool CommandLineInterface::inputIsValid(std::string& errorMessages) {
    //TODO: add checks for the paths, i.e. if the input folder really exists and whether if an output folder can be created there.

    errorMessages = "";

    bool valid = true;

    if (inputPath.empty()) {
        errorMessages += "Input path can't be empty.\n";
        valid = false;
    }

    if (outputDirectory.empty()) {
        errorMessages += "Output directory can't be empty.\n";
        valid = false;
    }

    if (windowWidth < 0 || windowWidth > 1) {
        errorMessages += "Window width needs to be a floating point number between 0 and 1.\n";
        valid = false;
    }

    if (thresholdPercentage < 0 || thresholdPercentage > 1) {
        errorMessages += "Threshold percentage needs to be a floating point number between 0 and 1.\n";
        valid = false;
    }

    return valid;
}

void CommandLineInterface::startInteractiveMode() {
    while(true) {
        std::vector<std::string> folders;
        for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
            if (entry.is_directory()) {
                folders.push_back(entry.path().filename().string());
            }
        }

        std::cout << "Folders in current directory:\n";
        for (size_t i = 0; i < folders.size(); ++i) {
            std::cout << i + 1 << ". " << folders[i] << '\n';
        }

        std::cout << folders.size()+1 << ". Refresh (I've copied my image folder in the same folder as the executable just now)." << "\n";

        int choice;
        std::cout << "Enter the number of the folder that contains your images: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cout << "Please enter an integer.\n" << std::endl;
            std::cin.clear();  //clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //ignore the lingering input.
            continue;  //restart the loop
        }

        if(choice == folders.size()+1) continue;  //refresh folders list and try again.

        if (choice > 0 && choice <= folders.size()) {
            inputPath = folders[choice - 1];
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }
    }

    std::cout << "Enter the desired name of the output folder: ";
    std::cin >> outputDirectory;

    std::string input;
    double widthInput = 0;

    //from this point on "getline" is used, so we clean the "\n" cin leaves behind one final time (getline does not leave a "\n" in the buffer)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while(true) {
        std::cout << "Enter the window width of the adaptive thresholding algorithm, in terms of the image width, i.e. a floating point number between 0 and 1 (press enter for the default value of 0.125): ";
        std::getline(std::cin, input);
        if (input.empty()) {
            windowWidth = 0.125;
            break;
        }
        else {
            std::istringstream numberstream(input);
            if (! (numberstream >> widthInput) || widthInput > 1 || widthInput < 0) {
                std::cout << "Please enter a valid number between 0 and 1.\n";
                continue;
            }
            else {
                windowWidth = widthInput;
                break;
            }
        }
    }

    double thresholdInput = 0;
    while(true) {
        std::cout << "Enter the threshold percentage of the adaptive thresholding algorithm, i.e. a floating point number between 0 and 1 (press enter for the default value of 0.15): ";
        std::getline(std::cin, input);
        if (input.empty()) {
            thresholdPercentage = 0.15;
            break;
        }
        else {
            std::istringstream percentstream(input);
            if (! (percentstream >> thresholdInput) || thresholdInput < 0 || thresholdInput > 1) {
                std::cout << "Please enter a valid number between 0 and 1.\n";
                continue;
            }
            else {
                thresholdPercentage = thresholdInput;
                break;
            }
        }
    }
}


const std::string& CommandLineInterface::getInputPath() {
    return inputPath;
}

const std::string& CommandLineInterface::getOutputDirectory() {
    return outputDirectory;
}

const double CommandLineInterface::getWindowWidth() {
    return windowWidth;
}

const double CommandLineInterface::getThresholdPercentage() {
    return thresholdPercentage;
}
