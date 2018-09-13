#ifndef ONIGURUMA_H
#define ONIGURUMA_H
/**********************************************************************
  oniguruma.h - Oniguruma (regular expression library)
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

#ifdef __cplusplus
extern "C" {
#endif

#define ONIGURUMA
#define ONIGURUMA_VERSION_MAJOR   6
#define ONIGURUMA_VERSION_MINOR   9
#define ONIGURUMA_VERSION_TEENY   0

#define ONIGURUMA_VERSION_INT     60900

#ifndef P_
#if defined(__STDC__) || defined(_WIN32)
# define P_(args) args
#else
# define P_(args) ()
#endif
#endif

#ifndef PV_
# define PV_(args) args
#endif

#ifndef ONIG_EXTERN
#if defined(_WIN32) && !defined(__GNUC__)
#if defined(ONIGURUMA_EXPORT)
#define ONIG_EXTERN   extern __declspec(dllexport)
#else
#define ONIG_EXTERN   extern __declspec(dllimport)
#endif
#endif
#endif

#ifndef ONIG_EXTERN
#define ONIG_EXTERN   extern
#endif

/* PART: character encoding */

#ifndef ONIG_ESCAPE_UCHAR_COLLISION
#define UChar OnigUChar
#endif

typedef unsigned int   OnigCodePoint;
typedef unsigned char  OnigUChar;
typedef unsigned int   OnigCtype;
typedef unsigned int   OnigLen;

#define ONIG_INFINITE_DISTANCE  ~((OnigLen )0)

typedef unsigned int OnigCaseFoldType; /* case fold flag */

ONIG_EXTERN OnigCaseFoldType OnigDefaultCaseFoldFlag;

/* #define ONIGENC_CASE_FOLD_HIRAGANA_KATAKANA  (1<<1) */
/* #define ONIGENC_CASE_FOLD_KATAKANA_WIDTH     (1<<2) */
#define ONIGENC_CASE_FOLD_TURKISH_AZERI         (1<<20)
#define INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR   (1<<30)

#define ONIGENC_CASE_FOLD_MIN      INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR
#define ONIGENC_CASE_FOLD_DEFAULT  OnigDefaultCaseFoldFlag


#define ONIGENC_MAX_COMP_CASE_FOLD_CODE_LEN       3
#define ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM      13
/* 13 => Unicode:0x1ffc */

/* code range */
#define ONIGENC_CODE_RANGE_NUM(range)     ((int )range[0])
#define ONIGENC_CODE_RANGE_FROM(range,i)  range[((i)*2) + 1]
#define ONIGENC_CODE_RANGE_TO(range,i)    range[((i)*2) + 2]

typedef struct {
  int byte_len;  /* argument(original) character(s) byte length */
  int code_len;  /* number of code */
  OnigCodePoint code[ONIGENC_MAX_COMP_CASE_FOLD_CODE_LEN];
} OnigCaseFoldCodeItem;

typedef struct {
  OnigCodePoint esc;
  OnigCodePoint anychar;
  OnigCodePoint anytime;
  OnigCodePoint zero_or_one_time;
  OnigCodePoint one_or_more_time;
  OnigCodePoint anychar_anytime;
} OnigMetaCharTableType;

typedef int (*OnigApplyAllCaseFoldFunc)(OnigCodePoint from, OnigCodePoint* to, int to_len, void* arg);

typedef struct OnigEncodingTypeST {
  int    (*mbc_enc_len)(const OnigUChar* p);
  const char*   name;
  int           max_enc_len;
  int           min_enc_len;
  int    (*is_mbc_newline)(const OnigUChar* p, const OnigUChar* end);
  OnigCodePoint (*mbc_to_code)(const OnigUChar* p, const OnigUChar* end);
  int    (*code_to_mbclen)(OnigCodePoint code);
  int    (*code_to_mbc)(OnigCodePoint code, OnigUChar *buf);
  int    (*mbc_case_fold)(OnigCaseFoldType flag, const OnigUChar** pp, const OnigUChar* end, OnigUChar* to);
  int    (*apply_all_case_fold)(OnigCaseFoldType flag, OnigApplyAllCaseFoldFunc f, void* arg);
  int    (*get_case_fold_codes_by_str)(OnigCaseFoldType flag, const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem acs[]);
  int    (*property_name_to_ctype)(struct OnigEncodingTypeST* enc, OnigUChar* p, OnigUChar* end);
  int    (*is_code_ctype)(OnigCodePoint code, OnigCtype ctype);
  int    (*get_ctype_code_range)(OnigCtype ctype, OnigCodePoint* sb_out, const OnigCodePoint* ranges[]);
  OnigUChar* (*left_adjust_char_head)(const OnigUChar* start, const OnigUChar* p);
  int    (*is_allowed_reverse_match)(const OnigUChar* p, const OnigUChar* end);
  int    (*init)(void);
  int    (*is_initialized)(void);
  int    (*is_valid_mbc_string)(const OnigUChar* s, const OnigUChar* end);
  unsigned int flag;
  OnigCodePoint sb_range;
  int index;
} OnigEncodingType;

typedef OnigEncodingType* OnigEncoding;

ONIG_EXTERN OnigEncodingType OnigEncodingASCII;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_1;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_2;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_3;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_4;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_5;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_6;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_7;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_8;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_9;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_10;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_11;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_13;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_14;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_15;
ONIG_EXTERN OnigEncodingType OnigEncodingISO_8859_16;
ONIG_EXTERN OnigEncodingType OnigEncodingUTF8;
ONIG_EXTERN OnigEncodingType OnigEncodingUTF16_BE;
ONIG_EXTERN OnigEncodingType OnigEncodingUTF16_LE;
ONIG_EXTERN OnigEncodingType OnigEncodingUTF32_BE;
ONIG_EXTERN OnigEncodingType OnigEncodingUTF32_LE;
ONIG_EXTERN OnigEncodingType OnigEncodingEUC_JP;
ONIG_EXTERN OnigEncodingType OnigEncodingEUC_TW;
ONIG_EXTERN OnigEncodingType OnigEncodingEUC_KR;
ONIG_EXTERN OnigEncodingType OnigEncodingEUC_CN;
ONIG_EXTERN OnigEncodingType OnigEncodingSJIS;
ONIG_EXTERN OnigEncodingType OnigEncodingKOI8;
ONIG_EXTERN OnigEncodingType OnigEncodingKOI8_R;
ONIG_EXTERN OnigEncodingType OnigEncodingCP1251;
ONIG_EXTERN OnigEncodingType OnigEncodingBIG5;
ONIG_EXTERN OnigEncodingType OnigEncodingGB18030;

