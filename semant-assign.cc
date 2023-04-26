#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool assign_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at '<-' at line %d", get_line_number());
    return Expression_class::semant(ctx);
}