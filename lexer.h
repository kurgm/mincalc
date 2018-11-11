/*
 * lexer.h
 */

#ifndef MINCALC_LEXER_H
#define MINCALC_LEXER_H

enum toktype {
    TOK_EOS,
    TOK_NUM,
    TOK_ID,
    TOK_DEFEQ,
    TOK_LPAR,
    TOK_RPAR,
    TOK_COMMA,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_MOD,
    TOK_AND,
    TOK_OR,
    TOK_XOR,
    TOK_NOT,
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_LEQ,
    TOK_GT,
    TOK_GEQ,
    TOK_LL,
    TOK_GG,
    TOK_GGG,
};

#define NTOKTYPE (TOK_GGG + 1)

typedef struct {
    enum toktype type;
    union {
        int num;
        char idname[4];
    };
} token_t;

int get_next_tok(token_t *tok, const char **str);

#endif /* MINCALC_LEXER_H */
