/**********************************************************************

  big5.c -  Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regenc.h"

static OnigCodePoint
big5_mbc_to_code(UChar* p, UChar* end)
{
  return onigenc_mbn_mbc_to_code(ONIG_ENCODING_BIG5, p, end);
}

static int
big5_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  return onigenc_mb2_code_to_mbc(ONIG_ENCODING_BIG5, code, buf);
}

static int
big5_mbc_to_lower(UChar* p, UChar* lower)
{
  return onigenc_mbn_mbc_to_lower(ONIG_ENCODING_BIG5, p, lower);
}

static int
big5_code_is_ctype(OnigCodePoint code, unsigned int ctype)
{
  return onigenc_mb2_code_is_ctype(ONIG_ENCODING_BIG5, code, ctype);
}

static const char BIG5_CAN_BE_TRAIL_TABLE[256] = {
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
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

#define BIG5_ISMB_FIRST(byte)  (OnigEncodingBIG5.len_table[byte] > 1)
#define BIG5_ISMB_TRAIL(byte)  BIG5_CAN_BE_TRAIL_TABLE[(byte)]

static UChar*
big5_left_adjust_char_head(UChar* start, UChar* s)
{
  UChar *p;
  int len;

  if (s <= start) return s;
  p = s;

  if (BIG5_ISMB_TRAIL(*p)) {
    while (p > start) {
      if (! BIG5_ISMB_FIRST(*--p)) {
	p++;
	break;
      }
    } 
  }
  len = enc_len(ONIG_ENCODING_BIG5, *p);
  if (p + len > s) return p;
  p += len;
  return p + ((s - p) & ~1);
}

static int
big5_is_allowed_reverse_match(UChar* s, UChar* end)
{
  UChar c = *s;

  return (BIG5_ISMB_TRAIL(c) ? FALSE : TRUE);
}

OnigEncodingType OnigEncodingBIG5 = {
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
  },
  "Big5",    /* name */
  2,         /* max byte length */
  FALSE,     /* is_fold_match */
  ONIGENC_CTYPE_SUPPORT_LEVEL_SB,  /* ctype_support_level */
  FALSE,     /* is continuous sb mb codepoint */
  big5_mbc_to_code,
  onigenc_mb2_code_to_mbclen,
  big5_code_to_mbc,
  big5_mbc_to_lower,
  onigenc_mbn_mbc_is_case_ambig,
  big5_code_is_ctype,
  onigenc_nothing_get_ctype_code_range,
  big5_left_adjust_char_head,
  big5_is_allowed_reverse_match,
  onigenc_nothing_get_all_fold_match_code,
  onigenc_nothing_get_fold_match_info
};