#define ONIG_ENCODING_ASCII        (&OnigEncodingASCII)
#define ONIG_ENCODING_ISO_8859_1   (&OnigEncodingISO_8859_1)
#define ONIG_ENCODING_ISO_8859_2   (&OnigEncodingISO_8859_2)
#define ONIG_ENCODING_ISO_8859_3   (&OnigEncodingISO_8859_3)
#define ONIG_ENCODING_ISO_8859_4   (&OnigEncodingISO_8859_4)
#define ONIG_ENCODING_ISO_8859_5   (&OnigEncodingISO_8859_5)
#define ONIG_ENCODING_ISO_8859_6   (&OnigEncodingISO_8859_6)
#define ONIG_ENCODING_ISO_8859_7   (&OnigEncodingISO_8859_7)
#define ONIG_ENCODING_ISO_8859_8   (&OnigEncodingISO_8859_8)
#define ONIG_ENCODING_ISO_8859_9   (&OnigEncodingISO_8859_9)
#define ONIG_ENCODING_ISO_8859_10  (&OnigEncodingISO_8859_10)
#define ONIG_ENCODING_ISO_8859_11  (&OnigEncodingISO_8859_11)
#define ONIG_ENCODING_ISO_8859_13  (&OnigEncodingISO_8859_13)
#define ONIG_ENCODING_ISO_8859_14  (&OnigEncodingISO_8859_14)
#define ONIG_ENCODING_ISO_8859_15  (&OnigEncodingISO_8859_15)
#define ONIG_ENCODING_ISO_8859_16  (&OnigEncodingISO_8859_16)
#define ONIG_ENCODING_UTF8         (&OnigEncodingUTF8)
#define ONIG_ENCODING_UTF16_BE     (&OnigEncodingUTF16_BE)
#define ONIG_ENCODING_UTF16_LE     (&OnigEncodingUTF16_LE)
#define ONIG_ENCODING_UTF32_BE     (&OnigEncodingUTF32_BE)
#define ONIG_ENCODING_UTF32_LE     (&OnigEncodingUTF32_LE)
#define ONIG_ENCODING_EUC_JP       (&OnigEncodingEUC_JP)
#define ONIG_ENCODING_EUC_TW       (&OnigEncodingEUC_TW)
#define ONIG_ENCODING_EUC_KR       (&OnigEncodingEUC_KR)
#define ONIG_ENCODING_EUC_CN       (&OnigEncodingEUC_CN)
#define ONIG_ENCODING_SJIS         (&OnigEncodingSJIS)
#define ONIG_ENCODING_KOI8         (&OnigEncodingKOI8)
#define ONIG_ENCODING_KOI8_R       (&OnigEncodingKOI8_R)
#define ONIG_ENCODING_CP1251       (&OnigEncodingCP1251)
#define ONIG_ENCODING_BIG5         (&OnigEncodingBIG5)
#define ONIG_ENCODING_GB18030      (&OnigEncodingGB18030)

#define ONIG_ENCODING_UNDEF    ((OnigEncoding )0)


/* work size */
#define ONIGENC_CODE_TO_MBC_MAXLEN       7
#define ONIGENC_MBC_CASE_FOLD_MAXLEN    18
/* 18: 6(max-byte) * 3(case-fold chars) */

/* character types */
typedef enum {
  ONIGENC_CTYPE_NEWLINE = 0,
  ONIGENC_CTYPE_ALPHA   = 1,
  ONIGENC_CTYPE_BLANK   = 2,
  ONIGENC_CTYPE_CNTRL   = 3,
  ONIGENC_CTYPE_DIGIT   = 4,
  ONIGENC_CTYPE_GRAPH   = 5,
  ONIGENC_CTYPE_LOWER   = 6,
  ONIGENC_CTYPE_PRINT   = 7,
  ONIGENC_CTYPE_PUNCT   = 8,
  ONIGENC_CTYPE_SPACE   = 9,
  ONIGENC_CTYPE_UPPER   = 10,
  ONIGENC_CTYPE_XDIGIT  = 11,
  ONIGENC_CTYPE_WORD    = 12,
  ONIGENC_CTYPE_ALNUM   = 13,  /* alpha || digit */
  ONIGENC_CTYPE_ASCII   = 14
} OnigEncCtype;

#define ONIGENC_MAX_STD_CTYPE  ONIGENC_CTYPE_ASCII


#define onig_enc_len(enc,p,end)        ONIGENC_MBC_ENC_LEN(enc,p)

#define ONIGENC_IS_UNDEF(enc)          ((enc) == ONIG_ENCODING_UNDEF)
#define ONIGENC_IS_SINGLEBYTE(enc)     (ONIGENC_MBC_MAXLEN(enc) == 1)
#define ONIGENC_IS_MBC_HEAD(enc,p)     (ONIGENC_MBC_ENC_LEN(enc,p) != 1)
#define ONIGENC_IS_MBC_ASCII(p)           (*(p)   < 128)
#define ONIGENC_IS_CODE_ASCII(code)       ((code) < 128)
#define ONIGENC_IS_MBC_WORD(enc,s,end) \
   ONIGENC_IS_CODE_WORD(enc,ONIGENC_MBC_TO_CODE(enc,s,end))
#define ONIGENC_IS_MBC_WORD_ASCII(enc,s,end) onigenc_is_mbc_word_ascii(enc,s,end)

#define ONIGENC_NAME(enc)                      ((enc)->name)

#define ONIGENC_MBC_CASE_FOLD(enc,flag,pp,end,buf) \
  (enc)->mbc_case_fold(flag,(const OnigUChar** )pp,end,buf)
#define ONIGENC_IS_ALLOWED_REVERSE_MATCH(enc,s,end) \
        (enc)->is_allowed_reverse_match(s,end)
#define ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc,start,s) \
        (enc)->left_adjust_char_head(start, s)
#define ONIGENC_IS_VALID_MBC_STRING(enc,s,end) \
        (enc)->is_valid_mbc_string(s,end)
#define ONIGENC_APPLY_ALL_CASE_FOLD(enc,case_fold_flag,f,arg) \
        (enc)->apply_all_case_fold(case_fold_flag,f,arg)
#define ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc,case_fold_flag,p,end,acs) \
       (enc)->get_case_fold_codes_by_str(case_fold_flag,p,end,acs)
#define ONIGENC_STEP_BACK(enc,start,s,n) \
        onigenc_step_back((enc),(start),(s),(n))

#define ONIGENC_MBC_ENC_LEN(enc,p)             (enc)->mbc_enc_len(p)
#define ONIGENC_MBC_MAXLEN(enc)               ((enc)->max_enc_len)
#define ONIGENC_MBC_MAXLEN_DIST(enc)           ONIGENC_MBC_MAXLEN(enc)
#define ONIGENC_MBC_MINLEN(enc)               ((enc)->min_enc_len)
#define ONIGENC_IS_MBC_NEWLINE(enc,p,end)      (enc)->is_mbc_newline((p),(end))
#define ONIGENC_MBC_TO_CODE(enc,p,end)         (enc)->mbc_to_code((p),(end))
#define ONIGENC_CODE_TO_MBCLEN(enc,code)       (enc)->code_to_mbclen(code)
#define ONIGENC_CODE_TO_MBC(enc,code,buf)      (enc)->code_to_mbc(code,buf)
#define ONIGENC_PROPERTY_NAME_TO_CTYPE(enc,p,end) \
  (enc)->property_name_to_ctype(enc,p,end)

