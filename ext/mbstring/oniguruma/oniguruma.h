/**********************************************************************

  oniguruma.h - Oniguruma (regular expression library)

  Copyright (C) 2002-2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef ONIGURUMA_H
#define ONIGURUMA_H

#include "php_compat.h"

#define ONIGURUMA
#define ONIGURUMA_VERSION_MAJOR   1
#define ONIGURUMA_VERSION_MINOR   9
#define ONIGURUMA_VERSION_TEENY   1

/* config parameters */
#ifndef RE_NREGS
#define RE_NREGS                             10
#endif
#define REG_NREGION                    RE_NREGS
#define REG_MAX_BACKREF_NUM                1000
#define REG_MAX_REPEAT_NUM               100000
#define REG_MAX_MULTI_BYTE_RANGES_NUM      1000
/* constants */
#define REG_MAX_ERROR_MESSAGE_LEN            90

#ifndef P_
#ifdef __STDC__
# define P_(args) args
#else
# define P_(args) ()
#endif
#endif

#ifndef PV_
#ifdef HAVE_STDARG_PROTOTYPES
# define PV_(args) args
#else
# define PV_(args) ()
#endif
#endif

#ifndef REG_EXTERN
#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(EXPORT) || defined(RUBY_EXPORT)
#define REG_EXTERN   extern __declspec(dllexport)
#elif defined(IMPORT)
#define REG_EXTERN   extern __declspec(dllimport)
#endif
#endif
#endif

#ifndef REG_EXTERN
#define REG_EXTERN   extern
#endif

#define REG_CHAR_TABLE_SIZE   256

#define REGCODE_UNDEF         ((RegCharEncoding )0)

#if defined(RUBY_PLATFORM) && defined(M17N_H)
#define REG_RUBY_M17N
typedef m17n_encoding*        RegCharEncoding;
#define REGCODE_DEFAULT       REGCODE_UNDEF
#else
typedef const char*           RegCharEncoding;
#define MBCTYPE_ASCII         0
#define MBCTYPE_EUC           1
#define MBCTYPE_SJIS          2
#define MBCTYPE_UTF8          3

#define REGCODE_ASCII         REG_MBLEN_TABLE[MBCTYPE_ASCII]
#define REGCODE_UTF8          REG_MBLEN_TABLE[MBCTYPE_UTF8]
#define REGCODE_EUCJP         REG_MBLEN_TABLE[MBCTYPE_EUC]
#define REGCODE_SJIS          REG_MBLEN_TABLE[MBCTYPE_SJIS]
#define REGCODE_DEFAULT       REGCODE_ASCII

REG_EXTERN const char REG_MBLEN_TABLE[][REG_CHAR_TABLE_SIZE];
#endif /* else RUBY && M17N */

REG_EXTERN RegCharEncoding RegDefaultCharEncoding;

#if defined(RUBY_PLATFORM) && !defined(M17N_H)
#undef ismbchar
#define ismbchar(c)    (mbclen((c)) != 1)
#define mbclen(c)      RegDefaultCharEncoding[(unsigned char )(c)]
#endif

typedef unsigned int        RegOptionType;
typedef unsigned char*      RegTransTableType;
typedef unsigned int        RegDistance;
typedef unsigned char       UChar;

#define REG_OPTION_DEFAULT            REG_OPTION_NONE

/* GNU regex options */
#define RE_OPTION_IGNORECASE    (1L)
#define RE_OPTION_EXTENDED      (RE_OPTION_IGNORECASE << 1)
#define RE_OPTION_MULTILINE     (RE_OPTION_EXTENDED   << 1)
#define RE_OPTION_SINGLELINE    (RE_OPTION_MULTILINE  << 1)
#define RE_OPTION_POSIXLINE     (RE_OPTION_MULTILINE|RE_OPTION_SINGLELINE)
#define RE_OPTION_LONGEST       (RE_OPTION_SINGLELINE << 1)

/* options */
#define REG_OPTION_NONE                      0
#define REG_OPTION_SINGLELINE                RE_OPTION_SINGLELINE
#define REG_OPTION_MULTILINE                 RE_OPTION_MULTILINE
#define REG_OPTION_IGNORECASE                RE_OPTION_IGNORECASE
#define REG_OPTION_EXTEND                    RE_OPTION_EXTENDED
#define REG_OPTION_FIND_LONGEST              RE_OPTION_LONGEST
#define REG_OPTION_FIND_NOT_EMPTY           (REG_OPTION_FIND_LONGEST   << 1)
#define REG_OPTION_NEGATE_SINGLELINE        (REG_OPTION_FIND_NOT_EMPTY << 1)
#define REG_OPTION_CAPTURE_ONLY_NAMED_GROUP (REG_OPTION_NEGATE_SINGLELINE << 1)
/* options (search time) */
#define REG_OPTION_NOTBOL            (REG_OPTION_CAPTURE_ONLY_NAMED_GROUP << 1)
#define REG_OPTION_NOTEOL            (REG_OPTION_NOTBOL << 1)
#define REG_OPTION_POSIX_REGION      (REG_OPTION_NOTEOL << 1)

