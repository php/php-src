#ifndef REGENC_H
#define REGENC_H
/**********************************************************************
  regenc.h -  Oniguruma (regular expression library)
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

#ifndef RUBY_PLATFORM
#include "config.h"
#endif
#include "oniguruma.h"

#ifndef NULL
#define NULL   ((void* )0)
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

/* error codes */
#define ONIGENCERR_MEMORY                                         -5
#define ONIGENCERR_TYPE_BUG                                       -6
#define ONIGENCERR_INVALID_WIDE_CHAR_VALUE                      -400
#define ONIGENCERR_TOO_BIG_WIDE_CHAR_VALUE                      -401

#define ONIG_IS_NULL(p)                    (((void*)(p)) == (void*)0)
#define ONIG_IS_NOT_NULL(p)                (((void*)(p)) != (void*)0)
#define ONIG_CHECK_NULL_RETURN(p)          if (ONIG_IS_NULL(p)) return NULL
#define ONIG_CHECK_NULL_RETURN_VAL(p,val)  if (ONIG_IS_NULL(p)) return (val)


#ifdef ONIG_RUBY_M17N

#define ONIG_ENCODING_INIT_DEFAULT            ONIG_ENCODING_UNDEF

#else  /* ONIG_RUBY_M17N */

#define USE_UNICODE_FULL_RANGE_CTYPE
/* following must not use with USE_CRNL_AS_LINE_TERMINATOR */
/* #define USE_UNICODE_ALL_LINE_TERMINATORS */  /* see Unicode.org UTF#18 */

#define ONIG_ENCODING_INIT_DEFAULT           ONIG_ENCODING_ASCII

/* for encoding system implementation (internal) */
ONIG_EXTERN int onigenc_ascii_get_all_pair_ambig_codes P_((OnigAmbigType flag, const OnigPairAmbigCodes** acs));
ONIG_EXTERN int onigenc_nothing_get_all_comp_ambig_codes P_((OnigAmbigType flag, const OnigCompAmbigCodes** acs));
ONIG_EXTERN int onigenc_iso_8859_1_get_all_pair_ambig_codes P_((OnigAmbigType flag, const OnigPairAmbigCodes** acs));
ONIG_EXTERN int onigenc_ess_tsett_get_all_comp_ambig_codes P_((OnigAmbigType flag, const OnigCompAmbigCodes** acs));
ONIG_EXTERN int onigenc_not_support_get_ctype_code_range P_((int ctype, const OnigCodePoint* sbr[], const OnigCodePoint* mbr[]));
ONIG_EXTERN int onigenc_is_mbc_newline_0x0a P_((const UChar* p, const UChar* end));

/* methods for single byte encoding */
ONIG_EXTERN int onigenc_ascii_mbc_to_normalize P_((OnigAmbigType flag, const UChar** p, const UChar* end, UChar* lower));
ONIG_EXTERN int onigenc_ascii_is_mbc_ambiguous P_((OnigAmbigType flag, const UChar** p, const UChar* end));
ONIG_EXTERN int onigenc_single_byte_mbc_enc_len P_((const UChar* p));
ONIG_EXTERN OnigCodePoint onigenc_single_byte_mbc_to_code P_((const UChar* p, const UChar* end));
ONIG_EXTERN int onigenc_single_byte_code_to_mbclen P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_single_byte_code_to_mbc_first P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_single_byte_code_to_mbc P_((OnigCodePoint code, UChar *buf));
ONIG_EXTERN UChar* onigenc_single_byte_left_adjust_char_head P_((const UChar* start, const UChar* s));
ONIG_EXTERN int onigenc_always_true_is_allowed_reverse_match P_((const UChar* s, const UChar* end));
ONIG_EXTERN int onigenc_always_false_is_allowed_reverse_match P_((const UChar* s, const UChar* end));

