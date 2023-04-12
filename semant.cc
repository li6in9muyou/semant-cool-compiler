#include <iostream>
using std::cerr;
#include <string>
using std::string;

#include "semant.h"

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

SemantContext::SemantContext() : semant_errors(0), error_stream(cerr) {}

void SemantContext::set_filename(Symbol filename) { this->filename = filename; }

void SemantContext::abort_if_error()
{
    if (0 < errors())
    {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}

MethodTable *SemantContext::get_or_create_family_method_table(Symbol class_symbol)
{
    auto *table = classMethodLookup.lookup(class_symbol);
    if (table == nullptr)
    {
        methodStore.emplace_back();
        table = &methodStore.back();
        classMethodLookup.enterscope();
        classMethodLookup.addid(class_symbol, table);
    }
    return table;
}

AttributeTable *SemantContext::get_or_create_family_attribute_table(Symbol class_symbol)
{
    auto *table = classAttributeLookup.lookup(class_symbol);
    if (table == nullptr)
    {
        attributeStore.emplace_back();
        table = &attributeStore.back();
        classAttributeLookup.enterscope();
        classAttributeLookup.addid(class_symbol, table);
    }
    return table;
}

ostream &SemantContext::semant_error(tree_node *t)
{
    return semant_error(filename, t);
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