#ifndef REGINT_H
#define REGINT_H
/**********************************************************************
  regint.h -  Oniguruma (regular expression library)
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

/* for debug */
/* #define ONIG_DEBUG_PARSE */
/* #define ONIG_DEBUG_COMPILE */
/* #define ONIG_DEBUG_SEARCH */
/* #define ONIG_DEBUG_MATCH */
/* #define ONIG_DONT_OPTIMIZE */

/* for byte-code statistical data. */
/* #define ONIG_DEBUG_STATISTICS */

#if defined(ONIG_DEBUG_PARSE) || defined(ONIG_DEBUG_MATCH) || \
    defined(ONIG_DEBUG_SEARCH) || defined(ONIG_DEBUG_COMPILE) || \
    defined(ONIG_DEBUG_STATISTICS)
#ifndef ONIG_DEBUG
#define ONIG_DEBUG
#endif
#endif

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || \
    (defined(__ppc__) && defined(__APPLE__)) || \
    defined(__x86_64) || defined(__x86_64__) || \
    defined(__mc68020__)
#define PLATFORM_UNALIGNED_WORD_ACCESS
#endif

/* config */
/* spec. config */
#define USE_CALL
#define USE_CALLOUT
#define USE_BACKREF_WITH_LEVEL        /* \k<name+n>, \k<name-n> */
#define USE_INSISTENT_CHECK_CAPTURES_IN_EMPTY_REPEAT    /* /(?:()|())*\2/ */
#define USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE     /* /\n$/ =~ "\n" */
#define USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR

#define USE_RETRY_LIMIT_IN_MATCH

/* internal config */
#define USE_OP_PUSH_OR_JUMP_EXACT
#define USE_QUANT_PEEK_NEXT
#define USE_ST_LIBRARY

#include "regenc.h"

#ifdef __cplusplus
# ifndef  HAVE_STDARG_PROTOTYPES
#  define HAVE_STDARG_PROTOTYPES 1
# endif
#endif

/* escape Mac OS X/Xcode 2.4/gcc 4.0.1 problem */
#if defined(__APPLE__) && defined(__GNUC__) && __GNUC__ >= 4
# ifndef  HAVE_STDARG_PROTOTYPES
#  define HAVE_STDARG_PROTOTYPES 1
# endif
#endif

#ifdef HAVE_STDARG_H
# ifndef  HAVE_STDARG_PROTOTYPES
#  define HAVE_STDARG_PROTOTYPES 1
# endif
#endif


#define INIT_MATCH_STACK_SIZE                     160
#define DEFAULT_MATCH_STACK_LIMIT_SIZE              0 /* unlimited */
#define DEFAULT_RETRY_LIMIT_IN_MATCH         10000000
#define DEFAULT_PARSE_DEPTH_LIMIT                4096

/* */
/* escape other system UChar definition */
#ifdef ONIG_ESCAPE_UCHAR_COLLISION
#undef ONIG_ESCAPE_UCHAR_COLLISION
#endif

#define USE_WORD_BEGIN_END        /* "\<", "\>" */
#define USE_CAPTURE_HISTORY
#define USE_VARIABLE_META_CHARS
#define USE_POSIX_API_REGION_OPTION
#define USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE

#define xmalloc     malloc
#define xrealloc    realloc
#define xcalloc     calloc
#define xfree       free

#define st_init_table                  onig_st_init_table
#define st_init_table_with_size        onig_st_init_table_with_size
#define st_init_numtable               onig_st_init_numtable
#define st_init_numtable_with_size     onig_st_init_numtable_with_size
#define st_init_strtable               onig_st_init_strtable
#define st_init_strtable_with_size     onig_st_init_strtable_with_size
#define st_delete                      onig_st_delete
#define st_delete_safe                 onig_st_delete_safe
#define st_insert                      onig_st_insert
#define st_lookup                      onig_st_lookup
#define st_foreach                     onig_st_foreach
#define st_add_direct                  onig_st_add_direct
#define st_free_table                  onig_st_free_table
#define st_cleanup_safe                onig_st_cleanup_safe
#define st_copy                        onig_st_copy
#define st_nothing_key_clone           onig_st_nothing_key_clone
#define st_nothing_key_free            onig_st_nothing_key_free
/* */
#define onig_st_is_member              st_is_member

#define xmemset     memset
#define xmemcpy     memcpy
#define xmemmove    memmove

#if defined(_WIN32) && !defined(__GNUC__)
#define xalloca     _alloca
#define xvsnprintf(buf,size,fmt,args)  _vsnprintf_s(buf,size,_TRUNCATE,fmt,args)
#define xsnprintf   sprintf_s
#define xstrcat(dest,src,size)   strcat_s(dest,size,src)
#else
#define xalloca     alloca
#define xvsnprintf  vsnprintf
#define xsnprintf   snprintf
#define xstrcat(dest,src,size)   strcat(dest,src)
#endif


#include <stddef.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#if defined(HAVE_ALLOCA_H) && !defined(__GNUC__)
#include <alloca.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include <ctype.h>
#ifdef HAVE_SYS_TYPES_H
#ifndef __BORLANDC__
#include <sys/types.h>
#endif
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef __BORLANDC__
#include <malloc.h>
#endif

#ifdef ONIG_DEBUG
# include <stdio.h>
#endif

