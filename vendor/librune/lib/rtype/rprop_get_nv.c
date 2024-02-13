#include "rtype.h"
#include "rune.h"

#include "internal/rtype/nv.h"

#define DEFAULT   (0.0 / 0.0) /* NAN */
#define HAS_VALUE 1
#define TABLE     rtype_nv_tbl
#define TYPE      double
#define RANGE     0
#include "internal/rtype/lookup-func.h"

double
rprop_get_nv(rune ch)
{
	return lookup(ch);
}
