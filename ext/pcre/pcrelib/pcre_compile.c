/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2008 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/


/* This module contains the external function pcre_compile(), along with
supporting internal functions that are not used by other modules. */


#include "config.h"

#define NLBLOCK cd             /* Block containing newline information */
#define PSSTART start_pattern  /* Field containing processed string start */
#define PSEND   end_pattern    /* Field containing processed string end */

#include "pcre_internal.h"


/* When DEBUG is defined, we need the pcre_printint() function, which is also
used by pcretest. DEBUG is not defined when building a production library. */

#ifdef DEBUG
#include "pcre_printint.src"
#endif


/* Macro for setting individual bits in class bitmaps. */

#define SETBIT(a,b) a[b/8] |= (1 << (b%8))

/* Maximum length value to check against when making sure that the integer that
holds the compiled pattern length does not overflow. We make it a bit less than
INT_MAX to allow for adding in group terminating bytes, so that we don't have
to check them every time. */

#define OFLOW_MAX (INT_MAX - 20)


/*************************************************
*      Code parameters and static tables         *
*************************************************/

/* This value specifies the size of stack workspace that is used during the
first pre-compile phase that determines how much memory is required. The regex
is partly compiled into this space, but the compiled parts are discarded as
soon as they can be, so that hopefully there will never be an overrun. The code
does, however, check for an overrun. The largest amount I've seen used is 218,
so this number is very generous.

The same workspace is used during the second, actual compile phase for
remembering forward references to groups so that they can be filled in at the
end. Each entry in this list occupies LINK_SIZE bytes, so even when LINK_SIZE
is 4 there is plenty of room. */

#define COMPILE_WORK_SIZE (4096)


/* Table for handling escaped characters in the range '0'-'z'. Positive returns
are simple data values; negative values are for special things like \d and so
on. Zero means further processing is needed (for things like \x), or the escape
is invalid. */

#ifndef EBCDIC  /* This is the "normal" table for ASCII systems */
static const short int escapes[] = {
     0,      0,      0,      0,      0,      0,      0,      0,   /* 0 - 7 */
     0,      0,    ':',    ';',    '<',    '=',    '>',    '?',   /* 8 - ? */
   '@', -ESC_A, -ESC_B, -ESC_C, -ESC_D, -ESC_E,      0, -ESC_G,   /* @ - G */
-ESC_H,      0,      0, -ESC_K,      0,      0,      0,      0,   /* H - O */
-ESC_P, -ESC_Q, -ESC_R, -ESC_S,      0,      0, -ESC_V, -ESC_W,   /* P - W */
-ESC_X,      0, -ESC_Z,    '[',   '\\',    ']',    '^',    '_',   /* X - _ */
   '`',      7, -ESC_b,      0, -ESC_d,  ESC_e,  ESC_f,      0,   /* ` - g */
-ESC_h,      0,      0, -ESC_k,      0,      0,  ESC_n,      0,   /* h - o */
-ESC_p,      0,  ESC_r, -ESC_s,  ESC_tee,    0, -ESC_v, -ESC_w,   /* p - w */
     0,      0, -ESC_z                                            /* x - z */
};

#else           /* This is the "abnormal" table for EBCDIC systems */
static const short int escapes[] = {
/*  48 */     0,     0,      0,     '.',    '<',   '(',    '+',    '|',
/*  50 */   '&',     0,      0,       0,      0,     0,      0,      0,
/*  58 */     0,     0,    '!',     '$',    '*',   ')',    ';',    '~',
/*  60 */   '-',   '/',      0,       0,      0,     0,      0,      0,
/*  68 */     0,     0,    '|',     ',',    '%',   '_',    '>',    '?',
/*  70 */     0,     0,      0,       0,      0,     0,      0,      0,
/*  78 */     0,   '`',    ':',     '#',    '@',  '\'',    '=',    '"',
/*  80 */     0,     7, -ESC_b,       0, -ESC_d, ESC_e,  ESC_f,      0,
/*  88 */-ESC_h,     0,      0,     '{',      0,     0,      0,      0,
/*  90 */     0,     0, -ESC_k,     'l',      0, ESC_n,      0, -ESC_p,
/*  98 */     0, ESC_r,      0,     '}',      0,     0,      0,      0,
/*  A0 */     0,   '~', -ESC_s, ESC_tee,      0,-ESC_v, -ESC_w,      0,
/*  A8 */     0,-ESC_z,      0,       0,      0,   '[',      0,      0,
/*  B0 */     0,     0,      0,       0,      0,     0,      0,      0,
/*  B8 */     0,     0,      0,       0,      0,   ']',    '=',    '-',
/*  C0 */   '{',-ESC_A, -ESC_B,  -ESC_C, -ESC_D,-ESC_E,      0, -ESC_G,
/*  C8 */-ESC_H,     0,      0,       0,      0,     0,      0,      0,
/*  D0 */   '}',     0, -ESC_K,       0,      0,     0,      0, -ESC_P,
/*  D8 */-ESC_Q,-ESC_R,      0,       0,      0,     0,      0,      0,
/*  E0 */  '\\',     0, -ESC_S,       0,      0,-ESC_V, -ESC_W, -ESC_X,
/*  E8 */     0,-ESC_Z,      0,       0,      0,     0,      0,      0,
/*  F0 */     0,     0,      0,       0,      0,     0,      0,      0,
/*  F8 */     0,     0,      0,       0,      0,     0,      0,      0
};
#endif


/* Table of special "verbs" like (*PRUNE). This is a short table, so it is
searched linearly. Put all the names into a single string, in order to reduce
the number of relocations when a shared library is dynamically linked. */

typedef struct verbitem {
  int   len;
  int   op;
} verbitem;

static const char verbnames[] =
  "ACCEPT\0"
  "COMMIT\0"
  "F\0"
  "FAIL\0"
  "PRUNE\0"
  "SKIP\0"
  "THEN";

static const verbitem verbs[] = {
  { 6, OP_ACCEPT },
  { 6, OP_COMMIT },
  { 1, OP_FAIL },
  { 4, OP_FAIL },
  { 5, OP_PRUNE },
  { 4, OP_SKIP  },
  { 4, OP_THEN  }
};

static const int verbcount = sizeof(verbs)/sizeof(verbitem);


/* Tables of names of POSIX character classes and their lengths. The names are
now all in a single string, to reduce the number of relocations when a shared
library is dynamically loaded. The list of lengths is terminated by a zero
length entry. The first three must be alpha, lower, upper, as this is assumed
for handling case independence. */

static const char posix_names[] =
  "alpha\0"  "lower\0"  "upper\0"  "alnum\0"  "ascii\0"  "blank\0"
  "cntrl\0"  "digit\0"  "graph\0"  "print\0"  "punct\0"  "space\0"
  "word\0"   "xdigit";

static const uschar posix_name_lengths[] = {
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 6, 0 };

/* Table of class bit maps for each POSIX class. Each class is formed from a
base map, with an optional addition or removal of another map. Then, for some
classes, there is some additional tweaking: for [:blank:] the vertical space
characters are removed, and for [:alpha:] and [:alnum:] the underscore
character is removed. The triples in the table consist of the base map offset,
second map offset or -1 if no second map, and a non-negative value for map
addition or a negative value for map subtraction (if there are two maps). The
absolute value of the third field has these meanings: 0 => no tweaking, 1 =>
remove vertical space characters, 2 => remove underscore. */

static const int posix_class_maps[] = {
  cbit_word,  cbit_digit, -2,             /* alpha */
  cbit_lower, -1,          0,             /* lower */
  cbit_upper, -1,          0,             /* upper */
  cbit_word,  -1,          2,             /* alnum - word without underscore */
  cbit_print, cbit_cntrl,  0,             /* ascii */
  cbit_space, -1,          1,             /* blank - a GNU extension */
  cbit_cntrl, -1,          0,             /* cntrl */
  cbit_digit, -1,          0,             /* digit */
  cbit_graph, -1,          0,             /* graph */
  cbit_print, -1,          0,             /* print */
  cbit_punct, -1,          0,             /* punct */
  cbit_space, -1,          0,             /* space */
  cbit_word,  -1,          0,             /* word - a Perl extension */
  cbit_xdigit,-1,          0              /* xdigit */
};


#define STRING(a)  # a
#define XSTRING(s) STRING(s)

/* The texts of compile-time error messages. These are "char *" because they
are passed to the outside world. Do not ever re-use any error number, because
they are documented. Always add a new error instead. Messages marked DEAD below
are no longer used. This used to be a table of strings, but in order to reduce
the number of relocations needed when a shared library is loaded dynamically,
it is now one long string. We cannot use a table of offsets, because the
lengths of inserts such as XSTRING(MAX_NAME_SIZE) are not known. Instead, we
simply count through to the one we want - this isn't a performance issue
because these strings are used only when there is a compilation error. */

static const char error_texts[] =
  "no error\0"
  "\\ at end of pattern\0"
  "\\c at end of pattern\0"
  "unrecognized character follows \\\0"
  "numbers out of order in {} quantifier\0"
  /* 5 */
  "number too big in {} quantifier\0"
  "missing terminating ] for character class\0"
  "invalid escape sequence in character class\0"
  "range out of order in character class\0"
  "nothing to repeat\0"
  /* 10 */
  "operand of unlimited repeat could match the empty string\0"  /** DEAD **/
  "internal error: unexpected repeat\0"
  "unrecognized character after (? or (?-\0"
  "POSIX named classes are supported only within a class\0"
  "missing )\0"
  /* 15 */
  "reference to non-existent subpattern\0"
  "erroffset passed as NULL\0"
  "unknown option bit(s) set\0"
  "missing ) after comment\0"
  "parentheses nested too deeply\0"  /** DEAD **/
  /* 20 */
  "regular expression is too large\0"
  "failed to get memory\0"
  "unmatched parentheses\0"
  "internal error: code overflow\0"
  "unrecognized character after (?<\0"
  /* 25 */
  "lookbehind assertion is not fixed length\0"
  "malformed number or name after (?(\0"
  "conditional group contains more than two branches\0"
  "assertion expected after (?(\0"
  "(?R or (?[+-]digits must be followed by )\0"
  /* 30 */
  "unknown POSIX class name\0"
  "POSIX collating elements are not supported\0"
  "this version of PCRE is not compiled with PCRE_UTF8 support\0"
  "spare error\0"  /** DEAD **/
  "character value in \\x{...} sequence is too large\0"
  /* 35 */
  "invalid condition (?(0)\0"
  "\\C not allowed in lookbehind assertion\0"
  "PCRE does not support \\L, \\l, \\N, \\U, or \\u\0"
  "number after (?C is > 255\0"
  "closing ) for (?C expected\0"
  /* 40 */
  "recursive call could loop indefinitely\0"
  "unrecognized character after (?P\0"
  "syntax error in subpattern name (missing terminator)\0"
  "two named subpatterns have the same name\0"
  "invalid UTF-8 string\0"
  /* 45 */
  "support for \\P, \\p, and \\X has not been compiled\0"
  "malformed \\P or \\p sequence\0"
  "unknown property name after \\P or \\p\0"
  "subpattern name is too long (maximum " XSTRING(MAX_NAME_SIZE) " characters)\0"
  "too many named subpatterns (maximum " XSTRING(MAX_NAME_COUNT) ")\0"
  /* 50 */
  "repeated subpattern is too long\0"    /** DEAD **/
  "octal value is greater than \\377 (not in UTF-8 mode)\0"
  "internal error: overran compiling workspace\0"
  "internal error: previously-checked referenced subpattern not found\0"
  "DEFINE group contains more than one branch\0"
  /* 55 */
  "repeating a DEFINE group is not allowed\0"
  "inconsistent NEWLINE options\0"
  "\\g is not followed by a braced, angle-bracketed, or quoted name/number or by a plain number\0"
  "a numbered reference must not be zero\0"
  "(*VERB) with an argument is not supported\0"
  /* 60 */
  "(*VERB) not recognized\0"
  "number is too big\0"
  "subpattern name expected\0"
  "digit expected after (?+\0"
  "] is an invalid data character in JavaScript compatibility mode";


/* Table to identify digits and hex digits. This is used when compiling
patterns. Note that the tables in chartables are dependent on the locale, and
may mark arbitrary characters as digits - but the PCRE compiling code expects
to handle only 0-9, a-z, and A-Z as digits when compiling. That is why we have
a private table here. It costs 256 bytes, but it is a lot faster than doing
character value tests (at least in some simple cases I timed), and in some
applications one wants PCRE to compile efficiently as well as match
efficiently.

For convenience, we use the same bit definitions as in chartables:

  0x04   decimal digit
  0x08   hexadecimal digit

Then we can use ctype_digit and ctype_xdigit in the code. */

#ifndef EBCDIC  /* This is the "normal" case, for ASCII systems */
static const unsigned char digitab[] =
  {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   0-  7 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   8- 15 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  16- 23 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*    - '  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  ( - /  */
  0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c, /*  0 - 7  */
  0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00, /*  8 - ?  */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /*  @ - G  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  H - O  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  P - W  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  X - _  */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /*  ` - g  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  h - o  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  p - w  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  x -127 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 128-135 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 136-143 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 144-151 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 152-159 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160-167 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 168-175 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 176-183 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 184-191 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 192-199 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 200-207 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 208-215 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 216-223 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 224-231 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 232-239 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 240-247 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};/* 248-255 */

#else           /* This is the "abnormal" case, for EBCDIC systems */
static const unsigned char digitab[] =
  {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   0-  7  0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   8- 15    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  16- 23 10 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  32- 39 20 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  40- 47    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  48- 55 30 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  56- 63    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*    - 71 40 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  72- |     */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  & - 87 50 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  88- 95    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  - -103 60 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 104- ?     */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 112-119 70 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 120- "     */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /* 128- g  80 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  h -143    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 144- p  90 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  q -159    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160- x  A0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  y -175    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  ^ -183 B0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 184-191    */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /*  { - G  C0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  H -207    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  } - P  D0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  Q -223    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  \ - X  E0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  Y -239    */
  0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c, /*  0 - 7  F0 */
  0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00};/*  8 -255    */

static const unsigned char ebcdic_chartab[] = { /* chartable partial dup */
  0x80,0x00,0x00,0x00,0x00,0x01,0x00,0x00, /*   0-  7 */
  0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00, /*   8- 15 */
  0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00, /*  16- 23 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31 */
  0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00, /*  32- 39 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  40- 47 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  48- 55 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  56- 63 */
  0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*    - 71 */
  0x00,0x00,0x00,0x80,0x00,0x80,0x80,0x80, /*  72- |  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  & - 87 */
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00, /*  88- 95 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  - -103 */
  0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x80, /* 104- ?  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 112-119 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 120- "  */
  0x00,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /* 128- g  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  h -143 */
  0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /* 144- p  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  q -159 */
  0x00,0x00,0x12,0x12,0x12,0x12,0x12,0x12, /* 160- x  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  y -175 */
  0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  ^ -183 */
  0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00, /* 184-191 */
  0x80,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /*  { - G  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  H -207 */
  0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  } - P  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  Q -223 */
  0x00,0x00,0x12,0x12,0x12,0x12,0x12,0x12, /*  \ - X  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  Y -239 */
  0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c, /*  0 - 7  */
  0x1c,0x1c,0x00,0x00,0x00,0x00,0x00,0x00};/*  8 -255 */
#endif


/* Definition to allow mutual recursion */

static BOOL
  compile_regex(int, int, uschar **, const uschar **, int *, BOOL, BOOL, int,
    int *, int *, branch_chain *, compile_data *, int *);



/*************************************************
*            Find an error text                  *
*************************************************/

/* The error texts are now all in one long string, to save on relocations. As
some of the text is of unknown length, we can't use a table of offsets.
Instead, just count through the strings. This is not a performance issue
because it happens only when there has been a compilation error.

Argument:   the error number
Returns:    pointer to the error string
*/

static const char *
find_error_text(int n)
{
const char *s = error_texts;
for (; n > 0; n--) while (*s++ != 0) {};
return s;
}


/*************************************************
*            Handle escapes                      *
*************************************************/

/* This function is called when a \ has been encountered. It either returns a
positive value for a simple escape such as \n, or a negative value which
encodes one of the more complicated things such as \d. A backreference to group
n is returned as -(ESC_REF + n); ESC_REF is the highest ESC_xxx macro. When
UTF-8 is enabled, a positive value greater than 255 may be returned. On entry,
ptr is pointing at the \. On exit, it is on the final character of the escape
sequence.

Arguments:
  ptrptr         points to the pattern position pointer
  errorcodeptr   points to the errorcode variable
  bracount       number of previous extracting brackets
  options        the options bits
  isclass        TRUE if inside a character class

Returns:         zero or positive => a data character
                 negative => a special escape sequence
                 on error, errorcodeptr is set
*/

static int
check_escape(const uschar **ptrptr, int *errorcodeptr, int bracount,
  int options, BOOL isclass)
{
BOOL utf8 = (options & PCRE_UTF8) != 0;
const uschar *ptr = *ptrptr + 1;
int c, i;

GETCHARINCTEST(c, ptr);           /* Get character value, increment pointer */
ptr--;                            /* Set pointer back to the last byte */

/* If backslash is at the end of the pattern, it's an error. */

if (c == 0) *errorcodeptr = ERR1;

/* Non-alphanumerics are literals. For digits or letters, do an initial lookup
in a table. A non-zero result is something that can be returned immediately.
Otherwise further processing may be required. */

#ifndef EBCDIC  /* ASCII coding */
else if (c < '0' || c > 'z') {}                           /* Not alphanumeric */
else if ((i = escapes[c - '0']) != 0) c = i;

#else           /* EBCDIC coding */
else if (c < 'a' || (ebcdic_chartab[c] & 0x0E) == 0) {}   /* Not alphanumeric */
else if ((i = escapes[c - 0x48]) != 0)  c = i;
#endif

/* Escapes that need further processing, or are illegal. */

else
  {
  const uschar *oldptr;
  BOOL braced, negated;

  switch (c)
    {
    /* A number of Perl escapes are not handled by PCRE. We give an explicit
    error. */

    case 'l':
    case 'L':
    case 'N':
    case 'u':
    case 'U':
    *errorcodeptr = ERR37;
    break;

    /* \g must be followed by one of a number of specific things:

    (1) A number, either plain or braced. If positive, it is an absolute
    backreference. If negative, it is a relative backreference. This is a Perl
    5.10 feature.

    (2) Perl 5.10 also supports \g{name} as a reference to a named group. This
    is part of Perl's movement towards a unified syntax for back references. As
    this is synonymous with \k{name}, we fudge it up by pretending it really
    was \k.

    (3) For Oniguruma compatibility we also support \g followed by a name or a
    number either in angle brackets or in single quotes. However, these are
    (possibly recursive) subroutine calls, _not_ backreferences. Just return
    the -ESC_g code (cf \k). */

    case 'g':
    if (ptr[1] == '<' || ptr[1] == '\'')
      {
      c = -ESC_g;
      break;
      }

    /* Handle the Perl-compatible cases */

    if (ptr[1] == '{')
      {
      const uschar *p;
      for (p = ptr+2; *p != 0 && *p != '}'; p++)
        if (*p != '-' && (digitab[*p] & ctype_digit) == 0) break;
      if (*p != 0 && *p != '}')
        {
        c = -ESC_k;
        break;
        }
      braced = TRUE;
      ptr++;
      }
    else braced = FALSE;

    if (ptr[1] == '-')
      {
      negated = TRUE;
      ptr++;
      }
    else negated = FALSE;

    c = 0;
    while ((digitab[ptr[1]] & ctype_digit) != 0)
      c = c * 10 + *(++ptr) - '0';

    if (c < 0)   /* Integer overflow */
      {
      *errorcodeptr = ERR61;
      break;
      }

    if (braced && *(++ptr) != '}')
      {
      *errorcodeptr = ERR57;
      break;
      }

    if (c == 0)
      {
      *errorcodeptr = ERR58;
      break;
      }

    if (negated)
      {
      if (c > bracount)
        {
        *errorcodeptr = ERR15;
        break;
        }
      c = bracount - (c - 1);
      }

    c = -(ESC_REF + c);
    break;

    /* The handling of escape sequences consisting of a string of digits
    starting with one that is not zero is not straightforward. By experiment,
    the way Perl works seems to be as follows:

    Outside a character class, the digits are read as a decimal number. If the
    number is less than 10, or if there are that many previous extracting
    left brackets, then it is a back reference. Otherwise, up to three octal
    digits are read to form an escaped byte. Thus \123 is likely to be octal
    123 (cf \0123, which is octal 012 followed by the literal 3). If the octal
    value is greater than 377, the least significant 8 bits are taken. Inside a
    character class, \ followed by a digit is always an octal number. */

    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':

    if (!isclass)
      {
      oldptr = ptr;
      c -= '0';
      while ((digitab[ptr[1]] & ctype_digit) != 0)
        c = c * 10 + *(++ptr) - '0';
      if (c < 0)    /* Integer overflow */
        {
        *errorcodeptr = ERR61;
        break;
        }
      if (c < 10 || c <= bracount)
        {
        c = -(ESC_REF + c);
        break;
        }
      ptr = oldptr;      /* Put the pointer back and fall through */
      }

    /* Handle an octal number following \. If the first digit is 8 or 9, Perl
    generates a binary zero byte and treats the digit as a following literal.
    Thus we have to pull back the pointer by one. */

    if ((c = *ptr) >= '8')
      {
      ptr--;
      c = 0;
      break;
      }

    /* \0 always starts an octal number, but we may drop through to here with a
    larger first octal digit. The original code used just to take the least
    significant 8 bits of octal numbers (I think this is what early Perls used
    to do). Nowadays we allow for larger numbers in UTF-8 mode, but no more
    than 3 octal digits. */

    case '0':
    c -= '0';
    while(i++ < 2 && ptr[1] >= '0' && ptr[1] <= '7')
        c = c * 8 + *(++ptr) - '0';
    if (!utf8 && c > 255) *errorcodeptr = ERR51;
    break;

    /* \x is complicated. \x{ddd} is a character number which can be greater
    than 0xff in utf8 mode, but only if the ddd are hex digits. If not, { is
    treated as a data character. */

    case 'x':
    if (ptr[1] == '{')
      {
      const uschar *pt = ptr + 2;
      int count = 0;

      c = 0;
      while ((digitab[*pt] & ctype_xdigit) != 0)
        {
        register int cc = *pt++;
        if (c == 0 && cc == '0') continue;     /* Leading zeroes */
        count++;

#ifndef EBCDIC  /* ASCII coding */
        if (cc >= 'a') cc -= 32;               /* Convert to upper case */
        c = (c << 4) + cc - ((cc < 'A')? '0' : ('A' - 10));
#else           /* EBCDIC coding */
        if (cc >= 'a' && cc <= 'z') cc += 64;  /* Convert to upper case */
        c = (c << 4) + cc - ((cc >= '0')? '0' : ('A' - 10));
#endif
        }

      if (*pt == '}')
        {
        if (c < 0 || count > (utf8? 8 : 2)) *errorcodeptr = ERR34;
        ptr = pt;
        break;
        }

      /* If the sequence of hex digits does not end with '}', then we don't
      recognize this construct; fall through to the normal \x handling. */
      }

    /* Read just a single-byte hex-defined char */

    c = 0;
    while (i++ < 2 && (digitab[ptr[1]] & ctype_xdigit) != 0)
      {
      int cc;                               /* Some compilers don't like ++ */
      cc = *(++ptr);                        /* in initializers */
#ifndef EBCDIC  /* ASCII coding */
      if (cc >= 'a') cc -= 32;              /* Convert to upper case */
      c = c * 16 + cc - ((cc < 'A')? '0' : ('A' - 10));
#else           /* EBCDIC coding */
      if (cc <= 'z') cc += 64;              /* Convert to upper case */
      c = c * 16 + cc - ((cc >= '0')? '0' : ('A' - 10));
#endif
      }
    break;

    /* For \c, a following letter is upper-cased; then the 0x40 bit is flipped.
    This coding is ASCII-specific, but then the whole concept of \cx is
    ASCII-specific. (However, an EBCDIC equivalent has now been added.) */

    case 'c':
    c = *(++ptr);
    if (c == 0)
      {
      *errorcodeptr = ERR2;
      break;
      }

#ifndef EBCDIC  /* ASCII coding */
    if (c >= 'a' && c <= 'z') c -= 32;
    c ^= 0x40;
#else           /* EBCDIC coding */
    if (c >= 'a' && c <= 'z') c += 64;
    c ^= 0xC0;
#endif
    break;

    /* PCRE_EXTRA enables extensions to Perl in the matter of escapes. Any
    other alphanumeric following \ is an error if PCRE_EXTRA was set;
    otherwise, for Perl compatibility, it is a literal. This code looks a bit
    odd, but there used to be some cases other than the default, and there may
    be again in future, so I haven't "optimized" it. */

    default:
    if ((options & PCRE_EXTRA) != 0) switch(c)
      {
      default:
      *errorcodeptr = ERR3;
      break;
      }
    break;
    }
  }

*ptrptr = ptr;
return c;
}



