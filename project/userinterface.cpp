#include <iostream>
#include <getopt.h>
#include <cstring>
#include <vector>
#include <filesystem>  //requires C++17

#include "userinterface.h"

/*
 * This class handles all communication with the user
 * Our program has two modes:
 * First is the command-line tool mode, this is intended for advanced users or for automated use from within other scripts.
 * The second is the interactive mode: in this mode the program guides the user through all steps and prompts the user to give the required arguments. *
 */

CommandLineInterface::CommandLineInterface(int argc, char** argv): inputPath(""), outputDirectory(""), interactive(false), argc(argc), argv(argv), windowWidth(0.125), thresholdPercentage(0.15) {}

void CommandLineInterface::printHelp() {
    std::cout <<
              "--interactive:               This starts the program in the interactive mode, where you will be guided to provide all the necessary information. When this mode is chosen, no other arguments should be passed.\n"
              "--inputPath <path>:          Path to the folder that contains your scanned images\n"
              "--outputFolder <name>:       Name of the folder in which your enhanced images are going to be saved in\n"
              "--windowWidth <val>:         The window width that will be used in the adaptive thresholding method. This is defined in terms of the width of the image, e.g. 0.125 (one-eighth) of the width. It determines the size of the pixel window which will be taken into consideration when calculating the average brightness. You can play around with this argument until your results look the way you want.\n"
              "--thresholdPercentage <val>: The threshold percentage that will be used in the adaptive thresholding method. This is defined as a percentage, e.g. 0.15 (fifteen percent). It determines how bright a pixel must be in comparison to the average of its window to be binarized into 1. You can play around with this argument until your results look the way you want.\n"
              "--help:                      Show help\n";
    exit(1);
}

void CommandLineInterface::parseArguments() {
    //The colons after the characters mean that that flag takes an argument
    //E.g. n: means n needs an argument, like "-n 10"
    const char* const short_opts = "i:o:w:t:h";

    //These are the non-abbreviated versions of the flags, the user can write either -n or --num
    const option long_opts[] = {
            {"inputPath", required_argument, nullptr, 'i'},
            {"outputDirectory", required_argument, nullptr, 'o'},
            {"windowWidth", required_argument, nullptr, 'w'},
            {"thresholdPercentage", required_argument, nullptr, 't'},
            {"help", no_argument, nullptr, 'h'},
            {"interactive", no_argument, nullptr, 0},
            {nullptr, no_argument, nullptr, 0}
    };

    while (true) {
        int optindex = 0;
        int opt = getopt_long(argc, argv, short_opts, long_opts, &optindex);

        if (-1 == opt) break;

        switch (opt) {
            case 'i':
                inputPath = optarg;
                std::cout << "Input folder is set to: " << inputPath << std::endl;
                break;

            case 'o':
                outputDirectory = optarg;
                std::cout << "Output folder is set to: " << outputDirectory << std::endl;
                break;

            case 'w':
                windowWidth = std::stoi(optarg);
                std::cout << "Window width parameter of the adaptive thresholding algorithm is set to: " << windowWidth << std::endl;
                break;

            case 't':
                thresholdPercentage = std::stod(optarg);
                std::cout << "Threshold percentage parameter of the adaptive thresholding algorithm is set to: " << thresholdPercentage << std::endl;
                break;

            case 0:
                if (strcmp(long_opts[optindex].name, "interactive") == 0) {
                    interactive = true;
                    startInteractiveMode();
                }
                break;

            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                printHelp();
                break;
        }
    }

    if(!inputIsValid()) {
        std::cout << "Invalid input detected. Showing help:\n";
        printHelp();
        exit(1);  //redundant, as printHelp does it anyway, but it shows intention.
    }

}

bool CommandLineInterface::inputIsValid() {
    //TODO: add checks for the paths, i.e. if the input folder really exists and whether if an output folder can be created there.

    bool valid = true;

    if (inputPath.empty()) valid = false;

    if (outputDirectory.empty()) valid = false;

    if (windowWidth < 0 || windowWidth > 1) valid = false;

    if (thresholdPercentage < 0 || thresholdPercentage > 1) valid = false;

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

        std::cout << folders.size()+1 << "Refresh (I've copied my image folder in the same folder as the executable just now)." << "\n";

        int choice;
        std::cout << "Enter the number of the folder that contains your images: ";
        std::cin >> choice;

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

    std::string input = "";
    double widthInput = 0;

    while(true) {
        std::cout << "Enter the window width of the adaptive thresholding algorithm, in terms of the image width, i.e. a floating point number between 0 and 1 (press enter for the default value of 0.125): ";
        std::getline(std::cin, input);
        if (input == "") {
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
        if (input == "") {
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
