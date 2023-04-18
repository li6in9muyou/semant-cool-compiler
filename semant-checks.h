#include "semant.h"
#include <functional>
using std::function;

void noop();

bool define_symbol_if_not_defined_already(
    SymbolTable<Symbol, tree_node> &scope,
    const Symbol &symbol,
    tree_node *node,
    function<void()> on_error);

bool check_symbol_is_not(const Symbol &s, const Symbol &bad, function<void()> on_error = noop);

bool check_symbol_is(const Symbol &s, const Symbol &good, function<void()> on_error = noop);

template <class T>
bool check_symbol_exists(const Symbol &want, SymbolTable<Symbol, T> &scope, function<void()> on_error = noop)
{
    const auto ok = scope.lookup(want) != nullptr;
    if (!ok)
    {
        on_error();
    }
    return ok;
}
