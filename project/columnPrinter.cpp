#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#include "columnPrinter.h"


//System-specific methods to get the console width:
#ifdef WIN32
#include <windows.h>
int ColumnPrinter::getConsoleSize() {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return columns;
    }
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
    int CommandLineInterface::getConsoleSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}
#endif

std::vector<std::string> ColumnPrinter::wrapText(const std::string& text, int width) {
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

void ColumnPrinter::print_twoColumnsLayout(const std::string& left, const std::string& right) {
    int width = getConsoleSize();
    int leftWidth = width/3;
    int rightWidth = width-leftWidth-1;

    auto lines_leftCol = wrapText(left,leftWidth);
    auto lines_rightCol = wrapText(right, rightWidth);

    int max = (lines_leftCol.size() > lines_rightCol.size() ? lines_leftCol.size() : lines_rightCol.size());
    //int max = std::max(lines_leftCol.size(), lines_rightCol.size());

    for(int i = 0; i < max; i++) {
        std::cout << std::left << std::setw(leftWidth) << (i < lines_leftCol.size() ? lines_leftCol[i] : "") << std::setw(rightWidth) << (i < lines_rightCol.size() ? lines_rightCol[i] : "");
        std::cout << std::setw(0) << "\n";
    }

    std::cout << "\n";
}