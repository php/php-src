/**********************************************************************

  euc_jp.c -  Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regenc.h"

#define eucjp_islead(c)    ((UChar )((c) - 0xa1) > 0xfe - 0xa1)

static OnigCodePoint
eucjp_mbc_to_code(UChar* p, UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  c = *p++;
  len = enc_len(ONIG_ENCODING_EUC_JP, c);
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
eucjp_code_to_mbclen(OnigCodePoint code)
{
  if      ((code & 0xff0000) != 0) return 3;
  else if ((code &   0xff00) != 0) return 2;
  else return 1;
}

static int
eucjp_code_to_mbc_first(OnigCodePoint code)
{
  int first;

  if ((code & 0xff0000) != 0) {
    first = (code >> 16) & 0xff;
    /*
    if (enc_len(ONIG_ENCODING_EUC_JP, first) != 3)
      return ONIGERR_INVALID_WIDE_CHAR_VALUE;
    */
  }
  else if ((code & 0xff00) != 0) {
    first = (code >> 8) & 0xff;
    /*
    if (enc_len(ONIG_ENCODING_EUC_JP, first) != 2)
      return ONIGERR_INVALID_WIDE_CHAR_VALUE;
    */
  }
  else {
    /*
    if (enc_len(ONIG_ENCODING_EUC_JP, code) != 1)
      return ONIGERR_INVALID_WIDE_CHAR_VALUE;
    */
    return (int )code;
  }
  return first;
}

static int
eucjp_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff0000) != 0) *p++ = (UChar )(((code >> 16) & 0xff));
  if ((code &   0xff00) != 0) *p++ = (UChar )(((code >>  8) & 0xff));
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enc_len(ONIG_ENCODING_EUC_JP, buf[0]) != (p - buf))
    return ONIGERR_INVALID_WIDE_CHAR_VALUE;
#endif  
  return p - buf;
}

static int
eucjp_mbc_to_lower(UChar* p, UChar* lower)
{
  int len;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    return 1;
  }
  else {
    len = enc_len(ONIG_ENCODING_EUC_JP, *p);
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
eucjp_code_is_ctype(OnigCodePoint code, unsigned int ctype)
{
  if ((ctype & ONIGENC_CTYPE_WORD) != 0) {
    if (code < 128)
      return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
    else {
      int first = eucjp_code_to_mbc_first(code);
      return (enc_len(ONIG_ENCODING_EUC_JP, first) > 1 ? TRUE : FALSE);
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
eucjp_left_adjust_char_head(UChar* start, UChar* s)
{
  /* Assumed in this encoding,
     mb-trail bytes don't mix with single bytes.
  */
  UChar *p;
  int len;

  if (s <= start) return s;
  p = s;

  while (!eucjp_islead(*p) && p > start) p--;
  len = enc_len(ONIG_ENCODING_EUC_JP, *p);
  if (p + len > s) return p;
  p += len;
  return p + ((s - p) & ~1);
}

static int
eucjp_is_allowed_reverse_match(UChar* s, UChar* end)
{
  UChar c = *s;
  if (c <= 0x7e || c == 0x8e || c == 0x8f)
    return TRUE;
  else
    return FALSE;
}

OnigEncodingType OnigEncodingEUC_JP = {
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
  },
  "EUC-JP",   /* name */
  3,          /* max byte length */
  FALSE,      /* is_fold_match */
  ONIGENC_CTYPE_SUPPORT_LEVEL_SB,    /* ctype_support_level */
  FALSE,      /* is continuous sb mb codepoint */
  eucjp_mbc_to_code,
  eucjp_code_to_mbclen,
  eucjp_code_to_mbc,
  eucjp_mbc_to_lower,
  onigenc_mbn_mbc_is_case_ambig,
  eucjp_code_is_ctype,
  onigenc_nothing_get_ctype_code_range,
  eucjp_left_adjust_char_head,
  eucjp_is_allowed_reverse_match,
  onigenc_nothing_get_all_fold_match_code,
  onigenc_nothing_get_fold_match_info
};
