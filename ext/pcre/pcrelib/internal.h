/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/


/* This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2003 University of Cambridge

-----------------------------------------------------------------------------
Permission is granted to anyone to use this software for any purpose on any
computer system, and to redistribute it freely, subject to the following
restrictions:

1. This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

4. If PCRE is embedded in any software that is released under the GNU
   General Purpose Licence (GPL), then the terms of that licence shall
   supersede any condition above with which it is incompatible.
-----------------------------------------------------------------------------
*/

/* This header contains definitions that are shared between the different
modules, but which are not relevant to the outside. */

/* Get the definitions provided by running "configure" */

#ifdef PHP_WIN32
# include "config.w32.h"
#elif defined(NETWARE)
# include "config.nw.h"
#else
# include <php_config.h>
#endif

#ifndef PCRE_SPY
#define PCRE_DEFINITION       /* Win32 __declspec(export) trigger for .dll */
#endif

/* The value of NEWLINE determines the newline character. The default is to
leave it up to the compiler, but some sites want to force a particular value.
On Unix systems, "configure" can be used to override this default. */

#ifndef NEWLINE
#define NEWLINE '\n'
#endif

/* The value of MATCH_LIMIT determines the default number of times the match()
function can be called during a single execution of pcre_exec(). (There is a
runtime method of setting a different limit.) The limit exists in order to
catch runaway regular expressions that take for ever to determine that they do
not match. The default is set very large so that it does not accidentally catch
legitimate cases. On Unix systems, "configure" can be used to override this
default default. */

#ifndef MATCH_LIMIT
#define MATCH_LIMIT 10000000
#endif

/* If you are compiling for a system that needs some magic to be inserted
 * before the definition of an exported function, define this macro to contain
 * the relevant magic. It apears at the start of every exported function. */
                                                                                                                                
#define EXPORT

#include "pcre.h"

/* When compiling for use with the Virtual Pascal compiler, these functions
need to have their names changed. PCRE must be compiled with the -DVPCOMPAT
option on the command line. */

#ifdef VPCOMPAT
#define strncmp(s1,s2,m) _strncmp(s1,s2,m)
#define memcpy(d,s,n)    _memcpy(d,s,n)
#define memmove(d,s,n)   _memmove(d,s,n)
#define memset(s,c,n)    _memset(s,c,n)
#else  /* VPCOMPAT */

/* To cope with SunOS4 and other systems that lack memmove() but have bcopy(),
define a macro for memmove() if HAVE_MEMMOVE is false, provided that HAVE_BCOPY
is set. Otherwise, include an emulating function for those systems that have
neither (there some non-Unix environments where this is the case). This assumes
that all calls to memmove are moving strings upwards in store, which is the
case in PCRE. */

/* Standard C headers plus the external interface definition. The only time
setjmp and stdarg are used is when NO_RECURSE is set. */

#include <ctype.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if ! HAVE_MEMMOVE
#undef  memmove        /* some systems may have a macro */
#if HAVE_BCOPY
#define memmove(a, b, c) bcopy(b, a, c)
#else  /* HAVE_BCOPY */
void *
pcre_memmove(unsigned char *dest, const unsigned char *src, size_t n)
{
int i;
dest += n;
src += n;
for (i = 0; i < n; ++i) *(--dest) =  *(--src);
}
#define memmove(a, b, c) pcre_memmove(a, b, c)
#endif   /* not HAVE_BCOPY */
#endif   /* not HAVE_MEMMOVE */
#endif   /* not VPCOMPAT */


/* PCRE keeps offsets in its compiled code as 2-byte quantities by default.
These are used, for example, to link from the start of a subpattern to its
alternatives and its end. The use of 2 bytes per offset limits the size of the
compiled regex to around 64K, which is big enough for almost everybody.
However, I received a request for an even bigger limit. For this reason, and
also to make the code easier to maintain, the storing and loading of offsets
from the byte string is now handled by the macros that are defined here.

The macros are controlled by the value of LINK_SIZE. This defaults to 2 in
the config.h file, but can be overridden by using -D on the command line. This
is automated on Unix systems via the "configure" command. */

#if LINK_SIZE == 2

#define PUT(a,n,d)   \
  (a[n] = (d) >> 8), \
  (a[(n)+1] = (d) & 255)

