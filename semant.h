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

class FeatureTable
{
public:
  SymbolTable<Symbol, method_class> methods;
  SymbolTable<Symbol, attr_class> attributes;
  FeatureTable() = default;
  void enterscope();
  void exitscope();
};

class SemantContext
{
private:
  int semant_errors;
  ostream &error_stream;

public:
  Symbol filename;
  unordered_map<Symbol, FeatureTable> programFeatureTable;
  SymbolTable<Symbol, class__class> classTable;
  SymbolTable<Symbol, method_class> *familyMethodTable;
  SymbolTable<Symbol, attr_class> *familyAttributeTable;
  SemantContext();
  void set_filename(Symbol filename);
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(tree_node *t);
  ostream &semant_error(Symbol filename, tree_node *t);
  void abort_if_error();
};

#endif