#ifdef SUPPORT_UCP
/*************************************************
*               Handle \P and \p                 *
*************************************************/

/* This function is called after \P or \p has been encountered, provided that
PCRE is compiled with support for Unicode properties. On entry, ptrptr is
pointing at the P or p. On exit, it is pointing at the final character of the
escape sequence.

Argument:
  ptrptr         points to the pattern position pointer
  negptr         points to a boolean that is set TRUE for negation else FALSE
  dptr           points to an int that is set to the detailed property value
  errorcodeptr   points to the error code variable

Returns:         type value from ucp_type_table, or -1 for an invalid type
*/

static int
get_ucp(const uschar **ptrptr, BOOL *negptr, int *dptr, int *errorcodeptr)
{
int c, i, bot, top;
const uschar *ptr = *ptrptr;
char name[32];

c = *(++ptr);
if (c == 0) goto ERROR_RETURN;

*negptr = FALSE;

/* \P or \p can be followed by a name in {}, optionally preceded by ^ for
negation. */

if (c == '{')
  {
  if (ptr[1] == '^')
    {
    *negptr = TRUE;
    ptr++;
    }
  for (i = 0; i < (int)sizeof(name) - 1; i++)
    {
    c = *(++ptr);
    if (c == 0) goto ERROR_RETURN;
    if (c == '}') break;
    name[i] = c;
    }
  if (c !='}') goto ERROR_RETURN;
  name[i] = 0;
  }

/* Otherwise there is just one following character */

else
  {
  name[0] = c;
  name[1] = 0;
  }

*ptrptr = ptr;

/* Search for a recognized property name using binary chop */

bot = 0;
top = _pcre_utt_size;

while (bot < top)
  {
  i = (bot + top) >> 1;
  c = strcmp(name, _pcre_utt_names + _pcre_utt[i].name_offset);
  if (c == 0)
    {
    *dptr = _pcre_utt[i].value;
    return _pcre_utt[i].type;
    }
  if (c > 0) bot = i + 1; else top = i;
  }

*errorcodeptr = ERR47;
*ptrptr = ptr;
return -1;

ERROR_RETURN:
*errorcodeptr = ERR46;
*ptrptr = ptr;
return -1;
}
#endif




/*************************************************
*            Check for counted repeat            *
*************************************************/

/* This function is called when a '{' is encountered in a place where it might
start a quantifier. It looks ahead to see if it really is a quantifier or not.
It is only a quantifier if it is one of the forms {ddd} {ddd,} or {ddd,ddd}
where the ddds are digits.

Arguments:
  p         pointer to the first char after '{'

Returns:    TRUE or FALSE
*/

static BOOL
is_counted_repeat(const uschar *p)
{
if ((digitab[*p++] & ctype_digit) == 0) return FALSE;
while ((digitab[*p] & ctype_digit) != 0) p++;
if (*p == '}') return TRUE;

if (*p++ != ',') return FALSE;
if (*p == '}') return TRUE;

if ((digitab[*p++] & ctype_digit) == 0) return FALSE;
while ((digitab[*p] & ctype_digit) != 0) p++;

return (*p == '}');
}



/*************************************************
*         Read repeat counts                     *
*************************************************/

/* Read an item of the form {n,m} and return the values. This is called only
after is_counted_repeat() has confirmed that a repeat-count quantifier exists,
so the syntax is guaranteed to be correct, but we need to check the values.

Arguments:
  p              pointer to first char after '{'
  minp           pointer to int for min
  maxp           pointer to int for max
                 returned as -1 if no max
  errorcodeptr   points to error code variable

Returns:         pointer to '}' on success;
                 current ptr on error, with errorcodeptr set non-zero
*/

static const uschar *
read_repeat_counts(const uschar *p, int *minp, int *maxp, int *errorcodeptr)
{
int min = 0;
int max = -1;

/* Read the minimum value and do a paranoid check: a negative value indicates
an integer overflow. */

while ((digitab[*p] & ctype_digit) != 0) min = min * 10 + *p++ - '0';
if (min < 0 || min > 65535)
  {
  *errorcodeptr = ERR5;
  return p;
  }

/* Read the maximum value if there is one, and again do a paranoid on its size.
Also, max must not be less than min. */

if (*p == '}') max = min; else
  {
  if (*(++p) != '}')
    {
    max = 0;
    while((digitab[*p] & ctype_digit) != 0) max = max * 10 + *p++ - '0';
    if (max < 0 || max > 65535)
      {
      *errorcodeptr = ERR5;
      return p;
      }
    if (max < min)
      {
      *errorcodeptr = ERR4;
      return p;
      }
    }
  }

/* Fill in the required variables, and pass back the pointer to the terminating
'}'. */

*minp = min;
*maxp = max;
return p;
}



/*************************************************
*       Find forward referenced subpattern       *
*************************************************/

/* This function scans along a pattern's text looking for capturing
subpatterns, and counting them. If it finds a named pattern that matches the
name it is given, it returns its number. Alternatively, if the name is NULL, it
returns when it reaches a given numbered subpattern. This is used for forward
references to subpatterns. We know that if (?P< is encountered, the name will
be terminated by '>' because that is checked in the first pass.

Arguments:
  ptr          current position in the pattern
  cd           compile background data
  name         name to seek, or NULL if seeking a numbered subpattern
  lorn         name length, or subpattern number if name is NULL
  xmode        TRUE if we are in /x mode

Returns:       the number of the named subpattern, or -1 if not found
*/

static int
find_parens(const uschar *ptr, compile_data *cd, const uschar *name, int lorn,
  BOOL xmode)
{
const uschar *thisname;
int count = cd->bracount;

for (; *ptr != 0; ptr++)
  {
  int term;

  /* Skip over backslashed characters and also entire \Q...\E */

  if (*ptr == '\\')
    {
    if (*(++ptr) == 0) return -1;
    if (*ptr == 'Q') for (;;)
      {
      while (*(++ptr) != 0 && *ptr != '\\') {};
      if (*ptr == 0) return -1;
      if (*(++ptr) == 'E') break;
      }
    continue;
    }

  /* Skip over character classes; this logic must be similar to the way they
  are handled for real. If the first character is '^', skip it. Also, if the
  first few characters (either before or after ^) are \Q\E or \E we skip them
  too. This makes for compatibility with Perl. */

  if (*ptr == '[')
    {
    BOOL negate_class = FALSE;
    for (;;)
      {
      int c = *(++ptr);
      if (c == '\\')
        {
        if (ptr[1] == 'E') ptr++;
          else if (strncmp((const char *)ptr+1, "Q\\E", 3) == 0) ptr += 3;
            else break;
        }
      else if (!negate_class && c == '^')
        negate_class = TRUE;
      else break;
      }

    /* If the next character is ']', it is a data character that must be
    skipped, except in JavaScript compatibility mode. */

    if (ptr[1] == ']' && (cd->external_options & PCRE_JAVASCRIPT_COMPAT) == 0)
      ptr++;

    while (*(++ptr) != ']')
      {
      if (*ptr == 0) return -1;
      if (*ptr == '\\')
        {
        if (*(++ptr) == 0) return -1;
        if (*ptr == 'Q') for (;;)
          {
          while (*(++ptr) != 0 && *ptr != '\\') {};
          if (*ptr == 0) return -1;
          if (*(++ptr) == 'E') break;
          }
        continue;
        }
      }
    continue;
    }

  /* Skip comments in /x mode */

  if (xmode && *ptr == '#')
    {
    while (*(++ptr) != 0 && *ptr != '\n') {};
    if (*ptr == 0) return -1;
    continue;
    }

  /* An opening parens must now be a real metacharacter */

  if (*ptr != '(') continue;
  if (ptr[1] != '?' && ptr[1] != '*')
    {
    count++;
    if (name == NULL && count == lorn) return count;
    continue;
    }

  ptr += 2;
  if (*ptr == 'P') ptr++;                      /* Allow optional P */

  /* We have to disambiguate (?<! and (?<= from (?<name> */

  if ((*ptr != '<' || ptr[1] == '!' || ptr[1] == '=') &&
       *ptr != '\'')
    continue;

  count++;

  if (name == NULL && count == lorn) return count;
  term = *ptr++;
  if (term == '<') term = '>';
  thisname = ptr;
  while (*ptr != term) ptr++;
  if (name != NULL && lorn == ptr - thisname &&
      strncmp((const char *)name, (const char *)thisname, lorn) == 0)
    return count;
  }

return -1;
}



/*************************************************
*      Find first significant op code            *
*************************************************/

/* This is called by several functions that scan a compiled expression looking
for a fixed first character, or an anchoring op code etc. It skips over things
that do not influence this. For some calls, a change of option is important.
For some calls, it makes sense to skip negative forward and all backward
assertions, and also the \b assertion; for others it does not.

Arguments:
  code         pointer to the start of the group
  options      pointer to external options
  optbit       the option bit whose changing is significant, or
                 zero if none are
  skipassert   TRUE if certain assertions are to be skipped

Returns:       pointer to the first significant opcode
*/

static const uschar*
first_significant_code(const uschar *code, int *options, int optbit,
  BOOL skipassert)
{
for (;;)
  {
  switch ((int)*code)
    {
    case OP_OPT:
    if (optbit > 0 && ((int)code[1] & optbit) != (*options & optbit))
      *options = (int)code[1];
    code += 2;
    break;

    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    if (!skipassert) return code;
    do code += GET(code, 1); while (*code == OP_ALT);
    code += _pcre_OP_lengths[*code];
    break;

    case OP_WORD_BOUNDARY:
    case OP_NOT_WORD_BOUNDARY:
    if (!skipassert) return code;
    /* Fall through */

    case OP_CALLOUT:
    case OP_CREF:
    case OP_RREF:
    case OP_DEF:
    code += _pcre_OP_lengths[*code];
    break;

    default:
    return code;
    }
  }
/* Control never reaches here */
}




/*************************************************
*        Find the fixed length of a pattern      *
*************************************************/

/* Scan a pattern and compute the fixed length of subject that will match it,
if the length is fixed. This is needed for dealing with backward assertions.
In UTF8 mode, the result is in characters rather than bytes.

Arguments:
  code     points to the start of the pattern (the bracket)
  options  the compiling options

Returns:   the fixed length, or -1 if there is no fixed length,
             or -2 if \C was encountered
*/

static int
find_fixedlength(uschar *code, int options)
{
int length = -1;

register int branchlength = 0;
register uschar *cc = code + 1 + LINK_SIZE;

/* Scan along the opcodes for this branch. If we get to the end of the
branch, check the length against that of the other branches. */

for (;;)
  {
  int d;
  register int op = *cc;
  switch (op)
    {
    case OP_CBRA:
    case OP_BRA:
    case OP_ONCE:
    case OP_COND:
    d = find_fixedlength(cc + ((op == OP_CBRA)? 2:0), options);
    if (d < 0) return d;
    branchlength += d;
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    cc += 1 + LINK_SIZE;
    break;

    /* Reached end of a branch; if it's a ket it is the end of a nested
    call. If it's ALT it is an alternation in a nested call. If it is
    END it's the end of the outer call. All can be handled by the same code. */

    case OP_ALT:
    case OP_KET:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_END:
    if (length < 0) length = branchlength;
      else if (length != branchlength) return -1;
    if (*cc != OP_ALT) return length;
    cc += 1 + LINK_SIZE;
    branchlength = 0;
    break;

    /* Skip over assertive subpatterns */

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    /* Fall through */

    /* Skip over things that don't match chars */

    case OP_REVERSE:
    case OP_CREF:
    case OP_RREF:
    case OP_DEF:
    case OP_OPT:
    case OP_CALLOUT:
    case OP_SOD:
    case OP_SOM:
    case OP_EOD:
    case OP_EODN:
    case OP_CIRC:
    case OP_DOLL:
    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
    cc += _pcre_OP_lengths[*cc];
    break;

    /* Handle literal characters */

    case OP_CHAR:
    case OP_CHARNC:
    case OP_NOT:
    branchlength++;
    cc += 2;
#ifdef SUPPORT_UTF8
    if ((options & PCRE_UTF8) != 0)
      {
      while ((*cc & 0xc0) == 0x80) cc++;
      }
#endif
    break;

    /* Handle exact repetitions. The count is already in characters, but we
    need to skip over a multibyte character in UTF8 mode.  */

    case OP_EXACT:
    branchlength += GET2(cc,1);
    cc += 4;
#ifdef SUPPORT_UTF8
    if ((options & PCRE_UTF8) != 0)
      {
      while((*cc & 0x80) == 0x80) cc++;
      }
#endif
    break;

    case OP_TYPEEXACT:
    branchlength += GET2(cc,1);
    if (cc[3] == OP_PROP || cc[3] == OP_NOTPROP) cc += 2;
    cc += 4;
    break;

    /* Handle single-char matchers */

    case OP_PROP:
    case OP_NOTPROP:
    cc += 2;
    /* Fall through */

    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    branchlength++;
    cc++;
    break;

    /* The single-byte matcher isn't allowed */

    case OP_ANYBYTE:
    return -2;

    /* Check a class for variable quantification */

#ifdef SUPPORT_UTF8
    case OP_XCLASS:
    cc += GET(cc, 1) - 33;
    /* Fall through */
#endif

    case OP_CLASS:
    case OP_NCLASS:
    cc += 33;

    switch (*cc)
      {
      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      return -1;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      if (GET2(cc,1) != GET2(cc,3)) return -1;
      branchlength += GET2(cc,1);
      cc += 5;
      break;

      default:
      branchlength++;
      }
    break;

    /* Anything else is variable length */

    default:
    return -1;
    }
  }
/* Control never gets here */
}




/*************************************************
*    Scan compiled regex for numbered bracket    *
*************************************************/

/* This little function scans through a compiled pattern until it finds a
capturing bracket with the given number.

Arguments:
  code        points to start of expression
  utf8        TRUE in UTF-8 mode
  number      the required bracket number

Returns:      pointer to the opcode for the bracket, or NULL if not found
*/

static const uschar *
find_bracket(const uschar *code, BOOL utf8, int number)
{
for (;;)
  {
  register int c = *code;
  if (c == OP_END) return NULL;

  /* XCLASS is used for classes that cannot be represented just by a bit
  map. This includes negated single high-valued characters. The length in
  the table is zero; the actual length is stored in the compiled code. */

  if (c == OP_XCLASS) code += GET(code, 1);

  /* Handle capturing bracket */

  else if (c == OP_CBRA)
    {
    int n = GET2(code, 1+LINK_SIZE);
    if (n == number) return (uschar *)code;
    code += _pcre_OP_lengths[c];
    }

  /* Otherwise, we can get the item's length from the table, except that for
  repeated character types, we have to test for \p and \P, which have an extra
  two bytes of parameters. */

  else
    {
    switch(c)
      {
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSSTAR:
      case OP_TYPEPOSPLUS:
      case OP_TYPEPOSQUERY:
      if (code[1] == OP_PROP || code[1] == OP_NOTPROP) code += 2;
      break;

      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEEXACT:
      case OP_TYPEPOSUPTO:
      if (code[3] == OP_PROP || code[3] == OP_NOTPROP) code += 2;
      break;
      }

    /* Add in the fixed length from the table */

    code += _pcre_OP_lengths[c];

  /* In UTF-8 mode, opcodes that are followed by a character may be followed by
  a multi-byte character. The length in the table is a minimum, so we have to
  arrange to skip the extra bytes. */

#ifdef SUPPORT_UTF8
    if (utf8) switch(c)
      {
      case OP_CHAR:
      case OP_CHARNC:
      case OP_EXACT:
      case OP_UPTO:
      case OP_MINUPTO:
      case OP_POSUPTO:
      case OP_STAR:
      case OP_MINSTAR:
      case OP_POSSTAR:
      case OP_PLUS:
      case OP_MINPLUS:
      case OP_POSPLUS:
      case OP_QUERY:
      case OP_MINQUERY:
      case OP_POSQUERY:
      if (code[-1] >= 0xc0) code += _pcre_utf8_table4[code[-1] & 0x3f];
      break;
      }
#else
    (void)(utf8);  /* Keep compiler happy by referencing function argument */
#endif
    }
  }
}



/*************************************************
*   Scan compiled regex for recursion reference  *
*************************************************/

/* This little function scans through a compiled pattern until it finds an
instance of OP_RECURSE.

Arguments:
  code        points to start of expression
  utf8        TRUE in UTF-8 mode

Returns:      pointer to the opcode for OP_RECURSE, or NULL if not found
*/

static const uschar *
find_recurse(const uschar *code, BOOL utf8)
{
for (;;)
  {
  register int c = *code;
  if (c == OP_END) return NULL;
  if (c == OP_RECURSE) return code;

  /* XCLASS is used for classes that cannot be represented just by a bit
  map. This includes negated single high-valued characters. The length in
  the table is zero; the actual length is stored in the compiled code. */

  if (c == OP_XCLASS) code += GET(code, 1);

  /* Otherwise, we can get the item's length from the table, except that for
  repeated character types, we have to test for \p and \P, which have an extra
  two bytes of parameters. */

  else
    {
    switch(c)
      {
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSSTAR:
      case OP_TYPEPOSPLUS:
      case OP_TYPEPOSQUERY:
      if (code[1] == OP_PROP || code[1] == OP_NOTPROP) code += 2;
      break;

      case OP_TYPEPOSUPTO:
      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEEXACT:
      if (code[3] == OP_PROP || code[3] == OP_NOTPROP) code += 2;
      break;
      }

    /* Add in the fixed length from the table */

    code += _pcre_OP_lengths[c];

    /* In UTF-8 mode, opcodes that are followed by a character may be followed
    by a multi-byte character. The length in the table is a minimum, so we have
    to arrange to skip the extra bytes. */

#ifdef SUPPORT_UTF8
    if (utf8) switch(c)
      {
      case OP_CHAR:
      case OP_CHARNC:
      case OP_EXACT:
      case OP_UPTO:
      case OP_MINUPTO:
      case OP_POSUPTO:
      case OP_STAR:
      case OP_MINSTAR:
      case OP_POSSTAR:
      case OP_PLUS:
      case OP_MINPLUS:
      case OP_POSPLUS:
      case OP_QUERY:
      case OP_MINQUERY:
      case OP_POSQUERY:
      if (code[-1] >= 0xc0) code += _pcre_utf8_table4[code[-1] & 0x3f];
      break;
      }
#else
    (void)(utf8);  /* Keep compiler happy by referencing function argument */
#endif
    }
  }
}



/*************************************************
*    Scan compiled branch for non-emptiness      *
*************************************************/

/* This function scans through a branch of a compiled pattern to see whether it
can match the empty string or not. It is called from could_be_empty()
below and from compile_branch() when checking for an unlimited repeat of a
group that can match nothing. Note that first_significant_code() skips over
backward and negative forward assertions when its final argument is TRUE. If we
hit an unclosed bracket, we return "empty" - this means we've struck an inner
bracket whose current branch will already have been scanned.

Arguments:
  code        points to start of search
  endcode     points to where to stop
  utf8        TRUE if in UTF8 mode

Returns:      TRUE if what is matched could be empty
*/

static BOOL
could_be_empty_branch(const uschar *code, const uschar *endcode, BOOL utf8)
{
register int c;
for (code = first_significant_code(code + _pcre_OP_lengths[*code], NULL, 0, TRUE);
     code < endcode;
     code = first_significant_code(code + _pcre_OP_lengths[c], NULL, 0, TRUE))
  {
  const uschar *ccode;

  c = *code;

  /* Skip over forward assertions; the other assertions are skipped by
  first_significant_code() with a TRUE final argument. */

  if (c == OP_ASSERT)
    {
    do code += GET(code, 1); while (*code == OP_ALT);
    c = *code;
    continue;
    }

  /* Groups with zero repeats can of course be empty; skip them. */

  if (c == OP_BRAZERO || c == OP_BRAMINZERO || c == OP_SKIPZERO)
    {
    code += _pcre_OP_lengths[c];
    do code += GET(code, 1); while (*code == OP_ALT);
    c = *code;
    continue;
    }

  /* For other groups, scan the branches. */

  if (c == OP_BRA || c == OP_CBRA || c == OP_ONCE || c == OP_COND)
    {
    BOOL empty_branch;
    if (GET(code, 1) == 0) return TRUE;    /* Hit unclosed bracket */

    /* Scan a closed bracket */

    empty_branch = FALSE;
    do
      {
      if (!empty_branch && could_be_empty_branch(code, endcode, utf8))
        empty_branch = TRUE;
      code += GET(code, 1);
      }
    while (*code == OP_ALT);
    if (!empty_branch) return FALSE;   /* All branches are non-empty */
    c = *code;
    continue;
    }

  /* Handle the other opcodes */

  switch (c)
    {
    /* Check for quantifiers after a class. XCLASS is used for classes that
    cannot be represented just by a bit map. This includes negated single
    high-valued characters. The length in _pcre_OP_lengths[] is zero; the
    actual length is stored in the compiled code, so we must update "code"
    here. */

#ifdef SUPPORT_UTF8
    case OP_XCLASS:
    ccode = code += GET(code, 1);
    goto CHECK_CLASS_REPEAT;
#endif

    case OP_CLASS:
    case OP_NCLASS:
    ccode = code + 33;

#ifdef SUPPORT_UTF8
    CHECK_CLASS_REPEAT:
#endif

    switch (*ccode)
      {
      case OP_CRSTAR:            /* These could be empty; continue */
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      break;

      default:                   /* Non-repeat => class must match */
      case OP_CRPLUS:            /* These repeats aren't empty */
      case OP_CRMINPLUS:
      return FALSE;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      if (GET2(ccode, 1) > 0) return FALSE;  /* Minimum > 0 */
      break;
      }
    break;

    /* Opcodes that must match a character */

    case OP_PROP:
    case OP_NOTPROP:
    case OP_EXTUNI:
    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    case OP_ANYBYTE:
    case OP_CHAR:
    case OP_CHARNC:
    case OP_NOT:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_POSPLUS:
    case OP_EXACT:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTPOSPLUS:
    case OP_NOTEXACT:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEPOSPLUS:
    case OP_TYPEEXACT:
    return FALSE;

    /* These are going to continue, as they may be empty, but we have to
    fudge the length for the \p and \P cases. */

    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPOSSTAR:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    case OP_TYPEPOSQUERY:
    if (code[1] == OP_PROP || code[1] == OP_NOTPROP) code += 2;
    break;

    /* Same for these */

    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    case OP_TYPEPOSUPTO:
    if (code[3] == OP_PROP || code[3] == OP_NOTPROP) code += 2;
    break;

    /* End of branch */

    case OP_KET:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_ALT:
    return TRUE;

    /* In UTF-8 mode, STAR, MINSTAR, POSSTAR, QUERY, MINQUERY, POSQUERY, UPTO,
    MINUPTO, and POSUPTO may be followed by a multibyte character */

#ifdef SUPPORT_UTF8
    case OP_STAR:
    case OP_MINSTAR:
    case OP_POSSTAR:
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_POSQUERY:
    case OP_UPTO:
    case OP_MINUPTO:
    case OP_POSUPTO:
    if (utf8) while ((code[2] & 0xc0) == 0x80) code++;
    break;
#endif
    }
  }

return TRUE;
}



