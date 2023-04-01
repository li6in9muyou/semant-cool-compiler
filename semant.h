#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class SemantContext
{
  using CoolSymbolTable = SymbolTable<Symbol, tree_node>;

private:
  int semant_errors;
  ostream &error_stream;

public:
  CoolSymbolTable sym;
  SemantContext();
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(Class_ c);
  ostream &semant_error(Symbol filename, tree_node *t);
  void abort_if_error();
};

#endif
