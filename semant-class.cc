#include <symtab.h>
#include <string>
#include <algorithm>
#include <array>
using std::string;

#include "semant.h"
#include "cool-tree.h"

string error_message_superclass_is_in_cycle(const string &class_name);
string error_message_superclass_is_not_defined(const string &class_name, const string &parent_name);
string error_message_superclass_is_primitive(const string &class_name, const string &parent_name);

void class__class::semant(SemantContext &ctx)
{

    check_superclass_is_defined(ctx);
    check_superclass_is_not_in_cycle(ctx);
    check_superclass_is_not_primitives(ctx);
    check_Main_has_main(ctx);

    ctx.methodTable.enterscope();
    ctx.attributeTable.enterscope();
    for (auto i = features->first(); features->more(i); i = features->next(i))
    {
        features->nth(i)->check_not_redefined_and_register(ctx);
    }
    ctx.methodTable.exitscope();
    ctx.attributeTable.exitscope();
}

string error_message_class_is_redefined()
{
    return "REDEFINED";
}

void class__class::check_not_redefined_and_register(SemantContext &ctx)
{
    const auto good = ctx.classTable.probe(name) == nullptr;
    if (good)
    {
        ctx.classTable.addid(name, this);
    }
    else
    {
        ctx.semant_error() << error_message_class_is_redefined();
    }
}

void class__class::check_superclass_is_defined(SemantContext &ctx)
{
    const auto bad = ctx.classTable.probe(parent) == nullptr;
    if (bad)
    {
        ctx.semant_error(this) << error_message_superclass_is_not_defined(
                                      name->get_string(),
                                      parent->get_string())
                               << "\n";
    }
}

void class__class::check_superclass_is_not_in_cycle(SemantContext &ctx)
{
}

void class__class::check_superclass_is_not_primitives(SemantContext &ctx)
{
    auto bad = false;

    if (Bool->equal_string(parent->get_string(), parent->get_len()))
    {
        bad = true;
    }
    if (Int->equal_string(parent->get_string(), parent->get_len()))
    {
        bad = true;
    }
    if (Str->equal_string(parent->get_string(), parent->get_len()))
    {
        bad = true;
    }

    if (bad)
    {
        ctx.semant_error(this) << error_message_superclass_is_primitive(
                                      name->get_string(),
                                      parent->get_string())
                               << "\n";
    }
}

void class__class::check_Main_has_main(SemantContext &ctx)
{
}

Symbol class__class::get_filename()
{
    return filename;
}

string error_message_superclass_is_in_cycle(const string &class_name)
{
    return std::string("Class ") + class_name + std::string(", or an ancestor of ") + class_name + std::string(", is involved in an inheritance cycle.");
}

string error_message_superclass_is_not_defined(const string &class_name, const string &parent_name)
{
    return std::string("Class ") + class_name + std::string(" inherits from an undefined class ") + parent_name + std::string(".");
}

string error_message_superclass_is_primitive(const string &class_name, const string &parent_name)
{
    return std::string("Class ") + class_name + " cannot inherit class " + parent_name + ".";
}
