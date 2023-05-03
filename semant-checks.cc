#include "loguru.h"

#include "semant-utility.h"
#include "semant-checks.h"

bool define_symbol_if_not_defined_already(
    SymbolTable<Symbol, tree_node> &scope,
    const Symbol &symbol,
    tree_node *node,
    function<void()> on_error)
{
    const auto ok = scope.lookup(symbol) == nullptr;
    if (!ok)
    {
        on_error();
    }
    else
    {
        scope.addid(symbol, node);
    }
    return ok;
}

bool check_symbol_not_eq(const Symbol &thiz, const Symbol &that, function<void()> on_error)
{
    const auto ok = !check_symbol_eq(thiz, that);
    if (!ok)
    {
        on_error();
    }
    return ok;
}

bool check_symbol_eq(const Symbol &thiz, const Symbol &that, function<void()> on_error)
{
    CHECK_F(thiz != nullptr && that != nullptr, "thiz=%s, that=%s at check symbol equal",
            (thiz == nullptr ? "<nullptr>" : thiz->get_string()),
            (that == nullptr ? "<nullptr>" : that->get_string()));

    const auto ok = that->equal_string(thiz->get_string(), thiz->get_len());
    if (!ok)
    {
        on_error();
    }
    return ok;
}

void noop()
{
}

bool check_operands_are_integer_after_semant(SemantContext &ctx, Expression &e1, Expression &e2, function<void()> on_error)
{
    auto ok = true;

    ok &= e1->semant(ctx);
    ok &= e2->semant(ctx);

    const auto e1Type = e1->get_type();
    const auto e2Type = e2->get_type();
    ok &= (check_symbol_eq(Int, e1Type, on_error) && check_symbol_eq(Int, e2Type, on_error));

    return ok;
}

bool check_type_conform_to(SemantContext &ctx, const Symbol &t, const Symbol &super, function<void()> on_error)
{
    const auto tType = translate_SELF_TYPE(ctx.typeEnv, t);
    const auto superType = translate_SELF_TYPE(ctx.typeEnv, super);

    LOG_F(INFO, "at check type conform %s <= %s",
          dump_symbols(ctx.familyHierarchyHash[tType]).c_str(),
          dump_symbols(ctx.familyHierarchyHash[superType]).c_str());

    auto ok = check_symbol_eq(tType, superType) || check_symbol_eq(least_upper_bound(ctx, tType, superType), super);
    if (!ok)
    {
        on_error();
    }
    return ok;
}