#define ONIGENC_IS_CODE_CTYPE(enc,code,ctype)  (enc)->is_code_ctype(code,ctype)

#define ONIGENC_IS_CODE_NEWLINE(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_NEWLINE)
#define ONIGENC_IS_CODE_GRAPH(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_GRAPH)
#define ONIGENC_IS_CODE_PRINT(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_PRINT)
#define ONIGENC_IS_CODE_ALNUM(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_ALNUM)
#define ONIGENC_IS_CODE_ALPHA(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_ALPHA)
#define ONIGENC_IS_CODE_LOWER(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_LOWER)
#define ONIGENC_IS_CODE_UPPER(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_UPPER)
#define ONIGENC_IS_CODE_CNTRL(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_CNTRL)
#define ONIGENC_IS_CODE_PUNCT(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_PUNCT)
#define ONIGENC_IS_CODE_SPACE(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_SPACE)
#define ONIGENC_IS_CODE_BLANK(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_BLANK)
#define ONIGENC_IS_CODE_DIGIT(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_DIGIT)
#define ONIGENC_IS_CODE_XDIGIT(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_XDIGIT)
#define ONIGENC_IS_CODE_WORD(enc,code) \
        ONIGENC_IS_CODE_CTYPE(enc,code,ONIGENC_CTYPE_WORD)

#define ONIGENC_GET_CTYPE_CODE_RANGE(enc,ctype,sbout,ranges) \
        (enc)->get_ctype_code_range(ctype,sbout,ranges)

ONIG_EXTERN
OnigUChar* onigenc_step_back P_((OnigEncoding enc, const OnigUChar* start, const OnigUChar* s, int n));


/* encoding API */
ONIG_EXTERN
int onigenc_init P_((void));
ONIG_EXTERN
int onig_initialize_encoding P_((OnigEncoding enc));
ONIG_EXTERN
int onigenc_set_default_encoding P_((OnigEncoding enc));
ONIG_EXTERN
OnigEncoding onigenc_get_default_encoding P_((void));
ONIG_EXTERN
void  onigenc_set_default_caseconv_table P_((const OnigUChar* table));
ONIG_EXTERN
OnigUChar* onigenc_get_right_adjust_char_head_with_prev P_((OnigEncoding enc, const OnigUChar* start, const OnigUChar* s, const OnigUChar** prev));
ONIG_EXTERN
OnigUChar* onigenc_get_prev_char_head P_((OnigEncoding enc, const OnigUChar* start, const OnigUChar* s));
ONIG_EXTERN
OnigUChar* onigenc_get_left_adjust_char_head P_((OnigEncoding enc, const OnigUChar* start, const OnigUChar* s));
ONIG_EXTERN
OnigUChar* onigenc_get_right_adjust_char_head P_((OnigEncoding enc, const OnigUChar* start, const OnigUChar* s));
ONIG_EXTERN
int onigenc_strlen P_((OnigEncoding enc, const OnigUChar* p, const OnigUChar* end));
ONIG_EXTERN
int onigenc_strlen_null P_((OnigEncoding enc, const OnigUChar* p));
ONIG_EXTERN
int onigenc_str_bytelen_null P_((OnigEncoding enc, const OnigUChar* p));
ONIG_EXTERN
int onigenc_is_valid_mbc_string P_((OnigEncoding enc, const OnigUChar* s, const OnigUChar* end));
ONIG_EXTERN
OnigUChar* onigenc_strdup P_((OnigEncoding enc, const OnigUChar* s, const OnigUChar* end));


/* PART: regular expression */

/* config parameters */
#define ONIG_NREGION                          10
#define ONIG_MAX_CAPTURE_NUM          2147483647  /* 2**31 - 1 */
#define ONIG_MAX_BACKREF_NUM                1000
#define ONIG_MAX_REPEAT_NUM               100000
#define ONIG_MAX_MULTI_BYTE_RANGES_NUM     10000
/* constants */
#define ONIG_MAX_ERROR_MESSAGE_LEN            90

typedef unsigned int        OnigOptionType;

#define ONIG_OPTION_DEFAULT            ONIG_OPTION_NONE

/* options */
#define ONIG_OPTION_NONE                 0U
/* options (compile time) */
#define ONIG_OPTION_IGNORECASE           1U
#define ONIG_OPTION_EXTEND               (ONIG_OPTION_IGNORECASE         << 1)
#define ONIG_OPTION_MULTILINE            (ONIG_OPTION_EXTEND             << 1)
#define ONIG_OPTION_SINGLELINE           (ONIG_OPTION_MULTILINE          << 1)
#define ONIG_OPTION_FIND_LONGEST         (ONIG_OPTION_SINGLELINE         << 1)
#define ONIG_OPTION_FIND_NOT_EMPTY       (ONIG_OPTION_FIND_LONGEST       << 1)
#define ONIG_OPTION_NEGATE_SINGLELINE    (ONIG_OPTION_FIND_NOT_EMPTY     << 1)
#define ONIG_OPTION_DONT_CAPTURE_GROUP   (ONIG_OPTION_NEGATE_SINGLELINE  << 1)
#define ONIG_OPTION_CAPTURE_GROUP        (ONIG_OPTION_DONT_CAPTURE_GROUP << 1)
/* options (search time) */
#define ONIG_OPTION_NOTBOL                    (ONIG_OPTION_CAPTURE_GROUP << 1)
#define ONIG_OPTION_NOTEOL                    (ONIG_OPTION_NOTBOL << 1)
#define ONIG_OPTION_POSIX_REGION              (ONIG_OPTION_NOTEOL << 1)
#define ONIG_OPTION_CHECK_VALIDITY_OF_STRING  (ONIG_OPTION_POSIX_REGION << 1)
/* #define ONIG_OPTION_CRLF_AS_LINE_SEPARATOR    (ONIG_OPTION_CHECK_VALIDITY_OF_STRING << 1) */
/* options (compile time) */
#define ONIG_OPTION_WORD_IS_ASCII        (ONIG_OPTION_CHECK_VALIDITY_OF_STRING << 4)
#define ONIG_OPTION_DIGIT_IS_ASCII       (ONIG_OPTION_WORD_IS_ASCII << 1)
#define ONIG_OPTION_SPACE_IS_ASCII       (ONIG_OPTION_DIGIT_IS_ASCII << 1)
#define ONIG_OPTION_POSIX_IS_ASCII       (ONIG_OPTION_SPACE_IS_ASCII << 1)

#define ONIG_OPTION_MAXBIT               ONIG_OPTION_POSIX_IS_ASCII  /* limit */

#define ONIG_OPTION_ON(options,regopt)      ((options) |= (regopt))
#define ONIG_OPTION_OFF(options,regopt)     ((options) &= ~(regopt))
#define ONIG_IS_OPTION_ON(options,option)   ((options) & (option))