#define REG_OPTION_ON(options,regopt)      ((options) |= (regopt))
#define REG_OPTION_OFF(options,regopt)     ((options) &= ~(regopt))
#define IS_REG_OPTION_ON(options,option)   ((options) & (option))

/* syntax */
typedef struct {
  unsigned int  op;
  unsigned int  op2;
  unsigned int  behavior;
  RegOptionType options;    /* default option */
} RegSyntaxType;

REG_EXTERN RegSyntaxType RegSyntaxPosixBasic;
REG_EXTERN RegSyntaxType RegSyntaxPosixExtended;
REG_EXTERN RegSyntaxType RegSyntaxEmacs;
REG_EXTERN RegSyntaxType RegSyntaxGrep;
REG_EXTERN RegSyntaxType RegSyntaxGnuRegex;
REG_EXTERN RegSyntaxType RegSyntaxJava;
REG_EXTERN RegSyntaxType RegSyntaxPerl;
REG_EXTERN RegSyntaxType RegSyntaxRuby;

/* predefined syntaxes (see regcomp.c) */
#define REG_SYNTAX_POSIX_BASIC        (&RegSyntaxPosixBasic)
#define REG_SYNTAX_POSIX_EXTENDED     (&RegSyntaxPosixExtended)
#define REG_SYNTAX_EMACS              (&RegSyntaxEmacs)
#define REG_SYNTAX_GREP               (&RegSyntaxGrep)
#define REG_SYNTAX_GNU_REGEX          (&RegSyntaxGnuRegex)
#define REG_SYNTAX_JAVA               (&RegSyntaxJava)
#define REG_SYNTAX_PERL               (&RegSyntaxPerl)
#define REG_SYNTAX_RUBY               (&RegSyntaxRuby)

/* default syntax */
#define REG_SYNTAX_DEFAULT             RegDefaultSyntax

REG_EXTERN RegSyntaxType*  RegDefaultSyntax;

/* syntax (operators) */
#define REG_SYN_OP_ANYCHAR                         1    /* . */
#define REG_SYN_OP_0INF                        (1<<1)   /* * */
#define REG_SYN_OP_ESC_0INF                    (1<<2)
#define REG_SYN_OP_1INF                        (1<<3)   /* + */
#define REG_SYN_OP_ESC_1INF                    (1<<4)
#define REG_SYN_OP_01                          (1<<5)   /* ? */
#define REG_SYN_OP_ESC_01                      (1<<6)
#define REG_SYN_OP_INTERVAL                    (1<<7)   /* {lower,upper} */
#define REG_SYN_OP_ESC_INTERVAL                (1<<8)
#define REG_SYN_OP_ALT                         (1<<9)   /* | */
#define REG_SYN_OP_ESC_ALT                     (1<<10)
#define REG_SYN_OP_SUBEXP                      (1<<11)  /* (...) */
#define REG_SYN_OP_ESC_SUBEXP                  (1<<12)
#define REG_SYN_OP_ESC_BUF_ANCHOR              (1<<13)  /* \A, \Z, \z */
#define REG_SYN_OP_ESC_GNU_BUF_ANCHOR          (1<<14)  /* \`, \' */
#define REG_SYN_OP_BACK_REF                    (1<<15)  /* \num */
#define REG_SYN_OP_CC                          (1<<16)  /* [...] */
#define REG_SYN_OP_ESC_WORD                    (1<<17)  /* \w, \W */
#define REG_SYN_OP_ESC_WORD_BEGIN_END          (1<<18)  /* \<. \> */
#define REG_SYN_OP_ESC_WORD_BOUND              (1<<19)  /* \b, \B */
#define REG_SYN_OP_ESC_WHITE_SPACE             (1<<20)  /* \s, \S */
#define REG_SYN_OP_ESC_DIGIT                   (1<<21)  /* \d, \D */
#define REG_SYN_OP_LINE_ANCHOR                 (1<<22)  /* ^, $ */
#define REG_SYN_OP_POSIX_BRACKET               (1<<23)  /* [:xxxx:] */
#define REG_SYN_OP_NON_GREEDY                  (1<<24)  /* ??,*?,+?,{n,m}? */
#define REG_SYN_OP_ESC_CONTROL_CHAR            (1<<25)  /* \n,\r,\t,\a ... */
#define REG_SYN_OP_ESC_C_CONTROL               (1<<26)  /* \cx  */
#define REG_SYN_OP_ESC_OCTAL3                  (1<<27)  /* \OOO         */
#define REG_SYN_OP_ESC_X_HEX2                  (1<<28)  /* \xHH         */
#define REG_SYN_OP_ESC_X_BRACE_HEX8            (1<<29)  /* \x{7HHHHHHH} */
#define REG_SYN_OP_SUBEXP_EFFECT               (1<<30)  /* (?...) */
#define REG_SYN_OP_QUOTE                       (1<<31)  /* \Q...\E */

