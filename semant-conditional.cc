#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool cond_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at cond at line %d", get_line_number());
    const auto not_bool = [&]()
    {
        err.print(LOC + "Predicate of 'if' does not have type Bool.\n");
    };

    auto ok = true;

    ok &= pred->semant(ctx);
    ok &= then_exp->semant(ctx);
    ok &= else_exp->semant(ctx);

    ok &= check_symbol_eq(Bool, pred->get_type(), not_bool);
    const auto thenExprT = then_exp->get_type();
    const auto elseExprT = else_exp->get_type();

    set_type(least_upper_bound(ctx, thenExprT, elseExprT));

    return ok;
}