#ifdef _WIN32
#if defined(_MSC_VER) && (_MSC_VER < 1300)
typedef int  intptr_t;
typedef unsigned int  uintptr_t;
#endif
#endif

#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif

#define MIN(a,b) (((a)>(b))?(b):(a))
#define MAX(a,b) (((a)<(b))?(b):(a))

#define IS_NULL(p)                    (((void*)(p)) == (void*)0)
#define IS_NOT_NULL(p)                (((void*)(p)) != (void*)0)
#define CHECK_NULL_RETURN(p)          if (IS_NULL(p)) return NULL
#define CHECK_NULL_RETURN_MEMERR(p)   if (IS_NULL(p)) return ONIGERR_MEMORY
#define NULL_UCHARP                   ((UChar* )0)

#define INFINITE_LEN        ONIG_INFINITE_DISTANCE

#ifdef PLATFORM_UNALIGNED_WORD_ACCESS

#define PLATFORM_GET_INC(val,p,type) do{\
  val  = *(type* )p;\
  (p) += sizeof(type);\
} while(0)

#else

#define PLATFORM_GET_INC(val,p,type) do{\
  xmemcpy(&val, (p), sizeof(type));\
  (p) += sizeof(type);\
} while(0)

/* sizeof(OnigCodePoint) */
#ifdef SIZEOF_SIZE_T
# define WORD_ALIGNMENT_SIZE     SIZEOF_SIZE_T
#else
# define WORD_ALIGNMENT_SIZE     SIZEOF_LONG
#endif

#define GET_ALIGNMENT_PAD_SIZE(addr,pad_size) do {\
  (pad_size) = WORD_ALIGNMENT_SIZE - ((uintptr_t )(addr) % WORD_ALIGNMENT_SIZE);\
  if ((pad_size) == WORD_ALIGNMENT_SIZE) (pad_size) = 0;\
} while (0)

#define ALIGNMENT_RIGHT(addr) do {\
  (addr) += (WORD_ALIGNMENT_SIZE - 1);\
  (addr) -= ((uintptr_t )(addr) % WORD_ALIGNMENT_SIZE);\
} while (0)

#endif /* PLATFORM_UNALIGNED_WORD_ACCESS */


#ifdef USE_CALLOUT

typedef struct {
  int           flag;
  OnigCalloutOf of;
  int           in;
  int           name_id;
  const UChar*  tag_start;
  const UChar*  tag_end;
  OnigCalloutType type;
  OnigCalloutFunc start_func;
  OnigCalloutFunc end_func;
  union {
    struct {
      const UChar* start;
      const UChar* end;
    } content;
    struct {
      int num;
      int passed_num;
      OnigType  types[ONIG_CALLOUT_MAX_ARGS_NUM];
      OnigValue vals[ONIG_CALLOUT_MAX_ARGS_NUM];
    } arg;
  } u;
} CalloutListEntry;

#endif

typedef struct {
  const UChar* pattern;
  const UChar* pattern_end;
#ifdef USE_CALLOUT
  void*  tag_table;
  int    callout_num;
  int    callout_list_alloc;
  CalloutListEntry* callout_list;    /* index: callout num */
#endif
} RegexExt;

#define REG_EXTP(reg)      ((RegexExt* )((reg)->chain))
#define REG_EXTPL(reg)     ((reg)->chain)

struct re_pattern_buffer {
  /* common members of BBuf(bytes-buffer) */
  unsigned char* p;         /* compiled pattern */
  unsigned int used;        /* used space for p */
  unsigned int alloc;       /* allocated space for p */

  int num_mem;                   /* used memory(...) num counted from 1 */
  int num_repeat;                /* OP_REPEAT/OP_REPEAT_NG id-counter */
  int num_null_check;            /* OP_EMPTY_CHECK_START/END id counter */
  int num_comb_exp_check;        /* no longer used (combination explosion check) */
  int num_call;                  /* number of subexp call */
  unsigned int capture_history;  /* (?@...) flag (1-31) */
  unsigned int bt_mem_start;     /* need backtrack flag */
  unsigned int bt_mem_end;       /* need backtrack flag */
  int stack_pop_level;
  int repeat_range_alloc;
  OnigRepeatRange* repeat_range;

  OnigEncoding      enc;
  OnigOptionType    options;
  OnigSyntaxType*   syntax;
  OnigCaseFoldType  case_fold_flag;
  void*             name_table;

  /* optimization info (string search, char-map and anchors) */
  int            optimize;          /* optimize flag */
  int            threshold_len;     /* search str-length for apply optimize */
  int            anchor;            /* BEGIN_BUF, BEGIN_POS, (SEMI_)END_BUF */
  OnigLen   anchor_dmin;       /* (SEMI_)END_BUF anchor distance */
  OnigLen   anchor_dmax;       /* (SEMI_)END_BUF anchor distance */
  int            sub_anchor;        /* start-anchor for exact or map */
  unsigned char *exact;
  unsigned char *exact_end;
  unsigned char  map[ONIG_CHAR_TABLE_SIZE]; /* used as BM skip or char-map */
  int           *int_map;                   /* BM skip for exact_len > 255 */
  int           *int_map_backward;          /* BM skip for backward search */
  OnigLen   dmin;                      /* min-distance of exact or map */
  OnigLen   dmax;                      /* max-distance of exact or map */

  /* regex_t link chain */
  struct re_pattern_buffer* chain;  /* escape compile-conflict */
};


