/*
 * calc.c
 */

#include <stdint.h>

#include "io.h"
#include "lexer.h"
#include "strutils.h"

#include "calc.h"

#define CALC_DIE(msg)              \
    do {                           \
        mc_puts("CALC ERR: " msg); \
        return 1;                  \
    } while (0)

#define NAME_AS_INT(name) (*(int32_t *)(uintptr_t)name)

#define CALC_VAR_SIZE 128
static var_entry_t vars[CALC_VAR_SIZE];

var_entry_t *lookup_var(const char *name) {
    return lookup_var_ctx(name, vars, CALC_VAR_SIZE);
}

var_entry_t *lookup_var_ctx(const char *name, var_entry_t *ctx,
                            size_t ctx_size) {
    for (size_t i = 0; i < ctx_size; i++) {
        if (NAME_AS_INT(ctx[i].name) == NAME_AS_INT(name)) {
            return &ctx[i];
        }
    }
    return NULL;
}

var_entry_t *create_var(const char *name) {
    int32_t n = 0;
    var_entry_t *e = lookup_var((char *)&n);
    if (e == NULL) {
        return NULL;
    }
    NAME_AS_INT(e->name) = NAME_AS_INT(name);
    e->fundef = NULL;
    return e;
}

var_entry_t *get_or_create_var(const char *name) {
    var_entry_t *e = lookup_var(name);
    if (e == NULL) {
        e = create_var(name);
    }
    return e;
}

#define CALC_FUNDEF_BUFSIZE 2048
static char fundefs[CALC_FUNDEF_BUFSIZE];
static char *fundefs_p = fundefs;

int do_svar(const symb_t *symb, const char *input) {
#ifndef NDEBUG
    if (symb->type != ~RL_STMT_SETVAR) CALC_DIE("not a statement");
#endif
    symb = symb->arg1;
    if (symb->type == ~RL_SETVAR_ASSIGN) {
#ifndef NDEBUG
        if (symb->arg1->type != ~RL_ASSIGN) CALC_DIE("not an assignment");
#endif
        const symb_t *id = symb->arg1->arg1;
#ifndef NDEBUG
        if (id->token.type != TOK_ID) CALC_DIE("assignment lhs not an id");
#endif
        var_entry_t *e = get_or_create_var(id->token.idname);
        if (e == NULL) CALC_DIE("ran out of variable space");
        return do_eval(&e->val, symb->arg1->arg2);
    }
    if (symb->type == ~RL_SETVAR_FUNDEF) {
#ifndef NDEBUG
        if (symb->arg1->type != ~RL_FUNDEF) CALC_DIE("not a fundef");
#endif
        const symb_t *id = symb->arg1->arg1;
#ifndef NDEBUG
        if (id->token.type != TOK_ID) CALC_DIE("fundef lhs id not an id");
#endif
        var_entry_t *e = get_or_create_var(id->token.idname);
        if (e == NULL) CALC_DIE("ran out of variable space");
        size_t len = mc_strlen(input);
        if (fundefs_p + len + 1 > fundefs + CALC_FUNDEF_BUFSIZE) {
            CALC_DIE("ran out of fundef buffer");
        }
        e->fundef = fundefs_p;
        mc_strcpy(fundefs_p, input);
        fundefs_p += len + 1;
        return 0;
    }
#ifndef NDEBUG
    CALC_DIE("not assign nor fundef");
#endif
    return 1;
}

int do_eval(int *result, const symb_t *symb) {
    return do_eval_ctx(result, symb, NULL, 0);
}

