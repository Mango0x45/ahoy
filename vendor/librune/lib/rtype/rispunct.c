#include "rtype.h"

bool
rispunct(rune ch)
{
	return rprop_get_gc(ch) & GC_P;
}
