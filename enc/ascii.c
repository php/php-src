/**********************************************************************
  ascii.c -  Onigmo (Oniguruma-mod) (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2006  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#include "regenc.h"

OnigEncodingDefine(ascii, ASCII) = {
  onigenc_single_byte_mbc_enc_len,
  "ASCII-8BIT",/* name */
  1,           /* max byte length */
  1,           /* min byte length */
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  onigenc_ascii_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  onigenc_ascii_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_NONE,
};
ENC_ALIAS("BINARY", "ASCII-8BIT")
ENC_REPLICATE("IBM437", "ASCII-8BIT")
ENC_ALIAS("CP437", "IBM437")
ENC_REPLICATE("IBM737", "ASCII-8BIT")
ENC_ALIAS("CP737", "IBM737")
ENC_REPLICATE("IBM775", "ASCII-8BIT")
ENC_ALIAS("CP775", "IBM775")
ENC_REPLICATE("CP850", "ASCII-8BIT")
ENC_ALIAS("IBM850", "CP850")
ENC_REPLICATE("IBM852", "ASCII-8BIT")
ENC_REPLICATE("CP852", "IBM852")
ENC_REPLICATE("IBM855", "ASCII-8BIT")
ENC_REPLICATE("CP855", "IBM855")
ENC_REPLICATE("IBM857", "ASCII-8BIT")
ENC_ALIAS("CP857", "IBM857")
ENC_REPLICATE("IBM860", "ASCII-8BIT")
ENC_ALIAS("CP860", "IBM860")
ENC_REPLICATE("IBM861", "ASCII-8BIT")
ENC_ALIAS("CP861", "IBM861")
ENC_REPLICATE("IBM862", "ASCII-8BIT")
ENC_ALIAS("CP862", "IBM862")
ENC_REPLICATE("IBM863", "ASCII-8BIT")
ENC_ALIAS("CP863", "IBM863")
ENC_REPLICATE("IBM864", "ASCII-8BIT")
ENC_ALIAS("CP864", "IBM864")
ENC_REPLICATE("IBM865", "ASCII-8BIT")
ENC_ALIAS("CP865", "IBM865")
ENC_REPLICATE("IBM866", "ASCII-8BIT")
ENC_ALIAS("CP866", "IBM866")
ENC_REPLICATE("IBM869", "ASCII-8BIT")
ENC_ALIAS("CP869", "IBM869")
ENC_REPLICATE("Windows-1258", "ASCII-8BIT")
ENC_ALIAS("CP1258", "Windows-1258")
ENC_REPLICATE("GB1988", "ASCII-8BIT")
ENC_REPLICATE("macCentEuro", "ASCII-8BIT")
ENC_REPLICATE("macCroatian", "ASCII-8BIT")
ENC_REPLICATE("macCyrillic", "ASCII-8BIT")
ENC_REPLICATE("macGreek", "ASCII-8BIT")
ENC_REPLICATE("macIceland", "ASCII-8BIT")
ENC_REPLICATE("macRoman", "ASCII-8BIT")
ENC_REPLICATE("macRomania", "ASCII-8BIT")
ENC_REPLICATE("macThai", "ASCII-8BIT")
ENC_REPLICATE("macTurkish", "ASCII-8BIT")
ENC_REPLICATE("macUkraine", "ASCII-8BIT")
