/**********************************************************************
  sjis.c -  Onigmo (Oniguruma-mod) (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2008  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
 * Copyright (c) 2011       K.Takata  <kentkt AT csc DOT jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "regint.h"

static const int EncLen_SJIS[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1
};

static const char SJIS_CAN_BE_TRAIL_TABLE[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0
};

static const OnigPairCaseFoldCodes CaseFoldMap[] = {
  /* Fullwidth Alphabet */
  { 0x8260, 0x8281 },
  { 0x8261, 0x8282 },
  { 0x8262, 0x8283 },
  { 0x8263, 0x8284 },
  { 0x8264, 0x8285 },
  { 0x8265, 0x8286 },
  { 0x8266, 0x8287 },
  { 0x8267, 0x8288 },
  { 0x8268, 0x8289 },
  { 0x8269, 0x828a },
  { 0x826a, 0x828b },
  { 0x826b, 0x828c },
  { 0x826c, 0x828d },
  { 0x826d, 0x828e },
  { 0x826e, 0x828f },
  { 0x826f, 0x8290 },
  { 0x8270, 0x8291 },
  { 0x8271, 0x8292 },
  { 0x8272, 0x8293 },
  { 0x8273, 0x8294 },
  { 0x8274, 0x8295 },
  { 0x8275, 0x8296 },
  { 0x8276, 0x8297 },
  { 0x8277, 0x8298 },
  { 0x8278, 0x8299 },
  { 0x8279, 0x829a },

  /* Greek */
  { 0x839f, 0x83bf },
  { 0x83a0, 0x83c0 },
  { 0x83a1, 0x83c1 },
  { 0x83a2, 0x83c2 },
  { 0x83a3, 0x83c3 },
  { 0x83a4, 0x83c4 },
  { 0x83a5, 0x83c5 },
  { 0x83a6, 0x83c6 },
  { 0x83a7, 0x83c7 },
  { 0x83a8, 0x83c8 },
  { 0x83a9, 0x83c9 },
  { 0x83aa, 0x83ca },
  { 0x83ab, 0x83cb },
  { 0x83ac, 0x83cc },
  { 0x83ad, 0x83cd },
  { 0x83ae, 0x83ce },
  { 0x83af, 0x83cf },
  { 0x83b0, 0x83d0 },
  { 0x83b1, 0x83d1 },
  { 0x83b2, 0x83d2 },
  { 0x83b3, 0x83d3 },
  { 0x83b4, 0x83d4 },
  { 0x83b5, 0x83d5 },
  { 0x83b6, 0x83d6 },

  /* Cyrillic */
  { 0x8440, 0x8470 },
  { 0x8441, 0x8471 },
  { 0x8442, 0x8472 },
  { 0x8443, 0x8473 },
  { 0x8444, 0x8474 },
  { 0x8445, 0x8475 },
  { 0x8446, 0x8476 },
  { 0x8447, 0x8477 },
  { 0x8448, 0x8478 },
  { 0x8449, 0x8479 },
  { 0x844a, 0x847a },
  { 0x844b, 0x847b },
  { 0x844c, 0x847c },
  { 0x844d, 0x847d },
  { 0x844e, 0x847e },
  { 0x844f, 0x8480 },
  { 0x8450, 0x8481 },
  { 0x8451, 0x8482 },
  { 0x8452, 0x8483 },
  { 0x8453, 0x8484 },
  { 0x8454, 0x8485 },
  { 0x8455, 0x8486 },
  { 0x8456, 0x8487 },
  { 0x8457, 0x8488 },
  { 0x8458, 0x8489 },
  { 0x8459, 0x848a },
  { 0x845a, 0x848b },
  { 0x845b, 0x848c },
  { 0x845c, 0x848d },
  { 0x845d, 0x848e },
  { 0x845e, 0x848f },
  { 0x845f, 0x8490 },
  { 0x8460, 0x8491 },
};

#define SJIS_ISMB_FIRST(byte)  (EncLen_SJIS[byte] > 1)
#define SJIS_ISMB_TRAIL(byte)  SJIS_CAN_BE_TRAIL_TABLE[(byte)]

