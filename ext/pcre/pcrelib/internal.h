/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/


/* This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2001 University of Cambridge

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
# include "php_config.h"
#endif

/* The value of NEWLINE determines the newline character. The default is to
leave it up to the compiler, but some sites want to force a particular value.
On Unix systems, "configure" can be used to override this default. */

#ifndef NEWLINE
#define NEWLINE '\n'
#endif
	
/* To cope with SunOS4 and other systems that lack memmove() but have bcopy(),
define a macro for memmove() if HAVE_MEMMOVE is false, provided that HAVE_BCOPY
is set. Otherwise, include an emulating function for those systems that have
neither (there some non-Unix environments where this is the case). This assumes
that all calls to memmove are moving strings upwards in store, which is the
case in PCRE. */

#if ! HAVE_MEMMOVE
#undef  memmove        /* some systems may have a macro */
#if HAVE_BCOPY
#define memmove(a, b, c) bcopy(b, a, c)
#else
void *
pcre_memmove(unsigned char *dest, const unsigned char *src, size_t n)
{
int i;
dest += n;
src += n;
for (i = 0; i < n; ++i) *(--dest) =  *(--src);
}
#define memmove(a, b, c) pcre_memmove(a, b, c)
#endif
#endif

/* Standard C headers plus the external interface definition */

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcre.h"

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

#define PCRE_FIRSTSET      0x40000000  /* first_char is set */
#define PCRE_REQCHSET      0x20000000  /* req_char is set */
#define PCRE_STARTLINE     0x10000000  /* start after \n for multiline */
#define PCRE_INGROUP       0x08000000  /* compiling inside a group */
#define PCRE_ICHANGED      0x04000000  /* i option changes within regex */

/* Options for the "extra" block produced by pcre_study(). */

#define PCRE_STUDY_MAPPED   0x01     /* a map of starting chars exists */

/* Masks for identifying the public options which are permitted at compile
time, run time or study time, respectively. */

#define PUBLIC_OPTIONS \
  (PCRE_CASELESS|PCRE_EXTENDED|PCRE_ANCHORED|PCRE_MULTILINE| \
   PCRE_DOTALL|PCRE_DOLLAR_ENDONLY|PCRE_EXTRA|PCRE_UNGREEDY|PCRE_UTF8)

#define PUBLIC_EXEC_OPTIONS \
  (PCRE_ANCHORED|PCRE_NOTBOL|PCRE_NOTEOL|PCRE_NOTEMPTY)

#define PUBLIC_STUDY_OPTIONS 0   /* None defined */

/* Magic number to provide a small check against being handed junk. */

#define MAGIC_NUMBER  0x50435245UL   /* 'PCRE' */

/* Miscellaneous definitions */

typedef int BOOL;

#define FALSE   0
#define TRUE    1

/* Escape items that are just an encoding of a particular data value. Note that
ESC_N is defined as yet another macro, which is set in config.h to either \n
(the default) or \r (which some people want). */

#ifndef ESC_E
#define ESC_E 27
#endif

#ifndef ESC_F
#define ESC_F '\f'
#endif

#ifndef ESC_N
#define ESC_N NEWLINE
#endif

#ifndef ESC_R
#define ESC_R '\r'
#endif

#ifndef ESC_T
#define ESC_T '\t'
#endif

/* These are escaped items that aren't just an encoding of a particular data
value such as \n. They must have non-zero values, as check_escape() returns
their negation. Also, they must appear in the same order as in the opcode
definitions below, up to ESC_z. The final one must be ESC_REF as subsequent
values are used for \1, \2, \3, etc. There is a test in the code for an escape
greater than ESC_b and less than ESC_Z to detect the types that may be
repeated. If any new escapes are put in-between that don't consume a character,
that code will have to change. */

enum { ESC_A = 1, ESC_B, ESC_b, ESC_D, ESC_d, ESC_S, ESC_s, ESC_W, ESC_w,
       ESC_Z, ESC_z, ESC_REF };

/* Opcode table: OP_BRA must be last, as all values >= it are used for brackets
that extract substrings. Starting from 1 (i.e. after OP_END), the values up to
OP_EOD must correspond in order to the list of escapes immediately above. */

enum {
  OP_END,            /* End of pattern */

  /* Values corresponding to backslashed metacharacters */

  OP_SOD,            /* Start of data: \A */
  OP_NOT_WORD_BOUNDARY,  /* \B */
  OP_WORD_BOUNDARY,      /* \b */
  OP_NOT_DIGIT,          /* \D */
  OP_DIGIT,              /* \d */
  OP_NOT_WHITESPACE,     /* \S */
  OP_WHITESPACE,         /* \s */
  OP_NOT_WORDCHAR,       /* \W */
  OP_WORDCHAR,           /* \w */
  OP_EODN,           /* End of data or \n at end of data: \Z. */
  OP_EOD,            /* End of data: \z */

  OP_OPT,            /* Set runtime options */
  OP_CIRC,           /* Start of line - varies with multiline switch */
  OP_DOLL,           /* End of line - varies with multiline switch */
  OP_ANY,            /* Match any character */
  OP_CHARS,          /* Match string of characters */
  OP_NOT,            /* Match anything but the following char */

  OP_STAR,           /* The maximizing and minimizing versions of */
  OP_MINSTAR,        /* all these opcodes must come in pairs, with */
  OP_PLUS,           /* the minimizing one second. */
  OP_MINPLUS,        /* This first set applies to single characters */
  OP_QUERY,
  OP_MINQUERY,
  OP_UPTO,           /* From 0 to n matches */
  OP_MINUPTO,
  OP_EXACT,          /* Exactly n matches */

