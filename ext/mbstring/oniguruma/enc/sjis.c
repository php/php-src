/**********************************************************************

  sjis.c -  Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regenc.h"

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

#define SJIS_ISMB_FIRST(byte)  (OnigEncodingSJIS.len_table[byte] > 1)
#define SJIS_ISMB_TRAIL(byte)  SJIS_CAN_BE_TRAIL_TABLE[(byte)]

static OnigCodePoint
sjis_mbc_to_code(UChar* p, UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  c = *p++;
  len = enc_len(ONIG_ENCODING_SJIS, c);
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
sjis_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff00) != 0) *p++ = (UChar )(((code >>  8) & 0xff));
  *p++ = (UChar )(code & 0xff);

#if 0
  if (enc_len(ONIG_ENCODING_SJIS, buf[0]) != (p - buf))
    return REGERR_INVALID_WIDE_CHAR_VALUE;
#endif
  return p - buf;
}

static int
sjis_mbc_to_lower(UChar* p, UChar* lower)
{
  int len;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    return 1;
  }
  else {
    len = enc_len(ONIG_ENCODING_SJIS, *p);
    if (lower != p) {
      /* memcpy(lower, p, len); */
      int i;
      for (i = 0; i < len; i++) {
	*lower++ = *p++;
      }
    }
    return len; /* return byte length of converted char to lower */
  }
}

static int
sjis_code_is_ctype(OnigCodePoint code, unsigned int ctype)
{
  if ((ctype & ONIGENC_CTYPE_WORD) != 0) {
    if (code < 128)
      return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
    else {
      int first = onigenc_mb2_code_to_mbc_first(code);
      return (enc_len(ONIG_ENCODING_SJIS, first) > 1 ? TRUE : FALSE);
    }

    ctype &= ~ONIGENC_CTYPE_WORD;
    if (ctype == 0) return FALSE;
  }

  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else
    return FALSE;
}

static UChar*
sjis_left_adjust_char_head(UChar* start, UChar* s)
{
  UChar *p;
  int len;

  if (s <= start) return s;
  p = s;

  if (SJIS_ISMB_TRAIL(*p)) {
    while (p > start) {
      if (! SJIS_ISMB_FIRST(*--p)) {
	p++;
	break;
      }
    } 
  }
  len = enc_len(ONIG_ENCODING_SJIS, *p);
  if (p + len > s) return p;
  p += len;
  return p + ((s - p) & ~1);
}

static int
sjis_is_allowed_reverse_match(UChar* s, UChar* end)
{
  UChar c = *s;
  return (SJIS_ISMB_TRAIL(c) ? FALSE : TRUE);
}

OnigEncodingType OnigEncodingSJIS = {
  {
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
  },
  "Shift_JIS",   /* name */
  2,             /* max byte length */
  FALSE,         /* is_fold_match */
  ONIGENC_CTYPE_SUPPORT_LEVEL_SB,     /* ctype_support_level */
  FALSE,         /* is continuous sb mb codepoint */
  sjis_mbc_to_code,
  onigenc_mb2_code_to_mbclen,
  sjis_code_to_mbc,
  sjis_mbc_to_lower,
  onigenc_mbn_mbc_is_case_ambig,
  sjis_code_is_ctype,
  onigenc_nothing_get_ctype_code_range,
  sjis_left_adjust_char_head,
  sjis_is_allowed_reverse_match,
  onigenc_nothing_get_all_fold_match_code,
  onigenc_nothing_get_fold_match_info
};
