#include "rtype.h"

bool
risnumber(rune ch)
{
	return rprop_get_gc(ch) & GC_N;
}
