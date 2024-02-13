#ifndef AHOY_C8ASM_COMMON_H
#define AHOY_C8ASM_COMMON_H

#include <rune.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define lengthof(a)    (sizeof(a) / sizeof(*(a)))
#define memeq(x, y, n) (!memcmp(x, y, n))
#define streq(x, y)    (!strcmp(x, y))
#define u8eq(x, y)     (!u8cmp(x, y))

extern size_t filesize;
extern const char *filename;
extern const char8_t *baseptr;

#endif /* !AHOY_C8ASM_COMMON_H */
