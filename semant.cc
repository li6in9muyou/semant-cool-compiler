#include <iostream>
using std::cerr;

#include "semant.h"

extern int semant_debug;
extern char *curr_filename;

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