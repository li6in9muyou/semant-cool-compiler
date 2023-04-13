#include <string>
using std::string;

#include "semant.h"
#include "loguru.h"

string error_message_method_redefined(const string &method_name);

void method_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_FUNCTION(INFO);
}

void method_class::check_not_redefined_and_register(SemantContext &ctx)
{
    LOG_F(INFO, "method declration check");
    const auto redefined = nullptr != ctx.familyMethodTable->probe(name);
    if (!redefined)
    {
        ctx.familyMethodTable->addid(name, this);
    }
    else
    {
        ctx.semant_error(this)
            << error_message_method_redefined(name->get_string())
            << "\n";
    }
}

string error_message_method_redefined(const string &method_name)
{
    return "Method " + method_name + " is multiply defined.";
}