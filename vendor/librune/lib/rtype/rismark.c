#include "rtype.h"

bool
rismark(rune ch)
{
	return rprop_get_gc(ch) & GC_M;
}
