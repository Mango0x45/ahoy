#include <assert.h>

#include "rtype.h"
#include "rune.h"

#include "internal/common.h"

static const rune patws[] = {
	RUNE_C(0x0009), RUNE_C(0x000A), RUNE_C(0x000B), RUNE_C(0x000C),
	RUNE_C(0x000D), RUNE_C(0x0020), RUNE_C(0x0085), RUNE_C(0x200E),
	RUNE_C(0x200F), RUNE_C(0x2028), RUNE_C(0x2029),
};

static_assert(lengthof(patws) == 11, "Unroll pragma needs updating");

bool
rprop_is_pat_ws(rune ch)
{
#pragma GCC unroll 11
	for (size_t i = 0; i < lengthof(patws); i++) {
		if (patws[i] == ch)
			return true;
	}
	return false;
}
