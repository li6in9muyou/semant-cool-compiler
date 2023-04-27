#pragma once

#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::cerr;
using std::ostream;
#include <unordered_set>
using std::unordered_set;

#include "stringtab.h"
#include "cool-tree.h"

#include "semant.h"

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

void abort_if_not_ok(bool ok);

bool set_type_if_ok(bool ok, Expression e, const Symbol &ok_type, const Symbol &fallback_type = Object);
