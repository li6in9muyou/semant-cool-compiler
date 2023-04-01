

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg = idtable.add_string("arg");
    arg2 = idtable.add_string("arg2");
    Bool = idtable.add_string("Bool");
    concat = idtable.add_string("concat");
    cool_abort = idtable.add_string("abort");
    copy = idtable.add_string("copy");
    Int = idtable.add_string("Int");
    in_int = idtable.add_string("in_int");
    in_string = idtable.add_string("in_string");
    IO = idtable.add_string("IO");
    length = idtable.add_string("length");
    Main = idtable.add_string("Main");
    main_meth = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any
    //   user-defined class.
    No_class = idtable.add_string("_no_class");
    No_type = idtable.add_string("_no_type");
    Object = idtable.add_string("Object");
    out_int = idtable.add_string("out_int");
    out_string = idtable.add_string("out_string");
    prim_slot = idtable.add_string("_prim_slot");
    self = idtable.add_string("self");
    SELF_TYPE = idtable.add_string("SELF_TYPE");
    Str = idtable.add_string("String");
    str_field = idtable.add_string("_str_field");
    substr = idtable.add_string("substr");
    type_name = idtable.add_string("type_name");
    val = idtable.add_string("_val");
}

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

void class__class::semant(SemantContext &ctx)
{
    ctx.sym.enterscope();

    ctx.sym.exitscope();
}

void class__class::check_duplicate_feature_names(SemantContext &ctx)
{
}

Symbol class__class::get_filename()
{
    return filename;
}

Symbol class__class::get_name()
{
    return name;
}

Symbol class__class::get_parent()
{
    return parent;
}

Features class__class::get_features()
{
    return features;
}

void program_class::semant()
{
    initialize_constants();

    SemantContext ctx;

    /* some semantic analysis code may go here */

    ctx.sym.enterscope();
    install_basic_classes(ctx.sym);

    check_inheritance_cycle(ctx);
    ctx.abort_if_error();

    check_superclass_undefined(ctx);
    ctx.abort_if_error();

    ctx.sym.exitscope();
}

bool contains(const string &needle, const set<string> &haystack)
{
    return haystack.cend() != haystack.find(needle);
}

bool check_class_in_loop(
    const string &me,
    set<string> &mark,
    map<string, string> &parent)
{
    if (contains(me, mark))
    {
        return true;
    }

    auto ans = false;

    if (parent[me] != "" && parent[me] != "Object")
    {
        mark.insert(me);
        ans = check_class_in_loop(parent[me], mark, parent);
        mark.erase(me);
    }

    return ans;
}

string error_message_inheritance_cycle(string class_name)
{
    return std::string("Class ") + class_name + std::string(", or an ancestor of ") + class_name + std::string(", is involved in an inheritance cycle.");
}

string error_message_superclass_undefined(string class_name, string parent_name)
{
    return std::string("Class ") + class_name + std::string(" inherits from an undefined class ") + parent_name + std::string(".");
}

void program_class::check_inheritance_cycle(SemantContext &ctx)
{
    auto parent = map<string, string>();
    auto klasses = vector<class__class *>();

    for (auto i = classes->first(); classes->more(i); i = classes->next(i))
    {
        auto *cls = (class__class *)classes->nth(i);
        parent.insert({cls->get_name()->get_string(),
                       cls->get_parent()->get_string()});
        klasses.push_back(cls);
    }

    auto mark = std::set<string>();
    std::reverse(klasses.begin(), klasses.end());
    for (const auto &cls : klasses)
    {
        auto in_loop = check_class_in_loop(cls->get_name()->get_string(), mark, parent);
        if (in_loop)
        {
            ctx.semant_error(cls)
                << error_message_inheritance_cycle(cls->get_name()->get_string())
                << std::endl;
        };
    }
}

void program_class::check_superclass_undefined(SemantContext &ctx)
{
    auto parent = map<string, string>();
    auto klasses = vector<class__class *>(classes->len());
    auto definedClasses = std::set<string>();

    auto len = classes->len();
    for (auto i = len - 1; i >= 0; i--)
    {
        auto *cls = (class__class *)classes->nth(i);
        auto parent = ctx.sym.probe(cls->get_parent());
        if (parent == nullptr)
        {
            ctx.semant_error(cls)
                << error_message_superclass_undefined(
                       cls->get_name()->get_string(),
                       cls->get_parent()->get_string())
                << std::endl;
        }
    }
}

void program_class::install_basic_classes(CoolSymbolTable &sym)
{

    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.

    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    //
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
        class_(Object,
               No_class,
               append_Features(
                   append_Features(
                       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                   single_Features(method(::copy, nil_Formals(), SELF_TYPE, no_expr()))),
               filename);

    //
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class =
        class_(IO,
               Object,
               append_Features(
                   append_Features(
                       append_Features(
                           single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                                  SELF_TYPE, no_expr())),
                           single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                                  SELF_TYPE, no_expr()))),
                       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                   single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
               filename);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer.
    //
    Class_ Int_class =
        class_(Int,
               Object,
               single_Features(attr(val, prim_slot, no_expr())),
               filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
        class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //
    Class_ Str_class =
        class_(Str,
               Object,
               append_Features(
                   append_Features(
                       append_Features(
                           append_Features(
                               single_Features(attr(val, Int, no_expr())),
                               single_Features(attr(str_field, prim_slot, no_expr()))),
                           single_Features(method(length, nil_Formals(), Int, no_expr()))),
                       single_Features(method(concat,
                                              single_Formals(formal(arg, Str)),
                                              Str,
                                              no_expr()))),
                   single_Features(method(substr,
                                          append_Formals(single_Formals(formal(arg, Int)),
                                                         single_Formals(formal(arg2, Int))),
                                          Str,
                                          no_expr()))),
               filename);

    sym.addid(Str, Str_class);
    sym.addid(Int, Int_class);
    sym.addid(Bool, Bool_class);
    sym.addid(IO, IO_class);
    sym.addid(Object, Object_class);
}

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