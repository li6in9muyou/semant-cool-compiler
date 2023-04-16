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
#include "cool-tree.h"
#include "list.h"
#include "loguru.h"
#include "semant-error-utility.h"
using semant_errors::ErrorType;
using semant_errors::K;
using semant_errors::report_errors;

bool class__class::semant(SemantContext &ctx)
{
    LOG_SCOPE_FUNCTION(INFO);
    LOG_F(INFO, "class %s semant", name->get_string());
    LOG_F(INFO, "parent is %s", parent->get_string());

    auto &familyFeatureTable = ctx.programFeatureTable[name];
    ctx.familyMethodTable = &familyFeatureTable.methods;
    ctx.familyAttributeTable = &familyFeatureTable.attributes;
    LOG_F(INFO, "bind family feature table to %p", &familyFeatureTable);

    {
        LOG_SCOPE_F(INFO, "check Main class has main method");
        const auto ok = check_Main_has_main(ctx);
        if (!ok)
        {
            report_errors(ErrorType::NoMainInClassMain,
                          {{K::methodName, name->get_string()},
                           {K::lineNumber, to_string(line_number)}});
        }
    }
    auto old_errors = ctx.errors();
    if (old_errors < ctx.errors())
    {
        LOG_F(INFO, "found errors in Main.main check, return");
    }
    else
    {
        LOG_F(INFO, "Main.main check pass");
    }

    LOG_F(INFO, "descending into features");

    vector<bool> results;
    const auto cnt = features->len();
    for (auto i = cnt - 1; i >= 0; i -= 1)
    {
        const auto ok = features->nth(i)->semant(ctx);
        results.emplace_back(ok);
    }
    return all_of(results.cbegin(), results.cend(), [](bool ok)
                  { return ok; });
}

bool class__class::create_family_feature_table(SemantContext &ctx)
{
    LOG_F(INFO, "create family feature table at %s", name->get_string());
    const auto alreadyCreated = ctx.programFeatureTable.find(name) != ctx.programFeatureTable.end();
    const auto foundFamilyFeatureTable = ctx.programFeatureTable.find(parent) != ctx.programFeatureTable.end();
    const auto parentIsNoClass = parent->equal_string(No_class->get_string(), No_class->get_len());

    if (alreadyCreated)
    {
        LOG_F(INFO, "created by recursion started by derived class, return");
        return true;
    }
    if (parentIsNoClass || foundFamilyFeatureTable)
    {
        FeatureTable *familyFeatureTable;
        if (parentIsNoClass)
        {
            LOG_F(INFO, "parent is No_class");
            familyFeatureTable = &ctx.programFeatureTable[name];
        }
        if (foundFamilyFeatureTable)
        {
            LOG_F(INFO, "family feature table is found, inherits it");
            ctx.programFeatureTable[name] = ctx.programFeatureTable[parent];
            familyFeatureTable = &ctx.programFeatureTable[name];
        }

        {
            LOG_SCOPE_F(INFO, "register my features in %p at %s", familyFeatureTable, name->get_string());
            familyFeatureTable->enterscope();
            ctx.familyMethodTable = &familyFeatureTable->methods;
            ctx.familyAttributeTable = &familyFeatureTable->attributes;

            vector<bool> results;
            for (auto i = features->first(); features->more(i); i = features->next(i))
            {
                LOG_F(INFO, "work on feature no.%d", i);
                results.emplace_back(features->nth(i)->register_symbol(ctx));
            }
            return all_of(results.cbegin(), results.cend(), [](bool ok)
                          { return ok; });
        }
    }

    LOG_F(INFO, "family feature table not found, check family hierarchy");
    if (!check_superclass_is_defined(ctx))
    {
        report_errors(ErrorType::SuperclassIsNotDefined,
                      {{K::className, name->get_string()},
                       {K::parentName, parent->get_string()},
                       {K::lineNumber, to_string(line_number)}});
        return false;
    }
    if (!check_superclass_is_not_in_cycle(ctx))
    {
        report_errors(ErrorType::SuperclassIsInCycle,
                      {{K::className, name->get_string()},
                       {K::lineNumber, to_string(line_number)}});
        return false;
    }
    if (!check_superclass_is_not_primitives(ctx))
    {
        report_errors(ErrorType::SuperclassIsPrimitive,
                      {{K::className, name->get_string()},
                       {K::parentName, parent->get_string()},
                       {K::lineNumber, to_string(line_number)}});
        return false;
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
    LOG_F(INFO, "register symbol at %s", name->get_string());
    const auto ok = ctx.classTable.probe(name) == nullptr;
    if (ok)
    {
        LOG_F(INFO, "add new class");
        ctx.classTable.addid(name, this);
        return true;
    }
    else
    {
        LOG_F(INFO, "it is redefined");
        report_errors(ErrorType::ClassIsRedefined,
                      {{K::className, name->get_string()},
                       {K::lineNumber, to_string(line_number)}});
        return false;
    }
}

bool class__class::check_superclass_is_defined(SemantContext &ctx)
{
    const auto bad = ctx.classTable.lookup(parent) == nullptr;
    return !bad;
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

bool class__class::check_superclass_is_not_in_cycle(SemantContext &ctx)
{
    std::set<string> mark;

    const auto my_parent = ctx.classTable.probe(parent);
    if (my_parent == nullptr)
    {
        return true;
    }
    const auto bad = check_class_in_loop(ctx.classTable, *my_parent, mark);
    return !bad;
}

bool class__class::check_superclass_is_not_primitives(SemantContext &ctx)
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

    return !bad;
}

bool class__class::check_Main_has_main(SemantContext &ctx)
{
    if (name->equal_string("Main", 4))
    {
        const auto not_found = nullptr == ctx.familyMethodTable->probe(main_meth);
        return !not_found;
    }
    return true;
}

Symbol class__class::get_filename()
{
    return filename;
}

Symbol class__class::get_name()
{
    return name;
}
