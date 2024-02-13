#ifndef RUNE_RTYPE_H
#define RUNE_RTYPE_H

#include <stdint.h>

#include "internal/types.h"

typedef uint_fast32_t rprop_gc_bf;
#define GC_CN ((rprop_gc_bf)1 << 0)  /* Not Assigned */
#define GC_CC ((rprop_gc_bf)1 << 1)  /* Control */
#define GC_CF ((rprop_gc_bf)1 << 2)  /* Format */
#define GC_CO ((rprop_gc_bf)1 << 3)  /* Private Use */
#define GC_CS ((rprop_gc_bf)1 << 4)  /* Surrogate */
#define GC_LL ((rprop_gc_bf)1 << 5)  /* Lowercase Letter */
#define GC_LM ((rprop_gc_bf)1 << 6)  /* Modifier Letter */
#define GC_LO ((rprop_gc_bf)1 << 7)  /* Other Letter */
#define GC_LT ((rprop_gc_bf)1 << 8)  /* Titlecase Letter */
#define GC_LU ((rprop_gc_bf)1 << 9)  /* Uppercase Letter */
#define GC_MC ((rprop_gc_bf)1 << 10) /* Spacing Mark */
#define GC_ME ((rprop_gc_bf)1 << 11) /* Enclosing Mark */
#define GC_MN ((rprop_gc_bf)1 << 12) /* Nonspacing Mark */
#define GC_ND ((rprop_gc_bf)1 << 13) /* Decimal Number */
#define GC_NL ((rprop_gc_bf)1 << 14) /* Letter Number */
#define GC_NO ((rprop_gc_bf)1 << 15) /* Other Number */
#define GC_PC ((rprop_gc_bf)1 << 16) /* Connector Punctuation */
#define GC_PD ((rprop_gc_bf)1 << 17) /* Dash Punctuation */
#define GC_PE ((rprop_gc_bf)1 << 18) /* Close Punctuation */
#define GC_PF ((rprop_gc_bf)1 << 19) /* Final Punctuation */
#define GC_PI ((rprop_gc_bf)1 << 20) /* Initial Punctuation */
#define GC_PO ((rprop_gc_bf)1 << 21) /* Other Punctuation */
#define GC_PS ((rprop_gc_bf)1 << 22) /* Open Punctuation */
#define GC_SC ((rprop_gc_bf)1 << 23) /* Currency Symbol */
#define GC_SK ((rprop_gc_bf)1 << 24) /* Modifier Symbol */
#define GC_SM ((rprop_gc_bf)1 << 25) /* Math Symbol */
#define GC_SO ((rprop_gc_bf)1 << 26) /* Other Symbol */
#define GC_ZL ((rprop_gc_bf)1 << 27) /* Line Separator */
#define GC_ZP ((rprop_gc_bf)1 << 28) /* Paragraph Separator */
#define GC_ZS ((rprop_gc_bf)1 << 29) /* Space Separator */
#define GC_C  (GC_CC | GC_CF | GC_CN | GC_CO | GC_CS) /* Other */
#define GC_LC (GC_LU | GC_LL | GC_LT)                 /* Cased Letter */
#define GC_L  (GC_LL | GC_LM | GC_LO | GC_LT | GC_LU) /* Letter */
#define GC_M  (GC_MC | GC_ME | GC_MN)                 /* Mark */
#define GC_N  (GC_ND | GC_NL | GC_NO)                 /* Number */
#define GC_S  (GC_SC | GC_SK | GC_SM | GC_SO)         /* Symbol */
#define GC_Z  (GC_ZL | GC_ZP | GC_ZS)                 /* Separator */
#define GC_P \
	(GC_PC | GC_PD | GC_PE | GC_PF | GC_PI | GC_PO | GC_PS) /* Punctuation */

typedef unsigned int rprop_jt_bf;
#define JT_U ((rprop_jt_bf)1 << 0) /* Non Joining */
#define JT_C ((rprop_jt_bf)1 << 1) /* Join Causing */
#define JT_D ((rprop_jt_bf)1 << 2) /* Dual Joining */
#define JT_R ((rprop_jt_bf)1 << 4) /* Right Joining */
#define JT_L ((rprop_jt_bf)1 << 3) /* Left Joining */
#define JT_T ((rprop_jt_bf)1 << 5) /* Transparent */