int do_eval_ctx(int *result, const symb_t *symb, var_entry_t *ctx,
                size_t ctx_size) {
    int arg1, arg2;
    if (symb == NULL) CALC_DIE("internal error");
    switch (symb->type) {
        case TOK_NUM:
            *result = symb->token.num;
            return 0;
        case TOK_ID: {
            var_entry_t *e;
            e = lookup_var_ctx(symb->token.idname, ctx, ctx_size);
            if (e == NULL) {
                e = lookup_var(symb->token.idname);
            }
            if (e == NULL) CALC_DIE("undefined variable");
            if (e->fundef != NULL) CALC_DIE("using function as a number");
            *result = e->val;
            return 0;
        }
        case ~RL_OR:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 | arg2;
            return 0;
        case ~RL_XOR:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 ^ arg2;
            return 0;
        case ~RL_AND:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 & arg2;
            return 0;
        case ~RL_EQ:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 == arg2;
            return 0;
        case ~RL_NEQ:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 != arg2;
            return 0;
        case ~RL_LT:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 < arg2;
            return 0;
        case ~RL_LEQ:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 <= arg2;
            return 0;
        case ~RL_GT:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 > arg2;
            return 0;
        case ~RL_GEQ:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 >= arg2;
            return 0;
        case ~RL_LL:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 << arg2;
            return 0;
        case ~RL_GG:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 >> arg2;
            return 0;
        case ~RL_GGG:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = (int)((unsigned int)arg1 >> arg2);
            return 0;
        case ~RL_ADD:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 + arg2;
            return 0;
        case ~RL_SUB:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 - arg2;
            return 0;
        case ~RL_MUL:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 * arg2;
            return 0;
        case ~RL_DIV:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 / arg2;
            return 0;
        case ~RL_MOD:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0 ||
                do_eval_ctx(&arg2, symb->arg2, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = arg1 % arg2;
            return 0;
        case ~RL_NOT:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = ~arg1;
            return 0;
        case ~RL_UPLUS:
            return do_eval_ctx(result, symb->arg1, ctx, ctx_size);
        case ~RL_UMINUS:
            if (do_eval_ctx(&arg1, symb->arg1, ctx, ctx_size) != 0) {
                return 1;
            }
            *result = -arg1;
            return 0;
        case ~RL_FUNCALL: {
#ifndef NDEBUG
            if (symb->arg1->token.type != TOK_ID)
                CALC_DIE("funcall id not an id");
#endif
            var_entry_t *e = lookup_var(symb->arg1->token.idname);
            if (e == NULL) CALC_DIE("undefined function");
            if (e->fundef == NULL) CALC_DIE("using number as function");
            return call_function(result, e->fundef, symb->arg2, ctx, ctx_size);
        }
        case ~RL_STMT_EXPR:
        case ~RL_EXPR_OR:
        case ~RL_EXPR_XOR:
        case ~RL_EXPR_AND:
        case ~RL_EXPR_EQ:
        case ~RL_EXPR_CMP:
        case ~RL_EXPR_SHIFT:
        case ~RL_EXPR_ADDSUB:
        case ~RL_EXPR_MULDIV:
        case ~RL_EXPR_UNARY:
        case ~RL_TERM_INT:
        case ~RL_TERM_ID:
        case ~RL_TERM_FUNCALL:
        case ~RL_TERM_GROUP:
            return do_eval_ctx(result, symb->arg1, ctx, ctx_size);
        default:
            CALC_DIE("unimplemented");
    }
}

int call_function(int *result, const char *fundef_str, symb_t *arglist_opt,
                  var_entry_t *ctx, size_t ctx_size) {
    size_t argc;
    if (arglist_opt->type == ~RL_ARGLIST_OPT_0) {
        argc = 0;
    } else if (arglist_opt->type == ~RL_ARGLIST_OPT_1) {
        argc = 1;
        symb_t *arg = arglist_opt->arg1;
        while (arg->type == ~RL_ARGLIST_CONS) {
            arg = arg->arg2;
            argc++;
        }
#ifndef NDEBUG
        if (arg->type != ~RL_ARGLIST) CALC_DIE("malformed arglist");
#endif
    } else
        CALC_DIE("arglist_opt not an arglist_opt");

    init_slr_svar();
    token_t tok;
    do {
        if (get_next_tok(&tok, &fundef_str) != 0)
            CALC_DIE("function lex error");
        if (slr_feed_token(&tok) != 0) CALC_DIE("function parse error");
    } while (tok.type != TOK_EOS);
    symb_t *fundef = slr_get_result();
    if (fundef == NULL) CALC_DIE("function parse internal error");
#ifndef NDEBUG
    if (fundef->type != ~RL_STMT_SETVAR) CALC_DIE("function definition error");
    if (fundef->arg1->type != ~RL_SETVAR_FUNDEF)
        CALC_DIE("function definition error 2");
    if (fundef->arg1->arg1->type != ~RL_FUNDEF)
        CALC_DIE("function definition error 3");
#endif
    symb_t *idlist_opt = fundef->arg1->arg1->arg2;
    size_t req_argc;
    if (idlist_opt->type == ~RL_IDLIST_OPT_0) {
        req_argc = 0;
    } else if (idlist_opt->type == ~RL_IDLIST_OPT_1) {
        req_argc = 1;
        symb_t *id = idlist_opt->arg1;
        while (id->type == ~RL_IDLIST_CONS) {
            id = id->arg2;
            req_argc++;
        }
#ifndef NDEBUG
        if (id->type != ~RL_IDLIST) CALC_DIE("malformed idlist");
#endif
    } else
        CALC_DIE("idlist_opt not an idlist_opt");

    if (argc != req_argc) CALC_DIE("wrong number of arguments");

    var_entry_t scope[argc];
    if (argc != 0) {
        int i = 0;
        symb_t *arg = arglist_opt->arg1;
        symb_t *id = idlist_opt->arg1;
        while (arg->type == ~RL_ARGLIST_CONS) {
#ifndef NDEBUG
            if (id->arg1->token.type != TOK_ID) CALC_DIE("idlist id not id");
#endif
            NAME_AS_INT(scope[i].name) = NAME_AS_INT(id->arg1->token.idname);
            if (do_eval_ctx(&scope[i].val, arg->arg1, ctx, ctx_size) != 0)
                return 1;
            scope[i].fundef = NULL;
            i++;
            arg = arg->arg2;
            id = id->arg2;
        }
#ifndef NDEBUG
        if (id->arg1->token.type != TOK_ID) CALC_DIE("idlist id not id 2");
#endif
        NAME_AS_INT(scope[i].name) = NAME_AS_INT(id->arg1->token.idname);
        if (do_eval_ctx(&scope[i].val, arg->arg1, ctx, ctx_size) != 0) return 1;
        scope[i].fundef = NULL;
    }
    return do_eval_ctx(result, fundef->arg1->arg1->arg3, scope, argc);
}