#define REG_SYN_OP2_OPTION_PERL                (1<<0)   /* (?imsx), (?-imsx) */
#define REG_SYN_OP2_OPTION_RUBY                (1<<1)   /* (?imx),  (?-imx)  */
#define REG_SYN_OP2_POSSESSIVE_REPEAT          (1<<2)   /* ?+,*+,++ */
#define REG_SYN_OP2_POSSESSIVE_INTERVAL        (1<<3)   /* {n,m}+ */
#define REG_SYN_OP2_CCLASS_SET                 (1<<4)   /* [...&&..[..].] */
#define REG_SYN_OP2_NAMED_SUBEXP               (1<<5)   /*(?<name>.),\k<name>*/
#define REG_SYN_OP2_SUBEXP_CALL                (1<<6)   /* \g<name> */
#define REG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL  (1<<7)   /* \C-x */
#define REG_SYN_OP2_ESC_M_BAR_META             (1<<8)   /* \M-x */
#define REG_SYN_OP2_ESC_V_VTAB                 (1<<9)   /* \v as VTAB */
#define REG_SYN_OP2_ESC_U_HEX4                 (1<<10)  /* \uHHHH */

/* syntax (behavior) */
#define REG_SYN_CONTEXT_INDEP_ANCHORS          (1<<0)   /* not implemented */
#define REG_SYN_CONTEXT_INDEP_OPS              (1<<1)   /* ?, *, +, {n,m} */
#define REG_SYN_CONTEXT_INVALID_OPS            (1<<2)   /* error or ignore */
#define REG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP   (1<<3)   /* ...)... */
#define REG_SYN_ALLOW_INVALID_INTERVAL         (1<<4)   /* {??? */
#define REG_SYN_STRICT_CHECK_BACKREF           (1<<5)   /* /(\1)/,/\1()/ etc.*/
#define REG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND  (1<<6)   /* (?<=a|bc) */

/* syntax in char class [...] */
#define REG_SYN_WARN_FOR_CC_OP_NOT_ESCAPED     (1<<10)   /* [,-,] */
#define REG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC     (1<<11)
#define REG_SYN_ESCAPE_IN_CC                   (1<<12)   /* [...\w..] etc.. */
#define REG_SYN_ALLOW_EMPTY_RANGE_IN_CC        (1<<13)
#define REG_SYN_ALLOW_RANGE_OP_IN_CC           (1<<14)  /* [0-9-a] */


