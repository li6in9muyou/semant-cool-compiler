#include <string>
using std::string;

#include "stringtab.h"

#include "semant.h"
#include "semant-utility.h"
#include "semant-checks.h"
#include "loguru.h"

bool int_const_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at integer constant %s, return true", token->get_string());
    set_type(Int);
    return true;
}

bool bool_const_class::semant(SemantContext &ctx)
{
    LOG_IF_F(INFO, val == 0, "semant at boolean constant \"false\", return true");
    LOG_IF_F(INFO, val == 1, "semant at boolean constant \"true\", return true");
    set_type(Bool);
    return true;
}

bool string_const_class::semant(SemantContext &ctx)
{
    LOG_F(INFO, "semant at string constant of length %d, return true", token->get_len());
    set_type(Str);
    return true;
}