/* stack pop level */
enum StackPopLevel {
  STACK_POP_LEVEL_FREE      = 0,
  STACK_POP_LEVEL_MEM_START = 1,
  STACK_POP_LEVEL_ALL       = 2
};

/* optimize flags */
enum OptimizeType {
  OPTIMIZE_NONE            = 0,
  OPTIMIZE_EXACT           = 1,  /* Slow Search */
  OPTIMIZE_EXACT_BM        = 2,  /* Boyer Moore Search */
  OPTIMIZE_EXACT_BM_NO_REV = 3,  /* BM   (but not simple match) */
  OPTIMIZE_EXACT_IC        = 4,  /* Slow Search (ignore case) */
  OPTIMIZE_MAP             = 5   /* char map */
};

/* bit status */
typedef unsigned int  MemStatusType;

#define MEM_STATUS_BITS_NUM          (sizeof(MemStatusType) * 8)
#define MEM_STATUS_CLEAR(stats)      (stats) = 0
#define MEM_STATUS_ON_ALL(stats)     (stats) = ~((MemStatusType )0)
#define MEM_STATUS_AT(stats,n) \
  ((n) < (int )MEM_STATUS_BITS_NUM  ?  ((stats) & ((MemStatusType )1 << n)) : ((stats) & 1))
#define MEM_STATUS_AT0(stats,n) \
  ((n) > 0 && (n) < (int )MEM_STATUS_BITS_NUM  ?  ((stats) & ((MemStatusType )1 << n)) : ((stats) & 1))

#define MEM_STATUS_ON(stats,n) do {\
  if ((n) < (int )MEM_STATUS_BITS_NUM) {\
    if ((n) != 0)\
      (stats) |= ((MemStatusType )1 << (n));\
  }\
  else\
    (stats) |= 1;\
} while (0)

#define MEM_STATUS_ON_SIMPLE(stats,n) do {\
    if ((n) < (int )MEM_STATUS_BITS_NUM)\
    (stats) |= ((MemStatusType )1 << (n));\
} while (0)


#define INT_MAX_LIMIT           ((1UL << (SIZEOF_INT * 8 - 1)) - 1)

#define IS_CODE_WORD_ASCII(enc,code) \
  (ONIGENC_IS_CODE_ASCII(code) && ONIGENC_IS_CODE_WORD(enc,code))
#define IS_CODE_DIGIT_ASCII(enc, code) \
  (ONIGENC_IS_CODE_ASCII(code) && ONIGENC_IS_CODE_DIGIT(enc,code))
#define IS_CODE_XDIGIT_ASCII(enc, code) \
  (ONIGENC_IS_CODE_ASCII(code) && ONIGENC_IS_CODE_XDIGIT(enc,code))

#define DIGITVAL(code)    ((code) - '0')
#define ODIGITVAL(code)   DIGITVAL(code)
#define XDIGITVAL(enc,code) \
  (IS_CODE_DIGIT_ASCII(enc,code) ? DIGITVAL(code) \
   : (ONIGENC_IS_CODE_UPPER(enc,code) ? (code) - 'A' + 10 : (code) - 'a' + 10))

#define IS_SINGLELINE(option)     ((option) & ONIG_OPTION_SINGLELINE)
#define IS_MULTILINE(option)      ((option) & ONIG_OPTION_MULTILINE)
#define IS_IGNORECASE(option)     ((option) & ONIG_OPTION_IGNORECASE)
#define IS_EXTEND(option)         ((option) & ONIG_OPTION_EXTEND)
#define IS_FIND_LONGEST(option)   ((option) & ONIG_OPTION_FIND_LONGEST)
#define IS_FIND_NOT_EMPTY(option) ((option) & ONIG_OPTION_FIND_NOT_EMPTY)
#define IS_FIND_CONDITION(option) ((option) & \
          (ONIG_OPTION_FIND_LONGEST | ONIG_OPTION_FIND_NOT_EMPTY))
#define IS_NOTBOL(option)         ((option) & ONIG_OPTION_NOTBOL)
#define IS_NOTEOL(option)         ((option) & ONIG_OPTION_NOTEOL)
#define IS_POSIX_REGION(option)   ((option) & ONIG_OPTION_POSIX_REGION)

#define IS_WORD_ASCII(option) \
  ((option) & (ONIG_OPTION_WORD_IS_ASCII | ONIG_OPTION_POSIX_IS_ASCII))
#define IS_DIGIT_ASCII(option) \
  ((option) & (ONIG_OPTION_DIGIT_IS_ASCII | ONIG_OPTION_POSIX_IS_ASCII))
#define IS_SPACE_ASCII(option) \
  ((option) & (ONIG_OPTION_SPACE_IS_ASCII | ONIG_OPTION_POSIX_IS_ASCII))
#define IS_POSIX_ASCII(option)    ((option) & ONIG_OPTION_POSIX_IS_ASCII)

#define IS_ASCII_MODE_CTYPE_OPTION(ctype, options) \
  ((ctype) >= 0 && \
  (((ctype) < ONIGENC_CTYPE_ASCII  && IS_POSIX_ASCII(options)) ||\
   ((ctype) == ONIGENC_CTYPE_WORD  && IS_WORD_ASCII(options))  ||\
   ((ctype) == ONIGENC_CTYPE_DIGIT && IS_DIGIT_ASCII(options)) ||\
   ((ctype) == ONIGENC_CTYPE_SPACE && IS_SPACE_ASCII(options))))

