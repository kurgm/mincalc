/*
 * parser.c
 */

#include "io.h"
#include "strutils.h"

#include "parser.h"

/*

mincalc grammar
===============

statement ::= set-variable | expression
set-variable ::= assignment | function-definition
assignment ::= identifier ":=" expression
function-definition ::= identifier "(" identifier-list-opt ")" ":=" expression
expression ::= expr1 | expression "|" expr1
expr1 ::= expr2 | expr1 "^" expr2
expr2 ::= expr3 | expr2 "&" expr3
expr3 ::= expr4 | expr3 "==" expr4 | expr3 "<>" expr4
expr4 ::= expr5 | \
    expr4 "<" expr5 | expr4 "<=" expr5 | expr4 ">" expr5 | expr4 ">=" expr5
expr5 ::= expr6 | expr5 "<<" expr6 | expr5 ">>" expr6 | expr5 ">>>" expr6
expr6 ::= expr7 | expr6 "+" expr7 | expr6 "-" expr7
expr7 ::= expr8 | expr7 "*" expr8 | expr7 "/" expr8 | expr7 "%" expr8
expr8 ::= term | "~" expr8 | "+" expr8 | "-" expr8
term ::= integer | identifier | func-call | "(" expression ")"
func-call ::= identifier "(" argument-list-opt ")"

identifier-list-opt ::= "" | identifier-list
identifier-list ::= identifier | identifier "," identifier-list
argument-list-opt ::= "" | argument-list
argument-list ::= expression | expression "," argument-list

identifier ::= alphabet | alphabet alphanum | \
    alphabet alphanum alphanum | alphabet alphanum alphanum alphanum
alphanum ::= alphabet | digit
integer ::= digit | digit integer
alphabet ::= \
    "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | \
    "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | \
    "y" | "z"
digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

*/

