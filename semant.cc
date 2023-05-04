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

#include "semant-arithmetics.cc"
#include "semant-assign.cc"
#include "semant-attr.cc"
#include "semant-block.cc"
#include "semant-checks.cc"
#include "semant-class.cc"
#include "semant-conditional.cc"
#include "semant-const.cc"
#include "semant-dispatch.cc"
#include "semant-formal.cc"
#include "semant-isvoid.cc"
#include "semant-let.cc"
#include "semant-logics.cc"
#include "semant-loop.cc"
#include "semant-method.cc"
#include "semant-misc.cc"
#include "semant-new.cc"
#include "semant-object.cc"
#include "semant-program.cc"
#include "semant-typcase.cc"
#include "semant-utility.cc"
