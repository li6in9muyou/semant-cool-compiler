#include "loguru.h"
#include "semant-error-utility.h"
#include <string>
using std::cerr;
using std::endl;

namespace semant_errors
{
    string env::FileName = "";

    const unordered_map<ErrorType, RenderFunction> handlers = {
        {ErrorType::FormalTypeIsNotSelfType,
         [](const unordered_map<K, string> &ctx)
         {
             return "Formal parameter " + ctx.at(K::name) + " cannot have type SELF_TYPE.";
         }},
        {ErrorType::FormalTypeIsNotDefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Class " + ctx.at(K::typeName) + " of formal parameter " + ctx.at(K::name) + " is undefined.";
         }},
        {ErrorType::MethodRedefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Method " + ctx.at(K::methodName) + " is multiply defined.";
         }},
        {ErrorType::MethodReturnTypeIsNotDefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Undefined return type " + ctx.at(K::typeName) + " in method " + ctx.at(K::methodName) + ".";
         }},
        {ErrorType::AttributeRedefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Attribute " + ctx.at(K::attributeName) + " is multiply defined in class.";
         }},
        {ErrorType::AttributeTypeIsNotDefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Class " + ctx.at(K::typeName) + " of attribute " + ctx.at(K::attributeName) + " is undefined.";
         }},
        {ErrorType::ShadowAttributeInSuperclass,
         [](const unordered_map<K, string> &ctx)
         {
             return "Attribute " + ctx.at(K::attributeName) + " is an attribute of an inherited class.";
         }},
        {ErrorType::NoMainInClassMain,
         [](const unordered_map<K, string> &_)
         {
             return "No 'main' method in class Main.";
         }},
        {ErrorType::SuperclassIsNotDefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Class " + ctx.at(K::className) + " inherits from an undefined class " + ctx.at(K::parentName) + ".";
         }},
        {ErrorType::SuperclassIsInCycle,
         [](const unordered_map<K, string> &ctx)
         {
             return "Class " + ctx.at(K::className) + ", or an ancestor of " + ctx.at(K::className) + ", is involved in an inheritance cycle.";
         }},
        {ErrorType::SuperclassIsPrimitive,
         [](const unordered_map<K, string> &ctx)
         {
             return "Class " + ctx.at(K::className) + " cannot inherit class " + ctx.at(K::parentName) + ".";
         }},
        {ErrorType::ClassIsRedefined,
         [](const unordered_map<K, string> &ctx)
         {
             return "Class " + ctx.at(K::className) + " was previously defined.";
         }},
    };

    bool report_errors(ErrorType type, const unordered_map<K, string> &ctx)
    {
        CHECK_NE_F(ctx.cend(), ctx.find(K::lineNumber), "no line number is supplied in error message context");
        CHECK_GT_F(env::FileName.size(), 0, "fileName is not initilized");
        const auto lineNumber = ctx.at(K::lineNumber);

        const auto text = env::FileName + ":" + lineNumber + ": " + handlers.at(type)(ctx);
        const auto notYetPrinted = printedMessages.end() == printedMessages.find(text);
        if (notYetPrinted)
        {
            printedMessages.emplace(text);
            cerr << text << "\n";
            return true;
        }
        else
        {
            return false;
        }
    }

    void abort_if_errors(bool ok)
    {
        if (!ok)
        {
            cerr << "Compilation halted due to static semantic errors." << endl;
            exit(1);
        }
    }
}