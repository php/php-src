/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: /usr/local/bin/gperf -pt -T -L ANSI-C -N onigenc_sjis_lookup_property_name --output-file gperf2.tmp sjis_prop.gperf  */
/* Computed positions: -k'1,3' */

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

#line 1 "sjis_prop.gperf"

#include <string.h>
#include "regenc.h"

#define TOTAL_KEYWORDS 16
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 55
/* maximum key range = 52, duplicates = 0 */

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
  static unsigned char asso_values[] =
    {
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56,  0,  3, 10, 25, 56,
      56, 30,  0, 56, 56,  0, 20, 56, 56, 56,
      15, 56, 56, 20, 56, 30, 56,  0,  0, 56,
      56, 56, 56, 56, 56, 56, 56, 15, 56, 56,
      56, 56, 56, 25, 56, 10, 56, 56, 56, 56,
       5, 56,  0, 56,  0, 56,  5, 56, 56, 20,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
      56, 56, 56, 56, 56, 56
    };
  return (unsigned int )len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[0]];
}

struct PropertyNameCtype *
onigenc_sjis_lookup_property_name (register const char *str, register size_t len)
{
  static struct PropertyNameCtype wordlist[] =
    {
      {""}, {""}, {""}, {""},
#line 23 "sjis_prop.gperf"
      {"Word",     12},
#line 12 "sjis_prop.gperf"
      {"Alpha",     1},
      {""}, {""},
#line 26 "sjis_prop.gperf"
      {"Hiragana", 15},
      {""},
#line 24 "sjis_prop.gperf"
      {"Alnum",    13},
      {""}, {""},
#line 27 "sjis_prop.gperf"
      {"Katakana", 16},
      {""},
#line 25 "sjis_prop.gperf"
      {"ASCII",    14},
#line 22 "sjis_prop.gperf"
      {"XDigit",   11},
      {""}, {""}, {""},
#line 14 "sjis_prop.gperf"
      {"Cntrl",     3},
      {""}, {""},
#line 13 "sjis_prop.gperf"
      {"Blank",     2},
      {""},
#line 19 "sjis_prop.gperf"
      {"Punct",     8},
      {""}, {""}, {""}, {""},
#line 18 "sjis_prop.gperf"
      {"Print",     7},
      {""}, {""}, {""}, {""},
#line 21 "sjis_prop.gperf"
      {"Upper",    10},
      {""}, {""}, {""}, {""},
#line 20 "sjis_prop.gperf"
      {"Space",     9},
      {""}, {""}, {""}, {""},
#line 17 "sjis_prop.gperf"
      {"Lower",     6},
      {""}, {""}, {""}, {""},
#line 16 "sjis_prop.gperf"
      {"Graph",     5},
      {""}, {""}, {""}, {""},
#line 15 "sjis_prop.gperf"
      {"Digit",     4}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
