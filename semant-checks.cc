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
