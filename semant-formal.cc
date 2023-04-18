#include <string>
using std::string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool formal_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at formal %s", name->get_string());
    auto ok = true;

    ok &= check_symbol_exists(
        type_decl,
        ctx.classTable,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Class " + type_decl->get_string() + " of formal parameter " + name->get_string() + " is undefined.\n");
        });
    ok &= check_symbol_is_not(
        type_decl,
        SELF_TYPE,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Formal parameter " + name->get_string() + " cannot have type SELF_TYPE.\n");
        });

    return ok;
}
