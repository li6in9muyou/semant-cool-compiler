#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool assign_class::semant(SemantContext &ctx)
{
    const auto &cant_assign_self = [&]()
    {
        err.print(LOC + "Cannot assign to 'self'.\n");
    };
    const auto &var_undefined = [&]()
    {
        err.print(LOC + "Assignment to undeclared variable " +
                  name->get_string() + ".\n");
    };
    const auto &bad_type = [&](const auto &idType)
    {
        return [&]()
        {
            err.print(LOC + "Type " +
                      expr->get_type()->get_string() + " of assigned expression does not conform to declared type " +
                      idType->get_string() + " of identifier " +
                      name->get_string() + ".\n");
        };
    };

    LOG_F(INFO, "semant at '<-' at line %d", get_line_number());

    auto ok = true;

    ok &= check_symbol_not_eq(name, self, cant_assign_self);

    const auto exprOk = expr->semant(ctx);
    ok &= exprOk;
    if (check_symbol_eq(No_type, expr->get_type()))
    {
        LOG_F(INFO, "rhs is a No_type, skip type conform check");
        return ok;
    }
    const auto exprType = expr->get_type();
    const auto exprTypeExists = check_symbol_exists(exprType, ctx.classTable);
    LOG_IF_F(INFO, !exprTypeExists, "expr type %s does not exist, skip type conform check", exprType->get_string());
    set_type_if_ok(exprTypeExists, this, expr->get_type(), Object);

    const auto idExists = check_symbol_exists(name, *ctx.typeEnv, var_undefined);
    if (idExists)
    {
        const auto idType = *ctx.typeEnv->lookup(name);
        const auto idTypeExists = check_symbol_exists(idType, ctx.classTable);
        LOG_IF_F(INFO, !idTypeExists, "id type %s does not exist, skip type conform check", idType->get_string());
        ok &= idTypeExists && idExists && check_type_conform_to(ctx, exprType, idType, bad_type(idType));
    }
    return ok;
}