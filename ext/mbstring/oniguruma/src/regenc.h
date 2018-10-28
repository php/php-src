#ifndef REGENC_H
#define REGENC_H
/**********************************************************************
  regenc.h -  Oniguruma (regular expression library)
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

#ifndef ONIGURUMA_EXPORT
#define ONIGURUMA_EXPORT
#endif

#include "config.h"
#include <stddef.h>

#ifdef ONIG_ESCAPE_UCHAR_COLLISION
#undef ONIG_ESCAPE_UCHAR_COLLISION
#endif

#include "oniguruma.h"

typedef struct {
  OnigCodePoint from;
  OnigCodePoint to;
} OnigPairCaseFoldCodes;


#ifndef NULL
#define NULL   ((void* )0)
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef ARG_UNUSED
#if defined(__GNUC__)
#  define ARG_UNUSED  __attribute__ ((unused))
#else
#  define ARG_UNUSED
#endif
#endif

#define ONIG_IS_NULL(p)                    (((void*)(p)) == (void*)0)
#define ONIG_IS_NOT_NULL(p)                (((void*)(p)) != (void*)0)
#define ONIG_CHECK_NULL_RETURN(p)          if (ONIG_IS_NULL(p)) return NULL
#define ONIG_CHECK_NULL_RETURN_VAL(p,val)  if (ONIG_IS_NULL(p)) return (val)

#define MAX_CODE_POINT         (~((OnigCodePoint )0))

#define enclen(enc,p)          ONIGENC_MBC_ENC_LEN(enc,p)

/* character types bit flag */
#define BIT_CTYPE_NEWLINE  (1<< ONIGENC_CTYPE_NEWLINE)
#define BIT_CTYPE_ALPHA    (1<< ONIGENC_CTYPE_ALPHA)
#define BIT_CTYPE_BLANK    (1<< ONIGENC_CTYPE_BLANK)
#define BIT_CTYPE_CNTRL    (1<< ONIGENC_CTYPE_CNTRL)
#define BIT_CTYPE_DIGIT    (1<< ONIGENC_CTYPE_DIGIT)
#define BIT_CTYPE_GRAPH    (1<< ONIGENC_CTYPE_GRAPH)
#define BIT_CTYPE_LOWER    (1<< ONIGENC_CTYPE_LOWER)
#define BIT_CTYPE_PRINT    (1<< ONIGENC_CTYPE_PRINT)
#define BIT_CTYPE_PUNCT    (1<< ONIGENC_CTYPE_PUNCT)
#define BIT_CTYPE_SPACE    (1<< ONIGENC_CTYPE_SPACE)
#define BIT_CTYPE_UPPER    (1<< ONIGENC_CTYPE_UPPER)
#define BIT_CTYPE_XDIGIT   (1<< ONIGENC_CTYPE_XDIGIT)
#define BIT_CTYPE_WORD     (1<< ONIGENC_CTYPE_WORD)
#define BIT_CTYPE_ALNUM    (1<< ONIGENC_CTYPE_ALNUM)
#define BIT_CTYPE_ASCII    (1<< ONIGENC_CTYPE_ASCII)

#define CTYPE_TO_BIT(ctype)  (1<<(ctype))
#define CTYPE_IS_WORD_GRAPH_PRINT(ctype) \
  ((ctype) == ONIGENC_CTYPE_WORD || (ctype) == ONIGENC_CTYPE_GRAPH ||\
   (ctype) == ONIGENC_CTYPE_PRINT)


typedef struct {
  UChar    *name;
  int       ctype;
  short int len;
} PosixBracketEntryType;

struct PropertyNameCtype {
  char *name;
  int ctype;
};

/* #define USE_CRNL_AS_LINE_TERMINATOR */
#define USE_UNICODE_PROPERTIES
#define USE_UNICODE_EXTENDED_GRAPHEME_CLUSTER
/* #define USE_UNICODE_CASE_FOLD_TURKISH_AZERI */
/* #define USE_UNICODE_ALL_LINE_TERMINATORS */  /* see Unicode.org UTS #18 */


#define ONIG_ENCODING_INIT_DEFAULT           ONIG_ENCODING_ASCII


#define ENC_FLAG_ASCII_COMPATIBLE      (1<<0)
#define ENC_FLAG_UNICODE               (1<<1)


/* for encoding system implementation (internal) */
extern int onigenc_end(void);
extern int onigenc_ascii_apply_all_case_fold P_((OnigCaseFoldType flag, OnigApplyAllCaseFoldFunc f, void* arg));
extern int onigenc_ascii_get_case_fold_codes_by_str P_((OnigCaseFoldType flag, const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[]));
extern int onigenc_apply_all_case_fold_with_map P_((int map_size, const OnigPairCaseFoldCodes map[], int ess_tsett_flag, OnigCaseFoldType flag, OnigApplyAllCaseFoldFunc f, void* arg));
extern int onigenc_get_case_fold_codes_by_str_with_map P_((int map_size, const OnigPairCaseFoldCodes map[], int ess_tsett_flag, OnigCaseFoldType flag, const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[]));
extern int onigenc_not_support_get_ctype_code_range P_((OnigCtype ctype, OnigCodePoint* sb_out, const OnigCodePoint* ranges[]));
extern int onigenc_is_mbc_newline_0x0a P_((const UChar* p, const UChar* end));