#define GET(a,n) \
  (((a)[n] << 8) | (a)[(n)+1])

#define MAX_PATTERN_SIZE (1 << 16)


#elif LINK_SIZE == 3

#define PUT(a,n,d)       \
  (a[n] = (d) >> 16),    \
  (a[(n)+1] = (d) >> 8), \
  (a[(n)+2] = (d) & 255)

#define GET(a,n) \
  (((a)[n] << 16) | ((a)[(n)+1] << 8) | (a)[(n)+2])

#define MAX_PATTERN_SIZE (1 << 24)


#elif LINK_SIZE == 4

#define PUT(a,n,d)        \
  (a[n] = (d) >> 24),     \
  (a[(n)+1] = (d) >> 16), \
  (a[(n)+2] = (d) >> 8),  \
  (a[(n)+3] = (d) & 255)

#define GET(a,n) \
  (((a)[n] << 24) | ((a)[(n)+1] << 16) | ((a)[(n)+2] << 8) | (a)[(n)+3])

#define MAX_PATTERN_SIZE (1 << 30)   /* Keep it positive */


#else
#error LINK_SIZE must be either 2, 3, or 4
#endif


/* Convenience macro defined in terms of the others */

#define PUTINC(a,n,d)   PUT(a,n,d), a += LINK_SIZE


/* PCRE uses some other 2-byte quantities that do not change when the size of
offsets changes. There are used for repeat counts and for other things such as
capturing parenthesis numbers in back references. */

#define PUT2(a,n,d)   \
  a[n] = (d) >> 8; \
  a[(n)+1] = (d) & 255

#define GET2(a,n) \
  (((a)[n] << 8) | (a)[(n)+1])

#define PUT2INC(a,n,d)  PUT2(a,n,d), a += 2


/* In case there is no definition of offsetof() provided - though any proper
Standard C system should have one. */

#ifndef offsetof
#define offsetof(p_type,field) ((size_t)&(((p_type *)0)->field))
#endif

/* These are the public options that can change during matching. */

#define PCRE_IMS (PCRE_CASELESS|PCRE_MULTILINE|PCRE_DOTALL)

/* Private options flags start at the most significant end of the four bytes,
but skip the top bit so we can use ints for convenience without getting tangled
with negative values. The public options defined in pcre.h start at the least
significant end. Make sure they don't overlap, though now that we have expanded
to four bytes there is plenty of space. */

#define PCRE_FIRSTSET      0x40000000  /* first_byte is set */
#define PCRE_REQCHSET      0x20000000  /* req_byte is set */
#define PCRE_STARTLINE     0x10000000  /* start after \n for multiline */
#define PCRE_ICHANGED      0x08000000  /* i option changes within regex */

/* Options for the "extra" block produced by pcre_study(). */

#define PCRE_STUDY_MAPPED   0x01     /* a map of starting chars exists */

/* Masks for identifying the public options which are permitted at compile
time, run time or study time, respectively. */

#define PUBLIC_OPTIONS \
  (PCRE_CASELESS|PCRE_EXTENDED|PCRE_ANCHORED|PCRE_MULTILINE| \
   PCRE_DOTALL|PCRE_DOLLAR_ENDONLY|PCRE_EXTRA|PCRE_UNGREEDY|PCRE_UTF8| \
   PCRE_NO_AUTO_CAPTURE|PCRE_NO_UTF8_CHECK)

#define PUBLIC_EXEC_OPTIONS \
  (PCRE_ANCHORED|PCRE_NOTBOL|PCRE_NOTEOL|PCRE_NOTEMPTY|PCRE_NO_UTF8_CHECK)

#define PUBLIC_STUDY_OPTIONS 0   /* None defined */

/* Magic number to provide a small check against being handed junk. */

#define MAGIC_NUMBER  0x50435245UL   /* 'PCRE' */

/* Negative values for the firstchar and reqchar variables */

#define REQ_UNSET (-2)
#define REQ_NONE  (-1)

/* Flags added to firstbyte or reqbyte; a "non-literal" item is either a
variable-length repeat, or a anything other than literal characters. */

#define REQ_CASELESS 0x0100    /* indicates caselessness */
#define REQ_VARY     0x0200    /* reqbyte followed non-literal item */

