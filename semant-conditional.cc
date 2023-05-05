#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool cond_class::semant(SemantContext &ctx)
{
    const auto not_bool = [&]()
    {
        err.print(LOC + "Predicate of 'if' does not have type Bool.\n");
    };

    LOG_F(INFO, "semant at cond at line %d", get_line_number());
    auto ok = true;

    ok &= pred->semant(ctx);
    {
        LOG_SCOPE_F(INFO, "semant cond branches");
        ok &= then_exp->semant(ctx);
        ok &= else_exp->semant(ctx);
    }

    ok &= check_symbol_eq(Bool, pred->get_type(), not_bool);
    set_type(least_upper_bound(ctx, then_exp->get_type(), else_exp->get_type()));

    return ok;
}