/* syntax */
typedef struct {
  unsigned int   op;
  unsigned int   op2;
  unsigned int   behavior;
  OnigOptionType options;   /* default option */
  OnigMetaCharTableType meta_char_table;
} OnigSyntaxType;

ONIG_EXTERN OnigSyntaxType OnigSyntaxASIS;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPosixBasic;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPosixExtended;
ONIG_EXTERN OnigSyntaxType OnigSyntaxEmacs;
ONIG_EXTERN OnigSyntaxType OnigSyntaxGrep;
ONIG_EXTERN OnigSyntaxType OnigSyntaxGnuRegex;
ONIG_EXTERN OnigSyntaxType OnigSyntaxJava;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPerl;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPerl_NG;
ONIG_EXTERN OnigSyntaxType OnigSyntaxRuby;
ONIG_EXTERN OnigSyntaxType OnigSyntaxOniguruma;

/* predefined syntaxes (see regsyntax.c) */
#define ONIG_SYNTAX_ASIS               (&OnigSyntaxASIS)
#define ONIG_SYNTAX_POSIX_BASIC        (&OnigSyntaxPosixBasic)
#define ONIG_SYNTAX_POSIX_EXTENDED     (&OnigSyntaxPosixExtended)
#define ONIG_SYNTAX_EMACS              (&OnigSyntaxEmacs)
#define ONIG_SYNTAX_GREP               (&OnigSyntaxGrep)
#define ONIG_SYNTAX_GNU_REGEX          (&OnigSyntaxGnuRegex)
#define ONIG_SYNTAX_JAVA               (&OnigSyntaxJava)
#define ONIG_SYNTAX_PERL               (&OnigSyntaxPerl)
#define ONIG_SYNTAX_PERL_NG            (&OnigSyntaxPerl_NG)
#define ONIG_SYNTAX_RUBY               (&OnigSyntaxRuby)
#define ONIG_SYNTAX_ONIGURUMA          (&OnigSyntaxOniguruma)

/* default syntax */
ONIG_EXTERN OnigSyntaxType*   OnigDefaultSyntax;
#define ONIG_SYNTAX_DEFAULT   OnigDefaultSyntax

/* syntax (operators) */
#define ONIG_SYN_OP_VARIABLE_META_CHARACTERS    (1U<<0)
#define ONIG_SYN_OP_DOT_ANYCHAR                 (1U<<1)   /* . */
#define ONIG_SYN_OP_ASTERISK_ZERO_INF           (1U<<2)   /* * */
#define ONIG_SYN_OP_ESC_ASTERISK_ZERO_INF       (1U<<3)
#define ONIG_SYN_OP_PLUS_ONE_INF                (1U<<4)   /* + */
#define ONIG_SYN_OP_ESC_PLUS_ONE_INF            (1U<<5)
#define ONIG_SYN_OP_QMARK_ZERO_ONE              (1U<<6)   /* ? */
#define ONIG_SYN_OP_ESC_QMARK_ZERO_ONE          (1U<<7)
#define ONIG_SYN_OP_BRACE_INTERVAL              (1U<<8)   /* {lower,upper} */
#define ONIG_SYN_OP_ESC_BRACE_INTERVAL          (1U<<9)   /* \{lower,upper\} */
#define ONIG_SYN_OP_VBAR_ALT                    (1U<<10)   /* | */
#define ONIG_SYN_OP_ESC_VBAR_ALT                (1U<<11)  /* \| */
#define ONIG_SYN_OP_LPAREN_SUBEXP               (1U<<12)  /* (...)   */
#define ONIG_SYN_OP_ESC_LPAREN_SUBEXP           (1U<<13)  /* \(...\) */
#define ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR           (1U<<14)  /* \A, \Z, \z */
#define ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR  (1U<<15)  /* \G     */
#define ONIG_SYN_OP_DECIMAL_BACKREF             (1U<<16)  /* \num   */
#define ONIG_SYN_OP_BRACKET_CC                  (1U<<17)  /* [...]  */
#define ONIG_SYN_OP_ESC_W_WORD                  (1U<<18)  /* \w, \W */
#define ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END     (1U<<19)  /* \<. \> */
#define ONIG_SYN_OP_ESC_B_WORD_BOUND            (1U<<20)  /* \b, \B */
#define ONIG_SYN_OP_ESC_S_WHITE_SPACE           (1U<<21)  /* \s, \S */
#define ONIG_SYN_OP_ESC_D_DIGIT                 (1U<<22)  /* \d, \D */
#define ONIG_SYN_OP_LINE_ANCHOR                 (1U<<23)  /* ^, $   */
#define ONIG_SYN_OP_POSIX_BRACKET               (1U<<24)  /* [:xxxx:] */
#define ONIG_SYN_OP_QMARK_NON_GREEDY            (1U<<25)  /* ??,*?,+?,{n,m}? */
#define ONIG_SYN_OP_ESC_CONTROL_CHARS           (1U<<26)  /* \n,\r,\t,\a ... */
#define ONIG_SYN_OP_ESC_C_CONTROL               (1U<<27)  /* \cx  */
#define ONIG_SYN_OP_ESC_OCTAL3                  (1U<<28)  /* \OOO */
#define ONIG_SYN_OP_ESC_X_HEX2                  (1U<<29)  /* \xHH */
#define ONIG_SYN_OP_ESC_X_BRACE_HEX8            (1U<<30)  /* \x{7HHHHHHH} */
#define ONIG_SYN_OP_ESC_O_BRACE_OCTAL           (1U<<31)  /* \o{1OOOOOOOOOO} */

