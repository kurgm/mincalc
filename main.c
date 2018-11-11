/*
 * main.c
 */

#include "calc.h"
#include "io.h"
#include "lexer.h"
#include "parser.h"
#include "strutils.h"

#define BUFSIZE 1024

static void show_caret(int pos) {
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
        if (mc_strstr(buf, ":=") != NULL) {
            init_slr_svar();
        } else {
            init_slr_expr();
        }
        token_t tok;
        const char *bufptr = buf;
        while (1) {
            if (get_next_tok(&tok, &bufptr) != 0) {
                mc_print(buf);
                show_caret(bufptr - buf);
                break;
            }
            if (slr_feed_token(&tok) != 0) {
                mc_print(buf);
                show_caret(bufptr - buf);
                break;
            }
            if (tok.type == TOK_EOS) {
                symb_t *symb = slr_get_result();
                if (symb == NULL) {
                    mc_puts("internal error");
                    break;
                }
                int ans = do_calc(symb);
                print_int(ans);
                mc_putchar('\n');
                break;
            }
        }
        clear_slr_mem();
    }
    return 0;
}
