#include <functional>
using std::function;
#include <iterator>
using std::prev;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool check_actual_args(
    SemantContext &ctx, const std::vector<std::pair<Symbol, Symbol>> &impl, Expressions actual,
    function<void()> on_bad_arg_len,
    function<void(const Symbol &formalName, const Symbol &formalType, const Symbol &actualType)> on_bad_type)
{
    LOG_F(INFO, "at check actual args");
    auto ok = true;
    {
        LOG_SCOPE_F(INFO, "descend into actual arguments of length %d", actual->len());
        for (auto i = actual->first(); actual->more(i); i = actual->next(i))
        {
            ok = ((Expression)actual->nth(i))->semant(ctx) && ok;
        }
    }

    LOG_F(INFO, "check actual args and formal args have same length");
    const auto implArgLen = ((int)impl.size()) - 1;
    const auto argLenOk = implArgLen == actual->len();
    if (!argLenOk)
    {
        LOG_F(INFO, "%d actual args != %d formal args", actual->len(), implArgLen);
        on_bad_arg_len();
    }
    ok = argLenOk && ok;
    if (argLenOk && actual->len() > 0)
    {
        LOG_SCOPE_F(INFO, "check actual args type conform to formals");
        auto actualIt = actual->first();
        for (auto i = 0; i < (int)impl.size() - 1; i++)
        {
            const auto p = impl.at(i);

            const auto formalName = p.first;
            const auto formalType = p.second;
            const auto actualType = actual->nth(actualIt)->get_type();
            actualIt = actual->next(actualIt);
            ok &= check_type_conform_to(
                ctx, actualType, formalType,
                [&]()
                {
                    on_bad_type(formalName, formalType, actualType);
                });
        }
    }
    return ok;
}

const auto &find_impl_in(
    SymbolTable<Symbol, std::vector<std::pair<Symbol, Symbol>>> &familyMethods,
    Symbol method_name)
{
    LOG_F(INFO, "at find impl of [%s] in %p", method_name->get_string(), &familyMethods);
    const auto impl = familyMethods.lookup(method_name);
    CHECK_NOTNULL_F(impl, "not found");
    return *impl;
}

bool dispatch_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at dispatch to [%s] at line %d", name->get_string(), get_line_number());
    auto ok = true;

    ok &= expr->semant(ctx);
    const auto receiverType = translate_SELF_TYPE(ctx.typeEnv, expr->get_type());
    LOG_F(INFO, "receiver has type %s", receiverType->get_string());

    CHECK_F(ctx.programFeatureTable.find(receiverType) != ctx.programFeatureTable.end(),
            "receiverType does not exist");
    auto &receiverFamilyFeatures = ctx.programFeatureTable.at(receiverType).methods;
    const auto methodOk = check_symbol_exists(
        name, receiverFamilyFeatures,
        [&]()
        {
            err.print(LOC + "Dispatch to undefined method " + name->get_string() + ".\n");
        });
    ok &= methodOk;
    if (!methodOk)
    {
        LOG_F(INFO, "method is undefined, return");
        set_type(Object);
    }
    else
    {
        LOG_F(INFO, "method found");
        const auto &impl = find_impl_in(receiverFamilyFeatures, name);
        {
            LOG_SCOPE_F(INFO, "descend into actual arguments of length %d", actual->len());
            for (auto i = actual->first(); actual->more(i); i = actual->next(i))
            {
                ok = ((Expression)actual->nth(i))->semant(ctx) && ok;
            }
        }
        ok &= check_actual_args(
            ctx, impl, actual,
            [&]()
            {
                err.print(LOC + "Method " + name->get_string() + " called with wrong number of arguments.\n");
            },
            [&](const Symbol &formalName, const Symbol &formalType, const Symbol &actualType)
            {
                err.print(LOC +
                          "In call of method " +
                          name->get_string() + ", type " +
                          actualType->get_string() + " of parameter " +
                          formalName->get_string() + " does not conform to declared type " +
                          formalType->get_string() + ".\n");
            });
        set_type_if_ok(ok, this, impl.back().second, Object);
    }
    return ok;
}

bool static_dispatch_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at static dispatch at line %d", get_line_number());
    auto ok = true;
    ok &= check_symbol_not_eq(
        type_name, SELF_TYPE,
        [&]()
        {
            err.print(LOC + "Static dispatch to SELF_TYPE.\n");
        });
    ok &= check_symbol_exists(
        type_name, ctx.classTable,
        [&]()
        {
            err.print(LOC + "Static dispatch to undefined class " + type_name->get_string() + ".\n");
        });
    if (ok)
    {
        ok &= expr->semant(ctx);
        ok &= check_type_conform_to(
            ctx, expr->get_type(), type_name,
            [&]()
            {
                err.print(LOC + "Expression type " +
                          expr->get_type()->get_string() + " does not conform to declared static dispatch type " +
                          type_name->get_string() + ".\n");
            });
        auto &receiverFamilyFeatures = ctx.programFeatureTable.at(type_name).methods;
        ok &= check_symbol_exists(
            name, receiverFamilyFeatures,
            [&]()
            {
                err.print(LOC + "Static dispatch to undefined method " + name->get_string() + ".\n");
            });
        if (ok)
        {
            const auto impl = receiverFamilyFeatures.lookup(name);
            ok &= check_actual_args(
                ctx, *impl, actual,
                [&]()
                {
                    err.print(LOC + "Method " + name->get_string() + " invoked with wrong number of arguments.\n");
                },
                [&](const Symbol &formalName, const Symbol &formalType, const Symbol &actualType)
                {
                    err.print(LOC +
                              "In call of method " +
                              name->get_string() + ", type " +
                              actualType->get_string() + " of parameter " +
                              formalName->get_string() + " does not conform to declared type " +
                              formalType->get_string() + ".\n");
                });
            set_type(impl->back().second);
        }
        else
        {
            set_type(Object);
        }
    }
    return ok;
}