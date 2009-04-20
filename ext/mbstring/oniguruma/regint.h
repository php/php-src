#ifndef REGINT_H
#define REGINT_H
/**********************************************************************
  regint.h -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2007  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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
/* #define ONIG_DEBUG_PARSE_TREE */
/* #define ONIG_DEBUG_COMPILE */
/* #define ONIG_DEBUG_SEARCH */
/* #define ONIG_DEBUG_MATCH */
/* #define ONIG_DONT_OPTIMIZE */

/* for byte-code statistical data. */
/* #define ONIG_DEBUG_STATISTICS */

#if defined(ONIG_DEBUG_PARSE_TREE) || defined(ONIG_DEBUG_MATCH) || \
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
/* #define USE_UNICODE_FULL_RANGE_CTYPE */ /* --> move to regenc.h */
#define USE_NAMED_GROUP
#define USE_SUBEXP_CALL
#define USE_INFINITE_REPEAT_MONOMANIAC_MEM_STATUS_CHECK /* /(?:()|())*\2/ */
#define USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE     /* /\n$/ =~ "\n" */
#define USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR
/* #define USE_RECOMPILE_API */
/* treat \r\n as line terminator.
   !!! NO SUPPORT !!!
   use this configuration on your own responsibility */
/* #define USE_CRNL_AS_LINE_TERMINATOR */

/* internal config */
#define USE_RECYCLE_NODE
#define USE_OP_PUSH_OR_JUMP_EXACT
#define USE_QUANTIFIER_PEEK_NEXT
#define USE_ST_HASH_TABLE
#define USE_SHARED_CCLASS_TABLE

#define INIT_MATCH_STACK_SIZE                     160
#define DEFAULT_MATCH_STACK_LIMIT_SIZE              0 /* unlimited */

/* interface to external system */
#ifdef NOT_RUBY      /* given from Makefile */
#include "config.h"
#define USE_BACKREF_AT_LEVEL
#define USE_CAPTURE_HISTORY
#define USE_VARIABLE_META_CHARS
#define USE_WORD_BEGIN_END          /* "\<": word-begin, "\>": word-end */
#define USE_POSIX_REGION_OPTION     /* needed for POSIX API support */
#define USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
/* #define USE_COMBINATION_EXPLOSION_CHECK */     /* (X*)* */
/* #define USE_MULTI_THREAD_SYSTEM */
#define THREAD_SYSTEM_INIT          /* depend on thread system */
#define THREAD_SYSTEM_END           /* depend on thread system */
#define THREAD_ATOMIC_START         /* depend on thread system */
#define THREAD_ATOMIC_END           /* depend on thread system */
#define THREAD_PASS                 /* depend on thread system */
#define xmalloc     malloc
#define xrealloc    realloc
#define xcalloc     calloc
#define xfree       free
#else
#include "ruby.h"
#include "rubysig.h"      /* for DEFER_INTS, ENABLE_INTS */

#define USE_COMBINATION_EXPLOSION_CHECK        /* (X*)* */
#define USE_MULTI_THREAD_SYSTEM
#define THREAD_SYSTEM_INIT
#define THREAD_SYSTEM_END
#define THREAD_ATOMIC_START          DEFER_INTS
#define THREAD_ATOMIC_END            ENABLE_INTS
#define THREAD_PASS                  rb_thread_schedule()

#define DEFAULT_WARN_FUNCTION        onig_rb_warn
#define DEFAULT_VERB_WARN_FUNCTION   onig_rb_warning

#endif /* else NOT_RUBY */

#define STATE_CHECK_STRING_THRESHOLD_LEN             7
#define STATE_CHECK_BUFF_MAX_SIZE               0x4000

#define THREAD_PASS_LIMIT_COUNT     8
#define xmemset     memset
#define xmemcpy     memcpy
#define xmemmove    memmove
#if defined(_WIN32) && !defined(__GNUC__)
#define xalloca     _alloca
#if _MSC_VER < 1500
#ifndef vsnprintf
#define vsnprintf   _vsnprintf
#endif
#endif
#else
#define xalloca     alloca
#endif

#if defined(USE_RECOMPILE_API) && defined(USE_MULTI_THREAD_SYSTEM)
#define ONIG_STATE_INC(reg) (reg)->state++
#define ONIG_STATE_DEC(reg) (reg)->state--

