/*
 * parser.c
 */

#include "strutils.h"

#include "parser.h"

/*

mincalc grammar
===============

statement ::= assignment | function-definition | expression
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
expr7 ::= term | "~" expr7 | "+" expr7 | "-" expr7
term ::= integer | identifier | func-call
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
