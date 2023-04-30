#include <string>
using std::string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool block_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at block at line %d", get_line_number());
    auto ok = true;

    LOG_F(INFO, "descending into body");
    for (auto i = body->first(); body->more(i); i = body->next(i))
    {
        const auto e = (Expression)body->nth(i);
        ok &= e->semant(ctx);
        set_type(e->get_type());
    }

    return ok;
}
