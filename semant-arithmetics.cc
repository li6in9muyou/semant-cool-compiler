#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool plus_class::semant(SemantContext &ctx)
{
    const auto &non_int = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) + "non-Int arguments: " +
                  e1->get_type()->get_string() + " + " +
                  e2->get_type()->get_string() + "\n");
    };

    LOG_SCOPE_F(INFO, "semant at plus at line %d", get_line_number());
    const auto ok = check_operands_are_integer_after_semant(ctx, e1, e2, non_int);
    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool sub_class::semant(SemantContext &ctx)
{
    const auto &non_int = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) + "non-Int arguments: " +
                  e1->get_type()->get_string() + " - " +
                  e2->get_type()->get_string() + "\n");
    };

    LOG_SCOPE_F(INFO, "semant at sub at line %d", get_line_number());
    const auto ok = check_operands_are_integer_after_semant(ctx, e1, e2, non_int);
    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool mul_class::semant(SemantContext &ctx)
{
    const auto &non_int = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) + "non-Int arguments: " +
                  e1->get_type()->get_string() + " * " +
                  e2->get_type()->get_string() + "\n");
    };

    LOG_SCOPE_F(INFO, "semant at mul at line %d", get_line_number());
    const auto ok = check_operands_are_integer_after_semant(ctx, e1, e2, non_int);
    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool divide_class::semant(SemantContext &ctx)
{
    const auto &non_int = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) + "non-Int arguments: " +
                  e1->get_type()->get_string() + " / " +
                  e2->get_type()->get_string() + "\n");
    };

    LOG_SCOPE_F(INFO, "semant at divide at line %d", get_line_number());
    const auto ok = check_operands_are_integer_after_semant(ctx, e1, e2, non_int);
    set_type_if_ok(ok, this, Int, Int);
    return ok;
}

bool neg_class::semant(SemantContext &ctx)
{
    const auto &non_int = [&]()
    {
        err.print(location(ctx.filename, get_line_number()) +
                  "Argument of '~' has type " + e1->get_type()->get_string() + " instead of Int.\n");
    };

    LOG_SCOPE_F(INFO, "semant at neg at line %d", get_line_number());
    auto ok = true;
    ok &= e1->semant(ctx);
    ok &= check_symbol_eq(Int, e1->get_type(), non_int);
    set_type_if_ok(ok, this, Int, Int);
    return ok;
}
