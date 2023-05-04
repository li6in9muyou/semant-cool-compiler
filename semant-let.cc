#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool let_class::semant(SemantContext &ctx)
{
    const auto &undefined_type = [&]()
    {
        err.print(LOC + "Class " +
                  type_decl->get_string() + " of let-bound identifier " +
                  identifier->get_string() + " is undefined.\n");
    };
    const auto &bad_type = [&](const auto &T)
    {
        return [&]()
        {
            err.print(LOC + "Inferred type " +
                      T.get_string() + " of initialization of " +
                      identifier->get_string() + " does not conform to identifier's declared type " +
                      type_decl->get_string() + ".\n");
        };
    };

    LOG_F(INFO, "semant at let");

    auto ok = true;
    ctx.typeEnv->enterscope();
    {
        LOG_SCOPE_F(INFO, "semant at let at line %d", get_line_number());

        ok &= check_symbol_exists(type_decl, ctx.classTable, undefined_type);
        ctx.typeEnv->addid(identifier, &type_decl);

        ok &= init->semant(ctx);
        const auto initType = init->get_type();
        CHECK_NOTNULL_F(initType, "init expression type is nullptr");
        LOG_F(INFO, "init has type %s", initType->get_string());
        const auto noInitExpr = check_symbol_eq(initType, No_type);
        LOG_IF_F(INFO, noInitExpr, "no init expression");
        ok &= noInitExpr || check_type_conform_to(ctx, init->get_type(), type_decl, bad_type(*init->get_type()));

        ok &= body->semant(ctx);
        LOG_F(INFO, "body has type %s", body->get_type()->get_string());
    }
    ctx.typeEnv->exitscope();
    set_type_if_ok(ok, this, body->get_type(), Object);
    return ok;
}
