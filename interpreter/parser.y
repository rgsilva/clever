%skeleton "lalr1.cc"
%require "2.4.1"
%defines
%define namespace "clever"
%define parser_class_name "Parser"

%code requires {
/**
 * Clever programming language
 * Copyright (c) 2011-2012 Clever Team
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stack>
#include "interpreter/scanner.h"
#include "compiler/cstring.h"

namespace clever {
class Driver;
class Compiler;
class Value;
} // clever
}

// The parsing context.
%parse-param { Driver& driver }
%parse-param { ScannerState& state }
%parse-param { Compiler& c }
%lex-param   { Driver& driver }
%lex-param   { ScannerState& state }

%locations
%initial-action {
@$.begin.filename = @$.end.filename = driver.getFile();
};

%union {
	const CString *str;
	Value* val;
};

%type <str> IDENT
%type <val> NUM_INTEGER NUM_DOUBLE
%type <val> r_value math_expr

%debug
%error-verbose

%code {
#include "interpreter/driver.h"
}

%token END  0        "end of file"
%token VAR           "var"
%token EXIT          "exit"
%token TYPE          "type specification"
%token IDENT         "identifier"
%token NUM_INTEGER   "number"
%token NUM_DOUBLE    "float-number"
%token STR           "string"
%token FOR           "for"
%token WHILE         "while"
%token IF            "if"
%token ELSE          "else"
%token ELSEIF        "else if"
%token LESS_EQUAL    "<="
%token GREATER_EQUAL ">="
%token LESS          "<"
%token GREATER       ">"
%token RSHIFT        ">>"
%token RSHIFT_EQUAL  ">>="
%token LSHIFT        "<<"
%token LSHIFT_EQUAL  "<<="
%token BREAK         "break"
%token EQUAL         "=="
%token NOT_EQUAL     "!="
%token IMPORT        "import"
%token PLUS_EQUAL    "+="
%token MULT_EQUAL    "*="
%token DIV_EQUAL     "/="
%token MINUS_EQUAL   "-="
%token MOD_EQUAL     "%="
%token BOOLEAN_OR    "||"
%token LOGICAL_OR    "or"
%token BOOLEAN_AND   "&&"
%token LOGICAL_AND   "and"
%token BW_AND_EQUAL  "&="
%token BW_OR_EQUAL   "|="
%token BW_XOR_EQUAL  "^="
%token RETURN        "return"
%token TRUE          "true"
%token FALSE         "false"
%token CONST         "const"
%token PRINT         "print"

%left ',';
%left LOGICAL_OR;
%left LOGICAL_AND;
%right '=' PLUS_EQUAL MINUS_EQUAL DIV_EQUAL MULT_EQUAL MOD_EQUAL BW_OR_EQUAL XOR_EQUAL BW_AND_EQUAL  RSHIFT_EQUAL LSHIFT_EQUAL;
%left ':';
%left BOOLEAN_OR;
%left BOOLEAN_AND;
%left '|';
%left '^';
%left '&';
%nonassoc EQUAL NOT_EQUAL;
%nonassoc LESS LESS_EQUAL GREATER GREATER_EQUAL;
%left LSHIFT RSHIFT;
%left '-' '+' '.';
%left '*' '/' '%';
%right '!';
%right '~' INCREMENT DECREMENT COPY DEEPCOPY;
%right '[' '{';
%left ELSEIF;
%left ELSE;
%left UMINUS;

%%

%start top_statements;

top_statements:
		{ c.init(); } statement_list { c.end(); }
;

statement_list:
		/* empty */
	|	non_empty_statement_list statement_list
;

non_empty_statement_list:
		'{' { c.newScope(); } non_empty_statement_list { c.endScope(); } '}'
	|	var_declaration ';'
	|	assignment ';'
	|	print_stmt ';'
;

var_declaration:
		VAR IDENT             { c.varDeclaration($2, NULL); }
	|	VAR IDENT '=' r_value { c.varDeclaration($2, $4);   }
;

assignment:
		IDENT '=' r_value { c.assignment($1, $3, yyloc); }
;

r_value:
		NUM_INTEGER
	|	NUM_DOUBLE
	|	math_expr
;

math_expr:
		r_value '+' r_value { $$ = c.binOp(OP_PLUS, $1, $3); }
;

print_stmt:
		PRINT '(' IDENT ')' { c.print($3, yyloc); }
;

%%

namespace clever {

void Parser::error(const Parser::location_type& line, const std::string& msg)
{
	driver.error(line, msg);
}

} // clever
