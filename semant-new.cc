#include <string>
using std::string;

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool new__class::semant(SemantContext &ctx)
{
    const auto &undefined_type = [&]()
    {
        err.print(LOC + "'new' used with undefined class " +
                  type_name->get_string() + ".\n");
    };

    LOG_F(INFO, "semant at 'new %s'", type_name->get_string());
    auto ok = true;

    ok &= check_symbol_eq(type_name, SELF_TYPE) ||
          check_symbol_exists(type_name, ctx.classTable, undefined_type);
    set_type_if_ok(ok, this, type_name, Object);
    return ok;
}
