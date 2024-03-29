/* This file is autogenerated by gen/rtype-prop; DO NOT EDIT. */

#include "rtype.h"
#include "rune.h"

#include "internal/common.h"

/* clang-format off */

#if BIT_LOOKUP
static const unsigned _BitInt(LATIN1_MAX + 1) mask =
	0x0000000000000000000000000000000000000000000000000000200000000000uwb;
#endif

static const struct {
	rune lo, hi;
} lookup_tbl[] = {
	{RUNE_C(0x00002D), RUNE_C(0x00002D)},
	{RUNE_C(0x00058A), RUNE_C(0x00058A)},
	{RUNE_C(0x0005BE), RUNE_C(0x0005BE)},
	{RUNE_C(0x001400), RUNE_C(0x001400)},
	{RUNE_C(0x001806), RUNE_C(0x001806)},
	{RUNE_C(0x002010), RUNE_C(0x002015)},
	{RUNE_C(0x002053), RUNE_C(0x002053)},
	{RUNE_C(0x00207B), RUNE_C(0x00207B)},
	{RUNE_C(0x00208B), RUNE_C(0x00208B)},
	{RUNE_C(0x002212), RUNE_C(0x002212)},
	{RUNE_C(0x002E17), RUNE_C(0x002E17)},
	{RUNE_C(0x002E1A), RUNE_C(0x002E1A)},
	{RUNE_C(0x002E3A), RUNE_C(0x002E3B)},
	{RUNE_C(0x002E40), RUNE_C(0x002E40)},
	{RUNE_C(0x002E5D), RUNE_C(0x002E5D)},
	{RUNE_C(0x00301C), RUNE_C(0x00301C)},
	{RUNE_C(0x003030), RUNE_C(0x003030)},
	{RUNE_C(0x0030A0), RUNE_C(0x0030A0)},
	{RUNE_C(0x00FE31), RUNE_C(0x00FE32)},
	{RUNE_C(0x00FE58), RUNE_C(0x00FE58)},
	{RUNE_C(0x00FE63), RUNE_C(0x00FE63)},
	{RUNE_C(0x00FF0D), RUNE_C(0x00FF0D)},
	{RUNE_C(0x010EAD), RUNE_C(0x010EAD)},
};

#define TYPE      bool
#define TABLE     lookup_tbl
#define DEFAULT   false
#define HAS_VALUE 0
#include "internal/rtype/lookup-func.h"

bool
rprop_is_dash(rune ch)
{
	return
#if BIT_LOOKUP
		ch <= LATIN1_MAX ? (mask & (1 << ch)) :
#endif
		lookup(ch);
}
