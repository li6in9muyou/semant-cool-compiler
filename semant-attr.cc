#include <string>
using std::string;

#include "semant.h"

string error_message_attribute_redefined(const string &method_name);
string error_message_attribute_type_is_not_defined(const string &attribute_name, const string type_name);

void attr_class::check_not_redefined_and_register(SemantContext &ctx)
{
    const auto redefined = nullptr != ctx.familyAttributeTable->lookup(name);
    if (!redefined)
    {
        ctx.familyAttributeTable->addid(name, this);
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
    const auto bad = nullptr == ctx.classTable.probe(this->type_decl);
    if (bad)
    {
        ctx.semant_error(this)
            << error_message_attribute_type_is_not_defined(name->get_string(), type_decl->get_string())
            << "\n";
    }
}

void attr_class::check_no_shadow_attribute_in_superclass(SemantContext &ctx)
{
}

void attr_class::semant(SemantContext &ctx)
{
    check_type_decl_is_defined(ctx);
    check_no_shadow_attribute_in_superclass(ctx);
}

string error_message_attribute_redefined(const string &method_name)
{
    return "Attribute " + method_name + " is multiply defined in class.";
}

string error_message_attribute_type_is_not_defined(const string &attribute_name, const string type_name)
{
    return "Class " + type_name + " of attribute " + attribute_name + " is undefined.";
}