#define ONIG_SYN_OP2_ESC_CAPITAL_Q_QUOTE        (1U<<0)  /* \Q...\E */
#define ONIG_SYN_OP2_QMARK_GROUP_EFFECT         (1U<<1)  /* (?...) */
#define ONIG_SYN_OP2_OPTION_PERL                (1U<<2)  /* (?imsx),(?-imsx) */
#define ONIG_SYN_OP2_OPTION_RUBY                (1U<<3)  /* (?imx), (?-imx)  */
#define ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT     (1U<<4)  /* ?+,*+,++ */
#define ONIG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL   (1U<<5)  /* {n,m}+   */
#define ONIG_SYN_OP2_CCLASS_SET_OP              (1U<<6)  /* [...&&..[..]..] */
#define ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP       (1U<<7)  /* (?<name>...) */
#define ONIG_SYN_OP2_ESC_K_NAMED_BACKREF        (1U<<8)  /* \k<name> */
#define ONIG_SYN_OP2_ESC_G_SUBEXP_CALL          (1U<<9)  /* \g<name>, \g<n> */
#define ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY     (1U<<10) /* (?@..),(?@<x>..) */
#define ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL  (1U<<11) /* \C-x */
#define ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META     (1U<<12) /* \M-x */
#define ONIG_SYN_OP2_ESC_V_VTAB                 (1U<<13) /* \v as VTAB */
#define ONIG_SYN_OP2_ESC_U_HEX4                 (1U<<14) /* \uHHHH */
#define ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR         (1U<<15) /* \`, \' */
#define ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY  (1U<<16) /* \p{...}, \P{...} */
#define ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT (1U<<17) /* \p{^..}, \P{^..} */
/* #define ONIG_SYN_OP2_CHAR_PROPERTY_PREFIX_IS (1U<<18) */
#define ONIG_SYN_OP2_ESC_H_XDIGIT               (1U<<19) /* \h, \H */
#define ONIG_SYN_OP2_INEFFECTIVE_ESCAPE         (1U<<20) /* \ */
#define ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE       (1U<<21) /* (?(n)) (?(...)...|...) */
#define ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP         (1U<<22) /* \K */
#define ONIG_SYN_OP2_ESC_CAPITAL_R_GENERAL_NEWLINE (1U<<23) /* \R \r\n else [\x0a-\x0d] */
#define ONIG_SYN_OP2_ESC_CAPITAL_N_O_SUPER_DOT  (1U<<24) /* \N (?-m:.), \O (?m:.) */
#define ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP   (1U<<25) /* (?~...) */
#define ONIG_SYN_OP2_ESC_X_Y_GRAPHEME_CLUSTER   (1U<<26) /* \X \y \Y */
#define ONIG_SYN_OP2_QMARK_PERL_SUBEXP_CALL     (1U<<27) /* (?R), (?&name)... */
#define ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS (1U<<28) /* (?{...}) (?{{...}}) */
#define ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME      (1U<<29) /* (*name) (*name{a,..}) */

/* syntax (behavior) */
#define ONIG_SYN_CONTEXT_INDEP_ANCHORS           (1U<<31) /* not implemented */
#define ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS        (1U<<0)  /* ?, *, +, {n,m} */
#define ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS      (1U<<1)  /* error or ignore */
#define ONIG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP    (1U<<2)  /* ...)... */
#define ONIG_SYN_ALLOW_INVALID_INTERVAL          (1U<<3)  /* {??? */
#define ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV       (1U<<4)  /* {,n} => {0,n} */
#define ONIG_SYN_STRICT_CHECK_BACKREF            (1U<<5)  /* /(\1)/,/\1()/ ..*/
#define ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND   (1U<<6)  /* (?<=a|bc) */
#define ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP        (1U<<7)  /* see doc/RE */
#define ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME (1U<<8)  /* (?<x>)(?<x>) */
#define ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY   (1U<<9)  /* a{n}?=(?:a{n})? */

/* syntax (behavior) in char class [...] */
#define ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC      (1U<<20) /* [^...] */
#define ONIG_SYN_BACKSLASH_ESCAPE_IN_CC          (1U<<21) /* [..\w..] etc.. */
#define ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC         (1U<<22)
#define ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC     (1U<<23) /* [0-9-a]=[0-9\-a] */
/* syntax (behavior) warning */
#define ONIG_SYN_WARN_CC_OP_NOT_ESCAPED          (1U<<24) /* [,-,] */
#define ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT    (1U<<25) /* (?:a*)+ */

/* meta character specifiers (onig_set_meta_char()) */
#define ONIG_META_CHAR_ESCAPE               0
#define ONIG_META_CHAR_ANYCHAR              1
#define ONIG_META_CHAR_ANYTIME              2
#define ONIG_META_CHAR_ZERO_OR_ONE_TIME     3
#define ONIG_META_CHAR_ONE_OR_MORE_TIME     4
#define ONIG_META_CHAR_ANYCHAR_ANYTIME      5

#define ONIG_INEFFECTIVE_META_CHAR          0

/* error codes */
#define ONIG_IS_PATTERN_ERROR(ecode)   ((ecode) <= -100 && (ecode) > -1000)
/* normal return */
#define ONIG_NORMAL                                            0
#define ONIG_MISMATCH                                         -1
#define ONIG_NO_SUPPORT_CONFIG                                -2
#define ONIG_ABORT                                            -3

/* internal error */
#define ONIGERR_MEMORY                                         -5
#define ONIGERR_TYPE_BUG                                       -6
#define ONIGERR_PARSER_BUG                                    -11
#define ONIGERR_STACK_BUG                                     -12
#define ONIGERR_UNDEFINED_BYTECODE                            -13
#define ONIGERR_UNEXPECTED_BYTECODE                           -14
#define ONIGERR_MATCH_STACK_LIMIT_OVER                        -15
#define ONIGERR_PARSE_DEPTH_LIMIT_OVER                        -16
#define ONIGERR_RETRY_LIMIT_IN_MATCH_OVER                     -17
#define ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED                -21
#define ONIGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR  -22
#define ONIGERR_FAIL_TO_INITIALIZE                            -23
/* general error */
#define ONIGERR_INVALID_ARGUMENT                              -30
/* syntax error */
#define ONIGERR_END_PATTERN_AT_LEFT_BRACE                    -100
#define ONIGERR_END_PATTERN_AT_LEFT_BRACKET                  -101
#define ONIGERR_EMPTY_CHAR_CLASS                             -102
#define ONIGERR_PREMATURE_END_OF_CHAR_CLASS                  -103
#define ONIGERR_END_PATTERN_AT_ESCAPE                        -104
#define ONIGERR_END_PATTERN_AT_META                          -105
#define ONIGERR_END_PATTERN_AT_CONTROL                       -106
#define ONIGERR_META_CODE_SYNTAX                             -108
#define ONIGERR_CONTROL_CODE_SYNTAX                          -109
#define ONIGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE             -110
#define ONIGERR_CHAR_CLASS_VALUE_AT_START_OF_RANGE           -111
#define ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS      -112
#define ONIGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED      -113
#define ONIGERR_TARGET_OF_REPEAT_OPERATOR_INVALID            -114
#define ONIGERR_NESTED_REPEAT_OPERATOR                       -115
#define ONIGERR_UNMATCHED_CLOSE_PARENTHESIS                  -116
#define ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS       -117
#define ONIGERR_END_PATTERN_IN_GROUP                         -118
#define ONIGERR_UNDEFINED_GROUP_OPTION                       -119
#define ONIGERR_INVALID_POSIX_BRACKET_TYPE                   -121
#define ONIGERR_INVALID_LOOK_BEHIND_PATTERN                  -122
#define ONIGERR_INVALID_REPEAT_RANGE_PATTERN                 -123
/* values error (syntax error) */
#define ONIGERR_TOO_BIG_NUMBER                               -200
#define ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE              -201
#define ONIGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE     -202
#define ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS                    -203
#define ONIGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE          -204
#define ONIGERR_TOO_MANY_MULTI_BYTE_RANGES                   -205
#define ONIGERR_TOO_SHORT_MULTI_BYTE_STRING                  -206
#define ONIGERR_TOO_BIG_BACKREF_NUMBER                       -207
#define ONIGERR_INVALID_BACKREF                              -208
#define ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED         -209
#define ONIGERR_TOO_MANY_CAPTURES                            -210
#define ONIGERR_TOO_LONG_WIDE_CHAR_VALUE                     -212
#define ONIGERR_EMPTY_GROUP_NAME                             -214
#define ONIGERR_INVALID_GROUP_NAME                           -215
#define ONIGERR_INVALID_CHAR_IN_GROUP_NAME                   -216
#define ONIGERR_UNDEFINED_NAME_REFERENCE                     -217
#define ONIGERR_UNDEFINED_GROUP_REFERENCE                    -218
#define ONIGERR_MULTIPLEX_DEFINED_NAME                       -219
#define ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL               -220
#define ONIGERR_NEVER_ENDING_RECURSION                       -221
#define ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY        -222
#define ONIGERR_INVALID_CHAR_PROPERTY_NAME                   -223
#define ONIGERR_INVALID_IF_ELSE_SYNTAX                       -224
#define ONIGERR_INVALID_ABSENT_GROUP_PATTERN                 -225
#define ONIGERR_INVALID_ABSENT_GROUP_GENERATOR_PATTERN       -226
#define ONIGERR_INVALID_CALLOUT_PATTERN                      -227
#define ONIGERR_INVALID_CALLOUT_NAME                         -228
#define ONIGERR_UNDEFINED_CALLOUT_NAME                       -229
#define ONIGERR_INVALID_CALLOUT_BODY                         -230
#define ONIGERR_INVALID_CALLOUT_TAG_NAME                     -231
#define ONIGERR_INVALID_CALLOUT_ARG                          -232
#define ONIGERR_INVALID_CODE_POINT_VALUE                     -400
#define ONIGERR_INVALID_WIDE_CHAR_VALUE                      -400
#define ONIGERR_TOO_BIG_WIDE_CHAR_VALUE                      -401
#define ONIGERR_NOT_SUPPORTED_ENCODING_COMBINATION           -402
#define ONIGERR_INVALID_COMBINATION_OF_OPTIONS               -403
#define ONIGERR_TOO_MANY_USER_DEFINED_OBJECTS                -404
#define ONIGERR_TOO_LONG_PROPERTY_NAME                       -405
#define ONIGERR_LIBRARY_IS_NOT_INITIALIZED                   -500