typedef enum { FAILURE = -2, ACCEPT = -1, S0 = 0, S1 } state_t;
#define A ACCEPT
#define F FAILURE
static const signed char trans[][0x100] = {
  { /* S0   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 0 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 1 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 2 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 3 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 4 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 5 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 6 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 7 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 8 */ F, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 9 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* a */ F, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* b */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* c */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* d */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* e */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* f */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, F, F, F
  },
  { /* S1   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 0 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 1 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 2 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 3 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 4 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 5 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 6 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 7 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, F,
    /* 8 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 9 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* a */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* b */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* c */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* d */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* e */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* f */ A, A, A, A, A, A, A, A, A, A, A, A, A, F, F, F
  }
};
#undef A
#undef F

static int
mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc ARG_UNUSED)
{
  int firstbyte = *p++;
  state_t s;
  s = trans[0][firstbyte];
  if (s < 0) return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(1) :
                                  ONIGENC_CONSTRUCT_MBCLEN_INVALID();
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(EncLen_SJIS[firstbyte]-1);
  s = trans[s][*p++];
  return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(2) :
                       ONIGENC_CONSTRUCT_MBCLEN_INVALID();
}

static int
code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
  if (code < 256) {
    if (EncLen_SJIS[(int )code] == 1)
      return 1;
    else
      return ONIGERR_INVALID_CODE_POINT_VALUE;
  }
  else if (code <= 0xffff) {
    return 2;
  }
  else
    return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
}

static OnigCodePoint
mbc_to_code(const UChar* p, const UChar* end, OnigEncoding enc)
{
  int c, i, len;
  OnigCodePoint n;

  len = mbc_enc_len(p, end, enc);
  c = *p++;
  n = c;
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}

static int
code_to_mbc(OnigCodePoint code, UChar *buf, OnigEncoding enc)
{
  UChar *p = buf;

  if ((code & 0xff00) != 0) *p++ = (UChar )(((code >>  8) & 0xff));
  *p++ = (UChar )(code & 0xff);

#if 0
  if (mbc_enc_len(buf, p, enc) != (p - buf))
    return REGERR_INVALID_CODE_POINT_VALUE;
#endif
  return (int )(p - buf);
}

static int
apply_all_case_fold(OnigCaseFoldType flag,
		    OnigApplyAllCaseFoldFunc f, void* arg, OnigEncoding enc)
{
  return onigenc_apply_all_case_fold_with_map(
            sizeof(CaseFoldMap)/sizeof(OnigPairCaseFoldCodes), CaseFoldMap, 0,
            flag, f, arg);
}

static OnigCodePoint
get_lower_case(OnigCodePoint code)
{
  if (ONIGENC_IS_IN_RANGE(code, 0x8260, 0x8279)) {
    /* Fullwidth Alphabet */
    return (OnigCodePoint )(code + 0x0021);
  }
  else if (ONIGENC_IS_IN_RANGE(code, 0x839f, 0x83b6)) {
    /* Greek */
    return (OnigCodePoint )(code + 0x0020);
  }
  else if (ONIGENC_IS_IN_RANGE(code, 0x8440, 0x8460)) {
    /* Cyrillic */
    int d = (code >= 0x844f) ? 1 : 0;
    return (OnigCodePoint )(code + (0x0030 + d));
  }
  return code;
}

static OnigCodePoint
get_upper_case(OnigCodePoint code)
{
  if (ONIGENC_IS_IN_RANGE(code, 0x8281, 0x829a)) {
    /* Fullwidth Alphabet */
    return (OnigCodePoint )(code - 0x0021);
  }
  else if (ONIGENC_IS_IN_RANGE(code, 0x83bf, 0x83d6)) {
    /* Greek */
    return (OnigCodePoint )(code - 0x0020);
  }
  else if (ONIGENC_IS_IN_RANGE(code, 0x8470, 0x847e) ||
	   ONIGENC_IS_IN_RANGE(code, 0x8480, 0x8491)) {
    /* Cyrillic */
    int d = (code >= 0x8480) ? 1 : 0;
    return (OnigCodePoint )(code - (0x0030 - d));
  }
  return code;
}

