#include <string>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>

namespace semant_errors
{

    using std::function;
    using std::string;
    using std::unordered_map;
    using std::unordered_set;

    enum class ErrorType
    {
        // program
        NoMainInClassMain,
        // class
        SuperclassIsInCycle,
        SuperclassIsNotDefined,
        SuperclassIsPrimitive,
        ClassIsRedefined,
            // method
            MethodRedefined,
        // attribute
        AttributeRedefined,
        AttributeTypeIsNotDefined,
        ShadowAttributeInSuperclass,
    };

    enum class K
    {
        methodName,
        attributeName,
        typeName,
        lineNumber,
        className,
        parentName,
    };

    using RenderFunction = function<string(const unordered_map<K, string> &)>;
    extern const unordered_map<ErrorType, RenderFunction> handlers;
    static unordered_set<string> printedMessages;
    bool report_errors(ErrorType type, const unordered_map<K, string> &ctx);
    void abort_if_errors(bool ok);
}

namespace semant_errors::env
{
    extern std::string FileName;
}