/* methods for single byte encoding */
extern int onigenc_ascii_mbc_case_fold P_((OnigCaseFoldType flag, const UChar** p, const UChar* end, UChar* lower));
extern int onigenc_single_byte_mbc_enc_len P_((const UChar* p));
extern OnigCodePoint onigenc_single_byte_mbc_to_code P_((const UChar* p, const UChar* end));
extern int onigenc_single_byte_code_to_mbclen P_((OnigCodePoint code));
extern int onigenc_single_byte_code_to_mbc P_((OnigCodePoint code, UChar *buf));
extern UChar* onigenc_single_byte_left_adjust_char_head P_((const UChar* start, const UChar* s));
extern int onigenc_always_true_is_allowed_reverse_match P_((const UChar* s, const UChar* end));
extern int onigenc_always_false_is_allowed_reverse_match P_((const UChar* s, const UChar* end));
extern int onigenc_always_true_is_valid_mbc_string P_((const UChar* s, const UChar* end));
extern int onigenc_length_check_is_valid_mbc_string P_((OnigEncoding enc, const UChar* s, const UChar* end));

/* methods for multi byte encoding */
extern OnigCodePoint onigenc_mbn_mbc_to_code P_((OnigEncoding enc, const UChar* p, const UChar* end));
extern int onigenc_mbn_mbc_case_fold P_((OnigEncoding enc, OnigCaseFoldType flag, const UChar** p, const UChar* end, UChar* lower));
extern int onigenc_mb2_code_to_mbclen P_((OnigCodePoint code));
extern int onigenc_mb2_code_to_mbc P_((OnigEncoding enc, OnigCodePoint code, UChar *buf));
extern int onigenc_minimum_property_name_to_ctype P_((OnigEncoding enc, UChar* p, UChar* end));
extern int onigenc_unicode_property_name_to_ctype P_((OnigEncoding enc, UChar* p, UChar* end));
extern int onigenc_is_mbc_word_ascii P_((OnigEncoding enc, UChar* s, const UChar* end));
extern int onigenc_mb2_is_code_ctype P_((OnigEncoding enc, OnigCodePoint code, unsigned int ctype));
extern int onigenc_mb4_code_to_mbclen P_((OnigCodePoint code));
extern int onigenc_mb4_code_to_mbc P_((OnigEncoding enc, OnigCodePoint code, UChar *buf));
extern int onigenc_mb4_is_code_ctype P_((OnigEncoding enc, OnigCodePoint code, unsigned int ctype));
extern struct PropertyNameCtype* onigenc_euc_jp_lookup_property_name P_((register const char *str, register size_t len));
extern struct PropertyNameCtype* onigenc_sjis_lookup_property_name P_((register const char *str, register size_t len));

/* in enc/unicode.c */
extern int onigenc_unicode_is_code_ctype P_((OnigCodePoint code, unsigned int ctype));
extern int onigenc_utf16_32_get_ctype_code_range P_((OnigCtype ctype, OnigCodePoint *sb_out, const OnigCodePoint* ranges[]));
extern int onigenc_unicode_ctype_code_range P_((OnigCtype ctype, const OnigCodePoint* ranges[]));
extern int onigenc_unicode_get_case_fold_codes_by_str P_((OnigEncoding enc, OnigCaseFoldType flag, const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[]));
extern int onigenc_unicode_mbc_case_fold P_((OnigEncoding enc, OnigCaseFoldType flag, const UChar** pp, const UChar* end, UChar* fold));
extern int onigenc_unicode_apply_all_case_fold P_((OnigCaseFoldType flag, OnigApplyAllCaseFoldFunc f, void* arg));
extern int onigenc_egcb_is_break_position P_((OnigEncoding enc, UChar* p, UChar* prev, const UChar* start, const UChar* end));


#define UTF16_IS_SURROGATE_FIRST(c)    (((c) & 0xfc) == 0xd8)
#define UTF16_IS_SURROGATE_SECOND(c)   (((c) & 0xfc) == 0xdc)