/* Miscellaneous definitions */

typedef int BOOL;

#define FALSE   0
#define TRUE    1

/* Escape items that are just an encoding of a particular data value. Note that
ESC_n is defined as yet another macro, which is set in config.h to either \n
(the default) or \r (which some people want). */

#ifndef ESC_e
#define ESC_e 27
#endif

#ifndef ESC_f
#define ESC_f '\f'
#endif

#ifndef ESC_n
#define ESC_n NEWLINE
#endif

#ifndef ESC_r
#define ESC_r '\r'
#endif

/* We can't officially use ESC_t because it is a POSIX reserved identifier
(presumably because of all the others like size_t). */

#ifndef ESC_tee
#define ESC_tee '\t'
#endif

/* These are escaped items that aren't just an encoding of a particular data
value such as \n. They must have non-zero values, as check_escape() returns
their negation. Also, they must appear in the same order as in the opcode
definitions below, up to ESC_z. There's a dummy for OP_ANY because it
corresponds to "." rather than an escape sequence. The final one must be
ESC_REF as subsequent values are used for \1, \2, \3, etc. There is are two
tests in the code for an escape greater than ESC_b and less than ESC_Z to
detect the types that may be repeated. These are the types that consume a
character. If any new escapes are put in between that don't consume a
character, that code will have to change. */

enum { ESC_A = 1, ESC_G, ESC_B, ESC_b, ESC_D, ESC_d, ESC_S, ESC_s, ESC_W,
       ESC_w, ESC_dum1, ESC_C, ESC_Z, ESC_z, ESC_E, ESC_Q, ESC_REF };

/* Flag bits and data types for the extended class (OP_XCLASS) for classes that
contain UTF-8 characters with values greater than 255. */

#define XCL_NOT    0x01    /* Flag: this is a negative class */
#define XCL_MAP    0x02    /* Flag: a 32-byte map is present */

#define XCL_END       0    /* Marks end of individual items */
#define XCL_SINGLE    1    /* Single item (one multibyte char) follows */
#define XCL_RANGE     2    /* A range (two multibyte chars) follows */


/* Opcode table: OP_BRA must be last, as all values >= it are used for brackets
that extract substrings. Starting from 1 (i.e. after OP_END), the values up to
OP_EOD must correspond in order to the list of escapes immediately above.
Note that whenever this list is updated, the two macro definitions that follow
must also be updated to match. */

enum {
  OP_END,            /* 0 End of pattern */

  /* Values corresponding to backslashed metacharacters */

  OP_SOD,            /* 1 Start of data: \A */
  OP_SOM,            /* 2 Start of match (subject + offset): \G */
  OP_NOT_WORD_BOUNDARY,  /*  3 \B */
  OP_WORD_BOUNDARY,      /*  4 \b */
  OP_NOT_DIGIT,          /*  5 \D */
  OP_DIGIT,              /*  6 \d */
  OP_NOT_WHITESPACE,     /*  7 \S */
  OP_WHITESPACE,         /*  8 \s */
  OP_NOT_WORDCHAR,       /*  9 \W */
  OP_WORDCHAR,           /* 10 \w */
  OP_ANY,            /* 11 Match any character */
  OP_ANYBYTE,        /* 12 Match any byte (\C); different to OP_ANY for UTF-8 */
  OP_EODN,           /* 13 End of data or \n at end of data: \Z. */
  OP_EOD,            /* 14 End of data: \z */

  OP_OPT,            /* 15 Set runtime options */
  OP_CIRC,           /* 16 Start of line - varies with multiline switch */
  OP_DOLL,           /* 17 End of line - varies with multiline switch */
  OP_CHARS,          /* 18 Match string of characters */
  OP_NOT,            /* 19 Match anything but the following char */

  OP_STAR,           /* 20 The maximizing and minimizing versions of */
  OP_MINSTAR,        /* 21 all these opcodes must come in pairs, with */
  OP_PLUS,           /* 22 the minimizing one second. */
  OP_MINPLUS,        /* 23 This first set applies to single characters */
  OP_QUERY,          /* 24 */
  OP_MINQUERY,       /* 25 */
  OP_UPTO,           /* 26 From 0 to n matches */
  OP_MINUPTO,        /* 27 */
  OP_EXACT,          /* 28 Exactly n matches */

