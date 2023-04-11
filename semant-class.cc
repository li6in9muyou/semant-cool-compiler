#include <symtab.h>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <array>
using std::map;
using std::set;
using std::string;

#include "semant.h"
#include "cool-tree.h"
#include "list.h"

string error_message_superclass_is_in_cycle(const string &class_name);
string error_message_superclass_is_not_defined(const string &class_name, const string &parent_name);
string error_message_superclass_is_primitive(const string &class_name, const string &parent_name);
string error_message_class_is_redefined(const string &class_name);
string error_message_no_main_in_Main();

void class__class::semant(SemantContext &ctx)
{

    const auto old_errors = ctx.errors();
    check_superclass_is_defined(ctx);
    check_superclass_is_not_in_cycle(ctx);
    check_superclass_is_not_primitives(ctx);
    if (old_errors < ctx.errors())
    {
        return;
    }

    ctx.methodTable.enterscope();
    ctx.attributeTable.enterscope();

    for (auto i = features->first(); features->more(i); i = features->next(i))
    {
        features->nth(i)->check_not_redefined_and_register(ctx);
    }
    check_Main_has_main(ctx);

    ctx.methodTable.exitscope();
    ctx.attributeTable.exitscope();
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
        ctx.semant_error(this)
            << error_message_class_is_redefined(name->get_string())
            << "\n";
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

bool contains(const string &needle, const std::set<string> &haystack)
{
    return haystack.cend() != haystack.find(needle);
}

bool class__class::check_class_in_loop(
    ClassTable &classTable,
    const class__class &me,
    std::set<string> &mark)
{
    const string my_name(me.name->get_string());

    if (my_name == "Object")
    {
        return false;
    }
    if (contains(my_name, mark))
    {
        return true;
    }

    auto ans = false;
    const auto my_parent = classTable.probe(me.parent);
    if (my_parent == nullptr)
    {
        return false;
    }
    mark.insert(my_name);
    ans = check_class_in_loop(classTable, *my_parent, mark);
    mark.erase(my_name);

    return ans;
}

void class__class::check_superclass_is_not_in_cycle(SemantContext &ctx)
{
    std::set<string> mark;

    const auto my_parent = ctx.classTable.probe(parent);
    if (my_parent == nullptr)
    {
        return;
    }
    const auto bad = check_class_in_loop(ctx.classTable, *my_parent, mark);
    if (bad)
    {
        ctx.semant_error(this)
            << error_message_superclass_is_in_cycle(this->name->get_string())
            << "\n";
    }
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
    if (name->equal_string("Main", 4))
    {
        const auto not_found = nullptr == ctx.methodTable.probe(main_meth);
        if (not_found)
        {
            ctx.semant_error(this)
                << error_message_no_main_in_Main()
                << "\n";
        }
    }
}

Symbol class__class::get_filename()
{
    return filename;
}

Symbol class__class::get_name()
{
    return name;
}

string error_message_superclass_is_in_cycle(const string &class_name)
{
    return "Class " + class_name + ", or an ancestor of " + class_name + ", is involved in an inheritance cycle.";
}

string error_message_superclass_is_not_defined(const string &class_name, const string &parent_name)
{
    return "Class " + class_name + " inherits from an undefined class " + parent_name + ".";
}

string error_message_superclass_is_primitive(const string &class_name, const string &parent_name)
{
    return "Class " + class_name + " cannot inherit class " + parent_name + ".";
}

string error_message_class_is_redefined(const string &class_name)
{
    return "Class " + class_name + " was previously defined.";
}

string error_message_no_main_in_Main()
{
    return "No 'main' method in class Main.";
}