#define ONIG_STATE_INC_THREAD(reg) do {\
  THREAD_ATOMIC_START;\
  (reg)->state++;\
  THREAD_ATOMIC_END;\
} while(0)
#define ONIG_STATE_DEC_THREAD(reg) do {\
  THREAD_ATOMIC_START;\
  (reg)->state--;\
  THREAD_ATOMIC_END;\
} while(0)
#else
#define ONIG_STATE_INC(reg)         /* Nothing */
#define ONIG_STATE_DEC(reg)         /* Nothing */
#define ONIG_STATE_INC_THREAD(reg)  /* Nothing */
#define ONIG_STATE_DEC_THREAD(reg)  /* Nothing */
#endif /* USE_RECOMPILE_API && USE_MULTI_THREAD_SYSTEM */


#define onig_st_is_member              st_is_member

#ifdef NOT_RUBY

#define st_init_table                  onig_st_init_table
#define st_init_table_with_size        onig_st_init_table_with_size
#define st_init_numtable               onig_st_init_numtable
#define st_init_numtable_with_size     onig_st_init_numtable_with_size
#define st_init_strtable               onig_st_init_strtable
#define st_init_strtable_with_size     onig_st_init_strtable_with_size
#define st_init_strend_table_with_size onig_st_init_strend_table_with_size
#define st_delete                      onig_st_delete
#define st_delete_safe                 onig_st_delete_safe
#define st_insert                      onig_st_insert
#define st_insert_strend               onig_st_insert_strend
#define st_lookup                      onig_st_lookup
#define st_lookup_strend               onig_st_lookup_strend
#define st_foreach                     onig_st_foreach
#define st_add_direct                  onig_st_add_direct
#define st_add_direct_strend           onig_st_add_direct_strend
#define st_free_table                  onig_st_free_table
#define st_cleanup_safe                onig_st_cleanup_safe
#define st_copy                        onig_st_copy
#define st_nothing_key_clone           onig_st_nothing_key_clone
#define st_nothing_key_free            onig_st_nothing_key_free

#else /* NOT_RUBY */

#define onig_st_init_table                  st_init_table
#define onig_st_init_table_with_size        st_init_table_with_size
#define onig_st_init_numtable               st_init_numtable
#define onig_st_init_numtable_with_size     st_init_numtable_with_size
#define onig_st_init_strtable               st_init_strtable
#define onig_st_init_strtable_with_size     st_init_strtable_with_size
#define onig_st_init_strend_table_with_size st_init_strend_table_with_size
#define onig_st_delete                      st_delete
#define onig_st_delete_safe                 st_delete_safe
#define onig_st_insert                      st_insert
#define onig_st_insert_strend               st_insert_strend
#define onig_st_lookup                      st_lookup
#define onig_st_lookup_strend               st_lookup_strend
#define onig_st_foreach                     st_foreach
#define onig_st_add_direct                  st_add_direct
#define onig_st_add_direct_strend           st_add_direct_strend
#define onig_st_free_table                  st_free_table
#define onig_st_cleanup_safe                st_cleanup_safe
#define onig_st_copy                        st_copy
#define onig_st_nothing_key_clone           st_nothing_key_clone
#define onig_st_nothing_key_free            st_nothing_key_free

#endif /* NOT_RUBY */


#ifdef HAVE_STDLIB_H
#include <stdlib.h>
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

#ifdef __BORLANDC__
#include <malloc.h>
#endif

#ifdef ONIG_DEBUG
# include <stdio.h>
#endif

#include "regenc.h"
#include "oniguruma.h"

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
#define CHECK_NULL_RETURN_VAL(p,val)  if (IS_NULL(p)) return (val)
#define NULL_UCHARP                   ((UChar* )0)

#ifndef PLATFORM_UNALIGNED_WORD_ACCESS
/* sizeof(OnigCodePoint) */
#define WORD_ALIGNMENT_SIZE     SIZEOF_LONG

#define GET_ALIGNMENT_PAD_SIZE(addr,pad_size) do {\
  (pad_size) = WORD_ALIGNMENT_SIZE \
               - ((unsigned int )(addr) % WORD_ALIGNMENT_SIZE);\
  if ((pad_size) == WORD_ALIGNMENT_SIZE) (pad_size) = 0;\
} while (0)

