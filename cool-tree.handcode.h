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
	void semant();

#define Class__EXTRAS                  \
	virtual Symbol get_filename() = 0; \
	virtual void dump_with_types(ostream &, int) = 0;

#include "symtab.h"
#include <set>
#include <string>
class SemantContext;
class class__class;
extern Symbol No_type;

#define class__EXTRAS                                                \
public:                                                              \
	void dump_with_types(ostream &, int);                            \
	Symbol get_filename();                                           \
	[[nodiscard]] bool semant(SemantContext &);                      \
	[[nodiscard]] bool register_symbol(SemantContext &);             \
	[[nodiscard]] bool create_family_feature_table(SemantContext &); \
                                                                     \
private:                                                             \
	[[nodiscard]] bool check_class_in_loop(SymbolTable<Symbol, class__class> &, const class__class &, std::set<std::string> &);

#define Feature_EXTRAS                                      \
public:                                                     \
	virtual void dump_with_types(ostream &, int) = 0;       \
	[[nodiscard]] virtual bool semant(SemantContext &) = 0; \
	[[nodiscard]] virtual bool register_symbol(SemantContext &) = 0;

#define Feature_SHARED_EXTRAS \
public:                       \
	void dump_with_types(ostream &, int);

#define method_EXTRAS                                    \
public:                                                  \
	[[nodiscard]] bool semant(SemantContext &) override; \
	[[nodiscard]] bool register_symbol(SemantContext &) override;

#define attr_EXTRAS                                      \
public:                                                  \
	[[nodiscard]] bool semant(SemantContext &) override; \
	[[nodiscard]] bool register_symbol(SemantContext &) override;

#define Formal_EXTRAS \
	virtual void dump_with_types(ostream &, int) = 0;

#define formal_EXTRAS                                    \
public:                                                  \
	[[nodiscard]] bool semant(SemantContext &);          \
	[[nodiscard]] bool register_symbol(SemantContext &); \
	void dump_with_types(ostream &, int);

#define Case_EXTRAS \
	virtual void dump_with_types(ostream &, int) = 0;

#include "loguru.h"

#define Expression_EXTRAS                                                                                          \
	Symbol type;                                                                                                   \
	Symbol get_type()                                                                                              \
	{                                                                                                              \
		return type;                                                                                               \
	}                                                                                                              \
	Expression set_type(Symbol s)                                                                                  \
	{                                                                                                              \
		type = s;                                                                                                  \
		return this;                                                                                               \
	}                                                                                                              \
	virtual void dump_with_types(ostream &, int) = 0;                                                              \
	void dump_type(ostream &, int);                                                                                \
	Expression_class()                                                                                             \
	{                                                                                                              \
		type = (Symbol)NULL;                                                                                       \
	}                                                                                                              \
	virtual bool semant(SemantContext &ctx)                                                                        \
	{                                                                                                              \
		LOG_F(WARNING,                                                                                             \
			  "semant at line %d, expression class does not override semant, return true and set type to No_type", \
			  get_line_number());                                                                                  \
		set_type(No_type);                                                                                         \
		return true;                                                                                               \
	}

#define Expression_SHARED_EXTRAS \
	void dump_with_types(ostream &, int);

#define new__EXTRAS \
public:             \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define block_EXTRAS \
public:              \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define typcase_EXTRAS \
public:                \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define branch_EXTRAS                     \
public:                                   \
	void dump_with_types(ostream &, int); \
	[[nodiscard]] bool semant(SemantContext &ctx);

#define plus_EXTRAS \
public:             \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define sub_EXTRAS \
public:            \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define mul_EXTRAS \
public:            \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define divide_EXTRAS \
public:               \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define neg_EXTRAS \
public:            \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define int_const_EXTRAS \
public:                  \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define bool_const_EXTRAS \
public:                   \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define string_const_EXTRAS \
public:                     \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define comp_EXTRAS \
public:             \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define lt_EXTRAS \
public:           \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define eq_EXTRAS \
public:           \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define leq_EXTRAS \
public:            \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define assign_EXTRAS \
public:               \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#define object_EXTRAS \
public:               \
	[[nodiscard]] bool semant(SemantContext &ctx) override;

#endif