/* error codes */
#define REG_IS_PATTERN_ERROR(ecode)   ((ecode) <= -100 && (ecode) > -300)
/* normal return */
#define REG_NORMAL                                             0
#define REG_MISMATCH                                          -1
#define REG_NO_SUPPORT_CONFIG                                 -2
/* internal error */
#define REGERR_MEMORY                                         -5
#define REGERR_MATCH_STACK_LIMIT_OVER                         -6
#define REGERR_TYPE_BUG                                      -10
#define REGERR_PARSER_BUG                                    -11
#define REGERR_STACK_BUG                                     -12
#define REGERR_UNDEFINED_BYTECODE                            -13
#define REGERR_UNEXPECTED_BYTECODE                           -14
#define REGERR_DEFAULT_ENCODING_IS_NOT_SETTED                -21
#define REGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR  -22
/* syntax error */
#define REGERR_END_PATTERN_AT_LEFT_BRACE                    -100
#define REGERR_END_PATTERN_AT_LEFT_BRACKET                  -101
#define REGERR_EMPTY_CHAR_CLASS                             -102
#define REGERR_PREMATURE_END_OF_CHAR_CLASS                  -103
#define REGERR_END_PATTERN_AT_BACKSLASH                     -104
#define REGERR_END_PATTERN_AT_META                          -105
#define REGERR_END_PATTERN_AT_CONTROL                       -106
#define REGERR_META_CODE_SYNTAX                             -108
#define REGERR_CONTROL_CODE_SYNTAX                          -109
#define REGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE             -110
#define REGERR_CHAR_CLASS_VALUE_AT_START_OF_RANGE           -111
#define REGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS      -112
#define REGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED      -113
#define REGERR_TARGET_OF_REPEAT_OPERATOR_INVALID            -114
#define REGERR_NESTED_REPEAT_OPERATOR                       -115
#define REGERR_UNMATCHED_CLOSE_PARENTHESIS                  -116
#define REGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS       -117
#define REGERR_END_PATTERN_IN_GROUP                         -118
#define REGERR_UNDEFINED_GROUP_OPTION                       -119
#define REGERR_INVALID_POSIX_BRACKET_TYPE                   -121
#define REGERR_INVALID_LOOK_BEHIND_PATTERN                  -122
#define REGERR_INVALID_REPEAT_RANGE_PATTERN                 -123
/* values error (syntax error) */
#define REGERR_TOO_BIG_NUMBER                               -200
#define REGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE              -201
#define REGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE     -202
#define REGERR_EMPTY_RANGE_IN_CHAR_CLASS                    -203
#define REGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE          -204
#define REGERR_TOO_MANY_MULTI_BYTE_RANGES                   -205
#define REGERR_TOO_SHORT_MULTI_BYTE_STRING                  -206
#define REGERR_TOO_BIG_BACKREF_NUMBER                       -207
#define REGERR_INVALID_BACKREF                              -208
#define REGERR_TOO_BIG_WIDE_CHAR_VALUE                      -209
#define REGERR_TOO_LONG_WIDE_CHAR_VALUE                     -210
#define REGERR_INVALID_WIDE_CHAR_VALUE                      -211
#define REGERR_INVALID_SUBEXP_NAME                          -212
#define REGERR_UNDEFINED_NAME_REFERENCE                     -213
#define REGERR_UNDEFINED_GROUP_REFERENCE                    -214
#define REGERR_MULTIPLEX_DEFINITION_NAME_CALL               -215
#define REGERR_NEVER_ENDING_RECURSION                       -216
/* errors related to thread */
#define REGERR_OVER_THREAD_PASS_LIMIT_COUNT                 -301


/* match result region type */
struct re_registers {
  int  allocated;
  int  num_regs;
  int* beg;
  int* end;
};

#define REG_REGION_NOTPOS            -1

typedef struct re_registers   RegRegion;

typedef struct {
  UChar* par;
  UChar* par_end;
} RegErrorInfo;

typedef struct {
  int lower;
  int upper;
} RegRepeatRange;

/* regex_t state */
#define REG_STATE_NORMAL              0
#define REG_STATE_SEARCHING           1
#define REG_STATE_COMPILING          -1
#define REG_STATE_MODIFY             -2

#define REG_STATE(regex) \
  ((regex)->state > 0 ? REG_STATE_SEARCHING : (regex)->state)

typedef struct re_pattern_buffer {
  /* common members in BBuf(bytes-buffer) type */
  unsigned char* p;         /* compiled pattern */
  unsigned int used;        /* used space for p */
  unsigned int alloc;       /* allocated space for p */

  int state;                /* normal, searching, compiling */
  int num_mem;              /* used memory(...) num counted from 1 */
  int num_repeat;           /* OP_REPEAT/OP_REPEAT_NG id-counter */
  int num_null_check;       /* OP_NULL_CHECK_START/END id counter */
  int num_call;             /* number of subexp call */
  unsigned int backtrack_mem;
  int stack_pop_level;
  int repeat_range_alloc;
  RegRepeatRange* repeat_range;

  RegCharEncoding   enc;
  RegOptionType     options;
  RegSyntaxType*    syntax;
  void*             name_table;

  /* optimize info (string search and char-map and anchor) */
  int            optimize;          /* optimize flag */
  int            threshold_len;     /* search str-length for apply optimize */
  int            anchor;            /* BEGIN_BUF, BEGIN_POS, (SEMI_)END_BUF */
  RegDistance    anchor_dmin;       /* (SEMI_)END_BUF anchor distance */
  RegDistance    anchor_dmax;       /* (SEMI_)END_BUF anchor distance */
  int            sub_anchor;        /* start-anchor for exact or map */
  unsigned char *exact;
  unsigned char *exact_end;
  unsigned char  map[REG_CHAR_TABLE_SIZE];  /* used as BM skip or char-map */
  int           *int_map;                   /* BM skip for exact_len > 255 */
  int           *int_map_backward;          /* BM skip for backward search */
  RegDistance    dmin;                      /* min-distance of exact or map */
  RegDistance    dmax;                      /* max-distance of exact or map */

  /* regex_t link chain */
  struct re_pattern_buffer* chain; /* escape compile-conflict on multi-thread */
} regex_t;