#define ALIGNMENT_RIGHT(addr) do {\
  (addr) += (WORD_ALIGNMENT_SIZE - 1);\
  (addr) -= ((unsigned int )(addr) % WORD_ALIGNMENT_SIZE);\
} while (0)


#define B_SHIFT  8
#define B_MASK   0xff

#define SERIALIZE_2BYTE_INT(i,p) do {\
  *(p)     = ((i) >> B_SHIFT) & B_MASK;\
  *((p)+1) = (i) & B_MASK;\
} while (0)

#define SERIALIZE_4BYTE_INT(i,p) do {\
  *(p)     = ((i) >> B_SHIFT*3) & B_MASK;\
  *((p)+1) = ((i) >> B_SHIFT*2) & B_MASK;\
  *((p)+2) = ((i) >> B_SHIFT  ) & B_MASK;\
  *((p)+3) = (i) & B_MASK;\
} while (0)

#define SERIALIZE_8BYTE_INT(i,p) do {\
  *(p)     = ((i) >> B_SHIFT*7) & B_MASK;\
  *((p)+1) = ((i) >> B_SHIFT*6) & B_MASK;\
  *((p)+2) = ((i) >> B_SHIFT*5) & B_MASK;\
  *((p)+3) = ((i) >> B_SHIFT*4) & B_MASK;\
  *((p)+4) = ((i) >> B_SHIFT*3) & B_MASK;\
  *((p)+5) = ((i) >> B_SHIFT*2) & B_MASK;\
  *((p)+6) = ((i) >> B_SHIFT  ) & B_MASK;\
  *((p)+7) = (i) & B_MASK;\
} while (0)

#define GET_2BYTE_INT_INC(type,i,p) do {\
  (i) = (type )(((unsigned int )(*(p)) << B_SHIFT) | (unsigned int )((p)[1]));\
  (p) += 2;\
} while (0)

#define GET_4BYTE_INT_INC(type,i,p) do {\
  (i) = (type )(((unsigned int )((p)[0]) << B_SHIFT*3) | \
		((unsigned int )((p)[1]) << B_SHIFT*2) | \
		((unsigned int )((p)[2]) << B_SHIFT  ) | \
		((unsigned int )((p)[3])             )); \
  (p) += 4;\
} while (0)

#define GET_8BYTE_INT_INC(type,i,p) do {\
  (i) = (type )(((unsigned long )((p)[0]) << B_SHIFT*7) | \
		((unsigned long )((p)[1]) << B_SHIFT*6) | \
		((unsigned long )((p)[2]) << B_SHIFT*5) | \
		((unsigned long )((p)[3]) << B_SHIFT*4) | \
		((unsigned long )((p)[4]) << B_SHIFT*3) | \
		((unsigned long )((p)[5]) << B_SHIFT*2) | \
		((unsigned long )((p)[6]) << B_SHIFT  ) | \
		((unsigned long )((p)[7])             )); \
  (p) += 8;\
} while (0)

#if SIZEOF_SHORT == 2
#define GET_SHORT_INC(i,p)     GET_2BYTE_INT_INC(short,i,p)
#define SERIALIZE_SHORT(i,p)   SERIALIZE_2BYTE_INT(i,p)
#elif SIZEOF_SHORT == 4
#define GET_SHORT_INC(i,p)     GET_4BYTE_INT_INC(short,i,p)
#define SERIALIZE_SHORT(i,p)   SERIALIZE_4BYTE_INT(i,p)
#elif SIZEOF_SHORT == 8
#define GET_SHORT_INC(i,p)     GET_8BYTE_INT_INC(short,i,p)
#define SERIALIZE_SHORT(i,p)   SERIALIZE_8BYTE_INT(i,p)
#endif

