#include <iostream>
#include <cstring>
#include <vector>
#include <filesystem>  //requires C++17
#include <string>
#include <sstream>
#include <iomanip>

#include "userInterface.h"
#include "termcolor.hpp"

CommandLineInterface::CommandLineInterface(int argc, char** argv): argc(argc), argv(argv), windowWidth(0.125), thresholdPercentage(0.15) {}

void CommandLineInterface::printHelp() {
    std::vector<std::string> lines{
                                "Usage syntax:", "",
                                "--interactive:", "This starts the program in the interactive mode, where you will be guided to provide all the necessary information.",
                                "-i, --inputPath <path>:", "Path of the folder that contains your scanned images.",
                                "-o, --outputDirectory <name>:", "Name of the folder in which your enhanced images are going to be saved in.",
                                "-w, --windowWidth <val>:", "[Optional] The window width that will be used in the adaptive thresholding method. This is a number between 0-1 and it is defined in terms of the width of the image, e.g. 0.125 (one-eighth).",
                                "-t, --thresholdPercentage <val>:", "[Optional] The threshold percentage that will be used in the adaptive thresholding method. This is a number between 0-1 and it is defined as a percentage, e.g. 0.15 (fifteen percent).",
                                "-h, --help:", "Show help."
                              };

    print_twoColumnsLayout(lines);

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
        std::cout << termcolor::red << "Invalid, missing or unknown arguments are detected:\n" << errorMessages << termcolor::reset << "\n";
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


//System-specific methods for Windows and Unix systems to get the console width.
#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <stdio.h>
    #include <unistd.h>
#endif

int CommandLineInterface::getConsoleSize() {
    #ifdef WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return columns;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    #endif
}

void CommandLineInterface::clearScreen()
{
    #ifdef WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

std::vector<std::string> CommandLineInterface::wrapText(const std::string& text, int width) {
    std::istringstream words(text);
    std::string word;
    std::vector<std::string> lines;
    std::string currentLine;

    while (words >> word) {
        // Check if adding the next word exceeds the width
        if (currentLine.length() + word.length() + 1 > width) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            if (!currentLine.empty()) {
                currentLine += " ";
            }
            currentLine += word;
        }
    }
    // Add the last line if it exists
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

void CommandLineInterface::print_twoColumnsLayout(std::vector<std::string> lines) {
    int width = getConsoleSize();

    if (lines.size() % 2 == 1) lines.emplace_back("");

    int leftWidth = 0;
    for(int i = 0; i < lines.size(); i+=2)
        if (lines[i].length() + 1 > leftWidth) leftWidth = lines[i].length()+1;

    int rightWidth = width - leftWidth - 1;

    for(int i = 0; i < lines.size(); i+=2) {
        std::string left = lines[i], right = lines[i+1];

        auto lines_leftCol = wrapText(left,leftWidth);
        auto lines_rightCol = wrapText(right, rightWidth);

        int max = (lines_leftCol.size() > lines_rightCol.size() ? lines_leftCol.size() : lines_rightCol.size());

        for(int i = 0; i < max; i++) {
            std::cout << std::left << std::setw(leftWidth) << (i < lines_leftCol.size() ? lines_leftCol[i] : "") << std::setw(rightWidth) << (i < lines_rightCol.size() ? lines_rightCol[i] : "");
            std::cout << std::setw(0) << "\n";
        }

        std::cout << "\n";
    }
}