/*************************************************
*    Scan compiled regex for non-emptiness       *
*************************************************/

/* This function is called to check for left recursive calls. We want to check
the current branch of the current pattern to see if it could match the empty
string. If it could, we must look outwards for branches at other levels,
stopping when we pass beyond the bracket which is the subject of the recursion.

Arguments:
  code        points to start of the recursion
  endcode     points to where to stop (current RECURSE item)
  bcptr       points to the chain of current (unclosed) branch starts
  utf8        TRUE if in UTF-8 mode

Returns:      TRUE if what is matched could be empty
*/

static BOOL
could_be_empty(const uschar *code, const uschar *endcode, branch_chain *bcptr,
  BOOL utf8)
{
while (bcptr != NULL && bcptr->current >= code)
  {
  if (!could_be_empty_branch(bcptr->current, endcode, utf8)) return FALSE;
  bcptr = bcptr->outer;
  }
return TRUE;
}



/*************************************************
*           Check for POSIX class syntax         *
*************************************************/

/* This function is called when the sequence "[:" or "[." or "[=" is
encountered in a character class. It checks whether this is followed by a
sequence of characters terminated by a matching ":]" or ".]" or "=]". If we
reach an unescaped ']' without the special preceding character, return FALSE.

Originally, this function only recognized a sequence of letters between the
terminators, but it seems that Perl recognizes any sequence of characters,
though of course unknown POSIX names are subsequently rejected. Perl gives an
"Unknown POSIX class" error for [:f\oo:] for example, where previously PCRE
didn't consider this to be a POSIX class. Likewise for [:1234:].

The problem in trying to be exactly like Perl is in the handling of escapes. We
have to be sure that [abc[:x\]pqr] is *not* treated as containing a POSIX
class, but [abc[:x\]pqr:]] is (so that an error can be generated). The code
below handles the special case of \], but does not try to do any other escape
processing. This makes it different from Perl for cases such as [:l\ower:]
where Perl recognizes it as the POSIX class "lower" but PCRE does not recognize
"l\ower". This is a lesser evil that not diagnosing bad classes when Perl does,
I think.

Arguments:
  ptr      pointer to the initial [
  endptr   where to return the end pointer

Returns:   TRUE or FALSE
*/

static BOOL
check_posix_syntax(const uschar *ptr, const uschar **endptr)
{
int terminator;          /* Don't combine these lines; the Solaris cc */
terminator = *(++ptr);   /* compiler warns about "non-constant" initializer. */
for (++ptr; *ptr != 0; ptr++)
  {
  if (*ptr == '\\' && ptr[1] == ']') ptr++; else
    {
    if (*ptr == ']') return FALSE;
    if (*ptr == terminator && ptr[1] == ']')
      {
      *endptr = ptr;
      return TRUE;
      }
    }
  }
return FALSE;
}




/*************************************************
*          Check POSIX class name                *
*************************************************/

/* This function is called to check the name given in a POSIX-style class entry
such as [:alnum:].

Arguments:
  ptr        points to the first letter
  len        the length of the name

Returns:     a value representing the name, or -1 if unknown
*/

static int
check_posix_name(const uschar *ptr, int len)
{
const char *pn = posix_names;
register int yield = 0;
while (posix_name_lengths[yield] != 0)
  {
  if (len == posix_name_lengths[yield] &&
    strncmp((const char *)ptr, pn, len) == 0) return yield;
  pn += posix_name_lengths[yield] + 1;
  yield++;
  }
return -1;
}


/*************************************************
*    Adjust OP_RECURSE items in repeated group   *
*************************************************/

/* OP_RECURSE items contain an offset from the start of the regex to the group
that is referenced. This means that groups can be replicated for fixed
repetition simply by copying (because the recursion is allowed to refer to
earlier groups that are outside the current group). However, when a group is
optional (i.e. the minimum quantifier is zero), OP_BRAZERO or OP_SKIPZERO is
inserted before it, after it has been compiled. This means that any OP_RECURSE
items within it that refer to the group itself or any contained groups have to
have their offsets adjusted. That one of the jobs of this function. Before it
is called, the partially compiled regex must be temporarily terminated with
OP_END.

This function has been extended with the possibility of forward references for
recursions and subroutine calls. It must also check the list of such references
for the group we are dealing with. If it finds that one of the recursions in
the current group is on this list, it adjusts the offset in the list, not the
value in the reference (which is a group number).

Arguments:
  group      points to the start of the group
  adjust     the amount by which the group is to be moved
  utf8       TRUE in UTF-8 mode
  cd         contains pointers to tables etc.
  save_hwm   the hwm forward reference pointer at the start of the group

Returns:     nothing
*/

static void
adjust_recurse(uschar *group, int adjust, BOOL utf8, compile_data *cd,
  uschar *save_hwm)
{
uschar *ptr = group;

while ((ptr = (uschar *)find_recurse(ptr, utf8)) != NULL)
  {
  int offset;
  uschar *hc;

  /* See if this recursion is on the forward reference list. If so, adjust the
  reference. */

  for (hc = save_hwm; hc < cd->hwm; hc += LINK_SIZE)
    {
    offset = GET(hc, 0);
    if (cd->start_code + offset == ptr + 1)
      {
      PUT(hc, 0, offset + adjust);
      break;
      }
    }

  /* Otherwise, adjust the recursion offset if it's after the start of this
  group. */

  if (hc >= cd->hwm)
    {
    offset = GET(ptr, 1);
    if (cd->start_code + offset >= group) PUT(ptr, 1, offset + adjust);
    }

  ptr += 1 + LINK_SIZE;
  }
}



/*************************************************
*        Insert an automatic callout point       *
*************************************************/

/* This function is called when the PCRE_AUTO_CALLOUT option is set, to insert
callout points before each pattern item.

Arguments:
  code           current code pointer
  ptr            current pattern pointer
  cd             pointers to tables etc

Returns:         new code pointer
*/

static uschar *
auto_callout(uschar *code, const uschar *ptr, compile_data *cd)
{
*code++ = OP_CALLOUT;
*code++ = 255;
PUT(code, 0, ptr - cd->start_pattern);  /* Pattern offset */
PUT(code, LINK_SIZE, 0);                /* Default length */
return code + 2*LINK_SIZE;
}



/*************************************************
*         Complete a callout item                *
*************************************************/

/* A callout item contains the length of the next item in the pattern, which
we can't fill in till after we have reached the relevant point. This is used
for both automatic and manual callouts.

Arguments:
  previous_callout   points to previous callout item
  ptr                current pattern pointer
  cd                 pointers to tables etc

Returns:             nothing
*/

static void
complete_callout(uschar *previous_callout, const uschar *ptr, compile_data *cd)
{
int length = ptr - cd->start_pattern - GET(previous_callout, 2);
PUT(previous_callout, 2 + LINK_SIZE, length);
}



#ifdef SUPPORT_UCP
/*************************************************
*           Get othercase range                  *
*************************************************/

/* This function is passed the start and end of a class range, in UTF-8 mode
with UCP support. It searches up the characters, looking for internal ranges of
characters in the "other" case. Each call returns the next one, updating the
start address.

Arguments:
  cptr        points to starting character value; updated
  d           end value
  ocptr       where to put start of othercase range
  odptr       where to put end of othercase range

Yield:        TRUE when range returned; FALSE when no more
*/

static BOOL
get_othercase_range(unsigned int *cptr, unsigned int d, unsigned int *ocptr,
  unsigned int *odptr)
{
unsigned int c, othercase, next;

for (c = *cptr; c <= d; c++)
  { if ((othercase = UCD_OTHERCASE(c)) != c) break; }

if (c > d) return FALSE;

*ocptr = othercase;
next = othercase + 1;

for (++c; c <= d; c++)
  {
  if (UCD_OTHERCASE(c) != next) break;
  next++;
  }

*odptr = next - 1;
*cptr = c;

return TRUE;
}
#endif  /* SUPPORT_UCP */



/*************************************************
*     Check if auto-possessifying is possible    *
*************************************************/

/* This function is called for unlimited repeats of certain items, to see
whether the next thing could possibly match the repeated item. If not, it makes
sense to automatically possessify the repeated item.

Arguments:
  op_code       the repeated op code
  this          data for this item, depends on the opcode
  utf8          TRUE in UTF-8 mode
  utf8_char     used for utf8 character bytes, NULL if not relevant
  ptr           next character in pattern
  options       options bits
  cd            contains pointers to tables etc.

Returns:        TRUE if possessifying is wanted
*/

static BOOL
check_auto_possessive(int op_code, int item, BOOL utf8, uschar *utf8_char,
  const uschar *ptr, int options, compile_data *cd)
{
int next;

/* Skip whitespace and comments in extended mode */

if ((options & PCRE_EXTENDED) != 0)
  {
  for (;;)
    {
    while ((cd->ctypes[*ptr] & ctype_space) != 0) ptr++;
    if (*ptr == '#')
      {
      while (*(++ptr) != 0)
        if (IS_NEWLINE(ptr)) { ptr += cd->nllen; break; }
      }
    else break;
    }
  }

/* If the next item is one that we can handle, get its value. A non-negative
value is a character, a negative value is an escape value. */

if (*ptr == '\\')
  {
  int temperrorcode = 0;
  next = check_escape(&ptr, &temperrorcode, cd->bracount, options, FALSE);
  if (temperrorcode != 0) return FALSE;
  ptr++;    /* Point after the escape sequence */
  }

else if ((cd->ctypes[*ptr] & ctype_meta) == 0)
  {
#ifdef SUPPORT_UTF8
  if (utf8) { GETCHARINC(next, ptr); } else
#endif
  next = *ptr++;
  }

else return FALSE;

/* Skip whitespace and comments in extended mode */

if ((options & PCRE_EXTENDED) != 0)
  {
  for (;;)
    {
    while ((cd->ctypes[*ptr] & ctype_space) != 0) ptr++;
    if (*ptr == '#')
      {
      while (*(++ptr) != 0)
        if (IS_NEWLINE(ptr)) { ptr += cd->nllen; break; }
      }
    else break;
    }
  }

/* If the next thing is itself optional, we have to give up. */

if (*ptr == '*' || *ptr == '?' || strncmp((char *)ptr, "{0,", 3) == 0)
  return FALSE;

/* Now compare the next item with the previous opcode. If the previous is a
positive single character match, "item" either contains the character or, if
"item" is greater than 127 in utf8 mode, the character's bytes are in
utf8_char. */


/* Handle cases when the next item is a character. */

if (next >= 0) switch(op_code)
  {
  case OP_CHAR:
#ifdef SUPPORT_UTF8
  if (utf8 && item > 127) { GETCHAR(item, utf8_char); }
#else
  (void)(utf8_char);  /* Keep compiler happy by referencing function argument */
#endif
  return item != next;

  /* For CHARNC (caseless character) we must check the other case. If we have
  Unicode property support, we can use it to test the other case of
  high-valued characters. */

  case OP_CHARNC:
#ifdef SUPPORT_UTF8
  if (utf8 && item > 127) { GETCHAR(item, utf8_char); }
#endif
  if (item == next) return FALSE;
#ifdef SUPPORT_UTF8
  if (utf8)
    {
    unsigned int othercase;
    if (next < 128) othercase = cd->fcc[next]; else
#ifdef SUPPORT_UCP
    othercase = UCD_OTHERCASE((unsigned int)next);
#else
    othercase = NOTACHAR;
#endif
    return (unsigned int)item != othercase;
    }
  else
#endif  /* SUPPORT_UTF8 */
  return (item != cd->fcc[next]);  /* Non-UTF-8 mode */

  /* For OP_NOT, "item" must be a single-byte character. */

  case OP_NOT:
  if (item == next) return TRUE;
  if ((options & PCRE_CASELESS) == 0) return FALSE;
#ifdef SUPPORT_UTF8
  if (utf8)
    {
    unsigned int othercase;
    if (next < 128) othercase = cd->fcc[next]; else
#ifdef SUPPORT_UCP
    othercase = UCD_OTHERCASE(next);
#else
    othercase = NOTACHAR;
#endif
    return (unsigned int)item == othercase;
    }
  else
#endif  /* SUPPORT_UTF8 */
  return (item == cd->fcc[next]);  /* Non-UTF-8 mode */

  case OP_DIGIT:
  return next > 127 || (cd->ctypes[next] & ctype_digit) == 0;

  case OP_NOT_DIGIT:
  return next <= 127 && (cd->ctypes[next] & ctype_digit) != 0;

  case OP_WHITESPACE:
  return next > 127 || (cd->ctypes[next] & ctype_space) == 0;

  case OP_NOT_WHITESPACE:
  return next <= 127 && (cd->ctypes[next] & ctype_space) != 0;

  case OP_WORDCHAR:
  return next > 127 || (cd->ctypes[next] & ctype_word) == 0;

  case OP_NOT_WORDCHAR:
  return next <= 127 && (cd->ctypes[next] & ctype_word) != 0;

  case OP_HSPACE:
  case OP_NOT_HSPACE:
  switch(next)
    {
    case 0x09:
    case 0x20:
    case 0xa0:
    case 0x1680:
    case 0x180e:
    case 0x2000:
    case 0x2001:
    case 0x2002:
    case 0x2003:
    case 0x2004:
    case 0x2005:
    case 0x2006:
    case 0x2007:
    case 0x2008:
    case 0x2009:
    case 0x200A:
    case 0x202f:
    case 0x205f:
    case 0x3000:
    return op_code != OP_HSPACE;
    default:
    return op_code == OP_HSPACE;
    }

  case OP_VSPACE:
  case OP_NOT_VSPACE:
  switch(next)
    {
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x85:
    case 0x2028:
    case 0x2029:
    return op_code != OP_VSPACE;
    default:
    return op_code == OP_VSPACE;
    }

  default:
  return FALSE;
  }


/* Handle the case when the next item is \d, \s, etc. */

switch(op_code)
  {
  case OP_CHAR:
  case OP_CHARNC:
#ifdef SUPPORT_UTF8
  if (utf8 && item > 127) { GETCHAR(item, utf8_char); }
#endif
  switch(-next)
    {
    case ESC_d:
    return item > 127 || (cd->ctypes[item] & ctype_digit) == 0;

    case ESC_D:
    return item <= 127 && (cd->ctypes[item] & ctype_digit) != 0;

    case ESC_s:
    return item > 127 || (cd->ctypes[item] & ctype_space) == 0;

    case ESC_S:
    return item <= 127 && (cd->ctypes[item] & ctype_space) != 0;

    case ESC_w:
    return item > 127 || (cd->ctypes[item] & ctype_word) == 0;

    case ESC_W:
    return item <= 127 && (cd->ctypes[item] & ctype_word) != 0;

    case ESC_h:
    case ESC_H:
    switch(item)
      {
      case 0x09:
      case 0x20:
      case 0xa0:
      case 0x1680:
      case 0x180e:
      case 0x2000:
      case 0x2001:
      case 0x2002:
      case 0x2003:
      case 0x2004:
      case 0x2005:
      case 0x2006:
      case 0x2007:
      case 0x2008:
      case 0x2009:
      case 0x200A:
      case 0x202f:
      case 0x205f:
      case 0x3000:
      return -next != ESC_h;
      default:
      return -next == ESC_h;
      }

    case ESC_v:
    case ESC_V:
    switch(item)
      {
      case 0x0a:
      case 0x0b:
      case 0x0c:
      case 0x0d:
      case 0x85:
      case 0x2028:
      case 0x2029:
      return -next != ESC_v;
      default:
      return -next == ESC_v;
      }

    default:
    return FALSE;
    }

  case OP_DIGIT:
  return next == -ESC_D || next == -ESC_s || next == -ESC_W ||
         next == -ESC_h || next == -ESC_v;

  case OP_NOT_DIGIT:
  return next == -ESC_d;

  case OP_WHITESPACE:
  return next == -ESC_S || next == -ESC_d || next == -ESC_w;

  case OP_NOT_WHITESPACE:
  return next == -ESC_s || next == -ESC_h || next == -ESC_v;

  case OP_HSPACE:
  return next == -ESC_S || next == -ESC_H || next == -ESC_d || next == -ESC_w;

  case OP_NOT_HSPACE:
  return next == -ESC_h;

  /* Can't have \S in here because VT matches \S (Perl anomaly) */
  case OP_VSPACE:
  return next == -ESC_V || next == -ESC_d || next == -ESC_w;

  case OP_NOT_VSPACE:
  return next == -ESC_v;

  case OP_WORDCHAR:
  return next == -ESC_W || next == -ESC_s || next == -ESC_h || next == -ESC_v;

  case OP_NOT_WORDCHAR:
  return next == -ESC_w || next == -ESC_d;

  default:
  return FALSE;
  }

/* Control does not reach here */
}



/*************************************************
*           Compile one branch                   *
*************************************************/

/* Scan the pattern, compiling it into the a vector. If the options are
changed during the branch, the pointer is used to change the external options
bits. This function is used during the pre-compile phase when we are trying
to find out the amount of memory needed, as well as during the real compile
phase. The value of lengthptr distinguishes the two phases.

Arguments:
  optionsptr     pointer to the option bits
  codeptr        points to the pointer to the current code point
  ptrptr         points to the current pattern pointer
  errorcodeptr   points to error code variable
  firstbyteptr   set to initial literal character, or < 0 (REQ_UNSET, REQ_NONE)
  reqbyteptr     set to the last literal character required, else < 0
  bcptr          points to current branch chain
  cd             contains pointers to tables etc.
  lengthptr      NULL during the real compile phase
                 points to length accumulator during pre-compile phase

Returns:         TRUE on success
                 FALSE, with *errorcodeptr set non-zero on error
*/