  OP_NOTSTAR,        /* 29 The maximizing and minimizing versions of */
  OP_NOTMINSTAR,     /* 30 all these opcodes must come in pairs, with */
  OP_NOTPLUS,        /* 31 the minimizing one second. */
  OP_NOTMINPLUS,     /* 32 This set applies to "not" single characters */
  OP_NOTQUERY,       /* 33 */
  OP_NOTMINQUERY,    /* 34 */
  OP_NOTUPTO,        /* 35 From 0 to n matches */
  OP_NOTMINUPTO,     /* 36 */
  OP_NOTEXACT,       /* 37 Exactly n matches */

  OP_TYPESTAR,       /* 38 The maximizing and minimizing versions of */
  OP_TYPEMINSTAR,    /* 39 all these opcodes must come in pairs, with */
  OP_TYPEPLUS,       /* 40 the minimizing one second. These codes must */
  OP_TYPEMINPLUS,    /* 41 be in exactly the same order as those above. */
  OP_TYPEQUERY,      /* 42 This set applies to character types such as \d */
  OP_TYPEMINQUERY,   /* 43 */
  OP_TYPEUPTO,       /* 44 From 0 to n matches */
  OP_TYPEMINUPTO,    /* 45 */
  OP_TYPEEXACT,      /* 46 Exactly n matches */

  OP_CRSTAR,         /* 47 The maximizing and minimizing versions of */
  OP_CRMINSTAR,      /* 48 all these opcodes must come in pairs, with */
  OP_CRPLUS,         /* 49 the minimizing one second. These codes must */
  OP_CRMINPLUS,      /* 50 be in exactly the same order as those above. */
  OP_CRQUERY,        /* 51 These are for character classes and back refs */
  OP_CRMINQUERY,     /* 52 */
  OP_CRRANGE,        /* 53 These are different to the three seta above. */
  OP_CRMINRANGE,     /* 54 */

  OP_CLASS,          /* 55 Match a character class, chars < 256 only */
  OP_NCLASS,         /* 56 Same, but the bitmap was created from a negative
                           class - the difference is relevant only when a UTF-8
                           character > 255 is encountered. */

  OP_XCLASS,         /* 57 Extended class for handling UTF-8 chars within the
                           class. This does both positive and negative. */

  OP_REF,            /* 58 Match a back reference */
  OP_RECURSE,        /* 59 Match a numbered subpattern (possibly recursive) */
  OP_CALLOUT,        /* 60 Call out to external function if provided */

  OP_ALT,            /* 61 Start of alternation */
  OP_KET,            /* 62 End of group that doesn't have an unbounded repeat */
  OP_KETRMAX,        /* 63 These two must remain together and in this */
  OP_KETRMIN,        /* 64 order. They are for groups the repeat for ever. */

  /* The assertions must come before ONCE and COND */

  OP_ASSERT,         /* 65 Positive lookahead */
  OP_ASSERT_NOT,     /* 66 Negative lookahead */
  OP_ASSERTBACK,     /* 67 Positive lookbehind */
  OP_ASSERTBACK_NOT, /* 68 Negative lookbehind */
  OP_REVERSE,        /* 69 Move pointer back - used in lookbehind assertions */

  /* ONCE and COND must come after the assertions, with ONCE first, as there's
  a test for >= ONCE for a subpattern that isn't an assertion. */

  OP_ONCE,           /* 70 Once matched, don't back up into the subpattern */
  OP_COND,           /* 71 Conditional group */
  OP_CREF,           /* 72 Used to hold an extraction string number (cond ref) */

  OP_BRAZERO,        /* 73 These two must remain together and in this */
  OP_BRAMINZERO,     /* 74 order. */

  OP_BRANUMBER,      /* 75 Used for extracting brackets whose number is greater
                           than can fit into an opcode. */

  OP_BRA             /* 76 This and greater values are used for brackets that
                           extract substrings up to a basic limit. After that,
                           use is made of OP_BRANUMBER. */
};

/* WARNING: There is an implicit assumption in study.c that all opcodes are
less than 128 in value. This makes handling UTF-8 character sequences easier.
*/


/* This macro defines textual names for all the opcodes. There are used only
for debugging, in pcre.c when DEBUG is defined, and also in pcretest.c. The
macro is referenced only in printint.c. */

