#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool loop_class::semant(SemantContext &ctx)
{
    const auto not_bool = [&]()
    {
        err.print(LOC + "Loop condition does not have type Bool.\n");
    };

    LOG_F(INFO, "semant at loop at line %d", get_line_number());
    auto ok = true;

    ok &= pred->semant(ctx);
    ok &= check_symbol_eq(Bool, pred->get_type(), not_bool);
    ok &= body->semant(ctx);
    set_type(Object);

    return ok;
}