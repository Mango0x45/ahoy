#include "rtype.h"

bool
risdigit(rune ch)
{
	return rprop_get_gc(ch) & GC_ND;
}
