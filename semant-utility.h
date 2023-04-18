#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::cerr;
using std::ostream;
#include <unordered_set>
using std::unordered_set;

#include "stringtab.h"

class NoRepeatPrinter
{
private:
    unordered_set<string> printed;

public:
    bool print(const string &text, ostream &out = cerr);
    NoRepeatPrinter() = default;
};

extern NoRepeatPrinter err;

string location(const Symbol &filename, int lineno);
