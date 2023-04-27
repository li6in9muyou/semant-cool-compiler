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

template <class T>
bool check_symbol_not_exists(const Symbol &forbid, SymbolTable<Symbol, T> &scope, function<void()> on_error = noop)
{
    const auto ok = scope.lookup(forbid) == nullptr;
    if (!ok)
    {
        on_error();
    }
    return ok;
}

template <class T>
bool check_symbol_exists_in_current_scope(const Symbol &want, SymbolTable<Symbol, T> &scope, function<void()> on_error = noop)
{
    const auto ok = scope.probe(want) != nullptr;
    if (!ok)
    {
        on_error();
    }
    return ok;
}

template <class T>
bool check_symbol_not_exists_in_current_scope(const Symbol &forbid, SymbolTable<Symbol, T> &scope, function<void()> on_error = noop)
{
    const auto ok = scope.probe(forbid) == nullptr;
    if (!ok)
    {
        on_error();
    }
    return ok;
}

bool check_both_operands_are_integer(SemantContext &ctx, Expression &e1, Expression &e2, function<void()> on_error = noop);
