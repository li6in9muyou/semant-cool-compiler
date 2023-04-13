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
#include "loguru.h"

string error_message_superclass_is_in_cycle(const string &class_name);
string error_message_superclass_is_not_defined(const string &class_name, const string &parent_name);
string error_message_superclass_is_primitive(const string &class_name, const string &parent_name);
string error_message_class_is_redefined(const string &class_name);
string error_message_no_main_in_Main();

AttributeTable *class__class::get_family_attribute_table(SemantContext &ctx)
{
    const auto parentIsObject = Object->equal_string(parent->get_string(), parent->get_len());
    const auto parentIsIO = IO->equal_string(parent->get_string(), parent->get_len());

    LOG_F(INFO, "parent is %s", parent->get_string());
    if (parentIsObject || parentIsIO)
    {
        auto &attributeTable = ctx.attributeStore.emplace_back();
        const auto se = ctx.programAttributeTable.addid(name, &attributeTable);
        LOG_F(INFO, "create new mapping %s -> attributeTable", se->get_id()->get_string());
        return se->get_info();
    }
    else
    {
        const auto attributeTable = ctx.programAttributeTable.probe(parent);
        LOG_IF_F(ERROR, attributeTable == nullptr, "%s has no parent and is not a direct descentant of Object/IO", name->get_string());
        return attributeTable;
    }
}

MethodTable *class__class::get_family_method_table(SemantContext &ctx)
{
    const auto parentIsObject = Object->equal_string(parent->get_string(), parent->get_len());
    const auto parentIsIO = IO->equal_string(parent->get_string(), parent->get_len());

    LOG_F(INFO, "parent is %s", parent->get_string());
    if (parentIsObject || parentIsIO)
    {
        auto &methodTable = ctx.methodStore.emplace_back();
        const auto se = ctx.programMethodTable.addid(name, &methodTable);
        LOG_F(INFO, "create new mapping %s -> methodTable", se->get_id()->get_string());
        return se->get_info();
    }
    else
    {
        const auto methodTable = ctx.programMethodTable.probe(parent);
        LOG_IF_F(ERROR, methodTable == nullptr, "%s has no parent and is not a direct descentant of Object/IO", name->get_string());
        return methodTable;
    }
}

void class__class::semant(SemantContext &ctx)
{
    LOG_SCOPE_FUNCTION(INFO);
    LOG_F(INFO, "class %s semant", name->get_string());

    auto old_errors = ctx.errors();

    check_superclass_is_defined(ctx);
    check_superclass_is_not_in_cycle(ctx);
    check_superclass_is_not_primitives(ctx);
    if (old_errors < ctx.errors())
    {
        LOG_F(INFO, "found errors in superclass check, return");
        return;
    }
    else
    {
        LOG_F(INFO, "class superclass check pass");
    }

    ctx.familyMethodTable = get_family_method_table(ctx);
    ctx.familyAttributeTable = get_family_attribute_table(ctx);

    ctx.familyMethodTable->enterscope();
    ctx.familyAttributeTable->enterscope();

    old_errors = ctx.errors();
    for (auto i = features->first(); features->more(i); i = features->next(i))
    {
        features->nth(i)->check_not_redefined_and_register(ctx);
    }
    if (old_errors < ctx.errors())
    {
        LOG_F(INFO, "found errors in feature declaration check, return");
    }
    else
    {
        LOG_F(INFO, "feature declaration check pass");
    }

    old_errors = ctx.errors();
    check_Main_has_main(ctx);
    if (old_errors < ctx.errors())
    {
        LOG_F(INFO, "found errors in Main.main check, return");
    }
    else
    {
        LOG_F(INFO, "Main.main check pass");
    }

    LOG_F(INFO, "descending into semant");

    const auto cnt = features->len();
    for (auto i = cnt - 1; i >= 0; i -= 1)
    {
        features->nth(i)->semant(ctx);
    }
}

void class__class::check_not_redefined_and_register(SemantContext &ctx)
{
    LOG_F(INFO, "class %s precheck", name->get_string());
    const auto good = ctx.classTable.probe(name) == nullptr;
    if (good)
    {
        LOG_F(INFO, "classTable.addid");
        ctx.classTable.addid(name, this);
    }
    else
    {
        LOG_F(INFO, "error: redefined");
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
        const auto not_found = nullptr == ctx.familyMethodTable->probe(main_meth);
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