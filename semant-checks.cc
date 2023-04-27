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

bool check_symbol_is_not(const Symbol &s, const Symbol &bad, function<void()> on_error)
{
    const auto ok = !bad->equal_string(s->get_string(), s->get_len());
    if (!ok)
    {
        on_error();
    }
    return ok;
}

bool check_symbol_is(const Symbol &s, const Symbol &good, function<void()> on_error)
{
    const auto ok = good->equal_string(s->get_string(), s->get_len());
    if (!ok)
    {
        on_error();
    }
    return ok;
}

void noop()
{
}

bool check_both_operands_are_integer(SemantContext &ctx, Expression &e1, Expression &e2, function<void()> on_error)
{
    auto ok = true;

    ok &= e1->semant(ctx);
    ok &= e2->semant(ctx);

    const auto e1Type = e1->get_type();
    const auto e2Type = e2->get_type();
    ok &= (check_symbol_is(Int, e1Type, on_error) && check_symbol_is(Int, e2Type, on_error));

    return ok;
}
