#include "rtype.h"
#include "rune.h"

bool
rprop_is_nchar(rune ch)
{
	return (ch >= RUNE_C(0xFDD0) && ch <= RUNE_C(0xFDEF))
	    || ((ch & RUNE_C(0xFFFE)) == RUNE_C(0xFFFE)
	        && (ch & RUNE_C(0xFF0000)) <= RUNE_C(0x0F0000));
}
