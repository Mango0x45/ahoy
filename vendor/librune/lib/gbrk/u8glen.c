#include "gbrk.h"

size_t
u8glen(const char8_t *s, size_t n)
{
	size_t m = 0;
	struct u8view unused;

	while (u8gnext(&unused, &s, &n))
		m++;

	return m;
}
