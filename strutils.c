/*
 * strutils.c
 */

#include <stdint.h>

#include "io.h"

#include "strutils.h"

// =======================
// common string functions
// =======================

// void *mc_memcpy(void *restrict dst, const void *restrict src, size_t n) {
//     size_t i = 0;
//     for (; i != n; i++) {
//         ((char *restrict)dst)[i] = ((const char *restrict)src)[i];
//     }
//     return dst;
// }

char *mc_strcpy(char *dst, const char *src) {
    size_t i = 0;
    char c;
    do {
        c = src[i];
        dst[i] = c;
        i++;
    } while (c != '\0');
    return dst;
}

char *mc_strncpy(char *dst, const char *src, size_t len) {
    size_t i = 0;
    for (; i != len; i++) {
        char c = src[i];
        dst[i] = c;
        if (c == '\0') {
            while (++i != len) {
                dst[i] = '\0';
            }
            break;
        }
    }
    return dst;
}

size_t mc_strlen(const char *s) {
    size_t r = 0;
    while (*(s++)) {
        r++;
    }
    return r;
}

// int mc_strncmp(const char *s1, const char *s2, size_t n) {
//     const unsigned char *s1_ = (const unsigned char *)s1,
//                         *s2_ = (const unsigned char *)s2;
//     size_t i = 0;
//     while (s1_[i] == s2_[i] && s1_[i] != '\0' && i < n) {
//         i++;
//     }
//     return (int)(s1_[i] - s2_[i]);
// }

char *mc_strstr(const char *haystack, const char *needle) {
    if (*needle == '\0') {
        return (char *)(uintptr_t)haystack;
    }
    for (; *haystack != '\0'; haystack++) {
        for (int i = 0; needle[i] == haystack[i];) {
            if (needle[++i] == '\0') {
                return (char *)(uintptr_t)haystack;
            }
        }
    }
    return NULL;
}

// ======================================
// integer from/to decimal representation
// ======================================

int mc_atoi(const char *str) {
    int neg = 0;
    unsigned char c = *(const unsigned char *)str;
    if (c == '-') {
        neg = 1;
        str++;
    } else if (c == '+') {
        str++;
    }
    int n = 0;
    while (1) {
        c = *(const unsigned char *)(str++) - '0';
        if (c > 9) {
            break;
        }
        n = n * 10 + c;
    }
    if (neg) {
        return -n;
    }
    return n;
}

char *mc_itoa(int num, char *s) {
    static char buf[12];
    char *p = &buf[11];
    int neg = 0;
    if (num == 0) {
        *(--p) = '0';
    } else if (num < 0) {
        neg = 1;
        num = -num;
    }
    while (num != 0) {
        unsigned int quot = (unsigned int)num / 10,
                     rem = (unsigned int)num % 10;
        *(--p) = (char)('0' + rem);
        num = (int)quot;
    }
    if (neg) {
        *(--p) = '-';
    }
    return mc_strcpy(s, p);
}

int print_int(int num) {
    static char buf[12];
    mc_itoa(num, buf);
    return mc_print(buf);
}