#define DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag) \
  ((case_fold_flag) & ~INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR)

#define REPEAT_INFINITE         -1
#define IS_REPEAT_INFINITE(n)   ((n) == REPEAT_INFINITE)

/* bitset */
#define BITS_PER_BYTE      8
#define SINGLE_BYTE_SIZE   (1 << BITS_PER_BYTE)
#define BITS_IN_ROOM       (sizeof(Bits) * BITS_PER_BYTE)
#define BITSET_SIZE        (SINGLE_BYTE_SIZE / BITS_IN_ROOM)

#ifdef PLATFORM_UNALIGNED_WORD_ACCESS
typedef unsigned int   Bits;
#else
typedef unsigned char  Bits;
#endif
typedef Bits           BitSet[BITSET_SIZE];
typedef Bits*          BitSetRef;

#define SIZE_BITSET        sizeof(BitSet)

#define BITSET_CLEAR(bs) do {\
  int i;\
  for (i = 0; i < (int )BITSET_SIZE; i++) { (bs)[i] = 0; } \
} while (0)

#define BS_ROOM(bs,pos)            (bs)[pos / BITS_IN_ROOM]
#define BS_BIT(pos)                (1 << (pos % BITS_IN_ROOM))

#define BITSET_AT(bs, pos)         (BS_ROOM(bs,pos) & BS_BIT(pos))
#define BITSET_SET_BIT(bs, pos)     BS_ROOM(bs,pos) |= BS_BIT(pos)
#define BITSET_CLEAR_BIT(bs, pos)   BS_ROOM(bs,pos) &= ~(BS_BIT(pos))
#define BITSET_INVERT_BIT(bs, pos)  BS_ROOM(bs,pos) ^= BS_BIT(pos)

/* bytes buffer */
typedef struct _BBuf {
  UChar* p;
  unsigned int used;
  unsigned int alloc;
} BBuf;

#define BB_INIT(buf,size)    onig_bbuf_init((BBuf* )(buf), (size))

#define BB_SIZE_INC(buf,inc) do{\
  (buf)->alloc += (inc);\
  (buf)->p = (UChar* )xrealloc((buf)->p, (buf)->alloc);\
  if (IS_NULL((buf)->p)) return(ONIGERR_MEMORY);\
} while (0)

#define BB_EXPAND(buf,low) do{\
  do { (buf)->alloc *= 2; } while ((buf)->alloc < (unsigned int )low);\
  (buf)->p = (UChar* )xrealloc((buf)->p, (buf)->alloc);\
  if (IS_NULL((buf)->p)) return(ONIGERR_MEMORY);\
} while (0)

#define BB_ENSURE_SIZE(buf,size) do{\
  unsigned int new_alloc = (buf)->alloc;\
  while (new_alloc < (unsigned int )(size)) { new_alloc *= 2; }\
  if ((buf)->alloc != new_alloc) {\
    (buf)->p = (UChar* )xrealloc((buf)->p, new_alloc);\
    if (IS_NULL((buf)->p)) return(ONIGERR_MEMORY);\
    (buf)->alloc = new_alloc;\
  }\
} while (0)

#define BB_WRITE(buf,pos,bytes,n) do{\
  int used = (pos) + (n);\
  if ((buf)->alloc < (unsigned int )used) BB_EXPAND((buf),used);\
  xmemcpy((buf)->p + (pos), (bytes), (n));\
  if ((buf)->used < (unsigned int )used) (buf)->used = used;\
} while (0)

#define BB_WRITE1(buf,pos,byte) do{\
  int used = (pos) + 1;\
  if ((buf)->alloc < (unsigned int )used) BB_EXPAND((buf),used);\
  (buf)->p[(pos)] = (byte);\
  if ((buf)->used < (unsigned int )used) (buf)->used = used;\
} while (0)

#define BB_ADD(buf,bytes,n)       BB_WRITE((buf),(buf)->used,(bytes),(n))
#define BB_ADD1(buf,byte)         BB_WRITE1((buf),(buf)->used,(byte))
#define BB_GET_ADD_ADDRESS(buf)   ((buf)->p + (buf)->used)
#define BB_GET_OFFSET_POS(buf)    ((buf)->used)

/* from < to */
#define BB_MOVE_RIGHT(buf,from,to,n) do {\
  if ((unsigned int )((to)+(n)) > (buf)->alloc) BB_EXPAND((buf),(to) + (n));\
  xmemmove((buf)->p + (to), (buf)->p + (from), (n));\
  if ((unsigned int )((to)+(n)) > (buf)->used) (buf)->used = (to) + (n);\
} while (0)

/* from > to */
#define BB_MOVE_LEFT(buf,from,to,n) do {\
  xmemmove((buf)->p + (to), (buf)->p + (from), (n));\
} while (0)

/* from > to */
#define BB_MOVE_LEFT_REDUCE(buf,from,to) do {\
  xmemmove((buf)->p + (to), (buf)->p + (from), (buf)->used - (from));\
  (buf)->used -= (from - to);\
} while (0)

#define BB_INSERT(buf,pos,bytes,n) do {\
  if (pos >= (buf)->used) {\
    BB_WRITE(buf,pos,bytes,n);\
  }\
  else {\
    BB_MOVE_RIGHT((buf),(pos),(pos) + (n),((buf)->used - (pos)));\
    xmemcpy((buf)->p + (pos), (bytes), (n));\
  }\
} while (0)

