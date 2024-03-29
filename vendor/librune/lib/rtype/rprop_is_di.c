/* This file is autogenerated by gen/rtype-prop; DO NOT EDIT. */

#include "rtype.h"
#include "rune.h"

#include "internal/common.h"

/* clang-format off */

#if BIT_LOOKUP
static const unsigned _BitInt(LATIN1_MAX + 1) mask =
	0x0000000000000000000020000000000000000000000000000000000000000000uwb;
#endif

static const struct {
	rune lo, hi;
} lookup_tbl[] = {
	{RUNE_C(0x0000AD), RUNE_C(0x0000AD)},
	{RUNE_C(0x00034F), RUNE_C(0x00034F)},
	{RUNE_C(0x00061C), RUNE_C(0x00061C)},
	{RUNE_C(0x00115F), RUNE_C(0x001160)},
	{RUNE_C(0x0017B4), RUNE_C(0x0017B5)},
	{RUNE_C(0x00180B), RUNE_C(0x00180F)},
	{RUNE_C(0x00200B), RUNE_C(0x00200F)},
	{RUNE_C(0x00202A), RUNE_C(0x00202E)},
	{RUNE_C(0x002060), RUNE_C(0x00206F)},
	{RUNE_C(0x003164), RUNE_C(0x003164)},
	{RUNE_C(0x00FE00), RUNE_C(0x00FE0F)},
	{RUNE_C(0x00FEFF), RUNE_C(0x00FEFF)},
	{RUNE_C(0x00FFA0), RUNE_C(0x00FFA0)},
	{RUNE_C(0x00FFF0), RUNE_C(0x00FFF8)},
	{RUNE_C(0x01BCA0), RUNE_C(0x01BCA3)},
	{RUNE_C(0x01D173), RUNE_C(0x01D17A)},
	{RUNE_C(0x0E0000), RUNE_C(0x0E0FFF)},
};

#define TYPE      bool
#define TABLE     lookup_tbl
#define DEFAULT   false
#define HAS_VALUE 0
#include "internal/rtype/lookup-func.h"

bool
rprop_is_di(rune ch)
{
	return
#if BIT_LOOKUP
		ch <= LATIN1_MAX ? (mask & (1 << ch)) :
#endif
		lookup(ch);
}