static BOOL
compile_branch(int *optionsptr, uschar **codeptr, const uschar **ptrptr,
  int *errorcodeptr, int *firstbyteptr, int *reqbyteptr, branch_chain *bcptr,
  compile_data *cd, int *lengthptr)
{
int repeat_type, op_type;
int repeat_min = 0, repeat_max = 0;      /* To please picky compilers */
int bravalue = 0;
int greedy_default, greedy_non_default;
int firstbyte, reqbyte;
int zeroreqbyte, zerofirstbyte;
int req_caseopt, reqvary, tempreqvary;
int options = *optionsptr;
int after_manual_callout = 0;
int length_prevgroup = 0;
register int c;
register uschar *code = *codeptr;
uschar *last_code = code;
uschar *orig_code = code;
uschar *tempcode;
BOOL inescq = FALSE;
BOOL groupsetfirstbyte = FALSE;
const uschar *ptr = *ptrptr;
const uschar *tempptr;
uschar *previous = NULL;
uschar *previous_callout = NULL;
uschar *save_hwm = NULL;
uschar classbits[32];

#ifdef SUPPORT_UTF8
BOOL class_utf8;
BOOL utf8 = (options & PCRE_UTF8) != 0;
uschar *class_utf8data;
uschar *class_utf8data_base;
uschar utf8_char[6];
#else
BOOL utf8 = FALSE;
uschar *utf8_char = NULL;
#endif

#ifdef DEBUG
if (lengthptr != NULL) DPRINTF((">> start branch\n"));
#endif

/* Set up the default and non-default settings for greediness */

greedy_default = ((options & PCRE_UNGREEDY) != 0);
greedy_non_default = greedy_default ^ 1;

/* Initialize no first byte, no required byte. REQ_UNSET means "no char
matching encountered yet". It gets changed to REQ_NONE if we hit something that
matches a non-fixed char first char; reqbyte just remains unset if we never
find one.

When we hit a repeat whose minimum is zero, we may have to adjust these values
to take the zero repeat into account. This is implemented by setting them to
zerofirstbyte and zeroreqbyte when such a repeat is encountered. The individual
item types that can be repeated set these backoff variables appropriately. */

firstbyte = reqbyte = zerofirstbyte = zeroreqbyte = REQ_UNSET;

/* The variable req_caseopt contains either the REQ_CASELESS value or zero,
according to the current setting of the caseless flag. REQ_CASELESS is a bit
value > 255. It is added into the firstbyte or reqbyte variables to record the
case status of the value. This is used only for ASCII characters. */

req_caseopt = ((options & PCRE_CASELESS) != 0)? REQ_CASELESS : 0;

/* Switch on next character until the end of the branch */

for (;; ptr++)
  {
  BOOL negate_class;
  BOOL should_flip_negation;
  BOOL possessive_quantifier;
  BOOL is_quantifier;
  BOOL is_recurse;
  BOOL reset_bracount;
  int class_charcount;
  int class_lastchar;
  int newoptions;
  int recno;
  int refsign;
  int skipbytes;
  int subreqbyte;
  int subfirstbyte;
  int terminator;
  int mclength;
  uschar mcbuffer[8];

  /* Get next byte in the pattern */

  c = *ptr;

  /* If we are in the pre-compile phase, accumulate the length used for the
  previous cycle of this loop. */

  if (lengthptr != NULL)
    {
#ifdef DEBUG
    if (code > cd->hwm) cd->hwm = code;                 /* High water info */
#endif
    if (code > cd->start_workspace + COMPILE_WORK_SIZE) /* Check for overrun */
      {
      *errorcodeptr = ERR52;
      goto FAILED;
      }

    /* There is at least one situation where code goes backwards: this is the
    case of a zero quantifier after a class (e.g. [ab]{0}). At compile time,
    the class is simply eliminated. However, it is created first, so we have to
    allow memory for it. Therefore, don't ever reduce the length at this point.
    */

    if (code < last_code) code = last_code;

    /* Paranoid check for integer overflow */

    if (OFLOW_MAX - *lengthptr < code - last_code)
      {
      *errorcodeptr = ERR20;
      goto FAILED;
      }

    *lengthptr += code - last_code;
    DPRINTF(("length=%d added %d c=%c\n", *lengthptr, code - last_code, c));

    /* If "previous" is set and it is not at the start of the work space, move
    it back to there, in order to avoid filling up the work space. Otherwise,
    if "previous" is NULL, reset the current code pointer to the start. */

    if (previous != NULL)
      {
      if (previous > orig_code)
        {
        memmove(orig_code, previous, code - previous);
        code -= previous - orig_code;
        previous = orig_code;
        }
      }
    else code = orig_code;

    /* Remember where this code item starts so we can pick up the length
    next time round. */

    last_code = code;
    }

  /* In the real compile phase, just check the workspace used by the forward
  reference list. */

  else if (cd->hwm > cd->start_workspace + COMPILE_WORK_SIZE)
    {
    *errorcodeptr = ERR52;
    goto FAILED;
    }

  /* If in \Q...\E, check for the end; if not, we have a literal */

  if (inescq && c != 0)
    {
    if (c == '\\' && ptr[1] == 'E')
      {
      inescq = FALSE;
      ptr++;
      continue;
      }
    else
      {
      if (previous_callout != NULL)
        {
        if (lengthptr == NULL)  /* Don't attempt in pre-compile phase */
          complete_callout(previous_callout, ptr, cd);
        previous_callout = NULL;
        }
      if ((options & PCRE_AUTO_CALLOUT) != 0)
        {
        previous_callout = code;
        code = auto_callout(code, ptr, cd);
        }
      goto NORMAL_CHAR;
      }
    }

  /* Fill in length of a previous callout, except when the next thing is
  a quantifier. */

  is_quantifier = c == '*' || c == '+' || c == '?' ||
    (c == '{' && is_counted_repeat(ptr+1));

  if (!is_quantifier && previous_callout != NULL &&
       after_manual_callout-- <= 0)
    {
    if (lengthptr == NULL)      /* Don't attempt in pre-compile phase */
      complete_callout(previous_callout, ptr, cd);
    previous_callout = NULL;
    }

  /* In extended mode, skip white space and comments */

  if ((options & PCRE_EXTENDED) != 0)
    {
    if ((cd->ctypes[c] & ctype_space) != 0) continue;
    if (c == '#')
      {
      while (*(++ptr) != 0)
        {
        if (IS_NEWLINE(ptr)) { ptr += cd->nllen - 1; break; }
        }
      if (*ptr != 0) continue;

      /* Else fall through to handle end of string */
      c = 0;
      }
    }

  /* No auto callout for quantifiers. */

  if ((options & PCRE_AUTO_CALLOUT) != 0 && !is_quantifier)
    {
    previous_callout = code;
    code = auto_callout(code, ptr, cd);
    }

  switch(c)
    {
    /* ===================================================================*/
    case 0:                        /* The branch terminates at string end */
    case '|':                      /* or | or ) */
    case ')':
    *firstbyteptr = firstbyte;
    *reqbyteptr = reqbyte;
    *codeptr = code;
    *ptrptr = ptr;
    if (lengthptr != NULL)
      {
      if (OFLOW_MAX - *lengthptr < code - last_code)
        {
        *errorcodeptr = ERR20;
        goto FAILED;
        }
      *lengthptr += code - last_code;   /* To include callout length */
      DPRINTF((">> end branch\n"));
      }
    return TRUE;


    /* ===================================================================*/
    /* Handle single-character metacharacters. In multiline mode, ^ disables
    the setting of any following char as a first character. */

    case '^':
    if ((options & PCRE_MULTILINE) != 0)
      {
      if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
      }
    previous = NULL;
    *code++ = OP_CIRC;
    break;

    case '$':
    previous = NULL;
    *code++ = OP_DOLL;
    break;

    /* There can never be a first char if '.' is first, whatever happens about
    repeats. The value of reqbyte doesn't change either. */

    case '.':
    if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
    zerofirstbyte = firstbyte;
    zeroreqbyte = reqbyte;
    previous = code;
    *code++ = ((options & PCRE_DOTALL) != 0)? OP_ALLANY: OP_ANY;
    break;


    /* ===================================================================*/
    /* Character classes. If the included characters are all < 256, we build a
    32-byte bitmap of the permitted characters, except in the special case
    where there is only one such character. For negated classes, we build the
    map as usual, then invert it at the end. However, we use a different opcode
    so that data characters > 255 can be handled correctly.

    If the class contains characters outside the 0-255 range, a different
    opcode is compiled. It may optionally have a bit map for characters < 256,
    but those above are are explicitly listed afterwards. A flag byte tells
    whether the bitmap is present, and whether this is a negated class or not.

    In JavaScript compatibility mode, an isolated ']' causes an error. In
    default (Perl) mode, it is treated as a data character. */

    case ']':
    if ((cd->external_options & PCRE_JAVASCRIPT_COMPAT) != 0)
      {
      *errorcodeptr = ERR64;
      goto FAILED;
      }
    goto NORMAL_CHAR;

    case '[':
    previous = code;

    /* PCRE supports POSIX class stuff inside a class. Perl gives an error if
    they are encountered at the top level, so we'll do that too. */

    if ((ptr[1] == ':' || ptr[1] == '.' || ptr[1] == '=') &&
        check_posix_syntax(ptr, &tempptr))
      {
      *errorcodeptr = (ptr[1] == ':')? ERR13 : ERR31;
      goto FAILED;
      }

    /* If the first character is '^', set the negation flag and skip it. Also,
    if the first few characters (either before or after ^) are \Q\E or \E we
    skip them too. This makes for compatibility with Perl. */

    negate_class = FALSE;
    for (;;)
      {
      c = *(++ptr);
      if (c == '\\')
        {
        if (ptr[1] == 'E') ptr++;
          else if (strncmp((const char *)ptr+1, "Q\\E", 3) == 0) ptr += 3;
            else break;
        }
      else if (!negate_class && c == '^')
        negate_class = TRUE;
      else break;
      }

    /* Empty classes are allowed in JavaScript compatibility mode. Otherwise,
    an initial ']' is taken as a data character -- the code below handles
    that. In JS mode, [] must always fail, so generate OP_FAIL, whereas
    [^] must match any character, so generate OP_ALLANY. */

    if (c ==']' && (cd->external_options & PCRE_JAVASCRIPT_COMPAT) != 0)
      {
      *code++ = negate_class? OP_ALLANY : OP_FAIL;
      if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
      zerofirstbyte = firstbyte;
      break;
      }

    /* If a class contains a negative special such as \S, we need to flip the
    negation flag at the end, so that support for characters > 255 works
    correctly (they are all included in the class). */

    should_flip_negation = FALSE;

    /* Keep a count of chars with values < 256 so that we can optimize the case
    of just a single character (as long as it's < 256). However, For higher
    valued UTF-8 characters, we don't yet do any optimization. */

    class_charcount = 0;
    class_lastchar = -1;

    /* Initialize the 32-char bit map to all zeros. We build the map in a
    temporary bit of memory, in case the class contains only 1 character (less
    than 256), because in that case the compiled code doesn't use the bit map.
    */

    memset(classbits, 0, 32 * sizeof(uschar));

#ifdef SUPPORT_UTF8
    class_utf8 = FALSE;                       /* No chars >= 256 */
    class_utf8data = code + LINK_SIZE + 2;    /* For UTF-8 items */
    class_utf8data_base = class_utf8data;     /* For resetting in pass 1 */
#endif

    /* Process characters until ] is reached. By writing this as a "do" it
    means that an initial ] is taken as a data character. At the start of the
    loop, c contains the first byte of the character. */

    if (c != 0) do
      {
      const uschar *oldptr;

#ifdef SUPPORT_UTF8
      if (utf8 && c > 127)
        {                           /* Braces are required because the */
        GETCHARLEN(c, ptr, ptr);    /* macro generates multiple statements */
        }

      /* In the pre-compile phase, accumulate the length of any UTF-8 extra
      data and reset the pointer. This is so that very large classes that
      contain a zillion UTF-8 characters no longer overwrite the work space
      (which is on the stack). */

      if (lengthptr != NULL)
        {
        *lengthptr += class_utf8data - class_utf8data_base;
        class_utf8data = class_utf8data_base;
        }

#endif

      /* Inside \Q...\E everything is literal except \E */

      if (inescq)
        {
        if (c == '\\' && ptr[1] == 'E')     /* If we are at \E */
          {
          inescq = FALSE;                   /* Reset literal state */
          ptr++;                            /* Skip the 'E' */
          continue;                         /* Carry on with next */
          }
        goto CHECK_RANGE;                   /* Could be range if \E follows */
        }

      /* Handle POSIX class names. Perl allows a negation extension of the
      form [:^name:]. A square bracket that doesn't match the syntax is
      treated as a literal. We also recognize the POSIX constructions
      [.ch.] and [=ch=] ("collating elements") and fault them, as Perl
      5.6 and 5.8 do. */

      if (c == '[' &&
          (ptr[1] == ':' || ptr[1] == '.' || ptr[1] == '=') &&
          check_posix_syntax(ptr, &tempptr))
        {
        BOOL local_negate = FALSE;
        int posix_class, taboffset, tabopt;
        register const uschar *cbits = cd->cbits;
        uschar pbits[32];

        if (ptr[1] != ':')
          {
          *errorcodeptr = ERR31;
          goto FAILED;
          }

        ptr += 2;
        if (*ptr == '^')
          {
          local_negate = TRUE;
          should_flip_negation = TRUE;  /* Note negative special */
          ptr++;
          }

        posix_class = check_posix_name(ptr, tempptr - ptr);
        if (posix_class < 0)
          {
          *errorcodeptr = ERR30;
          goto FAILED;
          }

        /* If matching is caseless, upper and lower are converted to
        alpha. This relies on the fact that the class table starts with
        alpha, lower, upper as the first 3 entries. */

        if ((options & PCRE_CASELESS) != 0 && posix_class <= 2)
          posix_class = 0;

        /* We build the bit map for the POSIX class in a chunk of local store
        because we may be adding and subtracting from it, and we don't want to
        subtract bits that may be in the main map already. At the end we or the
        result into the bit map that is being built. */

        posix_class *= 3;

        /* Copy in the first table (always present) */

        memcpy(pbits, cbits + posix_class_maps[posix_class],
          32 * sizeof(uschar));

        /* If there is a second table, add or remove it as required. */

        taboffset = posix_class_maps[posix_class + 1];
        tabopt = posix_class_maps[posix_class + 2];

        if (taboffset >= 0)
          {
          if (tabopt >= 0)
            for (c = 0; c < 32; c++) pbits[c] |= cbits[c + taboffset];
          else
            for (c = 0; c < 32; c++) pbits[c] &= ~cbits[c + taboffset];
          }

        /* Not see if we need to remove any special characters. An option
        value of 1 removes vertical space and 2 removes underscore. */

        if (tabopt < 0) tabopt = -tabopt;
        if (tabopt == 1) pbits[1] &= ~0x3c;
          else if (tabopt == 2) pbits[11] &= 0x7f;

        /* Add the POSIX table or its complement into the main table that is
        being built and we are done. */

        if (local_negate)
          for (c = 0; c < 32; c++) classbits[c] |= ~pbits[c];
        else
          for (c = 0; c < 32; c++) classbits[c] |= pbits[c];

        ptr = tempptr + 1;
        class_charcount = 10;  /* Set > 1; assumes more than 1 per class */
        continue;    /* End of POSIX syntax handling */
        }

      /* Backslash may introduce a single character, or it may introduce one
      of the specials, which just set a flag. The sequence \b is a special
      case. Inside a class (and only there) it is treated as backspace.
      Elsewhere it marks a word boundary. Other escapes have preset maps ready
      to 'or' into the one we are building. We assume they have more than one
      character in them, so set class_charcount bigger than one. */

      if (c == '\\')
        {
        c = check_escape(&ptr, errorcodeptr, cd->bracount, options, TRUE);
        if (*errorcodeptr != 0) goto FAILED;

        if (-c == ESC_b) c = '\b';       /* \b is backspace in a class */
        else if (-c == ESC_X) c = 'X';   /* \X is literal X in a class */
        else if (-c == ESC_R) c = 'R';   /* \R is literal R in a class */
        else if (-c == ESC_Q)            /* Handle start of quoted string */
          {
          if (ptr[1] == '\\' && ptr[2] == 'E')
            {
            ptr += 2; /* avoid empty string */
            }
          else inescq = TRUE;
          continue;
          }
        else if (-c == ESC_E) continue;  /* Ignore orphan \E */

        if (c < 0)
          {
          register const uschar *cbits = cd->cbits;
          class_charcount += 2;     /* Greater than 1 is what matters */

          /* Save time by not doing this in the pre-compile phase. */

          if (lengthptr == NULL) switch (-c)
            {
            case ESC_d:
            for (c = 0; c < 32; c++) classbits[c] |= cbits[c+cbit_digit];
            continue;

            case ESC_D:
            should_flip_negation = TRUE;
            for (c = 0; c < 32; c++) classbits[c] |= ~cbits[c+cbit_digit];
            continue;

            case ESC_w:
            for (c = 0; c < 32; c++) classbits[c] |= cbits[c+cbit_word];
            continue;

            case ESC_W:
            should_flip_negation = TRUE;
            for (c = 0; c < 32; c++) classbits[c] |= ~cbits[c+cbit_word];
            continue;

            case ESC_s:
            for (c = 0; c < 32; c++) classbits[c] |= cbits[c+cbit_space];
            classbits[1] &= ~0x08;   /* Perl 5.004 onwards omits VT from \s */
            continue;

            case ESC_S:
            should_flip_negation = TRUE;
            for (c = 0; c < 32; c++) classbits[c] |= ~cbits[c+cbit_space];
            classbits[1] |= 0x08;    /* Perl 5.004 onwards omits VT from \s */
            continue;

            default:    /* Not recognized; fall through */
            break;      /* Need "default" setting to stop compiler warning. */
            }

          /* In the pre-compile phase, just do the recognition. */

          else if (c == -ESC_d || c == -ESC_D || c == -ESC_w ||
                   c == -ESC_W || c == -ESC_s || c == -ESC_S) continue;

          /* We need to deal with \H, \h, \V, and \v in both phases because
          they use extra memory. */

          if (-c == ESC_h)
            {
            SETBIT(classbits, 0x09); /* VT */
            SETBIT(classbits, 0x20); /* SPACE */
            SETBIT(classbits, 0xa0); /* NSBP */
#ifdef SUPPORT_UTF8
            if (utf8)
              {
              class_utf8 = TRUE;
              *class_utf8data++ = XCL_SINGLE;
              class_utf8data += _pcre_ord2utf8(0x1680, class_utf8data);
              *class_utf8data++ = XCL_SINGLE;
              class_utf8data += _pcre_ord2utf8(0x180e, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x2000, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x200A, class_utf8data);
              *class_utf8data++ = XCL_SINGLE;
              class_utf8data += _pcre_ord2utf8(0x202f, class_utf8data);
              *class_utf8data++ = XCL_SINGLE;
              class_utf8data += _pcre_ord2utf8(0x205f, class_utf8data);
              *class_utf8data++ = XCL_SINGLE;
              class_utf8data += _pcre_ord2utf8(0x3000, class_utf8data);
              }
#endif
            continue;
            }

          if (-c == ESC_H)
            {
            for (c = 0; c < 32; c++)
              {
              int x = 0xff;
              switch (c)
                {
                case 0x09/8: x ^= 1 << (0x09%8); break;
                case 0x20/8: x ^= 1 << (0x20%8); break;
                case 0xa0/8: x ^= 1 << (0xa0%8); break;
                default: break;
                }
              classbits[c] |= x;
              }

#ifdef SUPPORT_UTF8
            if (utf8)
              {
              class_utf8 = TRUE;
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x0100, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x167f, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x1681, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x180d, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x180f, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x1fff, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x200B, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x202e, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x2030, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x205e, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x2060, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x2fff, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x3001, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x7fffffff, class_utf8data);
              }
#endif
            continue;
            }

          if (-c == ESC_v)
            {
            SETBIT(classbits, 0x0a); /* LF */
            SETBIT(classbits, 0x0b); /* VT */
            SETBIT(classbits, 0x0c); /* FF */
            SETBIT(classbits, 0x0d); /* CR */
            SETBIT(classbits, 0x85); /* NEL */
#ifdef SUPPORT_UTF8
            if (utf8)
              {
              class_utf8 = TRUE;
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x2028, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x2029, class_utf8data);
              }
#endif
            continue;
            }

          if (-c == ESC_V)
            {
            for (c = 0; c < 32; c++)
              {
              int x = 0xff;
              switch (c)
                {
                case 0x0a/8: x ^= 1 << (0x0a%8);
                             x ^= 1 << (0x0b%8);
                             x ^= 1 << (0x0c%8);
                             x ^= 1 << (0x0d%8);
                             break;
                case 0x85/8: x ^= 1 << (0x85%8); break;
                default: break;
                }
              classbits[c] |= x;
              }

#ifdef SUPPORT_UTF8
            if (utf8)
              {
              class_utf8 = TRUE;
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x0100, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x2027, class_utf8data);
              *class_utf8data++ = XCL_RANGE;
              class_utf8data += _pcre_ord2utf8(0x2029, class_utf8data);
              class_utf8data += _pcre_ord2utf8(0x7fffffff, class_utf8data);
              }
#endif
            continue;
            }

          /* We need to deal with \P and \p in both phases. */

#ifdef SUPPORT_UCP
          if (-c == ESC_p || -c == ESC_P)
            {
            BOOL negated;
            int pdata;
            int ptype = get_ucp(&ptr, &negated, &pdata, errorcodeptr);
            if (ptype < 0) goto FAILED;
            class_utf8 = TRUE;
            *class_utf8data++ = ((-c == ESC_p) != negated)?
              XCL_PROP : XCL_NOTPROP;
            *class_utf8data++ = ptype;
            *class_utf8data++ = pdata;
            class_charcount -= 2;   /* Not a < 256 character */
            continue;
            }
#endif
          /* Unrecognized escapes are faulted if PCRE is running in its
          strict mode. By default, for compatibility with Perl, they are
          treated as literals. */

          if ((options & PCRE_EXTRA) != 0)
            {
            *errorcodeptr = ERR7;
            goto FAILED;
            }

          class_charcount -= 2;  /* Undo the default count from above */
          c = *ptr;              /* Get the final character and fall through */
          }

        /* Fall through if we have a single character (c >= 0). This may be
        greater than 256 in UTF-8 mode. */

        }   /* End of backslash handling */

      /* A single character may be followed by '-' to form a range. However,
      Perl does not permit ']' to be the end of the range. A '-' character
      at the end is treated as a literal. Perl ignores orphaned \E sequences
      entirely. The code for handling \Q and \E is messy. */

      CHECK_RANGE:
      while (ptr[1] == '\\' && ptr[2] == 'E')
        {
        inescq = FALSE;
        ptr += 2;
        }

      oldptr = ptr;

      /* Remember \r or \n */

      if (c == '\r' || c == '\n') cd->external_flags |= PCRE_HASCRORLF;

      /* Check for range */

      if (!inescq && ptr[1] == '-')
        {
        int d;
        ptr += 2;
        while (*ptr == '\\' && ptr[1] == 'E') ptr += 2;

        /* If we hit \Q (not followed by \E) at this point, go into escaped
        mode. */

        while (*ptr == '\\' && ptr[1] == 'Q')
          {
          ptr += 2;
          if (*ptr == '\\' && ptr[1] == 'E') { ptr += 2; continue; }
          inescq = TRUE;
          break;
          }

        if (*ptr == 0 || (!inescq && *ptr == ']'))
          {
          ptr = oldptr;
          goto LONE_SINGLE_CHARACTER;
          }

#ifdef SUPPORT_UTF8
        if (utf8)
          {                           /* Braces are required because the */
          GETCHARLEN(d, ptr, ptr);    /* macro generates multiple statements */
          }
        else
#endif
        d = *ptr;  /* Not UTF-8 mode */

        /* The second part of a range can be a single-character escape, but
        not any of the other escapes. Perl 5.6 treats a hyphen as a literal
        in such circumstances. */

        if (!inescq && d == '\\')
          {
          d = check_escape(&ptr, errorcodeptr, cd->bracount, options, TRUE);
          if (*errorcodeptr != 0) goto FAILED;

          /* \b is backspace; \X is literal X; \R is literal R; any other
          special means the '-' was literal */

          if (d < 0)
            {
            if (d == -ESC_b) d = '\b';
            else if (d == -ESC_X) d = 'X';
            else if (d == -ESC_R) d = 'R'; else
              {
              ptr = oldptr;
              goto LONE_SINGLE_CHARACTER;  /* A few lines below */
              }
            }
          }

        /* Check that the two values are in the correct order. Optimize
        one-character ranges */

        if (d < c)
          {
          *errorcodeptr = ERR8;
          goto FAILED;
          }

        if (d == c) goto LONE_SINGLE_CHARACTER;  /* A few lines below */

        /* Remember \r or \n */

        if (d == '\r' || d == '\n') cd->external_flags |= PCRE_HASCRORLF;

        /* In UTF-8 mode, if the upper limit is > 255, or > 127 for caseless
        matching, we have to use an XCLASS with extra data items. Caseless
        matching for characters > 127 is available only if UCP support is
        available. */

#ifdef SUPPORT_UTF8
        if (utf8 && (d > 255 || ((options & PCRE_CASELESS) != 0 && d > 127)))
          {
          class_utf8 = TRUE;

          /* With UCP support, we can find the other case equivalents of
          the relevant characters. There may be several ranges. Optimize how
          they fit with the basic range. */

#ifdef SUPPORT_UCP
          if ((options & PCRE_CASELESS) != 0)
            {
            unsigned int occ, ocd;
            unsigned int cc = c;
            unsigned int origd = d;
            while (get_othercase_range(&cc, origd, &occ, &ocd))
              {
              if (occ >= (unsigned int)c &&
                  ocd <= (unsigned int)d)
                continue;                          /* Skip embedded ranges */

              if (occ < (unsigned int)c  &&
                  ocd >= (unsigned int)c - 1)      /* Extend the basic range */
                {                                  /* if there is overlap,   */
                c = occ;                           /* noting that if occ < c */
                continue;                          /* we can't have ocd > d  */
                }                                  /* because a subrange is  */
              if (ocd > (unsigned int)d &&
                  occ <= (unsigned int)d + 1)      /* always shorter than    */
                {                                  /* the basic range.       */
                d = ocd;
                continue;
                }

              if (occ == ocd)
                {
                *class_utf8data++ = XCL_SINGLE;
                }
              else
                {
                *class_utf8data++ = XCL_RANGE;
                class_utf8data += _pcre_ord2utf8(occ, class_utf8data);
                }
              class_utf8data += _pcre_ord2utf8(ocd, class_utf8data);
              }
            }
#endif  /* SUPPORT_UCP */

          /* Now record the original range, possibly modified for UCP caseless
          overlapping ranges. */

          *class_utf8data++ = XCL_RANGE;
          class_utf8data += _pcre_ord2utf8(c, class_utf8data);
          class_utf8data += _pcre_ord2utf8(d, class_utf8data);

          /* With UCP support, we are done. Without UCP support, there is no
          caseless matching for UTF-8 characters > 127; we can use the bit map
          for the smaller ones. */

#ifdef SUPPORT_UCP
          continue;    /* With next character in the class */
#else
          if ((options & PCRE_CASELESS) == 0 || c > 127) continue;

          /* Adjust upper limit and fall through to set up the map */

          d = 127;

#endif  /* SUPPORT_UCP */
          }
#endif  /* SUPPORT_UTF8 */

        /* We use the bit map for all cases when not in UTF-8 mode; else
        ranges that lie entirely within 0-127 when there is UCP support; else
        for partial ranges without UCP support. */

        class_charcount += d - c + 1;
        class_lastchar = d;

        /* We can save a bit of time by skipping this in the pre-compile. */

        if (lengthptr == NULL) for (; c <= d; c++)
          {
          classbits[c/8] |= (1 << (c&7));
          if ((options & PCRE_CASELESS) != 0)
            {
            int uc = cd->fcc[c];           /* flip case */
            classbits[uc/8] |= (1 << (uc&7));
            }
          }

        continue;   /* Go get the next char in the class */
        }

      /* Handle a lone single character - we can get here for a normal
      non-escape char, or after \ that introduces a single character or for an
      apparent range that isn't. */

      LONE_SINGLE_CHARACTER:

      /* Handle a character that cannot go in the bit map */

#ifdef SUPPORT_UTF8
      if (utf8 && (c > 255 || ((options & PCRE_CASELESS) != 0 && c > 127)))
        {
        class_utf8 = TRUE;
        *class_utf8data++ = XCL_SINGLE;
        class_utf8data += _pcre_ord2utf8(c, class_utf8data);

#ifdef SUPPORT_UCP
        if ((options & PCRE_CASELESS) != 0)
          {
          unsigned int othercase;
          if ((othercase = UCD_OTHERCASE(c)) != c)
            {
            *class_utf8data++ = XCL_SINGLE;
            class_utf8data += _pcre_ord2utf8(othercase, class_utf8data);
            }
          }
#endif  /* SUPPORT_UCP */

        }
      else
#endif  /* SUPPORT_UTF8 */

      /* Handle a single-byte character */
        {
        classbits[c/8] |= (1 << (c&7));
        if ((options & PCRE_CASELESS) != 0)
          {
          c = cd->fcc[c];   /* flip case */
          classbits[c/8] |= (1 << (c&7));
          }
        class_charcount++;
        class_lastchar = c;
        }
      }

    /* Loop until ']' reached. This "while" is the end of the "do" above. */

    while ((c = *(++ptr)) != 0 && (c != ']' || inescq));

    if (c == 0)                          /* Missing terminating ']' */
      {
      *errorcodeptr = ERR6;
      goto FAILED;
      }