static const char slr_table[][NSYMBOL] = {
    {0},

    {0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 80, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_SETVAR_ASSIGN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_SETVAR_FUNDEF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 6, 30, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_ASSIGN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 9, 0, 0, ~RL_IDLIST_OPT_0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 0, 0},

    {0, 0, 0, 0, 0, ~RL_IDLIST_OPT_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, ~RL_IDLIST, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0},

    {0, 0, 0, 0, 0, ~RL_IDLIST_CONS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 15, 30, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_FUNDEF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 27, 30, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_TERM_INT, 0, 0, 0, 0, ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT,
     ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT,
     ~RL_TERM_INT, ~RL_TERM_INT, 0, ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT,
     ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT, ~RL_TERM_INT,
     ~RL_TERM_INT,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_TERM_FUNCALL, 0, 0, 0, 0, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL,
     ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL,
     ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, 0,
     ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL,
     ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL, ~RL_TERM_FUNCALL,
     ~RL_TERM_FUNCALL,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_TERM_ID, 0, 0, 0, 20, ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID,
     ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID,
     ~RL_TERM_ID, ~RL_TERM_ID, 0, ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID,
     ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID, ~RL_TERM_ID,
     ~RL_TERM_ID,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, ~RL_ARGLIST_OPT_0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0,
     0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 24, 30, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 21, 23},

    {0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_FUNCALL, 0, 0, 0, 0, ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL,
     ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL,
     ~RL_FUNCALL, ~RL_FUNCALL, 0, ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL,
     ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL, ~RL_FUNCALL,
     ~RL_FUNCALL,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, ~RL_ARGLIST_OPT_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, ~RL_ARGLIST, 25, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 24, 30, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 26},

    {0, 0, 0, 0, 0, ~RL_ARGLIST_CONS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_STMT_EXPR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 29, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_OR, 0, 0, 0, 0, ~RL_OR, ~RL_OR, 0, 0, 0, 0, 0, 0, ~RL_OR, 31, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_OR, 0, 0, 0, 0, ~RL_EXPR_OR, ~RL_EXPR_OR, 0, 0, 0, 0, 0, 0,
     ~RL_EXPR_OR, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 32, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_XOR, 0, 0, 0, 0, ~RL_XOR, ~RL_XOR, 0, 0, 0, 0, 0, 34, ~RL_XOR, ~RL_XOR,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_XOR, 0, 0, 0, 0, ~RL_EXPR_XOR, ~RL_EXPR_XOR, 0, 0, 0, 0, 0, 34,
     ~RL_EXPR_XOR, ~RL_EXPR_XOR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 35, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_AND, 0, 0, 0, 0, ~RL_AND, ~RL_AND, 0, 0, 0, 0, 0, ~RL_AND, ~RL_AND,
     ~RL_AND, 0, 37, 38, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_AND, 0, 0, 0, 0, ~RL_EXPR_AND, ~RL_EXPR_AND, 0, 0, 0, 0, 0,
     ~RL_EXPR_AND, ~RL_EXPR_AND, ~RL_EXPR_AND, 0, 37, 38, 0, 0, 0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 39, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 40, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_EQ, 0, 0, 0, 0, ~RL_EQ, ~RL_EQ, 0, 0, 0, 0, 0, ~RL_EQ, ~RL_EQ, ~RL_EQ,
     0, ~RL_EQ, ~RL_EQ, 42, 43, 44, 45, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_NEQ, 0, 0, 0, 0, ~RL_NEQ, ~RL_NEQ, 0, 0, 0, 0, 0, ~RL_NEQ, ~RL_NEQ,
     ~RL_NEQ, 0, ~RL_NEQ, ~RL_NEQ, 42, 43, 44, 45, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_EQ, 0, 0, 0, 0, ~RL_EXPR_EQ, ~RL_EXPR_EQ, 0, 0, 0, 0, 0,
     ~RL_EXPR_EQ, ~RL_EXPR_EQ, ~RL_EXPR_EQ, 0, ~RL_EXPR_EQ, ~RL_EXPR_EQ, 42, 43,
     44, 45, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 46, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_LT, 0, 0, 0, 0, ~RL_LT, ~RL_LT, 0, 0, 0, 0, 0, ~RL_LT, ~RL_LT, ~RL_LT,
     0, ~RL_LT, ~RL_LT, ~RL_LT, ~RL_LT, ~RL_LT, ~RL_LT, 51, 52, 53,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_LEQ, 0, 0, 0, 0, ~RL_LEQ, ~RL_LEQ, 0, 0, 0, 0, 0, ~RL_LEQ, ~RL_LEQ,
     ~RL_LEQ, 0, ~RL_LEQ, ~RL_LEQ, ~RL_LEQ, ~RL_LEQ, ~RL_LEQ, ~RL_LEQ, 51, 52,
     53,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_GT, 0, 0, 0, 0, ~RL_GT, ~RL_GT, 0, 0, 0, 0, 0, ~RL_GT, ~RL_GT, ~RL_GT,
     0, ~RL_GT, ~RL_GT, ~RL_GT, ~RL_GT, ~RL_GT, ~RL_GT, 51, 52, 53,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_GEQ, 0, 0, 0, 0, ~RL_GEQ, ~RL_GEQ, 0, 0, 0, 0, 0, ~RL_GEQ, ~RL_GEQ,
     ~RL_GEQ, 0, ~RL_GEQ, ~RL_GEQ, ~RL_GEQ, ~RL_GEQ, ~RL_GEQ, ~RL_GEQ, 51, 52,
     53,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_CMP, 0, 0, 0, 0, ~RL_EXPR_CMP, ~RL_EXPR_CMP, 0, 0, 0, 0, 0,
     ~RL_EXPR_CMP, ~RL_EXPR_CMP, ~RL_EXPR_CMP, 0, ~RL_EXPR_CMP, ~RL_EXPR_CMP,
     ~RL_EXPR_CMP, ~RL_EXPR_CMP, ~RL_EXPR_CMP, ~RL_EXPR_CMP, 51, 52, 53,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 62, 69, 70, 18, 0, 0, 0, 0},

    {~RL_LL, 0, 0, 0, 0, ~RL_LL, ~RL_LL, 58, 59, 0, 0, 0, ~RL_LL, ~RL_LL,
     ~RL_LL, 0, ~RL_LL, ~RL_LL, ~RL_LL, ~RL_LL, ~RL_LL, ~RL_LL, ~RL_LL, ~RL_LL,
     ~RL_LL,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_GG, 0, 0, 0, 0, ~RL_GG, ~RL_GG, 58, 59, 0, 0, 0, ~RL_GG, ~RL_GG,
     ~RL_GG, 0, ~RL_GG, ~RL_GG, ~RL_GG, ~RL_GG, ~RL_GG, ~RL_GG, ~RL_GG, ~RL_GG,
     ~RL_GG,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_GGG, 0, 0, 0, 0, ~RL_GGG, ~RL_GGG, 58, 59, 0, 0, 0, ~RL_GGG, ~RL_GGG,
     ~RL_GGG, 0, ~RL_GGG, ~RL_GGG, ~RL_GGG, ~RL_GGG, ~RL_GGG, ~RL_GGG, ~RL_GGG,
     ~RL_GGG, ~RL_GGG,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_SHIFT, 0, 0, 0, 0, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, 58, 59, 0, 0,
     0, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, 0, ~RL_EXPR_SHIFT,
     ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT,
     ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT, ~RL_EXPR_SHIFT,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 69, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 61, 69, 70, 18, 0, 0, 0, 0},

    {~RL_ADD, 0, 0, 0, 0, ~RL_ADD, ~RL_ADD, ~RL_ADD, ~RL_ADD, 63, 64, 65,
     ~RL_ADD, ~RL_ADD, ~RL_ADD, 0, ~RL_ADD, ~RL_ADD, ~RL_ADD, ~RL_ADD, ~RL_ADD,
     ~RL_ADD, ~RL_ADD, ~RL_ADD, ~RL_ADD,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_SUB, 0, 0, 0, 0, ~RL_SUB, ~RL_SUB, ~RL_SUB, ~RL_SUB, 63, 64, 65,
     ~RL_SUB, ~RL_SUB, ~RL_SUB, 0, ~RL_SUB, ~RL_SUB, ~RL_SUB, ~RL_SUB, ~RL_SUB,
     ~RL_SUB, ~RL_SUB, ~RL_SUB, ~RL_SUB,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_ADDSUB, 0, 0, 0, 0, ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB,
     ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB, 63, 64, 65, ~RL_EXPR_ADDSUB,
     ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB, 0, ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB,
     ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB,
     ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB, ~RL_EXPR_ADDSUB,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 67, 70, 18, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 18, 0, 0, 0, 0},

    {~RL_MUL, 0, 0, 0, 0, ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL,
     ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL, 0, ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL,
     ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL, ~RL_MUL,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_DIV, 0, 0, 0, 0, ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV,
     ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV, 0, ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV,
     ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV, ~RL_DIV,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_MOD, 0, 0, 0, 0, ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD,
     ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD, 0, ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD,
     ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD, ~RL_MOD,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_MULDIV, 0, 0, 0, 0, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV,
     ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV,
     ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, 0,
     ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV,
     ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV, ~RL_EXPR_MULDIV,
     ~RL_EXPR_MULDIV,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_EXPR_UNARY, 0, 0, 0, 0, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY,
     ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY,
     ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, 0, ~RL_EXPR_UNARY,
     ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY,
     ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY, ~RL_EXPR_UNARY,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 70, 18, 0, 0, 0, 0},

    {~RL_NOT, 0, 0, 0, 0, ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT,
     ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT, 0, ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT,
     ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT, ~RL_NOT,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 74, 70, 18, 0, 0, 0, 0},

    {~RL_UPLUS, 0, 0, 0, 0, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS,
     ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, 0,
     ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS,
     ~RL_UPLUS, ~RL_UPLUS, ~RL_UPLUS,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 76, 70, 18, 0, 0, 0, 0},

    {~RL_UMINUS, 0, 0, 0, 0, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS,
     ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, 0,
     ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS,
     ~RL_UMINUS, ~RL_UMINUS, ~RL_UMINUS,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0, 17, 19, 0, 77, 0, 0, 73, 75, 0, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0,
     0, 0,
     //
     0, 0, 0, 0, 78, 30, 33, 36, 41, 50, 57, 62, 69, 70, 18, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_TERM_GROUP, 0, 0, 0, 0, ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP,
     ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP,
     ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP, 0, ~RL_TERM_GROUP,
     ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP,
     ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP, ~RL_TERM_GROUP,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {~RL_STMT_SETVAR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     //
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

};

typedef struct {
    enum nonterminal nt;
    char ntokens;
    char arg1pos;
    char arg2pos;
} ruledef_entry;

static const ruledef_entry rules[NRULES] = {
    {NT_STMT, 1, 0, -1},         {NT_STMT, 1, 0, -1},
    {NT_SETVAR, 1, 0, -1},       {NT_SETVAR, 1, 0, -1},
    {NT_ASSIGN, 3, 0, 2},        {NT_FUNDEF, 6, 0, 2},
    {NT_EXPR_OR, 1, 0, -1},      {NT_EXPR_OR, 3, 0, 2},
    {NT_EXPR_XOR, 1, 0, -1},     {NT_EXPR_XOR, 3, 0, 2},
    {NT_EXPR_AND, 1, 0, -1},     {NT_EXPR_AND, 3, 0, 2},
    {NT_EXPR_EQ, 1, 0, -1},      {NT_EXPR_EQ, 3, 0, 2},
    {NT_EXPR_EQ, 3, 0, 2},       {NT_EXPR_CMP, 1, 0, -1},
    {NT_EXPR_CMP, 3, 0, 2},      {NT_EXPR_CMP, 3, 0, 2},
    {NT_EXPR_CMP, 3, 0, 2},      {NT_EXPR_CMP, 3, 0, 2},
    {NT_EXPR_SHIFT, 1, 0, -1},   {NT_EXPR_SHIFT, 3, 0, 2},
    {NT_EXPR_SHIFT, 3, 0, 2},    {NT_EXPR_SHIFT, 3, 0, 2},
    {NT_EXPR_ADDSUB, 1, 0, -1},  {NT_EXPR_ADDSUB, 3, 0, 2},
    {NT_EXPR_ADDSUB, 3, 0, 2},   {NT_EXPR_MULDIV, 1, 0, -1},
    {NT_EXPR_MULDIV, 3, 0, 2},   {NT_EXPR_MULDIV, 3, 0, 2},
    {NT_EXPR_MULDIV, 3, 0, 2},   {NT_EXPR_UNARY, 1, 0, -1},
    {NT_EXPR_UNARY, 2, 1, -1},   {NT_EXPR_UNARY, 2, 1, -1},
    {NT_EXPR_UNARY, 2, 1, -1},   {NT_TERM, 1, 0, -1},
    {NT_TERM, 1, 0, -1},         {NT_TERM, 1, 0, -1},
    {NT_TERM, 3, 1, -1},         {NT_FUNCALL, 4, 0, 2},
    {NT_IDLIST_OPT, 0, -1, -1},  {NT_IDLIST_OPT, 1, 0, -1},
    {NT_IDLIST, 1, 0, -1},       {NT_IDLIST, 3, 0, 2},
    {NT_ARGLIST_OPT, 0, -1, -1}, {NT_ARGLIST_OPT, 1, 0, -1},
    {NT_ARGLIST, 1, 0, -1},      {NT_ARGLIST, 3, 0, 2}};

static int state_stack[1024];
static symb_t ast_stack[1024];
static int stack_len;

void init_slr_svar() {
    stack_len = 1;
    state_stack[0] = 1;
}

void init_slr_expr() {
    stack_len = 1;
    state_stack[0] = 16;
}

static symb_t mem[2048];
static symb_t *mem_p = mem;

void clear_slr_mem() { mem_p = mem; }

#define SLR_DIE(msg)                 \
    do {                             \
        mc_puts("PARSER ERR: " msg); \
        return 1;                    \
    } while (0)

int slr_feed_token(token_t *tok) {
    int next = (int)slr_table[state_stack[stack_len - 1]][tok->type];
    if (next == 0) SLR_DIE("unexpected token");
    while (next < 0) {
        // reduce
        ruledef_entry rule = rules[~next];
        int ntokens = (int)rule.ntokens;
        if (stack_len - 1 < ntokens) {
            SLR_DIE("internal error");
        }
        symb_t newsymb;
        newsymb.type = ~next;
        int arg1pos = (int)rule.arg1pos;
        int arg2pos = (int)rule.arg2pos;
        if (arg1pos >= 0) {
            newsymb.arg1 = mem_p;
            *(mem_p++) = ast_stack[stack_len - ntokens + arg1pos];
        }
        if (arg2pos >= 0) {
            newsymb.arg2 = mem_p;
            *(mem_p++) = ast_stack[stack_len - ntokens + arg2pos];
        }
        stack_len -= ntokens;
        ast_stack[stack_len] = newsymb;
        state_stack[stack_len] = slr_table[state_stack[stack_len - 1]][rule.nt];
        stack_len++;
        next = (int)slr_table[state_stack[stack_len - 1]][tok->type];
    }
    // shift
    ast_stack[stack_len].type = -1;
    ast_stack[stack_len].token = *tok;
    state_stack[stack_len] = next;
    stack_len++;
    return 0;
}

symb_t *slr_get_result() {
    if (stack_len != 3) {
        return NULL;
    }
    if (ast_stack[2].type != -1 || ast_stack[2].token.type != TOK_EOS) {
        return NULL;
    }
    if (state_stack[0] == 1 && ast_stack[1].type == RL_STMT_SETVAR) {
        return &ast_stack[1];
    }
    if (state_stack[0] == 16 && ast_stack[1].type == RL_STMT_EXPR) {
        return &ast_stack[1];
    }
    return NULL;
}
