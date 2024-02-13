#ifndef AHOY_COMMON_MACROS_H
#define AHOY_COMMON_MACROS_H

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define lengthof(a)    (sizeof(a) / sizeof(*(a)))
#define memeq(x, y, n) (!memcmp(x, y, n))
#define streq(x, y)    (!strcmp(x, y))
#define u8eq(x, y)     (!u8cmp(x, y))

#if DEBUG || !defined(unreachable)
#	if DEBUG
#		include "cerr.h"
#		ifdef unreachable
#			undef unreachable
#		endif
#		define unreachable() \
			diex("%s:%d: hit unreachable in %s()", __FILE__, __LINE__, __func__)
#	elifdef __clang__
#		define unreachable() __builtin_unreachable()
#	else
#		include <stddef.h>
#	endif
#endif

#endif /* !AHOY_COMMON_MACROS_H */
