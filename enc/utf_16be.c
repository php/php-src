/**********************************************************************
  utf_16be.c -  Oniguruma (regular expression library)
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

#define UTF16_IS_SURROGATE_FIRST(c)    (((c) & 0xfc) == 0xd8)
#define UTF16_IS_SURROGATE_SECOND(c)   (((c) & 0xfc) == 0xdc)
#define UTF16_IS_SURROGATE(c)          (((c) & 0xf8) == 0xd8)

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
utf16be_mbc_enc_len(const UChar* p, const OnigUChar* e ARG_UNUSED,
		    OnigEncoding enc ARG_UNUSED)
{
  int byte = p[0];
  if (!UTF16_IS_SURROGATE(byte)) {
    if (2 <= e-p)
      return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(2);
    else
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
  }
  if (UTF16_IS_SURROGATE_FIRST(byte)) {
    switch (e-p) {
      case 1: return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(3);
      case 2: return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(2);
      case 3:
        if (UTF16_IS_SURROGATE_SECOND(p[2]))
          return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
        break;
      default:
        if (UTF16_IS_SURROGATE_SECOND(p[2]))
          return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(4);
        break;
    }
  }
  return ONIGENC_CONSTRUCT_MBCLEN_INVALID();
}

static int
utf16be_is_mbc_newline(const UChar* p, const UChar* end,
		       OnigEncoding enc)
{
  if (p + 1 < end) {
    if (*(p+1) == 0x0a && *p == 0x00)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((*(p+1) == 0x0b || *(p+1) == 0x0c || *(p+1) == 0x0d || *(p+1) == 0x85)
	&& *p == 0x00)
      return 1;
    if (*p == 0x20 && (*(p+1) == 0x29 || *(p+1) == 0x28))
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf16be_mbc_to_code(const UChar* p, const UChar* end ARG_UNUSED,
		    OnigEncoding enc)
{
  OnigCodePoint code;

  if (UTF16_IS_SURROGATE_FIRST(*p)) {
    code = ((((p[0] << 8) + p[1]) & 0x03ff) << 10)
         + (((p[2] << 8) + p[3]) & 0x03ff) + 0x10000;
  }
  else {
    code = p[0] * 256 + p[1];
  }
  return code;
}

static int
utf16be_code_to_mbclen(OnigCodePoint code,
		       OnigEncoding enc)
{
  return (code > 0xffff ? 4 : 2);
}

static int
utf16be_code_to_mbc(OnigCodePoint code, UChar *buf,
		    OnigEncoding enc)
{
  UChar* p = buf;

  if (code > 0xffff) {
    unsigned int high = (code >> 10) + 0xD7C0;
    unsigned int low = (code & 0x3FF) + 0xDC00;
    *p++ = (high >> 8) & 0xFF;
    *p++ = high & 0xFF;
    *p++ = (low >> 8) & 0xFF;
    *p++ = low & 0xFF;
    return 4;
  }
  else {
    *p++ = (UChar )((code & 0xff00) >> 8);
    *p++ = (UChar )(code & 0xff);
    return 2;
  }
}

static int
utf16be_mbc_case_fold(OnigCaseFoldType flag,
		      const UChar** pp, const UChar* end, UChar* fold,
		      OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_ASCII_CODE(*(p+1)) && *p == 0) {
    p++;
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (*p == 0x49) {
	*fold++ = 0x01;
	*fold   = 0x31;
	(*pp) += 2;
	return 2;
      }
    }
#endif

    *fold++ = 0;
    *fold   = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    *pp += 2;
    return 2;
  }
  else
    return onigenc_unicode_mbc_case_fold(enc, flag,
					 pp, end, fold);
}

#if 0
static int
utf16be_is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += EncLen_UTF16[*p];

  if (*p == 0) {
    int c, v;

    p++;
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
utf16be_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end,
			      OnigEncoding enc ARG_UNUSED)
{
  if (s <= start) return (UChar* )s;

  if ((s - start) % 2 == 1) {
    s--;
  }

  if (UTF16_IS_SURROGATE_SECOND(*s) && s > start + 1)
    s -= 2;

  return (UChar* )s;
}

static int
utf16be_get_case_fold_codes_by_str(OnigCaseFoldType flag,
				   const OnigUChar* p, const OnigUChar* end,
				   OnigCaseFoldCodeItem items[],
				   OnigEncoding enc)
{
  return onigenc_unicode_get_case_fold_codes_by_str(enc,
						    flag, p, end, items);
}

OnigEncodingDefine(utf_16be, UTF_16BE) = {
  utf16be_mbc_enc_len,
  "UTF-16BE",   /* name */
  4,            /* max byte length */
  2,            /* min byte length */
  utf16be_is_mbc_newline,
  utf16be_mbc_to_code,
  utf16be_code_to_mbclen,
  utf16be_code_to_mbc,
  utf16be_mbc_case_fold,
  onigenc_unicode_apply_all_case_fold,
  utf16be_get_case_fold_codes_by_str,
  onigenc_unicode_property_name_to_ctype,
  onigenc_unicode_is_code_ctype,
  onigenc_utf16_32_get_ctype_code_range,
  utf16be_left_adjust_char_head,
  onigenc_always_false_is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_UNICODE,
};
ENC_ALIAS("UCS-2BE", "UTF-16BE")
