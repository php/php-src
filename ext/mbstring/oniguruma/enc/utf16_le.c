/**********************************************************************
  utf16_le.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2008  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#define UTF16_IS_SURROGATE_FIRST(c)    (c >= 0xd8 && c <= 0xdb)
#define UTF16_IS_SURROGATE_SECOND(c)   (c >= 0xdc && c <= 0xdf)

static const int EncLen_UTF16[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static int
utf16le_code_to_mbclen(OnigCodePoint code)
{
  return (code > 0xffff ? 4 : 2);
}

static int
utf16le_mbc_enc_len(const UChar* p)
{
  return EncLen_UTF16[*(p+1)];
}

static int
utf16le_is_mbc_newline(const UChar* p, const UChar* end)
{
  if (p + 1 < end) {
    if (*p == 0x0a && *(p+1) == 0x00)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((*p == 0x0d || *p == 0x85) && *(p+1) == 0x00)
      return 1;
    if (*(p+1) == 0x20 && (*p == 0x29 || *p == 0x28))
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf16le_mbc_to_code(const UChar* p, const UChar* end)
{
  OnigCodePoint code;
  UChar c0 = *p;
  UChar c1 = *(p+1);

  if (UTF16_IS_SURROGATE_FIRST(c1)) {
    code = ((((c1 - 0xd8) << 2) + ((c0  & 0xc0) >> 6) + 1) << 16)
         + ((((c0 & 0x3f) << 2) + (p[3] - 0xdc)) << 8)
         + p[2];
  }
  else {
    code = c1 * 256 + p[0];
  }
  return code;
}

static int
utf16le_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar* p = buf;

  if (code > 0xffff) {
    unsigned int plane, high;

    plane = code >> 16;
    high = (code & 0xff00) >> 8;

    *p++ = ((plane & 0x03) << 6) + (high >> 2);
    *p++ = (plane >> 2) + 0xd8;
    *p++ = (UChar )(code & 0xff);
    *p   = (high & 0x02) + 0xdc;
    return 4;
  }
  else {
    *p++ = (UChar )(code & 0xff);
    *p++ = (UChar )((code & 0xff00) >> 8);
    return 2;
  }
}

static int
utf16le_mbc_to_normalize(OnigAmbigType flag, const UChar** pp, const UChar* end,
                         UChar* lower)
{
  const UChar* p = *pp;

  if (*(p+1) == 0) {
    if (end > p + 3 &&
	(flag & ONIGENC_AMBIGUOUS_MATCH_COMPOUND) != 0 &&
	((*p == 's' && *(p+2) == 's') ||
	 ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	  (*p == 'S' && *(p+2) == 'S'))) &&
        *(p+3) == 0) {
      *lower++ = 0xdf;
      *lower   = '\0';
      (*pp) += 4;
      return 2;
    }

    *(lower+1) = '\0';
    if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	 ONIGENC_IS_MBC_ASCII(p)) ||
	((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
	 !ONIGENC_IS_MBC_ASCII(p))) {
      *lower = ONIGENC_ISO_8859_1_TO_LOWER_CASE(*p);
    }
    else {
      *lower = *p;
    }
    (*pp) += 2;
    return 2;  /* return byte length of converted char to lower */
  }
  else {
    int len = EncLen_UTF16[*(p+1)];
    if (lower != p) {
      int i;
      for (i = 0; i < len; i++) {
	*lower++ = *p++;
      }
    }
    (*pp) += len;
    return len; /* return byte length of converted char to lower */
  }
}

static int
utf16le_is_mbc_ambiguous(OnigAmbigType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += EncLen_UTF16[*(p+1)];

  if (*(p+1) == 0) {
    int c, v;

    if ((flag & ONIGENC_AMBIGUOUS_MATCH_COMPOUND) != 0) {
      if (end > p + 3 &&
	  ((*p == 's' && *(p+2) == 's') ||
	   ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	    (*p == 'S' && *(p+2) == 'S'))) &&
          *(p+3) == 0) {
        (*pp) += 2;
        return TRUE;
      }
    }

    if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	 ONIGENC_IS_MBC_ASCII(p)) ||
	((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
	 !ONIGENC_IS_MBC_ASCII(p))) {
      c = *p;
      v = ONIGENC_IS_UNICODE_ISO_8859_1_CTYPE(c,
                       (ONIGENC_CTYPE_UPPER | ONIGENC_CTYPE_LOWER));
      if ((v | ONIGENC_CTYPE_LOWER) != 0) {
        /* 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
        if (c >= 0xaa && c <= 0xba)
          return FALSE;
        else
          return TRUE;
      }
      return (v != 0 ? TRUE : FALSE);
    }
  }

  return FALSE;
}

static UChar*
utf16le_left_adjust_char_head(const UChar* start, const UChar* s)
{
  if (s <= start) return (UChar* )s;

  if ((s - start) % 2 == 1) {
    s--;
  }

  if (UTF16_IS_SURROGATE_SECOND(*(s+1)) && s > start + 1)
    s -= 2;

  return (UChar* )s;
}

OnigEncodingType OnigEncodingUTF16_LE = {
  utf16le_mbc_enc_len,
  "UTF-16LE",   /* name */
  4,            /* max byte length */
  2,            /* min byte length */
  (ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE |
   ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE |
   ONIGENC_AMBIGUOUS_MATCH_COMPOUND),
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  },
  utf16le_is_mbc_newline,
  utf16le_mbc_to_code,
  utf16le_code_to_mbclen,
  utf16le_code_to_mbc,
  utf16le_mbc_to_normalize,
  utf16le_is_mbc_ambiguous,
  onigenc_iso_8859_1_get_all_pair_ambig_codes,
  onigenc_ess_tsett_get_all_comp_ambig_codes,
  onigenc_unicode_is_code_ctype,
  onigenc_unicode_get_ctype_code_range,
  utf16le_left_adjust_char_head,
  onigenc_always_false_is_allowed_reverse_match
};
