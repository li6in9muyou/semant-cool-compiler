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

    ctx.typeEnv->enterscope();
    {
        LOG_SCOPE_F(INFO, "descending into cases");
        Symbol T = nullptr;
        for (auto i = cases->first(); cases->more(i); i = cases->next(i))
        {
            auto br = ((branch_class *)cases->nth(i));
            ok = br->semant(ctx) && ok;
            if (T == nullptr)
            {
                T = br->get_expr_type();
            }
            else
            {
                T = least_upper_bound(ctx, br->get_expr_type(), T);
            }
        }
        set_type(T);
    }
    ctx.typeEnv->exitscope();

    return ok;
}

bool branch_class::semant(SemantContext &ctx)
{
    const auto &bad_type = [&]()
    {
        err.print(LOC + "Class " +
                  type_decl->get_string() + " of case branch is undefined.\n");
    };

    LOG_SCOPE_F(INFO, "semant at branch at line %d", get_line_number());
    auto ok = true;

    ok &= check_symbol_exists(type_decl, ctx.classTable, bad_type);

    LOG_F(INFO, "descending into branch expression");
    ctx.typeEnv->enterscope();
    ctx.typeEnv->addid(name, &type_decl);
    ok &= expr->semant(ctx);
    type = expr->get_type();
    LOG_F(INFO, "branch body has type %s", type->get_string());
    ctx.typeEnv->exitscope();

    return ok;
}

Symbol branch_class::get_expr_type()
{
    return type;
}