/* methods for multi byte encoding */
ONIG_EXTERN OnigCodePoint onigenc_mbn_mbc_to_code P_((OnigEncoding enc, const UChar* p, const UChar* end));
ONIG_EXTERN int onigenc_mbn_mbc_to_normalize P_((OnigEncoding enc, OnigAmbigType flag, const UChar** p, const UChar* end, UChar* lower));
ONIG_EXTERN int onigenc_mbn_is_mbc_ambiguous P_((OnigEncoding enc, OnigAmbigType flag, const UChar** p, const UChar* end));
ONIG_EXTERN int onigenc_mb2_code_to_mbclen P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb2_code_to_mbc_first P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb2_code_to_mbc P_((OnigEncoding enc, OnigCodePoint code, UChar *buf));
ONIG_EXTERN int onigenc_mb2_is_code_ctype P_((OnigEncoding enc, OnigCodePoint code, unsigned int ctype));
ONIG_EXTERN int onigenc_mb4_code_to_mbclen P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb4_code_to_mbc_first P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb4_code_to_mbc P_((OnigEncoding enc, OnigCodePoint code, UChar *buf));
ONIG_EXTERN int onigenc_mb4_is_code_ctype P_((OnigEncoding enc, OnigCodePoint code, unsigned int ctype));

ONIG_EXTERN int onigenc_get_all_fold_match_code_ss_0xdf P_((OnigCodePoint** codes));

/* in enc/unicode.c */
ONIG_EXTERN int onigenc_unicode_is_code_ctype P_((OnigCodePoint code, unsigned int ctype));
ONIG_EXTERN int onigenc_unicode_get_ctype_code_range P_((int ctype, const OnigCodePoint* sbr[], const OnigCodePoint* mbr[]));


#define ONIGENC_ISO_8859_1_TO_LOWER_CASE(c) \
  OnigEncISO_8859_1_ToLowerCaseTable[c]
#define ONIGENC_ISO_8859_1_TO_UPPER_CASE(c) \
  OnigEncISO_8859_1_ToUpperCaseTable[c]
#define ONIGENC_IS_UNICODE_ISO_8859_1_CTYPE(code,ctype) \
  ((OnigEnc_Unicode_ISO_8859_1_CtypeTable[code] & ctype) != 0)

ONIG_EXTERN const UChar OnigEncISO_8859_1_ToLowerCaseTable[];
ONIG_EXTERN const UChar OnigEncISO_8859_1_ToUpperCaseTable[];
ONIG_EXTERN const unsigned short OnigEnc_Unicode_ISO_8859_1_CtypeTable[];
ONIG_EXTERN const OnigPairAmbigCodes OnigAsciiPairAmbigCodes[];

#endif /* is not ONIG_RUBY_M17N */

ONIG_EXTERN int
onigenc_with_ascii_strncmp P_((OnigEncoding enc, const UChar* p, const UChar* end, const UChar* sascii /* ascii */, int n));
ONIG_EXTERN UChar*
onigenc_step P_((OnigEncoding enc, const UChar* p, const UChar* end, int n));

/* defined in regexec.c, but used in enc/xxx.c */
extern int  onig_is_in_code_range P_((const UChar* p, OnigCodePoint code));

ONIG_EXTERN OnigEncoding  OnigEncDefaultCharEncoding;
ONIG_EXTERN const UChar* OnigEncAsciiToLowerCaseTable;
ONIG_EXTERN const UChar  OnigEncAsciiToUpperCaseTable[];
ONIG_EXTERN const unsigned short OnigEncAsciiCtypeTable[];

#define ONIGENC_ASCII_CODE_TO_LOWER_CASE(c) OnigEncAsciiToLowerCaseTable[c]
#define ONIGENC_ASCII_CODE_TO_UPPER_CASE(c) OnigEncAsciiToUpperCaseTable[c]
#define ONIGENC_IS_ASCII_CODE_CTYPE(code,ctype) \
  ((OnigEncAsciiCtypeTable[code] & ctype) != 0)
#define ONIGENC_IS_ASCII_CODE_CASE_AMBIG(code) \
    ONIGENC_IS_ASCII_CODE_CTYPE(code, (ONIGENC_CTYPE_UPPER | ONIGENC_CTYPE_LOWER))

#endif /* REGENC_H */
