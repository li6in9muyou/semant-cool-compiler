#include <string>
using std::string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool typcase_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at type case at line %d", get_line_number());
    auto ok = true;

    LOG_F(INFO, "descending into cases");
    for (auto i = cases->first(); cases->more(i); i = cases->next(i))
    {
        ok = ((branch_class *)cases->nth(i))->semant(ctx) && ok;
    }

    return ok;
}

bool branch_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at branch at line %d", get_line_number());
    auto ok = true;

    ok &= check_symbol_exists(
        type_decl,
        ctx.classTable,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Class " + type_decl->get_string() + " of case branch is undefined.\n");
        });

    LOG_F(INFO, "descending into branch expression");
    ok &= expr->semant(ctx);

    return ok;
}
