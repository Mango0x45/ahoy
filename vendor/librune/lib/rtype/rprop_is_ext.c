/* This file is autogenerated by gen/rtype-prop; DO NOT EDIT. */

#include "rtype.h"
#include "rune.h"

#include "internal/common.h"

/* clang-format off */

#if BIT_LOOKUP
static const unsigned _BitInt(LATIN1_MAX + 1) mask =
	0x0000000000000000008000000000000000000000000000000000000000000000uwb;
#endif

static const struct {
	rune lo, hi;
} lookup_tbl[] = {
	{RUNE_C(0x0000B7), RUNE_C(0x0000B7)},
	{RUNE_C(0x0002D0), RUNE_C(0x0002D1)},
	{RUNE_C(0x000640), RUNE_C(0x000640)},
	{RUNE_C(0x0007FA), RUNE_C(0x0007FA)},
	{RUNE_C(0x000B55), RUNE_C(0x000B55)},
	{RUNE_C(0x000E46), RUNE_C(0x000E46)},
	{RUNE_C(0x000EC6), RUNE_C(0x000EC6)},
	{RUNE_C(0x00180A), RUNE_C(0x00180A)},
	{RUNE_C(0x001843), RUNE_C(0x001843)},
	{RUNE_C(0x001AA7), RUNE_C(0x001AA7)},
	{RUNE_C(0x001C36), RUNE_C(0x001C36)},
	{RUNE_C(0x001C7B), RUNE_C(0x001C7B)},
	{RUNE_C(0x003005), RUNE_C(0x003005)},
	{RUNE_C(0x003031), RUNE_C(0x003035)},
	{RUNE_C(0x00309D), RUNE_C(0x00309E)},
	{RUNE_C(0x0030FC), RUNE_C(0x0030FE)},
	{RUNE_C(0x00A015), RUNE_C(0x00A015)},
	{RUNE_C(0x00A60C), RUNE_C(0x00A60C)},
	{RUNE_C(0x00A9CF), RUNE_C(0x00A9CF)},
	{RUNE_C(0x00A9E6), RUNE_C(0x00A9E6)},
	{RUNE_C(0x00AA70), RUNE_C(0x00AA70)},
	{RUNE_C(0x00AADD), RUNE_C(0x00AADD)},
	{RUNE_C(0x00AAF3), RUNE_C(0x00AAF4)},
	{RUNE_C(0x00FF70), RUNE_C(0x00FF70)},
	{RUNE_C(0x010781), RUNE_C(0x010782)},
	{RUNE_C(0x01135D), RUNE_C(0x01135D)},
	{RUNE_C(0x0115C6), RUNE_C(0x0115C8)},
	{RUNE_C(0x011A98), RUNE_C(0x011A98)},
	{RUNE_C(0x016B42), RUNE_C(0x016B43)},
	{RUNE_C(0x016FE0), RUNE_C(0x016FE1)},
	{RUNE_C(0x016FE3), RUNE_C(0x016FE3)},
	{RUNE_C(0x01E13C), RUNE_C(0x01E13D)},
	{RUNE_C(0x01E944), RUNE_C(0x01E946)},
};

#define TYPE      bool
#define TABLE     lookup_tbl
#define DEFAULT   false
#define HAS_VALUE 0
#include "internal/rtype/lookup-func.h"

bool
rprop_is_ext(rune ch)
{
	return
#if BIT_LOOKUP
		ch <= LATIN1_MAX ? (mask & (1 << ch)) :
#endif
		lookup(ch);
}
