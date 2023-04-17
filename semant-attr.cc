#include <string>
using std::string;
using std::to_string;

#include "semant.h"
#include "loguru.h"
#include "semant-error-utility.h"
using semant_errors::ErrorType;
using semant_errors::K;
using semant_errors::report_errors;

[[nodiscard]] bool attr_class::register_symbol(SemantContext &ctx)
{
    LOG_F(INFO, "attribute register symbol at %s", name->get_string());

    CHECK_NOTNULL_F(ctx.familyAttributeTable, "ctx.familyAttributeTable is nullptr");
    const auto redefined = nullptr != ctx.familyAttributeTable->probe(name);
    auto shadowed = false;
    if (!redefined)
    {
        shadowed = !check_no_shadow_attribute_in_superclass(ctx);
        if (shadowed)
        {
            LOG_F(INFO, "shadow check failed, does not add symbol");
            report_errors(
                ErrorType::ShadowAttributeInSuperclass,
                {{K::attributeName, name->get_string()},
                 {K::lineNumber, to_string(line_number)}});
        }
        else
        {
            LOG_F(INFO, "not redefining and not shadowing, add symbol");
            ctx.familyAttributeTable->addid(name, this);
        }
    }
    else
    {
        LOG_F(INFO, "redefinition check failed, add error");
        report_errors(
            ErrorType::AttributeRedefined,
            {{K::attributeName, name->get_string()},
             {K::lineNumber, to_string(line_number)}});
    }
    return !redefined && !shadowed;
}

[[nodiscard]] bool attr_class::check_type_decl_is_defined(SemantContext &ctx)
{
    return nullptr != ctx.classTable.lookup(this->type_decl);
}

[[nodiscard]] bool attr_class::check_no_shadow_attribute_in_superclass(SemantContext &ctx)
{
    return nullptr == ctx.familyAttributeTable->lookup(name);
}

bool attr_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_FUNCTION(INFO);

    const auto ok = check_type_decl_is_defined(ctx);
    if (!ok)
    {
        report_errors(
            ErrorType::AttributeTypeIsNotDefined,
            {{K::typeName, type_decl->get_string()},
             {K::attributeName, name->get_string()},
             {K::lineNumber, to_string(line_number)}});
    }
    return ok;
}
