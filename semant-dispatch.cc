#include <iterator>
using std::prev;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool dispatch_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at dispatch at line %d", get_line_number());
    auto ok = true;
    ok &= expr->semant(ctx);
    const auto receiver = translate_SELF_TYPE(ctx.typeEnv, expr->get_type());
    LOG_F(INFO, "receiver has type %s", receiver->get_string());
    const auto impl = ctx.familyMethodTable->lookup(name);
    CHECK_NOTNULL_F(impl, "%s does not exist in %p", name->get_string(), ctx.familyMethodTable);

    {
        LOG_SCOPE_F(INFO, "descend into actual arguments of length %d", actual->len());
        for (auto i = actual->first(); actual->more(i); i = actual->next(i))
        {
            ok = ((Expression)actual->nth(i))->semant(ctx) && ok;
        }
    }

    LOG_F(INFO, "check actual args and formal args have same length");
    const auto implArgLen = ((int)impl->size()) - 1;
    const auto argLenOk = implArgLen == actual->len();
    if (!argLenOk)
    {
        LOG_F(INFO, "%d actual args != %d formal args", actual->len(), implArgLen);
        err.print(LOC + "Method " + name->get_string() + " called with wrong number of arguments.\n");
    }
    ok = argLenOk && ok;
    if (argLenOk && actual->len() > 0)
    {
        LOG_SCOPE_F(INFO, "check actual args type conform to formals");
        auto actualIt = actual->first();
        for (auto i = 0; i < (int)impl->size() - 1; i++)
        {
            const auto p = impl->at(i);

            const auto formalName = p.first;
            const auto formalType = p.second;
            const auto actualType = actual->nth(actualIt)->get_type();
            actualIt = actual->next(actualIt);
            ok &= check_type_conform_to(
                ctx, actualType, formalType,
                [&]()
                {
                    err.print(LOC +
                              "In call of method " +
                              name->get_string() + ", type " +
                              actualType->get_string() + " of parameter " +
                              formalName->get_string() + " does not conform to declared type " +
                              formalType->get_string() + ".\n");
                });
        }
    }
    set_type(impl->back().second);

    return ok;
}

bool static_dispatch_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at static dispatch at line %d", get_line_number());
    return true;
}