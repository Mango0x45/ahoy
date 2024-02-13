#include "rtype.h"

bool
rissymbol(rune ch)
{
	return rprop_get_gc(ch) & GC_S;
}