typedef unsigned int rprop_nt_bf;
#define NT_NONE    ((rprop_nt_bf)1 << 0)
#define NT_DECIMAL ((rprop_nt_bf)1 << 1)
#define NT_DIGIT   ((rprop_nt_bf)1 << 2)
#define NT_NUMERIC ((rprop_nt_bf)1 << 3)

typedef uint_fast32_t rprop_dt_bf;
#define DT_NONE      ((rprop_dt_bf)1 << 0)
#define DT_CANONICAL ((rprop_dt_bf)1 << 1)
#define DT_CIRCLE    ((rprop_dt_bf)1 << 2)
#define DT_COMPAT    ((rprop_dt_bf)1 << 3)
#define DT_FINAL     ((rprop_dt_bf)1 << 4)
#define DT_FONT      ((rprop_dt_bf)1 << 5)
#define DT_FRACTION  ((rprop_dt_bf)1 << 6)
#define DT_INITIAL   ((rprop_dt_bf)1 << 7)
#define DT_ISOLATED  ((rprop_dt_bf)1 << 8)
#define DT_MEDIAL    ((rprop_dt_bf)1 << 9)
#define DT_NARROW    ((rprop_dt_bf)1 << 10)
#define DT_NOBREAK   ((rprop_dt_bf)1 << 11)
#define DT_SMALL     ((rprop_dt_bf)1 << 12)
#define DT_SQUARE    ((rprop_dt_bf)1 << 13)
#define DT_SUB       ((rprop_dt_bf)1 << 14)
#define DT_SUPER     ((rprop_dt_bf)1 << 15)
#define DT_VERTICAL  ((rprop_dt_bf)1 << 16)
#define DT_WIDE      ((rprop_dt_bf)1 << 17)

/* GCC at the time of writing doesn’t properly support _BitInt */
#ifdef __SIZEOF_INT128__
__extension__ typedef unsigned __int128 rprop_jg_bf;
#elif BITINT_MAXWIDTH >= 128
typedef unsigned _BitInt(128) rprop_jg_bf;
#elif BITINT_MAXWIDTH >= 103
/* Highly unlikely this is ever needed, but who knows.  Powers of 2 generate
   more efficient code so prefer 128. */
typedef unsigned _BitInt(103) rprop_jg_bf;
#else
#	error "_BitInt types of width >=103 not supported"
#endif

