/**********************************************************************
  euc_jp.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2020  K.Kosako
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

#define eucjp_islead(c)    ((UChar )((c) - 0xa1) > 0xfe - 0xa1)

static const int EncLen_EUCJP[] = {
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
};

static int
mbc_enc_len(const UChar* p)
{
  return EncLen_EUCJP[*p];
}

static int
is_valid_mbc_string(const UChar* p, const UChar* end)
{
  while (p < end) {
    if (*p < 0x80) {
      p++;
    }
    else if (*p > 0xa0) {
      if (*p == 0xff) return FALSE;
      p++;
      if (p >= end) return FALSE;
      if (*p < 0xa1 || *p == 0xff) return FALSE;
      p++;
    }
    else if (*p == 0x8e) {
      p++;
      if (p >= end) return FALSE;
      if (*p < 0xa1 || *p > 0xdf) return FALSE;
      p++;
    }
    else if (*p == 0x8f) {
      p++;
      if (p >= end) return FALSE;
      if (*p < 0xa1 || *p == 0xff) return FALSE;
      p++;
      if (p >= end) return FALSE;
      if (*p < 0xa1 || *p == 0xff) return FALSE;
      p++;
    }
    else
      return FALSE;
  }

  return TRUE;
}

static OnigCodePoint
mbc_to_code(const UChar* p, const UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  len = enclen(ONIG_ENCODING_EUC_JP, p);
  n = (OnigCodePoint )*p++;
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}

static int
code_to_mbclen(OnigCodePoint code)
{
  if (ONIGENC_IS_CODE_ASCII(code)) return 1;
  else if ((code & 0xff0000) != 0) {
    if (EncLen_EUCJP[(int )(code >> 16) & 0xff] == 3)
      return 3;
  }
  else if ((code & 0xff00) != 0) {
    if (EncLen_EUCJP[(int )(code >> 8) & 0xff] == 2)
      return 2;
  }
  else if (code < 256) {
    if (EncLen_EUCJP[(int )(code & 0xff)] == 1)
      return 1;
  }

  return ONIGERR_INVALID_CODE_POINT_VALUE;
}

static int
code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff0000) != 0) {
    *p++ = (UChar )(((code >> 16) & 0xff));
    *p++ = (UChar )(((code >>  8) & 0xff));
  }
  else if ((code & 0xff00) != 0)
    *p++ = (UChar )(((code >>  8) & 0xff));

  *p++ = (UChar )(code & 0xff);

#if 1
  if (enclen(ONIG_ENCODING_EUC_JP, buf) != (p - buf))
    return ONIGERR_INVALID_CODE_POINT_VALUE;
#endif
  return (int )(p - buf);
}

static int
mbc_case_fold(OnigCaseFoldType flag ARG_UNUSED,
              const UChar** pp, const UChar* end ARG_UNUSED, UChar* lower)
{
  int len;
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    (*pp)++;
    return 1;
  }
  else {
    int i;

    len = enclen(ONIG_ENCODING_EUC_JP, p);
    for (i = 0; i < len; i++) {
      *lower++ = *p++;
    }
    (*pp) += len;
    return len; /* return byte length of converted char to lower */
  }
}

static UChar*
left_adjust_char_head(const UChar* start, const UChar* s)
{
  /* In this encoding
     mb-trail bytes doesn't mix with single bytes.
  */
  const UChar *p;
  int len;

  if (s <= start) return (UChar* )s;
  p = s;

  while (!eucjp_islead(*p) && p > start) p--;
  len = enclen(ONIG_ENCODING_EUC_JP, p);
  if (p + len > s) return (UChar* )p;
  p += len;
  return (UChar* )(p + ((s - p) & ~1));
}

static int
is_allowed_reverse_match(const UChar* s, const UChar* end ARG_UNUSED)
{
  const UChar c = *s;
  if (c <= 0x7e || c == 0x8e || c == 0x8f)
    return TRUE;
  else
    return FALSE;
}


static const OnigCodePoint CR_Hiragana[] = {
  1,
  0xa4a1, 0xa4f3
}; /* CR_Hiragana */

static const OnigCodePoint CR_Katakana[] = {
  3,
  0xa5a1, 0xa5f6,
  0xaaa6, 0xaaaf,
  0xaab1, 0xaadd
}; /* CR_Katakana */

static const OnigCodePoint* PropertyList[] = {
  CR_Hiragana,
  CR_Katakana
};

static int
property_name_to_ctype(OnigEncoding enc, UChar* p, UChar* end)
{
  struct PropertyNameCtype* pc;
  int len = (int )(end - p);
  char q[32];

  if (len < sizeof(q) - 1) {
    xmemcpy(q, p, (size_t )len);
    q[len] = '\0';
    pc = onigenc_euc_jp_lookup_property_name(q, len);
    if (pc != 0)
      return pc->ctype;
  }

  return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
}

static int
is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (ctype <= ONIGENC_MAX_STD_CTYPE) {
    if (code < 128)
      return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
    else {
      if (CTYPE_IS_WORD_GRAPH_PRINT(ctype)) {
        return (code_to_mbclen(code) > 1 ? TRUE : FALSE);
      }
    }
  }
  else {
    ctype -= (ONIGENC_MAX_STD_CTYPE + 1);
    if (ctype >= (unsigned int )(sizeof(PropertyList)/sizeof(PropertyList[0])))
      return ONIGERR_TYPE_BUG;

    return onig_is_in_code_range((UChar* )PropertyList[ctype], code);
  }

  return FALSE;
}

static int
get_ctype_code_range(OnigCtype ctype, OnigCodePoint* sb_out,
                     const OnigCodePoint* ranges[])
{
  if (ctype <= ONIGENC_MAX_STD_CTYPE) {
    return ONIG_NO_SUPPORT_CONFIG;
  }
  else {
    *sb_out = 0x80;

    ctype -= (ONIGENC_MAX_STD_CTYPE + 1);
    if (ctype >= (OnigCtype )sizeof(PropertyList)/sizeof(PropertyList[0]))
      return ONIGERR_TYPE_BUG;

    *ranges = PropertyList[ctype];
    return 0;
  }
}


OnigEncodingType OnigEncodingEUC_JP = {
  mbc_enc_len,
  "EUC-JP",   /* name */
  3,          /* max enc length */
  1,          /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  mbc_to_code,
  code_to_mbclen,
  code_to_mbc,
  mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  property_name_to_ctype,
  is_code_ctype,
  get_ctype_code_range,
  left_adjust_char_head,
  is_allowed_reverse_match,
  NULL, /* init */
  NULL, /* is_initialized */
  is_valid_mbc_string,
  ENC_FLAG_ASCII_COMPATIBLE|ENC_FLAG_SKIP_OFFSET_1_OR_0,
  0, 0
};
