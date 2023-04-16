//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#define yylineno curr_lineno;
extern int yylineno;

inline Boolean copy_Boolean(Boolean b) { return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(ostream &stream, int padding, Boolean b)
{
	stream << pad(padding) << (int)b << "\n";
}

void dump_Symbol(ostream &stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

class Program_class;
typedef Program_class *Program;
class Class__class;
typedef Class__class *Class_;
class Feature_class;
typedef Feature_class *Feature;
class Formal_class;
typedef Formal_class *Formal;
class Expression_class;
typedef Expression_class *Expression;
class Case_class;
typedef Case_class *Case;

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;
typedef list_node<Feature> Features_class;
typedef Features_class *Features;
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

#define Program_EXTRAS         \
	virtual void semant() = 0; \
	virtual void dump_with_types(ostream &, int) = 0;

#define program_EXTRAS                           \
public:                                          \
	void dump_with_types(ostream &, int);        \
	void install_basic_classes(SemantContext &); \
	void semant();                               \
	void check_Main_is_defined(SemantContext &);

#define Class__EXTRAS                  \
	virtual Symbol get_filename() = 0; \
	virtual void dump_with_types(ostream &, int) = 0;

#include "symtab.h"
#include <set>
#include <string>
class SemantContext;
class class__class;

#define class__EXTRAS                                                                                             \
public:                                                                                                           \
	void dump_with_types(ostream &, int);                                                                         \
	Symbol get_name();                                                                                            \
	Symbol get_filename();                                                                                        \
	void semant(SemantContext &);                                                                                 \
	bool register_symbol(SemantContext &);                                                                        \
	bool create_family_feature_table(SemantContext &);                                                            \
                                                                                                                  \
private:                                                                                                          \
	bool check_class_in_loop(SymbolTable<Symbol, class__class> &, const class__class &, std::set<std::string> &); \
	void check_superclass_is_defined(SemantContext &);                                                            \
	void check_superclass_is_not_in_cycle(SemantContext &);                                                       \
	void check_superclass_is_not_primitives(SemantContext &);                                                     \
	void check_Main_has_main(SemantContext &);

#define Feature_EXTRAS                                \
public:                                               \
	virtual void dump_with_types(ostream &, int) = 0; \
	virtual void semant(SemantContext &) = 0;         \
	virtual bool register_symbol(SemantContext &) = 0;

#define Feature_SHARED_EXTRAS              \
public:                                    \
	void dump_with_types(ostream &, int);  \
	void semant(SemantContext &) override; \
	bool register_symbol(SemantContext &) override;

#define method_EXTRAS \
private:              \
	bool check_return_type_is_defined(SemantContext &);

#define attr_EXTRAS                                                \
private:                                                           \
	bool check_no_shadow_attribute_in_superclass(SemantContext &); \
	bool check_type_decl_is_defined(SemantContext &);

#define Formal_EXTRAS \
	virtual void dump_with_types(ostream &, int) = 0;

#define formal_EXTRAS \
	void dump_with_types(ostream &, int);

#define Case_EXTRAS \
	virtual void dump_with_types(ostream &, int) = 0;

#define branch_EXTRAS \
	void dump_with_types(ostream &, int);

#define Expression_EXTRAS                             \
	Symbol type;                                      \
	Symbol get_type()                                 \
	{                                                 \
		return type;                                  \
	}                                                 \
	Expression set_type(Symbol s)                     \
	{                                                 \
		type = s;                                     \
		return this;                                  \
	}                                                 \
	virtual void dump_with_types(ostream &, int) = 0; \
	void dump_type(ostream &, int);                   \
	Expression_class()                                \
	{                                                 \
		type = (Symbol)NULL;                          \
	}

#define Expression_SHARED_EXTRAS \
	void dump_with_types(ostream &, int);

#endif