/* from unicode generated codes */
#define FOLDS1_FOLD(i)         (OnigUnicodeFolds1 + (i))
#define FOLDS2_FOLD(i)         (OnigUnicodeFolds2 + (i))
#define FOLDS3_FOLD(i)         (OnigUnicodeFolds3 + (i))
#define FOLDS1_UNFOLDS_NUM(i)  (OnigUnicodeFolds1[(i)+1])
#define FOLDS2_UNFOLDS_NUM(i)  (OnigUnicodeFolds2[(i)+2])
#define FOLDS3_UNFOLDS_NUM(i)  (OnigUnicodeFolds3[(i)+3])
#define FOLDS1_UNFOLDS(i)      (OnigUnicodeFolds1 + (i) + 2)
#define FOLDS2_UNFOLDS(i)      (OnigUnicodeFolds2 + (i) + 3)
#define FOLDS3_UNFOLDS(i)      (OnigUnicodeFolds3 + (i) + 4)
#define FOLDS1_NEXT_INDEX(i)   ((i) + 2 + OnigUnicodeFolds1[(i)+1])
#define FOLDS2_NEXT_INDEX(i)   ((i) + 3 + OnigUnicodeFolds2[(i)+2])
#define FOLDS3_NEXT_INDEX(i)   ((i) + 4 + OnigUnicodeFolds3[(i)+3])

#define FOLDS_FOLD_ADDR_BUK(buk, addr) do {\
  if ((buk)->fold_len == 1)\
    addr = OnigUnicodeFolds1 + (buk)->index;\
  else if ((buk)->fold_len == 2)\
    addr = OnigUnicodeFolds2 + (buk)->index;\
  else if ((buk)->fold_len == 3)\
    addr = OnigUnicodeFolds3 + (buk)->index;\
  else\
    addr = 0;\
} while (0)

extern OnigCodePoint OnigUnicodeFolds1[];
extern OnigCodePoint OnigUnicodeFolds2[];
extern OnigCodePoint OnigUnicodeFolds3[];

struct ByUnfoldKey {
  OnigCodePoint code;
  short int     index;
  short int     fold_len;
};

extern const struct ByUnfoldKey* onigenc_unicode_unfold_key(OnigCodePoint code);
extern int onigenc_unicode_fold1_key(OnigCodePoint code[]);
extern int onigenc_unicode_fold2_key(OnigCodePoint code[]);
extern int onigenc_unicode_fold3_key(OnigCodePoint code[]);

extern int onig_codes_cmp(OnigCodePoint a[], OnigCodePoint b[], int n);
extern int onig_codes_byte_at(OnigCodePoint code[], int at);



#define ONIGENC_ISO_8859_1_TO_LOWER_CASE(c) \
  OnigEncISO_8859_1_ToLowerCaseTable[c]
#define ONIGENC_ISO_8859_1_TO_UPPER_CASE(c) \
  OnigEncISO_8859_1_ToUpperCaseTable[c]

extern const UChar OnigEncISO_8859_1_ToLowerCaseTable[];
extern const UChar OnigEncISO_8859_1_ToUpperCaseTable[];

extern int
onigenc_with_ascii_strncmp P_((OnigEncoding enc, const UChar* p, const UChar* end, const UChar* sascii /* ascii */, int n));
extern UChar*
onigenc_step P_((OnigEncoding enc, const UChar* p, const UChar* end, int n));

/* defined in regexec.c, but used in enc/xxx.c */
extern int  onig_is_in_code_range P_((const UChar* p, OnigCodePoint code));

extern OnigEncoding  OnigEncDefaultCharEncoding;
extern const UChar  OnigEncAsciiToLowerCaseTable[];
extern const UChar  OnigEncAsciiToUpperCaseTable[];
extern const unsigned short OnigEncAsciiCtypeTable[];


#define ONIGENC_IS_ASCII_CODE(code)  ((code) < 0x80)
#define ONIGENC_ASCII_CODE_TO_LOWER_CASE(c) OnigEncAsciiToLowerCaseTable[c]
#define ONIGENC_ASCII_CODE_TO_UPPER_CASE(c) OnigEncAsciiToUpperCaseTable[c]
#define ONIGENC_IS_ASCII_CODE_CTYPE(code,ctype) \
  ((OnigEncAsciiCtypeTable[code] & CTYPE_TO_BIT(ctype)) != 0)
#define ONIGENC_IS_ASCII_CODE_WORD(code) \
  ((OnigEncAsciiCtypeTable[code] & CTYPE_TO_BIT(ONIGENC_CTYPE_WORD)) != 0)
#define ONIGENC_IS_ASCII_CODE_CASE_AMBIG(code) \
 (ONIGENC_IS_ASCII_CODE_CTYPE(code, ONIGENC_CTYPE_UPPER) ||\
  ONIGENC_IS_ASCII_CODE_CTYPE(code, ONIGENC_CTYPE_LOWER))
   
#define ONIGENC_IS_UNICODE_ENCODING(enc) \
  (((enc)->flag & ENC_FLAG_UNICODE) != 0)

#define ONIGENC_IS_ASCII_COMPATIBLE_ENCODING(enc)  \
  (((enc)->flag & ENC_FLAG_ASCII_COMPATIBLE) != 0)

#endif /* REGENC_H */
