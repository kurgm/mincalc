/*
 * calc.h
 */

#ifndef MINCALC_CALC_H
#define MINCALC_CALC_H

#include <stddef.h>

#include "parser.h"

typedef struct {
    char name[4];
    int val;
    void *fundef;
} var_entry_t;

var_entry_t *lookup_var(const char *name);
var_entry_t *create_var(const char *name);
var_entry_t *get_or_create_var(const char *name);

var_entry_t *lookup_var_ctx(const char *name, var_entry_t *ctx,
                            size_t ctx_size);

int do_svar(const symb_t *symb, const char *input);
int do_eval(int *result, const symb_t *symb);
int do_eval_ctx(int *result, const symb_t *symb, var_entry_t *ctx,
                size_t ctx_size);
int call_function(int *result, const char *fundef_str, symb_t *arglist_opt,
                  var_entry_t *ctx, size_t ctx_size);

#endif /* MINCALC_CALC_H */
