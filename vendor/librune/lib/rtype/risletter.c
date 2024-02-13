#include "rtype.h"

bool
risletter(rune ch)
{
	return rprop_get_gc(ch) & GC_L;
}