static int
get_case_fold_codes_by_str(OnigCaseFoldType flag,
			   const OnigUChar* p, const OnigUChar* end,
			   OnigCaseFoldCodeItem items[], OnigEncoding enc)
{
  int len;
  OnigCodePoint code, code_lo, code_up;

  code = mbc_to_code(p, end, enc);
  if (ONIGENC_IS_ASCII_CODE(code))
    return onigenc_ascii_get_case_fold_codes_by_str(flag, p, end, items, enc);

  len = mbc_enc_len(p, end, enc);
  code_lo = get_lower_case(code);
  code_up = get_upper_case(code);

  if (code != code_lo) {
    items[0].byte_len = len;
    items[0].code_len = 1;
    items[0].code[0] = code_lo;
    return 1;
  }
  else if (code != code_up) {
    items[0].byte_len = len;
    items[0].code_len = 1;
    items[0].code[0] = code_up;
    return 1;
  }

  return 0;
}

static int
mbc_case_fold(OnigCaseFoldType flag,
	      const UChar** pp, const UChar* end, UChar* lower,
	      OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    (*pp)++;
    return 1;
  }
  else {
    OnigCodePoint code;
    int len;

    code = get_lower_case(mbc_to_code(p, end, enc));
    len = code_to_mbc(code, lower, enc);
    (*pp) += len;
    return len; /* return byte length of converted char to lower */
  }
}

#if 0
static int
is_mbc_ambiguous(OnigCaseFoldType flag,
		 const UChar** pp, const UChar* end)
{
  return onigenc_mbn_is_mbc_ambiguous(enc, flag, pp, end);

}
#endif

#if 0
static int
is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else {
    if (CTYPE_IS_WORD_GRAPH_PRINT(ctype)) {
      return (code_to_mbclen(code) > 1 ? TRUE : FALSE);
    }
  }

  return FALSE;
}
#endif

static UChar*
left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end, OnigEncoding enc)
{
  const UChar *p;
  int len;

  if (s <= start) return (UChar* )s;
  p = s;

  if (SJIS_ISMB_TRAIL(*p)) {
    while (p > start) {
      if (! SJIS_ISMB_FIRST(*--p)) {
	p++;
	break;
      }
    }
  }
  len = mbc_enc_len(p, end, enc);
  if (p + len > s) return (UChar* )p;
  p += len;
  return (UChar* )(p + ((s - p) & ~1));
}

static int
is_allowed_reverse_match(const UChar* s, const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  const UChar c = *s;
  return (SJIS_ISMB_TRAIL(c) ? FALSE : TRUE);
}


static int PropertyInited = 0;
static const OnigCodePoint** PropertyList;
static int PropertyListNum;
static int PropertyListSize;
static hash_table_type* PropertyNameTable;

static const OnigCodePoint CR_Hiragana[] = {
  1,
  0x829f, 0x82f1
}; /* CR_Hiragana */

static const OnigCodePoint CR_Katakana[] = {
  4,
  0x00a6, 0x00af,
  0x00b1, 0x00dd,
  0x8340, 0x837e,
  0x8380, 0x8396,
}; /* CR_Katakana */

#ifdef ENC_CP932
static const OnigCodePoint CR_Han[] = {
  6,
  0x8157, 0x8157,
  0x889f, 0x9872,	/* Kanji level 1 */
  0x989f, 0x9ffc,	/* Kanji level 2 */
  0xe040, 0xeaa4,	/* Kanji level 2 */
  0xed40, 0xeeec,	/* NEC-selected IBM extended characters (without symbols) */
  0xfa5c, 0xfc4b,	/* IBM extended characters (without symbols) */
}; /* CR_Han */
#else
static const OnigCodePoint CR_Han[] = {
  4,
  0x8157, 0x8157,
  0x889f, 0x9872,	/* Kanji level 1 */
  0x989f, 0x9ffc,	/* Kanji level 2 */
  0xe040, 0xeaa4,	/* Kanji level 2 */
}; /* CR_Han */
#endif

static const OnigCodePoint CR_Latin[] = {
  4,
  0x0041, 0x005a,
  0x0061, 0x007a,
  0x8260, 0x8279,
  0x8281, 0x829a,
}; /* CR_Latin */

