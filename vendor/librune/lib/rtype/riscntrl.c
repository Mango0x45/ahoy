#include "rtype.h"

bool
riscntrl(rune ch)
{
	return rprop_get_gc(ch) & GC_CC;
}
