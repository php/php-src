/**********************************************************************

  onigposix.h - Oniguruma (regular expression library)

  Copyright (C) 2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef ONIGPOSIX_H
#define ONIGPOSIX_H

/* options */
#define REG_ICASE          (1<<0)
#define REG_NEWLINE        (1<<1)
#define REG_NOTBOL         (1<<2)
#define REG_NOTEOL         (1<<3)
#define REG_EXTENDED       (1<<4) /* if not setted, Basic Regular Expression */
#define REG_NOSUB          (1<<5)

/* POSIX error codes */
#define REG_NOMATCH          1
#define REG_BADPAT           2
#define REG_ECOLLATE         3
#define REG_ECTYPE           4
#define REG_EESCAPE          5
#define REG_ESUBREG          6
#define REG_EBRACK           7
#define REG_EPAREN           8
#define REG_EBRACE           9
#define REG_BADBR           10
#define REG_ERANGE          11
#define REG_ESPACE          12
#define REG_BADRPT          13

/* extended error codes */
#define REG_EONIG_INTERNAL  14
#define REG_EONIG_BADWC     15
#define REG_EONIG_BADARG    16
#define REG_EONIG_THREAD    17

/* character encodings (for reg_set_encoding()) */
/* These value must be same with MBCTYPE_XXXX in oniguruma.h.*/
#define REG_ENCODING_ASCII   0
#define REG_ENCODING_EUC_JP  1
#define REG_ENCODING_SJIS    2
#define REG_ENCODING_UTF8    3

#include <stdlib.h>


typedef int regoff_t;

typedef struct {
  regoff_t  rm_so;
  regoff_t  rm_eo;
} regmatch_t;

/* POSIX regex_t */
typedef struct {
  void*   onig;          /* Oniguruma regex_t*  */
  size_t  re_nsub;
  int     comp_options;
} regex_t;


#ifndef P_
#ifdef __STDC__
# define P_(args) args
#else
# define P_(args) ()
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

#ifndef ONIGURUMA_H
typedef unsigned int        RegOptionType;

/* syntax */
typedef struct {
  unsigned int op;
  unsigned int op2;
  unsigned int behavior;
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

REG_EXTERN int    regex_set_default_syntax P_((RegSyntaxType* syntax));
#endif /* ONIGURUMA_H */


REG_EXTERN int    regcomp P_((regex_t* reg, const char* pat, int options));
REG_EXTERN int    regexec P_((regex_t* reg, const char* str, size_t nmatch, regmatch_t* matches, int options));
REG_EXTERN void   regfree P_((regex_t* reg));
REG_EXTERN size_t regerror P_((int code, const regex_t* reg, char* buf, size_t size));

/* extended API */
REG_EXTERN void reg_set_encoding P_((int enc));
REG_EXTERN int  reg_name_to_group_numbers P_((regex_t* reg, unsigned char* name, unsigned char* name_end, int** nums));
REG_EXTERN int  reg_foreach_name P_((regex_t* reg, int (*func)(unsigned char*,int,int*,void*), void* arg));

#endif /* ONIGPOSIX_H */