#define JG_NONE                     ((rprop_jg_bf)1 << 0)
#define JG_AFRICAN_FEH              ((rprop_jg_bf)1 << 1)
#define JG_AFRICAN_NOON             ((rprop_jg_bf)1 << 2)
#define JG_AFRICAN_QAF              ((rprop_jg_bf)1 << 3)
#define JG_AIN                      ((rprop_jg_bf)1 << 4)
#define JG_ALAPH                    ((rprop_jg_bf)1 << 5)
#define JG_ALEF                     ((rprop_jg_bf)1 << 6)
#define JG_BEH                      ((rprop_jg_bf)1 << 7)
#define JG_BETH                     ((rprop_jg_bf)1 << 8)
#define JG_BURUSHASKI_YEH_BARREE    ((rprop_jg_bf)1 << 9)
#define JG_DAL                      ((rprop_jg_bf)1 << 10)
#define JG_DALATH_RISH              ((rprop_jg_bf)1 << 11)
#define JG_E                        ((rprop_jg_bf)1 << 12)
#define JG_FARSI_YEH                ((rprop_jg_bf)1 << 13)
#define JG_FE                       ((rprop_jg_bf)1 << 14)
#define JG_FEH                      ((rprop_jg_bf)1 << 15)
#define JG_FINAL_SEMKATH            ((rprop_jg_bf)1 << 16)
#define JG_GAF                      ((rprop_jg_bf)1 << 17)
#define JG_GAMAL                    ((rprop_jg_bf)1 << 18)
#define JG_HAH                      ((rprop_jg_bf)1 << 19)
#define JG_HANIFI_ROHINGYA_KINNA_YA ((rprop_jg_bf)1 << 20)
#define JG_HANIFI_ROHINGYA_PA       ((rprop_jg_bf)1 << 21)
#define JG_HE                       ((rprop_jg_bf)1 << 22)
#define JG_HEH                      ((rprop_jg_bf)1 << 23)
#define JG_HEH_GOAL                 ((rprop_jg_bf)1 << 24)
#define JG_HETH                     ((rprop_jg_bf)1 << 25)
#define JG_KAF                      ((rprop_jg_bf)1 << 26)
#define JG_KAPH                     ((rprop_jg_bf)1 << 27)
#define JG_KHAPH                    ((rprop_jg_bf)1 << 28)
#define JG_KNOTTED_HEH              ((rprop_jg_bf)1 << 29)
#define JG_LAM                      ((rprop_jg_bf)1 << 30)
#define JG_LAMADH                   ((rprop_jg_bf)1 << 31)
#define JG_MALAYALAM_BHA            ((rprop_jg_bf)1 << 32)
#define JG_MALAYALAM_JA             ((rprop_jg_bf)1 << 33)
#define JG_MALAYALAM_LLA            ((rprop_jg_bf)1 << 34)
#define JG_MALAYALAM_LLLA           ((rprop_jg_bf)1 << 35)
#define JG_MALAYALAM_NGA            ((rprop_jg_bf)1 << 36)
#define JG_MALAYALAM_NNA            ((rprop_jg_bf)1 << 37)
#define JG_MALAYALAM_NNNA           ((rprop_jg_bf)1 << 38)
#define JG_MALAYALAM_NYA            ((rprop_jg_bf)1 << 39)
#define JG_MALAYALAM_RA             ((rprop_jg_bf)1 << 40)
#define JG_MALAYALAM_SSA            ((rprop_jg_bf)1 << 41)
#define JG_MALAYALAM_TTA            ((rprop_jg_bf)1 << 42)
#define JG_MANICHAEAN_ALEPH         ((rprop_jg_bf)1 << 43)
#define JG_MANICHAEAN_AYIN          ((rprop_jg_bf)1 << 44)
#define JG_MANICHAEAN_BETH          ((rprop_jg_bf)1 << 45)
#define JG_MANICHAEAN_DALETH        ((rprop_jg_bf)1 << 46)
#define JG_MANICHAEAN_DHAMEDH       ((rprop_jg_bf)1 << 47)
#define JG_MANICHAEAN_FIVE          ((rprop_jg_bf)1 << 48)
#define JG_MANICHAEAN_GIMEL         ((rprop_jg_bf)1 << 49)
#define JG_MANICHAEAN_HETH          ((rprop_jg_bf)1 << 50)
#define JG_MANICHAEAN_HUNDRED       ((rprop_jg_bf)1 << 51)
#define JG_MANICHAEAN_KAPH          ((rprop_jg_bf)1 << 52)
#define JG_MANICHAEAN_LAMEDH        ((rprop_jg_bf)1 << 53)
#define JG_MANICHAEAN_MEM           ((rprop_jg_bf)1 << 54)
#define JG_MANICHAEAN_NUN           ((rprop_jg_bf)1 << 55)
#define JG_MANICHAEAN_ONE           ((rprop_jg_bf)1 << 56)
#define JG_MANICHAEAN_PE            ((rprop_jg_bf)1 << 57)
#define JG_MANICHAEAN_QOPH          ((rprop_jg_bf)1 << 58)
#define JG_MANICHAEAN_RESH          ((rprop_jg_bf)1 << 59)
#define JG_MANICHAEAN_SADHE         ((rprop_jg_bf)1 << 60)
#define JG_MANICHAEAN_SAMEKH        ((rprop_jg_bf)1 << 61)
#define JG_MANICHAEAN_TAW           ((rprop_jg_bf)1 << 62)
#define JG_MANICHAEAN_TEN           ((rprop_jg_bf)1 << 63)
#define JG_MANICHAEAN_TETH          ((rprop_jg_bf)1 << 64)
#define JG_MANICHAEAN_THAMEDH       ((rprop_jg_bf)1 << 65)
#define JG_MANICHAEAN_TWENTY        ((rprop_jg_bf)1 << 66)
#define JG_MANICHAEAN_WAW           ((rprop_jg_bf)1 << 67)
#define JG_MANICHAEAN_YODH          ((rprop_jg_bf)1 << 68)
#define JG_MANICHAEAN_ZAYIN         ((rprop_jg_bf)1 << 69)
#define JG_MEEM                     ((rprop_jg_bf)1 << 70)
#define JG_MIM                      ((rprop_jg_bf)1 << 71)
#define JG_NOON                     ((rprop_jg_bf)1 << 72)
#define JG_NUN                      ((rprop_jg_bf)1 << 73)
#define JG_NYA                      ((rprop_jg_bf)1 << 74)
#define JG_PE                       ((rprop_jg_bf)1 << 75)
#define JG_QAF                      ((rprop_jg_bf)1 << 76)
#define JG_QAPH                     ((rprop_jg_bf)1 << 77)
#define JG_REH                      ((rprop_jg_bf)1 << 78)
#define JG_REVERSED_PE              ((rprop_jg_bf)1 << 79)
#define JG_ROHINGYA_YEH             ((rprop_jg_bf)1 << 80)
#define JG_SAD                      ((rprop_jg_bf)1 << 81)
#define JG_SADHE                    ((rprop_jg_bf)1 << 82)
#define JG_SEEN                     ((rprop_jg_bf)1 << 83)
#define JG_SEMKATH                  ((rprop_jg_bf)1 << 84)
#define JG_SHIN                     ((rprop_jg_bf)1 << 85)
#define JG_STRAIGHT_WAW             ((rprop_jg_bf)1 << 86)
#define JG_SWASH_KAF                ((rprop_jg_bf)1 << 87)
#define JG_SYRIAC_WAW               ((rprop_jg_bf)1 << 88)
#define JG_TAH                      ((rprop_jg_bf)1 << 89)
#define JG_TAW                      ((rprop_jg_bf)1 << 90)
#define JG_TEH_MARBUTA              ((rprop_jg_bf)1 << 91)
#define JG_TEH_MARBUTA_GOAL         ((rprop_jg_bf)1 << 92)
#define JG_TETH                     ((rprop_jg_bf)1 << 93)
#define JG_THIN_YEH                 ((rprop_jg_bf)1 << 94)
#define JG_VERTICAL_TAIL            ((rprop_jg_bf)1 << 95)
#define JG_WAW                      ((rprop_jg_bf)1 << 96)
#define JG_YEH                      ((rprop_jg_bf)1 << 97)
#define JG_YEH_BARREE               ((rprop_jg_bf)1 << 98)
#define JG_YEH_WITH_TAIL            ((rprop_jg_bf)1 << 99)
#define JG_YUDH                     ((rprop_jg_bf)1 << 100)
#define JG_YUDH_HE                  ((rprop_jg_bf)1 << 101)
#define JG_ZAIN                     ((rprop_jg_bf)1 << 102)
#define JG_ZHAIN                    ((rprop_jg_bf)1 << 103)

