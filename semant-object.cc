#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool object_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at object id %s", name->get_string());

    const auto idExists = check_symbol_exists(name, *ctx.typeEnv);
    const auto idType = *ctx.typeEnv->lookup(name);
    const auto idTypeExists = check_symbol_exists(idType, ctx.classTable);

    LOG_IF_F(INFO, !idExists, "id does not in scope");
    LOG_IF_F(INFO, !idTypeExists, "id has undefined type %s", idType->get_string());

    return set_type_if_ok(idExists && idTypeExists, this, idType, Object);
}