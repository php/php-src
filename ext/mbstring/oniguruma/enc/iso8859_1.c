/**********************************************************************
  iso8859_1.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2007  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#define ENC_IS_ISO_8859_1_CTYPE(code,ctype) \
  ((EncISO_8859_1_CtypeTable[code] & CTYPE_TO_BIT(ctype)) != 0)

static const unsigned short EncISO_8859_1_CtypeTable[256] = {
  0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008,
  0x4008, 0x420c, 0x4209, 0x4208, 0x4208, 0x4208, 0x4008, 0x4008,
  0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008,
  0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008,
  0x4284, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0,
  0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0,
  0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0,
  0x78b0, 0x78b0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0,
  0x41a0, 0x7ca2, 0x7ca2, 0x7ca2, 0x7ca2, 0x7ca2, 0x7ca2, 0x74a2,
  0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2,
  0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2,
  0x74a2, 0x74a2, 0x74a2, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x51a0,
  0x41a0, 0x78e2, 0x78e2, 0x78e2, 0x78e2, 0x78e2, 0x78e2, 0x70e2,
  0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2,
  0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2,
  0x70e2, 0x70e2, 0x70e2, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x4008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0284, 0x01a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x30e2, 0x01a0, 0x00a0, 0x01a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x10a0, 0x10a0, 0x00a0, 0x30e2, 0x00a0, 0x01a0,
  0x00a0, 0x10a0, 0x30e2, 0x01a0, 0x10a0, 0x10a0, 0x10a0, 0x01a0,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x00a0,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x00a0,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2
};

static const OnigPairCaseFoldCodes CaseFoldMap[] = {
  { 0xc0, 0xe0 },
  { 0xc1, 0xe1 },
  { 0xc2, 0xe2 },
  { 0xc3, 0xe3 },
  { 0xc4, 0xe4 },
  { 0xc5, 0xe5 },
  { 0xc6, 0xe6 },
  { 0xc7, 0xe7 },
  { 0xc8, 0xe8 },
  { 0xc9, 0xe9 },
  { 0xca, 0xea },
  { 0xcb, 0xeb },
  { 0xcc, 0xec },
  { 0xcd, 0xed },
  { 0xce, 0xee },
  { 0xcf, 0xef },

  { 0xd0, 0xf0 },
  { 0xd1, 0xf1 },
  { 0xd2, 0xf2 },
  { 0xd3, 0xf3 },
  { 0xd4, 0xf4 },
  { 0xd5, 0xf5 },
  { 0xd6, 0xf6 },
  { 0xd8, 0xf8 },
  { 0xd9, 0xf9 },
  { 0xda, 0xfa },
  { 0xdb, 0xfb },
  { 0xdc, 0xfc },
  { 0xdd, 0xfd },
  { 0xde, 0xfe }
};

static int
apply_all_case_fold(OnigCaseFoldType flag,
		    OnigApplyAllCaseFoldFunc f, void* arg)
{
  return onigenc_apply_all_case_fold_with_map(
            sizeof(CaseFoldMap)/sizeof(OnigPairCaseFoldCodes), CaseFoldMap, 1,
            flag, f, arg);
}

static int
get_case_fold_codes_by_str(OnigCaseFoldType flag ARG_UNUSED,
			   const OnigUChar* p, const OnigUChar* end,
			   OnigCaseFoldCodeItem items[])
{
  if (0x41 <= *p && *p <= 0x5a) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p + 0x20);
    if (*p == 0x53 && end > p + 1
	&& (*(p+1) == 0x53 || *(p+1) == 0x73)) { /* SS */
      items[1].byte_len = 2;
      items[1].code_len = 1;
      items[1].code[0] = (OnigCodePoint )0xdf;
      return 2;
    }
    else
      return 1;
  }
  else if (0x61 <= *p && *p <= 0x7a) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p - 0x20);
    if (*p == 0x73 && end > p + 1
	&& (*(p+1) == 0x73 || *(p+1) == 0x53)) { /* ss */
      items[1].byte_len = 2;
      items[1].code_len = 1;
      items[1].code[0] = (OnigCodePoint )0xdf;
      return 2;
    }
    else
      return 1;
  }
  else if (0xc0 <= *p && *p <= 0xcf) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p + 0x20);
    return 1;
  }
  else if (0xd0 <= *p && *p <= 0xdf) {
    if (*p == 0xdf) {
      items[0].byte_len = 1;
      items[0].code_len = 2;
      items[0].code[0] = (OnigCodePoint )'s';
      items[0].code[1] = (OnigCodePoint )'s';

      items[1].byte_len = 1;
      items[1].code_len = 2;
      items[1].code[0] = (OnigCodePoint )'S';
      items[1].code[1] = (OnigCodePoint )'S';

      items[2].byte_len = 1;
      items[2].code_len = 2;
      items[2].code[0] = (OnigCodePoint )'s';
      items[2].code[1] = (OnigCodePoint )'S';

      items[3].byte_len = 1;
      items[3].code_len = 2;
      items[3].code[0] = (OnigCodePoint )'S';
      items[3].code[1] = (OnigCodePoint )'s';

      return 4;
    }
    else if (*p != 0xd7) {
      items[0].byte_len = 1;
      items[0].code_len = 1;
      items[0].code[0] = (OnigCodePoint )(*p + 0x20);
      return 1;
    }
  }
  else if (0xe0 <= *p && *p <= 0xef) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p - 0x20);
    return 1;
  }
  else if (0xf0 <= *p && *p <= 0xfe) {
    if (*p != 0xf7) {
      items[0].byte_len = 1;
      items[0].code_len = 1;
      items[0].code[0] = (OnigCodePoint )(*p - 0x20);
      return 1;
    }
  }

  return 0;
}

static int
mbc_case_fold(OnigCaseFoldType flag, const UChar** pp,
	      const UChar* end ARG_UNUSED, UChar* lower)
{
  const UChar* p = *pp;

  if (*p == 0xdf && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    *lower++ = 's';
    *lower   = 's';
    (*pp)++;
    return 2;
  }

  *lower = ONIGENC_ISO_8859_1_TO_LOWER_CASE(*p);
  (*pp)++;
  return 1;
}

#if 0
static int
is_mbc_ambiguous(OnigCaseFoldType flag,
		 const UChar** pp, const UChar* end)
{
  int v;
  const UChar* p = *pp;

  if (*p == 0xdf && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    (*pp)++;
    return TRUE;
  }

  (*pp)++;
  v = (EncISO_8859_1_CtypeTable[*p] & (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
  if ((v | BIT_CTYPE_LOWER) != 0) {
    /* 0xdf, 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
    if (*p >= 0xaa && *p <= 0xba)
      return FALSE;
    else
      return TRUE;
  }

  return (v != 0 ? TRUE : FALSE);
}
#endif

static int
is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 256)
    return ENC_IS_ISO_8859_1_CTYPE(code, ctype);
  else
    return FALSE;
}

OnigEncodingType OnigEncodingISO_8859_1 = {
  onigenc_single_byte_mbc_enc_len,
  "ISO-8859-1",  /* name */
  1,             /* max enc length */
  1,             /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  mbc_case_fold,
  apply_all_case_fold,
  get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match
};