/* This code has been disabled because it would mean that \s counts as
an explicit \r or \n reference, and that's not really what is wanted. Now
we set the flag only if there is a literal "\r" or "\n" in the class. */

#if 0
    /* Remember whether \r or \n are in this class */

    if (negate_class)
      {
      if ((classbits[1] & 0x24) != 0x24) cd->external_flags |= PCRE_HASCRORLF;
      }
    else
      {
      if ((classbits[1] & 0x24) != 0) cd->external_flags |= PCRE_HASCRORLF;
      }
#endif


    /* If class_charcount is 1, we saw precisely one character whose value is
    less than 256. As long as there were no characters >= 128 and there was no
    use of \p or \P, in other words, no use of any XCLASS features, we can
    optimize.

    In UTF-8 mode, we can optimize the negative case only if there were no
    characters >= 128 because OP_NOT and the related opcodes like OP_NOTSTAR
    operate on single-bytes only. This is an historical hangover. Maybe one day
    we can tidy these opcodes to handle multi-byte characters.

    The optimization throws away the bit map. We turn the item into a
    1-character OP_CHAR[NC] if it's positive, or OP_NOT if it's negative. Note
    that OP_NOT does not support multibyte characters. In the positive case, it
    can cause firstbyte to be set. Otherwise, there can be no first char if
    this item is first, whatever repeat count may follow. In the case of
    reqbyte, save the previous value for reinstating. */

#ifdef SUPPORT_UTF8
    if (class_charcount == 1 && !class_utf8 &&
      (!utf8 || !negate_class || class_lastchar < 128))
#else
    if (class_charcount == 1)
#endif
      {
      zeroreqbyte = reqbyte;

      /* The OP_NOT opcode works on one-byte characters only. */

      if (negate_class)
        {
        if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
        zerofirstbyte = firstbyte;
        *code++ = OP_NOT;
        *code++ = class_lastchar;
        break;
        }

      /* For a single, positive character, get the value into mcbuffer, and
      then we can handle this with the normal one-character code. */

#ifdef SUPPORT_UTF8
      if (utf8 && class_lastchar > 127)
        mclength = _pcre_ord2utf8(class_lastchar, mcbuffer);
      else
#endif
        {
        mcbuffer[0] = class_lastchar;
        mclength = 1;
        }
      goto ONE_CHAR;
      }       /* End of 1-char optimization */

    /* The general case - not the one-char optimization. If this is the first
    thing in the branch, there can be no first char setting, whatever the
    repeat count. Any reqbyte setting must remain unchanged after any kind of
    repeat. */

    if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
    zerofirstbyte = firstbyte;
    zeroreqbyte = reqbyte;

    /* If there are characters with values > 255, we have to compile an
    extended class, with its own opcode, unless there was a negated special
    such as \S in the class, because in that case all characters > 255 are in
    the class, so any that were explicitly given as well can be ignored. If
    (when there are explicit characters > 255 that must be listed) there are no
    characters < 256, we can omit the bitmap in the actual compiled code. */

#ifdef SUPPORT_UTF8
    if (class_utf8 && !should_flip_negation)
      {
      *class_utf8data++ = XCL_END;    /* Marks the end of extra data */
      *code++ = OP_XCLASS;
      code += LINK_SIZE;
      *code = negate_class? XCL_NOT : 0;

      /* If the map is required, move up the extra data to make room for it;
      otherwise just move the code pointer to the end of the extra data. */

      if (class_charcount > 0)
        {
        *code++ |= XCL_MAP;
        memmove(code + 32, code, class_utf8data - code);
        memcpy(code, classbits, 32);
        code = class_utf8data + 32;
        }
      else code = class_utf8data;

      /* Now fill in the complete length of the item */

      PUT(previous, 1, code - previous);
      break;   /* End of class handling */
      }
