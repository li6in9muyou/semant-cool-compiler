#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

[[nodiscard]] bool attr_class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "attribute register symbol at attribute %s", name->get_string());

    CHECK_NOTNULL_F(ctx.familyAttributeTable, "ctx.familyAttributeTable is nullptr");

    auto ok = true;

    ok &= check_symbol_not_exists_in_current_scope(
        name,
        *ctx.familyAttributeTable,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Attribute " + name->get_string() + " is multiply defined in class.\n");
        });
    if (!ok)
    {
        LOG_F(INFO, "do not addid at %s", name->get_string());
        return ok;
    }

    ok &= check_symbol_not_exists(
        name,
        *ctx.familyAttributeTable,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Attribute " + name->get_string() + " is an attribute of an inherited class.\n");
        });
    if (!ok)
    {
        LOG_F(INFO, "do not addid at %s", name->get_string());
        return ok;
    }

    ctx.familyAttributeTable->addid(name, this);
    return ok;
}

bool attr_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at attribute %s", name->get_string());

    auto ok = true;

    ok &= check_symbol_exists(
        type_decl,
        ctx.classTable,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Class " + type_decl->get_string() + " of attribute " + name->get_string() + " is undefined.\n");
        });

    return ok;
}
