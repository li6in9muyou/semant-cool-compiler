#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool lt_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at '<' at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " < " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Bool, Bool);
    return ok;
}

bool eq_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at '=' at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " = " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Bool, Bool);
    return ok;
}

bool leq_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at '<=' at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " <= " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Bool, Bool);
    return ok;
}

bool comp_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at comp at line %d", get_line_number());
    auto ok = true;

    ok &= e1->semant(ctx);
    ok &= check_symbol_is(
        Bool,
        e1->get_type(),
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Argument of 'not' has type " + e1->get_type()->get_string() + " instead of Bool.\n");
        });

    set_type_if_ok(ok, this, Bool, Bool);
    return ok;
}