#define BB_GET_BYTE(buf, pos) (buf)->p[(pos)]


/* has body */
#define ANCHOR_PREC_READ        (1<<0)
#define ANCHOR_PREC_READ_NOT    (1<<1)
#define ANCHOR_LOOK_BEHIND      (1<<2)
#define ANCHOR_LOOK_BEHIND_NOT  (1<<3)
/* no body */
#define ANCHOR_BEGIN_BUF        (1<<4)
#define ANCHOR_BEGIN_LINE       (1<<5)
#define ANCHOR_BEGIN_POSITION   (1<<6)
#define ANCHOR_END_BUF          (1<<7)
#define ANCHOR_SEMI_END_BUF     (1<<8)
#define ANCHOR_END_LINE         (1<<9)
#define ANCHOR_WORD_BOUNDARY    (1<<10)
#define ANCHOR_NO_WORD_BOUNDARY (1<<11)
#define ANCHOR_WORD_BEGIN       (1<<12)
#define ANCHOR_WORD_END         (1<<13)
#define ANCHOR_ANYCHAR_INF      (1<<14)
#define ANCHOR_ANYCHAR_INF_ML   (1<<15)
#define ANCHOR_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY    (1<<16)
#define ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY (1<<17)


#define ANCHOR_HAS_BODY(a)      ((a)->type < ANCHOR_BEGIN_BUF)

#define IS_WORD_ANCHOR_TYPE(type) \
  ((type) == ANCHOR_WORD_BOUNDARY || (type) == ANCHOR_NO_WORD_BOUNDARY || \
   (type) == ANCHOR_WORD_BEGIN || (type) == ANCHOR_WORD_END)

/* operation code */
enum OpCode {
  OP_FINISH = 0,        /* matching process terminator (no more alternative) */
  OP_END    = 1,        /* pattern code terminator (success end) */

  OP_EXACT1 = 2,        /* single byte, N = 1 */
  OP_EXACT2,            /* single byte, N = 2 */
  OP_EXACT3,            /* single byte, N = 3 */
  OP_EXACT4,            /* single byte, N = 4 */
  OP_EXACT5,            /* single byte, N = 5 */
  OP_EXACTN,            /* single byte */
  OP_EXACTMB2N1,        /* mb-length = 2 N = 1 */
  OP_EXACTMB2N2,        /* mb-length = 2 N = 2 */
  OP_EXACTMB2N3,        /* mb-length = 2 N = 3 */
  OP_EXACTMB2N,         /* mb-length = 2 */
  OP_EXACTMB3N,         /* mb-length = 3 */
  OP_EXACTMBN,          /* other length */

  OP_EXACT1_IC,         /* single byte, N = 1, ignore case */
  OP_EXACTN_IC,         /* single byte,        ignore case */

  OP_CCLASS,
  OP_CCLASS_MB,
  OP_CCLASS_MIX,
  OP_CCLASS_NOT,
  OP_CCLASS_MB_NOT,
  OP_CCLASS_MIX_NOT,
#ifdef USE_OP_CCLASS_NODE
  OP_CCLASS_NODE,       /* pointer to CClassNode node */
#endif

  OP_ANYCHAR,                 /* "."  */
  OP_ANYCHAR_ML,              /* "."  multi-line */
  OP_ANYCHAR_STAR,            /* ".*" */
  OP_ANYCHAR_ML_STAR,         /* ".*" multi-line */
  OP_ANYCHAR_STAR_PEEK_NEXT,
  OP_ANYCHAR_ML_STAR_PEEK_NEXT,

  OP_WORD,
  OP_WORD_ASCII,
  OP_NO_WORD,
  OP_NO_WORD_ASCII,
  OP_WORD_BOUNDARY,
  OP_NO_WORD_BOUNDARY,
  OP_WORD_BEGIN,
  OP_WORD_END,

  OP_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY,
  OP_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY,

  OP_BEGIN_BUF,
  OP_END_BUF,
  OP_BEGIN_LINE,
  OP_END_LINE,
  OP_SEMI_END_BUF,
  OP_BEGIN_POSITION,

  OP_BACKREF1,
  OP_BACKREF2,
  OP_BACKREF_N,
  OP_BACKREF_N_IC,
  OP_BACKREF_MULTI,
  OP_BACKREF_MULTI_IC,
  OP_BACKREF_WITH_LEVEL,        /* \k<xxx+n>, \k<xxx-n> */
  OP_BACKREF_CHECK,             /* (?(n)), (?('name')) */
  OP_BACKREF_CHECK_WITH_LEVEL,  /* (?(n-level)), (?('name-level')) */

  OP_MEMORY_START,
  OP_MEMORY_START_PUSH,   /* push back-tracker to stack */
  OP_MEMORY_END_PUSH,     /* push back-tracker to stack */
  OP_MEMORY_END_PUSH_REC, /* push back-tracker to stack */
  OP_MEMORY_END,
  OP_MEMORY_END_REC,      /* push marker to stack */

