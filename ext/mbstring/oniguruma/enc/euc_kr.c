/**********************************************************************

  euc_kr.c -  Oniguruma (regular expression library)

  Copyright (C) 2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regenc.h"

static OnigCodePoint
euckr_mbc_to_code(UChar* p, UChar* end)
{
  return onigenc_mbn_mbc_to_code(ONIG_ENCODING_EUC_KR, p, end);
}

static int
euckr_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  return onigenc_mb2_code_to_mbc(ONIG_ENCODING_EUC_KR, code, buf);
}

static int
euckr_mbc_to_lower(UChar* p, UChar* lower)
{
  return onigenc_mbn_mbc_to_lower(ONIG_ENCODING_EUC_KR, p, lower);
}

static int
euckr_code_is_ctype(OnigCodePoint code, unsigned int ctype)
{
  return onigenc_mb2_code_is_ctype(ONIG_ENCODING_EUC_KR, code, ctype);
}

#define euckr_islead(c)    ((c) < 0xa1 || (c) == 0xff)

static UChar*
euckr_left_adjust_char_head(UChar* start, UChar* s)
{
  /* Assumed in this encoding,
     mb-trail bytes don't mix with single bytes.
  */
  UChar *p;
  int len;

  if (s <= start) return s;
  p = s;

  while (!euckr_islead(*p) && p > start) p--;
  len = enc_len(ONIG_ENCODING_EUC_KR, *p);
  if (p + len > s) return p;
  p += len;
  return p + ((s - p) & ~1);
}

static int
euckr_is_allowed_reverse_match(UChar* s, UChar* end)
{
  UChar c = *s;
  if (c <= 0x7e) return TRUE;
  else           return FALSE;
}

OnigEncodingType OnigEncodingEUC_KR = {
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
  "EUC-KR",   /* name */
  2,          /* max byte length */
  FALSE,      /* is_fold_match */
  ONIGENC_CTYPE_SUPPORT_LEVEL_SB,    /* ctype_support_level */
  FALSE,      /* is continuous sb mb codepoint */
  euckr_mbc_to_code,
  onigenc_mb2_code_to_mbclen,
  euckr_code_to_mbc,
  euckr_mbc_to_lower,
  onigenc_mbn_mbc_is_case_ambig,
  euckr_code_is_ctype,
  onigenc_nothing_get_ctype_code_range,
  euckr_left_adjust_char_head,
  euckr_is_allowed_reverse_match,
  onigenc_nothing_get_all_fold_match_code,
  onigenc_nothing_get_fold_match_info
};

/* Same with OnigEncodingEUC_KR except the name */
OnigEncodingType OnigEncodingEUC_CN = {
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
  "EUC-CN",   /* name */
  2,          /* max byte length */
  FALSE,      /* is_fold_match */
  ONIGENC_CTYPE_SUPPORT_LEVEL_SB,    /* ctype_support_level */
  FALSE,      /* is continuous sb mb codepoint */
  euckr_mbc_to_code,
  onigenc_mb2_code_to_mbclen,
  euckr_code_to_mbc,
  euckr_mbc_to_lower,
  onigenc_mbn_mbc_is_case_ambig,
  euckr_code_is_ctype,
  onigenc_nothing_get_ctype_code_range,
  euckr_left_adjust_char_head,
  euckr_is_allowed_reverse_match,
  onigenc_nothing_get_all_fold_match_code,
  onigenc_nothing_get_fold_match_info
};
