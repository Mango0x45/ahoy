#ifndef AHOY_C8ASM_COMMON_H
#define AHOY_C8ASM_COMMON_H

#include <mbstring.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define lengthof(a) (sizeof(a) / sizeof(*(a)))
#define streq(x, y) (!strcmp(x, y))

extern size_t filesize;
extern const char *filename;
extern const char8_t *baseptr;

#endif /* !AHOY_C8ASM_COMMON_H */
