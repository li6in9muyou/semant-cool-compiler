#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool assign_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at '<-' at line %d", get_line_number());

    auto ok = true;

    ok &= check_symbol_not_eq(
        name, self,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Cannot assign to 'self'.\n");
        });

    ok &= expr->semant(ctx);
    if (check_symbol_eq(No_type, expr->get_type()))
    {
        LOG_F(INFO, "rhs is a No_type, skip type conform check");
        return ok;
    }

    const auto type_decl = *ctx.typeEnv->lookup(name);
    ok &= set_type_if_ok(
        check_type_conform_to(
            ctx, expr->get_type(), type_decl,
            [&]()
            {
                err.print(location(ctx.filename, get_line_number()) +
                          "Type " + expr->get_type()->get_string() +
                          " of assigned expression does not conform to declared type " + type_decl->get_string() +
                          " of identifier " + name->get_string() + ".\n");
            }),
        this, type_decl, Object);

    return ok;
}