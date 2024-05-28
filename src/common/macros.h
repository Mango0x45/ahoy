#ifndef AHOY_COMMON_MACROS_H
#define AHOY_COMMON_MACROS_H

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define lengthof(a)    (sizeof(a) / sizeof(*(a)))
#define memeq(x, y, n) (!memcmp(x, y, n))
#define streq(x, y)    (!strcmp(x, y))
#define u8eq(x, y)     (!u8cmp(x, y))

#if DEBUG
#	include "cerr.h"
#	define assume(C)                                                          \
		((C) ? (void)0                                                         \
		     : diex("%s:%d: %s(): assumption ‘%s’ failed", __FILE__, __LINE__, \
		            __func__, #C))
#else
#	define assume(C) ((C) ? (void)0 : unreachable())
#endif

#endif /* !AHOY_COMMON_MACROS_H */
