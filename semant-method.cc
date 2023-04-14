#include <string>
using std::string;

#include "semant.h"
#include "loguru.h"

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
        ctx.semant_error(this)
            << error_message_method_redefined(name->get_string())
            << "\n";
    }
}

string error_message_method_redefined(const string &method_name)
{
    return "Method " + method_name + " is multiply defined.";
}