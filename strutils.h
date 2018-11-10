/*
 * strutils.h
 */

#ifndef MINCALC_STRUTILS_H
#define MINCALC_STRUTILS_H

#include <stddef.h>

void *mc_memcpy(void *restrict dst, const void *restrict src, size_t n);
char *mc_strcpy(char *dst, const char *src);
char *mc_strncpy(char *dst, const char *src, size_t len);
size_t mc_strlen(const char *s);
int mc_strncmp(const char *s1, const char *s2, size_t n);

int mc_atoi(const char *str);
char *mc_itoa(int num, char *s);

#endif /* MINCALC_STRUTILS_H */
