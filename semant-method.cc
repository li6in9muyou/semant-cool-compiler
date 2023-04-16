#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "loguru.h"
#include "semant-error-utility.h"
using semant_errors::ErrorType;
using semant_errors::K;
using semant_errors::report_errors;

string error_message_method_redefined(const string &method_name);

void method_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_FUNCTION(INFO);
}

void method_class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "method register symbol at %s", name->get_string());
    CHECK_NOTNULL_F(ctx.familyMethodTable, "ctx.familyMethodTable is nullptr");
    const auto redefined = nullptr != ctx.familyMethodTable->probe(name);
    if (!redefined)
    {
        LOG_IF_F(INFO, nullptr != ctx.familyMethodTable->lookup(name), "this method overrides");
        ctx.familyMethodTable->addid(name, this);
    }
    else
    {
        LOG_F(INFO, "redefinition check failed, add error");
        report_errors(ErrorType::MethodRedefined,
                      {{K::methodName, name->get_string()},
                       {K::lineNumber, to_string(line_number)}});
    }
}

string error_message_method_redefined(const string &method_name)
{
    return "Method " + method_name + " is multiply defined.";
}