#include "rtype.h"
#include "rune.h"

bool
rprop_is_ri(rune ch)
{
	return ch >= RUNE_C(0x1F1E6) && ch <= RUNE_C(0x1F1FF);
}