/* errors related to thread */
/* #define ONIGERR_OVER_THREAD_PASS_LIMIT_COUNT                -1001 */


/* must be smaller than MEM_STATUS_BITS_NUM (unsigned int * 8) */
#define ONIG_MAX_CAPTURE_HISTORY_GROUP   31
#define ONIG_IS_CAPTURE_HISTORY_GROUP(r, i) \
  ((i) <= ONIG_MAX_CAPTURE_HISTORY_GROUP && (r)->list && (r)->list[i])

typedef struct OnigCaptureTreeNodeStruct {
  int group;   /* group number */
  int beg;
  int end;
  int allocated;
  int num_childs;
  struct OnigCaptureTreeNodeStruct** childs;
} OnigCaptureTreeNode;

/* match result region type */
struct re_registers {
  int  allocated;
  int  num_regs;
  int* beg;
  int* end;
  /* extended */
  OnigCaptureTreeNode* history_root;  /* capture history tree root */
};

/* capture tree traverse */
#define ONIG_TRAVERSE_CALLBACK_AT_FIRST   1
#define ONIG_TRAVERSE_CALLBACK_AT_LAST    2
#define ONIG_TRAVERSE_CALLBACK_AT_BOTH \
  ( ONIG_TRAVERSE_CALLBACK_AT_FIRST | ONIG_TRAVERSE_CALLBACK_AT_LAST )


#define ONIG_REGION_NOTPOS            -1

typedef struct re_registers   OnigRegion;

typedef struct {
  OnigEncoding enc;
  OnigUChar* par;
  OnigUChar* par_end;
} OnigErrorInfo;

typedef struct {
  int lower;
  int upper;
} OnigRepeatRange;

typedef void (*OnigWarnFunc) P_((const char* s));
extern void onig_null_warn P_((const char* s));
#define ONIG_NULL_WARN       onig_null_warn

#define ONIG_CHAR_TABLE_SIZE   256

struct re_pattern_buffer;
typedef struct re_pattern_buffer OnigRegexType;
typedef OnigRegexType*  OnigRegex;

#ifndef ONIG_ESCAPE_REGEX_T_COLLISION
  typedef OnigRegexType  regex_t;
#endif


typedef struct {
  int             num_of_elements;
  OnigEncoding    pattern_enc;
  OnigEncoding    target_enc;
  OnigSyntaxType* syntax;
  OnigOptionType  option;
  OnigCaseFoldType   case_fold_flag;
} OnigCompileInfo;


/* types for callout */
typedef enum {
  ONIG_CALLOUT_IN_PROGRESS   = 1, /* 1<<0 */
  ONIG_CALLOUT_IN_RETRACTION = 2  /* 1<<1 */
} OnigCalloutIn;

#define ONIG_CALLOUT_IN_BOTH  (ONIG_CALLOUT_IN_PROGRESS | ONIG_CALLOUT_IN_RETRACTION)

typedef enum {
  ONIG_CALLOUT_OF_CONTENTS = 0,
  ONIG_CALLOUT_OF_NAME     = 1
} OnigCalloutOf;

typedef enum {
  ONIG_CALLOUT_TYPE_SINGLE              = 0,
  ONIG_CALLOUT_TYPE_START_CALL          = 1,
  ONIG_CALLOUT_TYPE_BOTH_CALL           = 2,
  ONIG_CALLOUT_TYPE_START_MARK_END_CALL = 3,
} OnigCalloutType;


#define ONIG_NON_NAME_ID        -1
#define ONIG_NON_CALLOUT_NUM     0

#define ONIG_CALLOUT_MAX_ARGS_NUM     4
#define ONIG_CALLOUT_DATA_SLOT_NUM    5

struct OnigCalloutArgsStruct;
typedef struct OnigCalloutArgsStruct OnigCalloutArgs;

typedef int (*OnigCalloutFunc)(OnigCalloutArgs* args, void* user_data);

/* callout function return values (less than -1: error code) */
typedef enum {
  ONIG_CALLOUT_FAIL     =  1,
  ONIG_CALLOUT_SUCCESS  =  0
} OnigCalloutResult;

typedef enum {
  ONIG_TYPE_VOID     = 0,
  ONIG_TYPE_LONG     = 1<<0,
  ONIG_TYPE_CHAR     = 1<<1,
  ONIG_TYPE_STRING   = 1<<2,
  ONIG_TYPE_POINTER  = 1<<3,
  ONIG_TYPE_TAG      = 1<<4,
} OnigType;

typedef union {
  long l;
  OnigCodePoint c;
  struct {
    OnigUChar* start;
    OnigUChar* end;
  } s;
  void* p;
  int tag;  /* tag -> callout_num */
} OnigValue;


struct OnigMatchParamStruct;
typedef struct OnigMatchParamStruct OnigMatchParam;


