#include "rtype.h"

#include "internal/rtype/nt.h"

#define DEFAULT      NT_NONE
#define HAS_VALUE    1
#define LATIN1_TABLE rtype_nt_lat1_tbl
#define TABLE        rtype_nt_tbl
#define TYPE         rprop_nt_bf
#include "internal/rtype/lookup-func.h"

rprop_nt_bf
rprop_get_nt(rune ch)
{
	return lookup(ch);
}