  OP_FAIL,               /* pop stack and move */
  OP_JUMP,
  OP_PUSH,
  OP_PUSH_SUPER,
  OP_POP_OUT,
  OP_PUSH_OR_JUMP_EXACT1,  /* if match exact then push, else jump. */
  OP_PUSH_IF_PEEK_NEXT,    /* if match exact then push, else none. */
  OP_REPEAT,               /* {n,m} */
  OP_REPEAT_NG,            /* {n,m}? (non greedy) */
  OP_REPEAT_INC,
  OP_REPEAT_INC_NG,        /* non greedy */
  OP_REPEAT_INC_SG,        /* search and get in stack */
  OP_REPEAT_INC_NG_SG,     /* search and get in stack (non greedy) */
  OP_EMPTY_CHECK_START,     /* null loop checker start */
  OP_EMPTY_CHECK_END,       /* null loop checker end   */
  OP_EMPTY_CHECK_END_MEMST, /* null loop checker end (with capture status) */
  OP_EMPTY_CHECK_END_MEMST_PUSH, /* with capture status and push check-end */

  OP_PREC_READ_START,       /* (?=...)  start */
  OP_PREC_READ_END,         /* (?=...)  end   */
  OP_PREC_READ_NOT_START,   /* (?!...)  start */
  OP_PREC_READ_NOT_END,     /* (?!...)  end   */
  OP_ATOMIC_START,          /* (?>...)  start */
  OP_ATOMIC_END,            /* (?>...)  end   */
  OP_LOOK_BEHIND,           /* (?<=...) start (no needs end opcode) */
  OP_LOOK_BEHIND_NOT_START, /* (?<!...) start */
  OP_LOOK_BEHIND_NOT_END,   /* (?<!...) end   */

  OP_CALL,                  /* \g<name> */
  OP_RETURN,
  OP_PUSH_SAVE_VAL,
  OP_UPDATE_VAR,
#ifdef USE_CALLOUT
  OP_CALLOUT_CONTENTS,      /* (?{...}) (?{{...}}) */
  OP_CALLOUT_NAME,          /* (*name) (*name[tag](args...)) */
#endif
};

enum SaveType {
  SAVE_KEEP = 0, /* SAVE S */
  SAVE_S = 1,
  SAVE_RIGHT_RANGE = 2,
};

enum UpdateVarType {
  UPDATE_VAR_KEEP_FROM_STACK_LAST     = 0,
  UPDATE_VAR_S_FROM_STACK             = 1,
  UPDATE_VAR_RIGHT_RANGE_FROM_STACK   = 2,
  UPDATE_VAR_RIGHT_RANGE_FROM_S_STACK = 3,
  UPDATE_VAR_RIGHT_RANGE_INIT         = 4,
};

typedef int RelAddrType;
typedef int AbsAddrType;
typedef int LengthType;
typedef int RepeatNumType;
typedef int MemNumType;
typedef void* PointerType;
typedef int SaveType;
typedef int UpdateVarType;
typedef int ModeType;

#define SIZE_OPCODE           1
#define SIZE_RELADDR          sizeof(RelAddrType)
#define SIZE_ABSADDR          sizeof(AbsAddrType)
#define SIZE_LENGTH           sizeof(LengthType)
#define SIZE_MEMNUM           sizeof(MemNumType)
#define SIZE_REPEATNUM        sizeof(RepeatNumType)
#define SIZE_OPTION           sizeof(OnigOptionType)
#define SIZE_CODE_POINT       sizeof(OnigCodePoint)
#define SIZE_POINTER          sizeof(PointerType)
#define SIZE_SAVE_TYPE        sizeof(SaveType)
#define SIZE_UPDATE_VAR_TYPE  sizeof(UpdateVarType)
#define SIZE_MODE             sizeof(ModeType)

#define GET_RELADDR_INC(addr,p)    PLATFORM_GET_INC(addr,   p, RelAddrType)
#define GET_ABSADDR_INC(addr,p)    PLATFORM_GET_INC(addr,   p, AbsAddrType)
#define GET_LENGTH_INC(len,p)      PLATFORM_GET_INC(len,    p, LengthType)
#define GET_MEMNUM_INC(num,p)      PLATFORM_GET_INC(num,    p, MemNumType)
#define GET_REPEATNUM_INC(num,p)   PLATFORM_GET_INC(num,    p, RepeatNumType)
#define GET_OPTION_INC(option,p)   PLATFORM_GET_INC(option, p, OnigOptionType)
#define GET_POINTER_INC(ptr,p)     PLATFORM_GET_INC(ptr,    p, PointerType)
#define GET_SAVE_TYPE_INC(type,p)       PLATFORM_GET_INC(type, p, SaveType)
#define GET_UPDATE_VAR_TYPE_INC(type,p) PLATFORM_GET_INC(type, p, UpdateVarType)
#define GET_MODE_INC(mode,p)            PLATFORM_GET_INC(mode, p, ModeType)

/* code point's address must be aligned address. */
#define GET_CODE_POINT(code,p)   code = *((OnigCodePoint* )(p))
#define GET_BYTE_INC(byte,p) do{\
  byte = *(p);\
  (p)++;\
} while(0)