#endif

    /* If there are no characters > 255, set the opcode to OP_CLASS or
    OP_NCLASS, depending on whether the whole class was negated and whether
    there were negative specials such as \S in the class. Then copy the 32-byte
    map into the code vector, negating it if necessary. */

    *code++ = (negate_class == should_flip_negation) ? OP_CLASS : OP_NCLASS;
    if (negate_class)
      {
      if (lengthptr == NULL)    /* Save time in the pre-compile phase */
        for (c = 0; c < 32; c++) code[c] = ~classbits[c];
      }
    else
      {
      memcpy(code, classbits, 32);
      }
    code += 32;
    break;


    /* ===================================================================*/
    /* Various kinds of repeat; '{' is not necessarily a quantifier, but this
    has been tested above. */

    case '{':
    if (!is_quantifier) goto NORMAL_CHAR;
    ptr = read_repeat_counts(ptr+1, &repeat_min, &repeat_max, errorcodeptr);
    if (*errorcodeptr != 0) goto FAILED;
    goto REPEAT;

    case '*':
    repeat_min = 0;
    repeat_max = -1;
    goto REPEAT;

    case '+':
    repeat_min = 1;
    repeat_max = -1;
    goto REPEAT;

    case '?':
    repeat_min = 0;
    repeat_max = 1;

    REPEAT:
    if (previous == NULL)
      {
      *errorcodeptr = ERR9;
      goto FAILED;
      }

    if (repeat_min == 0)
      {
      firstbyte = zerofirstbyte;    /* Adjust for zero repeat */
      reqbyte = zeroreqbyte;        /* Ditto */
      }

    /* Remember whether this is a variable length repeat */

    reqvary = (repeat_min == repeat_max)? 0 : REQ_VARY;

    op_type = 0;                    /* Default single-char op codes */
    possessive_quantifier = FALSE;  /* Default not possessive quantifier */

    /* Save start of previous item, in case we have to move it up to make space
    for an inserted OP_ONCE for the additional '+' extension. */

    tempcode = previous;

    /* If the next character is '+', we have a possessive quantifier. This
    implies greediness, whatever the setting of the PCRE_UNGREEDY option.
    If the next character is '?' this is a minimizing repeat, by default,
    but if PCRE_UNGREEDY is set, it works the other way round. We change the
    repeat type to the non-default. */

    if (ptr[1] == '+')
      {
      repeat_type = 0;                  /* Force greedy */
      possessive_quantifier = TRUE;
      ptr++;
      }
    else if (ptr[1] == '?')
      {
      repeat_type = greedy_non_default;
      ptr++;
      }
    else repeat_type = greedy_default;

    /* If previous was a character match, abolish the item and generate a
    repeat item instead. If a char item has a minumum of more than one, ensure
    that it is set in reqbyte - it might not be if a sequence such as x{3} is
    the first thing in a branch because the x will have gone into firstbyte
    instead.  */

    if (*previous == OP_CHAR || *previous == OP_CHARNC)
      {
      /* Deal with UTF-8 characters that take up more than one byte. It's
      easier to write this out separately than try to macrify it. Use c to
      hold the length of the character in bytes, plus 0x80 to flag that it's a
      length rather than a small character. */

#ifdef SUPPORT_UTF8
      if (utf8 && (code[-1] & 0x80) != 0)
        {
        uschar *lastchar = code - 1;
        while((*lastchar & 0xc0) == 0x80) lastchar--;
        c = code - lastchar;            /* Length of UTF-8 character */
        memcpy(utf8_char, lastchar, c); /* Save the char */
        c |= 0x80;                      /* Flag c as a length */
        }
      else
#endif

      /* Handle the case of a single byte - either with no UTF8 support, or
      with UTF-8 disabled, or for a UTF-8 character < 128. */

        {
        c = code[-1];
        if (repeat_min > 1) reqbyte = c | req_caseopt | cd->req_varyopt;
        }

      /* If the repetition is unlimited, it pays to see if the next thing on
      the line is something that cannot possibly match this character. If so,
      automatically possessifying this item gains some performance in the case
      where the match fails. */

      if (!possessive_quantifier &&
          repeat_max < 0 &&
          check_auto_possessive(*previous, c, utf8, utf8_char, ptr + 1,
            options, cd))
        {
        repeat_type = 0;    /* Force greedy */
        possessive_quantifier = TRUE;
        }

      goto OUTPUT_SINGLE_REPEAT;   /* Code shared with single character types */
      }

    /* If previous was a single negated character ([^a] or similar), we use
    one of the special opcodes, replacing it. The code is shared with single-
    character repeats by setting opt_type to add a suitable offset into
    repeat_type. We can also test for auto-possessification. OP_NOT is
    currently used only for single-byte chars. */

    else if (*previous == OP_NOT)
      {
      op_type = OP_NOTSTAR - OP_STAR;  /* Use "not" opcodes */
      c = previous[1];
      if (!possessive_quantifier &&
          repeat_max < 0 &&
          check_auto_possessive(OP_NOT, c, utf8, NULL, ptr + 1, options, cd))
        {
        repeat_type = 0;    /* Force greedy */
        possessive_quantifier = TRUE;
        }
      goto OUTPUT_SINGLE_REPEAT;
      }

    /* If previous was a character type match (\d or similar), abolish it and
    create a suitable repeat item. The code is shared with single-character
    repeats by setting op_type to add a suitable offset into repeat_type. Note
    the the Unicode property types will be present only when SUPPORT_UCP is
    defined, but we don't wrap the little bits of code here because it just
    makes it horribly messy. */

    else if (*previous < OP_EODN)
      {
      uschar *oldcode;
      int prop_type, prop_value;
      op_type = OP_TYPESTAR - OP_STAR;  /* Use type opcodes */
      c = *previous;

      if (!possessive_quantifier &&
          repeat_max < 0 &&
          check_auto_possessive(c, 0, utf8, NULL, ptr + 1, options, cd))
        {
        repeat_type = 0;    /* Force greedy */
        possessive_quantifier = TRUE;
        }

      OUTPUT_SINGLE_REPEAT:
      if (*previous == OP_PROP || *previous == OP_NOTPROP)
        {
        prop_type = previous[1];
        prop_value = previous[2];
        }
      else prop_type = prop_value = -1;

      oldcode = code;
      code = previous;                  /* Usually overwrite previous item */

      /* If the maximum is zero then the minimum must also be zero; Perl allows
      this case, so we do too - by simply omitting the item altogether. */

      if (repeat_max == 0) goto END_REPEAT;

      /* All real repeats make it impossible to handle partial matching (maybe
      one day we will be able to remove this restriction). */

      if (repeat_max != 1) cd->external_flags |= PCRE_NOPARTIAL;

      /* Combine the op_type with the repeat_type */

      repeat_type += op_type;

      /* A minimum of zero is handled either as the special case * or ?, or as
      an UPTO, with the maximum given. */

      if (repeat_min == 0)
        {
        if (repeat_max == -1) *code++ = OP_STAR + repeat_type;
          else if (repeat_max == 1) *code++ = OP_QUERY + repeat_type;
        else
          {
          *code++ = OP_UPTO + repeat_type;
          PUT2INC(code, 0, repeat_max);
          }
        }

      /* A repeat minimum of 1 is optimized into some special cases. If the
      maximum is unlimited, we use OP_PLUS. Otherwise, the original item is
      left in place and, if the maximum is greater than 1, we use OP_UPTO with
      one less than the maximum. */

      else if (repeat_min == 1)
        {
        if (repeat_max == -1)
          *code++ = OP_PLUS + repeat_type;
        else
          {
          code = oldcode;                 /* leave previous item in place */
          if (repeat_max == 1) goto END_REPEAT;
          *code++ = OP_UPTO + repeat_type;
          PUT2INC(code, 0, repeat_max - 1);
          }
        }

      /* The case {n,n} is just an EXACT, while the general case {n,m} is
      handled as an EXACT followed by an UPTO. */

      else
        {
        *code++ = OP_EXACT + op_type;  /* NB EXACT doesn't have repeat_type */
        PUT2INC(code, 0, repeat_min);

        /* If the maximum is unlimited, insert an OP_STAR. Before doing so,
        we have to insert the character for the previous code. For a repeated
        Unicode property match, there are two extra bytes that define the
        required property. In UTF-8 mode, long characters have their length in
        c, with the 0x80 bit as a flag. */

        if (repeat_max < 0)
          {
#ifdef SUPPORT_UTF8
          if (utf8 && c >= 128)
            {
            memcpy(code, utf8_char, c & 7);
            code += c & 7;
            }
          else
#endif
            {
            *code++ = c;
            if (prop_type >= 0)
              {
              *code++ = prop_type;
              *code++ = prop_value;
              }
            }
          *code++ = OP_STAR + repeat_type;
          }

        /* Else insert an UPTO if the max is greater than the min, again
        preceded by the character, for the previously inserted code. If the
        UPTO is just for 1 instance, we can use QUERY instead. */

        else if (repeat_max != repeat_min)
          {
#ifdef SUPPORT_UTF8
          if (utf8 && c >= 128)
            {
            memcpy(code, utf8_char, c & 7);
            code += c & 7;
            }
          else
#endif
          *code++ = c;
          if (prop_type >= 0)
            {
            *code++ = prop_type;
            *code++ = prop_value;
            }
          repeat_max -= repeat_min;

          if (repeat_max == 1)
            {
            *code++ = OP_QUERY + repeat_type;
            }
          else
            {
            *code++ = OP_UPTO + repeat_type;
            PUT2INC(code, 0, repeat_max);
            }
          }
        }

      /* The character or character type itself comes last in all cases. */

#ifdef SUPPORT_UTF8
      if (utf8 && c >= 128)
        {
        memcpy(code, utf8_char, c & 7);
        code += c & 7;
        }
      else
#endif
      *code++ = c;

      /* For a repeated Unicode property match, there are two extra bytes that
      define the required property. */

#ifdef SUPPORT_UCP
      if (prop_type >= 0)
        {
        *code++ = prop_type;
        *code++ = prop_value;
        }
#endif
      }

    /* If previous was a character class or a back reference, we put the repeat
    stuff after it, but just skip the item if the repeat was {0,0}. */

    else if (*previous == OP_CLASS ||
             *previous == OP_NCLASS ||
#ifdef SUPPORT_UTF8
             *previous == OP_XCLASS ||
#endif
             *previous == OP_REF)
      {
      if (repeat_max == 0)
        {
        code = previous;
        goto END_REPEAT;
        }

      /* All real repeats make it impossible to handle partial matching (maybe
      one day we will be able to remove this restriction). */

      if (repeat_max != 1) cd->external_flags |= PCRE_NOPARTIAL;

      if (repeat_min == 0 && repeat_max == -1)
        *code++ = OP_CRSTAR + repeat_type;
      else if (repeat_min == 1 && repeat_max == -1)
        *code++ = OP_CRPLUS + repeat_type;
      else if (repeat_min == 0 && repeat_max == 1)
        *code++ = OP_CRQUERY + repeat_type;
      else
        {
        *code++ = OP_CRRANGE + repeat_type;
        PUT2INC(code, 0, repeat_min);
        if (repeat_max == -1) repeat_max = 0;  /* 2-byte encoding for max */
        PUT2INC(code, 0, repeat_max);
        }
      }

    /* If previous was a bracket group, we may have to replicate it in certain
    cases. */

    else if (*previous == OP_BRA  || *previous == OP_CBRA ||
             *previous == OP_ONCE || *previous == OP_COND)
      {
      register int i;
      int ketoffset = 0;
      int len = code - previous;
      uschar *bralink = NULL;

      /* Repeating a DEFINE group is pointless */

      if (*previous == OP_COND && previous[LINK_SIZE+1] == OP_DEF)
        {
        *errorcodeptr = ERR55;
        goto FAILED;
        }

      /* If the maximum repeat count is unlimited, find the end of the bracket
      by scanning through from the start, and compute the offset back to it
      from the current code pointer. There may be an OP_OPT setting following
      the final KET, so we can't find the end just by going back from the code
      pointer. */

      if (repeat_max == -1)
        {
        register uschar *ket = previous;
        do ket += GET(ket, 1); while (*ket != OP_KET);
        ketoffset = code - ket;
        }

      /* The case of a zero minimum is special because of the need to stick
      OP_BRAZERO in front of it, and because the group appears once in the
      data, whereas in other cases it appears the minimum number of times. For
      this reason, it is simplest to treat this case separately, as otherwise
      the code gets far too messy. There are several special subcases when the
      minimum is zero. */

      if (repeat_min == 0)
        {
        /* If the maximum is also zero, we used to just omit the group from the
        output altogether, like this:

        ** if (repeat_max == 0)
        **   {
        **   code = previous;
        **   goto END_REPEAT;
        **   }

        However, that fails when a group is referenced as a subroutine from
        elsewhere in the pattern, so now we stick in OP_SKIPZERO in front of it
        so that it is skipped on execution. As we don't have a list of which
        groups are referenced, we cannot do this selectively.

        If the maximum is 1 or unlimited, we just have to stick in the BRAZERO
        and do no more at this point. However, we do need to adjust any
        OP_RECURSE calls inside the group that refer to the group itself or any
        internal or forward referenced group, because the offset is from the
        start of the whole regex. Temporarily terminate the pattern while doing
        this. */

        if (repeat_max <= 1)    /* Covers 0, 1, and unlimited */
          {
          *code = OP_END;
          adjust_recurse(previous, 1, utf8, cd, save_hwm);
          memmove(previous+1, previous, len);
          code++;
          if (repeat_max == 0)
            {
            *previous++ = OP_SKIPZERO;
            goto END_REPEAT;
            }
          *previous++ = OP_BRAZERO + repeat_type;
          }

        /* If the maximum is greater than 1 and limited, we have to replicate
        in a nested fashion, sticking OP_BRAZERO before each set of brackets.
        The first one has to be handled carefully because it's the original
        copy, which has to be moved up. The remainder can be handled by code
        that is common with the non-zero minimum case below. We have to
        adjust the value or repeat_max, since one less copy is required. Once
        again, we may have to adjust any OP_RECURSE calls inside the group. */

        else
          {
          int offset;
          *code = OP_END;
          adjust_recurse(previous, 2 + LINK_SIZE, utf8, cd, save_hwm);
          memmove(previous + 2 + LINK_SIZE, previous, len);
          code += 2 + LINK_SIZE;
          *previous++ = OP_BRAZERO + repeat_type;
          *previous++ = OP_BRA;

          /* We chain together the bracket offset fields that have to be
          filled in later when the ends of the brackets are reached. */

          offset = (bralink == NULL)? 0 : previous - bralink;
          bralink = previous;
          PUTINC(previous, 0, offset);
          }

        repeat_max--;
        }

      /* If the minimum is greater than zero, replicate the group as many
      times as necessary, and adjust the maximum to the number of subsequent
      copies that we need. If we set a first char from the group, and didn't
      set a required char, copy the latter from the former. If there are any
      forward reference subroutine calls in the group, there will be entries on
      the workspace list; replicate these with an appropriate increment. */

      else
        {
        if (repeat_min > 1)
          {
          /* In the pre-compile phase, we don't actually do the replication. We
          just adjust the length as if we had. Do some paranoid checks for
          potential integer overflow. */

          if (lengthptr != NULL)
            {
            int delta = (repeat_min - 1)*length_prevgroup;
            if ((double)(repeat_min - 1)*(double)length_prevgroup >
                                                            (double)INT_MAX ||
                OFLOW_MAX - *lengthptr < delta)
              {
              *errorcodeptr = ERR20;
              goto FAILED;
              }
            *lengthptr += delta;
            }

          /* This is compiling for real */

          else
            {
            if (groupsetfirstbyte && reqbyte < 0) reqbyte = firstbyte;
            for (i = 1; i < repeat_min; i++)
              {
              uschar *hc;
              uschar *this_hwm = cd->hwm;
              memcpy(code, previous, len);
              for (hc = save_hwm; hc < this_hwm; hc += LINK_SIZE)
                {
                PUT(cd->hwm, 0, GET(hc, 0) + len);
                cd->hwm += LINK_SIZE;
                }
              save_hwm = this_hwm;
              code += len;
              }
            }
          }

        if (repeat_max > 0) repeat_max -= repeat_min;
        }

      /* This code is common to both the zero and non-zero minimum cases. If
      the maximum is limited, it replicates the group in a nested fashion,
      remembering the bracket starts on a stack. In the case of a zero minimum,
      the first one was set up above. In all cases the repeat_max now specifies
      the number of additional copies needed. Again, we must remember to
      replicate entries on the forward reference list. */

      if (repeat_max >= 0)
        {
        /* In the pre-compile phase, we don't actually do the replication. We
        just adjust the length as if we had. For each repetition we must add 1
        to the length for BRAZERO and for all but the last repetition we must
        add 2 + 2*LINKSIZE to allow for the nesting that occurs. Do some
        paranoid checks to avoid integer overflow. */

        if (lengthptr != NULL && repeat_max > 0)
          {
          int delta = repeat_max * (length_prevgroup + 1 + 2 + 2*LINK_SIZE) -
                      2 - 2*LINK_SIZE;   /* Last one doesn't nest */
          if ((double)repeat_max *
                (double)(length_prevgroup + 1 + 2 + 2*LINK_SIZE)
                  > (double)INT_MAX ||
              OFLOW_MAX - *lengthptr < delta)
            {
            *errorcodeptr = ERR20;
            goto FAILED;
            }
          *lengthptr += delta;
          }

        /* This is compiling for real */

        else for (i = repeat_max - 1; i >= 0; i--)
          {
          uschar *hc;
          uschar *this_hwm = cd->hwm;

          *code++ = OP_BRAZERO + repeat_type;

          /* All but the final copy start a new nesting, maintaining the
          chain of brackets outstanding. */

          if (i != 0)
            {
            int offset;
            *code++ = OP_BRA;
            offset = (bralink == NULL)? 0 : code - bralink;
            bralink = code;
            PUTINC(code, 0, offset);
            }

          memcpy(code, previous, len);
          for (hc = save_hwm; hc < this_hwm; hc += LINK_SIZE)
            {
            PUT(cd->hwm, 0, GET(hc, 0) + len + ((i != 0)? 2+LINK_SIZE : 1));
            cd->hwm += LINK_SIZE;
            }
          save_hwm = this_hwm;
          code += len;
          }

        /* Now chain through the pending brackets, and fill in their length
        fields (which are holding the chain links pro tem). */

        while (bralink != NULL)
          {
          int oldlinkoffset;
          int offset = code - bralink + 1;
          uschar *bra = code - offset;
          oldlinkoffset = GET(bra, 1);
          bralink = (oldlinkoffset == 0)? NULL : bralink - oldlinkoffset;
          *code++ = OP_KET;
          PUTINC(code, 0, offset);
          PUT(bra, 1, offset);
          }
        }

      /* If the maximum is unlimited, set a repeater in the final copy. We
      can't just offset backwards from the current code point, because we
      don't know if there's been an options resetting after the ket. The
      correct offset was computed above.

      Then, when we are doing the actual compile phase, check to see whether
      this group is a non-atomic one that could match an empty string. If so,
      convert the initial operator to the S form (e.g. OP_BRA -> OP_SBRA) so
      that runtime checking can be done. [This check is also applied to
      atomic groups at runtime, but in a different way.] */

      else
        {
        uschar *ketcode = code - ketoffset;
        uschar *bracode = ketcode - GET(ketcode, 1);
        *ketcode = OP_KETRMAX + repeat_type;
        if (lengthptr == NULL && *bracode != OP_ONCE)
          {
          uschar *scode = bracode;
          do
            {
            if (could_be_empty_branch(scode, ketcode, utf8))
              {
              *bracode += OP_SBRA - OP_BRA;
              break;
              }
            scode += GET(scode, 1);
            }
          while (*scode == OP_ALT);
          }
        }
      }

    /* If previous is OP_FAIL, it was generated by an empty class [] in
    JavaScript mode. The other ways in which OP_FAIL can be generated, that is
    by (*FAIL) or (?!) set previous to NULL, which gives a "nothing to repeat"
    error above. We can just ignore the repeat in JS case. */

    else if (*previous == OP_FAIL) goto END_REPEAT;

    /* Else there's some kind of shambles */

    else
      {
      *errorcodeptr = ERR11;
      goto FAILED;
      }

    /* If the character following a repeat is '+', or if certain optimization
    tests above succeeded, possessive_quantifier is TRUE. For some of the
    simpler opcodes, there is an special alternative opcode for this. For
    anything else, we wrap the entire repeated item inside OP_ONCE brackets.
    The '+' notation is just syntactic sugar, taken from Sun's Java package,
    but the special opcodes can optimize it a bit. The repeated item starts at
    tempcode, not at previous, which might be the first part of a string whose
    (former) last char we repeated.

    Possessifying an 'exact' quantifier has no effect, so we can ignore it. But
    an 'upto' may follow. We skip over an 'exact' item, and then test the
    length of what remains before proceeding. */

    if (possessive_quantifier)
      {
      int len;
      if (*tempcode == OP_EXACT || *tempcode == OP_TYPEEXACT ||
          *tempcode == OP_NOTEXACT)
        tempcode += _pcre_OP_lengths[*tempcode] +
          ((*tempcode == OP_TYPEEXACT &&
             (tempcode[3] == OP_PROP || tempcode[3] == OP_NOTPROP))? 2:0);
      len = code - tempcode;
      if (len > 0) switch (*tempcode)
        {
        case OP_STAR:  *tempcode = OP_POSSTAR; break;
        case OP_PLUS:  *tempcode = OP_POSPLUS; break;
        case OP_QUERY: *tempcode = OP_POSQUERY; break;
        case OP_UPTO:  *tempcode = OP_POSUPTO; break;

        case OP_TYPESTAR:  *tempcode = OP_TYPEPOSSTAR; break;
        case OP_TYPEPLUS:  *tempcode = OP_TYPEPOSPLUS; break;
        case OP_TYPEQUERY: *tempcode = OP_TYPEPOSQUERY; break;
        case OP_TYPEUPTO:  *tempcode = OP_TYPEPOSUPTO; break;

        case OP_NOTSTAR:  *tempcode = OP_NOTPOSSTAR; break;
        case OP_NOTPLUS:  *tempcode = OP_NOTPOSPLUS; break;
        case OP_NOTQUERY: *tempcode = OP_NOTPOSQUERY; break;
        case OP_NOTUPTO:  *tempcode = OP_NOTPOSUPTO; break;

        default:
        memmove(tempcode + 1+LINK_SIZE, tempcode, len);
        code += 1 + LINK_SIZE;
        len += 1 + LINK_SIZE;
        tempcode[0] = OP_ONCE;
        *code++ = OP_KET;
        PUTINC(code, 0, len);
        PUT(tempcode, 1, len);
        break;
        }
      }

    /* In all case we no longer have a previous item. We also set the
    "follows varying string" flag for subsequently encountered reqbytes if
    it isn't already set and we have just passed a varying length item. */

    END_REPEAT:
    previous = NULL;
    cd->req_varyopt |= reqvary;
    break;


    /* ===================================================================*/
    /* Start of nested parenthesized sub-expression, or comment or lookahead or
    lookbehind or option setting or condition or all the other extended
    parenthesis forms.  */

    case '(':
    newoptions = options;
    skipbytes = 0;
    bravalue = OP_CBRA;
    save_hwm = cd->hwm;
    reset_bracount = FALSE;

    /* First deal with various "verbs" that can be introduced by '*'. */

    if (*(++ptr) == '*' && (cd->ctypes[ptr[1]] & ctype_letter) != 0)
      {
      int i, namelen;
      const char *vn = verbnames;
      const uschar *name = ++ptr;
      previous = NULL;
      while ((cd->ctypes[*++ptr] & ctype_letter) != 0) {};
      if (*ptr == ':')
        {
        *errorcodeptr = ERR59;   /* Not supported */
        goto FAILED;
        }
      if (*ptr != ')')
        {
        *errorcodeptr = ERR60;
        goto FAILED;
        }
      namelen = ptr - name;
      for (i = 0; i < verbcount; i++)
        {
        if (namelen == verbs[i].len &&
            strncmp((char *)name, vn, namelen) == 0)
          {
          *code = verbs[i].op;
          if (*code++ == OP_ACCEPT) cd->had_accept = TRUE;
          break;
          }
        vn += verbs[i].len + 1;
        }
      if (i < verbcount) continue;
      *errorcodeptr = ERR60;
      goto FAILED;
      }

    /* Deal with the extended parentheses; all are introduced by '?', and the
    appearance of any of them means that this is not a capturing group. */

    else if (*ptr == '?')
      {
      int i, set, unset, namelen;
      int *optset;
      const uschar *name;
      uschar *slot;

      switch (*(++ptr))
        {
        case '#':                 /* Comment; skip to ket */
        ptr++;
        while (*ptr != 0 && *ptr != ')') ptr++;
        if (*ptr == 0)
          {
          *errorcodeptr = ERR18;
          goto FAILED;
          }
        continue;


        /* ------------------------------------------------------------ */
        case '|':                 /* Reset capture count for each branch */
        reset_bracount = TRUE;
        /* Fall through */

        /* ------------------------------------------------------------ */
        case ':':                 /* Non-capturing bracket */
        bravalue = OP_BRA;
        ptr++;
        break;


        /* ------------------------------------------------------------ */
        case '(':
        bravalue = OP_COND;       /* Conditional group */

        /* A condition can be an assertion, a number (referring to a numbered
        group), a name (referring to a named group), or 'R', referring to
        recursion. R<digits> and R&name are also permitted for recursion tests.

        There are several syntaxes for testing a named group: (?(name)) is used
        by Python; Perl 5.10 onwards uses (?(<name>) or (?('name')).

        There are two unfortunate ambiguities, caused by history. (a) 'R' can
        be the recursive thing or the name 'R' (and similarly for 'R' followed
        by digits), and (b) a number could be a name that consists of digits.
        In both cases, we look for a name first; if not found, we try the other
        cases. */

        /* For conditions that are assertions, check the syntax, and then exit
        the switch. This will take control down to where bracketed groups,
        including assertions, are processed. */

        if (ptr[1] == '?' && (ptr[2] == '=' || ptr[2] == '!' || ptr[2] == '<'))
          break;

        /* Most other conditions use OP_CREF (a couple change to OP_RREF
        below), and all need to skip 3 bytes at the start of the group. */

        code[1+LINK_SIZE] = OP_CREF;
        skipbytes = 3;
        refsign = -1;

        /* Check for a test for recursion in a named group. */

        if (ptr[1] == 'R' && ptr[2] == '&')
          {
          terminator = -1;
          ptr += 2;
          code[1+LINK_SIZE] = OP_RREF;    /* Change the type of test */
          }

        /* Check for a test for a named group's having been set, using the Perl
        syntax (?(<name>) or (?('name') */

        else if (ptr[1] == '<')
          {
          terminator = '>';
          ptr++;
          }
        else if (ptr[1] == '\'')
          {
          terminator = '\'';
          ptr++;
          }
        else
          {
          terminator = 0;
          if (ptr[1] == '-' || ptr[1] == '+') refsign = *(++ptr);
          }

        /* We now expect to read a name; any thing else is an error */

        if ((cd->ctypes[ptr[1]] & ctype_word) == 0)
          {
          ptr += 1;  /* To get the right offset */
          *errorcodeptr = ERR28;
          goto FAILED;
          }

        /* Read the name, but also get it as a number if it's all digits */

        recno = 0;
        name = ++ptr;
        while ((cd->ctypes[*ptr] & ctype_word) != 0)
          {
          if (recno >= 0)
            recno = ((digitab[*ptr] & ctype_digit) != 0)?
              recno * 10 + *ptr - '0' : -1;
          ptr++;
          }
        namelen = ptr - name;

        if ((terminator > 0 && *ptr++ != terminator) || *ptr++ != ')')
          {
          ptr--;      /* Error offset */
          *errorcodeptr = ERR26;
          goto FAILED;
          }

        /* Do no further checking in the pre-compile phase. */

        if (lengthptr != NULL) break;

        /* In the real compile we do the work of looking for the actual
        reference. If the string started with "+" or "-" we require the rest to
        be digits, in which case recno will be set. */

        if (refsign > 0)
          {
          if (recno <= 0)
            {
            *errorcodeptr = ERR58;
            goto FAILED;
            }
          recno = (refsign == '-')?
            cd->bracount - recno + 1 : recno +cd->bracount;
          if (recno <= 0 || recno > cd->final_bracount)
            {
            *errorcodeptr = ERR15;
            goto FAILED;
            }
          PUT2(code, 2+LINK_SIZE, recno);
          break;
          }

        /* Otherwise (did not start with "+" or "-"), start by looking for the
        name. */

        slot = cd->name_table;
        for (i = 0; i < cd->names_found; i++)
          {
          if (strncmp((char *)name, (char *)slot+2, namelen) == 0) break;
          slot += cd->name_entry_size;
          }

        /* Found a previous named subpattern */

        if (i < cd->names_found)
          {
          recno = GET2(slot, 0);
          PUT2(code, 2+LINK_SIZE, recno);
          }

        /* Search the pattern for a forward reference */

        else if ((i = find_parens(ptr, cd, name, namelen,
                        (options & PCRE_EXTENDED) != 0)) > 0)
          {
          PUT2(code, 2+LINK_SIZE, i);
          }

        /* If terminator == 0 it means that the name followed directly after
        the opening parenthesis [e.g. (?(abc)...] and in this case there are
        some further alternatives to try. For the cases where terminator != 0
        [things like (?(<name>... or (?('name')... or (?(R&name)... ] we have
        now checked all the possibilities, so give an error. */

        else if (terminator != 0)
          {
          *errorcodeptr = ERR15;
          goto FAILED;
          }

        /* Check for (?(R) for recursion. Allow digits after R to specify a
        specific group number. */

        else if (*name == 'R')
          {
          recno = 0;
          for (i = 1; i < namelen; i++)
            {
            if ((digitab[name[i]] & ctype_digit) == 0)
              {
              *errorcodeptr = ERR15;
              goto FAILED;
              }
            recno = recno * 10 + name[i] - '0';
            }
          if (recno == 0) recno = RREF_ANY;
          code[1+LINK_SIZE] = OP_RREF;      /* Change test type */
          PUT2(code, 2+LINK_SIZE, recno);
          }

        /* Similarly, check for the (?(DEFINE) "condition", which is always
        false. */

        else if (namelen == 6 && strncmp((char *)name, "DEFINE", 6) == 0)
          {
          code[1+LINK_SIZE] = OP_DEF;
          skipbytes = 1;
          }

        /* Check for the "name" actually being a subpattern number. We are
        in the second pass here, so final_bracount is set. */

        else if (recno > 0 && recno <= cd->final_bracount)
          {
          PUT2(code, 2+LINK_SIZE, recno);
          }

        /* Either an unidentified subpattern, or a reference to (?(0) */

        else
          {
          *errorcodeptr = (recno == 0)? ERR35: ERR15;
          goto FAILED;
          }
        break;


        /* ------------------------------------------------------------ */
        case '=':                 /* Positive lookahead */
        bravalue = OP_ASSERT;
        ptr++;
        break;


        /* ------------------------------------------------------------ */
        case '!':                 /* Negative lookahead */
        ptr++;
        if (*ptr == ')')          /* Optimize (?!) */
          {
          *code++ = OP_FAIL;
          previous = NULL;
          continue;
          }
        bravalue = OP_ASSERT_NOT;
        break;


        /* ------------------------------------------------------------ */
        case '<':                 /* Lookbehind or named define */
        switch (ptr[1])
          {
          case '=':               /* Positive lookbehind */
          bravalue = OP_ASSERTBACK;
          ptr += 2;
          break;

          case '!':               /* Negative lookbehind */
          bravalue = OP_ASSERTBACK_NOT;
          ptr += 2;
          break;

          default:                /* Could be name define, else bad */
          if ((cd->ctypes[ptr[1]] & ctype_word) != 0) goto DEFINE_NAME;
          ptr++;                  /* Correct offset for error */
          *errorcodeptr = ERR24;
          goto FAILED;
          }
        break;


        /* ------------------------------------------------------------ */
        case '>':                 /* One-time brackets */
        bravalue = OP_ONCE;
        ptr++;
        break;


        /* ------------------------------------------------------------ */
        case 'C':                 /* Callout - may be followed by digits; */
        previous_callout = code;  /* Save for later completion */
        after_manual_callout = 1; /* Skip one item before completing */
        *code++ = OP_CALLOUT;
          {
          int n = 0;
          while ((digitab[*(++ptr)] & ctype_digit) != 0)
            n = n * 10 + *ptr - '0';
          if (*ptr != ')')
            {
            *errorcodeptr = ERR39;
            goto FAILED;
            }
          if (n > 255)
            {
            *errorcodeptr = ERR38;
            goto FAILED;
            }
          *code++ = n;
          PUT(code, 0, ptr - cd->start_pattern + 1);  /* Pattern offset */
          PUT(code, LINK_SIZE, 0);                    /* Default length */
          code += 2 * LINK_SIZE;
          }
        previous = NULL;
        continue;


        /* ------------------------------------------------------------ */
        case 'P':                 /* Python-style named subpattern handling */
        if (*(++ptr) == '=' || *ptr == '>')  /* Reference or recursion */
          {
          is_recurse = *ptr == '>';
          terminator = ')';
          goto NAMED_REF_OR_RECURSE;
          }
        else if (*ptr != '<')    /* Test for Python-style definition */
          {
          *errorcodeptr = ERR41;
          goto FAILED;
          }
        /* Fall through to handle (?P< as (?< is handled */


        /* ------------------------------------------------------------ */
        DEFINE_NAME:    /* Come here from (?< handling */
        case '\'':
          {
          terminator = (*ptr == '<')? '>' : '\'';
          name = ++ptr;

          while ((cd->ctypes[*ptr] & ctype_word) != 0) ptr++;
          namelen = ptr - name;

          /* In the pre-compile phase, just do a syntax check. */

          if (lengthptr != NULL)
            {
            if (*ptr != terminator)
              {
              *errorcodeptr = ERR42;
              goto FAILED;
              }
            if (cd->names_found >= MAX_NAME_COUNT)
              {
              *errorcodeptr = ERR49;
              goto FAILED;
              }
            if (namelen + 3 > cd->name_entry_size)
              {
              cd->name_entry_size = namelen + 3;
              if (namelen > MAX_NAME_SIZE)
                {
                *errorcodeptr = ERR48;
                goto FAILED;
                }
              }
            }

          /* In the real compile, create the entry in the table */

          else
            {
            slot = cd->name_table;
            for (i = 0; i < cd->names_found; i++)
              {
              int crc = memcmp(name, slot+2, namelen);
              if (crc == 0)
                {
                if (slot[2+namelen] == 0)
                  {
                  if ((options & PCRE_DUPNAMES) == 0)
                    {
                    *errorcodeptr = ERR43;
                    goto FAILED;
                    }
                  }
                else crc = -1;      /* Current name is substring */
                }
              if (crc < 0)
                {
                memmove(slot + cd->name_entry_size, slot,
                  (cd->names_found - i) * cd->name_entry_size);
                break;
                }
              slot += cd->name_entry_size;
              }

            PUT2(slot, 0, cd->bracount + 1);
            memcpy(slot + 2, name, namelen);
            slot[2+namelen] = 0;
            }
          }

        /* In both cases, count the number of names we've encountered. */

        ptr++;                    /* Move past > or ' */
        cd->names_found++;
        goto NUMBERED_GROUP;


        /* ------------------------------------------------------------ */
        case '&':                 /* Perl recursion/subroutine syntax */
        terminator = ')';
        is_recurse = TRUE;
        /* Fall through */

        /* We come here from the Python syntax above that handles both
        references (?P=name) and recursion (?P>name), as well as falling
        through from the Perl recursion syntax (?&name). We also come here from
        the Perl \k<name> or \k'name' back reference syntax and the \k{name}
        .NET syntax, and the Oniguruma \g<...> and \g'...' subroutine syntax. */

        NAMED_REF_OR_RECURSE:
        name = ++ptr;
        while ((cd->ctypes[*ptr] & ctype_word) != 0) ptr++;
        namelen = ptr - name;

        /* In the pre-compile phase, do a syntax check and set a dummy
        reference number. */

        if (lengthptr != NULL)
          {
          if (namelen == 0)
            {
            *errorcodeptr = ERR62;
            goto FAILED;
            }
          if (*ptr != terminator)
            {
            *errorcodeptr = ERR42;
            goto FAILED;
            }
          if (namelen > MAX_NAME_SIZE)
            {
            *errorcodeptr = ERR48;
            goto FAILED;
            }
          recno = 0;
          }

        /* In the real compile, seek the name in the table. We check the name
        first, and then check that we have reached the end of the name in the
        table. That way, if the name that is longer than any in the table,
        the comparison will fail without reading beyond the table entry. */

        else
          {
          slot = cd->name_table;
          for (i = 0; i < cd->names_found; i++)
            {
            if (strncmp((char *)name, (char *)slot+2, namelen) == 0 &&
                slot[2+namelen] == 0)
              break;
            slot += cd->name_entry_size;
            }

          if (i < cd->names_found)         /* Back reference */
            {
            recno = GET2(slot, 0);
            }
          else if ((recno =                /* Forward back reference */
                    find_parens(ptr, cd, name, namelen,
                      (options & PCRE_EXTENDED) != 0)) <= 0)
            {
            *errorcodeptr = ERR15;
            goto FAILED;
            }
          }

        /* In both phases, we can now go to the code than handles numerical
        recursion or backreferences. */

        if (is_recurse) goto HANDLE_RECURSION;
          else goto HANDLE_REFERENCE;


        /* ------------------------------------------------------------ */
        case 'R':                 /* Recursion */
        ptr++;                    /* Same as (?0)      */
        /* Fall through */


        /* ------------------------------------------------------------ */
        case '-': case '+':
        case '0': case '1': case '2': case '3': case '4':   /* Recursion or */
        case '5': case '6': case '7': case '8': case '9':   /* subroutine */
          {
          const uschar *called;
          terminator = ')';

          /* Come here from the \g<...> and \g'...' code (Oniguruma
          compatibility). However, the syntax has been checked to ensure that
          the ... are a (signed) number, so that neither ERR63 nor ERR29 will
          be called on this path, nor with the jump to OTHER_CHAR_AFTER_QUERY
          ever be taken. */

          HANDLE_NUMERICAL_RECURSION:

          if ((refsign = *ptr) == '+')
            {
            ptr++;
            if ((digitab[*ptr] & ctype_digit) == 0)
              {
              *errorcodeptr = ERR63;
              goto FAILED;
              }
            }
          else if (refsign == '-')
            {
            if ((digitab[ptr[1]] & ctype_digit) == 0)
              goto OTHER_CHAR_AFTER_QUERY;
            ptr++;
            }

          recno = 0;
          while((digitab[*ptr] & ctype_digit) != 0)
            recno = recno * 10 + *ptr++ - '0';

          if (*ptr != terminator)
            {
            *errorcodeptr = ERR29;
            goto FAILED;
            }

          if (refsign == '-')
            {
            if (recno == 0)
              {
              *errorcodeptr = ERR58;
              goto FAILED;
              }
            recno = cd->bracount - recno + 1;
            if (recno <= 0)
              {
              *errorcodeptr = ERR15;
              goto FAILED;
              }
            }
          else if (refsign == '+')
            {
            if (recno == 0)
              {
              *errorcodeptr = ERR58;
              goto FAILED;
              }
            recno += cd->bracount;
            }

          /* Come here from code above that handles a named recursion */

          HANDLE_RECURSION:

          previous = code;
          called = cd->start_code;

          /* When we are actually compiling, find the bracket that is being
          referenced. Temporarily end the regex in case it doesn't exist before
          this point. If we end up with a forward reference, first check that
          the bracket does occur later so we can give the error (and position)
          now. Then remember this forward reference in the workspace so it can
          be filled in at the end. */

          if (lengthptr == NULL)
            {
            *code = OP_END;
            if (recno != 0) called = find_bracket(cd->start_code, utf8, recno);

            /* Forward reference */

            if (called == NULL)
              {
              if (find_parens(ptr, cd, NULL, recno,
                    (options & PCRE_EXTENDED) != 0) < 0)
                {
                *errorcodeptr = ERR15;
                goto FAILED;
                }
              called = cd->start_code + recno;
              PUTINC(cd->hwm, 0, code + 2 + LINK_SIZE - cd->start_code);
              }

            /* If not a forward reference, and the subpattern is still open,
            this is a recursive call. We check to see if this is a left
            recursion that could loop for ever, and diagnose that case. */

            else if (GET(called, 1) == 0 &&
                     could_be_empty(called, code, bcptr, utf8))
              {
              *errorcodeptr = ERR40;
              goto FAILED;
              }
            }

          /* Insert the recursion/subroutine item, automatically wrapped inside
          "once" brackets. Set up a "previous group" length so that a
          subsequent quantifier will work. */

          *code = OP_ONCE;
          PUT(code, 1, 2 + 2*LINK_SIZE);
          code += 1 + LINK_SIZE;

          *code = OP_RECURSE;
          PUT(code, 1, called - cd->start_code);
          code += 1 + LINK_SIZE;

          *code = OP_KET;
          PUT(code, 1, 2 + 2*LINK_SIZE);
          code += 1 + LINK_SIZE;

          length_prevgroup = 3 + 3*LINK_SIZE;
          }

        /* Can't determine a first byte now */

        if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
        continue;


        /* ------------------------------------------------------------ */
        default:              /* Other characters: check option setting */
        OTHER_CHAR_AFTER_QUERY:
        set = unset = 0;
        optset = &set;

        while (*ptr != ')' && *ptr != ':')
          {
          switch (*ptr++)
            {
            case '-': optset = &unset; break;

            case 'J':    /* Record that it changed in the external options */
            *optset |= PCRE_DUPNAMES;
            cd->external_flags |= PCRE_JCHANGED;
            break;

            case 'i': *optset |= PCRE_CASELESS; break;
            case 'm': *optset |= PCRE_MULTILINE; break;
            case 's': *optset |= PCRE_DOTALL; break;
            case 'x': *optset |= PCRE_EXTENDED; break;
            case 'U': *optset |= PCRE_UNGREEDY; break;
            case 'X': *optset |= PCRE_EXTRA; break;

            default:  *errorcodeptr = ERR12;
                      ptr--;    /* Correct the offset */
                      goto FAILED;
            }
          }

        /* Set up the changed option bits, but don't change anything yet. */

        newoptions = (options | set) & (~unset);

        /* If the options ended with ')' this is not the start of a nested
        group with option changes, so the options change at this level. If this
        item is right at the start of the pattern, the options can be
        abstracted and made external in the pre-compile phase, and ignored in
        the compile phase. This can be helpful when matching -- for instance in
        caseless checking of required bytes.

        If the code pointer is not (cd->start_code + 1 + LINK_SIZE), we are
        definitely *not* at the start of the pattern because something has been
        compiled. In the pre-compile phase, however, the code pointer can have
        that value after the start, because it gets reset as code is discarded
        during the pre-compile. However, this can happen only at top level - if
        we are within parentheses, the starting BRA will still be present. At
        any parenthesis level, the length value can be used to test if anything
        has been compiled at that level. Thus, a test for both these conditions
        is necessary to ensure we correctly detect the start of the pattern in
        both phases.

        If we are not at the pattern start, compile code to change the ims
        options if this setting actually changes any of them, and reset the
        greedy defaults and the case value for firstbyte and reqbyte. */

        if (*ptr == ')')
          {
          if (code == cd->start_code + 1 + LINK_SIZE &&
               (lengthptr == NULL || *lengthptr == 2 + 2*LINK_SIZE))
            {
            cd->external_options = newoptions;
            }
         else
            {
            if ((options & PCRE_IMS) != (newoptions & PCRE_IMS))
              {
              *code++ = OP_OPT;
              *code++ = newoptions & PCRE_IMS;
              }
            greedy_default = ((newoptions & PCRE_UNGREEDY) != 0);
            greedy_non_default = greedy_default ^ 1;
            req_caseopt = ((newoptions & PCRE_CASELESS) != 0)? REQ_CASELESS : 0;
            }

          /* Change options at this level, and pass them back for use
          in subsequent branches. When not at the start of the pattern, this
          information is also necessary so that a resetting item can be
          compiled at the end of a group (if we are in a group). */

          *optionsptr = options = newoptions;
          previous = NULL;       /* This item can't be repeated */
          continue;              /* It is complete */
          }

        /* If the options ended with ':' we are heading into a nested group
        with possible change of options. Such groups are non-capturing and are
        not assertions of any kind. All we need to do is skip over the ':';
        the newoptions value is handled below. */

        bravalue = OP_BRA;
        ptr++;
        }     /* End of switch for character following (? */
      }       /* End of (? handling */

    /* Opening parenthesis not followed by '?'. If PCRE_NO_AUTO_CAPTURE is set,
    all unadorned brackets become non-capturing and behave like (?:...)
    brackets. */

    else if ((options & PCRE_NO_AUTO_CAPTURE) != 0)
      {
      bravalue = OP_BRA;
      }

    /* Else we have a capturing group. */

    else
      {
      NUMBERED_GROUP:
      cd->bracount += 1;
      PUT2(code, 1+LINK_SIZE, cd->bracount);
      skipbytes = 2;
      }

    /* Process nested bracketed regex. Assertions may not be repeated, but
    other kinds can be. All their opcodes are >= OP_ONCE. We copy code into a
    non-register variable in order to be able to pass its address because some
    compilers complain otherwise. Pass in a new setting for the ims options if
    they have changed. */

    previous = (bravalue >= OP_ONCE)? code : NULL;
    *code = bravalue;
    tempcode = code;
    tempreqvary = cd->req_varyopt;     /* Save value before bracket */
    length_prevgroup = 0;              /* Initialize for pre-compile phase */

    if (!compile_regex(
         newoptions,                   /* The complete new option state */
         options & PCRE_IMS,           /* The previous ims option state */
         &tempcode,                    /* Where to put code (updated) */
         &ptr,                         /* Input pointer (updated) */
         errorcodeptr,                 /* Where to put an error message */
         (bravalue == OP_ASSERTBACK ||
          bravalue == OP_ASSERTBACK_NOT), /* TRUE if back assert */
         reset_bracount,               /* True if (?| group */
         skipbytes,                    /* Skip over bracket number */
         &subfirstbyte,                /* For possible first char */
         &subreqbyte,                  /* For possible last char */
         bcptr,                        /* Current branch chain */
         cd,                           /* Tables block */
         (lengthptr == NULL)? NULL :   /* Actual compile phase */
           &length_prevgroup           /* Pre-compile phase */
         ))
      goto FAILED;

    /* At the end of compiling, code is still pointing to the start of the
    group, while tempcode has been updated to point past the end of the group
    and any option resetting that may follow it. The pattern pointer (ptr)
    is on the bracket. */

    /* If this is a conditional bracket, check that there are no more than
    two branches in the group, or just one if it's a DEFINE group. We do this
    in the real compile phase, not in the pre-pass, where the whole group may
    not be available. */

    if (bravalue == OP_COND && lengthptr == NULL)
      {
      uschar *tc = code;
      int condcount = 0;

      do {
         condcount++;
         tc += GET(tc,1);
         }
      while (*tc != OP_KET);

      /* A DEFINE group is never obeyed inline (the "condition" is always
      false). It must have only one branch. */

      if (code[LINK_SIZE+1] == OP_DEF)
        {
        if (condcount > 1)
          {
          *errorcodeptr = ERR54;
          goto FAILED;
          }
        bravalue = OP_DEF;   /* Just a flag to suppress char handling below */
        }

      /* A "normal" conditional group. If there is just one branch, we must not
      make use of its firstbyte or reqbyte, because this is equivalent to an
      empty second branch. */

      else
        {
        if (condcount > 2)
          {
          *errorcodeptr = ERR27;
          goto FAILED;
          }
        if (condcount == 1) subfirstbyte = subreqbyte = REQ_NONE;
        }
      }

    /* Error if hit end of pattern */

    if (*ptr != ')')
      {
      *errorcodeptr = ERR14;
      goto FAILED;
      }

    /* In the pre-compile phase, update the length by the length of the group,
    less the brackets at either end. Then reduce the compiled code to just a
    set of non-capturing brackets so that it doesn't use much memory if it is
    duplicated by a quantifier.*/

    if (lengthptr != NULL)
      {
      if (OFLOW_MAX - *lengthptr < length_prevgroup - 2 - 2*LINK_SIZE)
        {
        *errorcodeptr = ERR20;
        goto FAILED;
        }
      *lengthptr += length_prevgroup - 2 - 2*LINK_SIZE;
      *code++ = OP_BRA;
      PUTINC(code, 0, 1 + LINK_SIZE);
      *code++ = OP_KET;
      PUTINC(code, 0, 1 + LINK_SIZE);
      break;    /* No need to waste time with special character handling */
      }

    /* Otherwise update the main code pointer to the end of the group. */

    code = tempcode;

    /* For a DEFINE group, required and first character settings are not
    relevant. */

    if (bravalue == OP_DEF) break;

    /* Handle updating of the required and first characters for other types of
    group. Update for normal brackets of all kinds, and conditions with two
    branches (see code above). If the bracket is followed by a quantifier with
    zero repeat, we have to back off. Hence the definition of zeroreqbyte and
    zerofirstbyte outside the main loop so that they can be accessed for the
    back off. */

    zeroreqbyte = reqbyte;
    zerofirstbyte = firstbyte;
    groupsetfirstbyte = FALSE;

    if (bravalue >= OP_ONCE)
      {
      /* If we have not yet set a firstbyte in this branch, take it from the
      subpattern, remembering that it was set here so that a repeat of more
      than one can replicate it as reqbyte if necessary. If the subpattern has
      no firstbyte, set "none" for the whole branch. In both cases, a zero
      repeat forces firstbyte to "none". */

      if (firstbyte == REQ_UNSET)
        {
        if (subfirstbyte >= 0)
          {
          firstbyte = subfirstbyte;
          groupsetfirstbyte = TRUE;
          }
        else firstbyte = REQ_NONE;
        zerofirstbyte = REQ_NONE;
        }

      /* If firstbyte was previously set, convert the subpattern's firstbyte
      into reqbyte if there wasn't one, using the vary flag that was in
      existence beforehand. */

      else if (subfirstbyte >= 0 && subreqbyte < 0)
        subreqbyte = subfirstbyte | tempreqvary;

      /* If the subpattern set a required byte (or set a first byte that isn't
      really the first byte - see above), set it. */

      if (subreqbyte >= 0) reqbyte = subreqbyte;
      }

    /* For a forward assertion, we take the reqbyte, if set. This can be
    helpful if the pattern that follows the assertion doesn't set a different
    char. For example, it's useful for /(?=abcde).+/. We can't set firstbyte
    for an assertion, however because it leads to incorrect effect for patterns
    such as /(?=a)a.+/ when the "real" "a" would then become a reqbyte instead
    of a firstbyte. This is overcome by a scan at the end if there's no
    firstbyte, looking for an asserted first char. */

    else if (bravalue == OP_ASSERT && subreqbyte >= 0) reqbyte = subreqbyte;
    break;     /* End of processing '(' */


    /* ===================================================================*/
    /* Handle metasequences introduced by \. For ones like \d, the ESC_ values
    are arranged to be the negation of the corresponding OP_values. For the
    back references, the values are ESC_REF plus the reference number. Only
    back references and those types that consume a character may be repeated.
    We can test for values between ESC_b and ESC_Z for the latter; this may
    have to change if any new ones are ever created. */

    case '\\':
    tempptr = ptr;
    c = check_escape(&ptr, errorcodeptr, cd->bracount, options, FALSE);
    if (*errorcodeptr != 0) goto FAILED;

    if (c < 0)
      {
      if (-c == ESC_Q)            /* Handle start of quoted string */
        {
        if (ptr[1] == '\\' && ptr[2] == 'E') ptr += 2; /* avoid empty string */
          else inescq = TRUE;
        continue;
        }

      if (-c == ESC_E) continue;  /* Perl ignores an orphan \E */

      /* For metasequences that actually match a character, we disable the
      setting of a first character if it hasn't already been set. */

      if (firstbyte == REQ_UNSET && -c > ESC_b && -c < ESC_Z)
        firstbyte = REQ_NONE;

      /* Set values to reset to if this is followed by a zero repeat. */

      zerofirstbyte = firstbyte;
      zeroreqbyte = reqbyte;

      /* \g<name> or \g'name' is a subroutine call by name and \g<n> or \g'n'
      is a subroutine call by number (Oniguruma syntax). In fact, the value
      -ESC_g is returned only for these cases. So we don't need to check for <
      or ' if the value is -ESC_g. For the Perl syntax \g{n} the value is
      -ESC_REF+n, and for the Perl syntax \g{name} the result is -ESC_k (as
      that is a synonym for a named back reference). */

      if (-c == ESC_g)
        {
        const uschar *p;
        save_hwm = cd->hwm;   /* Normally this is set when '(' is read */
        terminator = (*(++ptr) == '<')? '>' : '\'';

        /* These two statements stop the compiler for warning about possibly
        unset variables caused by the jump to HANDLE_NUMERICAL_RECURSION. In
        fact, because we actually check for a number below, the paths that
        would actually be in error are never taken. */

        skipbytes = 0;
        reset_bracount = FALSE;

        /* Test for a name */

        if (ptr[1] != '+' && ptr[1] != '-')
          {
          BOOL isnumber = TRUE;
          for (p = ptr + 1; *p != 0 && *p != terminator; p++)
            {
            if ((cd->ctypes[*p] & ctype_digit) == 0) isnumber = FALSE;
            if ((cd->ctypes[*p] & ctype_word) == 0) break;
            }
          if (*p != terminator)
            {
            *errorcodeptr = ERR57;
            break;
            }
          if (isnumber)
            {
            ptr++;
            goto HANDLE_NUMERICAL_RECURSION;
            }
          is_recurse = TRUE;
          goto NAMED_REF_OR_RECURSE;
          }

        /* Test a signed number in angle brackets or quotes. */

        p = ptr + 2;
        while ((digitab[*p] & ctype_digit) != 0) p++;
        if (*p != terminator)
          {
          *errorcodeptr = ERR57;
          break;
          }
        ptr++;
        goto HANDLE_NUMERICAL_RECURSION;
        }

      /* \k<name> or \k'name' is a back reference by name (Perl syntax).
      We also support \k{name} (.NET syntax) */

      if (-c == ESC_k && (ptr[1] == '<' || ptr[1] == '\'' || ptr[1] == '{'))
        {
        is_recurse = FALSE;
        terminator = (*(++ptr) == '<')? '>' : (*ptr == '\'')? '\'' : '}';
        goto NAMED_REF_OR_RECURSE;
        }

      /* Back references are handled specially; must disable firstbyte if
      not set to cope with cases like (?=(\w+))\1: which would otherwise set
      ':' later. */

      if (-c >= ESC_REF)
        {
        recno = -c - ESC_REF;

        HANDLE_REFERENCE:    /* Come here from named backref handling */
        if (firstbyte == REQ_UNSET) firstbyte = REQ_NONE;
        previous = code;
        *code++ = OP_REF;
        PUT2INC(code, 0, recno);
        cd->backref_map |= (recno < 32)? (1 << recno) : 1;
        if (recno > cd->top_backref) cd->top_backref = recno;
        }

      /* So are Unicode property matches, if supported. */

#ifdef SUPPORT_UCP
      else if (-c == ESC_P || -c == ESC_p)
        {
        BOOL negated;
        int pdata;
        int ptype = get_ucp(&ptr, &negated, &pdata, errorcodeptr);
        if (ptype < 0) goto FAILED;
        previous = code;
        *code++ = ((-c == ESC_p) != negated)? OP_PROP : OP_NOTPROP;
        *code++ = ptype;
        *code++ = pdata;
        }
#else

      /* If Unicode properties are not supported, \X, \P, and \p are not
      allowed. */

      else if (-c == ESC_X || -c == ESC_P || -c == ESC_p)
        {
        *errorcodeptr = ERR45;
        goto FAILED;
        }
#endif

      /* For the rest (including \X when Unicode properties are supported), we
      can obtain the OP value by negating the escape value. */

      else
        {
        previous = (-c > ESC_b && -c < ESC_Z)? code : NULL;
        *code++ = -c;
        }
      continue;
      }

    /* We have a data character whose value is in c. In UTF-8 mode it may have
    a value > 127. We set its representation in the length/buffer, and then
    handle it as a data character. */

