#include "semant.h"
#include "loguru.h"

bool no_expr_class::semant(SemantContext &)
{
    LOG_F(WARNING, "semant at no expr, set type to no type and return true");
    set_type(No_type);
    return true;
}