/* op-code + arg size */
#define SIZE_OP_ANYCHAR_STAR            SIZE_OPCODE
#define SIZE_OP_ANYCHAR_STAR_PEEK_NEXT (SIZE_OPCODE + 1)
#define SIZE_OP_JUMP                   (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_PUSH                   (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_PUSH_SUPER             (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_POP_OUT                 SIZE_OPCODE
#define SIZE_OP_PUSH_OR_JUMP_EXACT1    (SIZE_OPCODE + SIZE_RELADDR + 1)
#define SIZE_OP_PUSH_IF_PEEK_NEXT      (SIZE_OPCODE + SIZE_RELADDR + 1)
#define SIZE_OP_REPEAT_INC             (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_REPEAT_INC_NG          (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_WORD_BOUNDARY          (SIZE_OPCODE + SIZE_MODE)
#define SIZE_OP_PREC_READ_START         SIZE_OPCODE
#define SIZE_OP_PREC_READ_NOT_START    (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_PREC_READ_END           SIZE_OPCODE
#define SIZE_OP_PREC_READ_NOT_END       SIZE_OPCODE
#define SIZE_OP_FAIL                    SIZE_OPCODE
#define SIZE_OP_MEMORY_START           (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_START_PUSH      (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_PUSH        (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_PUSH_REC    (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END             (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_REC         (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_ATOMIC_START            SIZE_OPCODE
#define SIZE_OP_ATOMIC_END              SIZE_OPCODE
#define SIZE_OP_EMPTY_CHECK_START       (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_EMPTY_CHECK_END         (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_LOOK_BEHIND            (SIZE_OPCODE + SIZE_LENGTH)
#define SIZE_OP_LOOK_BEHIND_NOT_START  (SIZE_OPCODE + SIZE_RELADDR + SIZE_LENGTH)
#define SIZE_OP_LOOK_BEHIND_NOT_END     SIZE_OPCODE
#define SIZE_OP_CALL                   (SIZE_OPCODE + SIZE_ABSADDR)
#define SIZE_OP_RETURN                  SIZE_OPCODE
#define SIZE_OP_PUSH_SAVE_VAL          (SIZE_OPCODE + SIZE_SAVE_TYPE + SIZE_MEMNUM)
#define SIZE_OP_UPDATE_VAR             (SIZE_OPCODE + SIZE_UPDATE_VAR_TYPE + SIZE_MEMNUM)

#ifdef USE_CALLOUT
#define SIZE_OP_CALLOUT_CONTENTS       (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_CALLOUT_NAME           (SIZE_OPCODE + SIZE_MEMNUM + SIZE_MEMNUM)
#endif

#define MC_ESC(syn)               (syn)->meta_char_table.esc
#define MC_ANYCHAR(syn)           (syn)->meta_char_table.anychar
#define MC_ANYTIME(syn)           (syn)->meta_char_table.anytime
#define MC_ZERO_OR_ONE_TIME(syn)  (syn)->meta_char_table.zero_or_one_time
#define MC_ONE_OR_MORE_TIME(syn)  (syn)->meta_char_table.one_or_more_time
#define MC_ANYCHAR_ANYTIME(syn)   (syn)->meta_char_table.anychar_anytime

#define IS_MC_ESC_CODE(code, syn) \
  ((code) == MC_ESC(syn) && \
   !IS_SYNTAX_OP2((syn), ONIG_SYN_OP2_INEFFECTIVE_ESCAPE))


#define SYN_POSIX_COMMON_OP \
 ( ONIG_SYN_OP_DOT_ANYCHAR | ONIG_SYN_OP_POSIX_BRACKET | \
   ONIG_SYN_OP_DECIMAL_BACKREF | \
   ONIG_SYN_OP_BRACKET_CC | ONIG_SYN_OP_ASTERISK_ZERO_INF | \
   ONIG_SYN_OP_LINE_ANCHOR | \
   ONIG_SYN_OP_ESC_CONTROL_CHARS )

#define SYN_GNU_REGEX_OP \
  ( ONIG_SYN_OP_DOT_ANYCHAR | ONIG_SYN_OP_BRACKET_CC | \
    ONIG_SYN_OP_POSIX_BRACKET | ONIG_SYN_OP_DECIMAL_BACKREF | \
    ONIG_SYN_OP_BRACE_INTERVAL | ONIG_SYN_OP_LPAREN_SUBEXP | \
    ONIG_SYN_OP_VBAR_ALT | \
    ONIG_SYN_OP_ASTERISK_ZERO_INF | ONIG_SYN_OP_PLUS_ONE_INF | \
    ONIG_SYN_OP_QMARK_ZERO_ONE | \
    ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR | ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR | \
    ONIG_SYN_OP_ESC_W_WORD | \
    ONIG_SYN_OP_ESC_B_WORD_BOUND | ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END | \
    ONIG_SYN_OP_ESC_S_WHITE_SPACE | ONIG_SYN_OP_ESC_D_DIGIT | \
    ONIG_SYN_OP_LINE_ANCHOR )

#define SYN_GNU_REGEX_BV \
  ( ONIG_SYN_CONTEXT_INDEP_ANCHORS | ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS | \
    ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS | ONIG_SYN_ALLOW_INVALID_INTERVAL | \
    ONIG_SYN_BACKSLASH_ESCAPE_IN_CC | ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC )


#define NCCLASS_FLAGS(cc)           ((cc)->flags)
#define NCCLASS_FLAG_SET(cc,flag)    (NCCLASS_FLAGS(cc) |= (flag))
#define NCCLASS_FLAG_CLEAR(cc,flag)  (NCCLASS_FLAGS(cc) &= ~(flag))
#define IS_NCCLASS_FLAG_ON(cc,flag) ((NCCLASS_FLAGS(cc) & (flag)) != 0)

/* cclass node */
#define FLAG_NCCLASS_NOT           (1<<0)
#define FLAG_NCCLASS_SHARE         (1<<1)

