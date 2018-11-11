/*
 * main.c
 */

#include "calc.h"
#include "io.h"
#include "lexer.h"
#include "parser.h"
#include "strutils.h"

#define BUFSIZE 1024

static void show_caret(size_t pos) {
    while (pos-- > 0) {
        mc_putchar(' ');
    }
    mc_putchar('^');
    mc_putchar('\n');
}

static char buf[BUFSIZE];

int main() {
    while (1) {
        mc_putchar('>');
        mc_getsn(buf, BUFSIZE);
        int is_svar;
        if (mc_strstr(buf, ":=") != NULL) {
            is_svar = 1;
            init_slr_svar();
        } else {
            is_svar = 0;
            init_slr_expr();
        }
        token_t tok;
        const char *bufptr = buf;
        int is_empty = 1;
        while (1) {
            if (get_next_tok(&tok, &bufptr) != 0) {
                mc_print(buf);
                show_caret((size_t)(bufptr - buf));
                break;
            }
            if (tok.type == TOK_EOS && is_empty) {
                break;
            }
            is_empty = 0;
            if (slr_feed_token(&tok) != 0) {
                mc_print(buf);
                show_caret((size_t)(bufptr - buf));
                break;
            }
            if (tok.type == TOK_EOS) {
                symb_t *symb = slr_get_result();
                if (symb == NULL) {
                    mc_puts("internal error");
                    break;
                }
                if (is_svar) {
                    if (do_svar(symb, buf) != 0) {
                        // error
                        break;
                    }
                } else {
                    int ans;
                    if (do_eval(&ans, symb) != 0) {
                        // error
                        break;
                    }
                    print_int(ans);
                    mc_putchar('\n');
                }
                break;
            }
        }
        clear_slr_mem();
    }
}
