#include <symtab.h>
#include <string>
using std::string;
using std::to_string;
#include <set>
using std::set;
#include <map>
using std::map;
#include <algorithm>
using std::all_of;

#include "semant.h"
#include "semant-checks.h"
#include "semant-utility.h"
#include "cool-tree.h"
#include "list.h"
#include "loguru.h"

bool class__class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "class %s semant", name->get_string());
    LOG_F(INFO, "hierarchy hash of %s is \"%s\"",
          name->get_string(), dump_symbols(ctx.familyHierarchyHash[name]).c_str());

    auto &familyFeatureTable = ctx.programFeatureTable[name];
    ctx.familyMethodTable = &familyFeatureTable.methods;
    ctx.typeEnv = &familyFeatureTable.attributes;
    LOG_F(INFO, "bind family feature table to %p", &familyFeatureTable);

    ctx.classTable.enterscope();
    ctx.classTable.addid(SELF_TYPE, this);

    ctx.typeEnv->enterscope();
    ctx.typeEnv->addid(SELF_TYPE, &name);
    ctx.typeEnv->addid(self, &SELF_TYPE);

    LOG_F(INFO, "check Main class has main method");
    if (check_symbol_eq(name, Main))
    {
        check_symbol_exists_in_current_scope(
            main_meth,
            *ctx.familyMethodTable,
            [&]()
            {
                err.print(location(get_filename(), get_line_number()) +
                          "No 'main' method in class Main.\n");
            });
    }

    LOG_F(INFO, "descending into features");

    auto ok = true;
    const auto cnt = features->len();
    for (auto i = cnt - 1; i >= 0; i -= 1)
    {
        ok &= features->nth(i)->semant(ctx);
    }

    ctx.classTable.exitscope();
    ctx.typeEnv->exitscope();
    return ok;
}

bool class__class::create_family_feature_table(SemantContext &ctx)
{
    LOG_F(INFO, "create family feature table at %s", name->get_string());
    const auto alreadyCreated = ctx.programFeatureTable.find(name) != ctx.programFeatureTable.end();
    if (alreadyCreated)
    {
        LOG_F(INFO, "created by recursion started by derived class, return");
        return true;
    }

    if (check_symbol_not_eq(Object, name))
    {
        auto superclassOk = true;
        LOG_F(INFO, "check superclass is defined");
        superclassOk &= check_symbol_exists(
            parent,
            ctx.classTable,
            [&]()
            {
                err.print(
                    location(get_filename(), get_line_number()) +
                    "Class " + name->get_string() + " inherits from an undefined class " + parent->get_string() + ".\n");
            });
        if (!superclassOk)
        {
            return false;
        }

        LOG_F(INFO, "check superclass is not primitives");
        const auto e = [&]()
        {
            err.print(
                location(get_filename(), get_line_number()) +
                "Class " + name->get_string() + " cannot inherit class " + parent->get_string() + ".\n");
        };
        superclassOk &= check_symbol_not_eq(parent, Bool, e);
        superclassOk &= check_symbol_not_eq(parent, Int, e);
        superclassOk &= check_symbol_not_eq(parent, Str, e);
        if (!superclassOk)
        {
            return false;
        }

        LOG_F(INFO, "check superclass is not in inheritance cycle");
        auto mark = std::set<string>();
        if (!check_class_in_loop(ctx.classTable, *ctx.classTable.lookup(parent), mark))
        {
            err.print(
                location(get_filename(), get_line_number()) +
                "Class " + name->get_string() + ", or an ancestor of " + name->get_string() + ", is involved in an inheritance cycle.\n");
            return false;
        }
    }

    const auto foundFamilyFeatureTable = ctx.programFeatureTable.find(parent) != ctx.programFeatureTable.end();
    const auto parentIsNoClass = parent->equal_string(No_class->get_string(), No_class->get_len());
    if (parentIsNoClass || foundFamilyFeatureTable)
    {
        FeatureTable *familyFeatureTable;
        if (parentIsNoClass)
        {
            LOG_F(INFO, "parent is No_class, do nothing");
        }
        if (foundFamilyFeatureTable)
        {
            LOG_F(INFO, "family feature table and hierarchy hash are found, inherit");
            ctx.programFeatureTable[name] = ctx.programFeatureTable[parent];
            ctx.familyHierarchyHash[name] = ctx.familyHierarchyHash[parent];
        }
        familyFeatureTable = &ctx.programFeatureTable[name];
        ctx.familyHierarchyHash[name].push_back(name);

        {
            LOG_SCOPE_F(INFO, "register my features in %p at %s", familyFeatureTable, name->get_string());
            LOG_F(INFO, "family hierarchy hash is %s", dump_symbols(ctx.familyHierarchyHash[name]).c_str());

            familyFeatureTable->enterscope();
            ctx.familyMethodTable = &familyFeatureTable->methods;
            ctx.typeEnv = &familyFeatureTable->attributes;

            vector<bool> results;
            for (auto i = features->first(); features->more(i); i = features->next(i))
            {
                results.emplace_back(features->nth(i)->register_symbol(ctx));
            }
            return all_of(results.cbegin(), results.cend(), [](bool ok)
                          { return ok; });
        }
    }

    LOG_F(INFO, "class family hierarchy check pass");
    auto *parent_class = ctx.classTable.lookup(parent);
    const auto ok = parent_class->create_family_feature_table(ctx);
    if (!ok)
    {
        LOG_F(INFO, "found errors in parent feature table creation, return");
        return false;
    }
    else
    {
        return this->create_family_feature_table(ctx);
    }
}

bool class__class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "register symbol at class %s", name->get_string());

    const auto ok = check_symbol_not_exists(
        name,
        ctx.classTable,
        [&]()
        {
            LOG_F(INFO, "it is redefined");
            err.print(
                location(get_filename(), get_line_number()) +
                "Class " + name->get_string() + " was previously defined.\n");
        });
    if (ok)
    {
        LOG_F(INFO, "add new class");
        ctx.classTable.addid(name, this);
    }

    return ok;
}

bool contains(const string &needle, const std::set<string> &haystack)
{
    return haystack.cend() != haystack.find(needle);
}

bool class__class::check_class_in_loop(
    SymbolTable<Symbol, class__class> &classTable,
    const class__class &me,
    std::set<string> &mark)
{
    LOG_SCOPE_F(INFO, "check inheritance loop on %s", me.name->get_string());
    if (check_symbol_eq(me.name, No_class))
    {
        LOG_F(INFO, "parent is No_class, return true");
        return true;
    }

    const string my_name(me.name->get_string());
    if (contains(my_name, mark))
    {
        LOG_F(INFO, "parent has been visited before, return false");
        return false;
    }

    const auto my_parent = classTable.lookup(me.parent);
    if (my_parent == nullptr)
    {
        LOG_F(INFO, "parent %s is nullptr, return true", me.parent->get_string());
        return true;
    }

    mark.insert(my_name);
    const auto ok = check_class_in_loop(classTable, *my_parent, mark);
    mark.erase(my_name);

    LOG_IF_F(INFO, ok, "return true");
    LOG_IF_F(INFO, !ok, "return true");
    return ok;
}

Symbol class__class::get_filename()
{
    return filename;
}
