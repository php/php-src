/* Extended regular expression matching and search library.
   Copyright (C) 1993, 94, 95, 96, 97, 98 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */
/* Multi-byte extension added May, 1993 by t^2 (Takahiro Tanimoto)
   Last change: May 21, 1993 by t^2  */
/* removed gapped buffer support, multiple syntax support by matz <matz@nts.co.jp> */
/* Perl5 extension added by matz <matz@caelum.co.jp> */
/* UTF-8 extension added Jan 16 1999 by Yoshida Masato  <yoshidam@tau.bekkoame.ne.jp> */

#include "php.h"

#define re_compile_pattern mbre_compile_pattern
#define re_free_pattern mbre_free_pattern
#define re_adjust_startpos mbre_adjust_startpos
#define re_compile_fastmap mbre_compile_fastmap
#define re_search mbre_search
#define re_match mbre_match
#define re_set_casetable mbre_set_casetable
#define re_copy_registers mbre_copy_registers
#define re_free_registers mbre_free_registers

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

/* We write fatal error messages on standard error.  */
#include <stdio.h>

/* isalpha(3) etc. are used for the character classes.  */
#include <ctype.h>
#include <sys/types.h>

#ifndef PARAMS
# if defined __GNUC__ || (defined __STDC__ && __STDC__)
#  define PARAMS(args) args
# else
#  define PARAMS(args) ()
# endif  /* GCC.  */
#endif  /* Not PARAMS.  */

#if defined(STDC_HEADERS)
# include <stddef.h>
#else
/* We need this for `regex.h', and perhaps for the Emacs include files.  */
# include <sys/types.h>
#endif

#ifndef __STDC__
# define volatile
#endif

#ifdef HAVE_PROTOTYPES
# define _(args) args
#else
# define _(args) ()
#endif

#ifdef RUBY_PLATFORM
#include "defines.h"

# define RUBY
extern int rb_prohibit_interrupt;
extern int rb_trap_pending;
void rb_trap_exec _((void));

# define CHECK_INTS if (!rb_prohibit_interrupt) {\
    if (rb_trap_pending) rb_trap_exec();\
}

#define xmalloc ruby_xmalloc
#define xcalloc ruby_xcalloc
#define xrealloc ruby_xrealloc
#define xfree ruby_xfree

void *xmalloc _((size_t));
void *xcalloc _((size_t,size_t));
void *xrealloc _((void*,size_t));
void xfree _((void*));
#endif


#define xmalloc emalloc
#define xcalloc ecalloc
#define xrealloc erealloc
#define xfree efree


/* Make alloca work the best possible way.  */
#ifdef __GNUC__
# ifndef atarist
#  ifndef alloca
#   define alloca __builtin_alloca
#  endif
# endif /* atarist */
#else
# if defined(HAVE_ALLOCA_H)
#  include <alloca.h>
# elif !defined(alloca)
char *alloca();
# endif
#endif /* __GNUC__ */

#ifdef _AIX
#pragma alloca
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifdef C_ALLOCA
#define FREE_VARIABLES() alloca(0)
#else
#define FREE_VARIABLES()
#endif

#define FREE_AND_RETURN_VOID(stackb)   do {				\
  FREE_VARIABLES();							\
  if (stackb != stacka) xfree(stackb);					\
  return;								\
} while(0)

#define FREE_AND_RETURN(stackb,val)    do {				\
  FREE_VARIABLES();							\
  if (stackb != stacka) xfree(stackb);					\
  return(val);								\
} while(0)

#define DOUBLE_STACK(type) do {						\
  type *stackx;								\
  unsigned int xlen = stacke - stackb; 					\
  if (stackb == stacka) {						\
    stackx = (type*)xmalloc(2 * xlen * sizeof(type));			\
    memcpy(stackx, stackb, xlen * sizeof (type));			\
  }									\
  else {								\
    stackx = (type*)xrealloc(stackb, 2 * xlen * sizeof(type));		\
  }									\
  /* Rearrange the pointers. */						\
  stackp = stackx + (stackp - stackb);					\
  stackb = stackx;							\
  stacke = stackb + 2 * xlen;						\
} while (0)

#define RE_TALLOC(n,t)  ((t*)alloca((n)*sizeof(t)))
#define TMALLOC(n,t)    ((t*)xmalloc((n)*sizeof(t)))
#define TREALLOC(s,n,t) (s=((t*)xrealloc(s,(n)*sizeof(t))))

#define EXPAND_FAIL_STACK() DOUBLE_STACK(unsigned char*)
#define ENSURE_FAIL_STACK(n)						\
  do {									\
    if (stacke - stackp <= (n)) {					\
	/* if (len > re_max_failures * MAX_NUM_FAILURE_ITEMS)		\
	   {								\
	   FREE_AND_RETURN(stackb,(-2));				\
	   }*/								\
									\
        /* Roughly double the size of the stack.  */			\
        EXPAND_FAIL_STACK();						\
      }									\
  } while (0)

/* Get the interface, including the syntax bits.  */
#include "mbregex.h"

/* Subroutines for re_compile_pattern.  */
static void store_jump _((char*, int, char*));
static void insert_jump _((int, char*, char*, char*));
static void store_jump_n _((char*, int, char*, unsigned));
static void insert_jump_n _((int, char*, char*, char*, unsigned));
#if 0
static void insert_op _((int, char*, char*));
#endif
static void insert_op_2 _((int, char*, char*, int, int));
static int memcmp_translate _((unsigned char*, unsigned char*, int, const unsigned char*));
static const unsigned char* re_mbctab_get _((int));

/* Define the syntax stuff, so we can do the \<, \>, etc.  */

/* This must be nonzero for the wordchar and notwordchar pattern
   commands in re_match.  */
#define Sword  1
#define Sword2 2

#define SYNTAX(c) re_syntax_table[c]

