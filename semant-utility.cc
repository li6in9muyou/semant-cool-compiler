#include "semant-utility.h"

NoRepeatPrinter err = NoRepeatPrinter();

bool NoRepeatPrinter::print(const string &text, ostream &out)
{
    if (printed.find(text) == printed.end())
    {
        printed.insert(text);
        out << text;
        return true;
    }
    else
    {
        return false;
    }
}

string location(Symbol &filename, int lineno)
{
    return string(filename->get_string()) + ":" + to_string(lineno) + ": ";
}
