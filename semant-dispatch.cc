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
    LOG_F(INFO, "check actual args");
    auto ok = true;
    {
        LOG_SCOPE_F(INFO, "descend to %d actual args", actual->len());
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

const auto *method_definition(SemantContext &ctx, const Symbol &type, const Symbol &method)
{
    const auto &T = translate_SELF_TYPE(ctx.typeEnv, type);
    LOG_F(INFO, "at method definition of %s::%s", T->get_string(), method->get_string());
    auto familyMethods = ctx.programFeatureTable.at(T).methods;
    LOG_F(INFO, "search in %p", &familyMethods);
    const auto impl = familyMethods.lookup(method);
    LOG_IF_F(INFO, impl == nullptr, "%s::%s not found", T->get_string(), method->get_string());
    return impl;
}

bool check_dispatch_method_to_type(
    SemantContext &ctx,
    const Symbol &type,
    const Symbol &name,
    Expressions &actual,
    function<void()> on_bad_type,
    function<void()> on_bad_method,
    function<void()> on_bad_arg_len,
    function<void(const Symbol &formalName, const Symbol &formalType, const Symbol &actualType)> on_bad_args)
{
    const auto &T = translate_SELF_TYPE(ctx.typeEnv, type);
    LOG_SCOPE_F(INFO, "check dispatch method %s::%s", T->get_string(), name->get_string());
    auto ok = true;

    LOG_F(INFO, "check type exists");
    ok &= ctx.programFeatureTable.find(T) != ctx.programFeatureTable.end();

    if (ok)
    {
        const auto *impl = method_definition(ctx, T, name);
        ok &= impl != nullptr;

        if (ok)
        {
            LOG_F(INFO, "method found");
            ok &= check_actual_args(ctx, *impl, actual, on_bad_arg_len, on_bad_args);
            return ok;
        }
        else
        {
            on_bad_method();
            return ok;
        }
    }
    else
    {
        on_bad_type();
        return ok;
    }
}

bool dispatch_class::semant(SemantContext &ctx)
{

    const auto &bad_method = [&]()
    {
        err.print(LOC + "Dispatch to undefined method " + name->get_string() + ".\n");
    };
    const auto &bad_type = [&](const auto &T)
    {
        return [&]()
        {
            err.print(LOC + "Dispatch on undefined class " + T->get_string() + ".\n");
        };
    };
    const auto &bad_cast = [&](const auto &T)
    {
        return [&]()
        {
            err.print(LOC + "Expression type " +
                      T->get_string() + " does not conform to declared static dispatch type " +
                      type_name->get_string() + ".\n");
        };
    };
    const auto &bad_arg_len = [&]()
    {
        err.print(LOC + "Method " + name->get_string() + " called with wrong number of arguments.\n");
    };
    const auto &bad_args = [&](const Symbol &formalName, const Symbol &formalType, const Symbol &actualType)
    {
        err.print(LOC +
                  "In call of method " +
                  name->get_string() + ", type " +
                  actualType->get_string() + " of parameter " +
                  formalName->get_string() + " does not conform to declared type " +
                  formalType->get_string() + ".\n");
    };

    LOG_F(INFO, "semant at dispatch at line %d", get_line_number());
    auto ok = true;
    ok &= expr->semant(ctx);
    const auto exprT = expr->get_type();
    ok &= check_dispatch_method_to_type(ctx, exprT, name, actual, bad_type(exprT), bad_method, bad_arg_len, bad_args);
    if (ok)
    {
        set_type(method_definition(ctx, exprT, name)->back().second);
    }
    else
    {
        set_type(Object);
    }
    return ok;
}

bool static_dispatch_class::semant(SemantContext &ctx)
{
    const auto &reject_SELF_TYPE = [&]()
    {
        err.print(LOC + "Static dispatch to SELF_TYPE.\n");
    };
    const auto &bad_type = [&]()
    {
        err.print(LOC + "Static dispatch to undefined class " + type_name->get_string() + ".\n");
    };
    const auto &bad_cast = [&](const auto &T)
    {
        return [&]()
        {
            err.print(LOC + "Expression type " +
                      T->get_string() + " does not conform to declared static dispatch type " +
                      type_name->get_string() + ".\n");
        };
    };
    const auto &bad_method = [&]()
    {
        err.print(LOC + "Static dispatch to undefined method " + name->get_string() + ".\n");
    };
    const auto &bad_arg_len = [&]()
    {
        err.print(LOC + "Method " + name->get_string() + " invoked with wrong number of arguments.\n");
    };
    const auto &bad_args = [&](const Symbol &formalName, const Symbol &formalType, const Symbol &actualType)
    {
        err.print(LOC +
                  "In call of method " +
                  name->get_string() + ", type " +
                  actualType->get_string() + " of parameter " +
                  formalName->get_string() + " does not conform to declared type " +
                  formalType->get_string() + ".\n");
    };

    LOG_F(INFO, "semant at static dispatch at line %d", get_line_number());
    auto ok = true;
    ok &= check_symbol_not_eq(type_name, SELF_TYPE, reject_SELF_TYPE);
    ok &= check_symbol_exists(type_name, ctx.classTable, bad_type);
    if (ok)
    {
        ok &= expr->semant(ctx);
        const auto exprT = expr->get_type();
        ok &= check_type_conform_to(ctx, exprT, type_name, bad_cast(exprT));
        ok &= ok && check_dispatch_method_to_type(ctx, type_name, name, actual, bad_type, bad_method, bad_arg_len, bad_args);
        if (ok)
        {
            set_type(method_definition(ctx, exprT, name)->back().second);
        }
        else
        {
            set_type(Object);
        }
    }
    return ok;
}