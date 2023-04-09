#ifndef SEMANT_H_
#define SEMANT_H_

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

class SemantContext
{
  using CoolSymbolTable = SymbolTable<Symbol, tree_node>;
  using AttributeScope = SymbolTable<Symbol, attr_class>;
  using MethodScope = SymbolTable<Symbol, method_class>;
  using FormalScope = SymbolTable<Symbol, formal_class>;
  using VariableScope = SymbolTable<Symbol, object_class>;

  using ClassTable = SymbolTable<Symbol, class__class>;
  using ObjectTable = SymbolTable<Symbol, Expression_class>;
  using MethodTable = SymbolTable<Symbol, method_class>;

private:
  int semant_errors;
  ostream &error_stream;
  Symbol filename;

public:
  ClassTable classTable;
  MethodScope methodTable;
  AttributeScope attributeTable;
  SemantContext();
  void set_filename(Symbol filename);
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(tree_node *t);
  ostream &semant_error(Symbol filename, tree_node *t);
  void abort_if_error();
};

#endif
