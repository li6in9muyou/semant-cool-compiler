#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "loguru.h"
#include "semant-error-utility.h"
using semant_errors::ErrorType;
using semant_errors::K;
using semant_errors::report_errors;

bool formal_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at formal %s", name->get_string());
    auto ok = true;
    if (!check_type_decl_is_defined(ctx))
    {
        ok = false;
        report_errors(ErrorType::FormalTypeIsNotDefined,
                      {{K::name, name->get_string()},
                       {K::typeName, type_decl->get_string()},
                       {K::lineNumber, to_string(line_number)}});
    }
    if (!check_type_decl_is_not_self_type(ctx))
    {
        ok = false;
        report_errors(ErrorType::FormalTypeIsNotSelfType,
                      {{K::name, name->get_string()},
                       {K::lineNumber, to_string(line_number)}});
    }
    return ok;
}

bool formal_class::check_type_decl_is_defined(SemantContext &ctx)
{
    return ctx.classTable.lookup(type_decl) != nullptr;
}

bool formal_class::check_type_decl_is_not_self_type(SemantContext &ctx)
{
    return !SELF_TYPE->equal_string(type_decl->get_string(), type_decl->get_len());
}