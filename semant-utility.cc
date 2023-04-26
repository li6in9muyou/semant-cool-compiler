#include "loguru.h"

#include "semant-utility.h"
#include "cool-tree.h"

NoRepeatPrinter err = NoRepeatPrinter();

bool NoRepeatPrinter::print(const string &text, ostream &out)
{
    if (printed.find(text) == printed.end())
    {
        printed.insert(text);
        out << text;
        return true;
    }
    else
    {
        return false;
    }
}

string location(const Symbol &filename, int lineno)
{
    return string(filename->get_string()) + ":" + to_string(lineno) + ": ";
}

void abort_if_not_ok(bool ok)
{
    if (!ok)
    {
        err.print("Compilation halted due to static semantic errors.\n");
        exit(1);
    }
}

bool set_type_if_ok(bool ok, Expression e, const Symbol &thiz, const Symbol &that)
{
    if (ok)
    {
        e->set_type(thiz);
    }
    else
    {
        LOG_F(INFO, "not ok, assumes %s", that->get_string());
        e->set_type(that);
    }

    return ok;
}
