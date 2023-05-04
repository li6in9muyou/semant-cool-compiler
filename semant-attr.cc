#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool attr_class::register_symbol(SemantContext &ctx)
{
    const auto &duplicate = [&]()
    {
        err.print(LOC + "Attribute " +
                  name->get_string() + " is multiply defined in class.\n");
    };
    const auto &shadow = [&]()
    {
        err.print(LOC + "Attribute " +
                  name->get_string() + " is an attribute of an inherited class.\n");
    };

    LOG_F(INFO, "attribute register symbol at attribute %s", name->get_string());
    CHECK_NOTNULL_F(ctx.typeEnv, "ctx.typeEnv is nullptr");
    auto ok = true;

    ok &= ok && check_symbol_not_exists_in_current_scope(name, *ctx.typeEnv, duplicate);
    ok &= ok && check_symbol_not_exists(name, *ctx.typeEnv, shadow);
    if (ok)
    {
        LOG_F(INFO, "add %s to %p", name->get_string(), ctx.typeEnv);
        ctx.typeEnv->addid(name, &this->type_decl);
    }
    else
    {
        LOG_F(INFO, "not ok, do nothing");
    }
    return ok;
}

bool attr_class::semant(SemantContext &ctx)
{
    const auto &bad_type = [&]()
    {
        err.print(LOC + "Inferred type " +
                  init->get_type()->get_string() + " of initialization of attribute " +
                  name->get_string() + " does not conform to declared type " +
                  type_decl->get_string() + ".\n");
    };
    const auto &undefined_type = [&]()
    {
        err.print(LOC + "Class " +
                  type_decl->get_string() + " of attribute " +
                  name->get_string() + " is undefined.\n");
    };

    LOG_F(INFO, "semant at attribute %s", name->get_string());
    auto ok = true;

    ok &= check_symbol_eq(type_decl, prim_slot) ||
          check_symbol_eq(type_decl, SELF_TYPE) ||
          check_symbol_exists(type_decl, ctx.classTable, undefined_type);

    ok &= init->semant(ctx);
    if (!check_symbol_eq(No_type, init->get_type()))
    {
        ok &= ok && check_type_conform_to(ctx, init->get_type(), type_decl, bad_type);
    }

    return ok;
}
