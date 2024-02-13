#include "rtype.h"
#include "rune.h"

bool
rprop_is_join_c(rune ch)
{
	return ch == RUNE_C(0x200C) || ch == RUNE_C(0x200D);
}
