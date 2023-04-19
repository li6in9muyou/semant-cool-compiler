#include <iostream>
using std::cerr;
#include <string>
using std::string;

#include "semant.h"
#include "loguru.h"

extern int semant_debug;
extern char *curr_filename;

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& SemantContext::semant_error()
//
//    ostream& SemantContext::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& SemantContext::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

void FeatureTable::enterscope()
{
    attributes.enterscope();
    methods.enterscope();
}

void FeatureTable::exitscope()
{
    attributes.exitscope();
    methods.exitscope();
}
