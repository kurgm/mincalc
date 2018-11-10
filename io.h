/*
 * io.h
 */

#ifndef MINCALC_IO_H
#define MINCALC_IO_H

int mc_putchar(int c);
int mc_getchar(void);

char *mc_gets(char *str);
int mc_puts(const char *s);
char *mc_getsn(char *str, int len);

#endif /* MINCALC_IO_H */
