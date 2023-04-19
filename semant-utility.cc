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

string location(const Symbol &filename, int lineno)
{
    return string(filename->get_string()) + ":" + to_string(lineno) + ": ";
}

void abort_if_not_ok(bool ok)
{
    if (!ok)
    {
        err.print("Compilation halted due to static semantic errors.\n");
        exit(1);
    }
}
