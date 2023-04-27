#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool lt_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at '<' at line %d", get_line_number());
    auto ok = true;

    ok &= check_operands_are_integer_after_semant(
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

    ok &= e1->semant(ctx);
    ok &= e2->semant(ctx);
    const auto e1T = e1->get_type();
    const auto e2T = e2->get_type();

    const auto e1IsPrimitive =
        check_symbol_eq(Int, e1T) ||
        check_symbol_eq(Bool, e1T) ||
        check_symbol_eq(Str, e1T);
    const auto e2IsPrimitive =
        check_symbol_eq(Int, e2T) ||
        check_symbol_eq(Bool, e2T) ||
        check_symbol_eq(Str, e2T);
    const auto involvePrimitive = e1IsPrimitive || e2IsPrimitive;
    if (involvePrimitive)
    {
        ok &= check_symbol_eq(
            e1T, e2T,
            [&]()
            {
                err.print(location(ctx.filename, get_line_number()) +
                          "Illegal comparison with a basic type.\n");
            });
    }
    else
    {
        ok &= true;
    }

    set_type_if_ok(ok, this, Bool, Bool);
    return ok;
}

bool leq_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at '<=' at line %d", get_line_number());
    auto ok = true;

    ok &= check_operands_are_integer_after_semant(
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
    ok &= check_symbol_eq(
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
