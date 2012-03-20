/**********************************************************************
  utf_32le.c -  Oniguruma (regular expression library)
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

static int
utf32le_mbc_enc_len(const UChar* p ARG_UNUSED, const OnigUChar* e ARG_UNUSED,
		    OnigEncoding enc ARG_UNUSED)
{
  return 4;
}

static int
utf32le_is_mbc_newline(const UChar* p, const UChar* end,
		       OnigEncoding enc ARG_UNUSED)
{
  if (p + 3 < end) {
    if (*p == 0x0a && *(p+1) == 0 && *(p+2) == 0 && *(p+3) == 0)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((*p == 0x0b ||*p == 0x0c ||*p == 0x0d || *p == 0x85)
	&& *(p+1) == 0x00 && (p+2) == 0x00 && *(p+3) == 0x00)
      return 1;
    if (*(p+1) == 0x20 && (*p == 0x29 || *p == 0x28)
	&& *(p+2) == 0x00 && *(p+3) == 0x00)
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf32le_mbc_to_code(const UChar* p, const UChar* end ARG_UNUSED,
		    OnigEncoding enc ARG_UNUSED)
{
  return (OnigCodePoint )(((p[3] * 256 + p[2]) * 256 + p[1]) * 256 + p[0]);
}

static int
utf32le_code_to_mbclen(OnigCodePoint code ARG_UNUSED,
		       OnigEncoding enc ARG_UNUSED)
{
  return 4;
}

static int
utf32le_code_to_mbc(OnigCodePoint code, UChar *buf,
		    OnigEncoding enc ARG_UNUSED)
{
  UChar* p = buf;

  *p++ = (UChar ) (code & 0xff);
  *p++ = (UChar )((code & 0xff00)     >> 8);
  *p++ = (UChar )((code & 0xff0000)   >>16);
  *p++ = (UChar )((code & 0xff000000) >>24);
  return 4;
}

static int
utf32le_mbc_case_fold(OnigCaseFoldType flag,
		      const UChar** pp, const UChar* end, UChar* fold,
		      OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_ASCII_CODE(*p) && *(p+1) == 0 && *(p+2) == 0 && *(p+3) == 0) {
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (*p == 0x49) {
	*fold++ = 0x31;
	*fold++ = 0x01;
      }
    }
    else {
#endif
      *fold++ = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
      *fold++ = 0;
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    }
#endif

    *fold++ = 0;
    *fold   = 0;
    *pp += 4;
    return 4;
  }
  else
    return onigenc_unicode_mbc_case_fold(enc, flag, pp,
					 end, fold);
}

#if 0
static int
utf32le_is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += 4;

  if (*(p+1) == 0 && *(p+2) == 0 && *(p+3) == 0) {
    int c, v;

    if (*p == 0xdf && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
      return TRUE;
    }

    c = *p;
    v = ONIGENC_IS_UNICODE_ISO_8859_1_BIT_CTYPE(c,
                       (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
    if ((v | BIT_CTYPE_LOWER) != 0) {
      /* 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
      if (c >= 0xaa && c <= 0xba)
	return FALSE;
      else
	return TRUE;
    }
    return (v != 0 ? TRUE : FALSE);
  }

  return FALSE;
}
#endif

static UChar*
utf32le_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end,
			      OnigEncoding enc ARG_UNUSED)
{
  ptrdiff_t rem;

  if (s <= start) return (UChar* )s;

  rem = (int )((s - start) % 4);
  return (UChar* )(s - rem);
}

static int
utf32le_get_case_fold_codes_by_str(OnigCaseFoldType flag,
				   const OnigUChar* p, const OnigUChar* end,
				   OnigCaseFoldCodeItem items[],
				   OnigEncoding enc)
{
  return onigenc_unicode_get_case_fold_codes_by_str(enc,
						    flag, p, end, items);
}

OnigEncodingDefine(utf_32le, UTF_32LE) = {
  utf32le_mbc_enc_len,
  "UTF-32LE",   /* name */
  4,            /* max byte length */
  4,            /* min byte length */
  utf32le_is_mbc_newline,
  utf32le_mbc_to_code,
  utf32le_code_to_mbclen,
  utf32le_code_to_mbc,
  utf32le_mbc_case_fold,
  onigenc_unicode_apply_all_case_fold,
  utf32le_get_case_fold_codes_by_str,
  onigenc_unicode_property_name_to_ctype,
  onigenc_unicode_is_code_ctype,
  onigenc_utf16_32_get_ctype_code_range,
  utf32le_left_adjust_char_head,
  onigenc_always_false_is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_UNICODE,
};
ENC_ALIAS("UCS-4LE", "UTF-32LE")
