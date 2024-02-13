#include "rtype.h"

bool
rislower(rune ch)
{
	return rprop_get_gc(ch) & GC_LL;
}
