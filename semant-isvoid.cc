#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool isvoid_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at isvoid");

    auto ok = true;
    ok &= e1->semant(ctx);
    set_type(Bool);
    return ok;
}