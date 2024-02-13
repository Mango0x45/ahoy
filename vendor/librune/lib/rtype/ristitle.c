#include "rtype.h"

bool
ristitle(rune ch)
{
	return rprop_get_gc(ch) & GC_LT;
}
