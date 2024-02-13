#include "rtype.h"

#include "internal/rtype/jt.h"

#define DEFAULT      JT_U
#define HAS_VALUE    1
#define LATIN1_TABLE rtype_jt_lat1_tbl
#define TABLE        rtype_jt_tbl
#define TYPE         rprop_jt_bf
#include "internal/rtype/lookup-func.h"

rprop_jt_bf
rprop_get_jt(rune ch)
{
	return lookup(ch);
}
