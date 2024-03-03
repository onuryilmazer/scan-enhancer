#include <iostream>
#include <string>
#include <vector>

#ifndef ENHANCER_COLUMNPRINTER_H
#define ENHANCER_COLUMNPRINTER_H

//This class serves the sole purpose of printing text on the console but in multiple columns.
class ColumnPrinter {
public:
    static void print_twoColumnsLayout(const std::string& left, const std::string& right);

private:
    //Returns the console width so you can do the word wrapping accordingly.
    static int getConsoleSize();

    //Takes a long text, and splits it into multiple lines (keeping individual words intact) according to the given maximum width.
    static std::vector<std::string> wrapText(const std::string& text, int width);
};


#endif //ENHANCER_COLUMNPRINTER_H
