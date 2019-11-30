/**********************************************************************
  euc_kr.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2019  K.Kosako
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

#include "regenc.h"

static const int EncLen_EUCKR[] = {
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
};

static int
euckr_mbc_enc_len(const UChar* p)
{
  return EncLen_EUCKR[*p];
}

static int
euckr_code_to_mbclen(OnigCodePoint code)
{
  if ((code & (~0xffff)) != 0) return ONIGERR_INVALID_CODE_POINT_VALUE;
  if ((code &    0xff00) != 0) return 2;
  if (EncLen_EUCKR[(int )(code & 0xff)] == 1) return 1;

  return ONIGERR_INVALID_CODE_POINT_VALUE;
}

static int
is_valid_mbc_string(const UChar* p, const UChar* end)
{
  while (p < end) {
    if (*p < 0x80) {
      p++;
    }
    else if (*p < 0xa1) {
      return FALSE;
    }
    else if (*p < 0xff) {
      p++;
      if (p >= end) return FALSE;
      if (*p < 0xa1 || *p == 0xff)
        return FALSE;
      p++;
    }
    else
      return FALSE;
  }

  return TRUE;
}

static OnigCodePoint
euckr_mbc_to_code(const UChar* p, const UChar* end)
{
  return onigenc_mbn_mbc_to_code(ONIG_ENCODING_EUC_KR, p, end);
}

static int
euckr_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  return onigenc_mb2_code_to_mbc(ONIG_ENCODING_EUC_KR, code, buf);
}

static int
euckr_mbc_case_fold(OnigCaseFoldType flag, const UChar** pp, const UChar* end,
                    UChar* lower)
{
  return onigenc_mbn_mbc_case_fold(ONIG_ENCODING_EUC_KR, flag,
                                   pp, end, lower);
}

static int
euckr_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  return onigenc_mb2_is_code_ctype(ONIG_ENCODING_EUC_KR, code, ctype);
}

#define euckr_islead(c)    ((c) < 0xa1 || (c) == 0xff)

static UChar*
euckr_left_adjust_char_head(const UChar* start, const UChar* s)
{
  /* Assumed in this encoding,
     mb-trail bytes don't mix with single bytes.
  */
  const UChar *p;
  int len;

  if (s <= start) return (UChar* )s;
  p = s;

  while (!euckr_islead(*p) && p > start) p--;
  len = enclen(ONIG_ENCODING_EUC_KR, p);
  if (p + len > s) return (UChar* )p;
  p += len;
  return (UChar* )(p + ((s - p) & ~1));
}

static int
euckr_is_allowed_reverse_match(const UChar* s, const UChar* end ARG_UNUSED)
{
  const UChar c = *s;
  if (c <= 0x7e) return TRUE;
  else           return FALSE;
}

OnigEncodingType OnigEncodingEUC_KR = {
  euckr_mbc_enc_len,
  "EUC-KR",   /* name */
  2,          /* max enc length */
  1,          /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  euckr_mbc_to_code,
  euckr_code_to_mbclen,
  euckr_code_to_mbc,
  euckr_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  euckr_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  euckr_left_adjust_char_head,
  euckr_is_allowed_reverse_match,
  NULL, /* init */
  NULL, /* is_initialized */
  is_valid_mbc_string,
  ENC_FLAG_ASCII_COMPATIBLE|ENC_FLAG_SKIP_OFFSET_1_OR_0,
  0, 0  
};

/* Same with OnigEncodingEUC_KR except the name */
OnigEncodingType OnigEncodingEUC_CN = {
  euckr_mbc_enc_len,
  "EUC-CN",   /* name */
  2,          /* max enc length */
  1,          /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  euckr_mbc_to_code,
  euckr_code_to_mbclen,
  euckr_code_to_mbc,
  euckr_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  euckr_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  euckr_left_adjust_char_head,
  euckr_is_allowed_reverse_match,
  NULL, /* init */
  NULL, /* is_initialized */
  is_valid_mbc_string,
  ENC_FLAG_ASCII_COMPATIBLE|ENC_FLAG_SKIP_OFFSET_1_OR_0,
  0, 0
};