#ifdef SUPPORT_UTF8
    if (utf8 && c > 127)
      mclength = _pcre_ord2utf8(c, mcbuffer);
    else
#endif

     {
     mcbuffer[0] = c;
     mclength = 1;
     }
    goto ONE_CHAR;


    /* ===================================================================*/
    /* Handle a literal character. It is guaranteed not to be whitespace or #
    when the extended flag is set. If we are in UTF-8 mode, it may be a
    multi-byte literal character. */

    default:
    NORMAL_CHAR:
    mclength = 1;
    mcbuffer[0] = c;

#ifdef SUPPORT_UTF8
    if (utf8 && c >= 0xc0)
      {
      while ((ptr[1] & 0xc0) == 0x80)
        mcbuffer[mclength++] = *(++ptr);
      }
#endif

    /* At this point we have the character's bytes in mcbuffer, and the length
    in mclength. When not in UTF-8 mode, the length is always 1. */

    ONE_CHAR:
    previous = code;
    *code++ = ((options & PCRE_CASELESS) != 0)? OP_CHARNC : OP_CHAR;
    for (c = 0; c < mclength; c++) *code++ = mcbuffer[c];

    /* Remember if \r or \n were seen */

    if (mcbuffer[0] == '\r' || mcbuffer[0] == '\n')
      cd->external_flags |= PCRE_HASCRORLF;

    /* Set the first and required bytes appropriately. If no previous first
    byte, set it from this character, but revert to none on a zero repeat.
    Otherwise, leave the firstbyte value alone, and don't change it on a zero
    repeat. */

    if (firstbyte == REQ_UNSET)
      {
      zerofirstbyte = REQ_NONE;
      zeroreqbyte = reqbyte;

      /* If the character is more than one byte long, we can set firstbyte
      only if it is not to be matched caselessly. */

      if (mclength == 1 || req_caseopt == 0)
        {
        firstbyte = mcbuffer[0] | req_caseopt;
        if (mclength != 1) reqbyte = code[-1] | cd->req_varyopt;
        }
      else firstbyte = reqbyte = REQ_NONE;
      }

    /* firstbyte was previously set; we can set reqbyte only the length is
    1 or the matching is caseful. */

    else
      {
      zerofirstbyte = firstbyte;
      zeroreqbyte = reqbyte;
      if (mclength == 1 || req_caseopt == 0)
        reqbyte = code[-1] | req_caseopt | cd->req_varyopt;
      }

    break;            /* End of literal character handling */
    }
  }                   /* end of big loop */


/* Control never reaches here by falling through, only by a goto for all the
error states. Pass back the position in the pattern so that it can be displayed
to the user for diagnosing the error. */

FAILED:
*ptrptr = ptr;
return FALSE;
}




/*************************************************
*     Compile sequence of alternatives           *
*************************************************/

/* On entry, ptr is pointing past the bracket character, but on return it
points to the closing bracket, or vertical bar, or end of string. The code
variable is pointing at the byte into which the BRA operator has been stored.
If the ims options are changed at the start (for a (?ims: group) or during any
branch, we need to insert an OP_OPT item at the start of every following branch
to ensure they get set correctly at run time, and also pass the new options
into every subsequent branch compile.

This function is used during the pre-compile phase when we are trying to find
out the amount of memory needed, as well as during the real compile phase. The
value of lengthptr distinguishes the two phases.

Arguments:
  options        option bits, including any changes for this subpattern
  oldims         previous settings of ims option bits
  codeptr        -> the address of the current code pointer
  ptrptr         -> the address of the current pattern pointer
  errorcodeptr   -> pointer to error code variable
  lookbehind     TRUE if this is a lookbehind assertion
  reset_bracount TRUE to reset the count for each branch
  skipbytes      skip this many bytes at start (for brackets and OP_COND)
  firstbyteptr   place to put the first required character, or a negative number
  reqbyteptr     place to put the last required character, or a negative number
  bcptr          pointer to the chain of currently open branches
  cd             points to the data block with tables pointers etc.
  lengthptr      NULL during the real compile phase
                 points to length accumulator during pre-compile phase

Returns:         TRUE on success
*/

static BOOL
compile_regex(int options, int oldims, uschar **codeptr, const uschar **ptrptr,
  int *errorcodeptr, BOOL lookbehind, BOOL reset_bracount, int skipbytes,
  int *firstbyteptr, int *reqbyteptr, branch_chain *bcptr, compile_data *cd,
  int *lengthptr)
{
const uschar *ptr = *ptrptr;
uschar *code = *codeptr;
uschar *last_branch = code;
uschar *start_bracket = code;
uschar *reverse_count = NULL;
int firstbyte, reqbyte;
int branchfirstbyte, branchreqbyte;
int length;
int orig_bracount;
int max_bracount;
branch_chain bc;

bc.outer = bcptr;
bc.current = code;

firstbyte = reqbyte = REQ_UNSET;

/* Accumulate the length for use in the pre-compile phase. Start with the
length of the BRA and KET and any extra bytes that are required at the
beginning. We accumulate in a local variable to save frequent testing of
lenthptr for NULL. We cannot do this by looking at the value of code at the
start and end of each alternative, because compiled items are discarded during
the pre-compile phase so that the work space is not exceeded. */

length = 2 + 2*LINK_SIZE + skipbytes;

/* WARNING: If the above line is changed for any reason, you must also change
the code that abstracts option settings at the start of the pattern and makes
them global. It tests the value of length for (2 + 2*LINK_SIZE) in the
pre-compile phase to find out whether anything has yet been compiled or not. */

/* Offset is set zero to mark that this bracket is still open */

PUT(code, 1, 0);
code += 1 + LINK_SIZE + skipbytes;

/* Loop for each alternative branch */

orig_bracount = max_bracount = cd->bracount;
for (;;)
  {
  /* For a (?| group, reset the capturing bracket count so that each branch
  uses the same numbers. */

  if (reset_bracount) cd->bracount = orig_bracount;

  /* Handle a change of ims options at the start of the branch */

  if ((options & PCRE_IMS) != oldims)
    {
    *code++ = OP_OPT;
    *code++ = options & PCRE_IMS;
    length += 2;
    }

  /* Set up dummy OP_REVERSE if lookbehind assertion */

  if (lookbehind)
    {
    *code++ = OP_REVERSE;
    reverse_count = code;
    PUTINC(code, 0, 0);
    length += 1 + LINK_SIZE;
    }

  /* Now compile the branch; in the pre-compile phase its length gets added
  into the length. */

  if (!compile_branch(&options, &code, &ptr, errorcodeptr, &branchfirstbyte,
        &branchreqbyte, &bc, cd, (lengthptr == NULL)? NULL : &length))
    {
    *ptrptr = ptr;
    return FALSE;
    }

  /* Keep the highest bracket count in case (?| was used and some branch
  has fewer than the rest. */

  if (cd->bracount > max_bracount) max_bracount = cd->bracount;

  /* In the real compile phase, there is some post-processing to be done. */

  if (lengthptr == NULL)
    {
    /* If this is the first branch, the firstbyte and reqbyte values for the
    branch become the values for the regex. */

    if (*last_branch != OP_ALT)
      {
      firstbyte = branchfirstbyte;
      reqbyte = branchreqbyte;
      }

    /* If this is not the first branch, the first char and reqbyte have to
    match the values from all the previous branches, except that if the
    previous value for reqbyte didn't have REQ_VARY set, it can still match,
    and we set REQ_VARY for the regex. */

    else
      {
      /* If we previously had a firstbyte, but it doesn't match the new branch,
      we have to abandon the firstbyte for the regex, but if there was
      previously no reqbyte, it takes on the value of the old firstbyte. */

      if (firstbyte >= 0 && firstbyte != branchfirstbyte)
        {
        if (reqbyte < 0) reqbyte = firstbyte;
        firstbyte = REQ_NONE;
        }

      /* If we (now or from before) have no firstbyte, a firstbyte from the
      branch becomes a reqbyte if there isn't a branch reqbyte. */

      if (firstbyte < 0 && branchfirstbyte >= 0 && branchreqbyte < 0)
          branchreqbyte = branchfirstbyte;

      /* Now ensure that the reqbytes match */

      if ((reqbyte & ~REQ_VARY) != (branchreqbyte & ~REQ_VARY))
        reqbyte = REQ_NONE;
      else reqbyte |= branchreqbyte;   /* To "or" REQ_VARY */
      }

    /* If lookbehind, check that this branch matches a fixed-length string, and
    put the length into the OP_REVERSE item. Temporarily mark the end of the
    branch with OP_END. */

    if (lookbehind)
      {
      int fixed_length;
      *code = OP_END;
      fixed_length = find_fixedlength(last_branch, options);
      DPRINTF(("fixed length = %d\n", fixed_length));
      if (fixed_length < 0)
        {
        *errorcodeptr = (fixed_length == -2)? ERR36 : ERR25;
        *ptrptr = ptr;
        return FALSE;
        }
      PUT(reverse_count, 0, fixed_length);
      }
    }

  /* Reached end of expression, either ')' or end of pattern. In the real
  compile phase, go back through the alternative branches and reverse the chain
  of offsets, with the field in the BRA item now becoming an offset to the
  first alternative. If there are no alternatives, it points to the end of the
  group. The length in the terminating ket is always the length of the whole
  bracketed item. If any of the ims options were changed inside the group,
  compile a resetting op-code following, except at the very end of the pattern.
  Return leaving the pointer at the terminating char. */

  if (*ptr != '|')
    {
    if (lengthptr == NULL)
      {
      int branch_length = code - last_branch;
      do
        {
        int prev_length = GET(last_branch, 1);
        PUT(last_branch, 1, branch_length);
        branch_length = prev_length;
        last_branch -= branch_length;
        }
      while (branch_length > 0);
      }

    /* Fill in the ket */

    *code = OP_KET;
    PUT(code, 1, code - start_bracket);
    code += 1 + LINK_SIZE;

    /* Resetting option if needed */

    if ((options & PCRE_IMS) != oldims && *ptr == ')')
      {
      *code++ = OP_OPT;
      *code++ = oldims;
      length += 2;
      }

    /* Retain the highest bracket number, in case resetting was used. */

    cd->bracount = max_bracount;

    /* Set values to pass back */

    *codeptr = code;
    *ptrptr = ptr;
    *firstbyteptr = firstbyte;
    *reqbyteptr = reqbyte;
    if (lengthptr != NULL)
      {
      if (OFLOW_MAX - *lengthptr < length)
        {
        *errorcodeptr = ERR20;
        return FALSE;
        }
      *lengthptr += length;
      }
    return TRUE;
    }

  /* Another branch follows. In the pre-compile phase, we can move the code
  pointer back to where it was for the start of the first branch. (That is,
  pretend that each branch is the only one.)

  In the real compile phase, insert an ALT node. Its length field points back
  to the previous branch while the bracket remains open. At the end the chain
  is reversed. It's done like this so that the start of the bracket has a
  zero offset until it is closed, making it possible to detect recursion. */

  if (lengthptr != NULL)
    {
    code = *codeptr + 1 + LINK_SIZE + skipbytes;
    length += 1 + LINK_SIZE;
    }
  else
    {
    *code = OP_ALT;
    PUT(code, 1, code - last_branch);
    bc.current = last_branch = code;
    code += 1 + LINK_SIZE;
    }

  ptr++;
  }
/* Control never reaches here */
}




