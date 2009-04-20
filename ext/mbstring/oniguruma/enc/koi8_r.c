/**********************************************************************
  koi8_r.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2006  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#define ENC_KOI8_R_TO_LOWER_CASE(c) EncKOI8_R_ToLowerCaseTable[c]
#define ENC_IS_KOI8_R_CTYPE(code,ctype) \
  ((EncKOI8_R_CtypeTable[code] & ctype) != 0)

static const UChar EncKOI8_R_ToLowerCaseTable[256] = {
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
  '\260', '\261', '\262', '\243', '\264', '\265', '\266', '\267',
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

static const unsigned short EncKOI8_R_CtypeTable[256] = {
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x220c, 0x2209, 0x2208, 0x2208, 0x2208, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2284, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0,
  0x38b0, 0x38b0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x31a0,
  0x21a0, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x2008,
  0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x0284, 0x00a0, 0x00a0, 0x10a0, 0x01a0, 0x00a0,
  0x00a0, 0x00a0, 0x00a0, 0x10e2, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x00a0, 0x14a2, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2
};

static int
koi8_r_mbc_to_normalize(OnigAmbigType flag,
                            const UChar** pp, const UChar* end, UChar* lower)
{
  const UChar* p = *pp;

  if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
       ONIGENC_IS_MBC_ASCII(p)) ||
      ((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
       !ONIGENC_IS_MBC_ASCII(p))) {
    *lower = ENC_KOI8_R_TO_LOWER_CASE(*p);
  }
  else {
    *lower = *p;
  }
  (*pp)++;
  return 1; /* return byte length of converted char to lower */
}

static int
koi8_r_is_mbc_ambiguous(OnigAmbigType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp)++;
  if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
       ONIGENC_IS_MBC_ASCII(p)) ||
      ((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
       !ONIGENC_IS_MBC_ASCII(p))) {
    int v = (EncKOI8_R_CtypeTable[*p] &
             (ONIGENC_CTYPE_UPPER | ONIGENC_CTYPE_LOWER));
    return (v != 0 ? TRUE : FALSE);
  }
  return FALSE;
}

static int
koi8_r_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 256)
    return ENC_IS_KOI8_R_CTYPE(code, ctype);
  else
    return FALSE;
}

static int
koi8_r_get_all_pair_ambig_codes(OnigAmbigType flag,
                                const OnigPairAmbigCodes** ccs)
{
  static const OnigPairAmbigCodes cc[] = {
    { 0xa3, 0xb3 },
    { 0xb3, 0xa3 },

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

  if (flag == ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) {
    *ccs = OnigAsciiPairAmbigCodes;
    return 52;
  }
  if (flag == ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) {
    *ccs = cc;
    return sizeof(cc) / sizeof(OnigPairAmbigCodes);
  }
  else
    return 0;
}

OnigEncodingType OnigEncodingKOI8_R = {
  onigenc_single_byte_mbc_enc_len,
  "KOI8-R",       /* name */
  1,             /* max enc length */
  1,             /* min enc length */
  (ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE |
   ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE ),
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  },
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  koi8_r_mbc_to_normalize,
  koi8_r_is_mbc_ambiguous,
  koi8_r_get_all_pair_ambig_codes,
  onigenc_nothing_get_all_comp_ambig_codes,
  koi8_r_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match
};
