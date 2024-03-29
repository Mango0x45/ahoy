/* This file is autogenerated by gen/rtype-prop; DO NOT EDIT. */

#include "rtype.h"
#include "rune.h"

#include "internal/common.h"

/* clang-format off */

#if BIT_LOOKUP
static const unsigned _BitInt(LATIN1_MAX + 1) mask =
	0x0000000000000000000000000000000000000000000000008C00500200000000uwb;
#endif

static const struct {
	rune lo, hi;
} lookup_tbl[] = {
	{RUNE_C(0x000021), RUNE_C(0x000021)},
	{RUNE_C(0x00002C), RUNE_C(0x00002C)},
	{RUNE_C(0x00002E), RUNE_C(0x00002E)},
	{RUNE_C(0x00003A), RUNE_C(0x00003B)},
	{RUNE_C(0x00003F), RUNE_C(0x00003F)},
	{RUNE_C(0x00037E), RUNE_C(0x00037E)},
	{RUNE_C(0x000387), RUNE_C(0x000387)},
	{RUNE_C(0x000589), RUNE_C(0x000589)},
	{RUNE_C(0x0005C3), RUNE_C(0x0005C3)},
	{RUNE_C(0x00060C), RUNE_C(0x00060C)},
	{RUNE_C(0x00061B), RUNE_C(0x00061B)},
	{RUNE_C(0x00061D), RUNE_C(0x00061F)},
	{RUNE_C(0x0006D4), RUNE_C(0x0006D4)},
	{RUNE_C(0x000700), RUNE_C(0x00070A)},
	{RUNE_C(0x00070C), RUNE_C(0x00070C)},
	{RUNE_C(0x0007F8), RUNE_C(0x0007F9)},
	{RUNE_C(0x000830), RUNE_C(0x00083E)},
	{RUNE_C(0x00085E), RUNE_C(0x00085E)},
	{RUNE_C(0x000964), RUNE_C(0x000965)},
	{RUNE_C(0x000E5A), RUNE_C(0x000E5B)},
	{RUNE_C(0x000F08), RUNE_C(0x000F08)},
	{RUNE_C(0x000F0D), RUNE_C(0x000F12)},
	{RUNE_C(0x00104A), RUNE_C(0x00104B)},
	{RUNE_C(0x001361), RUNE_C(0x001368)},
	{RUNE_C(0x00166E), RUNE_C(0x00166E)},
	{RUNE_C(0x0016EB), RUNE_C(0x0016ED)},
	{RUNE_C(0x001735), RUNE_C(0x001736)},
	{RUNE_C(0x0017D4), RUNE_C(0x0017D6)},
	{RUNE_C(0x0017DA), RUNE_C(0x0017DA)},
	{RUNE_C(0x001802), RUNE_C(0x001805)},
	{RUNE_C(0x001808), RUNE_C(0x001809)},
	{RUNE_C(0x001944), RUNE_C(0x001945)},
	{RUNE_C(0x001AA8), RUNE_C(0x001AAB)},
	{RUNE_C(0x001B5A), RUNE_C(0x001B5B)},
	{RUNE_C(0x001B5D), RUNE_C(0x001B5F)},
	{RUNE_C(0x001B7D), RUNE_C(0x001B7E)},
	{RUNE_C(0x001C3B), RUNE_C(0x001C3F)},
	{RUNE_C(0x001C7E), RUNE_C(0x001C7F)},
	{RUNE_C(0x00203C), RUNE_C(0x00203D)},
	{RUNE_C(0x002047), RUNE_C(0x002049)},
	{RUNE_C(0x002E2E), RUNE_C(0x002E2E)},
	{RUNE_C(0x002E3C), RUNE_C(0x002E3C)},
	{RUNE_C(0x002E41), RUNE_C(0x002E41)},
	{RUNE_C(0x002E4C), RUNE_C(0x002E4C)},
	{RUNE_C(0x002E4E), RUNE_C(0x002E4F)},
	{RUNE_C(0x002E53), RUNE_C(0x002E54)},
	{RUNE_C(0x003001), RUNE_C(0x003002)},
	{RUNE_C(0x00A4FE), RUNE_C(0x00A4FF)},
	{RUNE_C(0x00A60D), RUNE_C(0x00A60F)},
	{RUNE_C(0x00A6F3), RUNE_C(0x00A6F7)},
	{RUNE_C(0x00A876), RUNE_C(0x00A877)},
	{RUNE_C(0x00A8CE), RUNE_C(0x00A8CF)},
	{RUNE_C(0x00A92F), RUNE_C(0x00A92F)},
	{RUNE_C(0x00A9C7), RUNE_C(0x00A9C9)},
	{RUNE_C(0x00AA5D), RUNE_C(0x00AA5F)},
	{RUNE_C(0x00AADF), RUNE_C(0x00AADF)},
	{RUNE_C(0x00AAF0), RUNE_C(0x00AAF1)},
	{RUNE_C(0x00ABEB), RUNE_C(0x00ABEB)},
	{RUNE_C(0x00FE50), RUNE_C(0x00FE52)},
	{RUNE_C(0x00FE54), RUNE_C(0x00FE57)},
	{RUNE_C(0x00FF01), RUNE_C(0x00FF01)},
	{RUNE_C(0x00FF0C), RUNE_C(0x00FF0C)},
	{RUNE_C(0x00FF0E), RUNE_C(0x00FF0E)},
	{RUNE_C(0x00FF1A), RUNE_C(0x00FF1B)},
	{RUNE_C(0x00FF1F), RUNE_C(0x00FF1F)},
	{RUNE_C(0x00FF61), RUNE_C(0x00FF61)},
	{RUNE_C(0x00FF64), RUNE_C(0x00FF64)},
	{RUNE_C(0x01039F), RUNE_C(0x01039F)},
	{RUNE_C(0x0103D0), RUNE_C(0x0103D0)},
	{RUNE_C(0x010857), RUNE_C(0x010857)},
	{RUNE_C(0x01091F), RUNE_C(0x01091F)},
	{RUNE_C(0x010A56), RUNE_C(0x010A57)},
	{RUNE_C(0x010AF0), RUNE_C(0x010AF5)},
	{RUNE_C(0x010B3A), RUNE_C(0x010B3F)},
	{RUNE_C(0x010B99), RUNE_C(0x010B9C)},
	{RUNE_C(0x010F55), RUNE_C(0x010F59)},
	{RUNE_C(0x010F86), RUNE_C(0x010F89)},
	{RUNE_C(0x011047), RUNE_C(0x01104D)},
	{RUNE_C(0x0110BE), RUNE_C(0x0110C1)},
	{RUNE_C(0x011141), RUNE_C(0x011143)},
	{RUNE_C(0x0111C5), RUNE_C(0x0111C6)},
	{RUNE_C(0x0111CD), RUNE_C(0x0111CD)},
	{RUNE_C(0x0111DE), RUNE_C(0x0111DF)},
	{RUNE_C(0x011238), RUNE_C(0x01123C)},
	{RUNE_C(0x0112A9), RUNE_C(0x0112A9)},
	{RUNE_C(0x01144B), RUNE_C(0x01144D)},
	{RUNE_C(0x01145A), RUNE_C(0x01145B)},
	{RUNE_C(0x0115C2), RUNE_C(0x0115C5)},
	{RUNE_C(0x0115C9), RUNE_C(0x0115D7)},
	{RUNE_C(0x011641), RUNE_C(0x011642)},
	{RUNE_C(0x01173C), RUNE_C(0x01173E)},
	{RUNE_C(0x011944), RUNE_C(0x011944)},
	{RUNE_C(0x011946), RUNE_C(0x011946)},
	{RUNE_C(0x011A42), RUNE_C(0x011A43)},
	{RUNE_C(0x011A9B), RUNE_C(0x011A9C)},
	{RUNE_C(0x011AA1), RUNE_C(0x011AA2)},
	{RUNE_C(0x011C41), RUNE_C(0x011C43)},
	{RUNE_C(0x011C71), RUNE_C(0x011C71)},
	{RUNE_C(0x011EF7), RUNE_C(0x011EF8)},
	{RUNE_C(0x011F43), RUNE_C(0x011F44)},
	{RUNE_C(0x012470), RUNE_C(0x012474)},
	{RUNE_C(0x016A6E), RUNE_C(0x016A6F)},
	{RUNE_C(0x016AF5), RUNE_C(0x016AF5)},
	{RUNE_C(0x016B37), RUNE_C(0x016B39)},
	{RUNE_C(0x016B44), RUNE_C(0x016B44)},
	{RUNE_C(0x016E97), RUNE_C(0x016E98)},
	{RUNE_C(0x01BC9F), RUNE_C(0x01BC9F)},
	{RUNE_C(0x01DA87), RUNE_C(0x01DA8A)},
};

#define TYPE      bool
#define TABLE     lookup_tbl
#define DEFAULT   false
#define HAS_VALUE 0
#include "internal/rtype/lookup-func.h"

bool
rprop_is_term(rune ch)
{
	return
#if BIT_LOOKUP
		ch <= LATIN1_MAX ? (mask & (1 << ch)) :
#endif
		lookup(ch);
}