#if SIZEOF_INT == 2
#define GET_INT_INC(i,p)       GET_2BYTE_INT_INC(int,i,p)
#define GET_UINT_INC(i,p)      GET_2BYTE_INT_INC(unsigned,i,p)
#define SERIALIZE_INT(i,p)     SERIALIZE_2BYTE_INT(i,p)
#define SERIALIZE_UINT(i,p)    SERIALIZE_2BYTE_INT(i,p)
#elif SIZEOF_INT == 4
#define GET_INT_INC(i,p)       GET_4BYTE_INT_INC(int,i,p)
#define GET_UINT_INC(i,p)      GET_4BYTE_INT_INC(unsigned,i,p)
#define SERIALIZE_INT(i,p)     SERIALIZE_4BYTE_INT(i,p)
#define SERIALIZE_UINT(i,p)    SERIALIZE_4BYTE_INT(i,p)
#elif SIZEOF_INT == 8
#define GET_INT_INC(i,p)       GET_8BYTE_INT_INC(int,i,p)
#define GET_UINT_INC(i,p)      GET_8BYTE_INT_INC(unsigned,i,p)
#define SERIALIZE_INT(i,p)     SERIALIZE_8BYTE_INT(i,p)
#define SERIALIZE_UINT(i,p)    SERIALIZE_8BYTE_INT(i,p)
#endif

#endif /* PLATFORM_UNALIGNED_WORD_ACCESS */

/* stack pop level */
#define STACK_POP_LEVEL_FREE        0
#define STACK_POP_LEVEL_MEM_START   1
#define STACK_POP_LEVEL_ALL         2

/* optimize flags */
#define ONIG_OPTIMIZE_NONE              0
#define ONIG_OPTIMIZE_EXACT             1   /* Slow Search */
#define ONIG_OPTIMIZE_EXACT_BM          2   /* Boyer Moore Search */
#define ONIG_OPTIMIZE_EXACT_BM_NOT_REV  3   /* BM   (but not simple match) */
#define ONIG_OPTIMIZE_EXACT_IC          4   /* Slow Search (ignore case) */
#define ONIG_OPTIMIZE_MAP               5   /* char map */

/* bit status */
typedef unsigned int  BitStatusType;

#define BIT_STATUS_BITS_NUM          (sizeof(BitStatusType) * 8)
#define BIT_STATUS_CLEAR(stats)      (stats) = 0
#define BIT_STATUS_ON_ALL(stats)     (stats) = ~((BitStatusType )0)
#define BIT_STATUS_AT(stats,n) \
  ((n) < BIT_STATUS_BITS_NUM  ?  ((stats) & (1 << n)) : ((stats) & 1))

#define BIT_STATUS_ON_AT(stats,n) do {\
  if ((n) < BIT_STATUS_BITS_NUM)\
    (stats) |= (1 << (n));\
  else\
    (stats) |= 1;\
} while (0)

#define BIT_STATUS_ON_AT_SIMPLE(stats,n) do {\
  if ((n) < BIT_STATUS_BITS_NUM)\
    (stats) |= (1 << (n));\
} while (0)


#define INT_MAX_LIMIT           ((1UL << (SIZEOF_INT * 8 - 1)) - 1)

#define DIGITVAL(code)    ((code) - '0')
#define ODIGITVAL(code)   DIGITVAL(code)
#define XDIGITVAL(enc,code) \
  (ONIGENC_IS_CODE_DIGIT(enc,code) ? DIGITVAL(code) \
   : (ONIGENC_IS_CODE_UPPER(enc,code) ? (code) - 'A' + 10 : (code) - 'a' + 10))

#define IS_SINGLELINE(option)     ((option) & ONIG_OPTION_SINGLELINE)
#define IS_MULTILINE(option)      ((option) & ONIG_OPTION_MULTILINE)
#define IS_IGNORECASE(option)     ((option) & ONIG_OPTION_IGNORECASE)
#define IS_EXTEND(option)         ((option) & ONIG_OPTION_EXTEND)
#define IS_FIND_LONGEST(option)   ((option) & ONIG_OPTION_FIND_LONGEST)
#define IS_FIND_NOT_EMPTY(option) ((option) & ONIG_OPTION_FIND_NOT_EMPTY)
#define IS_POSIXLINE(option)      (IS_SINGLELINE(option) && IS_MULTILINE(option))
#define IS_FIND_CONDITION(option) ((option) & \
          (ONIG_OPTION_FIND_LONGEST | ONIG_OPTION_FIND_NOT_EMPTY))
