/**********************************************************************
  cp932.c -  Onigmo (Oniguruma-mod) (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2009  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
 * Copyright (c) 2011       K.Takata  <kentkt AT csc DOT jp>
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

#define ENC_CP932
#include "shift_jis.c"

OnigEncodingDefine(windows_31j, Windows_31J) = {
  mbc_enc_len,
  "Windows-31J",   /* name */
  2,             /* max byte length */
  1,             /* min byte length */
  onigenc_is_mbc_newline_0x0a,
  mbc_to_code,
  code_to_mbclen,
  code_to_mbc,
  mbc_case_fold,
  apply_all_case_fold,
  get_case_fold_codes_by_str,
  property_name_to_ctype,
  is_code_ctype,
  get_ctype_code_range,
  left_adjust_char_head,
  is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_NONE,
};
/*
 * Name: Windows-31J
 * MIBenum: 2024
 * Link: http://www.iana.org/assignments/character-sets
 * Link: http://www.microsoft.com/globaldev/reference/dbcs/932.mspx
 * Link: http://ja.wikipedia.org/wiki/Windows-31J
 * Link: http://source.icu-project.org/repos/icu/data/trunk/charset/data/ucm/windows-932-2000.ucm
 *
 * Windows Standard Character Set and its mapping to Unicode by Microsoft.
 * Since 1.9.3, SJIS is the alias of Windows-31J because its character
 * set is usually this one even if its mapping may differ.
 */
ENC_ALIAS("CP932", "Windows-31J")
ENC_ALIAS("csWindows31J", "Windows-31J") /* IANA.  IE6 don't accept Windows-31J but csWindows31J. */
ENC_ALIAS("SJIS", "Windows-31J")

/*
 * Name: PCK
 * Link: http://download.oracle.com/docs/cd/E19253-01/819-0606/x-2chn0/index.html
 * Link: http://download.oracle.com/docs/cd/E19253-01/819-0606/appb-pckwarn-1/index.html
 *
 * Solaris's SJIS variant. Its set is Windows Standard Character Set; it
 * consists JIS X 0201 Latin (US-ASCII), JIS X 0201 Katakana, JIS X 0208, NEC
 * special characters, NEC-selected IBM extended characters, and IBM extended
 * characters. Solaris's iconv seems to use SJIS-open.
 */
ENC_ALIAS("PCK", "Windows-31J")
