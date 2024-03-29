/* This file is autogenerated by gen/rtype-prop; DO NOT EDIT. */

#include "rtype.h"
#include "rune.h"

#include "internal/common.h"

/* clang-format off */

#if BIT_LOOKUP
static const unsigned _BitInt(LATIN1_MAX + 1) mask =
	0x00000000000000000000000000000000000000000000000003FF040800000000uwb;
#endif

static const struct {
	rune lo, hi;
} lookup_tbl[] = {
	{RUNE_C(0x000023), RUNE_C(0x000023)},
	{RUNE_C(0x00002A), RUNE_C(0x00002A)},
	{RUNE_C(0x000030), RUNE_C(0x000039)},
	{RUNE_C(0x00200D), RUNE_C(0x00200D)},
	{RUNE_C(0x0020E3), RUNE_C(0x0020E3)},
	{RUNE_C(0x00FE0F), RUNE_C(0x00FE0F)},
	{RUNE_C(0x01F1E6), RUNE_C(0x01F1FF)},
	{RUNE_C(0x01F3FB), RUNE_C(0x01F3FF)},
	{RUNE_C(0x01F9B0), RUNE_C(0x01F9B3)},
	{RUNE_C(0x0E0020), RUNE_C(0x0E007F)},
};

#define TYPE      bool
#define TABLE     lookup_tbl
#define DEFAULT   false
#define HAS_VALUE 0
#include "internal/rtype/lookup-func.h"

bool
rprop_is_ecomp(rune ch)
{
	return
#if BIT_LOOKUP
		ch <= LATIN1_MAX ? (mask & (1 << ch)) :
#endif
		lookup(ch);
}
