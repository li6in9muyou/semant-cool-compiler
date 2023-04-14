#ifndef SEMANT_H_
#define SEMANT_H_
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

using ClassTable = SymbolTable<Symbol, class__class>;
using AttributeTable = SymbolTable<Symbol, attr_class>;
using MethodTable = SymbolTable<Symbol, method_class>;

class FeatureTable
{
public:
  MethodTable methods;
  AttributeTable attributes;
  FeatureTable() = default;
  void enterscope();
  void exitscope();
};

class SemantContext
{
private:
  int semant_errors;
  ostream &error_stream;
  Symbol filename;

public:
  unordered_map<Symbol, FeatureTable> programFeatureTable;
  ClassTable classTable;
  MethodTable *familyMethodTable;
  AttributeTable *familyAttributeTable;
  SemantContext();
  void set_filename(Symbol filename);
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(tree_node *t);
  ostream &semant_error(Symbol filename, tree_node *t);
  void abort_if_error();
};

#endif
