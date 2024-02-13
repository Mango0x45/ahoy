#ifndef AHOY_COMMON_CERR_H
#define AHOY_COMMON_CERR_H

#include <stddef.h>

void cerrinit(const char *);

[[noreturn, gnu::nonnull, gnu::format(printf, 1, 2)]]
void die(const char *, ...);

[[noreturn, gnu::nonnull, gnu::format(printf, 1, 2)]]
void diex(const char *, ...);

[[noreturn, gnu::nonnull, gnu::format(printf, 3, 4)]]
void die_with_off(const char *, size_t, const char *, ...);

#endif /* !AHOY_COMMON_CERR_H */