#define OP_NAME_LIST \
  "End", "\\A", "\\G", "\\B", "\\b", "\\D", "\\d",                \
  "\\S", "\\s", "\\W", "\\w", "Any", "Anybyte", "\\Z", "\\z",     \
  "Opt", "^", "$", "chars", "not",                                \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*", "*?", "+", "+?", "?", "??", "{", "{",                      \
  "class", "nclass", "xclass", "Ref", "Recurse", "Callout",       \
  "Alt", "Ket", "KetRmax", "KetRmin", "Assert", "Assert not",     \
  "AssertB", "AssertB not", "Reverse", "Once", "Cond", "Cond ref",\
  "Brazero", "Braminzero", "Branumber", "Bra"


/* This macro defines the length of fixed length operations in the compiled
regex. The lengths are used when searching for specific things, and also in the
debugging printing of a compiled regex. We use a macro so that it can be
incorporated both into pcre.c and pcretest.c without being publicly exposed.

As things have been extended, some of these are no longer fixed lenths, but are
minima instead. For example, the length of a single-character repeat may vary
in UTF-8 mode. The code that uses this table must know about such things. */

#define OP_LENGTHS \
  1,                             /* End                                    */ \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* \A, \G, \B, \B, \D, \d, \S, \s, \W, \w */ \
  1, 1, 1, 1, 2, 1, 1,           /* Any, Anybyte, \Z, \z, Opt, ^, $        */ \
  2,                             /* Chars - the minimum length             */ \
  2,                             /* not                                    */ \
  /* Positive single-char repeats                            ** These are  */ \
  2, 2, 2, 2, 2, 2,              /* *, *?, +, +?, ?, ??      ** minima in  */ \
  4, 4, 4,                       /* upto, minupto, exact     ** UTF-8 mode */ \
  /* Negative single-char repeats - only for chars < 256                   */ \
  2, 2, 2, 2, 2, 2,              /* NOT *, *?, +, +?, ?, ??                */ \
  4, 4, 4,                       /* NOT upto, minupto, exact               */ \
  /* Positive type repeats                                                 */ \
  2, 2, 2, 2, 2, 2,              /* Type *, *?, +, +?, ?, ??               */ \
  4, 4, 4,                       /* Type upto, minupto, exact              */ \
  /* Character class & ref repeats                                         */ \
  1, 1, 1, 1, 1, 1,              /* *, *?, +, +?, ?, ??                    */ \
  5, 5,                          /* CRRANGE, CRMINRANGE                    */ \
 33,                             /* CLASS                                  */ \
 33,                             /* NCLASS                                 */ \
  0,                             /* XCLASS - variable length               */ \
  3,                             /* REF                                    */ \
  1+LINK_SIZE,                   /* RECURSE                                */ \
  2,                             /* CALLOUT                                */ \
  1+LINK_SIZE,                   /* Alt                                    */ \
  1+LINK_SIZE,                   /* Ket                                    */ \
  1+LINK_SIZE,                   /* KetRmax                                */ \
  1+LINK_SIZE,                   /* KetRmin                                */ \
  1+LINK_SIZE,                   /* Assert                                 */ \
  1+LINK_SIZE,                   /* Assert not                             */ \
  1+LINK_SIZE,                   /* Assert behind                          */ \
  1+LINK_SIZE,                   /* Assert behind not                      */ \
  1+LINK_SIZE,                   /* Reverse                                */ \
  1+LINK_SIZE,                   /* Once                                   */ \
  1+LINK_SIZE,                   /* COND                                   */ \
  3,                             /* CREF                                   */ \
  1, 1,                          /* BRAZERO, BRAMINZERO                    */ \
  3,                             /* BRANUMBER                              */ \
  1+LINK_SIZE                    /* BRA                                    */ \


/* The highest extraction number before we have to start using additional
bytes. (Originally PCRE didn't have support for extraction counts highter than
this number.) The value is limited by the number of opcodes left after OP_BRA,
i.e. 255 - OP_BRA. We actually set it a bit lower to leave room for additional
opcodes. */

#define EXTRACT_BASIC_MAX  150

/* A magic value for OP_CREF to indicate the "in recursion" condition. */

#define CREF_RECURSE  0xffff

/* The texts of compile-time error messages are defined as macros here so that
they can be accessed by the POSIX wrapper and converted into error codes.  Yes,
I could have used error codes in the first place, but didn't feel like changing
just to accommodate the POSIX wrapper. */

#define ERR1  "\\ at end of pattern"
#define ERR2  "\\c at end of pattern"
#define ERR3  "unrecognized character follows \\"
#define ERR4  "numbers out of order in {} quantifier"
#define ERR5  "number too big in {} quantifier"
#define ERR6  "missing terminating ] for character class"
#define ERR7  "invalid escape sequence in character class"
#define ERR8  "range out of order in character class"
#define ERR9  "nothing to repeat"
#define ERR10 "operand of unlimited repeat could match the empty string"
#define ERR11 "internal error: unexpected repeat"
#define ERR12 "unrecognized character after (?"
#define ERR13 "POSIX named classes are supported only within a class"
#define ERR14 "missing )"
#define ERR15 "reference to non-existent subpattern"
#define ERR16 "erroffset passed as NULL"
#define ERR17 "unknown option bit(s) set"
#define ERR18 "missing ) after comment"
#define ERR19 "parentheses nested too deeply"
#define ERR20 "regular expression too large"
#define ERR21 "failed to get memory"
#define ERR22 "unmatched parentheses"
#define ERR23 "internal error: code overflow"
#define ERR24 "unrecognized character after (?<"
#define ERR25 "lookbehind assertion is not fixed length"
#define ERR26 "malformed number after (?("
#define ERR27 "conditional group contains more than two branches"
#define ERR28 "assertion expected after (?("
#define ERR29 "(?R or (?digits must be followed by )"
#define ERR30 "unknown POSIX class name"
#define ERR31 "POSIX collating elements are not supported"
#define ERR32 "this version of PCRE is not compiled with PCRE_UTF8 support"
#define ERR33 "spare error"
#define ERR34 "character value in \\x{...} sequence is too large"
#define ERR35 "invalid condition (?(0)"
#define ERR36 "\\C not allowed in lookbehind assertion"
#define ERR37 "PCRE does not support \\L, \\l, \\N, \\P, \\p, \\U, \\u, or \\X"
#define ERR38 "number after (?C is > 255"
#define ERR39 "closing ) for (?C expected"
#define ERR40 "recursive call could loop indefinitely"
#define ERR41 "unrecognized character after (?P"
#define ERR42 "syntax error after (?P"
#define ERR43 "two named groups have the same name"
#define ERR44 "invalid UTF-8 string"

/* All character handling must be done as unsigned characters. Otherwise there
are problems with top-bit-set characters and functions such as isspace().
However, we leave the interface to the outside world as char *, because that
should make things easier for callers. We define a short type for unsigned char
to save lots of typing. I tried "uchar", but it causes problems on Digital
Unix, where it is defined in sys/types, so use "uschar" instead. */

typedef unsigned char uschar;

/* The real format of the start of the pcre block; the index of names and the
code vector run on as long as necessary after the end. */

typedef struct real_pcre {
  unsigned long int magic_number;
  size_t size;                        /* Total that was malloced */
  const unsigned char *tables;        /* Pointer to tables */
  unsigned long int options;
  unsigned short int top_bracket;
  unsigned short int top_backref;
  unsigned short int first_byte;
  unsigned short int req_byte;
  unsigned short int name_entry_size; /* Size of any name items; 0 => none */
  unsigned short int name_count;      /* Number of name items */
} real_pcre;

/* The format of the block used to store data from pcre_study(). */

typedef struct pcre_study_data {
  size_t size;                        /* Total that was malloced */
  uschar options;
  uschar start_bits[32];
} pcre_study_data;

/* Structure for passing "static" information around between the functions
doing the compiling, so that they are thread-safe. */

typedef struct compile_data {
  const uschar *lcc;            /* Points to lower casing table */
  const uschar *fcc;            /* Points to case-flipping table */
  const uschar *cbits;          /* Points to character type table */
  const uschar *ctypes;         /* Points to table of type maps */
  const uschar *start_code;     /* The start of the compiled code */
  uschar *name_table;           /* The name/number table */
  int  names_found;             /* Number of entries so far */
  int  name_entry_size;         /* Size of each entry */
  int  top_backref;             /* Maximum back reference */
  unsigned int backref_map;     /* Bitmap of low back refs */
  int  req_varyopt;             /* "After variable item" flag for reqbyte */
} compile_data;

/* Structure for maintaining a chain of pointers to the currently incomplete
branches, for testing for left recursion. */

typedef struct branch_chain {
  struct branch_chain *outer;
  uschar *current;
} branch_chain;

/* Structure for items in a linked list that represents an explicit recursive
call within the pattern. */

typedef struct recursion_info {
  struct recursion_info *prevrec; /* Previous recursion record (or NULL) */
  int group_num;                /* Number of group that was called */
  const uschar *after_call;     /* "Return value": points after the call in the expr */
  const uschar *save_start;     /* Old value of md->start_match */
  int *offset_save;             /* Pointer to start of saved offsets */
  int saved_max;                /* Number of saved offsets */
} recursion_info;

/* When compiling in a mode that doesn't use recursive calls to match(),
a structure is used to remember local variables on the heap. It is defined in
pcre.c, close to the match() function, so that it is easy to keep it in step
with any changes of local variable. However, the pointer to the current frame
must be saved in some "static" place over a longjmp(). We declare the
structure here so that we can put a pointer in the match_data structure.
NOTE: This isn't used for a "normal" compilation of pcre. */

struct heapframe;

/* Structure for passing "static" information around between the functions
doing the matching, so that they are thread-safe. */

typedef struct match_data {
  unsigned long int match_call_count; /* As it says */
  unsigned long int match_limit;/* As it says */
  int   *offset_vector;         /* Offset vector */
  int    offset_end;            /* One past the end */
  int    offset_max;            /* The maximum usable for return data */
  const uschar *lcc;            /* Points to lower casing table */
  const uschar *ctypes;         /* Points to table of type maps */
  BOOL   offset_overflow;       /* Set if too many extractions */
  BOOL   notbol;                /* NOTBOL flag */
  BOOL   noteol;                /* NOTEOL flag */
  BOOL   utf8;                  /* UTF8 flag */
  BOOL   endonly;               /* Dollar not before final \n */
  BOOL   notempty;              /* Empty string match not wanted */
  const uschar *start_code;     /* For use when recursing */
  const uschar *start_subject;  /* Start of the subject string */
  const uschar *end_subject;    /* End of the subject string */
  const uschar *start_match;    /* Start of this match attempt */
  const uschar *end_match_ptr;  /* Subject position at end match */
  int    end_offset_top;        /* Highwater mark at end of match */
  int    capture_last;          /* Most recent capture number */
  int    start_offset;          /* The start offset value */
  recursion_info *recursive;    /* Linked list of recursion data */
  void  *callout_data;          /* To pass back to callouts */
  struct heapframe *thisframe;  /* Used only when compiling for no recursion */
} match_data;

/* Bit definitions for entries in the pcre_ctypes table. */

#define ctype_space   0x01
#define ctype_letter  0x02
#define ctype_digit   0x04
#define ctype_xdigit  0x08
#define ctype_word    0x10   /* alphameric or '_' */
#define ctype_meta    0x80   /* regexp meta char or zero (end pattern) */

/* Offsets for the bitmap tables in pcre_cbits. Each table contains a set
of bits for a class map. Some classes are built by combining these tables. */

#define cbit_space     0      /* [:space:] or \s */
#define cbit_xdigit   32      /* [:xdigit:] */
#define cbit_digit    64      /* [:digit:] or \d */
#define cbit_upper    96      /* [:upper:] */
#define cbit_lower   128      /* [:lower:] */
#define cbit_word    160      /* [:word:] or \w */
#define cbit_graph   192      /* [:graph:] */
#define cbit_print   224      /* [:print:] */
#define cbit_punct   256      /* [:punct:] */
#define cbit_cntrl   288      /* [:cntrl:] */
#define cbit_length  320      /* Length of the cbits table */

/* Offsets of the various tables from the base tables pointer, and
total length. */

#define lcc_offset      0
#define fcc_offset    256
#define cbits_offset  512
#define ctypes_offset (cbits_offset + cbit_length)
#define tables_length (ctypes_offset + 256)

/* End of internal.h */
