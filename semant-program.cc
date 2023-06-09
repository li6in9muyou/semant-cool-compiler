#include <symtab.h>
#include <string>
using std::string;

#include "semant.h"
#include "semant-checks.h"
#include "semant-utility.h"
#include "loguru.h"

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol
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
    val,
    method_return_type;
//
// Initializing the predefined symbols.
//
void initialize_constants(void)
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
    method_return_type = idtable.add_string("_method_return_type");
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
void program_class::semant()
{
    if (!semant_debug)
    {
        loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    }
    loguru::g_preamble_date = false;
    loguru::g_preamble_time = false;
    loguru::g_preamble_thread = false;

    const auto &no_Main = [&]()
    {
        err.print("Class Main is not defined.\n");
    };

    const auto filename = classes->nth(classes->first())->get_filename();
    SemantContext ctx;
    ctx.filename = filename;
    LOG_F(INFO, "at semant in %s", ctx.filename->get_string());
    initialize_constants();
    LOG_F(INFO, "initialized constant symbols at semant phase");

    ctx.classTable.enterscope();
    install_basic_classes(ctx);

    ctx.classTable.enterscope();

    {
        LOG_SCOPE_F(INFO, "register class symbols");
        auto ok = true;
        for (auto i = classes->first(); classes->more(i); i = classes->next(i))
        {
            auto *cls = (class__class *)classes->nth(i);
            ok &= cls->register_symbol(ctx);
        }
        abort_if_not_ok(ok);
    }

    {
        LOG_SCOPE_F(INFO, "create family feature tables");
        auto ok = true;
        for (auto i = classes->first(); classes->more(i); i = classes->next(i))
        {
            auto *cls = (class__class *)classes->nth(i);
            ok &= cls->create_family_feature_table(ctx);
        }
        abort_if_not_ok(ok);
    }

    LOG_F(INFO, "check Main class is present");
    abort_if_not_ok(check_symbol_exists(Main, ctx.classTable, no_Main));

    LOG_F(INFO, "descend into class");
    {
        auto ok = true;
        for (auto i = classes->len() - 1; i >= 0; i -= 1)
        {
            auto *cls = (class__class *)classes->nth(i);
            ok &= cls->semant(ctx);
        }
        abort_if_not_ok(ok);
    }

    ctx.classTable.exitscope();
    ctx.classTable.exitscope();
}

void program_class::install_basic_classes(SemantContext &ctx)
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

    const auto prepare_builtin_classes = [&](Class_ &c)
    {
        auto *cls = (class__class *)c;
        auto _ = cls->register_symbol(ctx);
        _ = cls->create_family_feature_table(ctx);
    };

    {
        // the order of preparation matters, latter ones depend on previous ones
        LOG_SCOPE_F(INFO, "prepare built-in classes");
        prepare_builtin_classes(Object_class);
        prepare_builtin_classes(Int_class);
        prepare_builtin_classes(Bool_class);
        prepare_builtin_classes(Str_class);
        prepare_builtin_classes(IO_class);
    }
}
