/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* Copyright (c) 1997-2003 University of Cambridge */

#ifndef _PCRE_H
#define _PCRE_H

/* The file pcre.h is build by "configure". Do not edit it; instead
make changes to pcre.in. */

#include "php_compat.h"

#define PCRE_MAJOR          3
#define PCRE_MINOR          92
#define PCRE_DATE           11-Sep-2002

/* Win32 uses DLL by default */

#ifdef _WIN32
# ifdef PHP_EXPORTS
#  define PCRE_DL_IMPORT
# else
#  define PCRE_DL_IMPORT __declspec(dllimport)
# endif
#else
# define PCRE_DL_IMPORT
#endif

/* Have to include stdlib.h in order to ensure that size_t is defined;
it is needed here for malloc. */

#include <stdlib.h>

/* Allow for C++ users */

#ifdef __cplusplus
extern "C" {
#endif

/* Options */

#define PCRE_CASELESS        0x0001
#define PCRE_MULTILINE       0x0002
#define PCRE_DOTALL          0x0004
#define PCRE_EXTENDED        0x0008
#define PCRE_ANCHORED        0x0010
#define PCRE_DOLLAR_ENDONLY  0x0020
#define PCRE_EXTRA           0x0040
#define PCRE_NOTBOL          0x0080
#define PCRE_NOTEOL          0x0100
#define PCRE_UNGREEDY        0x0200
#define PCRE_NOTEMPTY        0x0400
#define PCRE_UTF8            0x0800

/* Exec-time and get-time error codes */

#define PCRE_ERROR_NOMATCH        (-1)
#define PCRE_ERROR_NULL           (-2)
#define PCRE_ERROR_BADOPTION      (-3)
#define PCRE_ERROR_BADMAGIC       (-4)
#define PCRE_ERROR_UNKNOWN_NODE   (-5)
#define PCRE_ERROR_NOMEMORY       (-6)
#define PCRE_ERROR_NOSUBSTRING    (-7)

/* Request types for pcre_fullinfo() */

#define PCRE_INFO_OPTIONS         0
#define PCRE_INFO_SIZE            1
#define PCRE_INFO_CAPTURECOUNT    2
#define PCRE_INFO_BACKREFMAX      3
#define PCRE_INFO_FIRSTCHAR       4
#define PCRE_INFO_FIRSTTABLE      5
#define PCRE_INFO_LASTLITERAL     6
#define PCRE_INFO_NAMEENTRYSIZE   7
#define PCRE_INFO_NAMECOUNT       8
#define PCRE_INFO_NAMETABLE       9

/* Types */

struct real_pcre;        /* declaration; the definition is private  */
struct real_pcre_extra;  /* declaration; the definition is private */

typedef struct real_pcre pcre;
typedef struct real_pcre_extra pcre_extra;

/* The structure for passing out data via the pcre_callout_function. We use a
structure so that new fields can be added on the end in future versions,
without changing the API of the function, thereby allowing old clients to work
without modification. */

typedef struct pcre_callout_block {
  int          version;           /* Identifies version of block */
  /* ------------------------ Version 0 ------------------------------- */
  int          callout_number;    /* Number compiled into pattern */
  int         *offset_vector;     /* The offset vector */
  const char  *subject;           /* The subject being matched */
  int          subject_length;    /* The length of the subject */
  int          start_match;       /* Offset to start of this match attempt */
  int          current_position;  /* Where we currently are */
  int          capture_top;       /* Max current capture */
  int          capture_last;      /* Most recently closed capture */
  /* ------------------------------------------------------------------ */
} pcre_callout_block;

/* Indirection for store get and free functions. These can be set to
alternative malloc/free functions if required. There is also an optional
callout function that is triggered by the (?) regex item. Some magic is
required for Win32 DLL; it is null on other OS. For Virtual Pascal, these have
to be different again. */

#ifndef VPCOMPAT
PCRE_DL_IMPORT extern void *(*pcre_malloc)(size_t);
PCRE_DL_IMPORT extern void  (*pcre_free)(void *);
PCRE_DL_IMPORT extern int   (*pcre_callout)(pcre_callout_block *);
#else   /* VPCOMPAT */
extern void *pcre_malloc(size_t);
extern void  pcre_free(void *);
extern int   pcre_callout(pcre_callout_block *);
#endif  /* VPCOMPAT */

/* Exported PCRE functions */

PCRE_DL_IMPORT extern pcre *pcre_compile(const char *, int, const char **,
                             int *, const unsigned char *);
PCRE_DL_IMPORT extern int  pcre_copy_substring(const char *, int *, int, int,
                             char *, int);
PCRE_DL_IMPORT extern int  pcre_exec(const pcre *, const pcre_extra *,
                             const char *, int, int, int, int *, int);
PCRE_DL_IMPORT extern void pcre_free_substring(const char *);
PCRE_DL_IMPORT extern void pcre_free_substring_list(const char **);
PCRE_DL_IMPORT extern int  pcre_get_substring(const char *, int *, int, int,
                             const char **);
PCRE_DL_IMPORT extern int  pcre_get_substring_list(const char *, int *, int,
                             const char ***);
PCRE_DL_IMPORT extern int  pcre_info(const pcre *, int *, int *);
PCRE_DL_IMPORT extern int  pcre_fullinfo(const pcre *, const pcre_extra *, int,
                             void *);
PCRE_DL_IMPORT extern const unsigned char *pcre_maketables(void);
PCRE_DL_IMPORT extern pcre_extra *pcre_study(const pcre *, int, const char **);
PCRE_DL_IMPORT extern const char *pcre_version(void);

#undef PCRE_DL_IMPORT

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* End of pcre.h */
