#include "rtype.h"
#include "rune.h"

#include "internal/rtype/equideo.h"

#define DEFAULT   RUNE_ERROR
#define HAS_VALUE 1
#define TABLE     rtype_equideo_tbl
#define TYPE      rune
#define RANGE     0
#include "internal/rtype/lookup-func.h"

rune
rprop_get_equideo(rune ch)
{
	return ch < RUNE_C(0x2E81) ? RUNE_ERROR : lookup(ch);
}