#ifdef RUBY_PLATFORM
#define re_mbcinit              ruby_re_mbcinit
#define re_compile_pattern      ruby_re_compile_pattern
#define re_recompile_pattern    ruby_re_recompile_pattern
#define re_free_pattern         ruby_re_free_pattern
#define re_adjust_startpos      ruby_re_adjust_startpos
#define re_search               ruby_re_search
#define re_match                ruby_re_match
#define re_set_casetable        ruby_re_set_casetable
#define re_copy_registers       ruby_re_copy_registers
#define re_free_registers       ruby_re_free_registers
#define register_info_type      ruby_register_info_type
#define re_error_code_to_str    ruby_error_code_to_str

#define ruby_error_code_to_str  regex_error_code_to_str
#define ruby_re_copy_registers  regex_region_copy
#else
#define re_error_code_to_str    regex_error_code_to_str
#define re_copy_registers       regex_region_copy
#endif

/* Oniguruma Native API */
REG_EXTERN
int regex_init P_((void));
REG_EXTERN
int regex_error_code_to_str PV_((UChar* s, int err_code, ...));
REG_EXTERN
int regex_new P_((regex_t**, UChar* pattern, UChar* pattern_end, RegOptionType option, RegCharEncoding code, RegSyntaxType* syntax, RegErrorInfo* einfo));
REG_EXTERN
void regex_free P_((regex_t*));
REG_EXTERN
int regex_recompile P_((regex_t*, UChar* pattern, UChar* pattern_end, RegOptionType option, RegCharEncoding code, RegSyntaxType* syntax, RegErrorInfo* einfo));
REG_EXTERN
int regex_search P_((regex_t*, UChar* str, UChar* end, UChar* start, UChar* range, RegRegion* region, RegOptionType option));
REG_EXTERN
int regex_match P_((regex_t*, UChar* str, UChar* end, UChar* at, RegRegion* region, RegOptionType option));
REG_EXTERN
RegRegion* regex_region_new P_((void));
REG_EXTERN
void regex_region_free P_((RegRegion* region, int free_self));
REG_EXTERN
void regex_region_copy P_((RegRegion* to, RegRegion* from));
REG_EXTERN
void regex_region_clear P_((RegRegion* region));
REG_EXTERN
int regex_region_resize P_((RegRegion* region, int n));
REG_EXTERN
int regex_name_to_group_numbers P_((regex_t* reg, UChar* name, UChar* name_end,
			            int** nums));
REG_EXTERN
int regex_foreach_name P_((regex_t* reg, int (*func)(UChar*,int,int*,void*), void* arg));
REG_EXTERN
UChar* regex_get_prev_char_head P_((RegCharEncoding code, UChar* start, UChar* s));
REG_EXTERN
UChar* regex_get_left_adjust_char_head P_((RegCharEncoding code, UChar* start, UChar* s));
REG_EXTERN
UChar* regex_get_right_adjust_char_head P_((RegCharEncoding code, UChar* start, UChar* s));
REG_EXTERN
void regex_set_default_trans_table P_((UChar* table));
REG_EXTERN
int regex_set_default_syntax P_((RegSyntaxType* syntax));
REG_EXTERN
int regex_end P_((void));
REG_EXTERN
const char* regex_version P_((void));


/* GNU regex API */
#ifdef REG_RUBY_M17N
REG_EXTERN
void re_mbcinit P_((RegCharEncoding));
#else
REG_EXTERN
void re_mbcinit P_((int));
#endif

REG_EXTERN
int re_compile_pattern P_((const char*, int, struct re_pattern_buffer*, char* err_buf));
REG_EXTERN
int re_recompile_pattern P_((const char*, int, struct re_pattern_buffer*, char* err_buf));
REG_EXTERN
void re_free_pattern P_((struct re_pattern_buffer*));
REG_EXTERN
int re_adjust_startpos P_((struct re_pattern_buffer*, const char*, int, int, int));
REG_EXTERN
int re_search P_((struct re_pattern_buffer*, const char*, int, int, int, struct re_registers*));
REG_EXTERN
int re_match P_((struct re_pattern_buffer*, const char *, int, int, struct re_registers*));
REG_EXTERN
void re_set_casetable P_((const char*));
REG_EXTERN
void re_free_registers P_((struct re_registers*));
REG_EXTERN
int re_alloc_pattern P_((struct re_pattern_buffer**));  /* added */

#endif /* ONIGURUMA_H */