#define IS_NOTBOL(option)         ((option) & ONIG_OPTION_NOTBOL)
#define IS_NOTEOL(option)         ((option) & ONIG_OPTION_NOTEOL)
#define IS_POSIX_REGION(option)   ((option) & ONIG_OPTION_POSIX_REGION)

/* OP_SET_OPTION is required for these options.
#define IS_DYNAMIC_OPTION(option) \
  (((option) & (ONIG_OPTION_MULTILINE | ONIG_OPTION_IGNORECASE)) != 0)
*/
/* ignore-case and multibyte status are included in compiled code. */
#define IS_DYNAMIC_OPTION(option)  0

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
  for (i = 0; i < BITSET_SIZE; i++) { (bs)[i] = 0; }\
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

#define BBUF_INIT(buf,size)    onig_bbuf_init((BBuf* )(buf), (size))

#define BBUF_SIZE_INC(buf,inc) do{\
  (buf)->alloc += (inc);\
  (buf)->p = (UChar* )xrealloc((buf)->p, (buf)->alloc);\
  if (IS_NULL((buf)->p)) return(ONIGERR_MEMORY);\
} while (0)

#define BBUF_EXPAND(buf,low) do{\
  do { (buf)->alloc *= 2; } while ((buf)->alloc < (unsigned int )low);\
  (buf)->p = (UChar* )xrealloc((buf)->p, (buf)->alloc);\
  if (IS_NULL((buf)->p)) return(ONIGERR_MEMORY);\
} while (0)

#define BBUF_ENSURE_SIZE(buf,size) do{\
  unsigned int new_alloc = (buf)->alloc;\
  while (new_alloc < (unsigned int )(size)) { new_alloc *= 2; }\
  if ((buf)->alloc != new_alloc) {\
    (buf)->p = (UChar* )xrealloc((buf)->p, new_alloc);\
    if (IS_NULL((buf)->p)) return(ONIGERR_MEMORY);\
    (buf)->alloc = new_alloc;\
  }\
} while (0)

#define BBUF_WRITE(buf,pos,bytes,n) do{\
  int used = (pos) + (n);\
  if ((buf)->alloc < (unsigned int )used) BBUF_EXPAND((buf),used);\
  xmemcpy((buf)->p + (pos), (bytes), (n));\
  if ((buf)->used < (unsigned int )used) (buf)->used = used;\
} while (0)

#define BBUF_WRITE1(buf,pos,byte) do{\
  int used = (pos) + 1;\
  if ((buf)->alloc < (unsigned int )used) BBUF_EXPAND((buf),used);\
  (buf)->p[(pos)] = (byte);\
  if ((buf)->used < (unsigned int )used) (buf)->used = used;\
} while (0)

#define BBUF_ADD(buf,bytes,n)       BBUF_WRITE((buf),(buf)->used,(bytes),(n))
#define BBUF_ADD1(buf,byte)         BBUF_WRITE1((buf),(buf)->used,(byte))
#define BBUF_GET_ADD_ADDRESS(buf)   ((buf)->p + (buf)->used)
#define BBUF_GET_OFFSET_POS(buf)    ((buf)->used)

/* from < to */
#define BBUF_MOVE_RIGHT(buf,from,to,n) do {\
  if ((unsigned int )((to)+(n)) > (buf)->alloc) BBUF_EXPAND((buf),(to) + (n));\
  xmemmove((buf)->p + (to), (buf)->p + (from), (n));\
  if ((unsigned int )((to)+(n)) > (buf)->used) (buf)->used = (to) + (n);\
} while (0)

/* from > to */
#define BBUF_MOVE_LEFT(buf,from,to,n) do {\
  xmemmove((buf)->p + (to), (buf)->p + (from), (n));\
} while (0)

/* from > to */
#define BBUF_MOVE_LEFT_REDUCE(buf,from,to) do {\
  xmemmove((buf)->p + (to), (buf)->p + (from), (buf)->used - (from));\
  (buf)->used -= (from - to);\
} while (0)

#define BBUF_INSERT(buf,pos,bytes,n) do {\
  if (pos >= (buf)->used) {\
    BBUF_WRITE(buf,pos,bytes,n);\
  }\
  else {\
    BBUF_MOVE_RIGHT((buf),(pos),(pos) + (n),((buf)->used - (pos)));\
    xmemcpy((buf)->p + (pos), (bytes), (n));\
  }\
} while (0)

