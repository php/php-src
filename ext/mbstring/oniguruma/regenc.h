/**********************************************************************

  regenc.h -  Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef REGENC_H
#define REGENC_H

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
/* internal error */
#define ONIGERR_MEMORY                                         -5
#define ONIGERR_TYPE_BUG                                       -6
/* syntax error [-400, -999] */
#define ONIGERR_INVALID_WIDE_CHAR_VALUE                      -400
#define ONIGERR_TOO_BIG_WIDE_CHAR_VALUE                      -401

#define ONIG_NEWLINE     '\n'
#define ONIG_IS_NEWLINE(c)                ((c) == ONIG_NEWLINE)
#define ONIG_IS_NULL(p)                    (((void*)(p)) == (void*)0)
#define ONIG_IS_NOT_NULL(p)                (((void*)(p)) != (void*)0)
#define ONIG_CHECK_NULL_RETURN(p)          if (ONIG_IS_NULL(p)) return NULL
#define ONIG_CHECK_NULL_RETURN_VAL(p,val)  if (ONIG_IS_NULL(p)) return (val)


#ifdef ONIG_RUBY_M17N

#define ONIG_ENCODING_INIT_DEFAULT            ONIG_ENCODING_UNDEF

#else  /* ONIG_RUBY_M17N */

#define ONIG_ENCODING_INIT_DEFAULT           ONIG_ENCODING_ASCII

/* for encoding system implementation (internal) */
ONIG_EXTERN int onigenc_nothing_get_all_fold_match_code P_((OnigCodePoint** codes));
ONIG_EXTERN int onigenc_nothing_get_fold_match_info P_((UChar* p, UChar* end, OnigEncFoldMatchInfo** info));
ONIG_EXTERN int onigenc_nothing_get_ctype_code_range P_((int ctype, int* nsb, int* nmb, OnigCodePointRange* sbr[], OnigCodePointRange* mbr[]));

/* methods for single byte encoding */
ONIG_EXTERN int onigenc_ascii_mbc_to_lower P_((UChar* p, UChar* lower));
ONIG_EXTERN int onigenc_ascii_mbc_is_case_ambig P_((UChar* p));
ONIG_EXTERN OnigCodePoint onigenc_single_byte_mbc_to_code P_((UChar* p, UChar* end));
ONIG_EXTERN int onigenc_single_byte_code_to_mbclen P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_single_byte_code_to_mbc_first P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_single_byte_code_to_mbc P_((OnigCodePoint code, UChar *buf));
ONIG_EXTERN UChar* onigenc_single_byte_left_adjust_char_head P_((UChar* start, UChar* s));
ONIG_EXTERN int onigenc_single_byte_is_allowed_reverse_match P_((UChar* s, UChar* end));

/* methods for multi byte encoding */
ONIG_EXTERN OnigCodePoint onigenc_mbn_mbc_to_code P_((OnigEncoding enc, UChar* p, UChar* end));
ONIG_EXTERN int onigenc_mbn_mbc_to_lower P_((OnigEncoding enc, UChar* p, UChar* lower));
ONIG_EXTERN int onigenc_mbn_mbc_is_case_ambig P_((UChar* p));
ONIG_EXTERN int onigenc_mb2_code_to_mbclen P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb2_code_to_mbc_first P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb2_code_to_mbc P_((OnigEncoding enc, OnigCodePoint code, UChar *buf));
ONIG_EXTERN int onigenc_mb2_code_is_ctype P_((OnigEncoding enc, OnigCodePoint code, unsigned int ctype));
ONIG_EXTERN int onigenc_mb4_code_to_mbclen P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb4_code_to_mbc_first P_((OnigCodePoint code));
ONIG_EXTERN int onigenc_mb4_code_to_mbc P_((OnigEncoding enc, OnigCodePoint code, UChar *buf));
ONIG_EXTERN int onigenc_mb4_code_is_ctype P_((OnigEncoding enc, OnigCodePoint code, unsigned int ctype));

ONIG_EXTERN int onigenc_get_all_fold_match_code_ss_0xdf P_((OnigCodePoint** codes));
ONIG_EXTERN int onigenc_get_fold_match_info_ss_0xdf P_((UChar* p, UChar* end, OnigEncFoldMatchInfo** info));

#endif /* is not ONIG_RUBY_M17N */


ONIG_EXTERN OnigEncoding  OnigEncDefaultCharEncoding;
ONIG_EXTERN UChar* OnigEncAsciiToLowerCaseTable;
ONIG_EXTERN unsigned short OnigEncAsciiCtypeTable[];

#define ONIGENC_ASCII_CODE_TO_LOWER_CASE(c) OnigEncAsciiToLowerCaseTable[c]
#define ONIGENC_IS_ASCII_CODE_CTYPE(code,ctype) \
  ((OnigEncAsciiCtypeTable[code] & ctype) != 0)
#define ONIGENC_IS_ASCII_CODE_CASE_AMBIG(code) \
    ONIGENC_IS_ASCII_CODE_CTYPE(code, (ONIGENC_CTYPE_UPPER | ONIGENC_CTYPE_LOWER))

#endif /* REGENC_H */
