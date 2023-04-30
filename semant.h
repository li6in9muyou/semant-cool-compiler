#ifndef SEMANT_H_
#define SEMANT_H_
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <unordered_map>
using std::unordered_map;

#include "cool-tree.h"

extern Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;

class FeatureTable
{
public:
  SymbolTable<Symbol, method_class> methods;
  SymbolTable<Symbol, Symbol> attributes;
  FeatureTable() = default;
  void enterscope();
  void exitscope();
};

class SemantContext
{
public:
  Symbol filename;
  unordered_map<Symbol, FeatureTable> programFeatureTable;
  SymbolTable<Symbol, class__class> classTable;
  unordered_map<Symbol, vector<Symbol>> familyHierarchyHash;
  SymbolTable<Symbol, method_class> *familyMethodTable;
  SymbolTable<Symbol, Symbol> *typeEnv;
  SemantContext() = default;
};

#endif