#define BBUF_GET_BYTE(buf, pos) (buf)->p[(pos)]


#define ANCHOR_BEGIN_BUF        (1<<0)
#define ANCHOR_BEGIN_LINE       (1<<1)
#define ANCHOR_BEGIN_POSITION   (1<<2)
#define ANCHOR_END_BUF          (1<<3)
#define ANCHOR_SEMI_END_BUF     (1<<4)
#define ANCHOR_END_LINE         (1<<5)

#define ANCHOR_WORD_BOUND       (1<<6)
#define ANCHOR_NOT_WORD_BOUND   (1<<7)
#define ANCHOR_WORD_BEGIN       (1<<8)
#define ANCHOR_WORD_END         (1<<9)
#define ANCHOR_PREC_READ        (1<<10)
#define ANCHOR_PREC_READ_NOT    (1<<11)
#define ANCHOR_LOOK_BEHIND      (1<<12)
#define ANCHOR_LOOK_BEHIND_NOT  (1<<13)

#define ANCHOR_ANYCHAR_STAR     (1<<14)   /* ".*" optimize info */
#define ANCHOR_ANYCHAR_STAR_ML  (1<<15)   /* ".*" optimize info (multi-line) */

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
  OP_CCLASS_NODE,       /* pointer to CClassNode node */

  OP_ANYCHAR,                 /* "."  */
  OP_ANYCHAR_ML,              /* "."  multi-line */
  OP_ANYCHAR_STAR,            /* ".*" */
  OP_ANYCHAR_ML_STAR,         /* ".*" multi-line */
  OP_ANYCHAR_STAR_PEEK_NEXT,
  OP_ANYCHAR_ML_STAR_PEEK_NEXT,

  OP_WORD,
  OP_NOT_WORD,
  OP_WORD_BOUND,
  OP_NOT_WORD_BOUND,
  OP_WORD_BEGIN,
  OP_WORD_END,

  OP_BEGIN_BUF,
  OP_END_BUF,
  OP_BEGIN_LINE,
  OP_END_LINE,
  OP_SEMI_END_BUF,
  OP_BEGIN_POSITION,

  OP_BACKREF1,
  OP_BACKREF2,
  OP_BACKREFN,
  OP_BACKREFN_IC,
  OP_BACKREF_MULTI,
  OP_BACKREF_MULTI_IC,
  OP_BACKREF_AT_LEVEL,    /* \k<xxx+n>, \k<xxx-n> */

  OP_MEMORY_START,
  OP_MEMORY_START_PUSH,   /* push back-tracker to stack */
  OP_MEMORY_END_PUSH,     /* push back-tracker to stack */
  OP_MEMORY_END_PUSH_REC, /* push back-tracker to stack */
  OP_MEMORY_END,
  OP_MEMORY_END_REC,      /* push marker to stack */

  OP_SET_OPTION_PUSH,    /* set option and push recover option */
  OP_SET_OPTION,         /* set option */

  OP_FAIL,               /* pop stack and move */
  OP_JUMP,
  OP_PUSH,
  OP_POP,
  OP_PUSH_OR_JUMP_EXACT1,  /* if match exact then push, else jump. */
  OP_PUSH_IF_PEEK_NEXT,    /* if match exact then push, else none. */
  OP_REPEAT,               /* {n,m} */
  OP_REPEAT_NG,            /* {n,m}? (non greedy) */
  OP_REPEAT_INC,
  OP_REPEAT_INC_NG,        /* non greedy */
  OP_REPEAT_INC_SG,        /* search and get in stack */
  OP_REPEAT_INC_NG_SG,     /* search and get in stack (non greedy) */
  OP_NULL_CHECK_START,     /* null loop checker start */
  OP_NULL_CHECK_END,       /* null loop checker end   */
  OP_NULL_CHECK_END_MEMST, /* null loop checker end (with capture status) */
  OP_NULL_CHECK_END_MEMST_PUSH, /* with capture status and push check-end */

  OP_PUSH_POS,             /* (?=...)  start */
  OP_POP_POS,              /* (?=...)  end   */
  OP_PUSH_POS_NOT,         /* (?!...)  start */
  OP_FAIL_POS,             /* (?!...)  end   */
  OP_PUSH_STOP_BT,         /* (?>...)  start */
  OP_POP_STOP_BT,          /* (?>...)  end   */
  OP_LOOK_BEHIND,          /* (?<=...) start (no needs end opcode) */
  OP_PUSH_LOOK_BEHIND_NOT, /* (?<!...) start */
  OP_FAIL_LOOK_BEHIND_NOT, /* (?<!...) end   */

  OP_CALL,                 /* \g<name> */
  OP_RETURN,

  OP_STATE_CHECK_PUSH,         /* combination explosion check and push */
  OP_STATE_CHECK_PUSH_OR_JUMP, /* check ok -> push, else jump  */
  OP_STATE_CHECK,              /* check only */
  OP_STATE_CHECK_ANYCHAR_STAR,
  OP_STATE_CHECK_ANYCHAR_ML_STAR
};