static const char casetable[] = {
        '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
        '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
        '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
        '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
        /* ' '     '!'     '"'     '#'     '$'     '%'     '&'     ''' */
        '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
        /* '('     ')'     '*'     '+'     ','     '-'     '.'     '/' */
        '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
        /* '0'     '1'     '2'     '3'     '4'     '5'     '6'     '7' */
        '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
        /* '8'     '9'     ':'     ';'     '<'     '='     '>'     '?' */
        '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
        /* '@'     'A'     'B'     'C'     'D'     'E'     'F'     'G' */
        '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        /* 'H'     'I'     'J'     'K'     'L'     'M'     'N'     'O' */
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        /* 'P'     'Q'     'R'     'S'     'T'     'U'     'V'     'W' */
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        /* 'X'     'Y'     'Z'     '['     '\'     ']'     '^'     '_' */
        '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
        /* '`'     'a'     'b'     'c'     'd'     'e'     'f'     'g' */
        '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        /* 'h'     'i'     'j'     'k'     'l'     'm'     'n'     'o' */
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        /* 'p'     'q'     'r'     's'     't'     'u'     'v'     'w' */
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        /* 'x'     'y'     'z'     '{'     '|'     '}'     '~' */
        '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
        '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
        '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
        '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
        '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
        '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
        '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
        '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
        '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
        '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
        '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
        '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
        '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
        '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

static char re_syntax_table[256];
static void init_syntax_once _((void));
static const unsigned char *translate = (const unsigned char*)casetable;
static void init_regs _((struct mbre_registers*, unsigned int));
static void bm_init_skip _((int *, unsigned char*, int, const unsigned char*));
#if 0
static int current_mbctype = MBCTYPE_ASCII;
#endif

#undef P

static unsigned long
scan_oct(start, len, retlen)
const char *start;
int len;
int *retlen;
{
    register const char *s = start;
    register unsigned long retval = 0;

    while (len-- && *s >= '0' && *s <= '7') {
	retval <<= 3;
	retval |= *s++ - '0';
    }
    *retlen = s - start;
    return retval;
}

static unsigned long
scan_hex(start, len, retlen)
const char *start;
int len;
int *retlen;
{
    static char hexdigit[] = "0123456789abcdef0123456789ABCDEFx";
    register const char *s = start;
    register unsigned long retval = 0;
    char *tmp;

    while (len-- && *s && (tmp = strchr(hexdigit, *s))) {
	retval <<= 4;
	retval |= (tmp - hexdigit) & 15;
	s++;
    }
    *retlen = s - start;
    return retval;
}

#define rt re_syntax_table
static void
init_syntax_once()
{
	register int c;
	static int done = 0;
	char *s = emalloc(10240), *sp;
	int ch;

   if (done)
     return;

   memset(re_syntax_table, 0, sizeof(re_syntax_table));

   for (c=0; c<=0x7f; c++)
     if (isalnum(c)) 
       re_syntax_table[c] = Sword;
   re_syntax_table['_'] = Sword;

   for (c=0x80; c<=0xff; c++)
     if (isalnum(c)) 
       re_syntax_table[c] = Sword2;
   done = 1;
   
   
	sp = s;
	sprintf(sp, "static const char re_syntax_table[256] = {\n");
	for(ch =0; ch < 256;) {
		sp = s+strlen(s);
		sprintf(sp, "\t% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,% 2d,\n", rt[ch+0], rt[ch+1], rt[ch+2], rt[ch+3], rt[ch+4], rt[ch+5], rt[ch+6], rt[ch+7], rt[ch+8], rt[ch+9], rt[ch+10], rt[ch+11], rt[ch+12], rt[ch+13], rt[ch+14], rt[ch+15]);
		ch += 16;
	}
	sprintf(sp, "};");
	php_error(E_NOTICE,"reverse_table:\n%s", s);
	efree(s);
}

void
re_set_casetable(table)
     const char *table;
{
  translate = (const unsigned char*)table;
}

/* Jim Meyering writes:

   "... Some ctype macros are valid only for character codes that
   isascii says are ASCII (SGI's IRIX-4.0.5 is one such system --when
   using /bin/cc or gcc but without giving an ansi option).  So, all
   ctype uses should be through macros like ISPRINT...  If
   STDC_HEADERS is defined, then autoconf has verified that the ctype
   macros don't need to be guarded with references to isascii. ...
   Defining isascii to 1 should let any compiler worth its salt
   eliminate the && through constant folding."
   Solaris defines some of these symbols so we must undefine them first.  */

#undef ISASCII
#if defined STDC_HEADERS || (!defined isascii && !defined HAVE_ISASCII)
# define ISASCII(c) 1
#else
# define ISASCII(c) isascii(c)
#endif

#ifdef isblank
# define ISBLANK(c) (ISASCII(c) && isblank(c))
#else
# define ISBLANK(c) ((c) == ' ' || (c) == '\t')
#endif
#ifdef isgraph
# define ISGRAPH(c) (ISASCII(c) && isgraph(c))
#else
# define ISGRAPH(c) (ISASCII(c) && isprint(c) && !isspace(c))
#endif

#undef ISPRINT
#define ISPRINT(c) (ISASCII(c) && isprint(c))
#define ISDIGIT(c) (ISASCII(c) && isdigit(c))
#define ISALNUM(c) (ISASCII(c) && isalnum(c))
#define ISALPHA(c) (ISASCII(c) && isalpha(c))
#define ISCNTRL(c) (ISASCII(c) && iscntrl(c))
#define ISLOWER(c) (ISASCII(c) && islower(c))
#define ISPUNCT(c) (ISASCII(c) && ispunct(c))
#define ISSPACE(c) (ISASCII(c) && isspace(c))
#define ISUPPER(c) (ISASCII(c) && isupper(c))
#define ISXDIGIT(c) (ISASCII(c) && isxdigit(c))

#ifndef NULL
# define NULL (void *)0
#endif

/* We remove any previous definition of `SIGN_EXTEND_CHAR',
   since ours (we hope) works properly with all combinations of
   machines, compilers, `char' and `unsigned char' argument types.
   (Per Bothner suggested the basic approach.)  */
#undef SIGN_EXTEND_CHAR
#if __STDC__
# define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#else  /* not __STDC__ */
/* As in Harbison and Steele.  */
# define SIGN_EXTEND_CHAR(c) ((((unsigned char)(c)) ^ 128) - 128)
#endif

/* These are the command codes that appear in compiled regular
   expressions, one per byte.  Some command codes are followed by
   argument bytes.  A command code can specify any interpretation
   whatsoever for its arguments.  Zero-bytes may appear in the compiled
   regular expression.

   The value of `exactn' is needed in search.c (search_buffer) in emacs.
   So regex.h defines a symbol `RE_EXACTN_VALUE' to be 1; the value of
   `exactn' we use here must also be 1.  */

enum regexpcode
  {
    unused=0,
    exactn=1, /* Followed by one byte giving n, then by n literal bytes.  */
    begline,  /* Fail unless at beginning of line.  */
    endline,  /* Fail unless at end of line.  */
    begbuf,   /* Succeeds if at beginning of buffer (if emacs) or at beginning
                 of string to be matched (if not).  */
    endbuf,   /* Analogously, for end of buffer/string.  */
    endbuf2,  /* End of buffer/string, or newline just before it.  */
    begpos,   /* Matches where last scan//gsub left off.  */
    jump,     /* Followed by two bytes giving relative address to jump to.  */
    jump_past_alt,/* Same as jump, but marks the end of an alternative.  */
    on_failure_jump,	 /* Followed by two bytes giving relative address of 
			    place to resume at in case of failure.  */
    finalize_jump,	 /* Throw away latest failure point and then jump to 
			    address.  */
    maybe_finalize_jump, /* Like jump but finalize if safe to do so.
			    This is used to jump back to the beginning
			    of a repeat.  If the command that follows
			    this jump is clearly incompatible with the
			    one at the beginning of the repeat, such that
			    we can be sure that there is no use backtracking
			    out of repetitions already completed,
			    then we finalize.  */
    dummy_failure_jump,  /* Jump, and push a dummy failure point. This 
			    failure point will be thrown away if an attempt 
                            is made to use it for a failure. A + construct 
                            makes this before the first repeat.  Also
                            use it as an intermediary kind of jump when
                            compiling an or construct.  */
    push_dummy_failure, /* Push a dummy failure point and continue.  Used at the end of
			   alternatives.  */
    succeed_n,	 /* Used like on_failure_jump except has to succeed n times;
		    then gets turned into an on_failure_jump. The relative
                    address following it is useless until then.  The
                    address is followed by two bytes containing n.  */
    jump_n,	 /* Similar to jump, but jump n times only; also the relative
		    address following is in turn followed by yet two more bytes
                    containing n.  */
    try_next,    /* Jump to next pattern for the first time,
		    leaving this pattern on the failure stack. */
    finalize_push,	/* Finalize stack and push the beginning of the pattern
			   on the stack to retry (used for non-greedy match) */
    finalize_push_n,	/* Similar to finalize_push, buf finalize n time only */
    set_number_at,	/* Set the following relative location to the
			   subsequent number.  */
    anychar,	 /* Matches any (more or less) one character excluding newlines.  */
    anychar_repeat,	 /* Matches sequence of characters excluding newlines.  */
    charset,     /* Matches any one char belonging to specified set.
		    First following byte is number of bitmap bytes.
		    Then come bytes for a bitmap saying which chars are in.
		    Bits in each byte are ordered low-bit-first.
		    A character is in the set if its bit is 1.
		    A character too large to have a bit in the map
		    is automatically not in the set.  */
    charset_not, /* Same parameters as charset, but match any character
                    that is not one of those specified.  */
    start_memory, /* Start remembering the text that is matched, for
		    storing in a memory register.  Followed by one
                    byte containing the register number.  Register numbers
                    must be in the range 0 through MBRE_NREGS.  */
    stop_memory, /* Stop remembering the text that is matched
		    and store it in a memory register.  Followed by
                    one byte containing the register number. Register
                    numbers must be in the range 0 through MBRE_NREGS.  */
    start_paren,    /* Place holder at the start of (?:..). */
    stop_paren,    /* Place holder at the end of (?:..). */
    casefold_on,   /* Turn on casefold flag. */
    casefold_off,  /* Turn off casefold flag. */
    option_set,	   /* Turn on multi line match (match with newlines). */
    start_nowidth, /* Save string point to the stack. */
    stop_nowidth,  /* Restore string place at the point start_nowidth. */
    pop_and_fail,  /* Fail after popping nowidth entry from stack. */
    stop_backtrack,  /* Restore backtrack stack at the point start_nowidth. */
    duplicate,   /* Match a duplicate of something remembered.
		    Followed by one byte containing the index of the memory 
                    register.  */
    fail,        /* always fails. */
    wordchar,    /* Matches any word-constituent character.  */
    notwordchar, /* Matches any char that is not a word-constituent.  */
    wordbeg,	 /* Succeeds if at word beginning.  */
    wordend,	 /* Succeeds if at word end.  */
    wordbound,   /* Succeeds if at a word boundary.  */
    notwordbound,/* Succeeds if not at a word boundary.  */
  };


/* Number of failure points to allocate space for initially,
   when matching.  If this number is exceeded, more space is allocated,
   so it is not a hard limit.  */

#ifndef NFAILURES
#define NFAILURES 160
#endif

/* Store NUMBER in two contiguous bytes starting at DESTINATION.  */
#define STORE_NUMBER(destination, number)				\
  do { (destination)[0] = (number) & 0377;				\
    (destination)[1] = (number) >> 8; } while (0)

/* Same as STORE_NUMBER, except increment the destination pointer to
   the byte after where the number is stored.  Watch out that values for
   DESTINATION such as p + 1 won't work, whereas p will.  */
#define STORE_NUMBER_AND_INCR(destination, number)			\
  do { STORE_NUMBER(destination, number);				\
    (destination) += 2; } while (0)


/* Put into DESTINATION a number stored in two contingous bytes starting
   at SOURCE.  */
#define EXTRACT_NUMBER(destination, source)				\
  do { (destination) = *(source) & 0377;				\
    (destination) += SIGN_EXTEND_CHAR(*(char*)((source) + 1)) << 8; } while (0)

/* Same as EXTRACT_NUMBER, except increment the pointer for source to
   point to second byte of SOURCE.  Note that SOURCE has to be a value
   such as p, not, e.g., p + 1. */
#define EXTRACT_NUMBER_AND_INCR(destination, source)			\
  do { EXTRACT_NUMBER(destination, source);				\
       (source) += 2; } while (0)


/* Specify the precise syntax of regexps for compilation.  This provides
   for compatibility for various utilities which historically have
   different, incompatible syntaxes.

   The argument SYNTAX is a bit-mask comprised of the various bits
   defined in regex.h.  */
#if 0
long
re_set_syntax(syntax)
  long syntax;
{
    /* obsolete */
    return 0;
}
#endif

/* Macros for re_compile_pattern, which is found below these definitions.  */

#define TRANSLATE_P() ((options&MBRE_OPTION_IGNORECASE) && translate)
#define MAY_TRANSLATE() ((bufp->options&(MBRE_OPTION_IGNORECASE|MBRE_MAY_IGNORECASE)) && translate)
/* Fetch the next character in the uncompiled pattern---translating it 
   if necessary.  Also cast from a signed character in the constant
   string passed to us by the user to an unsigned char that we can use
   as an array index (in, e.g., `translate').  */
#define PATFETCH(c)							\
  do {if (p == pend) goto end_of_pattern;				\
    c = (unsigned char) *p++; 						\
    if (TRANSLATE_P()) c = (unsigned char)translate[c];	\
  } while (0)

/* Fetch the next character in the uncompiled pattern, with no
   translation.  */
#define PATFETCH_RAW(c)							\
  do {if (p == pend) goto end_of_pattern;				\
    c = (unsigned char)*p++; 						\
  } while (0)

/* Go backwards one character in the pattern.  */
#define PATUNFETCH p--

#define MBC2WC(c, p)							\
  do {									\
    if (current_mbctype == MBCTYPE_UTF8) {				\
      int n = mbclen(c) - 1;						\
      c &= (1<<(MBRE_BYTEWIDTH-2-n)) - 1;				\
      while (n--) {							\
	c = c << 6 | (*p++ & ((1<<6)-1));				\
      }									\
    }									\
    else {								\
      c <<= 8;								\
      c |= (unsigned char)*(p)++;					\
    }									\
  } while (0)

#define PATFETCH_MBC(c)							\
  do {									\
    if (p + mbclen(c) - 1 >= pend) goto end_of_pattern;			\
    MBC2WC(c, p);							\
  } while(0)

#define WC2MBC1ST(c)							\
 ((c<0x100)?(c):((current_mbctype != MBCTYPE_UTF8)?(((c)>>8)&0xff):utf8_firstbyte(c)))

static unsigned int
utf8_firstbyte(c)
     unsigned long c;
{
  if (c < 0x80) return c;
  if (c <= 0x7ff) return ((c>>6)&0xff)|0xc0;
  if (c <= 0xffff) return ((c>>12)&0xff)|0xe0;
  if (c <= 0x1fffff) return ((c>>18)&0xff)|0xf0;
  if (c <= 0x3ffffff) return ((c>>24)&0xff)|0xf8;
  if (c <= 0x7fffffff) return ((c>>30)&0xff)|0xfc;
#if SIZEOF_INT > 4
  if (c <= 0xfffffffff) return 0xfe;
#else
  return 0xfe;
#endif
}

#if 0
static void
print_mbc(c)
     unsigned int c;
{
  if (current_mbctype == MBCTYPE_UTF8) {
    if (c < 0x80)
      printf("%c", c);
    else if (c <= 0x7ff)
      printf("%c%c", utf8_firstbyte(c), c&0x3f);
    else if (c <= 0xffff)
      printf("%c%c%c", utf8_firstbyte(c), (c>>6)&0x3f, c&0x3f);
    else if (c <= 0x1fffff) 
      printf("%c%c%c%c", utf8_firstbyte(c), (c>>12)&0x3f, (c>>6)&0x3f, c&0x3f);
    else if (c <= 0x3ffffff)
      printf("%c%c%c%c%c", utf8_firstbyte(c), (c>>18)&0x3f, (c>>12)&0x3f, (c>>6)&0x3f, c&0x3f);
    else if (c <= 0x7fffffff)
      printf("%c%c%c%c%c%c", utf8_firstbyte(c), (c>>24)&0x3f, (c>>18)&0x3f, (c>>12)&0x3f, (c>>6)&0x3f, c&0x3f);
  }
  else if (c < 0xff) {
    printf("\\%o", c);
  }
  else {
    printf("%c%c", c>>MBRE_BYTEWIDTH, c&0xff);
  }
}
#endif

/* If the buffer isn't allocated when it comes in, use this.  */
#define INIT_BUF_SIZE  28

/* Make sure we have at least N more bytes of space in buffer.  */
#define GET_BUFFER_SPACE(n)						\
  do {								        \
    while (b - bufp->buffer + (n) >= bufp->allocated)			\
      EXTEND_BUFFER;							\
  } while (0)

/* Make sure we have one more byte of buffer space and then add CH to it.  */
#define BUFPUSH(ch)							\
  do {									\
    GET_BUFFER_SPACE(1);						\
    *b++ = (char)(ch);							\
  } while (0)

/* Extend the buffer by twice its current size via reallociation and
   reset the pointers that pointed into the old allocation to point to
   the correct places in the new allocation.  If extending the buffer
   results in it being larger than 1 << 16, then flag memory exhausted.  */
#define EXTEND_BUFFER							\
  do { char *old_buffer = bufp->buffer;					\
    if (bufp->allocated == (1L<<16)) goto too_big;			\
    bufp->allocated *= 2;						\
    if (bufp->allocated > (1L<<16)) bufp->allocated = (1L<<16);		\
    bufp->buffer = (char*)xrealloc(bufp->buffer, bufp->allocated);	\
    if (bufp->buffer == 0)						\
      goto memory_exhausted;						\
    b = (b - old_buffer) + bufp->buffer;				\
    if (fixup_alt_jump)							\
      fixup_alt_jump = (fixup_alt_jump - old_buffer) + bufp->buffer;	\
    if (laststart)							\
      laststart = (laststart - old_buffer) + bufp->buffer;		\
    begalt = (begalt - old_buffer) + bufp->buffer;			\
    if (pending_exact)							\
      pending_exact = (pending_exact - old_buffer) + bufp->buffer;	\
  } while (0)


/* Set the bit for character C in a character set list.  */
#define SET_LIST_BIT(c)							\
  (b[(unsigned char)(c) / MBRE_BYTEWIDTH]					\
   |= 1 << ((unsigned char)(c) % MBRE_BYTEWIDTH))

/* Get the next unsigned number in the uncompiled pattern.  */
#define GET_UNSIGNED_NUMBER(num) 					\
  do { if (p != pend) { 						\
        PATFETCH(c); 							\
	while (ISDIGIT(c)) { 						\
	  if (num < 0) 							\
	     num = 0; 							\
	  num = num * 10 + c - '0'; 					\
	  if (p == pend) 						\
	     break; 							\
	  PATFETCH(c); 							\
	} 								\
     } 									\
  } while (0)

#define STREQ(s1, s2) ((strcmp(s1, s2) == 0))

#define CHAR_CLASS_MAX_LENGTH  6 /* Namely, `xdigit'.  */

#define IS_CHAR_CLASS(string)						\
   (STREQ(string, "alpha") || STREQ(string, "upper")			\
    || STREQ(string, "lower") || STREQ(string, "digit")			\
    || STREQ(string, "alnum") || STREQ(string, "xdigit")		\
    || STREQ(string, "space") || STREQ(string, "print")			\
    || STREQ(string, "punct") || STREQ(string, "graph")			\
    || STREQ(string, "cntrl") || STREQ(string, "blank"))

#define STORE_MBC(p, c)							\
  do {									\
    (p)[0] = (unsigned char)(((c) >>24) & 0xff);			\
    (p)[1] = (unsigned char)(((c) >>16) & 0xff);			\
    (p)[2] = (unsigned char)(((c) >> 8) & 0xff);			\
    (p)[3] = (unsigned char)(((c) >> 0) & 0xff);			\
  } while (0)

#define STORE_MBC_AND_INCR(p, c) 					\
  do {									\
    *(p)++ = (unsigned char)(((c) >>24) & 0xff);			\
    *(p)++ = (unsigned char)(((c) >>16) & 0xff);			\
    *(p)++ = (unsigned char)(((c) >> 8) & 0xff);			\
    *(p)++ = (unsigned char)(((c) >> 0) & 0xff);			\
  } while (0)

#define EXTRACT_MBC(p) 							\
  ((unsigned int)((unsigned char)(p)[0] << 24 |				\
		    (unsigned char)(p)[1] << 16 |			\
                    (unsigned char)(p)[2] <<  8 |			\
		    (unsigned char)(p)[3]))

#define EXTRACT_MBC_AND_INCR(p) 					\
  ((unsigned int)((p) += 4, 						\
		    (unsigned char)(p)[-4] << 24 |			\
		    (unsigned char)(p)[-3] << 16 |			\
                    (unsigned char)(p)[-2] <<  8 |			\
		    (unsigned char)(p)[-1]))

#define EXTRACT_UNSIGNED(p) \
  ((unsigned char)(p)[0] | (unsigned char)(p)[1] << 8)
#define EXTRACT_UNSIGNED_AND_INCR(p) \
  ((p) += 2, (unsigned char)(p)[-2] | (unsigned char)(p)[-1] << 8)

/* Handle (mb)?charset(_not)?.

   Structure of mbcharset(_not)? in compiled pattern.

     struct {
       unsinged char id;		mbcharset(_not)?
       unsigned char sbc_size;
       unsigned char sbc_map[sbc_size];	same as charset(_not)? up to here.
       unsigned short mbc_size;		number of intervals.
       struct {
	 unsigned long beg;		beginning of interval.
	 unsigned long end;		end of interval.
       } intervals[mbc_size];
     }; */

static void
set_list_bits(c1, c2, b)
    unsigned long c1, c2;
    unsigned char *b;
{
  unsigned char sbc_size = b[-1];
  unsigned short mbc_size = EXTRACT_UNSIGNED(&b[sbc_size]);
  unsigned short beg, end, upb;

  if (c1 > c2)
    return;
  b = &b[sbc_size + 2];

  for (beg = 0, upb = mbc_size; beg < upb; ) {
    unsigned short mid = (unsigned short)(beg + upb) >> 1;

    if ((int)c1 - 1 > (int)EXTRACT_MBC(&b[mid*8+4]))
      beg = mid + 1;
    else
      upb = mid;
  }

  for (end = beg, upb = mbc_size; end < upb; ) {
    unsigned short mid = (unsigned short)(end + upb) >> 1;

    if ((int)c2 >= (int)EXTRACT_MBC(&b[mid*8]) - 1)
      end = mid + 1;
    else
      upb = mid;
  }

  if (beg != end) {
    if (c1 > EXTRACT_MBC(&b[beg*8]))
      c1 = EXTRACT_MBC(&b[beg*8]);
    if (c2 < EXTRACT_MBC(&b[(end - 1)*8+4]))
      c2 = EXTRACT_MBC(&b[(end - 1)*8+4]);
  }
  if (end < mbc_size && end != beg + 1)
    /* NOTE: memcpy() would not work here.  */
    memmove(&b[(beg + 1)*8], &b[end*8], (mbc_size - end)*8);
  STORE_MBC(&b[beg*8 + 0], c1);
  STORE_MBC(&b[beg*8 + 4], c2);
  mbc_size += beg - end + 1;
  STORE_NUMBER(&b[-2], mbc_size);
}

static int
is_in_list(c, b)
    unsigned long c;
    const unsigned char *b;
{
  unsigned short size;
  unsigned short i, j;

  size = *b++;
  if ((int)c / MBRE_BYTEWIDTH < (int)size && b[c / MBRE_BYTEWIDTH] & 1 << c % MBRE_BYTEWIDTH) {
    return 1;
  }
  b += size + 2;
  size = EXTRACT_UNSIGNED(&b[-2]);
  if (size == 0) return 0;

  for (i = 0, j = size; i < j; ) {
    unsigned short k = (unsigned short)(i + j) >> 1;

    if (c > EXTRACT_MBC(&b[k*8+4]))
      i = k + 1;
    else
      j = k;
  }
  if (i < size && EXTRACT_MBC(&b[i*8]) <= c
      && ((unsigned char)c != '\n' && (unsigned char)c != '\0'))
    return 1;
  return 0;
}

#if 0
static void
print_partial_compiled_pattern(start, end)
    unsigned char *start;
    unsigned char *end;
{
  int mcnt, mcnt2;
  unsigned char *p = start;
  unsigned char *pend = end;

  if (start == NULL) {
    printf("(null)\n");
    return;
  }

  /* Loop over pattern commands.  */
  while (p < pend) {
    switch ((enum regexpcode)*p++) {
    case unused:
      printf("/unused");
      break;

    case exactn:
      mcnt = *p++;
      printf("/exactn/%d", mcnt);
      do {
	putchar('/');
	printf("%c", *p++);
      }
      while (--mcnt);
      break;

    case start_memory:
      mcnt = *p++;
      printf("/start_memory/%d/%d", mcnt, *p++);
      break;

    case stop_memory:
      mcnt = *p++;
      printf("/stop_memory/%d/%d", mcnt, *p++);
      break;

    case start_paren:
      printf("/start_paren");
      break;

    case stop_paren:
      printf("/stop_paren");
      break;

    case casefold_on:
      printf("/casefold_on");
      break;

    case casefold_off:
      printf("/casefold_off");
      break;

    case option_set:
      printf("/option_set/%d", *p++);
      break;

    case start_nowidth:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/start_nowidth//%d", mcnt);
      break;

    case stop_nowidth:
      printf("/stop_nowidth//");
      p += 2;
      break;

    case pop_and_fail:
      printf("/pop_and_fail");
      break;

    case stop_backtrack:
      printf("/stop_backtrack//");
      p += 2;
      break;

    case duplicate:
      printf("/duplicate/%d", *p++);
      break;

    case anychar:
      printf("/anychar");
      break;

    case anychar_repeat:
      printf("/anychar_repeat");
      break;

    case charset:
    case charset_not:
      {
	register int c;

	printf("/charset%s",
	       (enum regexpcode)*(p - 1) == charset_not ? "_not" : "");

	mcnt = *p++;
	printf("/%d", mcnt);
	for (c = 0; c < mcnt; c++) {
	  unsigned bit;
	  unsigned char map_byte = p[c];

	  putchar ('/');

	  for (bit = 0; bit < MBRE_BYTEWIDTH; bit++)
	    if (map_byte & (1 << bit))
	      printf("%c", c * MBRE_BYTEWIDTH + bit);
	}
	p += mcnt;
	mcnt = EXTRACT_UNSIGNED_AND_INCR(p);
	printf("/");
	while (mcnt--) {
	  print_mbc(EXTRACT_MBC_AND_INCR(p));
	  printf("-");
	  print_mbc(EXTRACT_MBC_AND_INCR(p));
	}
	break;
      }

    case begline:
      printf("/begline");
      break;

    case endline:
      printf("/endline");
      break;

    case on_failure_jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/on_failure_jump//%d", mcnt);
      break;

    case dummy_failure_jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/dummy_failure_jump//%d", mcnt);
      break;

    case push_dummy_failure:
      printf("/push_dummy_failure");
      break;

    case finalize_jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/finalize_jump//%d", mcnt);
      break;

    case maybe_finalize_jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/maybe_finalize_jump//%d", mcnt);
      break;

    case jump_past_alt:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/jump_past_alt//%d", mcnt);
      break;

    case jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/jump//%d", mcnt);
      break;

    case succeed_n: 
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      EXTRACT_NUMBER_AND_INCR(mcnt2, p);
      printf("/succeed_n//%d//%d", mcnt, mcnt2);
      break;

    case jump_n: 
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      EXTRACT_NUMBER_AND_INCR(mcnt2, p);
      printf("/jump_n//%d//%d", mcnt, mcnt2);
      break;

    case set_number_at: 
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      EXTRACT_NUMBER_AND_INCR(mcnt2, p);
      printf("/set_number_at//%d//%d", mcnt, mcnt2);
      break;

    case try_next:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/try_next//%d", mcnt);
      break;

    case finalize_push:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      printf("/finalize_push//%d", mcnt);
      break;

    case finalize_push_n:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      EXTRACT_NUMBER_AND_INCR(mcnt2, p);
      printf("/finalize_push_n//%d//%d", mcnt, mcnt2);
      break;

    case wordbound:
      printf("/wordbound");
      break;

    case notwordbound:
      printf("/notwordbound");
      break;

    case wordbeg:
      printf("/wordbeg");
      break;

    case wordend:
      printf("/wordend");

    case wordchar:
      printf("/wordchar");
      break;
	  
    case notwordchar:
      printf("/notwordchar");
      break;

    case begbuf:
      printf("/begbuf");
      break;

    case endbuf:
      printf("/endbuf");
      break;

    case endbuf2:
      printf("/endbuf2");
      break;

    case begpos:
      printf("/begpos");
      break;

    default:
      printf("?%d", *(p-1));
    }
  }
  printf("/\n");
}


static void
print_compiled_pattern(bufp)
     struct mbre_pattern_buffer *bufp;
{
  unsigned char *buffer = (unsigned char*)bufp->buffer;

  print_partial_compiled_pattern(buffer, buffer + bufp->used);
}
#endif

static char*
calculate_must_string(start, end)
     char *start;
     char *end;
{
  int mcnt;
  int max = 0;
  char *p = start;
  char *pend = end;
  char *must = 0;

  if (start == NULL) return 0;

  /* Loop over pattern commands.  */
  while (p < pend) {
    switch ((enum regexpcode)*p++) {
    case unused:
      break;

    case exactn:
      mcnt = *p;
      if (mcnt > max) {
	must = p;
	max = mcnt;
      }
      p += mcnt+1;
      break;

    case start_memory:
    case stop_memory:
      p += 2;
      break;

    case duplicate:
      p++;
      break;

    case casefold_on:
    case casefold_off:
      return 0;		/* should not check must_string */

    case pop_and_fail:
    case anychar:
    case anychar_repeat:
    case begline:
    case endline:
    case wordbound:
    case notwordbound:
    case wordbeg:
    case wordend:
    case wordchar:
    case notwordchar:
    case begbuf:
    case endbuf:
    case endbuf2:
    case begpos:
    case push_dummy_failure:
    case start_paren:
    case stop_paren:
    case option_set:
      break;

    case charset:
    case charset_not:
      mcnt = *p++;
      p += mcnt;
      mcnt = EXTRACT_UNSIGNED_AND_INCR(p);
      while (mcnt--) {
	p += 4;
      }
      break;

    case on_failure_jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      if (mcnt > 0) p += mcnt;
      if ((enum regexpcode)p[-3] == jump) {
       p -= 2;
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	if (mcnt > 0) p += mcnt;
      }
      break;

    case dummy_failure_jump:
    case succeed_n: 
    case try_next:
    case jump:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      if (mcnt > 0) p += mcnt;
      break;

    case start_nowidth:
    case stop_nowidth:
    case stop_backtrack:
    case finalize_jump:
    case maybe_finalize_jump:
    case finalize_push:
      p += 2;
      break;

    case jump_n: 
    case set_number_at: 
    case finalize_push_n:
      p += 4;
      break;

    default:
      break;
    }
  }
  return must;
}

static unsigned int
read_backslash(c)
     int c;
{
  switch (c) {
  case 'n':
    return '\n';

  case 't':
    return '\t';

  case 'r':
    return '\r';

  case 'f':
    return '\f';

  case 'v':
    return '\v';

  case 'a':
    return '\007';

  case 'b':
    return '\010';

  case 'e':
    return '\033';
  }
  return c;
}

static unsigned int
read_special(p, pend, pp)
     const char *p, *pend, **pp;
{
  int c;

  PATFETCH_RAW(c);
  switch (c) {
  case 'M':
    PATFETCH_RAW(c);
    if (c != '-') return -1;
    PATFETCH_RAW(c);
    *pp = p;
    if (c == '\\') {
      return read_special(p, pend, pp) | 0x80;
    }
    else if (c == -1) return ~0;
    else {
      return ((c & 0xff) | 0x80);
    }

  case 'C':
    PATFETCH_RAW(c);
    if (c != '-') return ~0;
  case 'c':
    PATFETCH_RAW(c);
    *pp = p;
    if (c == '\\') {
      c = read_special(p, pend, pp);
    }
    else if (c == '?') return 0177;
    else if (c == -1) return ~0;
    return c & 0x9f;
  default:
    return read_backslash(c);
  }

 end_of_pattern:
  return ~0;
}

/* re_compile_pattern takes a regular-expression string
   and converts it into a buffer full of byte commands for matching.

   PATTERN   is the address of the pattern string
   SIZE      is the length of it.
   BUFP	    is a  struct mbre_pattern_buffer *  which points to the info
	     on where to store the byte commands.
	     This structure contains a  char *  which points to the
	     actual space, which should have been obtained with malloc.
	     re_compile_pattern may use realloc to grow the buffer space.

   The number of bytes of commands can be found out by looking in
   the `struct mbre_pattern_buffer' that bufp pointed to, after
   re_compile_pattern returns. */

char *
re_compile_pattern(pattern, size, bufp)
     const char *pattern;
     int size;
     struct mbre_pattern_buffer *bufp;
{
  register char *b = bufp->buffer;
  register const char *p = pattern;
  const char *nextp;
  const char *pend = pattern + size;
  register unsigned int c, c1=0;
  const char *p0;
  int numlen;
#define ERROR_MSG_MAX_SIZE 200
  static char error_msg[ERROR_MSG_MAX_SIZE+1];

  /* Address of the count-byte of the most recently inserted `exactn'
     command.  This makes it possible to tell whether a new exact-match
     character can be added to that command or requires a new `exactn'
     command.  */

  char *pending_exact = 0;

  /* Address of the place where a forward-jump should go to the end of
     the containing expression.  Each alternative of an `or', except the
     last, ends with a forward-jump of this sort.  */

  char *fixup_alt_jump = 0;

  /* Address of start of the most recently finished expression.
     This tells postfix * where to find the start of its operand.  */

  char *laststart = 0;

  /* In processing a repeat, 1 means zero matches is allowed.  */

  char zero_times_ok;

  /* In processing a repeat, 1 means many matches is allowed.  */

  char many_times_ok;

  /* In processing a repeat, 1 means non-greedy matches.  */

  char greedy;

  /* Address of beginning of regexp, or inside of last (.  */

  char *begalt = b;

  /* Place in the uncompiled pattern (i.e., the {) to
     which to go back if the interval is invalid.  */
  const char *beg_interval;

  /* In processing an interval, at least this many matches must be made.  */
  int lower_bound;

  /* In processing an interval, at most this many matches can be made.  */
  int upper_bound;

  /* Stack of information saved by ( and restored by ).
     Five stack elements are pushed by each (:
     First, the value of b.
     Second, the value of fixup_alt_jump.
     Third, the value of begalt.
     Fourth, the value of regnum.
     Fifth, the type of the paren. */

  int stacka[40];
  int *stackb = stacka;
  int *stackp = stackb;
  int *stacke = stackb + 40;

  /* Counts ('s as they are encountered.  Remembered for the matching ),
     where it becomes the register number to put in the stop_memory
     command.  */

  int regnum = 1;

  int range = 0;
  int had_mbchar = 0;
  int had_num_literal = 0;
  int had_char_class = 0;

  int options = bufp->options;

  int current_mbctype = bufp->mbctype;
  const unsigned char *re_mbctab = re_mbctab_get(current_mbctype);

  bufp->fastmap_accurate = 0;
  bufp->must = 0;
  bufp->must_skip = 0;
  bufp->stclass = 0;

  /* Initialize the syntax table.  */
  init_syntax_once();

  if (bufp->allocated == 0) {
    bufp->allocated = INIT_BUF_SIZE;
    if (bufp->buffer)
      /* EXTEND_BUFFER loses when bufp->allocated is 0.  */
      bufp->buffer = (char*)xrealloc(bufp->buffer, INIT_BUF_SIZE);
    else
      /* Caller did not allocate a buffer.  Do it for them.  */
      bufp->buffer = (char*)xmalloc(INIT_BUF_SIZE);
    if (!bufp->buffer) goto memory_exhausted;
    begalt = b = bufp->buffer;
  }

  while (p != pend) {
    PATFETCH(c);

    switch (c) {
    case '$':
      if (bufp->options & MBRE_OPTION_SINGLELINE) {
	BUFPUSH(endbuf);
      }
      else {
	p0 = p;
	/* When testing what follows the $,
	   look past the \-constructs that don't consume anything.  */

	while (p0 != pend) {
	  if (*p0 == '\\' && p0 + 1 != pend
	      && (p0[1] == 'b' || p0[1] == 'B'))
	    p0 += 2;
	  else
	    break;
	}
	BUFPUSH(endline);
      }
      break;

    case '^':
      if (bufp->options & MBRE_OPTION_SINGLELINE)
	BUFPUSH(begbuf);
      else
	BUFPUSH(begline);
      break;

    case '+':
    case '?':
    case '*':
      /* If there is no previous pattern, char not special. */
      if (!laststart) {
	snprintf(error_msg, ERROR_MSG_MAX_SIZE, 
		 "invalid regular expression; there's no previous pattern, to which '%c' would define cardinality at %d", 
		 c, p-pattern);
	FREE_AND_RETURN(stackb, error_msg);
      }
      /* If there is a sequence of repetition chars,
	 collapse it down to just one.  */
      zero_times_ok = c != '+';
      many_times_ok = c != '?';
      greedy = 1;
      if (p != pend) {
	PATFETCH(c);
	switch (c) {
	case '?':
	  greedy = 0;
	  break;
	case '*':
	case '+':
	  goto nested_meta;
	default:
	  PATUNFETCH;
	  break;
	}
      }

    repeat:
      /* Star, etc. applied to an empty pattern is equivalent
	 to an empty pattern.  */
      if (!laststart)  
	break;

      if (greedy && many_times_ok && *laststart == anychar && b - laststart <= 2) {
	if (b[-1] == stop_paren)
	  b--;
	if (zero_times_ok)
	  *laststart = anychar_repeat;
	else {
	  BUFPUSH(anychar_repeat);
	}
	break;
      }
      /* Now we know whether or not zero matches is allowed
	 and also whether or not two or more matches is allowed.  */
      if (many_times_ok) {
	/* If more than one repetition is allowed, put in at the
	   end a backward relative jump from b to before the next
	   jump we're going to put in below (which jumps from
	   laststart to after this jump).  */
	GET_BUFFER_SPACE(3);
	store_jump(b,greedy?maybe_finalize_jump:finalize_push,laststart-3);
	b += 3;  	/* Because store_jump put stuff here.  */
      }

      /* On failure, jump from laststart to next pattern, which will be the
	 end of the buffer after this jump is inserted.  */
      GET_BUFFER_SPACE(3);
      insert_jump(on_failure_jump, laststart, b + 3, b);
      b += 3;

      if (zero_times_ok) {
	if (greedy == 0) {
	  GET_BUFFER_SPACE(3);
	  insert_jump(try_next, laststart, b + 3, b);
	  b += 3;
	}
      }
      else {
	/* At least one repetition is required, so insert a
	   `dummy_failure_jump' before the initial
	   `on_failure_jump' instruction of the loop. This
	   effects a skip over that instruction the first time
	   we hit that loop.  */
	GET_BUFFER_SPACE(3);
	insert_jump(dummy_failure_jump, laststart, laststart + 6, b);
	b += 3;
      }
      break;

    case '.':
      laststart = b;
      BUFPUSH(anychar);
      break;

    case '[':
      if (p == pend)
	FREE_AND_RETURN(stackb, "invalid regular expression; '[' can't be the last character ie. can't start range at the end of pattern");
      while ((b - bufp->buffer + 9 + (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH)
	     > bufp->allocated)
	EXTEND_BUFFER;

      laststart = b;
      if (*p == '^') {
	BUFPUSH(charset_not); 
	p++;
      }
      else
	BUFPUSH(charset);
      p0 = p;

      BUFPUSH((1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH);
      /* Clear the whole map */
      memset(b, 0, (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH + 2);

      had_mbchar = 0;
      had_num_literal = 0;
      had_char_class = 0;

      /* Read in characters and ranges, setting map bits.  */
      for (;;) {
	int size;
	unsigned last = (unsigned)-1;

	if ((size = EXTRACT_UNSIGNED(&b[(1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH]))
	    || current_mbctype) {
	  /* Ensure the space is enough to hold another interval
	     of multi-byte chars in charset(_not)?.  */
	  size = (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH + 2 + size*8 + 8;
	  while (b + size + 1 > bufp->buffer + bufp->allocated)
	    EXTEND_BUFFER;
	}
      range_retry:
	if (range && had_char_class) {
	  FREE_AND_RETURN(stackb, "invalid regular expression; can't use character class as an end value of range");
	}
	PATFETCH(c);

	if (c == ']') {
	  if (p == p0 + 1) {
	    if (p == pend)
	      FREE_AND_RETURN(stackb, "invalid regular expression; empty character class");
	  }
	  else 
	    /* Stop if this isn't merely a ] inside a bracket
	       expression, but rather the end of a bracket
	       expression.  */
	    break;
	}
	/* Look ahead to see if it's a range when the last thing
	   was a character class.  */
	if (had_char_class && c == '-' && *p != ']')
	  FREE_AND_RETURN(stackb, "invalid regular expression; can't use character class as a start value of range");
	if (ismbchar(c)) {
	  PATFETCH_MBC(c);
	  had_mbchar++;
	}
	had_char_class = 0;

	/* \ escapes characters when inside [...].  */
	if (c == '\\') {
	  PATFETCH_RAW(c);
	  switch (c) {
	  case 'w':
	    for (c = 0; c < (1 << MBRE_BYTEWIDTH); c++) {
	      if (SYNTAX(c) == Sword ||
		  (!current_mbctype && SYNTAX(c) == Sword2))
		SET_LIST_BIT(c);
	    }
	    if (current_mbctype) {
	      set_list_bits(0x80, 0xffffffff, b);
	    }
	    had_char_class = 1;
	    last = -1;
	    continue;

	  case 'W':
	    for (c = 0; c < (1 << MBRE_BYTEWIDTH); c++) {
	      if (SYNTAX(c) != Sword &&
		  ((current_mbctype && !re_mbctab[c]) ||
		  (!current_mbctype && SYNTAX(c) != Sword2)))
		SET_LIST_BIT(c);
	    }
	    had_char_class = 1;
	    last = -1;
	    continue;

	  case 's':
	    for (c = 0; c < 256; c++)
	      if (ISSPACE(c))
		SET_LIST_BIT(c);
	    had_char_class = 1;
	    last = -1;
	    continue;

	  case 'S':
	    for (c = 0; c < 256; c++)
	      if (!ISSPACE(c))
		SET_LIST_BIT(c);
	    if (current_mbctype)
	      set_list_bits(0x80, 0xffffffff, b);
	    had_char_class = 1;
	    last = -1;
	    continue;

	  case 'd':
	    for (c = '0'; c <= '9'; c++)
	      SET_LIST_BIT(c);
	    had_char_class = 1;
	    last = -1;
	    continue;

	  case 'D':
	    for (c = 0; c < 256; c++)
	      if (!ISDIGIT(c))
		SET_LIST_BIT(c);
	    if (current_mbctype)
	      set_list_bits(0x80, 0xffffffff, b);
	    had_char_class = 1;
	    last = -1;
	    continue;

	  case 'x':
	    c = scan_hex(p, 2, &numlen);
	    p += numlen;
	    had_num_literal = 1;
	    break;

	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	    PATUNFETCH;
	    c = scan_oct(p, 3, &numlen);
	    p += numlen;
	    had_num_literal = 1;
	    break;

	  case 'M':
	  case 'C':
	  case 'c':
	    p0 = --p;
	    c = read_special(p, pend, &p0);
	    if (c > 255) goto invalid_escape;
	    p = p0;
	    had_num_literal = 1;
	    break;

	  default:
	    c = read_backslash(c);
	    if (ismbchar(c)) {
	      PATFETCH_MBC(c);
	      had_mbchar++;
	    }
	    break;
	  }
	}

	/* Get a range.  */
	if (range) {
	  if (last > c)
	    goto invalid_pattern;

	  range = 0;
	  if (had_mbchar == 0) {
	    for (;last<=c;last++)
	      SET_LIST_BIT(last);
	  }
	  else if (had_mbchar == 2) {
	    set_list_bits(last, c, b);
	  }
	  else {
	    /* restriction: range between sbc and mbc */
	    goto invalid_pattern;
	  }
	}
	else if (p[0] == '-' && p[1] != ']') {
	  last = c;
	  PATFETCH(c1);
	  range = 1;
	  goto range_retry;
	}
	else if (c == '[' && *p == ':') {
	  /* Leave room for the null.  */
	  char str[CHAR_CLASS_MAX_LENGTH + 1];

	  PATFETCH_RAW(c);
	  c1 = 0;

	  /* If pattern is `[[:'.  */
	  if (p == pend) 
	    FREE_AND_RETURN(stackb, "invalid regular expression; re can't end '[[:'");

	  for (;;) {
	    PATFETCH (c);
	    if (c == ':' || c == ']' || p == pend
		|| c1 == CHAR_CLASS_MAX_LENGTH)
	      break;
	    str[c1++] = c;
	  }
	  str[c1] = '\0';

	  /* If isn't a word bracketed by `[:' and:`]':
	     undo the ending character, the letters, and leave 
	     the leading `:' and `[' (but set bits for them).  */
	  if (c == ':' && *p == ']') {
	    int ch;
	    char is_alnum = STREQ(str, "alnum");
	    char is_alpha = STREQ(str, "alpha");
	    char is_blank = STREQ(str, "blank");
	    char is_cntrl = STREQ(str, "cntrl");
	    char is_digit = STREQ(str, "digit");
	    char is_graph = STREQ(str, "graph");
	    char is_lower = STREQ(str, "lower");
	    char is_print = STREQ(str, "print");
	    char is_punct = STREQ(str, "punct");
	    char is_space = STREQ(str, "space");
	    char is_upper = STREQ(str, "upper");
	    char is_xdigit = STREQ(str, "xdigit");

	    if (!IS_CHAR_CLASS(str)){
	      snprintf(error_msg, ERROR_MSG_MAX_SIZE, 
		       "invalid regular expression; [:%s:] is not a character class", str);
	      FREE_AND_RETURN(stackb, error_msg);
	    }

	    /* Throw away the ] at the end of the character class.  */
	    PATFETCH(c);

	    if (p == pend) 
	      FREE_AND_RETURN(stackb, "invalid regular expression; range doesn't have ending ']' after a character class");

	    for (ch = 0; ch < 1 << MBRE_BYTEWIDTH; ch++) {
	      if (   (is_alnum  && ISALNUM(ch))
		  || (is_alpha  && ISALPHA(ch))
		  || (is_blank  && ISBLANK(ch))
		  || (is_cntrl  && ISCNTRL(ch))
		  || (is_digit  && ISDIGIT(ch))
		  || (is_graph  && ISGRAPH(ch))
		  || (is_lower  && ISLOWER(ch))
		  || (is_print  && ISPRINT(ch))
		  || (is_punct  && ISPUNCT(ch))
		  || (is_space  && ISSPACE(ch))
		  || (is_upper  && ISUPPER(ch))
		  || (is_xdigit && ISXDIGIT(ch)))
		SET_LIST_BIT(ch);
	    }
	    had_char_class = 1;
	  }
	  else {
	    c1++;
	    while (c1--)    
	      PATUNFETCH;
	    SET_LIST_BIT(TRANSLATE_P()?translate['[']:'[');
	    SET_LIST_BIT(TRANSLATE_P()?translate[':']:':');
	    had_char_class = 0;
	    last = ':';
	  }
	}
	else if (had_mbchar == 0 && (!current_mbctype || !had_num_literal)) {
	  SET_LIST_BIT(c);
	  had_num_literal = 0;
	}
	else
	  set_list_bits(c, c, b);
	had_mbchar = 0;
      }

      /* Discard any character set/class bitmap bytes that are all
	 0 at the end of the map. Decrement the map-length byte too.  */
      while ((int)b[-1] > 0 && b[(int)b[-1] - 1] == 0) 
	b[-1]--; 
      if (b[-1] != (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH)
	memmove(&b[(int)b[-1]], &b[(1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH],
		2 + EXTRACT_UNSIGNED(&b[(1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH])*8);
      b += b[-1] + 2 + EXTRACT_UNSIGNED(&b[(int)b[-1]])*8;
      break;

    case '(':
      {
	int old_options = options;
	int push_option = 0;
	int casefold = 0;

      PATFETCH(c);
      if (c == '?') {
	int negative = 0;

	PATFETCH_RAW(c);
	switch (c) {
	case 'x': case 'p': case 'm': case 'i': case '-':
	  for (;;) {
	    switch (c) {
	    case '-':
	      negative = 1;
	      break;

	    case ':':
	    case ')':
	      break;

	    case 'x':
	      if (negative)
		options &= ~MBRE_OPTION_EXTENDED;
	      else
		options |= MBRE_OPTION_EXTENDED;
	      break;

	    case 'p':
	      if (negative) {
		if ((options&MBRE_OPTION_POSIXLINE) == MBRE_OPTION_POSIXLINE) {
		  options &= ~MBRE_OPTION_POSIXLINE;
		}
	      }
	      else if ((options&MBRE_OPTION_POSIXLINE) != MBRE_OPTION_POSIXLINE) {
		options |= MBRE_OPTION_POSIXLINE;
	      }
	      push_option = 1;
	      break;

	    case 'm':
	      if (negative) {
		if (options&MBRE_OPTION_MULTILINE) {
		  options &= ~MBRE_OPTION_MULTILINE;
		}
	      }
	      else if (!(options&MBRE_OPTION_MULTILINE)) {
		options |= MBRE_OPTION_MULTILINE;
	      }
	      push_option = 1;
	      break;

	    case 'i':
	      if (negative) {
		if (options&MBRE_OPTION_IGNORECASE) {
		  options &= ~MBRE_OPTION_IGNORECASE;
		}
	      }
	      else if (!(options&MBRE_OPTION_IGNORECASE)) {
		options |= MBRE_OPTION_IGNORECASE;
	      }
		casefold = 1;
	      break;

	    default:
	      FREE_AND_RETURN(stackb, "undefined (?...) inline option");
	    }
	    if (c == ')') {
	      c = '#';	/* read whole in-line options */
	      break;
	    }
	    if (c == ':') break;
	    PATFETCH_RAW(c);
	  }
	  break;

	case '#':
	  for (;;) {
	    PATFETCH(c);
	    if (c == ')') break;
	  }
	  c = '#';
	  break;

	case ':':
	case '=':
	case '!':
	case '>':
	  break;

	default:
	  FREE_AND_RETURN(stackb, "undefined (?...) sequence");
	}
	}
	else {
	  PATUNFETCH;
	  c = '(';
	}
	if (c == '#') {
	if (push_option) {
	  BUFPUSH(option_set);
	  BUFPUSH(options);
	}
	  if (casefold) {
	    if (options & MBRE_OPTION_IGNORECASE)
	      BUFPUSH(casefold_on);
	    else
	      BUFPUSH(casefold_off);
      }
	  break;
      }
      if (stackp+8 >= stacke) {
	DOUBLE_STACK(int);
      }

      /* Laststart should point to the start_memory that we are about
	 to push (unless the pattern has MBRE_NREGS or more ('s).  */
      /* obsolete: now MBRE_NREGS is just a default register size. */
      *stackp++ = b - bufp->buffer;    
      *stackp++ = fixup_alt_jump ? fixup_alt_jump - bufp->buffer + 1 : 0;
      *stackp++ = begalt - bufp->buffer;
      switch (c) {
      case '(':
	BUFPUSH(start_memory);
	BUFPUSH(regnum);
	*stackp++ = regnum++;
	*stackp++ = b - bufp->buffer;
	BUFPUSH(0);
	/* too many ()'s to fit in a byte. (max 254) */
	if (regnum >= MBRE_REG_MAX) goto too_big;
	break;

      case '=':
      case '!':
      case '>':
	BUFPUSH(start_nowidth);
	*stackp++ = b - bufp->buffer;
	BUFPUSH(0);	/* temporary value */
	BUFPUSH(0);
	if (c != '!') break;

	BUFPUSH(on_failure_jump);
	*stackp++ = b - bufp->buffer;
	BUFPUSH(0);	/* temporary value */
	BUFPUSH(0);
	break;

      case ':':
	BUFPUSH(start_paren);
	pending_exact = 0;
      default:
	break;
      }
	if (push_option) {
	  BUFPUSH(option_set);
	  BUFPUSH(options);
	}
	if (casefold) {
	  if (options & MBRE_OPTION_IGNORECASE)
	    BUFPUSH(casefold_on);
	  else
	    BUFPUSH(casefold_off);
	}
      *stackp++ = c;
      *stackp++ = old_options;
      fixup_alt_jump = 0;
      laststart = 0;
      begalt = b;
      }
      break;

    case ')':
      if (stackp == stackb) 
	FREE_AND_RETURN(stackb, "unmatched )");

      pending_exact = 0;
      if (fixup_alt_jump) {
	/* Push a dummy failure point at the end of the
	   alternative for a possible future
	   `finalize_jump' to pop.  See comments at
	   `push_dummy_failure' in `re_match'.  */
	BUFPUSH(push_dummy_failure);

	/* We allocated space for this jump when we assigned
	   to `fixup_alt_jump', in the `handle_alt' case below.  */
	store_jump(fixup_alt_jump, jump, b);
      }
      if (options != stackp[-1]) {
	if ((options ^ stackp[-1]) & MBRE_OPTION_IGNORECASE) {
	  BUFPUSH((options&MBRE_OPTION_IGNORECASE)?casefold_off:casefold_on);
	}
	if ((options ^ stackp[-1]) != MBRE_OPTION_IGNORECASE) {
	  BUFPUSH(option_set);
	  BUFPUSH(stackp[-1]);
	}
      }
      p0 = b;
      options = *--stackp;
      switch (c = *--stackp) {
      case '(':
	{
	  char *loc = bufp->buffer + *--stackp;
	  *loc = regnum - stackp[-1];
	  BUFPUSH(stop_memory);
	  BUFPUSH(stackp[-1]);
	  BUFPUSH(regnum - stackp[-1]);
	  stackp--;
	}
	break;

      case '!':
	BUFPUSH(pop_and_fail);
	/* back patch */
	STORE_NUMBER(bufp->buffer+stackp[-1], b - bufp->buffer - stackp[-1] - 2);
	stackp--;
	/* fall through */
      case '=':
	BUFPUSH(stop_nowidth);
	/* tell stack-pos place to start_nowidth */
	STORE_NUMBER(bufp->buffer+stackp[-1], b - bufp->buffer - stackp[-1] - 2);
	BUFPUSH(0);	/* space to hold stack pos */
	BUFPUSH(0);
	stackp--;
	break;

      case '>':
	BUFPUSH(stop_backtrack);
	/* tell stack-pos place to start_nowidth */
	STORE_NUMBER(bufp->buffer+stackp[-1], b - bufp->buffer - stackp[-1] - 2);
	BUFPUSH(0);	/* space to hold stack pos */
	BUFPUSH(0);
	stackp--;
	break;

      case ':':
	BUFPUSH(stop_paren);
	break;

      default:
	break;
      }
      begalt = *--stackp + bufp->buffer;
      stackp--;
      fixup_alt_jump = *stackp ? *stackp + bufp->buffer - 1 : 0;
      laststart = *--stackp + bufp->buffer;
      if (c == '!' || c == '=') laststart = b;
      break;

    case '|':
      /* Insert before the previous alternative a jump which
	 jumps to this alternative if the former fails.  */
      GET_BUFFER_SPACE(3);
      insert_jump(on_failure_jump, begalt, b + 6, b);
      pending_exact = 0;
      b += 3;
      /* The alternative before this one has a jump after it
	 which gets executed if it gets matched.  Adjust that
	 jump so it will jump to this alternative's analogous
	 jump (put in below, which in turn will jump to the next
	 (if any) alternative's such jump, etc.).  The last such
	 jump jumps to the correct final destination.  A picture:
	 _____ _____ 
	 |   | |   |   
	 |   v |   v 
	 a | b   | c   

	 If we are at `b', then fixup_alt_jump right now points to a
	 three-byte space after `a'.  We'll put in the jump, set
	 fixup_alt_jump to right after `b', and leave behind three
	 bytes which we'll fill in when we get to after `c'.  */

      if (fixup_alt_jump)
	store_jump(fixup_alt_jump, jump_past_alt, b);

      /* Mark and leave space for a jump after this alternative,
	 to be filled in later either by next alternative or
	 when know we're at the end of a series of alternatives.  */
      fixup_alt_jump = b;
      GET_BUFFER_SPACE(3);
      b += 3;

      laststart = 0;
      begalt = b;
      break;

    case '{':
      /* If there is no previous pattern, this is an invalid pattern.  */
      if (!laststart) {
	snprintf(error_msg, ERROR_MSG_MAX_SIZE, 
		 "invalid regular expression; there's no previous pattern, to which '{' would define cardinality at %d", 
		 p-pattern);
	FREE_AND_RETURN(stackb, error_msg);
      }
      if( p == pend)
	FREE_AND_RETURN(stackb, "invalid regular expression; '{' can't be last character" );

      beg_interval = p - 1;

      lower_bound = -1;			/* So can see if are set.  */
      upper_bound = -1;
      GET_UNSIGNED_NUMBER(lower_bound);
      if (c == ',') {
	GET_UNSIGNED_NUMBER(upper_bound);
      }
      else
	/* Interval such as `{1}' => match exactly once. */
	upper_bound = lower_bound;

      if (lower_bound < 0 || c != '}')
	goto unfetch_interval;

      if (lower_bound >= MBRE_DUP_MAX || upper_bound >= MBRE_DUP_MAX)
	FREE_AND_RETURN(stackb, "too big quantifier in {,}");
      if (upper_bound < 0) upper_bound = MBRE_DUP_MAX;
      if (lower_bound > upper_bound)
	FREE_AND_RETURN(stackb, "can't do {n,m} with n > m");

      beg_interval = 0;
      pending_exact = 0;

      greedy = 1;
      if (p != pend) {
	PATFETCH(c);
	if (c == '?') greedy = 0;
	else PATUNFETCH;
      }

      if (lower_bound == 0) {
	zero_times_ok = 1;
	if (upper_bound == MBRE_DUP_MAX) {
	  many_times_ok = 1;
	  goto repeat;
	}
	if (upper_bound == 1) {
	  many_times_ok = 0;
	  goto repeat;
	}
      }
      if (lower_bound == 1) {
	if (upper_bound == 1) {
	  /* No need to repeat */
	  break;
	}
	if (upper_bound == MBRE_DUP_MAX) {
	  many_times_ok = 1;
	  zero_times_ok = 0;
	  goto repeat;
	}
      }

      /* If upper_bound is zero, don't want to succeed at all; 
	 jump from laststart to b + 3, which will be the end of
	 the buffer after this jump is inserted.  */

      if (upper_bound == 0) {
	GET_BUFFER_SPACE(3);
	insert_jump(jump, laststart, b + 3, b);
	b += 3;
	break;
      }

      /* If lower_bound == upper_bound, repeat count can be removed */
      if (lower_bound == upper_bound) {
	int mcnt;
	int skip_stop_paren = 0;

	if (b[-1] == stop_paren) {
	  skip_stop_paren = 1;
	  b--;
	}

	if (*laststart == exactn && laststart[1]+2 == b - laststart
	    && laststart[1]*lower_bound < 256) {
	  mcnt = laststart[1];
	  GET_BUFFER_SPACE((lower_bound-1)*mcnt);
	  laststart[1] = lower_bound*mcnt;
	  while (--lower_bound) {
	    memcpy(b, laststart+2, mcnt);
	    b += mcnt;
	  }
	  if (skip_stop_paren) BUFPUSH(stop_paren);
	  break;
	}

	if (lower_bound < 5 && b - laststart < 10) {
	  /* 5 and 10 are the magic numbers */

	  mcnt = b - laststart;
	  GET_BUFFER_SPACE((lower_bound-1)*mcnt);
	  while (--lower_bound) {
	    memcpy(b, laststart, mcnt);
	    b += mcnt;
	  }
	  if (skip_stop_paren) BUFPUSH(stop_paren);
	  break;
	}
	if (skip_stop_paren) b++; /* push back stop_paren */
      }

      /* Otherwise, we have a nontrivial interval.  When
	 we're all done, the pattern will look like:
	 set_number_at <jump count> <upper bound>
	 set_number_at <succeed_n count> <lower bound>
	 succeed_n <after jump addr> <succed_n count>
	 <body of loop>
	 jump_n <succeed_n addr> <jump count>
	 (The upper bound and `jump_n' are omitted if
	 `upper_bound' is 1, though.)  */
      { /* If the upper bound is > 1, we need to insert
	   more at the end of the loop.  */
	unsigned int nbytes = (unsigned int)upper_bound == 1 ? 10 : 20;

	GET_BUFFER_SPACE(nbytes);
	/* Initialize lower bound of the `succeed_n', even
	   though it will be set during matching by its
	   attendant `set_number_at' (inserted next),
	   because `re_compile_fastmap' needs to know.
	   Jump to the `jump_n' we might insert below.  */
	insert_jump_n(succeed_n, laststart, b + (nbytes/2), 
		      b, lower_bound);
	b += 5; 	/* Just increment for the succeed_n here.  */

	/* Code to initialize the lower bound.  Insert 
	   before the `succeed_n'.  The `5' is the last two
	   bytes of this `set_number_at', plus 3 bytes of
	   the following `succeed_n'.  */
	insert_op_2(set_number_at, laststart, b, 5, lower_bound);
	b += 5;

	if (upper_bound > 1) {
	  /* More than one repetition is allowed, so
	     append a backward jump to the `succeed_n'
	     that starts this interval.

	     When we've reached this during matching,
	     we'll have matched the interval once, so
	     jump back only `upper_bound - 1' times.  */
	  GET_BUFFER_SPACE(5);
	  store_jump_n(b, greedy?jump_n:finalize_push_n, laststart + 5,
		       upper_bound - 1);
	  b += 5;

	  /* The location we want to set is the second
	     parameter of the `jump_n'; that is `b-2' as
	     an absolute address.  `laststart' will be
	     the `set_number_at' we're about to insert;
	     `laststart+3' the number to set, the source
	     for the relative address.  But we are
	     inserting into the middle of the pattern --
	     so everything is getting moved up by 5.
	     Conclusion: (b - 2) - (laststart + 3) + 5,
	     i.e., b - laststart.

	     We insert this at the beginning of the loop
	     so that if we fail during matching, we'll
	     reinitialize the bounds.  */
	  insert_op_2(set_number_at, laststart, b, b - laststart,
		      upper_bound - 1);
	  b += 5;
	}
      }
      break;

    unfetch_interval:
      /* If an invalid interval, match the characters as literals.  */
      p = beg_interval;
      beg_interval = 0;

      /* normal_char and normal_backslash need `c'.  */
      PATFETCH(c);	
      goto normal_char;

    case '\\':
      if (p == pend)
	FREE_AND_RETURN(stackb, "invalid regular expression; '\\' can't be last character");
      /* Do not translate the character after the \, so that we can
	 distinguish, e.g., \B from \b, even if we normally would
	 translate, e.g., B to b.  */
      PATFETCH_RAW(c);
      switch (c) {
      case 's':
      case 'S':
      case 'd':
      case 'D':
	while (b - bufp->buffer + 9 + (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH
	       > bufp->allocated)
	  EXTEND_BUFFER;

	laststart = b;
	if (c == 's' || c == 'd') {
	  BUFPUSH(charset);
	}
	else {
	  BUFPUSH(charset_not);
	}

	BUFPUSH((1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH);
	memset(b, 0, (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH + 2);
	if (c == 's' || c == 'S') {
	  SET_LIST_BIT(' ');
	  SET_LIST_BIT('\t');
	  SET_LIST_BIT('\n');
	  SET_LIST_BIT('\r');
	  SET_LIST_BIT('\f');
	}
	else {
	  char cc;

	  for (cc = '0'; cc <= '9'; cc++) {
	    SET_LIST_BIT(cc);
	  }
	}

	while ((int)b[-1] > 0 && b[b[-1] - 1] == 0) 
	  b[-1]--; 
	if (b[-1] != (1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH)
	  memmove(&b[(int)b[-1]], &b[(1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH],
		  2 + EXTRACT_UNSIGNED(&b[(1 << MBRE_BYTEWIDTH) / MBRE_BYTEWIDTH])*8);
	b += b[-1] + 2 + EXTRACT_UNSIGNED(&b[(int)b[-1]])*8;
	break;

      case 'w':
	laststart = b;
	BUFPUSH(wordchar);
	break;

      case 'W':
	laststart = b;
	BUFPUSH(notwordchar);
	break;

#ifndef RUBY
      case '<':
	BUFPUSH(wordbeg);
	break;

      case '>':
	BUFPUSH(wordend);
	break;
#endif

      case 'b':
	BUFPUSH(wordbound);
	break;

      case 'B':
	BUFPUSH(notwordbound);
	break;

      case 'A':
	BUFPUSH(begbuf);
	break;

      case 'Z':
	if ((bufp->options & MBRE_OPTION_SINGLELINE) == 0) {
	  BUFPUSH(endbuf2);
	  break;
	}
	/* fall through */
      case 'z':
	BUFPUSH(endbuf);
	break;

      case 'G':
	BUFPUSH(begpos);
	break;

	/* hex */
      case 'x':
	had_mbchar = 0;
	c = scan_hex(p, 2, &numlen);
	p += numlen;
	had_num_literal = 1;
	goto numeric_char;

	/* octal */
      case '0':
	had_mbchar = 0;
	c = scan_oct(p, 3, &numlen);
	p += numlen;
	had_num_literal = 1;
	goto numeric_char;

	/* back-ref or octal */
      case '1': case '2': case '3':
      case '4': case '5': case '6':
      case '7': case '8': case '9':
	  PATUNFETCH;
	p0 = p;

	  had_mbchar = 0;
	  c1 = 0;
	  GET_UNSIGNED_NUMBER(c1);
	  if (!ISDIGIT(c)) PATUNFETCH;

	if (9 < c1 && c1 >= regnum) {
	    /* need to get octal */
	  c = scan_oct(p0, 3, &numlen) & 0xff;
	  p = p0 + numlen;
	    c1 = 0;
	    had_num_literal = 1;
	    goto numeric_char;
	  }

	laststart = b;
	BUFPUSH(duplicate);
	BUFPUSH(c1);
	break;

      case 'M':
      case 'C':
      case 'c':
	p0 = --p;
	c = read_special(p, pend, &p0);
	if (c > 255) goto invalid_escape;
	p = p0;
	had_num_literal = 1;
	goto numeric_char;

      default:
	c = read_backslash(c);
	goto normal_char;
      }
      break;

    case '#':
      if (options & MBRE_OPTION_EXTENDED) {
	while (p != pend) {
	  PATFETCH(c);
	  if (c == '\n') break;
	}
	break;
      }
      goto normal_char;

    case ' ':
    case '\t':
    case '\f':
    case '\r':
    case '\n':
      if (options & MBRE_OPTION_EXTENDED)
	break;

    default:
    normal_char:		/* Expects the character in `c'.  */
      had_mbchar = 0;
      if (ismbchar(c)) {
	had_mbchar = 1;
	c1 = p - pattern;
      }
    numeric_char:
      nextp = p + mbclen(c) - 1;
      if (!pending_exact || pending_exact + *pending_exact + 1 != b
	  || *pending_exact >= (c1 ? 0176 : 0177)
	  || *nextp == '+' || *nextp == '?'
	  || *nextp == '*' || *nextp == '^'
	  || *nextp == '{') {
	laststart = b;
	BUFPUSH(exactn);
	pending_exact = b;
	BUFPUSH(0);
      }
      if (had_num_literal || c == 0xff) {
	BUFPUSH(0xff);
	(*pending_exact)++;
	had_num_literal = 0;
      }
      BUFPUSH(c);
      (*pending_exact)++;
      if (had_mbchar) {
	int len = mbclen(c) - 1;
	while (len--) {
	  PATFETCH_RAW(c);
	  BUFPUSH(c);
	  (*pending_exact)++;
	}
      }
    }
  }

  if (fixup_alt_jump)
    store_jump(fixup_alt_jump, jump, b);

  if (stackp != stackb)
    FREE_AND_RETURN(stackb, "unmatched (");

  /* set optimize flags */
  laststart = bufp->buffer;
  if (laststart != b) {
    if (*laststart == start_memory) laststart += 3;
    if (*laststart == dummy_failure_jump) laststart += 3;
    else if (*laststart == try_next) laststart += 3;
    if (*laststart == anychar_repeat) {
      bufp->options |= MBRE_OPTIMIZE_ANCHOR;
    }
    else if (*laststart == on_failure_jump) {
      int mcnt;

      laststart++;
      EXTRACT_NUMBER_AND_INCR(mcnt, laststart);
      if (*laststart == charset || *laststart == charset_not) {
	p0 = laststart;
	mcnt = *++p0;
	p0 += mcnt+1;
	mcnt = EXTRACT_UNSIGNED_AND_INCR(p0);
	p0 += 8*mcnt;
	if (*p0 == maybe_finalize_jump) {
	  bufp->stclass = laststart;
	}
      }
    }
  }

  bufp->used = b - bufp->buffer;
  bufp->re_nsub = regnum;
  laststart = bufp->buffer;
  if (laststart != b) {
    if (*laststart == start_memory) laststart += 3;
    if (*laststart == exactn) {
      bufp->options |= MBRE_OPTIMIZE_EXACTN;
      bufp->must = laststart+1;
    }
  }
  if (!bufp->must) {
    bufp->must = calculate_must_string(bufp->buffer, b);
  }
  if (current_mbctype == MBCTYPE_SJIS) bufp->options |= MBRE_OPTIMIZE_NO_BM;
  else if (bufp->must) {
    int i;
    int len = (unsigned char)bufp->must[0];

    for (i=1; i<len; i++) {
      if ((unsigned char)bufp->must[i] == 0xff ||
	  (current_mbctype && ismbchar(bufp->must[i]))) {
	bufp->options |= MBRE_OPTIMIZE_NO_BM;
	break;
      }
    }
    if (!(bufp->options & MBRE_OPTIMIZE_NO_BM)) {
      bufp->must_skip = (int *) xmalloc((1 << MBRE_BYTEWIDTH)*sizeof(int));
      bm_init_skip(bufp->must_skip, (unsigned char*)bufp->must+1,
		   (unsigned char)bufp->must[0],
		   (unsigned char*)(MAY_TRANSLATE()?translate:0));
    }
  }

  bufp->regstart = TMALLOC(regnum, unsigned char*);
  bufp->regend = TMALLOC(regnum, unsigned char*);
  bufp->old_regstart = TMALLOC(regnum, unsigned char*);
  bufp->old_regend = TMALLOC(regnum, unsigned char*);
  bufp->reg_info = TMALLOC(regnum, mbre_register_info_type);
  bufp->best_regstart = TMALLOC(regnum, unsigned char*);
  bufp->best_regend = TMALLOC(regnum, unsigned char*);
  FREE_AND_RETURN(stackb, 0);

 invalid_pattern:
  FREE_AND_RETURN(stackb, "invalid regular expression");

 end_of_pattern:
  FREE_AND_RETURN(stackb, "premature end of regular expression");

 too_big:
  FREE_AND_RETURN(stackb, "regular expression too big");

 memory_exhausted:
  FREE_AND_RETURN(stackb, "memory exhausted");

 nested_meta:
  FREE_AND_RETURN(stackb, "nested *?+ in regexp");

 invalid_escape:
  FREE_AND_RETURN(stackb, "Invalid escape character syntax");
}

void
re_free_pattern(bufp)
     struct mbre_pattern_buffer *bufp;
{
  if(bufp){
    if (bufp->buffer) xfree(bufp->buffer);
    if (bufp->fastmap) xfree(bufp->fastmap);
    if (bufp->must_skip) xfree(bufp->must_skip);

    if (bufp->regstart) xfree(bufp->regstart);
    if (bufp->regend) xfree(bufp->regend);
    if (bufp->old_regstart) xfree(bufp->old_regstart);
    if (bufp->old_regend) xfree(bufp->old_regend);
    if (bufp->best_regstart) xfree(bufp->best_regstart);
    if (bufp->best_regend) xfree(bufp->best_regend);
    if (bufp->reg_info) xfree(bufp->reg_info);
  }
}

/* Store a jump of the form <OPCODE> <relative address>.
   Store in the location FROM a jump operation to jump to relative
   address FROM - TO.  OPCODE is the opcode to store.  */

static void
store_jump(from, opcode, to)
     char *from, *to;
     int opcode;
{
  from[0] = (char)opcode;
  STORE_NUMBER(from + 1, to - (from + 3));
}


/* Open up space before char FROM, and insert there a jump to TO.
   CURRENT_END gives the end of the storage not in use, so we know 
   how much data to copy up. OP is the opcode of the jump to insert.

   If you call this function, you must zero out pending_exact.  */

static void
insert_jump(op, from, to, current_end)
     int op;
     char *from, *to, *current_end;
{
  register char *pfrom = current_end;		/* Copy from here...  */
  register char *pto = current_end + 3;		/* ...to here.  */

  while (pfrom != from)			       
    *--pto = *--pfrom;
  store_jump(from, op, to);
}


/* Store a jump of the form <opcode> <relative address> <n> .

   Store in the location FROM a jump operation to jump to relative
   address FROM - TO.  OPCODE is the opcode to store, N is a number the
   jump uses, say, to decide how many times to jump.

   If you call this function, you must zero out pending_exact.  */

static void
store_jump_n(from, opcode, to, n)
     char *from, *to;
     int opcode;
     unsigned n;
{
  from[0] = (char)opcode;
  STORE_NUMBER(from + 1, to - (from + 3));
  STORE_NUMBER(from + 3, n);
}


/* Similar to insert_jump, but handles a jump which needs an extra
   number to handle minimum and maximum cases.  Open up space at
   location FROM, and insert there a jump to TO.  CURRENT_END gives the
   end of the storage in use, so we know how much data to copy up. OP is
   the opcode of the jump to insert.

   If you call this function, you must zero out pending_exact.  */

static void
insert_jump_n(op, from, to, current_end, n)
     int op;
     char *from, *to, *current_end;
     unsigned n;
{
  register char *pfrom = current_end;		/* Copy from here...  */
  register char *pto = current_end + 5;		/* ...to here.  */

  while (pfrom != from)			       
    *--pto = *--pfrom;
  store_jump_n(from, op, to, n);
}


/* Open up space at location THERE, and insert operation OP.
   CURRENT_END gives the end of the storage in use, so
   we know how much data to copy up.

   If you call this function, you must zero out pending_exact.  */

#if 0
static void
insert_op(op, there, current_end)
     int op;
     char *there, *current_end;
{
  register char *pfrom = current_end;		/* Copy from here...  */
  register char *pto = current_end + 1;		/* ...to here.  */

  while (pfrom != there)			       
    *--pto = *--pfrom;

  there[0] = (char)op;
}
#endif

/* Open up space at location THERE, and insert operation OP followed by
   NUM_1 and NUM_2.  CURRENT_END gives the end of the storage in use, so
   we know how much data to copy up.

   If you call this function, you must zero out pending_exact.  */

static void
insert_op_2(op, there, current_end, num_1, num_2)
     int op;
     char *there, *current_end;
     int num_1, num_2;
{
  register char *pfrom = current_end;		/* Copy from here...  */
  register char *pto = current_end + 5;		/* ...to here.  */

  while (pfrom != there)			       
    *--pto = *--pfrom;

  there[0] = (char)op;
  STORE_NUMBER(there + 1, num_1);
  STORE_NUMBER(there + 3, num_2);
}


#define trans_eq(c1, c2, translate) (translate?(translate[c1]==translate[c2]):((c1)==(c2)))
static int
slow_match(little, lend, big, bend, translate)
     unsigned char *little, *lend;
     unsigned char *big, *bend;
     unsigned char *translate;
{
  int c;

  while (little < lend && big < bend) {
    c = *little++;
    if (c == 0xff)
      c = *little++;
    if (!trans_eq(*big++, c, translate)) break;
  }
  if (little == lend) return 1;
  return 0;
}

static int
slow_search(little, llen, big, blen, translate, re_mbctab)
     unsigned char *little;
     int llen;
     unsigned char *big;
     int blen;
     char *translate;
     const unsigned char *re_mbctab;
{
  unsigned char *bsave = big;
  unsigned char *bend = big + blen;
  register int c;
  int fescape = 0;

  c = *little;
  if (c == 0xff) {
    c = little[1];
    fescape = 1;
  }
  else if (translate && !ismbchar(c)) {
    c = translate[c];
  }

  while (big < bend) {
    /* look for first character */
    if (fescape) {
      while (big < bend) {
	if (*big == c) break;
	big++;
      }
    }
    else if (translate && !ismbchar(c)) {
      while (big < bend) {
	if (ismbchar(*big)) big+=mbclen(*big)-1;
	else if (translate[*big] == c) break;
	big++;
      }
    }
    else {
      while (big < bend) {
	if (*big == c) break;
	if (ismbchar(*big)) big+=mbclen(*big)-1;
	big++;
      }
    }

    if (slow_match(little, little+llen, big, bend, translate))
      return big - bsave;

    big+=mbclen(*big);
  }
  return -1;
}

static void
bm_init_skip(skip, pat, m, translate)
     int *skip;
     unsigned char *pat;
     int m;
     const unsigned char *translate;
{
  int j, c;

  for (c=0; c<256; c++) {
    skip[c] = m;
  }
  if (translate) {
    for (j=0; j<m-1; j++) {
      skip[translate[pat[j]]] = m-1-j;
    }
  }
  else {
    for (j=0; j<m-1; j++) {
      skip[pat[j]] = m-1-j;
    }
  }
}

static int
bm_search(little, llen, big, blen, skip, translate)
     unsigned char *little;
     int llen;
     unsigned char *big;
     int blen;
     int *skip;
     unsigned char *translate;
{
  int i, j, k;

  i = llen-1;
  if (translate) {
    while (i < blen) {
      k = i;
      j = llen-1;
      while (j >= 0 && translate[big[k]] == translate[little[j]]) {
	k--;
	j--;
      }
      if (j < 0) return k+1;

      i += skip[translate[big[i]]];
    }
    return -1;
  }
  while (i < blen) {
    k = i;
    j = llen-1;
    while (j >= 0 && big[k] == little[j]) {
      k--;
      j--;
    }
    if (j < 0) return k+1;

    i += skip[big[i]];
  }
  return -1;
}

/* Given a pattern, compute a fastmap from it.  The fastmap records
   which of the (1 << MBRE_BYTEWIDTH) possible characters can start a string
   that matches the pattern.  This fastmap is used by re_search to skip
   quickly over totally implausible text.

   The caller must supply the address of a (1 << MBRE_BYTEWIDTH)-byte data 
   area as bufp->fastmap.
   The other components of bufp describe the pattern to be used.  */
void
re_compile_fastmap(bufp)
     struct mbre_pattern_buffer *bufp;
{
  unsigned char *pattern = (unsigned char*)bufp->buffer;
  int size = bufp->used;
  register char *fastmap = bufp->fastmap;
  register unsigned char *p = pattern;
  register unsigned char *pend = pattern + size;
  register int j, k;
  unsigned is_a_succeed_n;

  
  unsigned char *stacka[NFAILURES];
  unsigned char **stackb = stacka;
  unsigned char **stackp = stackb;
  unsigned char **stacke = stackb + NFAILURES;
  int options = bufp->options;

  int current_mbctype = bufp->mbctype;
  const unsigned char *re_mbctab = re_mbctab_get(current_mbctype);

  memset(fastmap, 0, (1 << MBRE_BYTEWIDTH));
  bufp->fastmap_accurate = 1;
  bufp->can_be_null = 0;

  while (p) {
    is_a_succeed_n = 0;
    if (p == pend) {
      bufp->can_be_null = 1;
      break;
    }
#ifdef SWITCH_ENUM_BUG
    switch ((int)((enum regexpcode)*p++))
#else
    switch ((enum regexpcode)*p++)
#endif
      {
      case exactn:
	if (p[1] == 0xff) {
	  if (TRANSLATE_P())
	    fastmap[translate[p[2]]] = 2;
	  else
	    fastmap[p[2]] = 2;
	  bufp->options |= MBRE_OPTIMIZE_BMATCH;
	}
	else if (TRANSLATE_P())
	  fastmap[translate[p[1]]] = 1;
	else
	  fastmap[p[1]] = 1;
	break;

      case begline:
      case begbuf:
      case endbuf:
      case endbuf2:
      case wordbound:
      case notwordbound:
      case wordbeg:
      case wordend:
      case pop_and_fail:
      case push_dummy_failure:
      case start_paren:
      case stop_paren:
	continue;

      case casefold_on:
	bufp->options |= MBRE_MAY_IGNORECASE;
      case casefold_off:
	options ^= MBRE_OPTION_IGNORECASE;
	continue;

      case option_set:
	options = *p++;
	continue;

      case endline:
	if (TRANSLATE_P())
	  fastmap[translate['\n']] = 1;
	else
	  fastmap['\n'] = 1;
	if ((options & MBRE_OPTION_SINGLELINE) == 0 && bufp->can_be_null == 0)
	  bufp->can_be_null = 2;
	break;

      case jump_n:
      case finalize_jump:
      case maybe_finalize_jump:
      case jump:
      case jump_past_alt:
      case dummy_failure_jump:
      case finalize_push:
      case finalize_push_n:
	EXTRACT_NUMBER_AND_INCR(j, p);
	p += j;	
	if (j > 0)
	  continue;
	/* Jump backward reached implies we just went through
	   the body of a loop and matched nothing.
	   Opcode jumped to should be an on_failure_jump.
	   Just treat it like an ordinary jump.
	   For a * loop, it has pushed its failure point already;
	   If so, discard that as redundant.  */

	if ((enum regexpcode)*p != on_failure_jump
	    && (enum regexpcode)*p != try_next
	    && (enum regexpcode)*p != succeed_n)
	  continue;
	p++;
	EXTRACT_NUMBER_AND_INCR(j, p);
	p += j;	
	if (stackp != stackb && *stackp == p)
	  stackp--;		/* pop */
	continue;

      case try_next:
      case start_nowidth:
      case stop_nowidth:
      case stop_backtrack:
	p += 2;
	continue;

      case succeed_n:
	is_a_succeed_n = 1;
	/* Get to the number of times to succeed.  */
	EXTRACT_NUMBER(k, p + 2);
	/* Increment p past the n for when k != 0.  */
	if (k != 0) {
	  p += 4;
	  continue;
	}
	/* fall through */

      case on_failure_jump:
      EXTRACT_NUMBER_AND_INCR(j, p);
      if (p + j < pend) {
	if (stackp == stacke) {
	  EXPAND_FAIL_STACK();
	}
	*++stackp = p + j;	/* push */
      }
      else {
	bufp->can_be_null = 1;
      }
      if (is_a_succeed_n)
	EXTRACT_NUMBER_AND_INCR(k, p);	/* Skip the n.  */
      continue;

      case set_number_at:
	p += 4;
	continue;

      case start_memory:
      case stop_memory:
	p += 2;
	continue;

      case duplicate:
	bufp->can_be_null = 1;
	fastmap['\n'] = 1;
      case anychar_repeat:
      case anychar:
	for (j = 0; j < (1 << MBRE_BYTEWIDTH); j++) {
	  if (j != '\n' || (options & MBRE_OPTION_MULTILINE))
	    fastmap[j] = 1;
	}
	if (bufp->can_be_null) {
	  FREE_AND_RETURN_VOID(stackb);
	}
	/* Don't return; check the alternative paths
	   so we can set can_be_null if appropriate.  */
	if ((enum regexpcode)p[-1] == anychar_repeat) {
	    continue;
	}
	break;

      case wordchar:
	for (j = 0; j < 0x80; j++) {
	  if (SYNTAX(j) == Sword)
	    fastmap[j] = 1;
	}
	switch (current_mbctype) {
	case MBCTYPE_ASCII:
	  for (j = 0x80; j < (1 << MBRE_BYTEWIDTH); j++) {
	    if (SYNTAX(j) == Sword2)
	      fastmap[j] = 1;
	  }
	  break;
	case MBCTYPE_EUC:
	case MBCTYPE_SJIS:
	case MBCTYPE_UTF8:
	  for (j = 0x80; j < (1 << MBRE_BYTEWIDTH); j++) {
	    if (re_mbctab[j])
	      fastmap[j] = 1;
	  }
	  break;
	}
	break;

      case notwordchar:
	for (j = 0; j < 0x80; j++)
	  if (SYNTAX(j) != Sword)
	    fastmap[j] = 1;
	switch (current_mbctype) {
	case MBCTYPE_ASCII:
	  for (j = 0x80; j < (1 << MBRE_BYTEWIDTH); j++) {
	    if (SYNTAX(j) != Sword2)
	      fastmap[j] = 1;
	  }
	  break;
	case MBCTYPE_EUC:
	case MBCTYPE_SJIS:
	case MBCTYPE_UTF8:
	  for (j = 0x80; j < (1 << MBRE_BYTEWIDTH); j++) {
	    if (!re_mbctab[j])
	      fastmap[j] = 1;
	  }
	  break;
	}
	break;

      case charset:
	/* NOTE: Charset for single-byte chars never contain
	   multi-byte char.  See set_list_bits().  */
	for (j = *p++ * MBRE_BYTEWIDTH - 1; j >= 0; j--)
	  if (p[j / MBRE_BYTEWIDTH] & (1 << (j % MBRE_BYTEWIDTH))) {
	    int tmp = TRANSLATE_P()?translate[j]:j;
	    fastmap[tmp] = 1;
	  }
	{
	  unsigned short size;
	  unsigned long c, beg, end;

	  p += p[-1] + 2;
	  size = EXTRACT_UNSIGNED(&p[-2]);
	  for (j = 0; j < (int)size; j++) {
	    c = EXTRACT_MBC(&p[j*8]);
	    beg = WC2MBC1ST(c);
	    c = EXTRACT_MBC(&p[j*8+4]);
	    end = WC2MBC1ST(c);
	    /* set bits for 1st bytes of multi-byte chars.  */
	    while (beg <= end) {
	      /* NOTE: Charset for multi-byte chars might contain
		 single-byte chars.  We must reject them. */
	      if (c < 0x100) {
		fastmap[beg] = 2;
		bufp->options |= MBRE_OPTIMIZE_BMATCH;
	      }
	      else if (ismbchar(beg))
		fastmap[beg] = 1;
	      beg++;
	    }
	  }
	}
	break;

      case charset_not:
	/* S: set of all single-byte chars.
	   M: set of all first bytes that can start multi-byte chars.
	   s: any set of single-byte chars.
	   m: any set of first bytes that can start multi-byte chars.

	   We assume S+M = U.
	   ___      _   _
	   s+m = (S*s+M*m).  */
	/* Chars beyond end of map must be allowed */
	/* NOTE: Charset_not for single-byte chars might contain
	   multi-byte chars.  See set_list_bits(). */
	for (j = *p * MBRE_BYTEWIDTH; j < (1 << MBRE_BYTEWIDTH); j++)
	  if (!ismbchar(j))
	    fastmap[j] = 1;

	for (j = *p++ * MBRE_BYTEWIDTH - 1; j >= 0; j--)
	  if (!(p[j / MBRE_BYTEWIDTH] & (1 << (j % MBRE_BYTEWIDTH)))) {
	    if (!ismbchar(j))
	      fastmap[j] = 1;
	  }
	{
	  unsigned short size;
	  unsigned long c, beg;
	  int num_literal = 0;

	  p += p[-1] + 2;
	  size = EXTRACT_UNSIGNED(&p[-2]);
	  if (size == 0) {
	    for (j = 0x80; j < (1 << MBRE_BYTEWIDTH); j++)
	      if (ismbchar(j))
		fastmap[j] = 1;
	    break;
	  }
	  for (j = 0,c = 0;j < (int)size; j++) {
	    unsigned int cc = EXTRACT_MBC(&p[j*8]);
	    beg = WC2MBC1ST(cc);
	    while (c <= beg) {
	      if (ismbchar(c))
		fastmap[c] = 1;
	      c++;
	    }

	    cc = EXTRACT_MBC(&p[j*8+4]);
	    if (cc < 0xff) {
	      num_literal = 1;
	      while (c <= cc) {
		if (ismbchar(c))
		  fastmap[c] = 1;
		c++;
	      }
	    }
	    c = WC2MBC1ST(cc);
	  }

	  for (j = c; j < (1 << MBRE_BYTEWIDTH); j++) {
	    if (num_literal)
	      fastmap[j] = 1;
	    if (ismbchar(j))
	      fastmap[j] = 1;
	  }
	}
	break;

      case begpos:
      case unused:	/* pacify gcc -Wall */
	break;
      case fail:
	break;
      }

    /* Get here means we have successfully found the possible starting
       characters of one path of the pattern.  We need not follow this
       path any farther.  Instead, look at the next alternative
       remembered in the stack.  */
    if (stackp != stackb)
      p = *stackp--;		/* pop */
    else
      break;
  }
  FREE_AND_RETURN_VOID(stackb);
}

/* adjust startpos value to the position between characters. */
int
re_adjust_startpos(bufp, string, size, startpos, range)
     struct mbre_pattern_buffer *bufp;
     const char *string;
     int size, startpos, range;
{
  int current_mbctype = bufp->mbctype;
  const unsigned char *re_mbctab = re_mbctab_get(current_mbctype);

  /* Update the fastmap now if not correct already.  */
  if (!bufp->fastmap_accurate) {
    re_compile_fastmap(bufp);
  }

  /* Adjust startpos for mbc string */
  if (current_mbctype && startpos>0 && !(bufp->options&MBRE_OPTIMIZE_BMATCH)) {
    int i = 0;

    if (range > 0) {
      while (i<size) {
	i += mbclen(string[i]);
	if (startpos <= i) {
	  startpos = i;
	  break;
	}
      }
    }
    else {
      int w;

      while (i<size) {
	w = mbclen(string[i]);
	if (startpos < i + w) {
	  startpos = i;
	  break;
	}
	i += w;
      }
    }
  }
  return startpos;
}


/* Using the compiled pattern in BUFP->buffer, first tries to match
   STRING, starting first at index STARTPOS, then at STARTPOS + 1, and
   so on.  RANGE is the number of places to try before giving up.  If
   RANGE is negative, it searches backwards, i.e., the starting
   positions tried are STARTPOS, STARTPOS - 1, etc.  STRING is of SIZE.
   In REGS, return the indices of STRING that matched the entire
   BUFP->buffer and its contained subexpressions.

   The value returned is the position in the strings at which the match
   was found, or -1 if no match was found, or -2 if error (such as
   failure stack overflow).  */

int
re_search(bufp, string, size, startpos, range, regs)
     struct mbre_pattern_buffer *bufp;
     const char *string;
     int size, startpos, range;
     struct mbre_registers *regs;
{
  register char *fastmap = bufp->fastmap;
  int val, anchor = 0;

  int current_mbctype = bufp->mbctype;
  const unsigned char *re_mbctab = re_mbctab_get(current_mbctype);

  /* Check for out-of-range starting position.  */
  if (startpos < 0  ||  startpos > size)
    return -1;

  /* Update the fastmap now if not correct already.  */
  if (fastmap && !bufp->fastmap_accurate) {
    re_compile_fastmap(bufp);
  }


  /* If the search isn't to be a backwards one, don't waste time in a
     search for a pattern that must be anchored.  */
  if (bufp->used > 0) {
    switch ((enum regexpcode)bufp->buffer[0]) {
    case begbuf:
    begbuf_match:
      if (range > 0) {
	if (startpos > 0) return -1;
	else {
	  val = re_match(bufp, string, size, 0, regs);
	  if (val >= 0) return 0;
	  return val;
	}
      }
      break;

    case begline:
      anchor = 1;
      break;

    case begpos:
      val = re_match(bufp, string, size, startpos, regs);
      if (val >= 0) return startpos;
      return val;

    default:
      break;
    }
  }
  if (bufp->options & MBRE_OPTIMIZE_ANCHOR) {
    if (bufp->options&MBRE_OPTION_SINGLELINE) {
      goto begbuf_match;
    }
    anchor = 1;
  }

  if (bufp->must) {
    int len = ((unsigned char*)bufp->must)[0];
    int pos, pbeg, pend;

    pbeg = startpos;
    pend = startpos + range;
    if (pbeg > pend) {		/* swap pbeg,pend */
      pos = pend; pend = pbeg; pbeg = pos;
    }
    pend = size;
    if (bufp->options & MBRE_OPTIMIZE_NO_BM) {
      pos = slow_search(bufp->must+1, len,
			string+pbeg, pend-pbeg,
			MAY_TRANSLATE()?translate:0, re_mbctab);
    }
    else {
      pos = bm_search(bufp->must+1, len,
		      string+pbeg, pend-pbeg,
		      bufp->must_skip,
		      MAY_TRANSLATE()?translate:0);
    }
    if (pos == -1) return -1;
    if (range > 0 && (bufp->options & MBRE_OPTIMIZE_EXACTN)) {
      startpos += pos;
      range -= pos;
      if (range < 0) return -1;
    }
  }

  for (;;) {
    /* If a fastmap is supplied, skip quickly over characters that
       cannot possibly be the start of a match.  Note, however, that
       if the pattern can possibly match the null string, we must
       test it at each starting point so that we take the first null
       string we get.  */

    if (fastmap && startpos < size
	&& bufp->can_be_null != 1 && !(anchor && startpos == 0)) {
      if (range > 0) {	/* Searching forwards.  */
	register unsigned char *p, c;
	int irange = range;

	p = (unsigned char*)string+startpos;

	while (range > 0) {
	  c = *p++;
	  if (ismbchar(c)) {
	    int len;

	    if (fastmap[c])
	      break;
	    len = mbclen(c) - 1;
	    while (len--) {
	      c = *p++;
	      range--;
	      if (fastmap[c] == 2)
		goto startpos_adjust;
	    }
	  }
	  else {
	    if (fastmap[MAY_TRANSLATE() ? translate[c] : c])
	      break;
	  }
	  range--;
	}
      startpos_adjust:
	startpos += irange - range;
      }
      else {			/* Searching backwards.  */
	register unsigned char c;

	c = string[startpos];
	c &= 0xff;
	if (MAY_TRANSLATE() ? !fastmap[translate[c]] : !fastmap[c])
	  goto advance;
      }
    }

    if (startpos > size) return -1;
    if ((anchor || !bufp->can_be_null) && range > 0 && size > 0 && startpos == size)
      return -1;
    val = re_match(bufp, string, size, startpos, regs);
    if (val >= 0) return startpos;
    if (val == -2) return -2;

#ifndef NO_ALLOCA
#ifdef C_ALLOCA
    alloca(0);
#endif /* C_ALLOCA */
#endif /* NO_ALLOCA */

    if (range > 0) {
      if (anchor && startpos < size &&
	  (startpos < 1 || string[startpos-1] != '\n')) {
	while (range > 0 && string[startpos] != '\n') {
	  range--;
	  startpos++;
	}
      }
      else if (fastmap && (bufp->stclass)) {
	register unsigned char *p;
	unsigned long c;
	int irange = range;

	p = (unsigned char*)string+startpos;
	while (range > 0) {
	  c = *p++;
	  if (ismbchar(c) && fastmap[c] != 2) {
	    MBC2WC(c, p);
	  }
	  else if (MAY_TRANSLATE())
	    c = translate[c];
	  if (*bufp->stclass == charset) {
	    if (!is_in_list(c, bufp->stclass+1)) break;
	  }
	  else {
	    if (is_in_list(c, bufp->stclass+1)) break;
	  }
	  range--;
	  if (c > 256) range--;
	}
	startpos += irange - range;
      }
    }

  advance:
    if (!range) 
      break;
    else if (range > 0) {
      const char *d = string + startpos;

      if (ismbchar(*d)) {
	int len = mbclen(*d) - 1;
	range-=len, startpos+=len;
	if (!range)
	  break;
      }
      range--, startpos++;
    }
    else {
      range++, startpos--;
      {
	const char *s, *d, *p;

	s = string; d = string + startpos;
	for (p = d; p-- > s && ismbchar(*p); )
	  /* --p >= s would not work on 80[12]?86. 
	     (when the offset of s equals 0 other than huge model.)  */
	  ;
	if (!((d - p) & 1)) {
	  if (!range)
	    break;
	  range++, startpos--;
	}
      }
    }
  }
  return -1;
}




/* The following are used for re_match, defined below:  */

/* Accessing macros used in re_match: */

#define IS_ACTIVE(R)  ((R).bits.is_active)
#define MATCHED_SOMETHING(R)  ((R).bits.matched_something)


/* Macros used by re_match:  */

/* I.e., regstart, regend, and reg_info.  */
#define NUM_REG_ITEMS  3

/* I.e., ptr and count.  */
#define NUM_COUNT_ITEMS 2

/* Individual items aside from the registers.  */
#define NUM_NONREG_ITEMS 4

/* We push at most this many things on the stack whenever we
   fail.  The `+ 2' refers to PATTERN_PLACE and STRING_PLACE, which are
   arguments to the PUSH_FAILURE_POINT macro.  */
#define MAX_NUM_FAILURE_ITEMS   (num_regs * NUM_REG_ITEMS + NUM_NONREG_ITEMS)

/* We push this many things on the stack whenever we fail.  */
#define NUM_FAILURE_ITEMS  (last_used_reg * NUM_REG_ITEMS + NUM_NONREG_ITEMS + 1)

/* This pushes counter information for succeed_n and jump_n */
#define PUSH_FAILURE_COUNT(ptr)						\
  do {									\
    int c;								\
    EXTRACT_NUMBER(c, ptr);						\
    ENSURE_FAIL_STACK(NUM_COUNT_ITEMS);					\
    *stackp++ = (unsigned char*)(long)c;				\
    *stackp++ = (ptr);							\
    num_failure_counts++;						\
  } while (0)

/* This pushes most of the information about the current state we will want
   if we ever fail back to it.  */

#define PUSH_FAILURE_POINT(pattern_place, string_place)			\
  do {									\
    long last_used_reg, this_reg;					\
									\
    /* Find out how many registers are active or have been matched.	\
       (Aside from register zero, which is only set at the end.) */	\
    for (last_used_reg = num_regs-1; last_used_reg > 0; last_used_reg--)\
      if (!REG_UNSET(regstart[last_used_reg]))				\
        break;								\
									\
    ENSURE_FAIL_STACK(NUM_FAILURE_ITEMS);				\
    *stackp++ = (unsigned char*)(long)num_failure_counts;		\
    num_failure_counts = 0;						\
									\
    /* Now push the info for each of those registers.  */		\
    for (this_reg = 1; this_reg <= last_used_reg; this_reg++) {		\
      *stackp++ = regstart[this_reg];					\
      *stackp++ = regend[this_reg];					\
      *stackp++ = reg_info[this_reg].word;				\
    }									\
									\
    /* Push how many registers we saved.  */				\
    *stackp++ = (unsigned char*)last_used_reg;				\
									\
    *stackp++ = pattern_place;                                          \
    *stackp++ = string_place;                                           \
    *stackp++ = (unsigned char*)options; /* current option status */	\
    *stackp++ = (unsigned char*)0; /* non-greedy flag */		\
  } while(0)

#define NON_GREEDY ((unsigned char*)1)

#define POP_FAILURE_COUNT()						\
  do {									\
    unsigned char *ptr = *--stackp;					\
    int count = (long)*--stackp;					\
    STORE_NUMBER(ptr, count);						\
  } while (0)

/* This pops what PUSH_FAILURE_POINT pushes.  */

#define POP_FAILURE_POINT()						\
  do {									\
    long temp;								\
    stackp -= NUM_NONREG_ITEMS;	/* Remove failure points (and flag). */	\
    temp = (long)*--stackp;	/* How many regs pushed.  */	        \
    temp *= NUM_REG_ITEMS;	/* How much to take off the stack.  */	\
    stackp -= temp; 		/* Remove the register info.  */	\
    temp = (long)*--stackp;	/* How many counters pushed.  */	\
    while (temp--) {							\
      POP_FAILURE_COUNT();      /* Remove the counter info.  */		\
    }									\
    num_failure_counts = 0;	/* Reset num_failure_counts.  */	\
  } while(0)

     /* Registers are set to a sentinel when they haven't yet matched.  */
#define REG_UNSET_VALUE ((unsigned char*)-1)
#define REG_UNSET(e) ((e) == REG_UNSET_VALUE)

#define PREFETCH if (d == dend) goto fail

     /* Call this when have matched something; it sets `matched' flags for the
   registers corresponding to the subexpressions of which we currently
   are inside.  */
#define SET_REGS_MATCHED 						\
  do { unsigned this_reg;						\
    for (this_reg = 0; this_reg < num_regs; this_reg++) { 		\
        if (IS_ACTIVE(reg_info[this_reg]))				\
          MATCHED_SOMETHING(reg_info[this_reg]) = 1;			\
        else								\
          MATCHED_SOMETHING(reg_info[this_reg]) = 0;			\
      } 								\
  } while(0)

#define AT_STRINGS_BEG(d)  ((d) == string)
#define AT_STRINGS_END(d)  ((d) == dend)

#define IS_A_LETTER(d) (SYNTAX(*(d)) == Sword ||			\
			(current_mbctype ?				\
			 (re_mbctab[*(d)] && ((d)+mbclen(*(d)))<=dend):	\
			 SYNTAX(*(d)) == Sword2))

#define PREV_IS_A_LETTER(d) ((current_mbctype == MBCTYPE_SJIS)?		\
			     IS_A_LETTER((d)-(!AT_STRINGS_BEG((d)-1)&&	\
					      ismbchar((d)[-2])?2:1)):	\
                             ((current_mbctype && ((d)[-1] >= 0x80)) ||	\
			      IS_A_LETTER((d)-1)))

static void
init_regs(regs, num_regs)
     struct mbre_registers *regs;
     unsigned int num_regs;
{
  int i;

  regs->num_regs = num_regs;
  if (num_regs < MBRE_NREGS)
    num_regs = MBRE_NREGS;

  if (regs->allocated == 0) {
    regs->beg = TMALLOC(num_regs, int);
    regs->end = TMALLOC(num_regs, int);
    regs->allocated = num_regs;
  }
  else if (regs->allocated < (int)num_regs) {
    TREALLOC(regs->beg, num_regs, int);
    TREALLOC(regs->end, num_regs, int);
    regs->allocated = num_regs;
  }
  for (i=0; i<(int)num_regs; i++) {
    regs->beg[i] = regs->end[i] = -1;
  }
}

/* Match the pattern described by BUFP against STRING, which is of
   SIZE.  Start the match at index POS in STRING.  In REGS, return the
   indices of STRING that matched the entire BUFP->buffer and its
   contained subexpressions.

   If bufp->fastmap is nonzero, then it had better be up to date.

   The reason that the data to match are specified as two components
   which are to be regarded as concatenated is so this function can be
   used directly on the contents of an Emacs buffer.

   -1 is returned if there is no match.  -2 is returned if there is an
   error (such as match stack overflow).  Otherwise the value is the
   length of the substring which was matched.  */

int
re_match(bufp, string_arg, size, pos, regs)
     struct mbre_pattern_buffer *bufp;
     const char *string_arg;
     int size, pos;
     struct mbre_registers *regs;
{
  register unsigned char *p = (unsigned char*)bufp->buffer;
  unsigned char *p1;

  /* Pointer to beyond end of buffer.  */
  register unsigned char *pend = p + bufp->used;

  unsigned num_regs = bufp->re_nsub;

  unsigned char *string = (unsigned char*)string_arg;

  register unsigned char *d, *dend;
  register int mcnt;			/* Multipurpose.  */
  int options = bufp->options;

  /* Failure point stack.  Each place that can handle a failure further
     down the line pushes a failure point on this stack.  It consists of
     restart, regend, and reg_info for all registers corresponding to the
     subexpressions we're currently inside, plus the number of such
     registers, and, finally, two char *'s.  The first char * is where to
     resume scanning the pattern; the second one is where to resume
     scanning the strings.  If the latter is zero, the failure point is a
     ``dummy''; if a failure happens and the failure point is a dummy, it
     gets discarded and the next next one is tried.  */

  unsigned char **stacka;
  unsigned char **stackb;
  unsigned char **stackp;
  unsigned char **stacke;

  /* Information on the contents of registers. These are pointers into
     the input strings; they record just what was matched (on this
     attempt) by a subexpression part of the pattern, that is, the
     regnum-th regstart pointer points to where in the pattern we began
     matching and the regnum-th regend points to right after where we
     stopped matching the regnum-th subexpression.  (The zeroth register
     keeps track of what the whole pattern matches.)  */

  unsigned char **regstart = bufp->regstart;
  unsigned char **regend = bufp->regend;

  /* If a group that's operated upon by a repetition operator fails to
     match anything, then the register for its start will need to be
     restored because it will have been set to wherever in the string we
     are when we last see its open-group operator.  Similarly for a
     register's end.  */
  unsigned char **old_regstart = bufp->old_regstart;
  unsigned char **old_regend = bufp->old_regend;

  /* The is_active field of reg_info helps us keep track of which (possibly
     nested) subexpressions we are currently in. The matched_something
     field of reg_info[reg_num] helps us tell whether or not we have
     matched any of the pattern so far this time through the reg_num-th
     subexpression.  These two fields get reset each time through any
     loop their register is in.  */

  mbre_register_info_type *reg_info = bufp->reg_info;

  /* The following record the register info as found in the above
     variables when we find a match better than any we've seen before. 
     This happens as we backtrack through the failure points, which in
     turn happens only if we have not yet matched the entire string.  */

  unsigned best_regs_set = 0;
  unsigned char **best_regstart = bufp->best_regstart;
  unsigned char **best_regend = bufp->best_regend;

  int num_failure_counts = 0;

  int current_mbctype = bufp->mbctype;
  const unsigned char *re_mbctab = re_mbctab_get(current_mbctype);

  if (regs) {
    init_regs(regs, num_regs);
  }

  /* Initialize the stack. */
  stacka = RE_TALLOC(MAX_NUM_FAILURE_ITEMS * NFAILURES, unsigned char*);
  stackb = stacka;
  stackp = stackb;
  stacke = &stackb[MAX_NUM_FAILURE_ITEMS * NFAILURES];

#ifdef DEBUG_REGEX
  fprintf(stderr, "Entering re_match(%s)\n", string_arg);
#endif

  /* Initialize subexpression text positions to -1 to mark ones that no
     ( or ( and ) or ) has been seen for. Also set all registers to
     inactive and mark them as not having matched anything or ever
     failed. */
  for (mcnt = 0; mcnt < (int)num_regs; mcnt++) {
    regstart[mcnt] = regend[mcnt]
      = old_regstart[mcnt] = old_regend[mcnt]
      = best_regstart[mcnt] = best_regend[mcnt] = REG_UNSET_VALUE;
#ifdef __CHECKER__
    reg_info[mcnt].word = 0;
#endif
    IS_ACTIVE (reg_info[mcnt]) = 0;
    MATCHED_SOMETHING (reg_info[mcnt]) = 0;
  }

  /* Set up pointers to ends of strings.
     Don't allow the second string to be empty unless both are empty.  */


  /* `p' scans through the pattern as `d' scans through the data. `dend'
     is the end of the input string that `d' points within. `d' is
     advanced into the following input string whenever necessary, but
     this happens before fetching; therefore, at the beginning of the
     loop, `d' can be pointing at the end of a string, but it cannot
     equal string2.  */

  d = string + pos, dend = string + size;

  /* This loops over pattern commands.  It exits by returning from the
     function if match is complete, or it drops through if match fails
     at this starting point in the input data.  */

  for (;;) {
#ifdef DEBUG_REGEX
    fprintf(stderr,
	    "regex loop(%d):  matching 0x%02d\n",
	    p - (unsigned char*)bufp->buffer,
	    *p);
#endif
    /* End of pattern means we might have succeeded.  */
    if (p == pend) {
      /* If not end of string, try backtracking.  Otherwise done.  */
      if ((bufp->options & MBRE_OPTION_LONGEST) && d != dend) {
	if (best_regs_set) /* non-greedy, no need to backtrack */
	  goto restore_best_regs;
	while (stackp != stackb && stackp[-1] == NON_GREEDY) {
	  if (best_regs_set) /* non-greedy, no need to backtrack */
	    goto restore_best_regs;
	  POP_FAILURE_POINT();
	}
	if (stackp != stackb) {
	  /* More failure points to try.  */

	  /* If exceeds best match so far, save it.  */
	  if (! best_regs_set || (d > best_regend[0])) {
	    best_regs_set = 1;
	    best_regend[0] = d;	/* Never use regstart[0].  */

	    for (mcnt = 1; mcnt < (int)num_regs; mcnt++) {
	      best_regstart[mcnt] = regstart[mcnt];
	      best_regend[mcnt] = regend[mcnt];
	    }
	  }
	  goto fail;	       
	}
	/* If no failure points, don't restore garbage.  */
	else if (best_regs_set) {
	restore_best_regs:
	  /* Restore best match.  */
	  d = best_regend[0];

	  for (mcnt = 0; mcnt < (int)num_regs; mcnt++) {
	    regstart[mcnt] = best_regstart[mcnt];
	    regend[mcnt] = best_regend[mcnt];
	  }
	}
      }

      /* If caller wants register contents data back, convert it 
	 to indices.  */
      if (regs) {
	regs->beg[0] = pos;
	regs->end[0] = d - string;
	for (mcnt = 1; mcnt < (int)num_regs; mcnt++) {
	  if (REG_UNSET(regend[mcnt])) {
	    regs->beg[mcnt] = -1;
	    regs->end[mcnt] = -1;
	    continue;
	  }
	  regs->beg[mcnt] = regstart[mcnt] - string;
	  regs->end[mcnt] = regend[mcnt] - string;
	}
      }
      FREE_AND_RETURN(stackb, (d - pos - string));
    }

    /* Otherwise match next pattern command.  */
#ifdef SWITCH_ENUM_BUG
    switch ((int)((enum regexpcode)*p++))
#else
    switch ((enum regexpcode)*p++)
#endif
      {
	/* ( [or `(', as appropriate] is represented by start_memory,
	   ) by stop_memory.  Both of those commands are followed by
	   a register number in the next byte.  The text matched
	   within the ( and ) is recorded under that number.  */
      case start_memory:
	old_regstart[*p] = regstart[*p];
	regstart[*p] = d;
	IS_ACTIVE(reg_info[*p]) = 1;
	MATCHED_SOMETHING(reg_info[*p]) = 0;
	p += 2;
	continue;

      case stop_memory:
	old_regend[*p] = regend[*p];
	regend[*p] = d;
	IS_ACTIVE(reg_info[*p]) = 0;
	p += 2;
	continue;

      case start_paren:
      case stop_paren:
	break;

	/* \<digit> has been turned into a `duplicate' command which is
	   followed by the numeric value of <digit> as the register number.  */
      case duplicate:
	{
	  int regno = *p++;   /* Get which register to match against */
	  register unsigned char *d2, *dend2;

	  /* Check if there's corresponding group */
	  if (regno >= num_regs) goto fail;
	  /* Check if corresponding group is still open */
	  if (IS_ACTIVE(reg_info[regno])) goto fail;

	  /* Where in input to try to start matching.  */
	  d2 = regstart[regno];
	  if (REG_UNSET(d2)) goto fail;

	  /* Where to stop matching; if both the place to start and
	     the place to stop matching are in the same string, then
	     set to the place to stop, otherwise, for now have to use
	     the end of the first string.  */

	  dend2 = regend[regno];
	  if (REG_UNSET(dend2)) goto fail;
	  for (;;) {
	    /* At end of register contents => success */
	    if (d2 == dend2) break;

	    /* If necessary, advance to next segment in data.  */
	    PREFETCH;

	    /* How many characters left in this segment to match.  */
	    mcnt = dend - d;

	    /* Want how many consecutive characters we can match in
	       one shot, so, if necessary, adjust the count.  */
	    if (mcnt > dend2 - d2)
	      mcnt = dend2 - d2;

	    /* Compare that many; failure if mismatch, else move
	       past them.  */
	    if ((options & MBRE_OPTION_IGNORECASE) 
		? memcmp_translate(d, d2, mcnt, re_mbctab) 
		: memcmp((char*)d, (char*)d2, mcnt))
	      goto fail;
	    d += mcnt, d2 += mcnt;
	  }
	}
	break;

      case start_nowidth:
	PUSH_FAILURE_POINT(0, d);
	if (stackp - stackb > MBRE_DUP_MAX) {
	   FREE_AND_RETURN(stackb,(-2));
	}
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	STORE_NUMBER(p+mcnt, stackp - stackb);
	continue;

      case stop_nowidth:
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	stackp = stackb + mcnt;
	d = stackp[-3];
	POP_FAILURE_POINT();
	continue;

      case stop_backtrack:
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	stackp = stackb + mcnt;
	POP_FAILURE_POINT();
	continue;

      case pop_and_fail:
	EXTRACT_NUMBER(mcnt, p+1);
	stackp = stackb + mcnt;
	POP_FAILURE_POINT();
	goto fail;

      case anychar:
	PREFETCH;
	if (ismbchar(*d)) {
	  if (d + mbclen(*d) > dend)
	    goto fail;
	  SET_REGS_MATCHED;
	  d += mbclen(*d);
	  break;
	}
	if (!(options&MBRE_OPTION_MULTILINE)
	    && (TRANSLATE_P() ? translate[*d] : *d) == '\n')
	  goto fail;
	SET_REGS_MATCHED;
	d++;
	break;

      case anychar_repeat:
	for (;;) {
	  PUSH_FAILURE_POINT(p, d);
	  PREFETCH;
	  if (ismbchar(*d)) {
	    if (d + mbclen(*d) > dend)
	      goto fail;
	    SET_REGS_MATCHED;
	    d += mbclen(*d);
	    continue;
	  }
	  if (!(options&MBRE_OPTION_MULTILINE) &&
	      (TRANSLATE_P() ? translate[*d] : *d) == '\n')
	    goto fail;
	  SET_REGS_MATCHED;
	  d++;
	}
	break;

      case charset:
      case charset_not:
	{
	  int not;	    /* Nonzero for charset_not.  */
	  int part = 0;	    /* true if matched part of mbc */
	  unsigned char *dsave = d + 1;
	  int cc, c;

	  PREFETCH;
	  cc = c = (unsigned char)*d++;
	  if (ismbchar(c)) {
	    if (d + mbclen(c) - 1 <= dend) {
	      MBC2WC(c, d);
	    }
	  }
	  else if (TRANSLATE_P())
	    cc = c = (unsigned char)translate[c];

	  not = is_in_list(c, p);
	  if (!not && cc != c) {
	      part = not = is_in_list(cc, p);
	  }
	  if (*(p - 1) == (unsigned char)charset_not) {
	    not = !not;
	  }
	  if (!not) goto fail;

	  p += 1 + *p + 2 + EXTRACT_UNSIGNED(&p[1 + *p])*8;
	  SET_REGS_MATCHED;

	  if (part) d = dsave;
	  break;
	}

      case begline:
	if (size == 0 || AT_STRINGS_BEG(d))
	  break;
	if (d[-1] == '\n' && !AT_STRINGS_END(d))
	  break;
	goto fail;

      case endline:
	if (AT_STRINGS_END(d)) {
	  if (size == 0 || d[-1] != '\n')
	    break;
	}
	else if (*d == '\n')
	  break;
	goto fail;

	/* Match at the very beginning of the string. */
      case begbuf:
	if (AT_STRINGS_BEG(d))
	  break;
	goto fail;

	/* Match at the very end of the data. */
      case endbuf:
	if (AT_STRINGS_END(d))
	  break;
	goto fail;

	/* Match at the very end of the data. */
      case endbuf2:
	if (AT_STRINGS_END(d)) {
	  if (size == 0 || d[-1] != '\n')
	    break;
	}
	/* .. or newline just before the end of the data. */
	if (*d == '\n' && AT_STRINGS_END(d+1))
	  break;
	goto fail;

	/* `or' constructs are handled by starting each alternative with
	   an on_failure_jump that points to the start of the next
	   alternative.  Each alternative except the last ends with a
	   jump to the joining point.  (Actually, each jump except for
	   the last one really jumps to the following jump, because
	   tensioning the jumps is a hassle.)  */

	/* The start of a stupid repeat has an on_failure_jump that points
	   past the end of the repeat text. This makes a failure point so 
	   that on failure to match a repetition, matching restarts past
	   as many repetitions have been found with no way to fail and
	   look for another one.  */

	/* A smart repeat is similar but loops back to the on_failure_jump
	   so that each repetition makes another failure point.  */

	/* Match at the starting position. */
      case begpos:
	if (d - string == pos)
	  break;
	goto fail;

      case on_failure_jump:
      on_failure:
      EXTRACT_NUMBER_AND_INCR(mcnt, p);
      PUSH_FAILURE_POINT(p + mcnt, d);
      continue;

      /* The end of a smart repeat has a maybe_finalize_jump back.
	 Change it either to a finalize_jump or an ordinary jump.  */
      case maybe_finalize_jump:
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	p1 = p;

	/* Compare the beginning of the repeat with what in the
	   pattern follows its end. If we can establish that there
	   is nothing that they would both match, i.e., that we
	   would have to backtrack because of (as in, e.g., `a*a')
	   then we can change to finalize_jump, because we'll
	   never have to backtrack.

	   This is not true in the case of alternatives: in
	   `(a|ab)*' we do need to backtrack to the `ab' alternative
	   (e.g., if the string was `ab').  But instead of trying to
	   detect that here, the alternative has put on a dummy
	   failure point which is what we will end up popping.  */

	/* Skip over open/close-group commands.  */
	while (p1 + 2 < pend) {
	  if ((enum regexpcode)*p1 == stop_memory ||
	      (enum regexpcode)*p1 == start_memory)
	    p1 += 3;	/* Skip over args, too.  */
	  else if (/*(enum regexpcode)*p1 == start_paren ||*/
		   (enum regexpcode)*p1 == stop_paren)
	      p1 += 1;
	  else
	    break;
	}

	if (p1 == pend)
	  p[-3] = (unsigned char)finalize_jump;
	else if (*p1 == (unsigned char)exactn ||
		 *p1 == (unsigned char)endline) {
	  register int c = *p1 == (unsigned char)endline ? '\n' : p1[2];
	  register unsigned char *p2 = p + mcnt;
	    /* p2[0] ... p2[2] are an on_failure_jump.
	       Examine what follows that.  */
	  if (p2[3] == (unsigned char)exactn && p2[5] != c)
	    p[-3] = (unsigned char)finalize_jump;
	  else if (p2[3] == (unsigned char)charset ||
		   p2[3] == (unsigned char)charset_not) {
	    int not;
	    if (ismbchar(c)) {
	      unsigned char *pp = p1+3;
	      MBC2WC(c, pp);
	    }
	    /* `is_in_list()' is TRUE if c would match */
	    /* That means it is not safe to finalize.  */
	    not = is_in_list(c, p2 + 4);
	    if (p2[3] == (unsigned char)charset_not)
	      not = !not;
	    if (!not)
	      p[-3] = (unsigned char)finalize_jump;
	  }
	}
	p -= 2;		/* Point at relative address again.  */
	if (p[-1] != (unsigned char)finalize_jump) {
	  p[-1] = (unsigned char)jump;	
	  goto nofinalize;
	}
	/* Note fall through.  */

	/* The end of a stupid repeat has a finalize_jump back to the
	   start, where another failure point will be made which will
	   point to after all the repetitions found so far.  */

	/* Take off failure points put on by matching on_failure_jump 
	   because didn't fail.  Also remove the register information
	   put on by the on_failure_jump.  */
      case finalize_jump:
	if (stackp > stackb && stackp[-3] == d) {
	  p = stackp[-4];
	  POP_FAILURE_POINT();
	  continue;
	}
	POP_FAILURE_POINT(); 
	/* Note fall through.  */

      /* We need this opcode so we can detect where alternatives end
	 in `group_match_null_string_p' et al.  */
      case jump_past_alt:
	/* fall through */

	/* Jump without taking off any failure points.  */
      case jump:
      nofinalize:
        EXTRACT_NUMBER_AND_INCR(mcnt, p);
        if (mcnt < 0 && stackp > stackb && stackp[-3] == d) /* avoid infinite loop */
	   goto fail;
        p += mcnt;
        continue;

      case dummy_failure_jump:
	/* Normally, the on_failure_jump pushes a failure point, which
	   then gets popped at finalize_jump.  We will end up at
	   finalize_jump, also, and with a pattern of, say, `a+', we
	   are skipping over the on_failure_jump, so we have to push
	   something meaningless for finalize_jump to pop.  */
	PUSH_FAILURE_POINT(0, 0);
	goto nofinalize;

	/* At the end of an alternative, we need to push a dummy failure
	   point in case we are followed by a `finalize_jump', because
	   we don't want the failure point for the alternative to be
	   popped.  For example, matching `(a|ab)*' against `aab'
	   requires that we match the `ab' alternative.  */
      case push_dummy_failure:
	/* See comments just above at `dummy_failure_jump' about the
	   two zeroes.  */
	p1 = p;
	/* Skip over open/close-group commands.  */
	while (p1 + 2 < pend) {
	  if ((enum regexpcode)*p1 == stop_memory ||
	      (enum regexpcode)*p1 == start_memory)
	    p1 += 3;	/* Skip over args, too.  */
	  else if (/*(enum regexpcode)*p1 == start_paren ||*/
		   (enum regexpcode)*p1 == stop_paren)
	      p1 += 1;
	  else
	    break;
	}
	if ((enum regexpcode)*p1 == jump)
	  p[-1] = unused;
	else
	  PUSH_FAILURE_POINT(0, 0);
	break;

	/* Have to succeed matching what follows at least n times.  Then
	   just handle like an on_failure_jump.  */
      case succeed_n: 
	EXTRACT_NUMBER(mcnt, p + 2);
	/* Originally, this is how many times we HAVE to succeed.  */
	if (mcnt != 0) {
	  mcnt--;
	  p += 2;
	  PUSH_FAILURE_COUNT(p);
	  STORE_NUMBER_AND_INCR(p, mcnt);
	  PUSH_FAILURE_POINT(0, 0);
	}
	else  {
	  goto on_failure;
	}
	continue;

      case jump_n:
	EXTRACT_NUMBER(mcnt, p + 2);
	/* Originally, this is how many times we CAN jump.  */
	if (mcnt) {
	  mcnt--;
	  PUSH_FAILURE_COUNT(p + 2);
	  STORE_NUMBER(p + 2, mcnt);
	  goto nofinalize;	     /* Do the jump without taking off
					any failure points.  */
	}
	/* If don't have to jump any more, skip over the rest of command.  */
	else      
	  p += 4;		     
	continue;

      case set_number_at:
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	p1 = p + mcnt;
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	STORE_NUMBER(p1, mcnt);
	continue;

      case try_next:
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
	if (p + mcnt < pend) {
	  PUSH_FAILURE_POINT(p, d);
	  stackp[-1] = NON_GREEDY;
	}
	p += mcnt;
	continue;

      case finalize_push:
	POP_FAILURE_POINT();
	EXTRACT_NUMBER_AND_INCR(mcnt, p);
        if (mcnt < 0 && stackp > stackb  && stackp[-3] == d) /* avoid infinite loop */
	   goto fail;
	PUSH_FAILURE_POINT(p + mcnt, d);
	stackp[-1] = NON_GREEDY;
	continue;

      case finalize_push_n:
	EXTRACT_NUMBER(mcnt, p + 2); 
	/* Originally, this is how many times we CAN jump.  */
	if (mcnt) {
	  int pos, i;

	  mcnt--;
	  STORE_NUMBER(p + 2, mcnt);
	  EXTRACT_NUMBER(pos, p);
	  EXTRACT_NUMBER(i, p+pos+5);
	  if (i > 0) goto nofinalize;
	  POP_FAILURE_POINT();
	  EXTRACT_NUMBER_AND_INCR(mcnt, p);
	  PUSH_FAILURE_POINT(p + mcnt, d);
	  stackp[-1] = NON_GREEDY;
	  p += 2;		/* skip n */
	}
	/* If don't have to push any more, skip over the rest of command.  */
	else 
	  p += 4;   
	continue;

	/* Ignore these.  Used to ignore the n of succeed_n's which
	   currently have n == 0.  */
      case unused:
	continue;

      case casefold_on:
	options |= MBRE_OPTION_IGNORECASE;
	continue;

      case casefold_off:
	options &= ~MBRE_OPTION_IGNORECASE;
	continue;

      case option_set:
	options = *p++;
	continue;

      case wordbound:
	if (AT_STRINGS_BEG(d)) {
	  if (IS_A_LETTER(d)) break;
	  else goto fail;
	}
	if (AT_STRINGS_END(d)) {
	  if (PREV_IS_A_LETTER(d)) break;
	  else goto fail;
	}
	if (PREV_IS_A_LETTER(d) != IS_A_LETTER(d))
	  break;
	goto fail;

      case notwordbound:
	if (AT_STRINGS_BEG(d)) {
	  if (IS_A_LETTER(d)) goto fail;
	  else break;
	}
	if (AT_STRINGS_END(d)) {
	  if (PREV_IS_A_LETTER(d)) goto fail;
	  else break;
	}
	if (PREV_IS_A_LETTER(d) != IS_A_LETTER(d))
	  goto fail;
	break;

      case wordbeg:
	if (IS_A_LETTER(d) && (AT_STRINGS_BEG(d) || !PREV_IS_A_LETTER(d)))
	  break;
	goto fail;

      case wordend:
	if (!AT_STRINGS_BEG(d) && PREV_IS_A_LETTER(d)
	    && (!IS_A_LETTER(d) || AT_STRINGS_END(d)))
	  break;
	goto fail;

      case wordchar:
	PREFETCH;
	if (!IS_A_LETTER(d))
	  goto fail;
	if (ismbchar(*d) && d + mbclen(*d) - 1 < dend)
	  d += mbclen(*d) - 1;
	d++;
	SET_REGS_MATCHED;
	break;

      case notwordchar:
	PREFETCH;
	if (IS_A_LETTER(d))
	  goto fail;
	if (ismbchar(*d) && d + mbclen(*d) - 1 < dend)
	  d += mbclen(*d) - 1;
	d++;
	SET_REGS_MATCHED;
	break;

      case exactn:
	/* Match the next few pattern characters exactly.
	   mcnt is how many characters to match.  */
	mcnt = *p++;
	/* This is written out as an if-else so we don't waste time
	   testing `translate' inside the loop.  */
	if (TRANSLATE_P()) {
	  do {
	    unsigned char c;

	    PREFETCH;
	    if (*p == 0xff) {
	      p++;  
	      if (!--mcnt
		  || AT_STRINGS_END(d)
		  || (unsigned char)*d++ != (unsigned char)*p++)
		goto fail;
	      continue;
	    }
	    c = *d++;
	    if (ismbchar(c)) {
	      int n;

	      if (c != (unsigned char)*p++)
		goto fail;
	      for (n = mbclen(c) - 1; n > 0; n--)
		if (!--mcnt	/* redundant check if pattern was
				   compiled properly. */
		    || AT_STRINGS_END(d)
		    || (unsigned char)*d++ != (unsigned char)*p++)
		  goto fail;
	      continue;
	    }
	    /* compiled code translation needed for ruby */
	    if ((unsigned char)translate[c] != (unsigned char)translate[*p++])
	      goto fail;
	  }
	  while (--mcnt);
	}
	else {
	  do {
	    PREFETCH;
	    if (*p == 0xff) {p++; mcnt--;}
	    if (*d++ != *p++) goto fail;
	  }
	  while (--mcnt);
	}
	SET_REGS_MATCHED;
	break;
      case fail:
	goto fail;
	break;
      }
#ifdef RUBY
    CHECK_INTS;
#endif
    continue;  /* Successfully executed one pattern command; keep going.  */

    /* Jump here if any matching operation fails. */
  fail:
    if (stackp != stackb) {
      /* A restart point is known.  Restart there and pop it. */
      long last_used_reg, this_reg;

      /* If this failure point is from a dummy_failure_point, just
	 skip it.  */
      if (stackp[-4] == 0 || (best_regs_set && stackp[-1] == NON_GREEDY)) {
	POP_FAILURE_POINT();
	goto fail;
      }
      stackp--;		/* discard greedy flag */
      options = (int)*--stackp;
      d = *--stackp;
      p = *--stackp;
      /* Restore register info.  */
      last_used_reg = (long)*--stackp;

      /* Make the ones that weren't saved -1 or 0 again. */
      for (this_reg = num_regs - 1; this_reg > last_used_reg; this_reg--) {
	regend[this_reg] = REG_UNSET_VALUE;
	regstart[this_reg] = REG_UNSET_VALUE;
	IS_ACTIVE(reg_info[this_reg]) = 0;
	MATCHED_SOMETHING(reg_info[this_reg]) = 0;
      }

      /* And restore the rest from the stack.  */
      for ( ; this_reg > 0; this_reg--) {
	reg_info[this_reg].word = *--stackp;
	regend[this_reg] = *--stackp;
	regstart[this_reg] = *--stackp;
      }
      mcnt = (long)*--stackp;
      while (mcnt--) {
	POP_FAILURE_COUNT();
      }
      if (p < pend) {
	int is_a_jump_n = 0;
	int failed_paren = 0;

	p1 = p;
	/* If failed to a backwards jump that's part of a repetition
	   loop, need to pop this failure point and use the next one.  */
	switch ((enum regexpcode)*p1) {
	case jump_n:
	case finalize_push_n:
	  is_a_jump_n = 1;
	case maybe_finalize_jump:
	case finalize_jump:
	case finalize_push:
	case jump:
	  p1++;
	  EXTRACT_NUMBER_AND_INCR(mcnt, p1);

	  if (mcnt >= 0) break;	/* should be backward jump */
	  p1 += mcnt;

	  if (( is_a_jump_n && (enum regexpcode)*p1 == succeed_n) ||
	      (!is_a_jump_n && (enum regexpcode)*p1 == on_failure_jump)) {
	    if (failed_paren) {
	      p1++;
	      EXTRACT_NUMBER_AND_INCR(mcnt, p1);
	      PUSH_FAILURE_POINT(p1 + mcnt, d);
	    }
	    goto fail;
	  }
	  break;
	default:
	  /* do nothing */;
	}
      }
    }
    else
      break;   /* Matching at this starting point really fails.  */
  }

  if (best_regs_set)
    goto restore_best_regs;

  FREE_AND_RETURN(stackb,(-1)); 	/* Failure to match.  */
}


static int
memcmp_translate(s1, s2, len, re_mbctab)
     unsigned char *s1, *s2;
     register int len;
     const unsigned char *re_mbctab;
{
  register unsigned char *p1 = s1, *p2 = s2, c;
  while (len) {
    c = *p1++;
    if (ismbchar(c)) {
      int n;

      if (c != *p2++) return 1;
      for (n = mbclen(c) - 1; n > 0; n--)
	if (!--len || *p1++ != *p2++)
	  return 1;
    }
    else
      if (translate[c] != translate[*p2++])
	return 1;
    len--;
  }
  return 0;
}

void
re_copy_registers(regs1, regs2)
     struct mbre_registers *regs1, *regs2;
{
  int i;

  if (regs1 == regs2) return;
  if (regs1->allocated == 0) {
    regs1->beg = TMALLOC(regs2->num_regs, int);
    regs1->end = TMALLOC(regs2->num_regs, int);
    regs1->allocated = regs2->num_regs;
  }
  else if (regs1->allocated < regs2->num_regs) {
    TREALLOC(regs1->beg, regs2->num_regs, int);
    TREALLOC(regs1->end, regs2->num_regs, int);
    regs1->allocated = regs2->num_regs;
  }
  for (i=0; i<regs2->num_regs; i++) {
    regs1->beg[i] = regs2->beg[i];
    regs1->end[i] = regs2->end[i];
  }
  regs1->num_regs = regs2->num_regs;
}

void
re_free_registers(regs)
     struct mbre_registers *regs;
{
  if (regs->allocated == 0) return;
  if (regs->beg) xfree(regs->beg);
  if (regs->end) xfree(regs->end);
}

/* Functions for multi-byte support.
   Created for grep multi-byte extension Jul., 1993 by t^2 (Takahiro Tanimoto)
   Last change: Jul. 9, 1993 by t^2  */
static const unsigned char mbctab_ascii[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char mbctab_euc[] = { /* 0xA1-0xFE */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

static const unsigned char mbctab_sjis[] = { /* 0x80-0x9f,0xE0-0xFF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static const unsigned char mbctab_utf8[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 0, 0
};

#if 0
const unsigned char *re_mbctab = mbctab_ascii;

void
re_mbcinit(mbctype)
     int mbctype;
{
  switch (mbctype) {
  case MBCTYPE_ASCII:
    re_mbctab = mbctab_ascii;
    current_mbctype = MBCTYPE_ASCII;
    break;
  case MBCTYPE_EUC:
    re_mbctab = mbctab_euc;
    current_mbctype = MBCTYPE_EUC;
    break;
  case MBCTYPE_SJIS:
    re_mbctab = mbctab_sjis;
    current_mbctype = MBCTYPE_SJIS;
    break;
  case MBCTYPE_UTF8:
    re_mbctab = mbctab_utf8;
    current_mbctype = MBCTYPE_UTF8;
    break;
  }
}
#endif

static const unsigned char*
re_mbctab_get(mbctype)
     int mbctype;
{
  const unsigned char *p;

  switch (mbctype) {
  case MBCTYPE_EUC:
    p = mbctab_euc;
    break;
  case MBCTYPE_SJIS:
    p = mbctab_sjis;
    break;
  case MBCTYPE_UTF8:
    p = mbctab_utf8;
    break;
  default:
    p = mbctab_ascii;
    break;
  }

  return p;
}
