/**********************************************************************
  utf16_be.c -  Oniguruma (regular expression library)
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

#include "regint.h"  /* for USE_CALLOUT */


static int
init(void)
{
#ifdef USE_CALLOUT

    int id;
    OnigEncoding enc;
    char* name;
    unsigned int args[4];
    OnigValue opts[4];

    enc = ONIG_ENCODING_UTF16_BE;

    name = "\000F\000A\000I\000L\000\000";            BC0_P(name, fail);
    name = "\000M\000I\000S\000M\000A\000T\000C\000H\000\000"; BC0_P(name, mismatch);

    name = "\000M\000A\000X\000\000";
    args[0] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    args[1] = ONIG_TYPE_CHAR;
    opts[0].c = 'X';
    BC_B_O(name, max, 2, args, 1, opts);

    name = "\000E\000R\000R\000O\000R\000\000";
    args[0] = ONIG_TYPE_LONG; opts[0].l = ONIG_ABORT;
    BC_P_O(name, error, 1, args, 1, opts);

    name = "\000C\000O\000U\000N\000T\000\000";
    args[0] = ONIG_TYPE_CHAR; opts[0].c = '>';
    BC_B_O(name, count, 1, args, 1, opts);

    name = "\000T\000O\000T\000A\000L\000_\000C\000O\000U\000N\000T\000\000";
    args[0] = ONIG_TYPE_CHAR; opts[0].c = '>';
    BC_B_O(name, total_count, 1, args, 1, opts);

    name = "\000C\000M\000P\000\000";
    args[0] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    args[1] = ONIG_TYPE_STRING;
    args[2] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    BC_P(name, cmp, 3, args);

#endif /* USE_CALLOUT */

  return ONIG_NORMAL;
}

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
utf16be_mbc_enc_len(const UChar* p)
{
  return EncLen_UTF16[*p];
}

static int
is_valid_mbc_string(const UChar* s, const UChar* end)
{
  return onigenc_length_check_is_valid_mbc_string(ONIG_ENCODING_UTF16_BE, s, end);
}

static int
utf16be_is_mbc_newline(const UChar* p, const UChar* end)
{
  if (p + 1 < end) {
    if (*(p+1) == 0x0a && *p == 0x00)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((
#ifndef USE_CRNL_AS_LINE_TERMINATOR
         *(p+1) == 0x0d ||
#endif
         *(p+1) == 0x85) && *p == 0x00)
      return 1;

    if (*p == 0x20 && (*(p+1) == 0x29 || *(p+1) == 0x28))
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf16be_mbc_to_code(const UChar* p, const UChar* end)
{
  OnigCodePoint code;

  if (UTF16_IS_SURROGATE_FIRST(*p)) {
    if (end - p < 4) return 0;
    code = ((((p[0] - 0xd8) << 2) + ((p[1] & 0xc0) >> 6) + 1) << 16)
         + ((((p[1] & 0x3f) << 2) + (p[2] - 0xdc)) << 8)
         + p[3];
  }
  else {
    if (end - p < 2) return 0;
    code = p[0] * 256 + p[1];
  }
  return code;
}

static int
utf16be_code_to_mbclen(OnigCodePoint code)
{
  return (code > 0xffff ? 4 : 2);
}

static int
utf16be_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar* p = buf;

  if (code > 0xffff) {
    unsigned int plane, high;

    plane = (code >> 16) - 1;
    *p++ = (plane >> 2) + 0xd8;
    high = (code & 0xff00) >> 8;
    *p++ = ((plane & 0x03) << 6) + (high >> 2);
    *p++ = (high & 0x03) + 0xdc;
    *p   = (UChar )(code & 0xff);
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
		      const UChar** pp, const UChar* end, UChar* fold)
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
    return onigenc_unicode_mbc_case_fold(ONIG_ENCODING_UTF16_BE, flag,
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
utf16be_left_adjust_char_head(const UChar* start, const UChar* s)
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
    const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[])
{
  return onigenc_unicode_get_case_fold_codes_by_str(ONIG_ENCODING_UTF16_BE,
						    flag, p, end, items);
}

OnigEncodingType OnigEncodingUTF16_BE = {
  utf16be_mbc_enc_len,
  "UTF-16BE",   /* name */
  4,            /* max enc length */
  2,            /* min enc length */
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
  init,
  0, /* is_initialized */
  is_valid_mbc_string,
  ENC_FLAG_UNICODE,
  0, 0
};