typedef int RelAddrType;
typedef int AbsAddrType;
typedef int LengthType;
typedef int RepeatNumType;
typedef short int MemNumType;
typedef short int StateCheckNumType;
typedef void* PointerType;

#define SIZE_OPCODE           1
#define SIZE_RELADDR          sizeof(RelAddrType)
#define SIZE_ABSADDR          sizeof(AbsAddrType)
#define SIZE_LENGTH           sizeof(LengthType)
#define SIZE_MEMNUM           sizeof(MemNumType)
#define SIZE_STATE_CHECK_NUM  sizeof(StateCheckNumType)
#define SIZE_REPEATNUM        sizeof(RepeatNumType)
#define SIZE_OPTION           sizeof(OnigOptionType)
#define SIZE_CODE_POINT       sizeof(OnigCodePoint)
#define SIZE_POINTER          sizeof(PointerType)


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

#endif  /* PLATFORM_UNALIGNED_WORD_ACCESS */

#define GET_RELADDR_INC(addr,p)    PLATFORM_GET_INC(addr,   p, RelAddrType)
#define GET_ABSADDR_INC(addr,p)    PLATFORM_GET_INC(addr,   p, AbsAddrType)
#define GET_LENGTH_INC(len,p)      PLATFORM_GET_INC(len,    p, LengthType)
#define GET_MEMNUM_INC(num,p)      PLATFORM_GET_INC(num,    p, MemNumType)
#define GET_REPEATNUM_INC(num,p)   PLATFORM_GET_INC(num,    p, RepeatNumType)
#define GET_OPTION_INC(option,p)   PLATFORM_GET_INC(option, p, OnigOptionType)
#define GET_POINTER_INC(ptr,p)     PLATFORM_GET_INC(ptr,    p, PointerType)
#define GET_STATE_CHECK_NUM_INC(num,p)  PLATFORM_GET_INC(num, p, StateCheckNumType)

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
#define SIZE_OP_POP                     SIZE_OPCODE
#define SIZE_OP_PUSH_OR_JUMP_EXACT1    (SIZE_OPCODE + SIZE_RELADDR + 1)
#define SIZE_OP_PUSH_IF_PEEK_NEXT      (SIZE_OPCODE + SIZE_RELADDR + 1)
#define SIZE_OP_REPEAT_INC             (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_REPEAT_INC_NG          (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_PUSH_POS                SIZE_OPCODE
#define SIZE_OP_PUSH_POS_NOT           (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_POP_POS                 SIZE_OPCODE
#define SIZE_OP_FAIL_POS                SIZE_OPCODE
#define SIZE_OP_SET_OPTION             (SIZE_OPCODE + SIZE_OPTION)
#define SIZE_OP_SET_OPTION_PUSH        (SIZE_OPCODE + SIZE_OPTION)
#define SIZE_OP_FAIL                    SIZE_OPCODE
#define SIZE_OP_MEMORY_START           (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_START_PUSH      (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_PUSH        (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_PUSH_REC    (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END             (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_REC         (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_PUSH_STOP_BT            SIZE_OPCODE
#define SIZE_OP_POP_STOP_BT             SIZE_OPCODE
#define SIZE_OP_NULL_CHECK_START       (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_NULL_CHECK_END         (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_LOOK_BEHIND            (SIZE_OPCODE + SIZE_LENGTH)
#define SIZE_OP_PUSH_LOOK_BEHIND_NOT   (SIZE_OPCODE + SIZE_RELADDR + SIZE_LENGTH)
#define SIZE_OP_FAIL_LOOK_BEHIND_NOT    SIZE_OPCODE
#define SIZE_OP_CALL                   (SIZE_OPCODE + SIZE_ABSADDR)
#define SIZE_OP_RETURN                  SIZE_OPCODE

