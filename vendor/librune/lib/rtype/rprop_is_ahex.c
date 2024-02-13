#include "rtype.h"

#include "internal/common.h"

static const bool lookup[] = {
	[0x30] = true, [0x31] = true, [0x32] = true, [0x33] = true, [0x34] = true,
	[0x35] = true, [0x36] = true, [0x37] = true, [0x38] = true, [0x39] = true,
	[0x41] = true, [0x42] = true, [0x43] = true, [0x44] = true, [0x45] = true,
	[0x46] = true, [0x61] = true, [0x62] = true, [0x63] = true, [0x64] = true,
	[0x65] = true, [0x66] = true,
};

bool
rprop_is_ahex(rune ch)
{
	return ch <= lengthof(lookup) ? lookup[ch] : false;
}
