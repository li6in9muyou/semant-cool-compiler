#include <string>
using std::string;

#include "semant.h"

string error_message_attribute_redefined(const string &method_name);

void attr_class::check_not_redefined_and_register(SemantContext &ctx)
{
    const auto redefined = nullptr != ctx.attributeTable.probe(name);
    if (!redefined)
    {

        ctx.attributeTable.addid(name, this);
    }
    else
    {
        ctx.semant_error(this)
            << error_message_attribute_redefined(name->get_string())
            << "\n";
    }
}

void attr_class::check_type_decl_is_defined(SemantContext &ctx)
{
}

void attr_class::semant(SemantContext &ctx)
{
}

string error_message_attribute_redefined(const string &method_name)
{
    return "Attribute " + method_name + " is multiply defined in class.";
}