/*************************************************
*          Check for anchored expression         *
*************************************************/

/* Try to find out if this is an anchored regular expression. Consider each
alternative branch. If they all start with OP_SOD or OP_CIRC, or with a bracket
all of whose alternatives start with OP_SOD or OP_CIRC (recurse ad lib), then
it's anchored. However, if this is a multiline pattern, then only OP_SOD
counts, since OP_CIRC can match in the middle.

We can also consider a regex to be anchored if OP_SOM starts all its branches.
This is the code for \G, which means "match at start of match position, taking
into account the match offset".

A branch is also implicitly anchored if it starts with .* and DOTALL is set,
because that will try the rest of the pattern at all possible matching points,
so there is no point trying again.... er ....

.... except when the .* appears inside capturing parentheses, and there is a
subsequent back reference to those parentheses. We haven't enough information
to catch that case precisely.

At first, the best we could do was to detect when .* was in capturing brackets
and the highest back reference was greater than or equal to that level.
However, by keeping a bitmap of the first 31 back references, we can catch some
of the more common cases more precisely.

Arguments:
  code           points to start of expression (the bracket)
  options        points to the options setting
  bracket_map    a bitmap of which brackets we are inside while testing; this
                  handles up to substring 31; after that we just have to take
                  the less precise approach
  backref_map    the back reference bitmap

Returns:     TRUE or FALSE
*/

static BOOL
is_anchored(register const uschar *code, int *options, unsigned int bracket_map,
  unsigned int backref_map)
{
do {
   const uschar *scode = first_significant_code(code + _pcre_OP_lengths[*code],
     options, PCRE_MULTILINE, FALSE);
   register int op = *scode;

   /* Non-capturing brackets */

   if (op == OP_BRA)
     {
     if (!is_anchored(scode, options, bracket_map, backref_map)) return FALSE;
     }

   /* Capturing brackets */

   else if (op == OP_CBRA)
     {
     int n = GET2(scode, 1+LINK_SIZE);
     int new_map = bracket_map | ((n < 32)? (1 << n) : 1);
     if (!is_anchored(scode, options, new_map, backref_map)) return FALSE;
     }

   /* Other brackets */

   else if (op == OP_ASSERT || op == OP_ONCE || op == OP_COND)
     {
     if (!is_anchored(scode, options, bracket_map, backref_map)) return FALSE;
     }

   /* .* is not anchored unless DOTALL is set (which generates OP_ALLANY) and
   it isn't in brackets that are or may be referenced. */

   else if ((op == OP_TYPESTAR || op == OP_TYPEMINSTAR ||
             op == OP_TYPEPOSSTAR))
     {
     if (scode[1] != OP_ALLANY || (bracket_map & backref_map) != 0)
       return FALSE;
     }

   /* Check for explicit anchoring */

   else if (op != OP_SOD && op != OP_SOM &&
           ((*options & PCRE_MULTILINE) != 0 || op != OP_CIRC))
     return FALSE;
   code += GET(code, 1);
   }
while (*code == OP_ALT);   /* Loop for each alternative */
return TRUE;
}



/*************************************************
*         Check for starting with ^ or .*        *
*************************************************/

/* This is called to find out if every branch starts with ^ or .* so that
"first char" processing can be done to speed things up in multiline
matching and for non-DOTALL patterns that start with .* (which must start at
the beginning or after \n). As in the case of is_anchored() (see above), we
have to take account of back references to capturing brackets that contain .*
because in that case we can't make the assumption.

Arguments:
  code           points to start of expression (the bracket)
  bracket_map    a bitmap of which brackets we are inside while testing; this
                  handles up to substring 31; after that we just have to take
                  the less precise approach
  backref_map    the back reference bitmap

Returns:         TRUE or FALSE
*/

static BOOL
is_startline(const uschar *code, unsigned int bracket_map,
  unsigned int backref_map)
{
do {
   const uschar *scode = first_significant_code(code + _pcre_OP_lengths[*code],
     NULL, 0, FALSE);
   register int op = *scode;

   /* Non-capturing brackets */

   if (op == OP_BRA)
     {
     if (!is_startline(scode, bracket_map, backref_map)) return FALSE;
     }

   /* Capturing brackets */

   else if (op == OP_CBRA)
     {
     int n = GET2(scode, 1+LINK_SIZE);
     int new_map = bracket_map | ((n < 32)? (1 << n) : 1);
     if (!is_startline(scode, new_map, backref_map)) return FALSE;
     }

   /* Other brackets */

   else if (op == OP_ASSERT || op == OP_ONCE || op == OP_COND)
     { if (!is_startline(scode, bracket_map, backref_map)) return FALSE; }

   /* .* means "start at start or after \n" if it isn't in brackets that
   may be referenced. */

   else if (op == OP_TYPESTAR || op == OP_TYPEMINSTAR || op == OP_TYPEPOSSTAR)
     {
     if (scode[1] != OP_ANY || (bracket_map & backref_map) != 0) return FALSE;
     }

   /* Check for explicit circumflex */

   else if (op != OP_CIRC) return FALSE;

   /* Move on to the next alternative */

   code += GET(code, 1);
   }
while (*code == OP_ALT);  /* Loop for each alternative */
return TRUE;
}



/*************************************************
*       Check for asserted fixed first char      *
*************************************************/

/* During compilation, the "first char" settings from forward assertions are
discarded, because they can cause conflicts with actual literals that follow.
However, if we end up without a first char setting for an unanchored pattern,
it is worth scanning the regex to see if there is an initial asserted first
char. If all branches start with the same asserted char, or with a bracket all
of whose alternatives start with the same asserted char (recurse ad lib), then
we return that char, otherwise -1.

Arguments:
  code       points to start of expression (the bracket)
  options    pointer to the options (used to check casing changes)
  inassert   TRUE if in an assertion

Returns:     -1 or the fixed first char
*/

static int
find_firstassertedchar(const uschar *code, int *options, BOOL inassert)
{
register int c = -1;
do {
   int d;
   const uschar *scode =
     first_significant_code(code + 1+LINK_SIZE, options, PCRE_CASELESS, TRUE);
   register int op = *scode;

   switch(op)
     {
     default:
     return -1;

     case OP_BRA:
     case OP_CBRA:
     case OP_ASSERT:
     case OP_ONCE:
     case OP_COND:
     if ((d = find_firstassertedchar(scode, options, op == OP_ASSERT)) < 0)
       return -1;
     if (c < 0) c = d; else if (c != d) return -1;
     break;

     case OP_EXACT:       /* Fall through */
     scode += 2;

     case OP_CHAR:
     case OP_CHARNC:
     case OP_PLUS:
     case OP_MINPLUS:
     case OP_POSPLUS:
     if (!inassert) return -1;
     if (c < 0)
       {
       c = scode[1];
       if ((*options & PCRE_CASELESS) != 0) c |= REQ_CASELESS;
       }
     else if (c != scode[1]) return -1;
     break;
     }

   code += GET(code, 1);
   }
while (*code == OP_ALT);
return c;
}



/*************************************************
*        Compile a Regular Expression            *
*************************************************/

/* This function takes a string and returns a pointer to a block of store
holding a compiled version of the expression. The original API for this
function had no error code return variable; it is retained for backwards
compatibility. The new function is given a new name.

Arguments:
  pattern       the regular expression
  options       various option bits
  errorcodeptr  pointer to error code variable (pcre_compile2() only)
                  can be NULL if you don't want a code value
  errorptr      pointer to pointer to error text
  erroroffset   ptr offset in pattern where error was detected
  tables        pointer to character tables or NULL

Returns:        pointer to compiled data block, or NULL on error,
                with errorptr and erroroffset set
*/

PCRE_EXP_DEFN pcre * PCRE_CALL_CONVENTION
pcre_compile(const char *pattern, int options, const char **errorptr,
  int *erroroffset, const unsigned char *tables)
{
return pcre_compile2(pattern, options, NULL, errorptr, erroroffset, tables);
}


PCRE_EXP_DEFN pcre * PCRE_CALL_CONVENTION
pcre_compile2(const char *pattern, int options, int *errorcodeptr,
  const char **errorptr, int *erroroffset, const unsigned char *tables)
{
real_pcre *re;
int length = 1;  /* For final END opcode */
int firstbyte, reqbyte, newline;
int errorcode = 0;
int skipatstart = 0;
#ifdef SUPPORT_UTF8
BOOL utf8;
#endif
size_t size;
uschar *code;
const uschar *codestart;
const uschar *ptr;
compile_data compile_block;
compile_data *cd = &compile_block;

/* This space is used for "compiling" into during the first phase, when we are
computing the amount of memory that is needed. Compiled items are thrown away
as soon as possible, so that a fairly large buffer should be sufficient for
this purpose. The same space is used in the second phase for remembering where
to fill in forward references to subpatterns. */

uschar cworkspace[COMPILE_WORK_SIZE];

/* Set this early so that early errors get offset 0. */

ptr = (const uschar *)pattern;

/* We can't pass back an error message if errorptr is NULL; I guess the best we
can do is just return NULL, but we can set a code value if there is a code
pointer. */

if (errorptr == NULL)
  {
  if (errorcodeptr != NULL) *errorcodeptr = 99;
  return NULL;
  }

*errorptr = NULL;
if (errorcodeptr != NULL) *errorcodeptr = ERR0;

/* However, we can give a message for this error */

if (erroroffset == NULL)
  {
  errorcode = ERR16;
  goto PCRE_EARLY_ERROR_RETURN2;
  }

*erroroffset = 0;

/* Can't support UTF8 unless PCRE has been compiled to include the code. */

#ifdef SUPPORT_UTF8
utf8 = (options & PCRE_UTF8) != 0;
if (utf8 && (options & PCRE_NO_UTF8_CHECK) == 0 &&
     (*erroroffset = _pcre_valid_utf8((uschar *)pattern, -1)) >= 0)
  {
  errorcode = ERR44;
  goto PCRE_EARLY_ERROR_RETURN2;
  }
#else
if ((options & PCRE_UTF8) != 0)
  {
  errorcode = ERR32;
  goto PCRE_EARLY_ERROR_RETURN;
  }
#endif

if ((options & ~PUBLIC_OPTIONS) != 0)
  {
  errorcode = ERR17;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Set up pointers to the individual character tables */

if (tables == NULL) tables = _pcre_default_tables;
cd->lcc = tables + lcc_offset;
cd->fcc = tables + fcc_offset;
cd->cbits = tables + cbits_offset;
cd->ctypes = tables + ctypes_offset;

/* Check for global one-time settings at the start of the pattern, and remember
the offset for later. */

while (ptr[skipatstart] == '(' && ptr[skipatstart+1] == '*')
  {
  int newnl = 0;
  int newbsr = 0;

  if (strncmp((char *)(ptr+skipatstart+2), "CR)", 3) == 0)
    { skipatstart += 5; newnl = PCRE_NEWLINE_CR; }
  else if (strncmp((char *)(ptr+skipatstart+2), "LF)", 3)  == 0)
    { skipatstart += 5; newnl = PCRE_NEWLINE_LF; }
  else if (strncmp((char *)(ptr+skipatstart+2), "CRLF)", 5)  == 0)
    { skipatstart += 7; newnl = PCRE_NEWLINE_CR + PCRE_NEWLINE_LF; }
  else if (strncmp((char *)(ptr+skipatstart+2), "ANY)", 4) == 0)
    { skipatstart += 6; newnl = PCRE_NEWLINE_ANY; }
  else if (strncmp((char *)(ptr+skipatstart+2), "ANYCRLF)", 8)  == 0)
    { skipatstart += 10; newnl = PCRE_NEWLINE_ANYCRLF; }

  else if (strncmp((char *)(ptr+skipatstart+2), "BSR_ANYCRLF)", 12) == 0)
    { skipatstart += 14; newbsr = PCRE_BSR_ANYCRLF; }
  else if (strncmp((char *)(ptr+skipatstart+2), "BSR_UNICODE)", 12) == 0)
    { skipatstart += 14; newbsr = PCRE_BSR_UNICODE; }

  if (newnl != 0)
    options = (options & ~PCRE_NEWLINE_BITS) | newnl;
  else if (newbsr != 0)
    options = (options & ~(PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)) | newbsr;
  else break;
  }

/* Check validity of \R options. */

switch (options & (PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE))
  {
  case 0:
  case PCRE_BSR_ANYCRLF:
  case PCRE_BSR_UNICODE:
  break;
  default: errorcode = ERR56; goto PCRE_EARLY_ERROR_RETURN;
  }

/* Handle different types of newline. The three bits give seven cases. The
current code allows for fixed one- or two-byte sequences, plus "any" and
"anycrlf". */

switch (options & PCRE_NEWLINE_BITS)
  {
  case 0: newline = NEWLINE; break;   /* Build-time default */
  case PCRE_NEWLINE_CR: newline = '\r'; break;
  case PCRE_NEWLINE_LF: newline = '\n'; break;
  case PCRE_NEWLINE_CR+
       PCRE_NEWLINE_LF: newline = ('\r' << 8) | '\n'; break;
  case PCRE_NEWLINE_ANY: newline = -1; break;
  case PCRE_NEWLINE_ANYCRLF: newline = -2; break;
  default: errorcode = ERR56; goto PCRE_EARLY_ERROR_RETURN;
  }

if (newline == -2)
  {
  cd->nltype = NLTYPE_ANYCRLF;
  }
else if (newline < 0)
  {
  cd->nltype = NLTYPE_ANY;
  }
else
  {
  cd->nltype = NLTYPE_FIXED;
  if (newline > 255)
    {
    cd->nllen = 2;
    cd->nl[0] = (newline >> 8) & 255;
    cd->nl[1] = newline & 255;
    }
  else
    {
    cd->nllen = 1;
    cd->nl[0] = newline;
    }
  }

/* Maximum back reference and backref bitmap. The bitmap records up to 31 back
references to help in deciding whether (.*) can be treated as anchored or not.
*/

cd->top_backref = 0;
cd->backref_map = 0;

/* Reflect pattern for debugging output */

DPRINTF(("------------------------------------------------------------------\n"));
DPRINTF(("%s\n", pattern));

/* Pretend to compile the pattern while actually just accumulating the length
of memory required. This behaviour is triggered by passing a non-NULL final
argument to compile_regex(). We pass a block of workspace (cworkspace) for it
to compile parts of the pattern into; the compiled code is discarded when it is
no longer needed, so hopefully this workspace will never overflow, though there
is a test for its doing so. */

cd->bracount = cd->final_bracount = 0;
cd->names_found = 0;
cd->name_entry_size = 0;
cd->name_table = NULL;
cd->start_workspace = cworkspace;
cd->start_code = cworkspace;
cd->hwm = cworkspace;
cd->start_pattern = (const uschar *)pattern;
cd->end_pattern = (const uschar *)(pattern + strlen(pattern));
cd->req_varyopt = 0;
cd->external_options = options;
cd->external_flags = 0;

/* Now do the pre-compile. On error, errorcode will be set non-zero, so we
don't need to look at the result of the function here. The initial options have
been put into the cd block so that they can be changed if an option setting is
found within the regex right at the beginning. Bringing initial option settings
outside can help speed up starting point checks. */

ptr += skipatstart;
code = cworkspace;
*code = OP_BRA;
(void)compile_regex(cd->external_options, cd->external_options & PCRE_IMS,
  &code, &ptr, &errorcode, FALSE, FALSE, 0, &firstbyte, &reqbyte, NULL, cd,
  &length);
if (errorcode != 0) goto PCRE_EARLY_ERROR_RETURN;

DPRINTF(("end pre-compile: length=%d workspace=%d\n", length,
  cd->hwm - cworkspace));

if (length > MAX_PATTERN_SIZE)
  {
  errorcode = ERR20;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Compute the size of data block needed and get it, either from malloc or
externally provided function. Integer overflow should no longer be possible
because nowadays we limit the maximum value of cd->names_found and
cd->name_entry_size. */

size = length + sizeof(real_pcre) + cd->names_found * (cd->name_entry_size + 3);
re = (real_pcre *)(pcre_malloc)(size);

if (re == NULL)
  {
  errorcode = ERR21;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Put in the magic number, and save the sizes, initial options, internal
flags, and character table pointer. NULL is used for the default character
tables. The nullpad field is at the end; it's there to help in the case when a
regex compiled on a system with 4-byte pointers is run on another with 8-byte
pointers. */

re->magic_number = MAGIC_NUMBER;
re->size = size;
re->options = cd->external_options;
re->flags = cd->external_flags;
re->dummy1 = 0;
re->first_byte = 0;
re->req_byte = 0;
re->name_table_offset = sizeof(real_pcre);
re->name_entry_size = cd->name_entry_size;
re->name_count = cd->names_found;
re->ref_count = 0;
re->tables = (tables == _pcre_default_tables)? NULL : tables;
re->nullpad = NULL;

/* The starting points of the name/number translation table and of the code are
passed around in the compile data block. The start/end pattern and initial
options are already set from the pre-compile phase, as is the name_entry_size
field. Reset the bracket count and the names_found field. Also reset the hwm
field; this time it's used for remembering forward references to subpatterns.
*/

cd->final_bracount = cd->bracount;  /* Save for checking forward references */
cd->bracount = 0;
cd->names_found = 0;
cd->name_table = (uschar *)re + re->name_table_offset;
codestart = cd->name_table + re->name_entry_size * re->name_count;
cd->start_code = codestart;
cd->hwm = cworkspace;
cd->req_varyopt = 0;
cd->had_accept = FALSE;

/* Set up a starting, non-extracting bracket, then compile the expression. On
error, errorcode will be set non-zero, so we don't need to look at the result
of the function here. */

ptr = (const uschar *)pattern + skipatstart;
code = (uschar *)codestart;
*code = OP_BRA;
(void)compile_regex(re->options, re->options & PCRE_IMS, &code, &ptr,
  &errorcode, FALSE, FALSE, 0, &firstbyte, &reqbyte, NULL, cd, NULL);
re->top_bracket = cd->bracount;
re->top_backref = cd->top_backref;
re->flags = cd->external_flags;

if (cd->had_accept) reqbyte = -1;   /* Must disable after (*ACCEPT) */

/* If not reached end of pattern on success, there's an excess bracket. */

if (errorcode == 0 && *ptr != 0) errorcode = ERR22;

/* Fill in the terminating state and check for disastrous overflow, but
if debugging, leave the test till after things are printed out. */

*code++ = OP_END;

#ifndef DEBUG
if (code - codestart > length) errorcode = ERR23;
#endif

/* Fill in any forward references that are required. */

while (errorcode == 0 && cd->hwm > cworkspace)
  {
  int offset, recno;
  const uschar *groupptr;
  cd->hwm -= LINK_SIZE;
  offset = GET(cd->hwm, 0);
  recno = GET(codestart, offset);
  groupptr = find_bracket(codestart, (re->options & PCRE_UTF8) != 0, recno);
  if (groupptr == NULL) errorcode = ERR53;
    else PUT(((uschar *)codestart), offset, groupptr - codestart);
  }

/* Give an error if there's back reference to a non-existent capturing
subpattern. */

if (errorcode == 0 && re->top_backref > re->top_bracket) errorcode = ERR15;

/* Failed to compile, or error while post-processing */

if (errorcode != 0)
  {
  (pcre_free)(re);
  PCRE_EARLY_ERROR_RETURN:
  *erroroffset = ptr - (const uschar *)pattern;
  PCRE_EARLY_ERROR_RETURN2:
  *errorptr = find_error_text(errorcode);
  if (errorcodeptr != NULL) *errorcodeptr = errorcode;
  return NULL;
  }

/* If the anchored option was not passed, set the flag if we can determine that
the pattern is anchored by virtue of ^ characters or \A or anything else (such
as starting with .* when DOTALL is set).

Otherwise, if we know what the first byte has to be, save it, because that
speeds up unanchored matches no end. If not, see if we can set the
PCRE_STARTLINE flag. This is helpful for multiline matches when all branches
start with ^. and also when all branches start with .* for non-DOTALL matches.
*/

if ((re->options & PCRE_ANCHORED) == 0)
  {
  int temp_options = re->options;   /* May get changed during these scans */
  if (is_anchored(codestart, &temp_options, 0, cd->backref_map))
    re->options |= PCRE_ANCHORED;
  else
    {
    if (firstbyte < 0)
      firstbyte = find_firstassertedchar(codestart, &temp_options, FALSE);
    if (firstbyte >= 0)   /* Remove caseless flag for non-caseable chars */
      {
      int ch = firstbyte & 255;
      re->first_byte = ((firstbyte & REQ_CASELESS) != 0 &&
         cd->fcc[ch] == ch)? ch : firstbyte;
      re->flags |= PCRE_FIRSTSET;
      }
    else if (is_startline(codestart, 0, cd->backref_map))
      re->flags |= PCRE_STARTLINE;
    }
  }

/* For an anchored pattern, we use the "required byte" only if it follows a
variable length item in the regex. Remove the caseless flag for non-caseable
bytes. */

if (reqbyte >= 0 &&
     ((re->options & PCRE_ANCHORED) == 0 || (reqbyte & REQ_VARY) != 0))
  {
  int ch = reqbyte & 255;
  re->req_byte = ((reqbyte & REQ_CASELESS) != 0 &&
    cd->fcc[ch] == ch)? (reqbyte & ~REQ_CASELESS) : reqbyte;
  re->flags |= PCRE_REQCHSET;
  }

/* Print out the compiled data if debugging is enabled. This is never the
case when building a production library. */

#ifdef DEBUG

printf("Length = %d top_bracket = %d top_backref = %d\n",
  length, re->top_bracket, re->top_backref);

printf("Options=%08x\n", re->options);

if ((re->flags & PCRE_FIRSTSET) != 0)
  {
  int ch = re->first_byte & 255;
  const char *caseless = ((re->first_byte & REQ_CASELESS) == 0)?
    "" : " (caseless)";
  if (isprint(ch)) printf("First char = %c%s\n", ch, caseless);
    else printf("First char = \\x%02x%s\n", ch, caseless);
  }

if ((re->flags & PCRE_REQCHSET) != 0)
  {
  int ch = re->req_byte & 255;
  const char *caseless = ((re->req_byte & REQ_CASELESS) == 0)?
    "" : " (caseless)";
  if (isprint(ch)) printf("Req char = %c%s\n", ch, caseless);
    else printf("Req char = \\x%02x%s\n", ch, caseless);
  }

pcre_printint(re, stdout, TRUE);

/* This check is done here in the debugging case so that the code that
was compiled can be seen. */

if (code - codestart > length)
  {
  (pcre_free)(re);
  *errorptr = find_error_text(ERR23);
  *erroroffset = ptr - (uschar *)pattern;
  if (errorcodeptr != NULL) *errorcodeptr = ERR23;
  return NULL;
  }
#endif   /* DEBUG */

return (pcre *)re;
}

/* End of pcre_compile.c */