typedef unsigned int rprop_vo_bf;
#define VO_U  ((rprop_vo_bf)1 << 0) /* Upright */
#define VO_R  ((rprop_vo_bf)1 << 1) /* Rotated */
#define VO_TU ((rprop_vo_bf)1 << 2) /* Transformed ∨ Upright */
#define VO_TR ((rprop_vo_bf)1 << 3) /* Transformed ∨ Rotated */

[[unsequenced]] bool riscntrl(rune);
[[unsequenced]] bool risdigit(rune);
[[unsequenced]] bool risgraph(rune);
[[unsequenced]] bool rislower(rune);
[[unsequenced]] bool rismark(rune);
[[unsequenced]] bool risnumber(rune);
[[unsequenced]] bool rispunct(rune);
[[unsequenced]] bool risspace(rune);
[[unsequenced]] bool rissymbol(rune);
[[unsequenced]] bool ristitle(rune);
[[unsequenced]] bool risupper(rune);

[[unsequenced]] rprop_dt_bf rprop_get_dt(rune);
[[unsequenced]] rprop_gc_bf rprop_get_gc(rune);
[[unsequenced]] rprop_jg_bf rprop_get_jg(rune);
[[unsequenced]] rprop_jt_bf rprop_get_jt(rune);
[[unsequenced]] rprop_nt_bf rprop_get_nt(rune);
[[unsequenced]] rprop_vo_bf rprop_get_vo(rune);

