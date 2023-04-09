#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
#include <symtab.h>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

using std::map;
using std::set;
using std::string;
using std::vector;

void method_class::check_not_redefined_and_register(SemantContext &ctx) {}

// bool contains(const string &needle, const set<string> &haystack)
// {
//     return haystack.cend() != haystack.find(needle);
// }

// bool check_class_in_loop(
//     const string &me,
//     set<string> &mark,
//     map<string, string> &parent)
// {
//     if (contains(me, mark))
//     {
//         return true;
//     }

//     auto ans = false;

//     if (parent[me] != "" && parent[me] != "Object")
//     {
//         mark.insert(me);
//         ans = check_class_in_loop(parent[me], mark, parent);
//         mark.erase(me);
//     }

//     return ans;
// }

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

SemantContext::SemantContext() : semant_errors(0), error_stream(cerr){};

void SemantContext::abort_if_error()
{
    if (0 < errors())
    {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}

ostream &SemantContext::semant_error(Class_ c)
{
    return semant_error(c->get_filename(), c);
}

ostream &SemantContext::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream &SemantContext::semant_error()
{
    semant_errors++;
    return error_stream;
}