#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "loguru.h"
#include "semant-error-utility.h"
using semant_errors::ErrorType;
using semant_errors::K;
using semant_errors::report_errors;

bool method_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at method %s", name->get_string());
    auto ok = true;
    ok = check_return_type_is_defined(ctx) && ok;
    if (!ok)
    {
        report_errors(ErrorType::MethodReturnTypeIsNotDefined,
                      {{K::methodName, name->get_string()},
                       {K::typeName, return_type->get_string()},
                       {K::lineNumber, to_string(line_number)}});
    }
    LOG_F(INFO, "descending into formals");
    for (auto i = formals->first(); formals->more(i); i = formals->next(i))
    {
        ok = ((formal_class *)formals->nth(i))->semant(ctx) && ok;
    }
    return ok;
}

bool method_class::check_return_type_is_defined(SemantContext &ctx)
{
    return ctx.classTable.lookup(return_type) != nullptr;
}

bool method_class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "method register symbol at %s", name->get_string());
    CHECK_NOTNULL_F(ctx.familyMethodTable, "ctx.familyMethodTable is nullptr");
    const auto redefined = nullptr != ctx.familyMethodTable->probe(name);
    if (!redefined)
    {
        LOG_IF_F(INFO, nullptr != ctx.familyMethodTable->lookup(name), "this method overrides");
        ctx.familyMethodTable->addid(name, this);
        return true;
    }
    else
    {
        LOG_F(INFO, "redefinition check failed, add error");
        report_errors(ErrorType::MethodRedefined,
                      {{K::methodName, name->get_string()},
                       {K::lineNumber, to_string(line_number)}});
        return false;
    }
}
