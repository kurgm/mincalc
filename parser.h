/*
 * parser.h
 */

#ifndef MINCALC_PARSER_H
#define MINCALC_PARSER_H

#include "lexer.h"

enum nonterminal {
    NT_STMT = NTOKTYPE,
    NT_SETVAR,
    NT_ASSIGN,
    NT_FUNDEF,
    NT_EXPR_OR,
    NT_EXPR_XOR,
    NT_EXPR_AND,
    NT_EXPR_EQ,
    NT_EXPR_CMP,
    NT_EXPR_SHIFT,
    NT_EXPR_ADDSUB,
    NT_EXPR_MULDIV,
    NT_EXPR_UNARY,
    NT_TERM,
    NT_FUNCALL,
    NT_IDLIST_OPT,
    NT_IDLIST,
    NT_ARGLIST_OPT,
    NT_ARGLIST,
};
#define NSYMBOL (NT_ARGLIST + 1)

enum grules {
    RL_STMT_SETVAR,
    RL_STMT_EXPR,
    RL_SETVAR_ASSIGN,
    RL_SETVAR_FUNDEF,
    RL_ASSIGN,
    RL_FUNDEF,
    RL_EXPR_OR,
    RL_OR,
    RL_EXPR_XOR,
    RL_XOR,
    RL_EXPR_AND,
    RL_AND,
    RL_EXPR_EQ,
    RL_EQ,
    RL_NEQ,
    RL_EXPR_CMP,
    RL_LT,
    RL_LEQ,
    RL_GT,
    RL_GEQ,
    RL_EXPR_SHIFT,
    RL_LL,
    RL_GG,
    RL_GGG,
    RL_EXPR_ADDSUB,
    RL_ADD,
    RL_SUB,
    RL_EXPR_MULDIV,
    RL_MUL,
    RL_DIV,
    RL_MOD,
    RL_EXPR_UNARY,
    RL_NOT,
    RL_UPLUS,
    RL_UMINUS,
    RL_TERM_INT,
    RL_TERM_ID,
    RL_TERM_FUNCALL,
    RL_TERM_GROUP,
    RL_FUNCALL,
    RL_IDLIST_OPT_0,
    RL_IDLIST_OPT_1,
    RL_IDLIST,
    RL_IDLIST_CONS,
    RL_ARGLIST_OPT_0,
    RL_ARGLIST_OPT_1,
    RL_ARGLIST,
    RL_ARGLIST_CONS,
};
#define NRULES (RL_ARGLIST_CONS + 1)

typedef struct _symb_t {
    int type;
    union {
        token_t token;
        struct {
            struct _symb_t *arg1;
            struct _symb_t *arg2;
        };
    };
} symb_t;

void init_slr_svar(void);
void init_slr_expr(void);

void clear_slr_mem(void);

int slr_feed_token(token_t *tok);
symb_t *slr_get_result(void);

#endif /* MINCALC_PARSER_H */