#define NCCLASS_SET_NOT(nd)     NCCLASS_FLAG_SET(nd, FLAG_NCCLASS_NOT)
#define NCCLASS_CLEAR_NOT(nd)   NCCLASS_FLAG_CLEAR(nd, FLAG_NCCLASS_NOT)
#define IS_NCCLASS_NOT(nd)      IS_NCCLASS_FLAG_ON(nd, FLAG_NCCLASS_NOT)

extern void onig_add_end_call(void (*func)(void));


#ifdef ONIG_DEBUG

#ifdef ONIG_DEBUG_COMPILE
extern void onig_print_compiled_byte_code_list(FILE* f, regex_t* reg);
#endif

#ifdef ONIG_DEBUG_STATISTICS
extern void onig_statistics_init P_((void));
extern int  onig_print_statistics P_((FILE* f));
#endif
#endif

extern void   onig_warning(const char* s);
extern UChar* onig_error_code_to_format P_((int code));
extern void   onig_snprintf_with_pattern PV_((UChar buf[], int bufsize, OnigEncoding enc, UChar* pat, UChar* pat_end, const UChar *fmt, ...));
extern int    onig_bbuf_init P_((BBuf* buf, int size));
extern int    onig_compile P_((regex_t* reg, const UChar* pattern, const UChar* pattern_end, OnigErrorInfo* einfo));
extern void   onig_transfer P_((regex_t* to, regex_t* from));
extern int    onig_is_code_in_cc_len P_((int enclen, OnigCodePoint code, void* /* CClassNode* */ cc));
extern RegexExt* onig_get_regex_ext(regex_t* reg);
extern int    onig_ext_set_pattern(regex_t* reg, const UChar* pattern, const UChar* pattern_end);

#ifdef USE_CALLOUT

extern OnigCalloutType onig_get_callout_type_by_name_id(int name_id);
extern OnigCalloutFunc onig_get_callout_start_func_by_name_id(int id);
extern OnigCalloutFunc onig_get_callout_end_func_by_name_id(int id);
extern int             onig_callout_tag_table_free(void* table);
extern void            onig_free_reg_callout_list(int n, CalloutListEntry* list);
extern CalloutListEntry* onig_reg_callout_list_at(regex_t* reg, int num);
extern OnigCalloutFunc onig_get_callout_start_func(regex_t* reg, int callout_num);

/* for definition of builtin callout */
#define BC0_P(name, func)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                              (UChar* )(name), (UChar* )((name) + len),\
                              ONIG_CALLOUT_IN_PROGRESS,\
                              onig_builtin_ ## func, 0, 0, 0, 0, 0);\
  if (id < 0) return id;\
} while(0)

#define BC0_R(name, func)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                              (UChar* )(name), (UChar* )((name) + len),\
                              ONIG_CALLOUT_IN_RETRACTION,\
                              onig_builtin_ ## func, 0, 0, 0, 0, 0);\
  if (id < 0) return id;\
} while(0)

#define BC0_B(name, func)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                              (UChar* )(name), (UChar* )((name) + len),\
                              ONIG_CALLOUT_IN_BOTH,\
                              onig_builtin_ ## func, 0, 0, 0, 0, 0);\
  if (id < 0) return id;\
} while(0)

#define BC_P(name, func, na, ts)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                              (UChar* )(name), (UChar* )((name) + len),\
                              ONIG_CALLOUT_IN_PROGRESS,\
                                onig_builtin_ ## func, 0, (na), (ts), 0, 0); \
  if (id < 0) return id;\
} while(0)

#define BC_P_O(name, func, nts, ts, nopts, opts)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                           (UChar* )(name), (UChar* )((name) + len),\
                           ONIG_CALLOUT_IN_PROGRESS,\
                           onig_builtin_ ## func, 0, (nts), (ts), (nopts), (opts));\
  if (id < 0) return id;\
} while(0)

#define BC_B(name, func, na, ts)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                              (UChar* )(name), (UChar* )((name) + len),\
                              ONIG_CALLOUT_IN_BOTH,\
                              onig_builtin_ ## func, 0, (na), (ts), 0, 0);\
  if (id < 0) return id;\
} while(0)

#define BC_B_O(name, func, nts, ts, nopts, opts)  do {\
  int len = onigenc_str_bytelen_null(enc, (UChar* )name);\
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,\
                           (UChar* )(name), (UChar* )((name) + len),\
                           ONIG_CALLOUT_IN_BOTH,\
                           onig_builtin_ ## func, 0, (nts), (ts), (nopts), (opts));\
  if (id < 0) return id;\
} while(0)

#endif /* USE_CALLOUT */


/* strend hash */
typedef void hash_table_type;

#ifdef _WIN32
# include <windows.h>
typedef ULONG_PTR hash_data_type;
#else
typedef unsigned long hash_data_type;
#endif

extern hash_table_type* onig_st_init_strend_table_with_size P_((int size));
extern int onig_st_lookup_strend P_((hash_table_type* table, const UChar* str_key, const UChar* end_key, hash_data_type *value));
extern int onig_st_insert_strend P_((hash_table_type* table, const UChar* str_key, const UChar* end_key, hash_data_type value));

typedef int (*ONIGENC_INIT_PROPERTY_LIST_FUNC_TYPE)(void);

#endif /* REGINT_H */
