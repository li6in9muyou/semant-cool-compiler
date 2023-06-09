#include <string>
using std::string;
#include <utility>
using std::pair;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool formal_class::semant(SemantContext &ctx)
{
    const auto &bad_type = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) +
                  "Class " + type_decl->get_string() + " of formal parameter " + name->get_string() + " is undefined.\n");
    };
    const auto &no_SELF_TYPE = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) +
                  "Formal parameter " + name->get_string() + " cannot have type SELF_TYPE.\n");
    };

    LOG_F(INFO, "semant at formal %s", name->get_string());
    auto ok = true;

    ok &= check_symbol_exists(type_decl, ctx.classTable, bad_type);
    ok &= check_symbol_not_eq(type_decl, SELF_TYPE, no_SELF_TYPE);

    return ok;
}

bool formal_class::register_symbol(SemantContext &ctx)
{
    const auto &duplicate = [&]()
    {
        err.print(LOC + "Formal parameter " + name->get_string() + " is multiply defined.\n");
    };

    LOG_F(INFO, "register symbol at formal %s using scope %p", name->get_string(), ctx.typeEnv);
    const auto ok = check_symbol_not_exists_in_current_scope(name, *ctx.typeEnv, duplicate);
    if (ok)
    {
        ctx.typeEnv->addid(name, &type_decl);
    }
    return ok;
}

pair<Symbol, Symbol> formal_class::get_definition() const
{
    return {copy_Symbol(name), copy_Symbol(type_decl)};
}