/**********************************************************************

  onigposix.h - Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef ONIGPOSIX_H
#define ONIGPOSIX_H

/* options */
#define REG_ICASE          (1<<0)
#define REG_NEWLINE        (1<<1)
#define REG_NOTBOL         (1<<2)
#define REG_NOTEOL         (1<<3)
#define REG_EXTENDED       (1<<4) /* if not setted, Basic Onigular Expression */
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
#define REG_POSIX_ENCODING_ASCII   0
#define REG_POSIX_ENCODING_EUC_JP  1
#define REG_POSIX_ENCODING_SJIS    2
#define REG_POSIX_ENCODING_UTF8    3

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
#if defined(__STDC__) || defined(_WIN32)
# define P_(args) args
#else
# define P_(args) ()
#endif
#endif

#ifndef ONIG_EXTERN
#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(EXPORT) || defined(RUBY_EXPORT)
#define ONIG_EXTERN   extern __declspec(dllexport)
#else
#define ONIG_EXTERN   extern __declspec(dllimport)
#endif
#endif
#endif

#ifndef ONIG_EXTERN
#define ONIG_EXTERN   extern
#endif

#ifndef ONIGURUMA_H
typedef unsigned int        OnigOptionType;

/* syntax */
typedef struct {
  unsigned int op;
  unsigned int op2;
  unsigned int behavior;
  OnigOptionType options;    /* default option */
} OnigSyntaxType;

ONIG_EXTERN OnigSyntaxType OnigSyntaxPosixBasic;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPosixExtended;
ONIG_EXTERN OnigSyntaxType OnigSyntaxEmacs;
ONIG_EXTERN OnigSyntaxType OnigSyntaxGrep;
ONIG_EXTERN OnigSyntaxType OnigSyntaxGnuRegex;
ONIG_EXTERN OnigSyntaxType OnigSyntaxJava;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPerl;
ONIG_EXTERN OnigSyntaxType OnigSyntaxRuby;

/* predefined syntaxes (see regparse.c) */
#define ONIG_SYNTAX_POSIX_BASIC        (&OnigSyntaxPosixBasic)
#define ONIG_SYNTAX_POSIX_EXTENDED     (&OnigSyntaxPosixExtended)
#define ONIG_SYNTAX_EMACS              (&OnigSyntaxEmacs)
#define ONIG_SYNTAX_GREP               (&OnigSyntaxGrep)
#define ONIG_SYNTAX_GNU_REGEX          (&OnigSyntaxGnuRegex)
#define ONIG_SYNTAX_JAVA               (&OnigSyntaxJava)
#define ONIG_SYNTAX_PERL               (&OnigSyntaxPerl)
#define ONIG_SYNTAX_RUBY               (&OnigSyntaxRuby)
/* default syntax */
#define ONIG_SYNTAX_DEFAULT             OnigDefaultSyntax

ONIG_EXTERN OnigSyntaxType*  OnigDefaultSyntax;

ONIG_EXTERN int  onig_set_default_syntax P_((OnigSyntaxType* syntax));
ONIG_EXTERN void onig_copy_syntax P_((OnigSyntaxType* to, OnigSyntaxType* from));
#endif /* ONIGURUMA_H */


ONIG_EXTERN int    regcomp P_((regex_t* reg, const char* pat, int options));
ONIG_EXTERN int    regexec P_((regex_t* reg, const char* str, size_t nmatch, regmatch_t* matches, int options));
ONIG_EXTERN void   regfree P_((regex_t* reg));
ONIG_EXTERN size_t regerror P_((int code, const regex_t* reg, char* buf, size_t size));

/* extended API */
ONIG_EXTERN void reg_set_encoding P_((int enc));
ONIG_EXTERN int  reg_name_to_group_numbers P_((regex_t* reg, unsigned char* name, unsigned char* name_end, int** nums));
ONIG_EXTERN int  reg_foreach_name P_((regex_t* reg, int (*func)(unsigned char*,unsigned char*,int,int*,regex_t*,void*), void* arg));
ONIG_EXTERN int  reg_number_of_names P_((regex_t* reg));

#endif /* ONIGPOSIX_H */
