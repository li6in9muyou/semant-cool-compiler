#include <functional>
using std::function;
#include <iterator>
using std::next;
using std::prev;
#include <algorithm>
using std::mismatch;
#include "loguru.h"

#include "semant.h"
#include "semant-utility.h"
#include "cool-tree.h"

NoRepeatPrinter err = NoRepeatPrinter();

bool NoRepeatPrinter::print_once(const string &text, ostream &out)
{
    if (printed.find(text) == printed.end())
    {
        printed.insert(text);
        out << text;
        return true;
    }
    else
    {
        auto rstrip = string(text);
        while (rstrip.back() == '\n' || rstrip.back() == '\r')
        {
            rstrip.pop_back();
        }
        LOG_F(INFO, "refuse to print %s", rstrip.c_str());
        return false;
    }
}

bool NoRepeatPrinter::print(const string &text, ostream &out)
{
    out << text;
    if (printed.find(text) == printed.end())
    {
        printed.insert(text);
    }
    return true;
}

string location(const Symbol &filename, int lineno)
{
    return string(filename->get_string()) + ":" + to_string(lineno) + ": ";
}

void abort_if_not_ok(bool ok)
{
    if (!ok)
    {
        err.print("Compilation halted due to static semantic errors.\n");
        exit(1);
    }
}

bool set_type_if_ok(bool ok, Expression e, const Symbol &ok_type, const Symbol &fallback_type)
{
    if (ok)
    {
        e->set_type(ok_type);
    }
    else
    {
        LOG_F(INFO, "not ok, assumes %s", fallback_type->get_string());
        e->set_type(fallback_type);
    }

    return ok;
}

Symbol least_upper_bound(SemantContext &ctx, const Symbol &thiz, const Symbol &that)
{
    LOG_SCOPE_FUNCTION(INFO);
    CHECK_NOTNULL_F(thiz, "at least upper bound");
    CHECK_NOTNULL_F(that, "at least upper bound");
    const auto &thizFamily = ctx.familyHierarchyHash[translate_SELF_TYPE(ctx.typeEnv, thiz)];
    const auto &thatFamily = ctx.familyHierarchyHash[translate_SELF_TYPE(ctx.typeEnv, that)];
    LOG_IF_F(ERROR, thizFamily.empty(), "%s has empty family hierarchy hash", thiz->get_string());
    LOG_IF_F(ERROR, thatFamily.empty(), "%s has empty family hierarchy hash", that->get_string());
    LOG_F(INFO, "lub over %s and %s", dump_symbols(thizFamily).c_str(), dump_symbols(thatFamily).c_str());
    const auto ans = mismatch(
        thizFamily.cbegin(), thizFamily.cend(), thatFamily.cbegin(), thatFamily.cend(),
        [](const auto &a, const auto &b)
        {
            return check_symbol_eq(a, b);
        });
    return *prev(ans.first);
}

string dump_symbols(const vector<Symbol> &symbols)
{
    if (symbols.empty())
    {
        return "<empty>";
    }

    auto it = symbols.cbegin();
    string text = (*it++)->get_string();

    while (it != symbols.cend())
    {
        text.append(",");
        text.append((*it++)->get_string());
    }
    return text;
}

void noop();
bool check_symbol_eq(const Symbol &thiz, const Symbol &that, function<void()> on_error);

const Symbol &translate_SELF_TYPE(SymbolTable<Symbol, Symbol> *env, const Symbol &t)
{
    LOG_SCOPE_FUNCTION(INFO);
    if (check_symbol_eq(t, SELF_TYPE, noop))
    {
        const auto p = env->lookup(SELF_TYPE);
        CHECK_NOTNULL_F(p, "SELF_TYPE is not in env %p", env);
        LOG_F(INFO, "SELF_TYPE -> %s with %p", (*p)->get_string(), env);
        return *p;
    }
    else
    {
        return t;
    }
}