[[unsequenced]] double rprop_get_nv(rune);
[[unsequenced]] rune rprop_get_equideo(rune);

/* Non-autogenerated rprop_is_*() functions */
[[unsequenced]] bool rprop_is_ahex(rune);
[[unsequenced]] bool rprop_is_idst(rune);
[[unsequenced]] bool rprop_is_idsu(rune);
[[unsequenced]] bool rprop_is_join_c(rune);
[[unsequenced]] bool rprop_is_nchar(rune);
[[unsequenced]] bool rprop_is_pat_ws(rune);
[[unsequenced]] bool rprop_is_ri(rune);

/* PROP PREDICATES START */
[[unsequenced]] bool rprop_is_alpha(rune);
[[unsequenced]] bool rprop_is_bidi_c(rune);
[[unsequenced]] bool rprop_is_bidi_m(rune);
[[unsequenced]] bool rprop_is_cased(rune);
[[unsequenced]] bool rprop_is_ci(rune);
[[unsequenced]] bool rprop_is_cwcf(rune);
[[unsequenced]] bool rprop_is_cwcm(rune);
[[unsequenced]] bool rprop_is_cwkcf(rune);
[[unsequenced]] bool rprop_is_cwl(rune);
[[unsequenced]] bool rprop_is_cwt(rune);
[[unsequenced]] bool rprop_is_cwu(rune);
[[unsequenced]] bool rprop_is_dash(rune);
[[unsequenced]] bool rprop_is_dep(rune);
[[unsequenced]] bool rprop_is_di(rune);
[[unsequenced]] bool rprop_is_dia(rune);
[[unsequenced]] bool rprop_is_ebase(rune);
[[unsequenced]] bool rprop_is_ecomp(rune);
[[unsequenced]] bool rprop_is_emod(rune);
[[unsequenced]] bool rprop_is_emoji(rune);
[[unsequenced]] bool rprop_is_epres(rune);
[[unsequenced]] bool rprop_is_ext(rune);
[[unsequenced]] bool rprop_is_extpic(rune);
[[unsequenced]] bool rprop_is_gr_base(rune);
[[unsequenced]] bool rprop_is_gr_ext(rune);
[[unsequenced]] bool rprop_is_hex(rune);
[[unsequenced]] bool rprop_is_id_compat_math_continue(rune);
[[unsequenced]] bool rprop_is_id_compat_math_start(rune);
[[unsequenced]] bool rprop_is_idbo(rune);
[[unsequenced]] bool rprop_is_idc(rune);
[[unsequenced]] bool rprop_is_ideo(rune);
[[unsequenced]] bool rprop_is_ids(rune);
[[unsequenced]] bool rprop_is_incb(rune);
[[unsequenced]] bool rprop_is_loe(rune);
[[unsequenced]] bool rprop_is_lower(rune);
[[unsequenced]] bool rprop_is_math(rune);
[[unsequenced]] bool rprop_is_pat_syn(rune);
[[unsequenced]] bool rprop_is_pcm(rune);
[[unsequenced]] bool rprop_is_qmark(rune);
[[unsequenced]] bool rprop_is_radical(rune);
[[unsequenced]] bool rprop_is_sd(rune);
[[unsequenced]] bool rprop_is_sterm(rune);
[[unsequenced]] bool rprop_is_term(rune);
[[unsequenced]] bool rprop_is_uideo(rune);
[[unsequenced]] bool rprop_is_upper(rune);
[[unsequenced]] bool rprop_is_vs(rune);
[[unsequenced]] bool rprop_is_wspace(rune);
[[unsequenced]] bool rprop_is_xidc(rune);
[[unsequenced]] bool rprop_is_xids(rune);
/* PROP PREDICATES END */

#endif
