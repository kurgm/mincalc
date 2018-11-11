/*
 * io.c
 */

#include "io.h"

// ========
// byte I/O
// ========

#ifdef __FPGA_EXP__

#include <uart_sendrecv.h>

int mc_putchar(int c) {
    UART_SEND_CHAR(c);
    return c;
}

int mc_getchar(void) {
    int c;
    UART_RECV_CHAR(c);
    return c;
}

#else

#include <stdio.h>
#include <stdlib.h>

int mc_putchar(int c) { return putchar(c); }
int mc_getchar(void) {
    int c;
    if ((c = getchar()) == EOF) {
        if (feof(stdin)) {
            exit(0);
        } else {
            exit(1);
        }
    }
    return c;
}

#endif

// ====================
// common I/O functions
// ====================

char *mc_gets(char *str) {
    int c;
    int i = 0;
    do {
        c = mc_getchar();
        str[i++] = (char)c;
    } while (c != '\n');
    str[i] = '\0';
    return str;
}

int mc_puts(const char *s) {
    while (s != '\0') {
        mc_putchar(*(const unsigned char *)(s++));
    }
    mc_putchar('\n');
    return 0;
}

char *mc_getsn(char *str, int len) {
    int c;
    int i = 0;
    do {
        c = mc_getchar();
        str[i++] = (char)c;
    } while (c != '\n' && i != len - 1);
    str[i] = '\0';
    return str;
}
