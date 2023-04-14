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

void class__class::semant(SemantContext &ctx)
{
    LOG_SCOPE_FUNCTION(INFO);
    LOG_F(INFO, "class %s semant", name->get_string());
    LOG_F(INFO, "parent is %s", parent->get_string());

    auto &familyFeatureTable = ctx.programFeatureTable[name];
    ctx.familyMethodTable = &familyFeatureTable.methods;
    ctx.familyAttributeTable = &familyFeatureTable.attributes;
    LOG_F(INFO, "bind family feature table to %p", &familyFeatureTable);

    auto old_errors = ctx.errors();
    check_Main_has_main(ctx);
    if (old_errors < ctx.errors())
    {
        LOG_F(INFO, "found errors in Main.main check, return");
    }
    else
    {
        LOG_F(INFO, "Main.main check pass");
    }

    LOG_F(INFO, "descending into features");

    const auto cnt = features->len();
    for (auto i = cnt - 1; i >= 0; i -= 1)
    {
        features->nth(i)->semant(ctx);
    }
}

void class__class::create_family_feature_table(SemantContext &ctx)
{
    LOG_F(INFO, "create family feature table at %s", name->get_string());
    const auto alreadyCreated = ctx.programFeatureTable.find(name) != ctx.programFeatureTable.end();
    if (alreadyCreated)
    {
        LOG_F(INFO, "created by recursion started by derived class, return");
        return;
    }

    auto old_errors = ctx.errors();
    check_superclass_is_defined(ctx);
    check_superclass_is_not_in_cycle(ctx);
    check_superclass_is_not_primitives(ctx);
    if (old_errors < ctx.errors())
    {
        LOG_F(INFO, "found errors in class family hierarchy check, create an empty feature table");
        ctx.programFeatureTable.emplace(name, FeatureTable());
        return;
    }
    else
    {
        LOG_F(INFO, "class family hierarchy check pass");
    }

    const auto found = ctx.programFeatureTable.find(parent) != ctx.programFeatureTable.end();
    if (!found)
    {
        LOG_F(INFO, "parent's feature table is not found");
        auto *parent_class = ctx.classTable.lookup(parent);
        const auto parentIsObject = parent_class->name->equal_string(Object->get_string(), Object->get_len());
        if (parentIsObject)
        {
            LOG_F(INFO, "parent is Object, create symbol tables and register Object symbols");
            auto &familyFeatureTable = ctx.programFeatureTable[name];
            LOG_F(INFO, "create family feature table address %p for %s", &familyFeatureTable, name->get_string());
            familyFeatureTable.enterscope();
            ctx.familyMethodTable = &familyFeatureTable.methods;
            ctx.familyAttributeTable = &familyFeatureTable.attributes;
            auto *feats = parent_class->features;
            for (auto i = feats->first(); feats->more(i); i = feats->next(i))
            {
                auto *feat = feats->nth(i);
                feat->register_symbol(ctx);
            }

            LOG_F(INFO, "register my symbols at %s", name->get_string());
            familyFeatureTable.enterscope();
            ctx.familyMethodTable = &familyFeatureTable.methods;
            ctx.familyAttributeTable = &familyFeatureTable.attributes;
            for (auto i = features->first(); features->more(i); i = features->next(i))
            {
                auto *feat = features->nth(i);
                feat->register_symbol(ctx);
            }
        }
        else
        {
            LOG_F(INFO, "parent is user-defined class, recurse");
            parent_class->create_family_feature_table(ctx);
            LOG_F(INFO, "recursion returns, share parent's feature table");
            ctx.programFeatureTable[name] = ctx.programFeatureTable[parent];

            auto &familyFeatureTable = ctx.programFeatureTable[name];
            LOG_F(INFO, "register my symbols into %p at %s", &familyFeatureTable, name->get_string());
            familyFeatureTable.enterscope();
            ctx.familyMethodTable = &familyFeatureTable.methods;
            ctx.familyAttributeTable = &familyFeatureTable.attributes;
            for (auto i = features->first(); features->more(i); i = features->next(i))
            {
                auto *feat = features->nth(i);
                feat->register_symbol(ctx);
            }
        }
    }
    else
    {
        LOG_F(INFO, "parent's feature table is found");
        LOG_F(INFO, "register my symbols at %s", name->get_string());
        auto &familyFeatureTable = ctx.programFeatureTable[parent];
        familyFeatureTable.enterscope();
        ctx.familyMethodTable = &familyFeatureTable.methods;
        ctx.familyAttributeTable = &familyFeatureTable.attributes;
        for (auto i = features->first(); features->more(i); i = features->next(i))
        {
            auto *feat = features->nth(i);
            feat->register_symbol(ctx);
        }
    }
}

void class__class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "register symbol at %s", name->get_string());
    const auto good = ctx.classTable.probe(name) == nullptr;
    if (good)
    {
        LOG_F(INFO, "add new class");
        ctx.classTable.addid(name, this);
    }
    else
    {
        LOG_F(INFO, "it is redefined");
        ctx.semant_error(this)
            << error_message_class_is_redefined(name->get_string())
            << "\n";
    }
}

void class__class::check_superclass_is_defined(SemantContext &ctx)
{
    const auto bad = ctx.classTable.lookup(parent) == nullptr;
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