#ifdef USE_COMBINATION_EXPLOSION_CHECK
#define SIZE_OP_STATE_CHECK            (SIZE_OPCODE + SIZE_STATE_CHECK_NUM)
#define SIZE_OP_STATE_CHECK_PUSH       (SIZE_OPCODE + SIZE_STATE_CHECK_NUM + SIZE_RELADDR)
#define SIZE_OP_STATE_CHECK_PUSH_OR_JUMP (SIZE_OPCODE + SIZE_STATE_CHECK_NUM + SIZE_RELADDR)
#define SIZE_OP_STATE_CHECK_ANYCHAR_STAR (SIZE_OPCODE + SIZE_STATE_CHECK_NUM)
#endif

#define MC_ESC(enc)               (enc)->meta_char_table.esc
#define MC_ANYCHAR(enc)           (enc)->meta_char_table.anychar
#define MC_ANYTIME(enc)           (enc)->meta_char_table.anytime
#define MC_ZERO_OR_ONE_TIME(enc)  (enc)->meta_char_table.zero_or_one_time
#define MC_ONE_OR_MORE_TIME(enc)  (enc)->meta_char_table.one_or_more_time
#define MC_ANYCHAR_ANYTIME(enc)   (enc)->meta_char_table.anychar_anytime

#define IS_MC_ESC_CODE(code, enc, syn) \
  ((code) == MC_ESC(enc) && \
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

/* cclass node */
#define FLAG_CCLASS_NOT            1
#define FLAG_CCLASS_SHARE         (1<<1)

#define CCLASS_SET_NOT(cc)      (cc)->flags |= FLAG_CCLASS_NOT
#define CCLASS_CLEAR_NOT(cc)    (cc)->flags &= ~FLAG_CCLASS_NOT
#define CCLASS_SET_SHARE(cc)    (cc)->flags |= FLAG_CCLASS_SHARE
#define IS_CCLASS_NOT(cc)     (((cc)->flags & FLAG_CCLASS_NOT) != 0)
#define IS_CCLASS_SHARE(cc)   (((cc)->flags & FLAG_CCLASS_SHARE) != 0)

typedef struct {
  int    flags;
  BitSet bs;
  BBuf*  mbuf;     /* multi-byte info or NULL */
} CClassNode;


#ifdef ONIG_DEBUG

typedef struct {
  short int opcode;
  char*     name;
  short int arg_type;
} OnigOpInfoType;

extern OnigOpInfoType OnigOpInfo[];

extern void onig_print_compiled_byte_code P_((FILE* f, UChar* bp, UChar** nextp, OnigEncoding enc));

#ifdef ONIG_DEBUG_STATISTICS
extern void onig_statistics_init P_((void));
extern void onig_print_statistics P_((FILE* f));
#endif
#endif

extern UChar* onig_error_code_to_format P_((int code));
extern void  onig_snprintf_with_pattern PV_((UChar buf[], int bufsize, OnigEncoding enc, UChar* pat, UChar* pat_end, const UChar *fmt, ...));
extern int  onig_bbuf_init P_((BBuf* buf, int size));
extern int  onig_alloc_init P_((regex_t** reg, OnigOptionType option, OnigAmbigType ambig_flag, OnigEncoding enc, OnigSyntaxType* syntax));
extern int  onig_compile P_((regex_t* reg, const UChar* pattern, const UChar* pattern_end, OnigErrorInfo* einfo));
extern void onig_chain_reduce P_((regex_t* reg));
extern void onig_chain_link_add P_((regex_t* to, regex_t* add));
extern void onig_transfer P_((regex_t* to, regex_t* from));
extern int  onig_is_code_in_cc P_((OnigEncoding enc, OnigCodePoint code, CClassNode* cc));

#endif /* REGINT_H */
