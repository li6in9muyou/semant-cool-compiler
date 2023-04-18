#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool method_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at method %s", name->get_string());
    auto ok = true;

    ok &= check_symbol_exists(
        return_type,
        ctx.classTable,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Undefined return type " + return_type->get_string() + " in method " + name->get_string() + ".\n");
        });

    LOG_F(INFO, "descending into formals");
    for (auto i = formals->first(); formals->more(i); i = formals->next(i))
    {
        ok = ((formal_class *)formals->nth(i))->semant(ctx) && ok;
    }
    return ok;
}

bool method_class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "register symbol at method %s", name->get_string());
    CHECK_NOTNULL_F(ctx.familyMethodTable, "ctx.familyMethodTable is nullptr");

    const auto ok = check_symbol_not_exists_in_current_scope(
        name,
        *ctx.familyMethodTable,
        [&]()
        {
            LOG_F(INFO, "redefinition check failed, add error");
            err.print(location(ctx.filename, get_line_number()) +
                      "Method " + name->get_string() + " is multiply defined.\n");
        });

    if (ok)
    {
        LOG_IF_F(INFO, nullptr != ctx.familyMethodTable->lookup(name), "this method overrides");
        ctx.familyMethodTable->addid(name, this);
    }

    return ok;
}
