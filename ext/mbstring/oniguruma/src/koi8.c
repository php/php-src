/**********************************************************************
  koi8.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2018  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#define ENC_KOI8_TO_LOWER_CASE(c) EncKOI8_ToLowerCaseTable[c]
#define ENC_IS_KOI8_CTYPE(code,ctype) \
  ((EncKOI8_CtypeTable[code] & CTYPE_TO_BIT(ctype)) != 0)

static const UChar EncKOI8_ToLowerCaseTable[256] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337'
};

static const unsigned short EncKOI8_CtypeTable[256] = {
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
  0x0284, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2
};


static int
koi8_mbc_case_fold(OnigCaseFoldType flag ARG_UNUSED,
	   const UChar** pp, const UChar* end ARG_UNUSED, UChar* lower)
{
  const UChar* p = *pp;

  *lower = ENC_KOI8_TO_LOWER_CASE(*p);
  (*pp)++;
  return 1;
}

#if 0
static int
koi8_is_mbc_ambiguous(OnigAmbigType flag, const OnigUChar** pp, const OnigUChar* end)
{
  const OnigUChar* p = *pp;

  (*pp)++;
  if (((flag & ONIGENC_CASE_FOLD_ASCII_CASE) != 0 &&
       ONIGENC_IS_MBC_ASCII(p)) ||
      ((flag & ONIGENC_CASE_FOLD_NONASCII_CASE) != 0 &&
       !ONIGENC_IS_MBC_ASCII(p))) {
    int v = (EncKOI8_CtypeTable[*p] &
             (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
    return (v != 0 ? TRUE : FALSE);
  }
  return FALSE;
}
#endif

static int
koi8_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 256)
    return ENC_IS_KOI8_CTYPE(code, ctype);
  else
    return FALSE;
}

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
 { 0xd7, 0xf7 },
 { 0xd8, 0xf8 },
 { 0xd9, 0xf9 },
 { 0xda, 0xfa },
 { 0xdb, 0xfb },
 { 0xdc, 0xfc },
 { 0xdd, 0xfd },
 { 0xde, 0xfe },
 { 0xdf, 0xff },

 { 0xe0, 0xc0 },
 { 0xe1, 0xc1 },
 { 0xe2, 0xc2 },
 { 0xe3, 0xc3 },
 { 0xe4, 0xc4 },
 { 0xe5, 0xc5 },
 { 0xe6, 0xc6 },
 { 0xe7, 0xc7 },
 { 0xe8, 0xc8 },
 { 0xe9, 0xc9 },
 { 0xea, 0xca },
 { 0xeb, 0xcb },
 { 0xec, 0xcc },
 { 0xed, 0xcd },
 { 0xee, 0xce },
 { 0xef, 0xcf },

 { 0xf0, 0xd0 },
 { 0xf1, 0xd1 },
 { 0xf2, 0xd2 },
 { 0xf3, 0xd3 },
 { 0xf4, 0xd4 },
 { 0xf5, 0xd5 },
 { 0xf6, 0xd6 },
 { 0xf7, 0xd7 },
 { 0xf8, 0xd8 },
 { 0xf9, 0xd9 },
 { 0xfa, 0xda },
 { 0xfb, 0xdb },
 { 0xfc, 0xdc },
 { 0xfe, 0xde },
 { 0xff, 0xdf }
};

static int
koi8_apply_all_case_fold(OnigCaseFoldType flag,
			 OnigApplyAllCaseFoldFunc f, void* arg)
{
  return onigenc_apply_all_case_fold_with_map(
             sizeof(CaseFoldMap)/sizeof(OnigPairCaseFoldCodes), CaseFoldMap, 0,
             flag, f, arg);
}

static int
koi8_get_case_fold_codes_by_str(OnigCaseFoldType flag,
    const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[])
{
  return onigenc_get_case_fold_codes_by_str_with_map(
	     sizeof(CaseFoldMap)/sizeof(OnigPairCaseFoldCodes), CaseFoldMap, 0,
	     flag, p, end, items);
}

OnigEncodingType OnigEncodingKOI8 = {
  onigenc_single_byte_mbc_enc_len,
  "KOI8",        /* name */
  1,             /* max enc length */
  1,             /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  koi8_mbc_case_fold,
  koi8_apply_all_case_fold,
  koi8_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  koi8_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match,
  NULL, /* init */
  NULL, /* is_initialized */
  onigenc_always_true_is_valid_mbc_string,
  ENC_FLAG_ASCII_COMPATIBLE,
  0, 0
};