/* Oniguruma Native API */

ONIG_EXTERN
int onig_initialize P_((OnigEncoding encodings[], int number_of_encodings));
/* onig_init(): deprecated function. Use onig_initialize(). */
ONIG_EXTERN
int onig_init P_((void));
ONIG_EXTERN
int onig_error_code_to_str PV_((OnigUChar* s, int err_code, ...));
ONIG_EXTERN
void onig_set_warn_func P_((OnigWarnFunc f));
ONIG_EXTERN
void onig_set_verb_warn_func P_((OnigWarnFunc f));
ONIG_EXTERN
int onig_new P_((OnigRegex*, const OnigUChar* pattern, const OnigUChar* pattern_end, OnigOptionType option, OnigEncoding enc, OnigSyntaxType* syntax, OnigErrorInfo* einfo));
ONIG_EXTERN
int  onig_reg_init P_((OnigRegex reg, OnigOptionType option, OnigCaseFoldType case_fold_flag, OnigEncoding enc, OnigSyntaxType* syntax));
int onig_new_without_alloc P_((OnigRegex, const OnigUChar* pattern, const OnigUChar* pattern_end, OnigOptionType option, OnigEncoding enc, OnigSyntaxType* syntax, OnigErrorInfo* einfo));
ONIG_EXTERN
int onig_new_deluxe P_((OnigRegex* reg, const OnigUChar* pattern, const OnigUChar* pattern_end, OnigCompileInfo* ci, OnigErrorInfo* einfo));
ONIG_EXTERN
void onig_free P_((OnigRegex));
ONIG_EXTERN
void onig_free_body P_((OnigRegex));
ONIG_EXTERN
int onig_scan(OnigRegex reg, const OnigUChar* str, const OnigUChar* end, OnigRegion* region, OnigOptionType option, int (*scan_callback)(int, int, OnigRegion*, void*), void* callback_arg);
ONIG_EXTERN
int onig_search P_((OnigRegex, const OnigUChar* str, const OnigUChar* end, const OnigUChar* start, const OnigUChar* range, OnigRegion* region, OnigOptionType option));
ONIG_EXTERN
int onig_search_with_param P_((OnigRegex, const OnigUChar* str, const OnigUChar* end, const OnigUChar* start, const OnigUChar* range, OnigRegion* region, OnigOptionType option, OnigMatchParam* mp));
ONIG_EXTERN
int onig_match P_((OnigRegex, const OnigUChar* str, const OnigUChar* end, const OnigUChar* at, OnigRegion* region, OnigOptionType option));
ONIG_EXTERN
int onig_match_with_param P_((OnigRegex, const OnigUChar* str, const OnigUChar* end, const OnigUChar* at, OnigRegion* region, OnigOptionType option, OnigMatchParam* mp));
ONIG_EXTERN
OnigRegion* onig_region_new P_((void));
ONIG_EXTERN
void onig_region_init P_((OnigRegion* region));
ONIG_EXTERN
void onig_region_free P_((OnigRegion* region, int free_self));
ONIG_EXTERN
void onig_region_copy P_((OnigRegion* to, OnigRegion* from));
ONIG_EXTERN
void onig_region_clear P_((OnigRegion* region));
ONIG_EXTERN
int onig_region_resize P_((OnigRegion* region, int n));
ONIG_EXTERN
int onig_region_set P_((OnigRegion* region, int at, int beg, int end));
ONIG_EXTERN
int onig_name_to_group_numbers P_((OnigRegex reg, const OnigUChar* name, const OnigUChar* name_end, int** nums));
ONIG_EXTERN
int onig_name_to_backref_number P_((OnigRegex reg, const OnigUChar* name, const OnigUChar* name_end, OnigRegion *region));
ONIG_EXTERN
int onig_foreach_name P_((OnigRegex reg, int (*func)(const OnigUChar*, const OnigUChar*,int,int*,OnigRegex,void*), void* arg));
ONIG_EXTERN
int onig_number_of_names P_((OnigRegex reg));
ONIG_EXTERN
int onig_number_of_captures P_((OnigRegex reg));
ONIG_EXTERN
int onig_number_of_capture_histories P_((OnigRegex reg));
ONIG_EXTERN
OnigCaptureTreeNode* onig_get_capture_tree P_((OnigRegion* region));
ONIG_EXTERN
int onig_capture_tree_traverse P_((OnigRegion* region, int at, int(*callback_func)(int,int,int,int,int,void*), void* arg));
ONIG_EXTERN
int onig_noname_group_capture_is_active P_((OnigRegex reg));
ONIG_EXTERN
OnigEncoding onig_get_encoding P_((OnigRegex reg));
ONIG_EXTERN
OnigOptionType onig_get_options P_((OnigRegex reg));
ONIG_EXTERN
OnigCaseFoldType onig_get_case_fold_flag P_((OnigRegex reg));
ONIG_EXTERN
OnigSyntaxType* onig_get_syntax P_((OnigRegex reg));
ONIG_EXTERN
int onig_set_default_syntax P_((OnigSyntaxType* syntax));
ONIG_EXTERN
void onig_copy_syntax P_((OnigSyntaxType* to, OnigSyntaxType* from));
ONIG_EXTERN
unsigned int onig_get_syntax_op P_((OnigSyntaxType* syntax));
ONIG_EXTERN
unsigned int onig_get_syntax_op2 P_((OnigSyntaxType* syntax));
ONIG_EXTERN
unsigned int onig_get_syntax_behavior P_((OnigSyntaxType* syntax));
ONIG_EXTERN
OnigOptionType onig_get_syntax_options P_((OnigSyntaxType* syntax));
ONIG_EXTERN
void onig_set_syntax_op P_((OnigSyntaxType* syntax, unsigned int op));
ONIG_EXTERN
void onig_set_syntax_op2 P_((OnigSyntaxType* syntax, unsigned int op2));
ONIG_EXTERN
void onig_set_syntax_behavior P_((OnigSyntaxType* syntax, unsigned int behavior));
ONIG_EXTERN
void onig_set_syntax_options P_((OnigSyntaxType* syntax, OnigOptionType options));
ONIG_EXTERN
int onig_set_meta_char P_((OnigSyntaxType* syntax, unsigned int what, OnigCodePoint code));
ONIG_EXTERN
void onig_copy_encoding P_((OnigEncoding to, OnigEncoding from));
ONIG_EXTERN
OnigCaseFoldType onig_get_default_case_fold_flag P_((void));
ONIG_EXTERN
int onig_set_default_case_fold_flag P_((OnigCaseFoldType case_fold_flag));
ONIG_EXTERN
unsigned int onig_get_match_stack_limit_size P_((void));
ONIG_EXTERN
int onig_set_match_stack_limit_size P_((unsigned int size));
ONIG_EXTERN
unsigned long onig_get_retry_limit_in_match P_((void));
ONIG_EXTERN
int onig_set_retry_limit_in_match P_((unsigned long n));
ONIG_EXTERN
unsigned int onig_get_parse_depth_limit P_((void));
ONIG_EXTERN
int onig_set_capture_num_limit P_((int num));
ONIG_EXTERN
int onig_set_parse_depth_limit P_((unsigned int depth));
ONIG_EXTERN
int onig_unicode_define_user_property P_((const char* name, OnigCodePoint* ranges));
ONIG_EXTERN
int onig_end P_((void));
ONIG_EXTERN
const char* onig_version P_((void));
ONIG_EXTERN
const char* onig_copyright P_((void));

