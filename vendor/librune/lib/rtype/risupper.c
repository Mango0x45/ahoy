#include "rtype.h"

bool
risupper(rune ch)
{
	return rprop_get_gc(ch) & GC_LU;
}
