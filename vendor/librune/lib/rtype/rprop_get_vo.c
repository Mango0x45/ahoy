#include "rtype.h"

#include "internal/rtype/vo.h"

#define DEFAULT      VO_R
#define HAS_VALUE    1
#define LATIN1_TABLE rtype_vo_lat1_tbl
#define TABLE        rtype_vo_tbl
#define TYPE         rprop_vo_bf
#include "internal/rtype/lookup-func.h"

rprop_vo_bf
rprop_get_vo(rune ch)
{
	return lookup(ch);
}
