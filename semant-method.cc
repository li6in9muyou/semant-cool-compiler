#include "semant.h"

void method_class::semant(SemantContext &ctx)
{
}

void method_class::check_duplicate_method_class(SemantContext &ctx)
{
    const auto bad = ctx.methodTable.probe(name);
}