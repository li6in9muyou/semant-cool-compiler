#include <string>
using std::string;

#include "stringtab.h"

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

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

bool plus_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at plus at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " + " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool sub_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at sub at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " - " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool mul_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at mul at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " * " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool divide_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at divide at line %d", get_line_number());
    auto ok = true;

    ok &= check_both_operands_are_integer(
        ctx, e1, e2,
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "non-Int arguments: " +
                      e1->get_type()->get_string() + " / " +
                      e2->get_type()->get_string() + "\n");
        });

    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool neg_class::semant(SemantContext &ctx)
{
    LOG_SCOPE_F(INFO, "semant at neg at line %d", get_line_number());
    auto ok = true;

    ok &= e1->semant(ctx);
    ok &= check_symbol_is(
        Int,
        e1->get_type(),
        [&]()
        {
            err.print(location(ctx.filename, get_line_number()) +
                      "Argument of '~' has type " + e1->get_type()->get_string() + " instead of Int.\n");
        });

    set_type_if_ok(ok, this, Int, Int);
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
