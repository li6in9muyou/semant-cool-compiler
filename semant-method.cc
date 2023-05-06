#include <vector>
using std::vector;
#include <string>
using std::string;
using std::to_string;

#include "stringtab.h"

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool method_class::semant(SemantContext &ctx)
{
    const auto &type_mismatch = [&]()
    {
        err.print(LOC + "Inferred return type " +
                  expr->get_type()->get_string() + " of method " +
                  name->get_string() + " does not conform to declared return type " +
                  return_type->get_string() + ".\n");
    };
    const auto &bad_return_type = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) +
                  "Undefined return type " + return_type->get_string() + " in method " + name->get_string() + ".\n");
    };

    LOG_SCOPE_F(INFO, "semant at method %s", name->get_string());
    auto ok = true;

    LOG_F(INFO, "enterscope on %p", ctx.typeEnv);
    ctx.typeEnv->enterscope();

    {
        LOG_SCOPE_F(INFO, "prepare formals in %p", ctx.typeEnv);

        LOG_IF_F(INFO, formals->len() == 0, "no formals");
        for (auto i = formals->first(); formals->more(i); i = formals->next(i))
        {
            ok = ((formal_class *)formals->nth(i))->register_symbol(ctx) && ok;
        }
        for (auto i = formals->first(); formals->more(i); i = formals->next(i))
        {
            ok = ((formal_class *)formals->nth(i))->semant(ctx) && ok;
        }
    }

    {
        LOG_SCOPE_F(INFO, "descend into body");
        ok = expr->semant(ctx) && ok;
    }

    const auto returnTypeOk = check_symbol_exists(return_type, ctx.classTable, bad_return_type);
    LOG_IF_F(INFO, !returnTypeOk, "declared return type %s does not exist", return_type->get_string());

    LOG_IF_F(INFO, returnTypeOk, "check body type %s conform to declared type", expr->get_type()->get_string());
    ok = returnTypeOk &&
         check_type_conform_to(ctx, expr->get_type(), return_type, type_mismatch) &&
         ok;

    LOG_F(INFO, "exitscope on %p", ctx.typeEnv);
    ctx.typeEnv->exitscope();
    return ok;
}

bool method_class::register_symbol(SemantContext &ctx)
{
    const auto &duplicate = [&]()
    {
        LOG_F(INFO, "redefinition check failed, add error");
        err.print(LOC + "Method " +
                  name->get_string() + " is multiply defined.\n");
    };

    LOG_SCOPE_F(INFO, "register symbol at method %s", name->get_string());
    CHECK_NOTNULL_F(ctx.familyMethodTable, "ctx.familyMethodTable is nullptr");

    auto ok = check_symbol_not_exists_in_current_scope(name, *ctx.familyMethodTable, duplicate);

    LOG_IF_F(INFO, nullptr != ctx.familyMethodTable->lookup(name), "this method overrides");
    if (ok)
    {
        auto *impl = new vector<pair<Symbol, Symbol>>();
        for (auto i = formals->first(); formals->more(i); i = formals->next(i))
        {
            impl->push_back(((formal_class *)formals->nth(i))->get_definition());
        }
        impl->push_back({method_return_type, return_type});
        ctx.familyMethodTable->addid(name, impl);
        LOG_F(INFO, "[%s] impl has length %zu, added to %p",
              name->get_string(), impl->size(), ctx.familyMethodTable);
    }

    return ok;
}
