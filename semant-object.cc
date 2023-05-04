#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool object_class::semant(SemantContext &ctx)
{
    const auto undefined = [&]()
    {
        err.print(LOC + "Undeclared identifier " + name->get_string() + ".\n");
    };

    LOG_F(INFO, "semant at object id %s", name->get_string());

    if (check_symbol_exists(name, *ctx.typeEnv, undefined))
    {
        const auto idType = *ctx.typeEnv->lookup(name);
        const auto idTypeExist = check_symbol_exists(idType, ctx.classTable);
        LOG_IF_F(INFO, !idTypeExist, "id has undefined type %s", idType->get_string());
        LOG_F(INFO, "set type to %s", idType->get_string());
        set_type(idType);
        return idTypeExist;
    }
    else
    {
        LOG_F(INFO, "id not found, set type to Object");
        set_type(Object);
        return false;
    }
}