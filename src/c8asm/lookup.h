/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf --output-file src/c8asm/lookup.h src/c8asm/instr.gperf  */
/* Computed positions: -k'1-3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 7 "src/c8asm/instr.gperf"
struct opf_pair { char *name; void (*pfn)(void); };
#include <string.h>

#define TOTAL_KEYWORDS 25
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 49
/* maximum key range = 48, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register size_t len)
{
  static const unsigned char asso_values[] =
    {
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50,  5, 15, 15,
      10,  0, 50, 50, 10, 50,  0,  5, 20, 50,
       5,  5, 18, 50,  0,  0,  0, 30, 50,  8,
       3,  3, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

const struct opf_pair *
oplookup (register const char *str, register size_t len)
{
  static const unsigned char lengthtable[] =
    {
       0,  0,  2,  3,  4,  0,  3,  2,  3,  4,  0,  3,  0,  3,
       4,  0,  3,  0,  3,  0,  2,  3,  0,  3,  0,  0,  3,  2,
       3,  0,  0,  0,  2,  3,  0,  0,  0,  0,  3,  0,  0,  0,
       0,  3,  4,  0,  0,  0,  3,  4
    };
  static const struct opf_pair wordlist[] =
    {
      {"",nullptr}, {"",nullptr},
#line 23 "src/c8asm/instr.gperf"
      {"se",   parseop_se},
#line 20 "src/c8asm/instr.gperf"
      {"ret",  parseop_ret},
#line 22 "src/c8asm/instr.gperf"
      {"rstr", parseop_rstr},
      {"",nullptr},
#line 32 "src/c8asm/instr.gperf"
      {"sys",  parseop_sys},
#line 19 "src/c8asm/instr.gperf"
      {"or",   parseop_or},
#line 28 "src/c8asm/instr.gperf"
      {"sne",  parseop_sne},
#line 29 "src/c8asm/instr.gperf"
      {"stor", parseop_stor},
      {"",nullptr},
#line 33 "src/c8asm/instr.gperf"
      {"xor",  parseop_xor},
      {"",nullptr},
#line 25 "src/c8asm/instr.gperf"
      {"shr",  parseop_shr},
#line 26 "src/c8asm/instr.gperf"
      {"sknp", parseop_sknp},
      {"",nullptr},
#line 16 "src/c8asm/instr.gperf"
      {"hex",  parseop_hex},
      {"",nullptr},
#line 21 "src/c8asm/instr.gperf"
      {"rnd",  parseop_rnd},
      {"",nullptr},
#line 17 "src/c8asm/instr.gperf"
      {"jp",   parseop_jp},
#line 15 "src/c8asm/instr.gperf"
      {"drw",  parseop_drw},
      {"",nullptr},
#line 10 "src/c8asm/instr.gperf"
      {"and",  parseop_and},
      {"",nullptr}, {"",nullptr},
#line 27 "src/c8asm/instr.gperf"
      {"skp",  parseop_skp},
#line 14 "src/c8asm/instr.gperf"
      {"db",   parseop_db},
#line 9 "src/c8asm/instr.gperf"
      {"add",  parseop_add},
      {"",nullptr}, {"",nullptr}, {"",nullptr},
#line 18 "src/c8asm/instr.gperf"
      {"ld",   parseop_ld},
#line 24 "src/c8asm/instr.gperf"
      {"shl",  parseop_shl},
      {"",nullptr}, {"",nullptr}, {"",nullptr}, {"",nullptr},
#line 13 "src/c8asm/instr.gperf"
      {"cls",  parseop_cls},
      {"",nullptr}, {"",nullptr}, {"",nullptr}, {"",nullptr},
#line 11 "src/c8asm/instr.gperf"
      {"bcd",  parseop_bcd},
#line 12 "src/c8asm/instr.gperf"
      {"call", parseop_call},
      {"",nullptr}, {"",nullptr}, {"",nullptr},
#line 30 "src/c8asm/instr.gperf"
      {"sub",  parseop_sub},
#line 31 "src/c8asm/instr.gperf"
      {"subn", parseop_subn}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        if (len == lengthtable[key])
          {
            register const char *s = wordlist[key].name;

            if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
              return &wordlist[key];
          }
    }
  return 0;
}
