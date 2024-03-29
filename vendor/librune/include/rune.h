#ifndef RUNE_RUNE_H
#define RUNE_RUNE_H

#include <inttypes.h>

#include "internal/types.h" /* IWYU pragma: export */

#define _RUNE_PRIDEF(c) PRI##c##LEAST32
#define _RUNE_SCNDEF(c) SCN##c##LEAST32

#ifdef PRIBLEAST32
#	define PRIBRUNE _RUNE_PRIDEF(B)
#endif
#define PRIbRUNE _RUNE_PRIDEF(b)
#define PRIdRUNE _RUNE_PRIDEF(d)
#define PRIiRUNE _RUNE_PRIDEF(i)
#define PRIoRUNE _RUNE_PRIDEF(o)
#define PRIuRUNE _RUNE_PRIDEF(u)
#define PRIxRUNE _RUNE_PRIDEF(x)
#define PRIXRUNE _RUNE_PRIDEF(X)

#define SCNbRUNE _RUNE_SCNDEF(b)
#define SCNdRUNE _RUNE_SCNDEF(d)
#define SCNiRUNE _RUNE_SCNDEF(i)
#define SCNuRUNE _RUNE_SCNDEF(u)
#define SCNoRUNE _RUNE_SCNDEF(o)
#define SCNxRUNE _RUNE_SCNDEF(x)

#define RUNE_C(x) UINT32_C(x)

#define RUNE_ERROR ((rune)0xFFFD)
#define RUNE_MAX   ((rune)RUNE_C(0x10FFFF))

#endif /* !RUNE_RUNE_H */
