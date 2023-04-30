#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool method_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at method %s", name->get_string());
    auto ok = true;

    LOG_F(INFO, "enter scope on %p", ctx.typeEnv);
    ctx.typeEnv->enterscope();

    {
        LOG_SCOPE_F(INFO, "descend into formals");
        LOG_IF_F(INFO, formals->len() == 0, "no formals");
        for (auto i = formals->first(); formals->more(i); i = formals->next(i))
        {
            ok = ((formal_class *)formals->nth(i))->semant(ctx) && ok;
        }
    }

    {
        LOG_SCOPE_F(INFO, "descend into expression");
        ok = expr->semant(ctx) && ok;

        const auto returnTypeOk = check_symbol_exists(
            return_type, ctx.classTable,
            [&]()
            {
                err.print(location(ctx.filename, get_line_number()) +
                          "Undefined return type " + return_type->get_string() + " in method " + name->get_string() + ".\n");
            });
        LOG_IF_F(INFO, !returnTypeOk, "return type %s does no exist", return_type->get_string());
        ok = returnTypeOk &&
             check_type_conform_to(
                 ctx, expr->get_type(), return_type,
                 [&]()
                 {
                     err.print(location(ctx.filename, get_line_number()) +
                               "Inferred return type " + expr->get_type()->get_string() +
                               " of method " + name->get_string() +
                               " does not conform to declared return type " +
                               return_type->get_string() + ".\n");
                 }) &&
             ok;
    }
    ctx.typeEnv->exitscope();
    return ok;
}

bool method_class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "register symbol at method %s", name->get_string());
    CHECK_NOTNULL_F(ctx.familyMethodTable, "ctx.familyMethodTable is nullptr");

    const auto ok = check_symbol_not_exists_in_current_scope(
        name,
        *ctx.familyMethodTable,
        [&]()
        {
            LOG_F(INFO, "redefinition check failed, add error");
            err.print(location(ctx.filename, get_line_number()) +
                      "Method " + name->get_string() + " is multiply defined.\n");
        });

    if (ok)
    {
        LOG_IF_F(INFO, nullptr != ctx.familyMethodTable->lookup(name), "this method overrides");
        ctx.familyMethodTable->addid(name, this);
    }

    return ok;
}