/* for OnigMatchParam */
ONIG_EXTERN
OnigMatchParam* onig_new_match_param P_((void));
ONIG_EXTERN
void onig_free_match_param P_((OnigMatchParam* p));
ONIG_EXTERN
void onig_free_match_param_content P_((OnigMatchParam* p));
ONIG_EXTERN
int onig_initialize_match_param P_((OnigMatchParam* mp));
ONIG_EXTERN
int onig_set_match_stack_limit_size_of_match_param P_((OnigMatchParam* param, unsigned int limit));
ONIG_EXTERN
int onig_set_retry_limit_in_match_of_match_param P_((OnigMatchParam* param, unsigned long limit));
ONIG_EXTERN
int onig_set_progress_callout_of_match_param P_((OnigMatchParam* param, OnigCalloutFunc f));
ONIG_EXTERN
int onig_set_retraction_callout_of_match_param P_((OnigMatchParam* param, OnigCalloutFunc f));
ONIG_EXTERN
int onig_set_callout_user_data_of_match_param P_((OnigMatchParam* param, void* user_data));

/* for callout functions */
ONIG_EXTERN
OnigCalloutFunc onig_get_progress_callout P_((void));
ONIG_EXTERN
int onig_set_progress_callout P_((OnigCalloutFunc f));
ONIG_EXTERN
OnigCalloutFunc onig_get_retraction_callout P_((void));
ONIG_EXTERN
int onig_set_retraction_callout P_((OnigCalloutFunc f));
ONIG_EXTERN
int onig_set_callout_of_name P_((OnigEncoding enc, OnigCalloutType type, OnigUChar* name, OnigUChar* name_end, int callout_in, OnigCalloutFunc callout, OnigCalloutFunc end_callout, int arg_num, unsigned int arg_types[], int optional_arg_num, OnigValue opt_defaults[]));
ONIG_EXTERN
OnigUChar* onig_get_callout_name_by_name_id P_((int id));
ONIG_EXTERN
int onig_get_callout_num_by_tag P_((OnigRegex reg, const OnigUChar* tag, const OnigUChar* tag_end));
ONIG_EXTERN
int onig_get_callout_data_by_tag P_((OnigRegex reg, OnigMatchParam* mp, const OnigUChar* tag, const OnigUChar* tag_end, int slot, OnigType* type, OnigValue* val));
ONIG_EXTERN
int onig_set_callout_data_by_tag P_((OnigRegex reg, OnigMatchParam* mp, const OnigUChar* tag, const OnigUChar* tag_end, int slot, OnigType type, OnigValue* val));

/* used in callout functions */
ONIG_EXTERN
int onig_get_callout_num_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
OnigCalloutIn onig_get_callout_in_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
int onig_get_name_id_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
const OnigUChar* onig_get_contents_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
const OnigUChar* onig_get_contents_end_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
int onig_get_args_num_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
int onig_get_passed_args_num_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
int onig_get_arg_by_callout_args P_((OnigCalloutArgs* args, int index, OnigType* type, OnigValue* val));
ONIG_EXTERN
const OnigUChar* onig_get_string_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
const OnigUChar* onig_get_string_end_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
const OnigUChar* onig_get_start_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
const OnigUChar* onig_get_right_range_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
const OnigUChar* onig_get_current_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
OnigRegex onig_get_regex_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
unsigned long onig_get_retry_counter_by_callout_args P_((OnigCalloutArgs* args));
ONIG_EXTERN
int onig_callout_tag_is_exist_at_callout_num P_((OnigRegex reg, int callout_num));
ONIG_EXTERN
const OnigUChar* onig_get_callout_tag_start P_((OnigRegex reg, int callout_num));
ONIG_EXTERN
const OnigUChar* onig_get_callout_tag_end P_((OnigRegex reg, int callout_num));
ONIG_EXTERN
int onig_get_callout_data_dont_clear_old P_((OnigRegex reg, OnigMatchParam* mp, int callout_num, int slot, OnigType* type, OnigValue* val));
ONIG_EXTERN
int onig_get_callout_data_by_callout_args_self_dont_clear_old P_((OnigCalloutArgs* args, int slot, OnigType* type, OnigValue* val));
ONIG_EXTERN
int onig_get_callout_data P_((OnigRegex reg, OnigMatchParam* mp, int callout_num, int slot, OnigType* type, OnigValue* val));
ONIG_EXTERN
int onig_get_callout_data_by_callout_args P_((OnigCalloutArgs* args, int callout_num, int slot, OnigType* type, OnigValue* val));
ONIG_EXTERN
int onig_get_callout_data_by_callout_args_self P_((OnigCalloutArgs* args, int slot, OnigType* type, OnigValue* val));
ONIG_EXTERN
int onig_set_callout_data P_((OnigRegex reg, OnigMatchParam* mp, int callout_num, int slot, OnigType type, OnigValue* val));
ONIG_EXTERN
int onig_set_callout_data_by_callout_args P_((OnigCalloutArgs* args, int callout_num, int slot, OnigType type, OnigValue* val));
ONIG_EXTERN
int onig_set_callout_data_by_callout_args_self P_((OnigCalloutArgs* args, int slot, OnigType type, OnigValue* val));
ONIG_EXTERN
int onig_get_capture_range_in_callout P_((OnigCalloutArgs* args, int mem_num, int* begin, int* end));
ONIG_EXTERN
int onig_get_used_stack_size_in_callout P_((OnigCalloutArgs* args, int* used_num, int* used_bytes));

/* builtin callout functions */
ONIG_EXTERN
int onig_builtin_fail P_((OnigCalloutArgs* args, void* user_data));
ONIG_EXTERN
int onig_builtin_mismatch P_((OnigCalloutArgs* args, void* user_data));
ONIG_EXTERN
int onig_builtin_error P_((OnigCalloutArgs* args, void* user_data));
ONIG_EXTERN
int onig_builtin_count P_((OnigCalloutArgs* args, void* user_data));
ONIG_EXTERN
int onig_builtin_total_count P_((OnigCalloutArgs* args, void* user_data));
ONIG_EXTERN
int onig_builtin_max P_((OnigCalloutArgs* args, void* user_data));
ONIG_EXTERN
int onig_builtin_cmp P_((OnigCalloutArgs* args, void* user_data));

ONIG_EXTERN
int onig_setup_builtin_monitors_by_ascii_encoded_name P_((void* fp));

#ifdef __cplusplus
}
#endif

#endif /* ONIGURUMA_H */
