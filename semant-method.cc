#include <string>
using std::string;

#include "semant.h"

string error_message_method_redefined(const string &method_name);

void method_class::semant(SemantContext &ctx)
{
}

void method_class::check_not_redefined_and_register(SemantContext &ctx)
{
    const auto redefined = nullptr != ctx.familyMethodTable->lookup(name);
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