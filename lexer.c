/*
 * lexer.c
 */

#include "io.h"
#include "strutils.h"

#include "lexer.h"

#define LEX_DIE(msg)                \
    do {                            \
        mc_puts("LEXER ERR: " msg); \
        return 1;                   \
    } while (0)

int get_next_tok(token_t *tok, const char **str) {
    static char buf[11];
    char c = **str;
    while (c == ' ' || c == '\n') {
        c = *(++*str);
    }
    if ('0' <= c && c <= '9') {
        int ok = 0;
        int i;
        for (i = 0; i < 10; i++) {
            buf[i] = c;
            c = *(++*str);
            if (!('0' <= c && c <= '9')) {
                ok = 1;
                break;
            }
        }
        if (!ok) LEX_DIE("number literal too long");
        buf[i] = '\0';
        tok->type = TOK_NUM;
        tok->num = mc_atoi(buf);
    } else if ('a' <= c && c <= 'z') {
        int ok = 0;
        tok->type = TOK_ID;
        int i;
        for (i = 0; i < 4; i++) {
            tok->idname[i] = c;
            c = *(++*str);
            if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'z'))) {
                ok = 1;
                break;
            }
        }
        if (!ok) LEX_DIE("identifier too long");
        for (; i < 4; i++) {
            tok->idname[i] = '\0';
        }
    } else {
        switch (c) {
            case ':':
                if (*(++*str) == '=') {
                    ++*str;
                    tok->type = TOK_DEFEQ;
                } else
                    LEX_DIE("expected '='");
                break;
            case '(':
                ++*str;
                tok->type = TOK_LPAR;
                break;
            case ')':
                ++*str;
                tok->type = TOK_RPAR;
                break;
            case ',':
                ++*str;
                tok->type = TOK_COMMA;
                break;
            case '+':
                ++*str;
                tok->type = TOK_PLUS;
                break;
            case '-':
                ++*str;
                tok->type = TOK_MINUS;
                break;
            case '*':
                ++*str;
                tok->type = TOK_MUL;
                break;
            case '/':
                ++*str;
                tok->type = TOK_DIV;
                break;
            case '%':
                ++*str;
                tok->type = TOK_MOD;
                break;
            case '&':
                ++*str;
                tok->type = TOK_AND;
                break;
            case '|':
                ++*str;
                tok->type = TOK_OR;
                break;
            case '^':
                ++*str;
                tok->type = TOK_XOR;
                break;
            case '~':
                ++*str;
                tok->type = TOK_NOT;
                break;
            case '=':
                if (*(++*str) == '=') {
                    ++*str;
                    tok->type = TOK_EQ;
                } else
                    LEX_DIE("expected '='");
                break;
            case '<':
                c = *(++*str);
                if (c == '=') {
                    ++*str;
                    tok->type = TOK_LEQ;
                } else if (c == '<') {
                    ++*str;
                    tok->type = TOK_LL;
                } else if (c == '>') {
                    ++*str;
                    tok->type = TOK_NEQ;
                } else {
                    tok->type = TOK_LT;
                }
                break;
            case '>':
                c = *(++*str);
                if (c == '=') {
                    ++*str;
                    tok->type = TOK_GEQ;
                } else if (c == '>') {
                    c = *(++*str);
                    if (c == '>') {
                        ++*str;
                        tok->type = TOK_GGG;
                    } else {
                        tok->type = TOK_GG;
                    }
                } else {
                    tok->type = TOK_GT;
                }
                break;
            case '\0':
                tok->type = TOK_EOS;
                break;
            default:
                LEX_DIE("invalid character");
        }
    }
    return 0;
}