  OP_NOTSTAR,        /* The maximizing and minimizing versions of */
  OP_NOTMINSTAR,     /* all these opcodes must come in pairs, with */
  OP_NOTPLUS,        /* the minimizing one second. */
  OP_NOTMINPLUS,     /* This first set applies to "not" single characters */
  OP_NOTQUERY,
  OP_NOTMINQUERY,
  OP_NOTUPTO,        /* From 0 to n matches */
  OP_NOTMINUPTO,
  OP_NOTEXACT,       /* Exactly n matches */

  OP_TYPESTAR,       /* The maximizing and minimizing versions of */
  OP_TYPEMINSTAR,    /* all these opcodes must come in pairs, with */
  OP_TYPEPLUS,       /* the minimizing one second. These codes must */
  OP_TYPEMINPLUS,    /* be in exactly the same order as those above. */
  OP_TYPEQUERY,      /* This set applies to character types such as \d */
  OP_TYPEMINQUERY,
  OP_TYPEUPTO,       /* From 0 to n matches */
  OP_TYPEMINUPTO,
  OP_TYPEEXACT,      /* Exactly n matches */

  OP_CRSTAR,         /* The maximizing and minimizing versions of */
  OP_CRMINSTAR,      /* all these opcodes must come in pairs, with */
  OP_CRPLUS,         /* the minimizing one second. These codes must */
  OP_CRMINPLUS,      /* be in exactly the same order as those above. */
  OP_CRQUERY,        /* These are for character classes and back refs */
  OP_CRMINQUERY,
  OP_CRRANGE,        /* These are different to the three seta above. */
  OP_CRMINRANGE,

  OP_CLASS,          /* Match a character class */
  OP_REF,            /* Match a back reference */
  OP_RECURSE,        /* Match this pattern recursively */

  OP_ALT,            /* Start of alternation */
  OP_KET,            /* End of group that doesn't have an unbounded repeat */
  OP_KETRMAX,        /* These two must remain together and in this */
  OP_KETRMIN,        /* order. They are for groups the repeat for ever. */

  /* The assertions must come before ONCE and COND */

  OP_ASSERT,         /* Positive lookahead */
  OP_ASSERT_NOT,     /* Negative lookahead */
  OP_ASSERTBACK,     /* Positive lookbehind */
  OP_ASSERTBACK_NOT, /* Negative lookbehind */
  OP_REVERSE,        /* Move pointer back - used in lookbehind assertions */

  /* ONCE and COND must come after the assertions, with ONCE first, as there's
  a test for >= ONCE for a subpattern that isn't an assertion. */

  OP_ONCE,           /* Once matched, don't back up into the subpattern */
  OP_COND,           /* Conditional group */
  OP_CREF,           /* Used to hold an extraction string number (cond ref) */

  OP_BRAZERO,        /* These two must remain together and in this */
  OP_BRAMINZERO,     /* order. */

  OP_BRANUMBER,      /* Used for extracting brackets whose number is greater
                        than can fit into an opcode. */

  OP_BRA             /* This and greater values are used for brackets that
                        extract substrings up to a basic limit. After that,
                        use is made of OP_BRANUMBER. */
};

/* The highest extraction number before we have to start using additional
bytes. (Originally PCRE didn't have support for extraction counts highter than
this number.) The value is limited by the number of opcodes left after OP_BRA,
i.e. 255 - OP_BRA. We actually set it a bit lower to leave room for additional
opcodes. */

#define EXTRACT_BASIC_MAX  150

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
#define ERR13 "unused error"
#define ERR14 "missing )"
#define ERR15 "back reference to non-existent subpattern"
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
#define ERR29 "(?p must be followed by )"
#define ERR30 "unknown POSIX class name"
#define ERR31 "POSIX collating elements are not supported"
#define ERR32 "this version of PCRE is not compiled with PCRE_UTF8 support"
#define ERR33 "characters with values > 255 are not yet supported in classes"
#define ERR34 "character value in \\x{...} sequence is too large"
#define ERR35 "invalid condition (?(0)"

/* All character handling must be done as unsigned characters. Otherwise there
are problems with top-bit-set characters and functions such as isspace().
However, we leave the interface to the outside world as char *, because that
should make things easier for callers. We define a short type for unsigned char
to save lots of typing. I tried "uchar", but it causes problems on Digital
Unix, where it is defined in sys/types, so use "uschar" instead. */

typedef unsigned char uschar;

/* The real format of the start of the pcre block; the actual code vector
runs on as long as necessary after the end. */

typedef struct real_pcre {
  unsigned long int magic_number;
  size_t size;
  const unsigned char *tables;
  unsigned long int options;
  unsigned short int top_bracket;
  unsigned short int top_backref;
  uschar first_char;
  uschar req_char;
  uschar code[1];
} real_pcre;

/* The real format of the extra block returned by pcre_study(). */

typedef struct real_pcre_extra {
  uschar options;
  uschar start_bits[32];
} real_pcre_extra;


/* Structure for passing "static" information around between the functions
doing the compiling, so that they are thread-safe. */

typedef struct compile_data {
  const uschar *lcc;            /* Points to lower casing table */
  const uschar *fcc;            /* Points to case-flipping table */
  const uschar *cbits;          /* Points to character type table */
  const uschar *ctypes;         /* Points to table of type maps */
} compile_data;

/* Structure for passing "static" information around between the functions
doing the matching, so that they are thread-safe. */

typedef struct match_data {
  int    errorcode;             /* As it says */
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
  const uschar *start_pattern;  /* For use when recursing */
  const uschar *start_subject;  /* Start of the subject string */
  const uschar *end_subject;    /* End of the subject string */
  const uschar *start_match;    /* Start of this match attempt */
  const uschar *end_match_ptr;  /* Subject position at end match */
  int    end_offset_top;        /* Highwater mark at end of match */
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