static const OnigCodePoint CR_Greek[] = {
  2,
  0x839f, 0x83b6,
  0x83bf, 0x83d6,
}; /* CR_Greek */

static const OnigCodePoint CR_Cyrillic[] = {
  3,
  0x8440, 0x8460,
  0x8470, 0x847f,
  0x8480, 0x8491,
}; /* CR_Cyrillic */

static int
init_property_list(void)
{
  int r;

  PROPERTY_LIST_ADD_PROP("hiragana", CR_Hiragana);
  PROPERTY_LIST_ADD_PROP("katakana", CR_Katakana);
  PROPERTY_LIST_ADD_PROP("han", CR_Han);
  PROPERTY_LIST_ADD_PROP("latin", CR_Latin);
  PROPERTY_LIST_ADD_PROP("greek", CR_Greek);
  PROPERTY_LIST_ADD_PROP("cyrillic", CR_Cyrillic);
  PropertyInited = 1;

 end:
  return r;
}

static int
property_name_to_ctype(OnigEncoding enc, UChar* p, UChar* end)
{
  hash_data_type ctype;
  UChar *s, *e;

  PROPERTY_LIST_INIT_CHECK;

  s = e = ALLOCA_N(UChar, end-p+1);
  for (; p < end; p++) {
    *e++ = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
  }

  if (onig_st_lookup_strend(PropertyNameTable, s, e, &ctype) == 0) {
    return onigenc_minimum_property_name_to_ctype(enc, s, e);
  }

  return (int )ctype;
}

static int
is_code_ctype(OnigCodePoint code, unsigned int ctype, OnigEncoding enc)
{
  if (ctype <= ONIGENC_MAX_STD_CTYPE) {
    if (code < 128)
      return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
    else {
      if (CTYPE_IS_WORD_GRAPH_PRINT(ctype)) {
	return TRUE;
      }
    }
  }
  else {
    PROPERTY_LIST_INIT_CHECK;

    ctype -= (ONIGENC_MAX_STD_CTYPE + 1);
    if (ctype >= (unsigned int )PropertyListNum)
      return ONIGERR_TYPE_BUG;

    return onig_is_in_code_range((UChar* )PropertyList[ctype], code);
  }

  return FALSE;
}

static int
get_ctype_code_range(OnigCtype ctype, OnigCodePoint* sb_out,
		     const OnigCodePoint* ranges[], OnigEncoding enc ARG_UNUSED)
{
  if (ctype <= ONIGENC_MAX_STD_CTYPE) {
    return ONIG_NO_SUPPORT_CONFIG;
  }
  else {
    *sb_out = 0x80;

    PROPERTY_LIST_INIT_CHECK;

    ctype -= (ONIGENC_MAX_STD_CTYPE + 1);
    if (ctype >= (OnigCtype )PropertyListNum)
      return ONIGERR_TYPE_BUG;

    *ranges = PropertyList[ctype];
    return 0;
  }
}

#ifndef ENC_CP932
OnigEncodingDefine(shift_jis, Shift_JIS) = {
  mbc_enc_len,
  "Shift_JIS",   /* name */
  2,             /* max byte length */
  1,             /* min byte length */
  onigenc_is_mbc_newline_0x0a,
  mbc_to_code,
  code_to_mbclen,
  code_to_mbc,
  mbc_case_fold,
  apply_all_case_fold,
  get_case_fold_codes_by_str,
  property_name_to_ctype,
  is_code_ctype,
  get_ctype_code_range,
  left_adjust_char_head,
  is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_NONE,
};
/*
 * Name: Shift_JIS
 * MIBenum: 17
 * Link: http://www.iana.org/assignments/character-sets
 * Link: http://ja.wikipedia.org/wiki/Shift_JIS
 */

/*
 * Name: MacJapanese
 * Link: http://unicode.org/Public/MAPPINGS/VENDORS/APPLE/JAPANESE.TXT
 * Link: http://ja.wikipedia.org/wiki/MacJapanese
 */
ENC_REPLICATE("MacJapanese", "Shift_JIS")
ENC_ALIAS("MacJapan", "MacJapanese")
#endif
