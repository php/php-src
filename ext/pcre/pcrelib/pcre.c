/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2002 University of Cambridge

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


/* Define DEBUG to get debugging output on stdout. */

/* #define DEBUG */

/* Use a macro for debugging printing, 'cause that eliminates the use of #ifdef
inline, and there are *still* stupid compilers about that don't like indented
pre-processor statements. I suppose it's only been 10 years... */

#ifdef DEBUG
#define DPRINTF(p) printf p
#else
#define DPRINTF(p) /*nothing*/
#endif

/* Include the internals header, which itself includes Standard C headers plus
the external pcre header. */

#include "internal.h"


/* Allow compilation as C++ source code, should anybody want to do that. */

#ifdef __cplusplus
#define class pcre_class
#endif


/* Maximum number of items on the nested bracket stacks at compile time. This
applies to the nesting of all kinds of parentheses. It does not limit
un-nested, non-capturing parentheses. This number can be made bigger if
necessary - it is used to dimension one int and one unsigned char vector at
compile time. */

#define BRASTACK_SIZE 200



/* Maximum number of ints of offset to save on the stack for recursive calls.
If the offset vector is bigger, malloc is used. This should be a multiple of 3,
because the offset vector is always a multiple of 3 long. */

#define REC_STACK_SAVE_MAX 30


/* The number of bytes in a literal character string above which we can't add
any more is different when UTF-8 characters may be encountered. */

#ifdef SUPPORT_UTF8
#define MAXLIT 250
#else
#define MAXLIT 255
#endif


/* Table of sizes for the fixed-length opcodes. It's defined in a macro so that
the definition is next to the definition of the opcodes in internal.h. */

static uschar OP_lengths[] = { OP_LENGTHS };

/* Min and max values for the common repeats; for the maxima, 0 => infinity */

static const char rep_min[] = { 0, 0, 1, 1, 0, 0 };
static const char rep_max[] = { 0, 0, 0, 0, 1, 1 };

/* Table for handling escaped characters in the range '0'-'z'. Positive returns
are simple data values; negative values are for special things like \d and so
on. Zero means further processing is needed (for things like \x), or the escape
is invalid. */

static const short int escapes[] = {
    0,      0,      0,      0,      0,      0,      0,      0,   /* 0 - 7 */
    0,      0,    ':',    ';',    '<',    '=',    '>',    '?',   /* 8 - ? */
  '@', -ESC_A, -ESC_B, -ESC_C, -ESC_D, -ESC_E,      0, -ESC_G,   /* @ - G */
    0,      0,      0,      0,      0,      0,      0,      0,   /* H - O */
    0, -ESC_Q,      0, -ESC_S,      0,      0,      0, -ESC_W,   /* P - W */
    0,      0, -ESC_Z,    '[',   '\\',    ']',    '^',    '_',   /* X - _ */
  '`',      7, -ESC_b,      0, -ESC_d,  ESC_e,  ESC_f,      0,   /* ` - g */
    0,      0,      0,      0,      0,      0,  ESC_n,      0,   /* h - o */
    0,      0,  ESC_r, -ESC_s,  ESC_t,      0,      0, -ESC_w,   /* p - w */
    0,      0, -ESC_z                                            /* x - z */
};

/* Tables of names of POSIX character classes and their lengths. The list is
terminated by a zero length entry. The first three must be alpha, upper, lower,
as this is assumed for handling case independence. */

static const char *posix_names[] = {
  "alpha", "lower", "upper",
  "alnum", "ascii", "blank", "cntrl", "digit", "graph",
  "print", "punct", "space", "word",  "xdigit" };

static const uschar posix_name_lengths[] = {
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 6, 0 };

/* Table of class bit maps for each POSIX class; up to three may be combined
to form the class. The table for [:blank:] is dynamically modified to remove
the vertical space characters. */

static const int posix_class_maps[] = {
  cbit_lower, cbit_upper, -1,             /* alpha */
  cbit_lower, -1,         -1,             /* lower */
  cbit_upper, -1,         -1,             /* upper */
  cbit_digit, cbit_lower, cbit_upper,     /* alnum */
  cbit_print, cbit_cntrl, -1,             /* ascii */
  cbit_space, -1,         -1,             /* blank - a GNU extension */
  cbit_cntrl, -1,         -1,             /* cntrl */
  cbit_digit, -1,         -1,             /* digit */
  cbit_graph, -1,         -1,             /* graph */
  cbit_print, -1,         -1,             /* print */
  cbit_punct, -1,         -1,             /* punct */
  cbit_space, -1,         -1,             /* space */
  cbit_word,  -1,         -1,             /* word - a Perl extension */
  cbit_xdigit,-1,         -1              /* xdigit */
};


/* Definition to allow mutual recursion */

static BOOL
  compile_regex(int, int, int *, uschar **, const uschar **, const char **,
    BOOL, int, int *, int *, branch_chain *, compile_data *);

/* Structure for building a chain of data that actually lives on the
stack, for holding the values of the subject pointer at the start of each
subpattern, so as to detect when an empty string has been matched by a
subpattern - to break infinite loops. */

typedef struct eptrblock {
  struct eptrblock *prev;
  const uschar *saved_eptr;
} eptrblock;

/* Flag bits for the match() function */

#define match_condassert   0x01    /* Called to check a condition assertion */
#define match_isgroup      0x02    /* Set if start of bracketed group */



/*************************************************
*               Global variables                 *
*************************************************/

/* PCRE is thread-clean and doesn't use any global variables in the normal
sense. However, it calls memory allocation and free functions via the two
indirections below, and it can optionally do callouts. These values can be
changed by the caller, but are shared between all threads. However, when
compiling for Virtual Pascal, things are done differently (see pcre.in). */

#ifndef VPCOMPAT
void *(*pcre_malloc)(size_t) = malloc;
void  (*pcre_free)(void *) = free;
int   (*pcre_callout)(pcre_callout_block *) = NULL;
#endif


/*************************************************
*    Macros and tables for character handling    *
*************************************************/

/* When UTF-8 encoding is being used, a character is no longer just a single
byte. The macros for character handling generate simple sequences when used in
byte-mode, and more complicated ones for UTF-8 characters. */

#ifndef SUPPORT_UTF8
#define GETCHARINC(c, eptr) c = *eptr++;
#define GETCHARLEN(c, eptr, len) c = *eptr;
#define BACKCHAR(eptr)

#else   /* SUPPORT_UTF8 */

/* Get the next UTF-8 character, advancing the pointer */

#define GETCHARINC(c, eptr) \
  c = *eptr++; \
  if (md->utf8 && (c & 0xc0) == 0xc0) \
    { \
    int a = utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int s = 6*a; \
    c = (c & utf8_table3[a]) << s; \
    while (a-- > 0) \
      { \
      s -= 6; \
      c |= (*eptr++ & 0x3f) << s; \
      } \
    }

/* Get the next UTF-8 character, not advancing the pointer, setting length */

#define GETCHARLEN(c, eptr, len) \
  c = *eptr; \
  len = 1; \
  if (md->utf8 && (c & 0xc0) == 0xc0) \
    { \
    int i; \
    int a = utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int s = 6*a; \
    c = (c & utf8_table3[a]) << s; \
    for (i = 1; i <= a; i++) \
      { \
      s -= 6; \
      c |= (eptr[i] & 0x3f) << s; \
      } \
    len += a; \
    }

/* If the pointer is not at the start of a character, move it back until
it is. */

#define BACKCHAR(eptr) while((*eptr & 0xc0) == 0x80) eptr--;

#endif



/*************************************************
*             Default character tables           *
*************************************************/

/* A default set of character tables is included in the PCRE binary. Its source
is built by the maketables auxiliary program, which uses the default C ctypes
functions, and put in the file chartables.c. These tables are used by PCRE
whenever the caller of pcre_compile() does not provide an alternate set of
tables. */

#include "chartables.c"



#ifdef SUPPORT_UTF8
/*************************************************
*           Tables for UTF-8 support             *
*************************************************/

/* These are the breakpoints for different numbers of bytes in a UTF-8
character. */

static int utf8_table1[] = { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};

/* These are the indicator bits and the mask for the data bits to set in the
first byte of a character, indexed by the number of additional bytes. */

static int utf8_table2[] = { 0,    0xc0, 0xe0, 0xf0, 0xf8, 0xfc};
static int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

/* Table of the number of extra characters, indexed by the first character
masked with 0x3f. The highest number for a valid UTF-8 character is in fact
0x3d. */

static uschar utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };


/*************************************************
*       Convert character value to UTF-8         *
*************************************************/

/* This function takes an integer value in the range 0 - 0x7fffffff
and encodes it as a UTF-8 character in 0 to 6 bytes.

Arguments:
  cvalue     the character value
  buffer     pointer to buffer for result - at least 6 bytes long

Returns:     number of characters placed in the buffer
*/

static int
ord2utf8(int cvalue, uschar *buffer)
{
register int i, j;
for (i = 0; i < sizeof(utf8_table1)/sizeof(int); i++)
  if (cvalue <= utf8_table1[i]) break;
buffer += i;
for (j = i; j > 0; j--)
 {
 *buffer-- = 0x80 | (cvalue & 0x3f);
 cvalue >>= 6;
 }
*buffer = utf8_table2[i] | cvalue;
return i + 1;
}
#endif



/*************************************************
*         Print compiled regex                   *
*************************************************/

/* The code for doing this is held in a separate file that is also included in
pcretest.c. It defines a function called print_internals(). */

#ifdef DEBUG
#include "printint.c"
#endif



/*************************************************
*          Return version string                 *
*************************************************/

#define STRING(a)  # a
#define XSTRING(s) STRING(s)

const char *
pcre_version(void)
{
return XSTRING(PCRE_MAJOR) "." XSTRING(PCRE_MINOR) " " XSTRING(PCRE_DATE);
}




/*************************************************
* (Obsolete) Return info about compiled pattern  *
*************************************************/

/* This is the original "info" function. It picks potentially useful data out
of the private structure, but its interface was too rigid. It remains for
backwards compatibility. The public options are passed back in an int - though
the re->options field has been expanded to a long int, all the public options
at the low end of it, and so even on 16-bit systems this will still be OK.
Therefore, I haven't changed the API for pcre_info().

Arguments:
  external_re   points to compiled code
  optptr        where to pass back the options
  first_char    where to pass back the first character,
                or -1 if multiline and all branches start ^,
                or -2 otherwise

Returns:        number of capturing subpatterns
                or negative values on error
*/

int
pcre_info(const pcre *external_re, int *optptr, int *first_char)
{
const real_pcre *re = (const real_pcre *)external_re;
if (re == NULL) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;
if (optptr != NULL) *optptr = (int)(re->options & PUBLIC_OPTIONS);
if (first_char != NULL)
  *first_char = ((re->options & PCRE_FIRSTSET) != 0)? re->first_char :
     ((re->options & PCRE_STARTLINE) != 0)? -1 : -2;
return re->top_bracket;
}



/*************************************************
*        Return info about compiled pattern      *
*************************************************/

/* This is a newer "info" function which has an extensible interface so
that additional items can be added compatibly.

Arguments:
  external_re      points to compiled code
  external_study   points to study data, or NULL
  what             what information is required
  where            where to put the information

Returns:           0 if data returned, negative on error
*/

int
pcre_fullinfo(const pcre *external_re, const pcre_extra *study_data, int what,
  void *where)
{
const real_pcre *re = (const real_pcre *)external_re;
const real_pcre_extra *study = (const real_pcre_extra *)study_data;

if (re == NULL || where == NULL) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;

switch (what)
  {
  case PCRE_INFO_OPTIONS:
  *((unsigned long int *)where) = re->options & PUBLIC_OPTIONS;
  break;

  case PCRE_INFO_SIZE:
  *((size_t *)where) = re->size;
  break;

  case PCRE_INFO_CAPTURECOUNT:
  *((int *)where) = re->top_bracket;
  break;

  case PCRE_INFO_BACKREFMAX:
  *((int *)where) = re->top_backref;
  break;

  case PCRE_INFO_FIRSTCHAR:
  *((int *)where) =
    ((re->options & PCRE_FIRSTSET) != 0)? re->first_char :
    ((re->options & PCRE_STARTLINE) != 0)? -1 : -2;
  break;

  case PCRE_INFO_FIRSTTABLE:
  *((const uschar **)where) =
    (study != NULL && (study->options & PCRE_STUDY_MAPPED) != 0)?
      study->start_bits : NULL;
  break;

  case PCRE_INFO_LASTLITERAL:
  *((int *)where) =
    ((re->options & PCRE_REQCHSET) != 0)? re->req_char : -1;
  break;

  case PCRE_INFO_NAMEENTRYSIZE:
  *((int *)where) = re->name_entry_size;
  break;

  case PCRE_INFO_NAMECOUNT:
  *((int *)where) = re->name_count;
  break;

  case PCRE_INFO_NAMETABLE:
  *((const uschar **)where) = (const uschar *)re + sizeof(real_pcre);
  break;

  default: return PCRE_ERROR_BADOPTION;
  }

return 0;
}



#ifdef DEBUG
/*************************************************
*        Debugging function to print chars       *
*************************************************/

/* Print a sequence of chars in printable format, stopping at the end of the
subject if the requested.

Arguments:
  p           points to characters
  length      number to print
  is_subject  TRUE if printing from within md->start_subject
  md          pointer to matching data block, if is_subject is TRUE

Returns:     nothing
*/

static void
pchars(const uschar *p, int length, BOOL is_subject, match_data *md)
{
int c;
if (is_subject && length > md->end_subject - p) length = md->end_subject - p;
while (length-- > 0)
  if (isprint(c = *(p++))) printf("%c", c); else printf("\\x%02x", c);
}
#endif




/*************************************************
*            Handle escapes                      *
*************************************************/

/* This function is called when a \ has been encountered. It either returns a
positive value for a simple escape such as \n, or a negative value which
encodes one of the more complicated things such as \d. When UTF-8 is enabled,
a positive value greater than 255 may be returned. On entry, ptr is pointing at
the \. On exit, it is on the final character of the escape sequence.

Arguments:
  ptrptr     points to the pattern position pointer
  errorptr   points to the pointer to the error message
  bracount   number of previous extracting brackets
  options    the options bits
  isclass    TRUE if inside a character class
  cd         pointer to char tables block

Returns:     zero or positive => a data character
             negative => a special escape sequence
             on error, errorptr is set
*/

static int
check_escape(const uschar **ptrptr, const char **errorptr, int bracount,
  int options, BOOL isclass, compile_data *cd)
{
const uschar *ptr = *ptrptr;
int c, i;

/* If backslash is at the end of the pattern, it's an error. */

c = *(++ptr);
if (c == 0) *errorptr = ERR1;

/* Digits or letters may have special meaning; all others are literals. */

else if (c < '0' || c > 'z') {}

/* Do an initial lookup in a table. A non-zero result is something that can be
returned immediately. Otherwise further processing may be required. */

else if ((i = escapes[c - '0']) != 0) c = i;

/* Escapes that need further processing, or are illegal. */

else
  {
  const uschar *oldptr;
  switch (c)
    {
    /* A number of Perl escapes are not handled by PCRE. We give an explicit
    error. */

    case 'l':
    case 'L':
    case 'N':
    case 'p':
    case 'P':
    case 'u':
    case 'U':
    case 'X':
    *errorptr = ERR37;
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
      while ((cd->ctypes[ptr[1]] & ctype_digit) != 0)
        c = c * 10 + *(++ptr) - '0';
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
    larger first octal digit. */

    case '0':
    c -= '0';
    while(i++ < 2 && (cd->ctypes[ptr[1]] & ctype_digit) != 0 &&
      ptr[1] != '8' && ptr[1] != '9')
        c = c * 8 + *(++ptr) - '0';
    c &= 255;     /* Take least significant 8 bits */
    break;

    /* \x is complicated when UTF-8 is enabled. \x{ddd} is a character number
    which can be greater than 0xff, but only if the ddd are hex digits. */

    case 'x':
#ifdef SUPPORT_UTF8
    if (ptr[1] == '{' && (options & PCRE_UTF8) != 0)
      {
      const uschar *pt = ptr + 2;
      register int count = 0;
      c = 0;
      while ((cd->ctypes[*pt] & ctype_xdigit) != 0)
        {
        count++;
        c = c * 16 + cd->lcc[*pt] -
          (((cd->ctypes[*pt] & ctype_digit) != 0)? '0' : 'W');
        pt++;
        }
      if (*pt == '}')
        {
        if (c < 0 || count > 8) *errorptr = ERR34;
        ptr = pt;
        break;
        }
      /* If the sequence of hex digits does not end with '}', then we don't
      recognize this construct; fall through to the normal \x handling. */
      }
#endif

    /* Read just a single hex char */

    c = 0;
    while (i++ < 2 && (cd->ctypes[ptr[1]] & ctype_xdigit) != 0)
      {
      ptr++;
      c = c * 16 + cd->lcc[*ptr] -
        (((cd->ctypes[*ptr] & ctype_digit) != 0)? '0' : 'W');
      }
    break;

    /* Other special escapes not starting with a digit are straightforward */

    case 'c':
    c = *(++ptr);
    if (c == 0)
      {
      *errorptr = ERR2;
      return 0;
      }

    /* A letter is upper-cased; then the 0x40 bit is flipped */

    if (c >= 'a' && c <= 'z') c = cd->fcc[c];
    c ^= 0x40;
    break;

    /* PCRE_EXTRA enables extensions to Perl in the matter of escapes. Any
    other alphameric following \ is an error if PCRE_EXTRA was set; otherwise,
    for Perl compatibility, it is a literal. This code looks a bit odd, but
    there used to be some cases other than the default, and there may be again
    in future, so I haven't "optimized" it. */

    default:
    if ((options & PCRE_EXTRA) != 0) switch(c)
      {
      default:
      *errorptr = ERR3;
      break;
      }
    break;
    }
  }

*ptrptr = ptr;
return c;
}



/*************************************************
*            Check for counted repeat            *
*************************************************/

/* This function is called when a '{' is encountered in a place where it might
start a quantifier. It looks ahead to see if it really is a quantifier or not.
It is only a quantifier if it is one of the forms {ddd} {ddd,} or {ddd,ddd}
where the ddds are digits.

Arguments:
  p         pointer to the first char after '{'
  cd        pointer to char tables block

Returns:    TRUE or FALSE
*/

static BOOL
is_counted_repeat(const uschar *p, compile_data *cd)
{
if ((cd->ctypes[*p++] & ctype_digit) == 0) return FALSE;
while ((cd->ctypes[*p] & ctype_digit) != 0) p++;
if (*p == '}') return TRUE;

if (*p++ != ',') return FALSE;
if (*p == '}') return TRUE;

if ((cd->ctypes[*p++] & ctype_digit) == 0) return FALSE;
while ((cd->ctypes[*p] & ctype_digit) != 0) p++;
return (*p == '}');
}



/*************************************************
*         Read repeat counts                     *
*************************************************/

/* Read an item of the form {n,m} and return the values. This is called only
after is_counted_repeat() has confirmed that a repeat-count quantifier exists,
so the syntax is guaranteed to be correct, but we need to check the values.

Arguments:
  p          pointer to first char after '{'
  minp       pointer to int for min
  maxp       pointer to int for max
             returned as -1 if no max
  errorptr   points to pointer to error message
  cd         pointer to character tables clock

Returns:     pointer to '}' on success;
             current ptr on error, with errorptr set
*/

static const uschar *
read_repeat_counts(const uschar *p, int *minp, int *maxp,
  const char **errorptr, compile_data *cd)
{
int min = 0;
int max = -1;

while ((cd->ctypes[*p] & ctype_digit) != 0) min = min * 10 + *p++ - '0';

if (*p == '}') max = min; else
  {
  if (*(++p) != '}')
    {
    max = 0;
    while((cd->ctypes[*p] & ctype_digit) != 0) max = max * 10 + *p++ - '0';
    if (max < min)
      {
      *errorptr = ERR4;
      return p;
      }
    }
  }

/* Do paranoid checks, then fill in the required variables, and pass back the
pointer to the terminating '}'. */

if (min > 65535 || max > 65535)
  *errorptr = ERR5;
else
  {
  *minp = min;
  *maxp = max;
  }
return p;
}



/*************************************************
*      Find first significant op code            *
*************************************************/

/* This is called by several functions that scan a compiled expression looking
for a fixed first character, or an anchoring op code etc. It skips over things
that do not influence this. For some calls, a change of option is important.

Arguments:
  code       pointer to the start of the group
  options    pointer to external options
  optbit     the option bit whose changing is significant, or
               zero if none are

Returns:     pointer to the first significant opcode
*/

static const uschar*
first_significant_code(const uschar *code, int *options, int optbit)
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
    do code += GET(code, 1); while (*code == OP_ALT);
    /* Fall through */

    case OP_CALLOUT:
    case OP_CREF:
    case OP_BRANUMBER:
    case OP_WORD_BOUNDARY:
    case OP_NOT_WORD_BOUNDARY:
    code += OP_lengths[*code];
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
  if (op >= OP_BRA) op = OP_BRA;

  switch (op)
    {
    case OP_BRA:
    case OP_ONCE:
    case OP_COND:
    d = find_fixedlength(cc, options);
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
    case OP_BRANUMBER:
    case OP_CREF:
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
    cc += OP_lengths[*cc];
    break;

    /* Handle char strings. In UTF-8 mode we must count characters, not bytes.
    This requires a scan of the string, unfortunately. We assume valid UTF-8
    strings, so all we do is reduce the length by one for every byte whose bits
    are 10xxxxxx. */

    case OP_CHARS:
    branchlength += *(++cc);
#ifdef SUPPORT_UTF8
    for (d = 1; d <= *cc; d++)
      if ((cc[d] & 0xc0) == 0x80) branchlength--;
#endif
    cc += *cc + 1;
    break;

    /* Handle exact repetitions */

    case OP_EXACT:
    case OP_TYPEEXACT:
    branchlength += GET2(cc,1);
    cc += 4;
    break;

    /* Handle single-char matchers */

    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    branchlength++;
    cc++;
    break;

    /* The single-byte matcher isn't allowed */

    case OP_ANYBYTE:
    return -2;

    /* Check a class for variable quantification */

    case OP_CLASS:
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
  number      the required bracket number

Returns:      pointer to the opcode for the bracket, or NULL if not found
*/

static const uschar *
find_bracket(const uschar *code, int number)
{
for (;;)
  {
  register int c = *code;
  if (c == OP_END) return NULL;
  else if (c == OP_CHARS) code += code[1] + OP_lengths[c];
  else if (c > OP_BRA)
    {
    int n = c - OP_BRA;
    if (n > EXTRACT_BASIC_MAX) n = GET2(code, 2+LINK_SIZE);
    if (n == number) return (uschar *)code;
    code += OP_lengths[OP_BRA];
    }
  else code += OP_lengths[c];
  }
}



/*************************************************
*    Scan compiled branch for non-emptiness      *
*************************************************/

/* This function scans through a branch of a compiled pattern to see whether it
can match the empty string or not. It is called only from could_be_empty()
below. Note that first_significant_code() skips over assertions. If we hit an
unclosed bracket, we return "empty" - this means we've struck an inner bracket
whose current branch will already have been scanned.

Arguments:
  code        points to start of search
  endcode     points to where to stop

Returns:      TRUE if what is matched could be empty
*/

static BOOL
could_be_empty_branch(const uschar *code, const uschar *endcode)
{
register int c;
for (code = first_significant_code(code + 1 + LINK_SIZE, NULL, 0);
     code < endcode;
     code = first_significant_code(code + OP_lengths[c], NULL, 0))
  {
  c = *code;

  if (c >= OP_BRA)
    {
    BOOL empty_branch;
    if (GET(code, 1) == 0) return TRUE;    /* Hit unclosed bracket */

    /* Scan a closed bracket */

    empty_branch = FALSE;
    do
      {
      if (!empty_branch && could_be_empty_branch(code, endcode))
        empty_branch = TRUE;
      code += GET(code, 1);
      }
    while (*code == OP_ALT);
    if (!empty_branch) return FALSE;   /* All branches are non-empty */
    code += 1 + LINK_SIZE;
    c = *code;
    }

  /* Check for any quantifier after a class */

  else if (c == OP_CLASS)
    {
    const uschar *ccode = code + 33;

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
    }

  /* Test for an opcode that must match a character. */

  else switch (c)
    {
    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ANYBYTE:
    case OP_CHARS:
    case OP_NOT:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_EXACT:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTEXACT:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEEXACT:
    return FALSE;

    /* End of branch */

    case OP_KET:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_ALT:
    return TRUE;
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

Returns:      TRUE if what is matched could be empty
*/

static BOOL
could_be_empty(const uschar *code, const uschar *endcode, branch_chain *bcptr)
{
while (bcptr != NULL && bcptr->current >= code)
  {
  if (!could_be_empty_branch(bcptr->current, endcode)) return FALSE;
  bcptr = bcptr->outer;
  }
return TRUE;
}



/*************************************************
*           Check for POSIX class syntax         *
*************************************************/

/* This function is called when the sequence "[:" or "[." or "[=" is
encountered in a character class. It checks whether this is followed by an
optional ^ and then a sequence of letters, terminated by a matching ":]" or
".]" or "=]".

Argument:
  ptr      pointer to the initial [
  endptr   where to return the end pointer
  cd       pointer to compile data

Returns:   TRUE or FALSE
*/

static BOOL
check_posix_syntax(const uschar *ptr, const uschar **endptr, compile_data *cd)
{
int terminator;          /* Don't combine these lines; the Solaris cc */
terminator = *(++ptr);   /* compiler warns about "non-constant" initializer. */
if (*(++ptr) == '^') ptr++;
while ((cd->ctypes[*ptr] & ctype_letter) != 0) ptr++;
if (*ptr == terminator && ptr[1] == ']')
  {
  *endptr = ptr;
  return TRUE;
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
register int yield = 0;
while (posix_name_lengths[yield] != 0)
  {
  if (len == posix_name_lengths[yield] &&
    strncmp((const char *)ptr, posix_names[yield], len) == 0) return yield;
  yield++;
  }
return -1;
}




/*************************************************
*           Compile one branch                   *
*************************************************/

/* Scan the pattern, compiling it into the code vector. If the options are
changed during the branch, the pointer is used to change the external options
bits.

Arguments:
  optionsptr     pointer to the option bits
  brackets       points to number of extracting brackets used
  code           points to the pointer to the current code point
  ptrptr         points to the current pattern pointer
  errorptr       points to pointer to error message
  firstcharptr   set to initial literal character, or < 0 (REQ_UNSET, REQ_NONE)
  reqcharptr     set to the last literal character required, else < 0
  bcptr          points to current branch chain
  cd             contains pointers to tables etc.

Returns:         TRUE on success
                 FALSE, with *errorptr set on error
*/

static BOOL
compile_branch(int *optionsptr, int *brackets, uschar **codeptr,
  const uschar **ptrptr, const char **errorptr, int *firstcharptr,
  int *reqcharptr, branch_chain *bcptr, compile_data *cd)
{
int repeat_type, op_type;
int repeat_min = 0, repeat_max = 0;      /* To please picky compilers */
int bravalue = 0;
int length;
int greedy_default, greedy_non_default;
int firstchar, reqchar;
int zeroreqchar, zerofirstchar;
int req_caseopt;
int condcount = 0;
int options = *optionsptr;
register int c;
register uschar *code = *codeptr;
uschar *tempcode;
BOOL inescq = FALSE;
BOOL groupsetfirstchar = FALSE;
const uschar *ptr = *ptrptr;
const uschar *tempptr;
uschar *previous = NULL;
uschar class[32];

/* Set up the default and non-default settings for greediness */

greedy_default = ((options & PCRE_UNGREEDY) != 0);
greedy_non_default = greedy_default ^ 1;

/* Initialize no first char, no required char. REQ_UNSET means "no char
matching encountered yet". It gets changed to REQ_NONE if we hit something that
matches a non-fixed char first char; reqchar just remains unset if we never
find one.

When we hit a repeat whose minimum is zero, we may have to adjust these values
to take the zero repeat into account. This is implemented by setting them to
zerofirstchar and zeroreqchar when such a repeat is encountered. The individual
item types that can be repeated set these backoff variables appropriately. */

firstchar = reqchar = zerofirstchar = zeroreqchar = REQ_UNSET;

/* The variable req_caseopt contains either the REQ_CASELESS value or zero,
according to the current setting of the caseless flag. REQ_CASELESS is a bit
value > 255. It is added into the firstchar or reqchar variables to record the
case status of the value. */

req_caseopt = ((options & PCRE_CASELESS) != 0)? REQ_CASELESS : 0;

/* Switch on next character until the end of the branch */

for (;; ptr++)
  {
  BOOL negate_class;
  BOOL possessive_quantifier;
  int class_charcount;
  int class_lastchar;
  int newoptions;
  int recno;
  int skipbytes;
  int subreqchar;
  int subfirstchar;

  c = *ptr;
  if (inescq && c != 0) goto NORMAL_CHAR;

  if ((options & PCRE_EXTENDED) != 0)
    {
    if ((cd->ctypes[c] & ctype_space) != 0) continue;
    if (c == '#')
      {
      /* The space before the ; is to avoid a warning on a silly compiler
      on the Macintosh. */
      while ((c = *(++ptr)) != 0 && c != NEWLINE) ;
      if (c != 0) continue;   /* Else fall through to handle end of string */
      }
    }

  switch(c)
    {
    /* The branch terminates at end of string, |, or ). */

    case 0:
    case '|':
    case ')':
    *firstcharptr = firstchar;
    *reqcharptr = reqchar;
    *codeptr = code;
    *ptrptr = ptr;
    return TRUE;

    /* Handle single-character metacharacters. In multiline mode, ^ disables
    the setting of any following char as a first character. */

    case '^':
    if ((options & PCRE_MULTILINE) != 0)
      {
      if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
      }
    previous = NULL;
    *code++ = OP_CIRC;
    break;

    case '$':
    previous = NULL;
    *code++ = OP_DOLL;
    break;

    /* There can never be a first char if '.' is first, whatever happens about
    repeats. The value of reqchar doesn't change either. */

    case '.':
    if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
    zerofirstchar = firstchar;
    zeroreqchar = reqchar;
    previous = code;
    *code++ = OP_ANY;
    break;

    /* Character classes. These always build a 32-byte bitmap of the permitted
    characters, except in the special case where there is only one character.
    For negated classes, we build the map as usual, then invert it at the end.
    */

    case '[':
    previous = code;
    *code++ = OP_CLASS;

    /* PCRE supports POSIX class stuff inside a class. Perl gives an error if
    they are encountered at the top level, so we'll do that too. */

    if ((ptr[1] == ':' || ptr[1] == '.' || ptr[1] == '=') &&
        check_posix_syntax(ptr, &tempptr, cd))
      {
      *errorptr = (ptr[1] == ':')? ERR13 : ERR31;
      goto FAILED;
      }

    /* If the first character is '^', set the negation flag and skip it. */

    if ((c = *(++ptr)) == '^')
      {
      negate_class = TRUE;
      c = *(++ptr);
      }
    else negate_class = FALSE;

    /* Keep a count of chars so that we can optimize the case of just a single
    character. */

    class_charcount = 0;
    class_lastchar = -1;

    /* Initialize the 32-char bit map to all zeros. We have to build the
    map in a temporary bit of store, in case the class contains only 1
    character, because in that case the compiled code doesn't use the
    bit map. */

    memset(class, 0, 32 * sizeof(uschar));

    /* Process characters until ] is reached. By writing this as a "do" it
    means that an initial ] is taken as a data character. The first pass
    checked the overall syntax. */

    do
      {
      /* Handle POSIX class names. Perl allows a negation extension of the
      form [:^name:]. A square bracket that doesn't match the syntax is
      treated as a literal. We also recognize the POSIX constructions
      [.ch.] and [=ch=] ("collating elements") and fault them, as Perl
      5.6 and 5.8 do. */

      if (c == '[' &&
          (ptr[1] == ':' || ptr[1] == '.' || ptr[1] == '=') &&
          check_posix_syntax(ptr, &tempptr, cd))
        {
        BOOL local_negate = FALSE;
        int posix_class, i;
        register const uschar *cbits = cd->cbits;

        if (ptr[1] != ':')
          {
          *errorptr = ERR31;
          goto FAILED;
          }

        ptr += 2;
        if (*ptr == '^')
          {
          local_negate = TRUE;
          ptr++;
          }

        posix_class = check_posix_name(ptr, tempptr - ptr);
        if (posix_class < 0)
          {
          *errorptr = ERR30;
          goto FAILED;
          }

        /* If matching is caseless, upper and lower are converted to
        alpha. This relies on the fact that the class table starts with
        alpha, lower, upper as the first 3 entries. */

        if ((options & PCRE_CASELESS) != 0 && posix_class <= 2)
          posix_class = 0;

        /* Or into the map we are building up to 3 of the static class
        tables, or their negations. The [:blank:] class sets up the same
        chars as the [:space:] class (all white space). We remove the vertical
        white space chars afterwards. */

        posix_class *= 3;
        for (i = 0; i < 3; i++)
          {
          BOOL isblank = strncmp(ptr, "blank", 5) == 0;
          int taboffset = posix_class_maps[posix_class + i];
          if (taboffset < 0) break;
          if (local_negate)
            {
            for (c = 0; c < 32; c++) class[c] |= ~cbits[c+taboffset];
            if (isblank) class[1] |= 0x3c;
            }
          else
            {
            for (c = 0; c < 32; c++) class[c] |= cbits[c+taboffset];
            if (isblank) class[1] &= ~0x3c;
            }
          }

        ptr = tempptr + 1;
        class_charcount = 10;  /* Set > 1; assumes more than 1 per class */
        continue;
        }

      /* Backslash may introduce a single character, or it may introduce one
      of the specials, which just set a flag. Escaped items are checked for
      validity in the pre-compiling pass. The sequence \b is a special case.
      Inside a class (and only there) it is treated as backspace. Elsewhere
      it marks a word boundary. Other escapes have preset maps ready to
      or into the one we are building. We assume they have more than one
      character in them, so set class_count bigger than one. */

      if (c == '\\')
        {
        c = check_escape(&ptr, errorptr, *brackets, options, TRUE, cd);
        if (-c == ESC_b) c = '\b';
        else if (c < 0)
          {
          register const uschar *cbits = cd->cbits;
          class_charcount = 10;     /* Greater than 1 is what matters */
          switch (-c)
            {
            case ESC_d:
            for (c = 0; c < 32; c++) class[c] |= cbits[c+cbit_digit];
            continue;

            case ESC_D:
            for (c = 0; c < 32; c++) class[c] |= ~cbits[c+cbit_digit];
            continue;

            case ESC_w:
            for (c = 0; c < 32; c++) class[c] |= cbits[c+cbit_word];
            continue;

            case ESC_W:
            for (c = 0; c < 32; c++) class[c] |= ~cbits[c+cbit_word];
            continue;

            case ESC_s:
            for (c = 0; c < 32; c++) class[c] |= cbits[c+cbit_space];
            class[1] &= ~0x08;   /* Perl 5.004 onwards omits VT from \s */
            continue;

            case ESC_S:
            for (c = 0; c < 32; c++) class[c] |= ~cbits[c+cbit_space];
            class[1] |= 0x08;    /* Perl 5.004 onwards omits VT from \s */
            continue;

            default:
            *errorptr = ERR7;
            goto FAILED;
            }
          }

        /* Fall through if single character, but don't at present allow
        chars > 255 in UTF-8 mode. */

#ifdef SUPPORT_UTF8
        if (c > 255)
          {
          *errorptr = ERR33;
          goto FAILED;
          }
#endif
        }

      /* A single character may be followed by '-' to form a range. However,
      Perl does not permit ']' to be the end of the range. A '-' character
      here is treated as a literal. */

      if (ptr[1] == '-' && ptr[2] != ']')
        {
        int d;
        ptr += 2;
        d = *ptr;

        /* The second part of a range can be a single-character escape, but
        not any of the other escapes. Perl 5.6 treats a hyphen as a literal
        in such circumstances. */

        if (d == '\\')
          {
          const uschar *oldptr = ptr;
          d = check_escape(&ptr, errorptr, *brackets, options, TRUE, cd);

#ifdef SUPPORT_UTF8
          if (d > 255)
            {
            *errorptr = ERR33;
            goto FAILED;
            }
#endif
          /* \b is backslash; any other special means the '-' was literal */

          if (d < 0)
            {
            if (d == -ESC_b) d = '\b'; else
              {
              ptr = oldptr - 2;
              goto SINGLE_CHARACTER;  /* A few lines below */
              }
            }
          }

        if (d < c)
          {
          *errorptr = ERR8;
          goto FAILED;
          }

        for (; c <= d; c++)
          {
          class[c/8] |= (1 << (c&7));
          if ((options & PCRE_CASELESS) != 0)
            {
            int uc = cd->fcc[c];           /* flip case */
            class[uc/8] |= (1 << (uc&7));
            }
          class_charcount++;                /* in case a one-char range */
          class_lastchar = c;
          }
        continue;   /* Go get the next char in the class */
        }

      /* Handle a lone single character - we can get here for a normal
      non-escape char, or after \ that introduces a single character. */

      SINGLE_CHARACTER:

      class [c/8] |= (1 << (c&7));
      if ((options & PCRE_CASELESS) != 0)
        {
        c = cd->fcc[c];   /* flip case */
        class[c/8] |= (1 << (c&7));
        }
      class_charcount++;
      class_lastchar = c;
      }

    /* Loop until ']' reached; the check for end of string happens inside the
    loop. This "while" is the end of the "do" above. */

    while ((c = *(++ptr)) != ']');

    /* If class_charcount is 1 and class_lastchar is not negative, we saw
    precisely one character. This doesn't need the whole 32-byte bit map. We
    turn it into a 1-character OP_CHARS if it's positive, or OP_NOT if it's
    negative. In the positive case, it can cause firstchar to be set.
    Otherwise, there can be no first char if this item is first, whatever
    repeat count may follow. In the case of reqchar, save the previous value
    for reinstating. */

    if (class_charcount == 1 && class_lastchar >= 0)
      {
      zeroreqchar = reqchar;
      if (negate_class)
        {
        if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
        zerofirstchar = firstchar;
        code[-1] = OP_NOT;
        }
      else
        {
        if (firstchar == REQ_UNSET)
          {
          zerofirstchar = REQ_NONE;
          firstchar = class_lastchar | req_caseopt;
          }
        else
          {
          zerofirstchar = firstchar;
          reqchar = class_lastchar | req_caseopt;
          }
        code[-1] = OP_CHARS;
        *code++ = 1;
        }
      *code++ = class_lastchar;
      }

    /* Otherwise, negate the 32-byte map if necessary, and copy it into
    the code vector. If this is the first thing in the branch, there can be
    no first char setting, whatever the repeat count. Any reqchar setting
    must remain unchanged after any kind of repeat. */

    else
      {
      if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
      zerofirstchar = firstchar;
      zeroreqchar = reqchar;
      if (negate_class)
        for (c = 0; c < 32; c++) code[c] = ~class[c];
      else
        memcpy(code, class, 32);
      code += 32;
      }
    break;

    /* Various kinds of repeat */

    case '{':
    if (!is_counted_repeat(ptr+1, cd)) goto NORMAL_CHAR;
    ptr = read_repeat_counts(ptr+1, &repeat_min, &repeat_max, errorptr, cd);
    if (*errorptr != NULL) goto FAILED;
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
      *errorptr = ERR9;
      goto FAILED;
      }

    if (repeat_min == 0)
      {
      firstchar = zerofirstchar;   /* Adjust for zero repeat */
      reqchar = zeroreqchar;       /* Ditto */
      }

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

    /* If previous was a recursion, we need to wrap it inside brackets so that
    it can be replicated if necessary. */

    if (*previous == OP_RECURSE)
      {
      memmove(previous + 1 + LINK_SIZE, previous, 1 + LINK_SIZE);
      code += 1 + LINK_SIZE;
      *previous = OP_BRA;
      PUT(previous, 1, code - previous);
      *code = OP_KET;
      PUT(code, 1, code - previous);
      code += 1 + LINK_SIZE;
      }

    /* If previous was a string of characters, chop off the last one and use it
    as the subject of the repeat. If there was only one character, we can
    abolish the previous item altogether. If a one-char item has a minumum of
    more than one, ensure that it is set in reqchar - it might not be if a
    sequence such as x{3} is the first thing in a branch because the x will
    have gone into firstchar instead.  */

    if (*previous == OP_CHARS)
      {
      int len = previous[1];
      if (len == 1)
        {
        c = previous[2];
        code = previous;
        if (repeat_min > 1) reqchar = c | req_caseopt;
        }
      else
        {
        c = previous[len+1];
        previous[1]--;
        code--;
        tempcode = code;           /* Adjust position to be moved for '+' */
        }

      goto OUTPUT_SINGLE_REPEAT;   /* Code shared with single character types */
      }

    /* If previous was a single negated character ([^a] or similar), we use
    one of the special opcodes, replacing it. The code is shared with single-
    character repeats by setting opt_type to add a suitable offset into
    repeat_type. */

    else if (*previous == OP_NOT)
      {
      op_type = OP_NOTSTAR - OP_STAR;  /* Use "not" opcodes */
      c = previous[1];
      code = previous;
      goto OUTPUT_SINGLE_REPEAT;
      }

    /* If previous was a character type match (\d or similar), abolish it and
    create a suitable repeat item. The code is shared with single-character
    repeats by setting op_type to add a suitable offset into repeat_type. */

    else if (*previous < OP_EODN)
      {
      op_type = OP_TYPESTAR - OP_STAR;  /* Use type opcodes */
      c = *previous;
      code = previous;

      OUTPUT_SINGLE_REPEAT:

      /* If the maximum is zero then the minimum must also be zero; Perl allows
      this case, so we do too - by simply omitting the item altogether. */

      if (repeat_max == 0) goto END_REPEAT;

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

      /* The case {1,} is handled as the special case + */

      else if (repeat_min == 1 && repeat_max == -1)
        *code++ = OP_PLUS + repeat_type;

      /* The case {n,n} is just an EXACT, while the general case {n,m} is
      handled as an EXACT followed by an UPTO. An EXACT of 1 is optimized. */

      else
        {
        if (repeat_min != 1)
          {
          *code++ = OP_EXACT + op_type;  /* NB EXACT doesn't have repeat_type */
          PUT2INC(code, 0, repeat_min);
          }

        /* If the mininum is 1 and the previous item was a character string,
        we either have to put back the item that got cancelled if the string
        length was 1, or add the character back onto the end of a longer
        string. For a character type nothing need be done; it will just get
        put back naturally. Note that the final character is always going to
        get added below. */

        else if (*previous == OP_CHARS)
          {
          if (code == previous) code += 2; else previous[1]++;
          }

        /*  For a single negated character we also have to put back the
        item that got cancelled. */

        else if (*previous == OP_NOT) code++;

        /* If the maximum is unlimited, insert an OP_STAR. */

        if (repeat_max < 0)
          {
          *code++ = c;
          *code++ = OP_STAR + repeat_type;
          }

        /* Else insert an UPTO if the max is greater than the min. */

        else if (repeat_max != repeat_min)
          {
          *code++ = c;
          repeat_max -= repeat_min;
          *code++ = OP_UPTO + repeat_type;
          PUT2INC(code, 0, repeat_max);
          }
        }

      /* The character or character type itself comes last in all cases. */

      *code++ = c;
      }

    /* If previous was a character class or a back reference, we put the repeat
    stuff after it, but just skip the item if the repeat was {0,0}. */

    else if (*previous == OP_CLASS || *previous == OP_REF)
      {
      if (repeat_max == 0)
        {
        code = previous;
        goto END_REPEAT;
        }
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

    else if (*previous >= OP_BRA || *previous == OP_ONCE ||
             *previous == OP_COND)
      {
      register int i;
      int ketoffset = 0;
      int len = code - previous;
      uschar *bralink = NULL;

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
        /* If the maximum is also zero, we just omit the group from the output
        altogether. */

        if (repeat_max == 0)
          {
          code = previous;
          goto END_REPEAT;
          }

        /* If the maximum is 1 or unlimited, we just have to stick in the
        BRAZERO and do no more at this point. */

        if (repeat_max <= 1)
          {
          memmove(previous+1, previous, len);
          code++;
          *previous++ = OP_BRAZERO + repeat_type;
          }

        /* If the maximum is greater than 1 and limited, we have to replicate
        in a nested fashion, sticking OP_BRAZERO before each set of brackets.
        The first one has to be handled carefully because it's the original
        copy, which has to be moved up. The remainder can be handled by code
        that is common with the non-zero minimum case below. We just have to
        adjust the value or repeat_max, since one less copy is required. */

        else
          {
          int offset;
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
      set a required char, copy the latter from the former. */

      else
        {
        if (repeat_min > 1)
          {
          if (groupsetfirstchar && reqchar < 0) reqchar = firstchar;
          for (i = 1; i < repeat_min; i++)
            {
            memcpy(code, previous, len);
            code += len;
            }
          }
        if (repeat_max > 0) repeat_max -= repeat_min;
        }

      /* This code is common to both the zero and non-zero minimum cases. If
      the maximum is limited, it replicates the group in a nested fashion,
      remembering the bracket starts on a stack. In the case of a zero minimum,
      the first one was set up above. In all cases the repeat_max now specifies
      the number of additional copies needed. */

      if (repeat_max >= 0)
        {
        for (i = repeat_max - 1; i >= 0; i--)
          {
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
      correct offset was computed above. */

      else code[-ketoffset] = OP_KETRMAX + repeat_type;
      }

    /* Else there's some kind of shambles */

    else
      {
      *errorptr = ERR11;
      goto FAILED;
      }

    /* If the character following a repeat is '+', we wrap the entire repeated
    item inside OP_ONCE brackets. This is just syntactic sugar, taken from
    Sun's Java package. The repeated item starts at tempcode, not at previous,
    which might be the first part of a string whose (former) last char we
    repeated. However, we don't support '+' after a greediness '?'. */

    if (possessive_quantifier)
      {
      int len = code - tempcode;
      memmove(tempcode + 1+LINK_SIZE, tempcode, len);
      code += 1 + LINK_SIZE;
      len += 1 + LINK_SIZE;
      tempcode[0] = OP_ONCE;
      *code++ = OP_KET;
      PUTINC(code, 0, len);
      PUT(tempcode, 1, len);
      }

    /* In all case we no longer have a previous item. */

    END_REPEAT:
    previous = NULL;
    break;


    /* Start of nested bracket sub-expression, or comment or lookahead or
    lookbehind or option setting or condition. First deal with special things
    that can come after a bracket; all are introduced by ?, and the appearance
    of any of them means that this is not a referencing group. They were
    checked for validity in the first pass over the string, so we don't have to
    check for syntax errors here.  */

    case '(':
    newoptions = options;
    skipbytes = 0;

    if (*(++ptr) == '?')
      {
      int set, unset;
      int *optset;

      switch (*(++ptr))
        {
        case '#':                 /* Comment; skip to ket */
        ptr++;
        while (*ptr != ')') ptr++;
        continue;

        case ':':                 /* Non-extracting bracket */
        bravalue = OP_BRA;
        ptr++;
        break;

        case '(':
        bravalue = OP_COND;       /* Conditional group */

        /* Condition to test for recursion */

        if (ptr[1] == 'R')
          {
          code[1+LINK_SIZE] = OP_CREF;
          PUT2(code, 2+LINK_SIZE, CREF_RECURSE);
          skipbytes += 1+LINK_SIZE;
          ptr += 3;
          }

        /* Condition to test for a numbered subpattern match */

        else if ((cd->ctypes[ptr[1]] & ctype_digit) != 0)
          {
          int condref = *(++ptr) - '0';
          while (*(++ptr) != ')') condref = condref*10 + *ptr - '0';
          if (condref == 0)
            {
            *errorptr = ERR35;
            goto FAILED;
            }
          ptr++;
          code[1+LINK_SIZE] = OP_CREF;
          PUT2(code, 2+LINK_SIZE, condref);
          skipbytes = 3;
          }
        /* For conditions that are assertions, we just fall through, having
        set bravalue above. */
        break;

        case '=':                 /* Positive lookahead */
        bravalue = OP_ASSERT;
        ptr++;
        break;

        case '!':                 /* Negative lookahead */
        bravalue = OP_ASSERT_NOT;
        ptr++;
        break;

        case '<':                 /* Lookbehinds */
        switch (*(++ptr))
          {
          case '=':               /* Positive lookbehind */
          bravalue = OP_ASSERTBACK;
          ptr++;
          break;

          case '!':               /* Negative lookbehind */
          bravalue = OP_ASSERTBACK_NOT;
          ptr++;
          break;
          }
        break;

        case '>':                 /* One-time brackets */
        bravalue = OP_ONCE;
        ptr++;
        break;

        case 'C':                 /* Callout - may be followed by digits */
        *code++ = OP_CALLOUT;
          {
          int n = 0;
          while ((cd->ctypes[*(++ptr)] & ctype_digit) != 0)
            n = n * 10 + *ptr - '0';
          if (n > 255)
            {
            *errorptr = ERR38;
            goto FAILED;
            }
          *code++ = n;
          }
        previous = NULL;
        continue;

        case 'P':                 /* Named subpattern handling */
        if (*(++ptr) == '<')      /* Definition */
          {
          int i, namelen;
          const uschar *name = ++ptr;
          uschar *slot = cd->name_table;

          while (*ptr++ != '>');
          namelen = ptr - name - 1;

          for (i = 0; i < cd->names_found; i++)
            {
            int c = strncmp(name, slot+2, namelen);
            if (c == 0)
              {
              *errorptr = ERR43;
              goto FAILED;
              }
            if (c < 0)
              {
              memmove(slot + cd->name_entry_size, slot,
                (cd->names_found - i) * cd->name_entry_size);
              break;
              }
            slot += cd->name_entry_size;
            }

          PUT2(slot, 0, *brackets + 1);
          memcpy(slot + 2, name, namelen);
          slot[2+namelen] = 0;
          cd->names_found++;
          goto NUMBERED_GROUP;
          }

        if (*ptr == '=' || *ptr == '>')  /* Reference or recursion */
          {
          int i, namelen;
          int type = *ptr++;
          const uschar *name = ptr;
          uschar *slot = cd->name_table;

          while (*ptr != ')') ptr++;
          namelen = ptr - name;

          for (i = 0; i < cd->names_found; i++)
            {
            if (strncmp(name, slot+2, namelen) == 0) break;
            slot += cd->name_entry_size;
            }
          if (i >= cd->names_found)
            {
            *errorptr = ERR15;
            goto FAILED;
            }

          recno = GET2(slot, 0);

          if (type == '>') goto HANDLE_RECURSION;  /* A few lines below */

          /* Back reference */

          previous = code;
          *code++ = OP_REF;
          PUT2INC(code, 0, recno);
          continue;
          }

        /* Should never happen */
        break;

        case 'R':                 /* Pattern recursion */
        ptr++;                    /* Same as (?0)      */
        /* Fall through */

        /* Recursion or "subroutine" call */

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          {
          const uschar *called;
          recno = 0;

          while ((cd->ctypes[*ptr] & ctype_digit) != 0)
            recno = recno * 10 + *ptr++ - '0';

          /* Come here from code above that handles a named recursion */

          HANDLE_RECURSION:

          previous = code;

          /* Find the bracket that is being referenced. Temporarily end the
          regex in case it doesn't exist. */

          *code = OP_END;
          called = (recno == 0)?
            cd->start_code : find_bracket(cd->start_code, recno);
          if (called == NULL)
            {
            *errorptr = ERR15;
            goto FAILED;
            }

          /* If the subpattern is still open, this is a recursive call. We
          check to see if this is a left recursion that could loop for ever,
          and diagnose that case. */

          if (GET(called, 1) == 0 && could_be_empty(called, code, bcptr))
            {
            *errorptr = ERR40;
            goto FAILED;
            }

          /* Insert the recursion/subroutine item */

          *code = OP_RECURSE;
          PUT(code, 1, called - cd->start_code);
          code += 1 + LINK_SIZE;
          }
        continue;

        /* Character after (? not specially recognized */

        default:                  /* Option setting */
        set = unset = 0;
        optset = &set;

        while (*ptr != ')' && *ptr != ':')
          {
          switch (*ptr++)
            {
            case '-': optset = &unset; break;

            case 'i': *optset |= PCRE_CASELESS; break;
            case 'm': *optset |= PCRE_MULTILINE; break;
            case 's': *optset |= PCRE_DOTALL; break;
            case 'x': *optset |= PCRE_EXTENDED; break;
            case 'U': *optset |= PCRE_UNGREEDY; break;
            case 'X': *optset |= PCRE_EXTRA; break;
            }
          }

        /* Set up the changed option bits, but don't change anything yet. */

        newoptions = (options | set) & (~unset);

        /* If the options ended with ')' this is not the start of a nested
        group with option changes, so the options change at this level. Compile
        code to change the ims options if this setting actually changes any of
        them. We also pass the new setting back so that it can be put at the
        start of any following branches, and when this group ends (if we are in
        a group), a resetting item can be compiled.

        Note that if this item is right at the start of the pattern, the
        options will have been abstracted and made global, so there will be no
        change to compile. */

        if (*ptr == ')')
          {
          if ((options & PCRE_IMS) != (newoptions & PCRE_IMS))
            {
            *code++ = OP_OPT;
            *code++ = newoptions & PCRE_IMS;
            }

          /* Change options at this level, and pass them back for use
          in subsequent branches. Reset the greedy defaults and the case
          value for firstchar and reqchar. */

          *optionsptr = options = newoptions;
          greedy_default = ((newoptions & PCRE_UNGREEDY) != 0);
          greedy_non_default = greedy_default ^ 1;
          req_caseopt = ((options & PCRE_CASELESS) != 0)? REQ_CASELESS : 0;

          previous = NULL;       /* This item can't be repeated */
          continue;              /* It is complete */
          }

        /* If the options ended with ':' we are heading into a nested group
        with possible change of options. Such groups are non-capturing and are
        not assertions of any kind. All we need to do is skip over the ':';
        the newoptions value is handled below. */

        bravalue = OP_BRA;
        ptr++;
        }
      }

    /* Else we have a referencing group; adjust the opcode. If the bracket
    number is greater than EXTRACT_BASIC_MAX, we set the opcode one higher, and
    arrange for the true number to follow later, in an OP_BRANUMBER item. */

    else
      {
      NUMBERED_GROUP:
      if (++(*brackets) > EXTRACT_BASIC_MAX)
        {
        bravalue = OP_BRA + EXTRACT_BASIC_MAX + 1;
        code[1+LINK_SIZE] = OP_BRANUMBER;
        PUT2(code, 2+LINK_SIZE, *brackets);
        skipbytes = 3;
        }
      else bravalue = OP_BRA + *brackets;
      }

    /* Process nested bracketed re. Assertions may not be repeated, but other
    kinds can be. We copy code into a non-register variable in order to be able
    to pass its address because some compilers complain otherwise. Pass in a
    new setting for the ims options if they have changed. */

    previous = (bravalue >= OP_ONCE)? code : NULL;
    *code = bravalue;
    tempcode = code;

    if (!compile_regex(
         newoptions,                   /* The complete new option state */
         options & PCRE_IMS,           /* The previous ims option state */
         brackets,                     /* Extracting bracket count */
         &tempcode,                    /* Where to put code (updated) */
         &ptr,                         /* Input pointer (updated) */
         errorptr,                     /* Where to put an error message */
         (bravalue == OP_ASSERTBACK ||
          bravalue == OP_ASSERTBACK_NOT), /* TRUE if back assert */
         skipbytes,                    /* Skip over OP_COND/OP_BRANUMBER */
         &subfirstchar,                /* For possible first char */
         &subreqchar,                  /* For possible last char */
         bcptr,                        /* Current branch chain */
         cd))                          /* Tables block */
      goto FAILED;

    /* At the end of compiling, code is still pointing to the start of the
    group, while tempcode has been updated to point past the end of the group
    and any option resetting that may follow it. The pattern pointer (ptr)
    is on the bracket. */

    /* If this is a conditional bracket, check that there are no more than
    two branches in the group. */

    else if (bravalue == OP_COND)
      {
      uschar *tc = code;
      condcount = 0;

      do {
         condcount++;
         tc += GET(tc,1);
         }
      while (*tc != OP_KET);

      if (condcount > 2)
        {
        *errorptr = ERR27;
        goto FAILED;
        }

      /* If there is just one branch, we must not make use of its firstchar or
      reqchar, because this is equivalent to an empty second branch. */

      if (condcount == 1) subfirstchar = subreqchar = REQ_NONE;
      }

    /* Handle updating of the required and first characters. Update for normal
    brackets of all kinds, and conditions with two branches (see code above).
    If the bracket is followed by a quantifier with zero repeat, we have to
    back off. Hence the definition of zeroreqchar and zerofirstchar outside the
    main loop so that they can be accessed for the back off. */

    zeroreqchar = reqchar;
    zerofirstchar = firstchar;
    groupsetfirstchar = FALSE;

    if (bravalue >= OP_BRA || bravalue == OP_ONCE || bravalue == OP_COND)
      {
      /* If we have not yet set a firstchar in this branch, take it from the
      subpattern, remembering that it was set here so that a repeat of more
      than one can replicate it as reqchar if necessary. If the subpattern has
      no firstchar, set "none" for the whole branch. In both cases, a zero
      repeat forces firstchar to "none". */

      if (firstchar == REQ_UNSET)
        {
        if (subfirstchar >= 0)
          {
          firstchar = subfirstchar;
          groupsetfirstchar = TRUE;
          }
        else firstchar = REQ_NONE;
        zerofirstchar = REQ_NONE;
        }

      /* If firstchar was previously set, convert the subpattern's firstchar
      into reqchar if there wasn't one. */

      else if (subfirstchar >= 0 && subreqchar < 0) subreqchar = subfirstchar;

      /* If the subpattern set a required char (or set a first char that isn't
      really the first char - see above), set it. */

      if (subreqchar >= 0) reqchar = subreqchar;
      }

    /* For a forward assertion, we take the reqchar, if set. This can be
    helpful if the pattern that follows the assertion doesn't set a different
    char. For example, it's useful for /(?=abcde).+/. We can't set firstchar
    for an assertion, however because it leads to incorrect effect for patterns
    such as /(?=a)a.+/ when the "real" "a" would then become a reqchar instead
    of a firstchar. This is overcome by a scan at the end if there's no
    firstchar, looking for an asserted first char. */

    else if (bravalue == OP_ASSERT && subreqchar >= 0) reqchar = subreqchar;

    /* Now update the main code pointer to the end of the group. */

    code = tempcode;

    /* Error if hit end of pattern */

    if (*ptr != ')')
      {
      *errorptr = ERR14;
      goto FAILED;
      }
    break;

    /* Check \ for being a real metacharacter; if not, fall through and handle
    it as a data character at the start of a string. Escape items are checked
    for validity in the pre-compiling pass. */

    case '\\':
    tempptr = ptr;
    c = check_escape(&ptr, errorptr, *brackets, options, FALSE, cd);

    /* Handle metacharacters introduced by \. For ones like \d, the ESC_ values
    are arranged to be the negation of the corresponding OP_values. For the
    back references, the values are ESC_REF plus the reference number. Only
    back references and those types that consume a character may be repeated.
    We can test for values between ESC_b and ESC_Z for the latter; this may
    have to change if any new ones are ever created. */

    if (c < 0)
      {
      if (-c == ESC_Q)            /* Handle start of quoted string */
        {
        if (ptr[1] == '\\' && ptr[2] == 'E') ptr += 2; /* avoid empty string */
          else inescq = TRUE;
        continue;
        }

      /* For metasequences that actually match a character, we disable the
      setting of a first character if it hasn't already been set. */

      if (firstchar == REQ_UNSET && -c > ESC_b && -c < ESC_Z)
        firstchar = REQ_NONE;

      /* Set values to reset to if this is followed by a zero repeat. */

      zerofirstchar = firstchar;
      zeroreqchar = reqchar;

      /* Back references are handled specially */

      if (-c >= ESC_REF)
        {
        int number = -c - ESC_REF;
        previous = code;
        *code++ = OP_REF;
        PUT2INC(code, 0, number);
        }
      else
        {
        previous = (-c > ESC_b && -c < ESC_Z)? code : NULL;
        *code++ = -c;
        }
      continue;
      }

    /* Data character: reset and fall through */

    ptr = tempptr;
    c = '\\';

    /* Handle a run of data characters until a metacharacter is encountered.
    The first character is guaranteed not to be whitespace or # when the
    extended flag is set. */

    NORMAL_CHAR:
    default:
    previous = code;
    *code = OP_CHARS;
    code += 2;
    length = 0;

    do
      {
      /* If in \Q...\E, check for the end; if not, we always have a literal */

      if (inescq)
        {
        if (c == '\\' && ptr[1] == 'E')
          {
          inescq = FALSE;
          ptr++;
          }
        else
          {
          *code++ = c;
          length++;
          }
        continue;
        }

      /* Skip white space and comments for /x patterns */

      if ((options & PCRE_EXTENDED) != 0)
        {
        if ((cd->ctypes[c] & ctype_space) != 0) continue;
        if (c == '#')
          {
          /* The space before the ; is to avoid a warning on a silly compiler
          on the Macintosh. */
          while ((c = *(++ptr)) != 0 && c != NEWLINE) ;
          if (c == 0) break;
          continue;
          }
        }

      /* Backslash may introduce a data char or a metacharacter. Escaped items
      are checked for validity in the pre-compiling pass. Stop the string
      before a metaitem. */

      if (c == '\\')
        {
        tempptr = ptr;
        c = check_escape(&ptr, errorptr, *brackets, options, FALSE, cd);
        if (c < 0) { ptr = tempptr; break; }

        /* If a character is > 127 in UTF-8 mode, we have to turn it into
        two or more characters in the UTF-8 encoding. */

#ifdef SUPPORT_UTF8
        if (c > 127 && (options & PCRE_UTF8) != 0)
          {
          uschar buffer[8];
          int len = ord2utf8(c, buffer);
          for (c = 0; c < len; c++) *code++ = buffer[c];
          length += len;
          continue;
          }
#endif
        }

      /* Ordinary character or single-char escape */

      *code++ = c;
      length++;
      }

    /* This "while" is the end of the "do" above. */

    while (length < MAXLIT && (cd->ctypes[c = *(++ptr)] & ctype_meta) == 0);

    /* Update the first and last character */

    if (firstchar == REQ_UNSET)
      {
      if (length > 1)
        {
        zerofirstchar = firstchar = previous[2] | req_caseopt;
        zeroreqchar = (length > 2)? (code[-2] | req_caseopt) : reqchar;
        reqchar = code[-1] | req_caseopt;
        }
      else
        {
        zerofirstchar = REQ_NONE;
        firstchar = code[-1] | req_caseopt;
        zeroreqchar = reqchar;
        }
      }
    else   /* firstchar previously set */
      {
      zerofirstchar = firstchar;
      zeroreqchar = (length > 1)? (code[-2] | req_caseopt) : reqchar;
      reqchar = code[-1] | req_caseopt;
      }

    /* Set the length in the data vector, and advance to the next state. */

    previous[1] = length;
    if (length < MAXLIT) ptr--;
    break;
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

/* On entry, ptr is pointing past the bracket character, but on return
it points to the closing bracket, or vertical bar, or end of string.
The code variable is pointing at the byte into which the BRA operator has been
stored. If the ims options are changed at the start (for a (?ims: group) or
during any branch, we need to insert an OP_OPT item at the start of every
following branch to ensure they get set correctly at run time, and also pass
the new options into every subsequent branch compile.

Argument:
  options        option bits, including any changes for this subpattern
  oldims         previous settings of ims option bits
  brackets       -> int containing the number of extracting brackets used
  codeptr        -> the address of the current code pointer
  ptrptr         -> the address of the current pattern pointer
  errorptr       -> pointer to error message
  lookbehind     TRUE if this is a lookbehind assertion
  skipbytes      skip this many bytes at start (for OP_COND, OP_BRANUMBER)
  firstcharptr   place to put the first required character, or a negative number
  reqcharptr     place to put the last required character, or a negative number
  bcptr          pointer to the chain of currently open branches
  cd             points to the data block with tables pointers etc.

Returns:      TRUE on success
*/

static BOOL
compile_regex(int options, int oldims, int *brackets, uschar **codeptr,
  const uschar **ptrptr, const char **errorptr, BOOL lookbehind, int skipbytes,
  int *firstcharptr, int *reqcharptr, branch_chain *bcptr, compile_data *cd)
{
const uschar *ptr = *ptrptr;
uschar *code = *codeptr;
uschar *last_branch = code;
uschar *start_bracket = code;
uschar *reverse_count = NULL;
int firstchar, reqchar;
int branchfirstchar, branchreqchar;
branch_chain bc;

bc.outer = bcptr;
bc.current = code;

firstchar = reqchar = REQ_UNSET;

/* Offset is set zero to mark that this bracket is still open */

PUT(code, 1, 0);
code += 1 + LINK_SIZE + skipbytes;

/* Loop for each alternative branch */

for (;;)
  {
  /* Handle a change of ims options at the start of the branch */

  if ((options & PCRE_IMS) != oldims)
    {
    *code++ = OP_OPT;
    *code++ = options & PCRE_IMS;
    }

  /* Set up dummy OP_REVERSE if lookbehind assertion */

  if (lookbehind)
    {
    *code++ = OP_REVERSE;
    reverse_count = code;
    PUTINC(code, 0, 0);
    }

  /* Now compile the branch */

  if (!compile_branch(&options, brackets, &code, &ptr, errorptr,
        &branchfirstchar, &branchreqchar, &bc, cd))
    {
    *ptrptr = ptr;
    return FALSE;
    }

  /* If this is the first branch, the firstchar and reqchar values for the
  branch become the values for the regex. */

  if (*last_branch != OP_ALT)
    {
    firstchar = branchfirstchar;
    reqchar = branchreqchar;
    }

  /* If this is not the first branch, the first char and reqchar have to
  match the values from all the previous branches. */

  else
    {
    /* If we previously had a firstchar, but it doesn't match the new branch,
    we have to abandon the firstchar for the regex, but if there was previously
    no reqchar, it takes on the value of the old firstchar. */

    if (firstchar >= 0 && firstchar != branchfirstchar)
      {
      if (reqchar < 0) reqchar = firstchar;
      firstchar = REQ_NONE;
      }

    /* If we (now or from before) have no firstchar, a firstchar from the
    branch becomes a reqchar if there isn't a branch reqchar. */

    if (firstchar < 0 && branchfirstchar >= 0 && branchreqchar < 0)
        branchreqchar = branchfirstchar;

    /* Now ensure that the reqchars match */

    if (reqchar != branchreqchar) reqchar = REQ_NONE;
    }

  /* If lookbehind, check that this branch matches a fixed-length string,
  and put the length into the OP_REVERSE item. Temporarily mark the end of
  the branch with OP_END. */

  if (lookbehind)
    {
    int length;
    *code = OP_END;
    length = find_fixedlength(last_branch, options);
    DPRINTF(("fixed length = %d\n", length));
    if (length < 0)
      {
      *errorptr = (length == -2)? ERR36 : ERR25;
      *ptrptr = ptr;
      return FALSE;
      }
    PUT(reverse_count, 0, length);
    }

  /* Reached end of expression, either ')' or end of pattern. Go back through
  the alternative branches and reverse the chain of offsets, with the field in
  the BRA item now becoming an offset to the first alternative. If there are
  no alternatives, it points to the end of the group. The length in the
  terminating ket is always the length of the whole bracketed item. If any of
  the ims options were changed inside the group, compile a resetting op-code
  following, except at the very end of the pattern. Return leaving the pointer
  at the terminating char. */

  if (*ptr != '|')
    {
    int length = code - last_branch;
    do
      {
      int prev_length = GET(last_branch, 1);
      PUT(last_branch, 1, length);
      length = prev_length;
      last_branch -= length;
      }
    while (length > 0);

    /* Fill in the ket */

    *code = OP_KET;
    PUT(code, 1, code - start_bracket);
    code += 1 + LINK_SIZE;

    /* Resetting option if needed */

    if ((options & PCRE_IMS) != oldims && *ptr == ')')
      {
      *code++ = OP_OPT;
      *code++ = oldims;
      }

    /* Set values to pass back */

    *codeptr = code;
    *ptrptr = ptr;
    *firstcharptr = firstchar;
    *reqcharptr = reqchar;
    return TRUE;
    }

  /* Another branch follows; insert an "or" node. Its length field points back
  to the previous branch while the bracket remains open. At the end the chain
  is reversed. It's done like this so that the start of the bracket has a
  zero offset until it is closed, making it possible to detect recursion. */

  *code = OP_ALT;
  PUT(code, 1, code - last_branch);
  bc.current = last_branch = code;
  code += 1 + LINK_SIZE;
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

A branch is also implicitly anchored if it starts with .* and DOTALL is set,
because that will try the rest of the pattern at all possible matching points,
so there is no point trying again.... er ....

.... except when the .* appears inside capturing parentheses, and there is a
subsequent back reference to those parentheses. We haven't enough information
to catch that case precisely. The best we can do is to detect when .* is in
capturing brackets and the highest back reference is greater than or equal to
that level.

Arguments:
  code           points to start of expression (the bracket)
  options        points to the options setting
  in_brackets    TRUE if inside capturing parentheses
  top_backref    the highest back reference in the regex

Returns:     TRUE or FALSE
*/

static BOOL
is_anchored(register const uschar *code, int *options, BOOL in_brackets,
  int top_backref)
{
do {
   const uschar *scode =
     first_significant_code(code + 1+LINK_SIZE, options, PCRE_MULTILINE);
   register int op = *scode;

   /* Capturing brackets */

   if (op > OP_BRA)
     {
     if (!is_anchored(scode, options, TRUE, top_backref)) return FALSE;
     }

   /* Other brackets */

   else if (op == OP_BRA || op == OP_ASSERT || op == OP_ONCE || op == OP_COND)
     {
     if (!is_anchored(scode, options, in_brackets, top_backref))
       return FALSE;
     }

   /* .* is not anchored unless DOTALL is set and it isn't in brackets that
   may be referenced. */

   else if ((op == OP_TYPESTAR || op == OP_TYPEMINSTAR) &&
            (*options & PCRE_DOTALL) != 0)
     {
     if (scode[1] != OP_ANY || (in_brackets && top_backref > 0)) return FALSE;
     }

   /* Check for explicit anchoring */

   else if (op != OP_SOD &&
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
  in_brackets    TRUE if inside capturing parentheses
  top_backref    the highest back reference in the regex

Returns:         TRUE or FALSE
*/

static BOOL
is_startline(const uschar *code, BOOL in_brackets, int top_backref)
{
do {
   const uschar *scode = first_significant_code(code + 1+LINK_SIZE, NULL, 0);
   register int op = *scode;

   /* Capturing brackets */

   if (op > OP_BRA)
     { if (!is_startline(scode, TRUE, top_backref)) return FALSE; }

   /* Other brackets */

   else if (op == OP_BRA || op == OP_ASSERT || op == OP_ONCE || op == OP_COND)
     { if (!is_startline(scode, in_brackets, top_backref)) return FALSE; }

   /* .* is not anchored unless DOTALL is set and it isn't in brackets that
   may be referenced. */

   else if (op == OP_TYPESTAR || op == OP_TYPEMINSTAR)
     {
     if (scode[1] != OP_ANY || (in_brackets && top_backref > 0)) return FALSE;
     }

   /* Check for explicit circumflex */

   else if (op != OP_CIRC) return FALSE;
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
     first_significant_code(code + 1+LINK_SIZE, options, PCRE_CASELESS);
   register int op = *scode;

   if (op >= OP_BRA) op = OP_BRA;

   switch(op)
     {
     default:
     return -1;

     case OP_BRA:
     case OP_ASSERT:
     case OP_ONCE:
     case OP_COND:
     if ((d = find_firstassertedchar(scode, options, op == OP_ASSERT)) < 0)
       return -1;
     if (c < 0) c = d; else if (c != d) return -1;
     break;

     case OP_EXACT:       /* Fall through */
     scode++;

     case OP_CHARS:       /* Fall through */
     scode++;

     case OP_PLUS:
     case OP_MINPLUS:
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
holding a compiled version of the expression.

Arguments:
  pattern      the regular expression
  options      various option bits
  errorptr     pointer to pointer to error text
  erroroffset  ptr offset in pattern where error was detected
  tables       pointer to character tables or NULL

Returns:       pointer to compiled data block, or NULL on error,
               with errorptr and erroroffset set
*/

pcre *
pcre_compile(const char *pattern, int options, const char **errorptr,
  int *erroroffset, const unsigned char *tables)
{
real_pcre *re;
int length = 1 + LINK_SIZE;      /* For initial BRA plus length */
int runlength;
int c, firstchar, reqchar;
int bracount = 0;
int top_backref = 0;
int branch_extra = 0;
int branch_newextra;
int item_count = -1;
int name_count = 0;
int max_name_size = 0;
BOOL inescq = FALSE;
unsigned int brastackptr = 0;
size_t size;
uschar *code;
const uschar *codestart;
const uschar *ptr;
compile_data compile_block;
int brastack[BRASTACK_SIZE];
uschar bralenstack[BRASTACK_SIZE];

/* Can't support UTF8 unless PCRE has been compiled to include the code. */

#ifndef SUPPORT_UTF8
if ((options & PCRE_UTF8) != 0)
  {
  *errorptr = ERR32;
  return NULL;
  }
#endif

/* We can't pass back an error message if errorptr is NULL; I guess the best we
can do is just return NULL. */

if (errorptr == NULL) return NULL;
*errorptr = NULL;

/* However, we can give a message for this error */

if (erroroffset == NULL)
  {
  *errorptr = ERR16;
  return NULL;
  }
*erroroffset = 0;

if ((options & ~PUBLIC_OPTIONS) != 0)
  {
  *errorptr = ERR17;
  return NULL;
  }

/* Set up pointers to the individual character tables */

if (tables == NULL) tables = pcre_default_tables;
compile_block.lcc = tables + lcc_offset;
compile_block.fcc = tables + fcc_offset;
compile_block.cbits = tables + cbits_offset;
compile_block.ctypes = tables + ctypes_offset;

/* Reflect pattern for debugging output */

DPRINTF(("------------------------------------------------------------------\n"));
DPRINTF(("%s\n", pattern));

/* The first thing to do is to make a pass over the pattern to compute the
amount of store required to hold the compiled code. This does not have to be
perfect as long as errors are overestimates. At the same time we can detect any
flag settings right at the start, and extract them. Make an attempt to correct
for any counted white space if an "extended" flag setting appears late in the
pattern. We can't be so clever for #-comments. */

ptr = (const uschar *)(pattern - 1);
while ((c = *(++ptr)) != 0)
  {
  int min, max;
  int class_charcount;
  int bracket_length;
  int duplength;

  /* If we are inside a \Q...\E sequence, all chars are literal */

  if (inescq) goto NORMAL_CHAR;

  /* Otherwise, first check for ignored whitespace and comments */

  if ((options & PCRE_EXTENDED) != 0)
    {
    if ((compile_block.ctypes[c] & ctype_space) != 0) continue;
    if (c == '#')
      {
      /* The space before the ; is to avoid a warning on a silly compiler
      on the Macintosh. */
      while ((c = *(++ptr)) != 0 && c != NEWLINE) ;
      if (c == 0) break;
      continue;
      }
    }

  item_count++;    /* Is zero for the first non-comment item */

  switch(c)
    {
    /* A backslashed item may be an escaped "normal" character or a
    character type. For a "normal" character, put the pointers and
    character back so that tests for whitespace etc. in the input
    are done correctly. */

    case '\\':
      {
      const uschar *save_ptr = ptr;
      c = check_escape(&ptr, errorptr, bracount, options, FALSE, &compile_block);
      if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
      if (c >= 0)
        {
        ptr = save_ptr;
        c = '\\';
        goto NORMAL_CHAR;
        }
      }

    /* If \Q, enter "literal" mode */

    if (-c == ESC_Q)
      {
      inescq = TRUE;
      continue;
      }

    /* Other escapes need one byte */

    length++;

    /* A back reference needs an additional 2 bytes, plus either one or 5
    bytes for a repeat. We also need to keep the value of the highest
    back reference. */

    if (c <= -ESC_REF)
      {
      int refnum = -c - ESC_REF;
      if (refnum > top_backref) top_backref = refnum;
      length += 2;   /* For single back reference */
      if (ptr[1] == '{' && is_counted_repeat(ptr+2, &compile_block))
        {
        ptr = read_repeat_counts(ptr+2, &min, &max, errorptr, &compile_block);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if ((min == 0 && (max == 1 || max == -1)) ||
          (min == 1 && max == -1))
            length++;
        else length += 5;
        if (ptr[1] == '?') ptr++;
        }
      }
    continue;

    case '*':            /* These repeats won't be after brackets; */
    case '+':            /* those are handled separately */
    case '?':
    if (ptr[1] == '+')   /* Handle "possessive quantifier" */
      {
      length += 2 + 2*LINK_SIZE;
      ptr++;
      }
    /* Fall through */

    case '^':     /* Single-byte metacharacters */
    case '.':
    case '$':
    length++;
    continue;

    /* This covers the cases of repeats after a single char, metachar, class,
    or back reference. */

    case '{':
    if (!is_counted_repeat(ptr+1, &compile_block)) goto NORMAL_CHAR;
    ptr = read_repeat_counts(ptr+1, &min, &max, errorptr, &compile_block);
    if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
    if ((min == 0 && (max == 1 || max == -1)) ||
      (min == 1 && max == -1))
        length++;
    else
      {
      length--;   /* Uncount the original char or metachar */
      if (min == 1) length++; else if (min > 0) length += 4;
      if (max > 0) length += 4; else length += 2;
      }
    if (ptr[1] == '?') ptr++;                   /* Needs no extra length */
    if (ptr[1] == '+')                          /* Possessive quantifier */
      {
      ptr++;
      length += 2 + 2*LINK_SIZE;              /* Allow for atomic brackets */
      }
    continue;

    /* An alternation contains an offset to the next branch or ket. If any ims
    options changed in the previous branch(es), and/or if we are in a
    lookbehind assertion, extra space will be needed at the start of the
    branch. This is handled by branch_extra. */

    case '|':
    length += 1 + LINK_SIZE + branch_extra;
    continue;

    /* A character class uses 33 characters. Don't worry about character types
    that aren't allowed in classes - they'll get picked up during the compile.
    A character class that contains only one character uses 2 or 3 bytes,
    depending on whether it is negated or not. Notice this where we can. */

    case '[':
    class_charcount = 0;
    if (*(++ptr) == '^') ptr++;

    /* Written as a "do" so that an initial ']' is taken as data */

    if (*ptr != 0) do
      {
      if (*ptr == '\\')
        {
        int ch = check_escape(&ptr, errorptr, bracount, options, TRUE,
          &compile_block);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if (-ch == ESC_b) class_charcount++; else class_charcount = 10;
        }

      /* Check the syntax for POSIX stuff. The bits we actually handle are
      checked during the real compile phase. */

      else if (*ptr == '[' && check_posix_syntax(ptr, &ptr, &compile_block))
        {
        ptr++;
        class_charcount = 10;    /* Make sure > 1 */
        }

      /* Anything else just counts as one char */

      else class_charcount++;
      }
    while (*(++ptr) != 0 && *ptr != ']');   /* Concludes "do" above */

    if (*ptr == 0)                          /* Missing terminating ']' */
      {
      *errorptr = ERR6;
      goto PCRE_ERROR_RETURN;
      }

    /* Repeats for negated single chars are handled by the general code */

    if (class_charcount == 1) length += 3; else
      {
      length += 33;

      /* A repeat needs either 1 or 5 bytes. */

      if (*ptr != 0 && ptr[1] == '{' && is_counted_repeat(ptr+2, &compile_block))
        {
        ptr = read_repeat_counts(ptr+2, &min, &max, errorptr, &compile_block);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if ((min == 0 && (max == 1 || max == -1)) ||
          (min == 1 && max == -1))
            length++;
        else length += 5;
        if (ptr[1] == '?') ptr++;
        }
      }
    continue;

    /* Brackets may be genuine groups or special things */

    case '(':
    branch_newextra = 0;
    bracket_length = 1 + LINK_SIZE;

    /* Handle special forms of bracket, which all start (? */

    if (ptr[1] == '?')
      {
      int set, unset;
      int *optset;

      switch (c = ptr[2])
        {
        /* Skip over comments entirely */
        case '#':
        ptr += 3;
        while (*ptr != 0 && *ptr != ')') ptr++;
        if (*ptr == 0)
          {
          *errorptr = ERR18;
          goto PCRE_ERROR_RETURN;
          }
        continue;

        /* Non-referencing groups and lookaheads just move the pointer on, and
        then behave like a non-special bracket, except that they don't increment
        the count of extracting brackets. Ditto for the "once only" bracket,
        which is in Perl from version 5.005. */

        case ':':
        case '=':
        case '!':
        case '>':
        ptr += 2;
        break;

        /* (?R) specifies a recursive call to the regex, which is an extension
        to provide the facility which can be obtained by (?p{perl-code}) in
        Perl 5.6. In Perl 5.8 this has become (??{perl-code}).

        From PCRE 4.00, items such as (?3) specify subroutine-like "calls" to
        the appropriate numbered brackets. This includes both recursive and
        non-recursive calls. (?R) is now synonymous with (?0). */

        case 'R':
        ptr++;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        ptr += 2;
        if (c != 'R')
          while ((compile_block.ctypes[*(++ptr)] & ctype_digit) != 0);
        if (*ptr != ')')
          {
          *errorptr = ERR29;
          goto PCRE_ERROR_RETURN;
          }
        length += 1 + LINK_SIZE;

        /* If this item is quantified, it will get wrapped inside brackets so
        as to use the code for quantified brackets. We jump down and use the
        code that handles this for real brackets. */

        if (ptr[1] == '+' || ptr[1] == '*' || ptr[1] == '?' || ptr[1] == '{')
          {
          length += 2 + 2 * LINK_SIZE;       /* to make bracketed */
          duplength = 5 + 3 * LINK_SIZE;
          goto HANDLE_QUANTIFIED_BRACKETS;
          }
        continue;

        /* (?C) is an extension which provides "callout" - to provide a bit of
        the functionality of the Perl (?{...}) feature. An optional number may
        follow (default is zero). */

        case 'C':
        ptr += 2;
        while ((compile_block.ctypes[*(++ptr)] & ctype_digit) != 0);
        if (*ptr != ')')
          {
          *errorptr = ERR39;
          goto PCRE_ERROR_RETURN;
          }
        length += 2;
        continue;

        /* Named subpatterns are an extension copied from Python */

        case 'P':
        ptr += 3;
        if (*ptr == '<')
          {
          const uschar *p = ++ptr;
          while ((compile_block.ctypes[*ptr] & ctype_word) != 0) ptr++;
          if (*ptr != '>')
            {
            *errorptr = ERR42;
            goto PCRE_ERROR_RETURN;
            }
          name_count++;
          if (ptr - p > max_name_size) max_name_size = (ptr - p);
          break;
          }

        if (*ptr == '=' || *ptr == '>')
          {
          while ((compile_block.ctypes[*(++ptr)] & ctype_word) != 0);
          if (*ptr != ')')
            {
            *errorptr = ERR42;
            goto PCRE_ERROR_RETURN;
            }
          break;
          }

        /* Unknown character after (?P */

        *errorptr = ERR41;
        goto PCRE_ERROR_RETURN;

        /* Lookbehinds are in Perl from version 5.005 */

        case '<':
        ptr += 3;
        if (*ptr == '=' || *ptr == '!')
          {
          branch_newextra = 1 + LINK_SIZE;
          length += 1 + LINK_SIZE;         /* For the first branch */
          break;
          }
        *errorptr = ERR24;
        goto PCRE_ERROR_RETURN;

        /* Conditionals are in Perl from version 5.005. The bracket must either
        be followed by a number (for bracket reference) or by an assertion
        group, or (a PCRE extension) by 'R' for a recursion test. */

        case '(':
        if (ptr[3] == 'R' && ptr[4] == ')')
          {
          ptr += 4;
          length += 3;
          }
        else if ((compile_block.ctypes[ptr[3]] & ctype_digit) != 0)
          {
          ptr += 4;
          length += 3;
          while ((compile_block.ctypes[*ptr] & ctype_digit) != 0) ptr++;
          if (*ptr != ')')
            {
            *errorptr = ERR26;
            goto PCRE_ERROR_RETURN;
            }
          }
        else   /* An assertion must follow */
          {
          ptr++;   /* Can treat like ':' as far as spacing is concerned */
          if (ptr[2] != '?' ||
             (ptr[3] != '=' && ptr[3] != '!' && ptr[3] != '<') )
            {
            ptr += 2;    /* To get right offset in message */
            *errorptr = ERR28;
            goto PCRE_ERROR_RETURN;
            }
          }
        break;

        /* Else loop checking valid options until ) is met. Anything else is an
        error. If we are without any brackets, i.e. at top level, the settings
        act as if specified in the options, so massage the options immediately.
        This is for backward compatibility with Perl 5.004. */

        default:
        set = unset = 0;
        optset = &set;
        ptr += 2;

        for (;; ptr++)
          {
          c = *ptr;
          switch (c)
            {
            case 'i':
            *optset |= PCRE_CASELESS;
            continue;

            case 'm':
            *optset |= PCRE_MULTILINE;
            continue;

            case 's':
            *optset |= PCRE_DOTALL;
            continue;

            case 'x':
            *optset |= PCRE_EXTENDED;
            continue;

            case 'X':
            *optset |= PCRE_EXTRA;
            continue;

            case 'U':
            *optset |= PCRE_UNGREEDY;
            continue;

            case '-':
            optset = &unset;
            continue;

            /* A termination by ')' indicates an options-setting-only item; if
            this is at the very start of the pattern (indicated by item_count
            being zero), we use it to set the global options. This is helpful
            when analyzing the pattern for first characters, etc. Otherwise
            nothing is done here and it is handled during the compiling
            process.

            [Historical note: Up to Perl 5.8, options settings at top level
            were always global settings, wherever they appeared in the pattern.
            That is, they were equivalent to an external setting. From 5.8
            onwards, they apply only to what follows (which is what you might
            expect).] */

            case ')':
            if (item_count == 0)
              {
              options = (options | set) & (~unset);
              set = unset = 0;     /* To save length */
              item_count--;        /* To allow for several */
              }

            /* Fall through */

            /* A termination by ':' indicates the start of a nested group with
            the given options set. This is again handled at compile time, but
            we must allow for compiled space if any of the ims options are
            set. We also have to allow for resetting space at the end of
            the group, which is why 4 is added to the length and not just 2.
            If there are several changes of options within the same group, this
            will lead to an over-estimate on the length, but this shouldn't
            matter very much. We also have to allow for resetting options at
            the start of any alternations, which we do by setting
            branch_newextra to 2. Finally, we record whether the case-dependent
            flag ever changes within the regex. This is used by the "required
            character" code. */

            case ':':
            if (((set|unset) & PCRE_IMS) != 0)
              {
              length += 4;
              branch_newextra = 2;
              if (((set|unset) & PCRE_CASELESS) != 0) options |= PCRE_ICHANGED;
              }
            goto END_OPTIONS;

            /* Unrecognized option character */

            default:
            *errorptr = ERR12;
            goto PCRE_ERROR_RETURN;
            }
          }

        /* If we hit a closing bracket, that's it - this is a freestanding
        option-setting. We need to ensure that branch_extra is updated if
        necessary. The only values branch_newextra can have here are 0 or 2.
        If the value is 2, then branch_extra must either be 2 or 5, depending
        on whether this is a lookbehind group or not. */

        END_OPTIONS:
        if (c == ')')
          {
          if (branch_newextra == 2 &&
              (branch_extra == 0 || branch_extra == 1+LINK_SIZE))
            branch_extra += branch_newextra;
          continue;
          }

        /* If options were terminated by ':' control comes here. Fall through
        to handle the group below. */
        }
      }

    /* Extracting brackets must be counted so we can process escapes in a
    Perlish way. If the number exceeds EXTRACT_BASIC_MAX we are going to
    need an additional 3 bytes of store per extracting bracket. */

    else
      {
      bracount++;
      if (bracount > EXTRACT_BASIC_MAX) bracket_length += 3;
      }

    /* Save length for computing whole length at end if there's a repeat that
    requires duplication of the group. Also save the current value of
    branch_extra, and start the new group with the new value. If non-zero, this
    will either be 2 for a (?imsx: group, or 3 for a lookbehind assertion. */

    if (brastackptr >= sizeof(brastack)/sizeof(int))
      {
      *errorptr = ERR19;
      goto PCRE_ERROR_RETURN;
      }

    bralenstack[brastackptr] = branch_extra;
    branch_extra = branch_newextra;

    brastack[brastackptr++] = length;
    length += bracket_length;
    continue;

    /* Handle ket. Look for subsequent max/min; for certain sets of values we
    have to replicate this bracket up to that many times. If brastackptr is
    0 this is an unmatched bracket which will generate an error, but take care
    not to try to access brastack[-1] when computing the length and restoring
    the branch_extra value. */

    case ')':
    length += 1 + LINK_SIZE;
    if (brastackptr > 0)
      {
      duplength = length - brastack[--brastackptr];
      branch_extra = bralenstack[brastackptr];
      }
    else duplength = 0;

    /* The following code is also used when a recursion such as (?3) is
    followed by a quantifier, because in that case, it has to be wrapped inside
    brackets so that the quantifier works. The value of duplength must be
    set before arrival. */

    HANDLE_QUANTIFIED_BRACKETS:

    /* Leave ptr at the final char; for read_repeat_counts this happens
    automatically; for the others we need an increment. */

    if ((c = ptr[1]) == '{' && is_counted_repeat(ptr+2, &compile_block))
      {
      ptr = read_repeat_counts(ptr+2, &min, &max, errorptr, &compile_block);
      if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
      }
    else if (c == '*') { min = 0; max = -1; ptr++; }
    else if (c == '+') { min = 1; max = -1; ptr++; }
    else if (c == '?') { min = 0; max = 1;  ptr++; }
    else { min = 1; max = 1; }

    /* If the minimum is zero, we have to allow for an OP_BRAZERO before the
    group, and if the maximum is greater than zero, we have to replicate
    maxval-1 times; each replication acquires an OP_BRAZERO plus a nesting
    bracket set. */

    if (min == 0)
      {
      length++;
      if (max > 0) length += (max - 1) * (duplength + 3 + 2*LINK_SIZE);
      }

    /* When the minimum is greater than zero, we have to replicate up to
    minval-1 times, with no additions required in the copies. Then, if there
    is a limited maximum we have to replicate up to maxval-1 times allowing
    for a BRAZERO item before each optional copy and nesting brackets for all
    but one of the optional copies. */

    else
      {
      length += (min - 1) * duplength;
      if (max > min)   /* Need this test as max=-1 means no limit */
        length += (max - min) * (duplength + 3 + 2*LINK_SIZE)
          - (2 + 2*LINK_SIZE);
      }

    /* Allow space for once brackets for "possessive quantifier" */

    if (ptr[1] == '+')
      {
      ptr++;
      length += 2 + 2*LINK_SIZE;
      }
    continue;

    /* Non-special character. For a run of such characters the length required
    is the number of characters + 2, except that the maximum run length is 255.
    We won't get a skipped space or a non-data escape or the start of a #
    comment as the first character, so the length can't be zero. */

    NORMAL_CHAR:
    default:
    length += 2;
    runlength = 0;
    do
      {
      /* If in a \Q...\E sequence, check for end; otherwise it's a literal */
      if (inescq)
        {
        if (c == '\\' && ptr[1] == 'E')
          {
          inescq = FALSE;
          ptr++;
          }
        else runlength++;
        continue;
        }

      /* Skip whitespace and comments for /x */

      if ((options & PCRE_EXTENDED) != 0)
        {
        if ((compile_block.ctypes[c] & ctype_space) != 0) continue;
        if (c == '#')
          {
          /* The space before the ; is to avoid a warning on a silly compiler
          on the Macintosh. */
          while ((c = *(++ptr)) != 0 && c != NEWLINE) ;
          continue;
          }
        }

      /* Backslash may introduce a data char or a metacharacter; stop the
      string before the latter. */

      if (c == '\\')
        {
        const uschar *saveptr = ptr;
        c = check_escape(&ptr, errorptr, bracount, options, FALSE,
          &compile_block);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if (c < 0) { ptr = saveptr; break; }

#ifdef SUPPORT_UTF8
        if (c > 127 && (options & PCRE_UTF8) != 0)
          {
          int i;
          for (i = 0; i < sizeof(utf8_table1)/sizeof(int); i++)
            if (c <= utf8_table1[i]) break;
          runlength += i;
          }
#endif
        }

      /* Ordinary character or single-char escape */

      runlength++;
      }

    /* This "while" is the end of the "do" above. */

    while (runlength < MAXLIT &&
      (compile_block.ctypes[c = *(++ptr)] & ctype_meta) == 0);

    if (runlength < MAXLIT) ptr--;
    length += runlength;
    continue;
    }
  }

length += 2 + LINK_SIZE;    /* For final KET and END */

if (length > MAX_PATTERN_SIZE)
  {
  *errorptr = ERR20;
  return NULL;
  }

/* Compute the size of data block needed and get it, either from malloc or
externally provided function. */

size = length + sizeof(real_pcre) + name_count * (max_name_size + 3);
re = (real_pcre *)(pcre_malloc)(size);

if (re == NULL)
  {
  *errorptr = ERR21;
  return NULL;
  }

/* Put in the magic number, and save the size, options, and table pointer */

re->magic_number = MAGIC_NUMBER;
re->size = size;
re->options = options;
re->tables = tables;
re->name_entry_size = max_name_size + 3;
re->name_count = name_count;

/* The starting points of the name/number translation table and of the code are
passed around in the compile data block. */

compile_block.names_found = 0;
compile_block.name_entry_size = max_name_size + 3;
compile_block.name_table = (uschar *)re + sizeof(real_pcre);
codestart = compile_block.name_table + re->name_entry_size * re->name_count;
compile_block.start_code = codestart;

/* Set up a starting, non-extracting bracket, then compile the expression. On
error, *errorptr will be set non-NULL, so we don't need to look at the result
of the function here. */

ptr = (const uschar *)pattern;
code = (uschar *)codestart;
*code = OP_BRA;
bracount = 0;
(void)compile_regex(options, options & PCRE_IMS, &bracount, &code, &ptr,
  errorptr, FALSE, 0, &firstchar, &reqchar, NULL, &compile_block);
re->top_bracket = bracount;
re->top_backref = top_backref;

/* If not reached end of pattern on success, there's an excess bracket. */

if (*errorptr == NULL && *ptr != 0) *errorptr = ERR22;

/* Fill in the terminating state and check for disastrous overflow, but
if debugging, leave the test till after things are printed out. */

*code++ = OP_END;

#ifndef DEBUG
if (code - codestart > length) *errorptr = ERR23;
#endif

/* Give an error if there's back reference to a non-existent capturing
subpattern. */

if (top_backref > re->top_bracket) *errorptr = ERR15;

/* Failed to compile, or error while post-processing */

if (*errorptr != NULL)
  {
  (pcre_free)(re);
  PCRE_ERROR_RETURN:
  *erroroffset = ptr - (const uschar *)pattern;
  return NULL;
  }

/* If the anchored option was not passed, set the flag if we can determine that
the pattern is anchored by virtue of ^ characters or \A or anything else (such
as starting with .* when DOTALL is set).

Otherwise, if we know what the first character has to be, save it, because that
speeds up unanchored matches no end. If not, see if we can set the
PCRE_STARTLINE flag. This is helpful for multiline matches when all branches
start with ^. and also when all branches start with .* for non-DOTALL matches.
*/

if ((options & PCRE_ANCHORED) == 0)
  {
  int temp_options = options;
  if (is_anchored(codestart, &temp_options, FALSE, top_backref))
    re->options |= PCRE_ANCHORED;
  else
    {
    if (firstchar < 0)
      firstchar = find_firstassertedchar(codestart, &temp_options, FALSE);
    if (firstchar >= 0)   /* Remove caseless flag for non-caseable chars */
      {
      int ch = firstchar & 255;
      re->first_char = ((firstchar & REQ_CASELESS) != 0 &&
         compile_block.fcc[ch] == ch)? ch : firstchar;
      re->options |= PCRE_FIRSTSET;
      }
    else if (is_startline(codestart, FALSE, top_backref))
      re->options |= PCRE_STARTLINE;
    }
  }

/* Save the last required character if any. Remove caseless flag for
non-caseable chars. */

if ((re->options & PCRE_ANCHORED) != 0 && reqchar < 0 && firstchar >= 0)
  reqchar = firstchar;

if (reqchar >= 0)
  {
  int ch = reqchar & 255;
  re->req_char = ((reqchar & REQ_CASELESS) != 0 &&
    compile_block.fcc[ch] == ch)? ch : reqchar;
  re->options |= PCRE_REQCHSET;
  }

/* Print out the compiled data for debugging */

#ifdef DEBUG

printf("Length = %d top_bracket = %d top_backref = %d\n",
  length, re->top_bracket, re->top_backref);

if (re->options != 0)
  {
  printf("%s%s%s%s%s%s%s%s%s\n",
    ((re->options & PCRE_ANCHORED) != 0)? "anchored " : "",
    ((re->options & PCRE_CASELESS) != 0)? "caseless " : "",
    ((re->options & PCRE_ICHANGED) != 0)? "case state changed " : "",
    ((re->options & PCRE_EXTENDED) != 0)? "extended " : "",
    ((re->options & PCRE_MULTILINE) != 0)? "multiline " : "",
    ((re->options & PCRE_DOTALL) != 0)? "dotall " : "",
    ((re->options & PCRE_DOLLAR_ENDONLY) != 0)? "endonly " : "",
    ((re->options & PCRE_EXTRA) != 0)? "extra " : "",
    ((re->options & PCRE_UNGREEDY) != 0)? "ungreedy " : "");
  }

if ((re->options & PCRE_FIRSTSET) != 0)
  {
  int ch = re->first_char & 255;
  char *caseless = ((re->first_char & REQ_CASELESS) == 0)? "" : " (caseless)";
  if (isprint(ch)) printf("First char = %c%s\n", ch, caseless);
    else printf("First char = \\x%02x%s\n", ch, caseless);
  }

if ((re->options & PCRE_REQCHSET) != 0)
  {
  int ch = re->req_char & 255;
  char *caseless = ((re->req_char & REQ_CASELESS) == 0)? "" : " (caseless)";
  if (isprint(ch)) printf("Req char = %c%s\n", ch, caseless);
    else printf("Req char = \\x%02x%s\n", ch, caseless);
  }

print_internals(re, stdout);

/* This check is done here in the debugging case so that the code that
was compiled can be seen. */

if (code - codestart > length)
  {
  *errorptr = ERR23;
  (pcre_free)(re);
  *erroroffset = ptr - (uschar *)pattern;
  return NULL;
  }
#endif

return (pcre *)re;
}



/*************************************************
*          Match a back-reference                *
*************************************************/

/* If a back reference hasn't been set, the length that is passed is greater
than the number of characters left in the string, so the match fails.

Arguments:
  offset      index into the offset vector
  eptr        points into the subject
  length      length to be matched
  md          points to match data block
  ims         the ims flags

Returns:      TRUE if matched
*/

static BOOL
match_ref(int offset, register const uschar *eptr, int length, match_data *md,
  unsigned long int ims)
{
const uschar *p = md->start_subject + md->offset_vector[offset];

#ifdef DEBUG
if (eptr >= md->end_subject)
  printf("matching subject <null>");
else
  {
  printf("matching subject ");
  pchars(eptr, length, TRUE, md);
  }
printf(" against backref ");
pchars(p, length, FALSE, md);
printf("\n");
#endif

/* Always fail if not enough characters left */

if (length > md->end_subject - eptr) return FALSE;

/* Separate the caselesss case for speed */

if ((ims & PCRE_CASELESS) != 0)
  {
  while (length-- > 0)
    if (md->lcc[*p++] != md->lcc[*eptr++]) return FALSE;
  }
else
  { while (length-- > 0) if (*p++ != *eptr++) return FALSE; }

return TRUE;
}



/*************************************************
*         Match from current position            *
*************************************************/

/* On entry ecode points to the first opcode, and eptr to the first character
in the subject string, while eptrb holds the value of eptr at the start of the
last bracketed group - used for breaking infinite loops matching zero-length
strings.

Arguments:
   eptr        pointer in subject
   ecode       position in code
   offset_top  current top pointer
   md          pointer to "static" info for the match
   ims         current /i, /m, and /s options
   eptrb       pointer to chain of blocks containing eptr at start of
                 brackets - for testing for empty matches
   flags       can contain
                 match_condassert - this is an assertion condition
                 match_isgroup - this is the start of a bracketed group

Returns:       TRUE if matched
*/

static BOOL
match(register const uschar *eptr, register const uschar *ecode,
  int offset_top, match_data *md, unsigned long int ims, eptrblock *eptrb,
  int flags)
{
unsigned long int original_ims = ims;   /* Save for resetting on ')' */
eptrblock newptrb;

/* At the start of a bracketed group, add the current subject pointer to the
stack of such pointers, to be re-instated at the end of the group when we hit
the closing ket. When match() is called in other circumstances, we don't add to
the stack. */

if ((flags & match_isgroup) != 0)
  {
  newptrb.prev = eptrb;
  newptrb.saved_eptr = eptr;
  eptrb = &newptrb;
  }

/* Now start processing the operations. */

for (;;)
  {
  int op = (int)*ecode;
  int min, max, ctype;
  register int i;
  register int c;
  BOOL minimize = FALSE;

  /* Opening capturing bracket. If there is space in the offset vector, save
  the current subject position in the working slot at the top of the vector. We
  mustn't change the current values of the data slot, because they may be set
  from a previous iteration of this group, and be referred to by a reference
  inside the group.

  If the bracket fails to match, we need to restore this value and also the
  values of the final offsets, in case they were set by a previous iteration of
  the same bracket.

  If there isn't enough space in the offset vector, treat this as if it were a
  non-capturing bracket. Don't worry about setting the flag for the error case
  here; that is handled in the code for KET. */

  if (op > OP_BRA)
    {
    int offset;
    int number = op - OP_BRA;

    /* For extended extraction brackets (large number), we have to fish out the
    number from a dummy opcode at the start. */

    if (number > EXTRACT_BASIC_MAX)
      number = GET2(ecode, 2+LINK_SIZE);
    offset = number << 1;

#ifdef DEBUG
    printf("start bracket %d subject=", number);
    pchars(eptr, 16, TRUE, md);
    printf("\n");
#endif

    if (offset < md->offset_max)
      {
      int save_offset1 = md->offset_vector[offset];
      int save_offset2 = md->offset_vector[offset+1];
      int save_offset3 = md->offset_vector[md->offset_end - number];
      int save_capture_last = md->capture_last;

      DPRINTF(("saving %d %d %d\n", save_offset1, save_offset2, save_offset3));
      md->offset_vector[md->offset_end - number] = eptr - md->start_subject;

      do
        {
        if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, eptrb,
          match_isgroup)) return TRUE;
        md->capture_last = save_capture_last;
        ecode += GET(ecode, 1);
        }
      while (*ecode == OP_ALT);

      DPRINTF(("bracket %d failed\n", number));

      md->offset_vector[offset] = save_offset1;
      md->offset_vector[offset+1] = save_offset2;
      md->offset_vector[md->offset_end - number] = save_offset3;

      return FALSE;
      }

    /* Insufficient room for saving captured contents */

    else op = OP_BRA;
    }

  /* Other types of node can be handled by a switch */

  switch(op)
    {
    case OP_BRA:     /* Non-capturing bracket: optimized */
    DPRINTF(("start bracket 0\n"));
    do
      {
      if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, eptrb,
        match_isgroup)) return TRUE;
      ecode += GET(ecode, 1);
      }
    while (*ecode == OP_ALT);
    DPRINTF(("bracket 0 failed\n"));
    return FALSE;

    /* Conditional group: compilation checked that there are no more than
    two branches. If the condition is false, skipping the first branch takes us
    past the end if there is only one branch, but that's OK because that is
    exactly what going to the ket would do. */

    case OP_COND:
    if (ecode[LINK_SIZE+1] == OP_CREF) /* Condition extract or recurse test */
      {
      int offset = GET2(ecode, LINK_SIZE+2) << 1;  /* Doubled ref number */
      BOOL condition = (offset == CREF_RECURSE * 2)?
        (md->recursive != NULL) :
        (offset < offset_top && md->offset_vector[offset] >= 0);
      return match(eptr, ecode + (condition?
        (LINK_SIZE + 4) : (LINK_SIZE + 1 + GET(ecode, 1))),
        offset_top, md, ims, eptrb, match_isgroup);
      }

    /* The condition is an assertion. Call match() to evaluate it - setting
    the final argument TRUE causes it to stop at the end of an assertion. */

    else
      {
      if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, NULL,
          match_condassert | match_isgroup))
        {
        ecode += 1 + LINK_SIZE + GET(ecode, LINK_SIZE+2);
        while (*ecode == OP_ALT) ecode += GET(ecode, 1);
        }
      else ecode += GET(ecode, 1);
      return match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, eptrb,
        match_isgroup);
      }
    /* Control never reaches here */

    /* Skip over conditional reference or large extraction number data if
    encountered. */

    case OP_CREF:
    case OP_BRANUMBER:
    ecode += 3;
    break;

    /* End of the pattern. If we are in a recursion, we should restore the
    offsets appropriately and continue from after the call. */

    case OP_END:
    if (md->recursive != NULL && md->recursive->group_num == 0)
      {
      recursion_info *rec = md->recursive;
      DPRINTF(("Hit the end in a (?0) recursion\n"));
      md->recursive = rec->prev;
      memmove(md->offset_vector, rec->offset_save,
        rec->saved_max * sizeof(int));
      md->start_match = rec->save_start;
      ims = original_ims;
      ecode = rec->after_call;
      break;
      }

    /* Otherwise, if PCRE_NOTEMPTY is set, fail if we have matched an empty
    string - backtracking will then try other alternatives, if any. */

    if (md->notempty && eptr == md->start_match) return FALSE;
    md->end_match_ptr = eptr;          /* Record where we ended */
    md->end_offset_top = offset_top;   /* and how many extracts were taken */
    return TRUE;

    /* Change option settings */

    case OP_OPT:
    ims = ecode[1];
    ecode += 2;
    DPRINTF(("ims set to %02lx\n", ims));
    break;

    /* Assertion brackets. Check the alternative branches in turn - the
    matching won't pass the KET for an assertion. If any one branch matches,
    the assertion is true. Lookbehind assertions have an OP_REVERSE item at the
    start of each branch to move the current point backwards, so the code at
    this level is identical to the lookahead case. */

    case OP_ASSERT:
    case OP_ASSERTBACK:
    do
      {
      if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, NULL,
        match_isgroup)) break;
      ecode += GET(ecode, 1);
      }
    while (*ecode == OP_ALT);
    if (*ecode == OP_KET) return FALSE;

    /* If checking an assertion for a condition, return TRUE. */

    if ((flags & match_condassert) != 0) return TRUE;

    /* Continue from after the assertion, updating the offsets high water
    mark, since extracts may have been taken during the assertion. */

    do ecode += GET(ecode,1); while (*ecode == OP_ALT);
    ecode += 1 + LINK_SIZE;
    offset_top = md->end_offset_top;
    continue;

    /* Negative assertion: all branches must fail to match */

    case OP_ASSERT_NOT:
    case OP_ASSERTBACK_NOT:
    do
      {
      if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, NULL,
        match_isgroup)) return FALSE;
      ecode += GET(ecode,1);
      }
    while (*ecode == OP_ALT);

    if ((flags & match_condassert) != 0) return TRUE;

    ecode += 1 + LINK_SIZE;
    continue;

    /* Move the subject pointer back. This occurs only at the start of
    each branch of a lookbehind assertion. If we are too close to the start to
    move back, this match function fails. When working with UTF-8 we move
    back a number of characters, not bytes. */

    case OP_REVERSE:
#ifdef SUPPORT_UTF8
    c = GET(ecode,1);
    for (i = 0; i < c; i++)
      {
      eptr--;
      BACKCHAR(eptr)
      }
#else
    eptr -= GET(ecode,1);
#endif

    if (eptr < md->start_subject) return FALSE;
    ecode += 1 + LINK_SIZE;
    break;

    /* The callout item calls an external function, if one is provided, passing
    details of the match so far. This is mainly for debugging, though the
    function is able to force a failure. */

    case OP_CALLOUT:
    if (pcre_callout != NULL)
      {
      pcre_callout_block cb;
      cb.version          = 0;   /* Version 0 of the callout block */
      cb.callout_number   = ecode[1];
      cb.offset_vector    = md->offset_vector;
      cb.subject          = (const char *)md->start_subject;
      cb.subject_length   = md->end_subject - md->start_subject;
      cb.start_match      = md->start_match - md->start_subject;
      cb.current_position = eptr - md->start_subject;
      cb.capture_top      = offset_top/2;
      cb.capture_last     = md->capture_last;
      if ((*pcre_callout)(&cb) != 0) return FALSE;
      }
    ecode += 2;
    break;

    /* Recursion either matches the current regex, or some subexpression. The
    offset data is the offset to the starting bracket from the start of the
    whole pattern. However, it is possible that a BRAZERO was inserted before
    this bracket after we took the offset - we just skip it if encountered.

    If there are any capturing brackets started but not finished, we have to
    save their starting points and reinstate them after the recursion. However,
    we don't know how many such there are (offset_top records the completed
    total) so we just have to save all the potential data. There may be up to
    65535 such values, which is too large to put on the stack, but using malloc
    for small numbers seems expensive. As a compromise, the stack is used when
    there are no more than REC_STACK_SAVE_MAX values to store; otherwise malloc
    is used. A problem is what to do if the malloc fails ... there is no way of
    returning to the top level with an error. Save the top REC_STACK_SAVE_MAX
    values on the stack, and accept that the rest may be wrong.

    There are also other values that have to be saved. We use a chained
    sequence of blocks that actually live on the stack. Thanks to Robin Houston
    for the original version of this logic. */

    case OP_RECURSE:
      {
      int stacksave[REC_STACK_SAVE_MAX];
      recursion_info new_recursive;
      const uschar *callpat = md->start_code + GET(ecode, 1);

      if (*callpat == OP_BRAZERO) callpat++;

      new_recursive.group_num = *callpat - OP_BRA;

      /* For extended extraction brackets (large number), we have to fish out
      the number from a dummy opcode at the start. */

      if (new_recursive.group_num > EXTRACT_BASIC_MAX)
        new_recursive.group_num = GET2(callpat, 2+LINK_SIZE);

      /* Add to "recursing stack" */

      new_recursive.prev = md->recursive;
      md->recursive = &new_recursive;

      /* Find where to continue from afterwards */

      ecode += 1 + LINK_SIZE;
      new_recursive.after_call = ecode;

      /* Now save the offset data. */

      new_recursive.saved_max = md->offset_end;
      if (new_recursive.saved_max <= REC_STACK_SAVE_MAX)
        new_recursive.offset_save = stacksave;
      else
        {
        new_recursive.offset_save = (int *)
            (pcre_malloc)(new_recursive.saved_max * sizeof(int));

        /* RH: Warning: This may cause INCORRECT RESULTS if we run out of
        memory here, because we won't be restoring all the stored strings
        correctly. We either need proper run-time error handling or, at the
        very least, some way to warn the user. Could we just spit a message to
        stderr?

        PH: No, Robin, no! You must NEVER write to stderr from inside a general
        library function, because you don't know anything about the state of
        the file descriptor.

        RH: Returning error values would be very tedious because of the
        recursion; and Philip Hazel says that longjmp() - in many ways the
        obvious solution - has previously caused problems on some platforms. */

        if (new_recursive.offset_save == NULL)
          {
          DPRINTF(("malloc() failed - results may be wrong\n"));
          new_recursive.offset_save = stacksave;
          new_recursive.saved_max = REC_STACK_SAVE_MAX;
          }
        }

      memcpy(new_recursive.offset_save, md->offset_vector,
            new_recursive.saved_max * sizeof(int));
      new_recursive.save_start = md->start_match;
      md->start_match = eptr;

      /* OK, now we can do the recursion. For each top-level alternative we
      restore the offset and recursion data. */

      DPRINTF(("Recursing into group %d\n", new_recursive.group_num));
      do
        {
        if (match(eptr, callpat + 1 + LINK_SIZE, offset_top, md, ims, eptrb,
            match_isgroup))
          {
          md->recursive = new_recursive.prev;
          if (new_recursive.offset_save != stacksave)
            (pcre_free)(new_recursive.offset_save);
          return TRUE;
          }

        md->recursive = &new_recursive;
        memcpy(md->offset_vector, new_recursive.offset_save,
            new_recursive.saved_max * sizeof(int));
        callpat += GET(callpat, 1);
        }
      while (*callpat == OP_ALT);

      DPRINTF(("Recursion didn't match\n"));
      md->recursive = new_recursive.prev;
      if (new_recursive.offset_save != stacksave)
        (pcre_free)(new_recursive.offset_save);
      return FALSE;
      }
    break;

    /* "Once" brackets are like assertion brackets except that after a match,
    the point in the subject string is not moved back. Thus there can never be
    a move back into the brackets. Friedl calls these "atomic" subpatterns.
    Check the alternative branches in turn - the matching won't pass the KET
    for this kind of subpattern. If any one branch matches, we carry on as at
    the end of a normal bracket, leaving the subject pointer. */

    case OP_ONCE:
      {
      const uschar *prev = ecode;
      const uschar *saved_eptr = eptr;

      do
        {
        if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, eptrb,
          match_isgroup)) break;
        ecode += GET(ecode,1);
        }
      while (*ecode == OP_ALT);

      /* If hit the end of the group (which could be repeated), fail */

      if (*ecode != OP_ONCE && *ecode != OP_ALT) return FALSE;

      /* Continue as from after the assertion, updating the offsets high water
      mark, since extracts may have been taken. */

      do ecode += GET(ecode,1); while (*ecode == OP_ALT);

      offset_top = md->end_offset_top;
      eptr = md->end_match_ptr;

      /* For a non-repeating ket, just continue at this level. This also
      happens for a repeating ket if no characters were matched in the group.
      This is the forcible breaking of infinite loops as implemented in Perl
      5.005. If there is an options reset, it will get obeyed in the normal
      course of events. */

      if (*ecode == OP_KET || eptr == saved_eptr)
        {
        ecode += 1+LINK_SIZE;
        break;
        }

      /* The repeating kets try the rest of the pattern or restart from the
      preceding bracket, in the appropriate order. We need to reset any options
      that changed within the bracket before re-running it, so check the next
      opcode. */

      if (ecode[1+LINK_SIZE] == OP_OPT)
        {
        ims = (ims & ~PCRE_IMS) | ecode[4];
        DPRINTF(("ims set to %02lx at group repeat\n", ims));
        }

      if (*ecode == OP_KETRMIN)
        {
        if (match(eptr, ecode + 1 + LINK_SIZE, offset_top, md, ims, eptrb, 0)
            ||
            match(eptr, prev, offset_top, md, ims, eptrb, match_isgroup))
              return TRUE;
        }
      else  /* OP_KETRMAX */
        {
        if (match(eptr, prev, offset_top, md, ims, eptrb, match_isgroup) ||
            match(eptr, ecode + 1+LINK_SIZE, offset_top, md, ims, eptrb, 0))
            return TRUE;
        }
      }
    return FALSE;

    /* An alternation is the end of a branch; scan along to find the end of the
    bracketed group and go to there. */

    case OP_ALT:
    do ecode += GET(ecode,1); while (*ecode == OP_ALT);
    break;

    /* BRAZERO and BRAMINZERO occur just before a bracket group, indicating
    that it may occur zero times. It may repeat infinitely, or not at all -
    i.e. it could be ()* or ()? in the pattern. Brackets with fixed upper
    repeat limits are compiled as a number of copies, with the optional ones
    preceded by BRAZERO or BRAMINZERO. */

    case OP_BRAZERO:
      {
      const uschar *next = ecode+1;
      if (match(eptr, next, offset_top, md, ims, eptrb, match_isgroup))
        return TRUE;
      do next += GET(next,1); while (*next == OP_ALT);
      ecode = next + 1+LINK_SIZE;
      }
    break;

    case OP_BRAMINZERO:
      {
      const uschar *next = ecode+1;
      do next += GET(next,1); while (*next == OP_ALT);
      if (match(eptr, next + 1+LINK_SIZE, offset_top, md, ims, eptrb,
        match_isgroup)) return TRUE;
      ecode++;
      }
    break;

    /* End of a group, repeated or non-repeating. If we are at the end of
    an assertion "group", stop matching and return TRUE, but record the
    current high water mark for use by positive assertions. Do this also
    for the "once" (not-backup up) groups. */

    case OP_KET:
    case OP_KETRMIN:
    case OP_KETRMAX:
      {
      const uschar *prev = ecode - GET(ecode, 1);
      const uschar *saved_eptr = eptrb->saved_eptr;

      eptrb = eptrb->prev;    /* Back up the stack of bracket start pointers */

      if (*prev == OP_ASSERT || *prev == OP_ASSERT_NOT ||
          *prev == OP_ASSERTBACK || *prev == OP_ASSERTBACK_NOT ||
          *prev == OP_ONCE)
        {
        md->end_match_ptr = eptr;      /* For ONCE */
        md->end_offset_top = offset_top;
        return TRUE;
        }

      /* In all other cases except a conditional group we have to check the
      group number back at the start and if necessary complete handling an
      extraction by setting the offsets and bumping the high water mark. */

      if (*prev != OP_COND)
        {
        int offset;
        int number = *prev - OP_BRA;

        /* For extended extraction brackets (large number), we have to fish out
        the number from a dummy opcode at the start. */

        if (number > EXTRACT_BASIC_MAX) number = GET2(prev, 2+LINK_SIZE);
        offset = number << 1;

#ifdef DEBUG
        printf("end bracket %d", number);
        printf("\n");
#endif

        /* Test for a numbered group. This includes groups called as a result
        of recursion. Note that whole-pattern recursion is coded as a recurse
        into group 0, so it won't be picked up here. Instead, we catch it when
        the OP_END is reached. */

        if (number > 0)
          {
          md->capture_last = number;
          if (offset >= md->offset_max) md->offset_overflow = TRUE; else
            {
            md->offset_vector[offset] =
              md->offset_vector[md->offset_end - number];
            md->offset_vector[offset+1] = eptr - md->start_subject;
            if (offset_top <= offset) offset_top = offset + 2;
            }

          /* Handle a recursively called group. Restore the offsets
          appropriately and continue from after the call. */

          if (md->recursive != NULL && md->recursive->group_num == number)
            {
            recursion_info *rec = md->recursive;
            DPRINTF(("Recursion (%d) succeeded - continuing\n", number));
            md->recursive = rec->prev;
            md->start_match = rec->save_start;
            memcpy(md->offset_vector, rec->offset_save,
              rec->saved_max * sizeof(int));
            ecode = rec->after_call;
            ims = original_ims;
            break;
            }
          }
        }

      /* Reset the value of the ims flags, in case they got changed during
      the group. */

      ims = original_ims;
      DPRINTF(("ims reset to %02lx\n", ims));

      /* For a non-repeating ket, just continue at this level. This also
      happens for a repeating ket if no characters were matched in the group.
      This is the forcible breaking of infinite loops as implemented in Perl
      5.005. If there is an options reset, it will get obeyed in the normal
      course of events. */

      if (*ecode == OP_KET || eptr == saved_eptr)
        {
        ecode += 1 + LINK_SIZE;
        break;
        }

      /* The repeating kets try the rest of the pattern or restart from the
      preceding bracket, in the appropriate order. */

      if (*ecode == OP_KETRMIN)
        {
        if (match(eptr, ecode + 1+LINK_SIZE, offset_top, md, ims, eptrb, 0) ||
            match(eptr, prev, offset_top, md, ims, eptrb, match_isgroup))
              return TRUE;
        }
      else  /* OP_KETRMAX */
        {
        if (match(eptr, prev, offset_top, md, ims, eptrb, match_isgroup) ||
            match(eptr, ecode + 1+LINK_SIZE, offset_top, md, ims, eptrb, 0))
          return TRUE;
        }
      }
    return FALSE;

    /* Start of subject unless notbol, or after internal newline if multiline */

    case OP_CIRC:
    if (md->notbol && eptr == md->start_subject) return FALSE;
    if ((ims & PCRE_MULTILINE) != 0)
      {
      if (eptr != md->start_subject && eptr[-1] != NEWLINE) return FALSE;
      ecode++;
      break;
      }
    /* ... else fall through */

    /* Start of subject assertion */

    case OP_SOD:
    if (eptr != md->start_subject) return FALSE;
    ecode++;
    break;

    /* Start of match assertion */

    case OP_SOM:
    if (eptr != md->start_subject + md->start_offset) return FALSE;
    ecode++;
    break;

    /* Assert before internal newline if multiline, or before a terminating
    newline unless endonly is set, else end of subject unless noteol is set. */

    case OP_DOLL:
    if ((ims & PCRE_MULTILINE) != 0)
      {
      if (eptr < md->end_subject) { if (*eptr != NEWLINE) return FALSE; }
        else { if (md->noteol) return FALSE; }
      ecode++;
      break;
      }
    else
      {
      if (md->noteol) return FALSE;
      if (!md->endonly)
        {
        if (eptr < md->end_subject - 1 ||
           (eptr == md->end_subject - 1 && *eptr != NEWLINE)) return FALSE;

        ecode++;
        break;
        }
      }
    /* ... else fall through */

    /* End of subject assertion (\z) */

    case OP_EOD:
    if (eptr < md->end_subject) return FALSE;
    ecode++;
    break;

    /* End of subject or ending \n assertion (\Z) */

    case OP_EODN:
    if (eptr < md->end_subject - 1 ||
       (eptr == md->end_subject - 1 && *eptr != NEWLINE)) return FALSE;
    ecode++;
    break;

    /* Word boundary assertions */

    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
      {
      BOOL prev_is_word = (eptr != md->start_subject) &&
        ((md->ctypes[eptr[-1]] & ctype_word) != 0);
      BOOL cur_is_word = (eptr < md->end_subject) &&
        ((md->ctypes[*eptr] & ctype_word) != 0);
      if ((*ecode++ == OP_WORD_BOUNDARY)?
           cur_is_word == prev_is_word : cur_is_word != prev_is_word)
        return FALSE;
      }
    break;

    /* Match a single character type; inline for speed */

    case OP_ANY:
    if ((ims & PCRE_DOTALL) == 0 && eptr < md->end_subject && *eptr == NEWLINE)
      return FALSE;
    if (eptr++ >= md->end_subject) return FALSE;
#ifdef SUPPORT_UTF8
    if (md->utf8)
      while (eptr < md->end_subject && (*eptr & 0xc0) == 0x80) eptr++;
#endif
    ecode++;
    break;

    /* Match a single byte, even in UTF-8 mode. This opcode really does match
    any byte, even newline, independent of the setting of PCRE_DOTALL. */

    case OP_ANYBYTE:
    if (eptr++ >= md->end_subject) return FALSE;
    ecode++;
    break;

    case OP_NOT_DIGIT:
    if (eptr >= md->end_subject ||
       (md->ctypes[*eptr++] & ctype_digit) != 0)
      return FALSE;
    ecode++;
    break;

    case OP_DIGIT:
    if (eptr >= md->end_subject ||
       (md->ctypes[*eptr++] & ctype_digit) == 0)
      return FALSE;
    ecode++;
    break;

    case OP_NOT_WHITESPACE:
    if (eptr >= md->end_subject ||
       (md->ctypes[*eptr++] & ctype_space) != 0)
      return FALSE;
    ecode++;
    break;

    case OP_WHITESPACE:
    if (eptr >= md->end_subject ||
       (md->ctypes[*eptr++] & ctype_space) == 0)
      return FALSE;
    ecode++;
    break;

    case OP_NOT_WORDCHAR:
    if (eptr >= md->end_subject ||
       (md->ctypes[*eptr++] & ctype_word) != 0)
      return FALSE;
    ecode++;
    break;

    case OP_WORDCHAR:
    if (eptr >= md->end_subject ||
       (md->ctypes[*eptr++] & ctype_word) == 0)
      return FALSE;
    ecode++;
    break;

    /* Match a back reference, possibly repeatedly. Look past the end of the
    item to see if there is repeat information following. The code is similar
    to that for character classes, but repeated for efficiency. Then obey
    similar code to character type repeats - written out again for speed.
    However, if the referenced string is the empty string, always treat
    it as matched, any number of times (otherwise there could be infinite
    loops). */

    case OP_REF:
      {
      int length;
      int offset = GET2(ecode, 1) << 1;               /* Doubled ref number */
      ecode += 3;                                     /* Advance past item */

      /* If the reference is unset, set the length to be longer than the amount
      of subject left; this ensures that every attempt at a match fails. We
      can't just fail here, because of the possibility of quantifiers with zero
      minima. */

      length = (offset >= offset_top || md->offset_vector[offset] < 0)?
        md->end_subject - eptr + 1 :
        md->offset_vector[offset+1] - md->offset_vector[offset];

      /* Set up for repetition, or handle the non-repeated case */

      switch (*ecode)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        c = *ecode++ - OP_CRSTAR;
        minimize = (c & 1) != 0;
        min = rep_min[c];                 /* Pick up values from tables; */
        max = rep_max[c];                 /* zero for max => infinity */
        if (max == 0) max = INT_MAX;
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        minimize = (*ecode == OP_CRMINRANGE);
        min = GET2(ecode, 1);
        max = GET2(ecode, 3);
        if (max == 0) max = INT_MAX;
        ecode += 5;
        break;

        default:               /* No repeat follows */
        if (!match_ref(offset, eptr, length, md, ims)) return FALSE;
        eptr += length;
        continue;              /* With the main loop */
        }

      /* If the length of the reference is zero, just continue with the
      main loop. */

      if (length == 0) continue;

      /* First, ensure the minimum number of matches are present. We get back
      the length of the reference string explicitly rather than passing the
      address of eptr, so that eptr can be a register variable. */

      for (i = 1; i <= min; i++)
        {
        if (!match_ref(offset, eptr, length, md, ims)) return FALSE;
        eptr += length;
        }

      /* If min = max, continue at the same level without recursion.
      They are not both allowed to be zero. */

      if (min == max) continue;

      /* If minimizing, keep trying and advancing the pointer */

      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          if (i >= max || !match_ref(offset, eptr, length, md, ims))
            return FALSE;
          eptr += length;
          }
        /* Control never gets here */
        }

      /* If maximizing, find the longest string and work backwards */

      else
        {
        const uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (!match_ref(offset, eptr, length, md, ims)) break;
          eptr += length;
          }
        while (eptr >= pp)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          eptr -= length;
          }
        return FALSE;
        }
      }
    /* Control never gets here */



    /* Match a character class, possibly repeatedly. Look past the end of the
    item to see if there is repeat information following. Then obey similar
    code to character type repeats - written out again for speed. */

    case OP_CLASS:
      {
      const uschar *data = ecode + 1;  /* Save for matching */
      ecode += 33;                     /* Advance past the item */

      switch (*ecode)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        c = *ecode++ - OP_CRSTAR;
        minimize = (c & 1) != 0;
        min = rep_min[c];                 /* Pick up values from tables; */
        max = rep_max[c];                 /* zero for max => infinity */
        if (max == 0) max = INT_MAX;
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        minimize = (*ecode == OP_CRMINRANGE);
        min = GET2(ecode, 1);
        max = GET2(ecode, 3);
        if (max == 0) max = INT_MAX;
        ecode += 5;
        break;

        default:               /* No repeat follows */
        min = max = 1;
        break;
        }

      /* First, ensure the minimum number of matches are present. */

      for (i = 1; i <= min; i++)
        {
        if (eptr >= md->end_subject) return FALSE;
        GETCHARINC(c, eptr)         /* Get character; increment eptr */

#ifdef SUPPORT_UTF8
        /* We do not yet support class members > 255 */
        if (c > 255) return FALSE;
#endif

        if ((data[c/8] & (1 << (c&7))) != 0) continue;
        return FALSE;
        }

      /* If max == min we can continue with the main loop without the
      need to recurse. */

      if (min == max) continue;

      /* If minimizing, keep testing the rest of the expression and advancing
      the pointer while it matches the class. */

      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          if (i >= max || eptr >= md->end_subject) return FALSE;
          GETCHARINC(c, eptr)       /* Get character; increment eptr */

#ifdef SUPPORT_UTF8
          /* We do not yet support class members > 255 */
          if (c > 255) return FALSE;
#endif
          if ((data[c/8] & (1 << (c&7))) != 0) continue;
          return FALSE;
          }
        /* Control never gets here */
        }

      /* If maximizing, find the longest possible run, then work backwards. */

      else
        {
        const uschar *pp = eptr;
        int len = 1;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject) break;
          GETCHARLEN(c, eptr, len)  /* Get character, set length if UTF-8 */

#ifdef SUPPORT_UTF8
          /* We do not yet support class members > 255 */
          if (c > 255) break;
#endif
          if ((data[c/8] & (1 << (c&7))) == 0) break;
          eptr += len;
          }

        while (eptr >= pp)
          {
          if (match(eptr--, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;

#ifdef SUPPORT_UTF8
          BACKCHAR(eptr)
#endif
          }
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a run of characters */

    case OP_CHARS:
      {
      register int length = ecode[1];
      ecode += 2;

#ifdef DEBUG    /* Sigh. Some compilers never learn. */
      if (eptr >= md->end_subject)
        printf("matching subject <null> against pattern ");
      else
        {
        printf("matching subject ");
        pchars(eptr, length, TRUE, md);
        printf(" against pattern ");
        }
      pchars(ecode, length, FALSE, md);
      printf("\n");
#endif

      if (length > md->end_subject - eptr) return FALSE;
      if ((ims & PCRE_CASELESS) != 0)
        {
        while (length-- > 0)
          if (md->lcc[*ecode++] != md->lcc[*eptr++])
            return FALSE;
        }
      else
        {
        while (length-- > 0) if (*ecode++ != *eptr++) return FALSE;
        }
      }
    break;

    /* Match a single character repeatedly; different opcodes share code. */

    case OP_EXACT:
    min = max = GET2(ecode, 1);
    ecode += 3;
    goto REPEATCHAR;

    case OP_UPTO:
    case OP_MINUPTO:
    min = 0;
    max = GET2(ecode, 1);
    minimize = *ecode == OP_MINUPTO;
    ecode += 3;
    goto REPEATCHAR;

    case OP_STAR:
    case OP_MINSTAR:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_QUERY:
    case OP_MINQUERY:
    c = *ecode++ - OP_STAR;
    minimize = (c & 1) != 0;
    min = rep_min[c];                 /* Pick up values from tables; */
    max = rep_max[c];                 /* zero for max => infinity */
    if (max == 0) max = INT_MAX;

    /* Common code for all repeated single-character matches. We can give
    up quickly if there are fewer than the minimum number of characters left in
    the subject. */

    REPEATCHAR:
    if (min > md->end_subject - eptr) return FALSE;
    c = *ecode++;

    /* The code is duplicated for the caseless and caseful cases, for speed,
    since matching characters is likely to be quite common. First, ensure the
    minimum number of matches are present. If min = max, continue at the same
    level without recursing. Otherwise, if minimizing, keep trying the rest of
    the expression and advancing one matching character if failing, up to the
    maximum. Alternatively, if maximizing, find the maximum number of
    characters and work backwards. */

    DPRINTF(("matching %c{%d,%d} against subject %.*s\n", c, min, max,
      max, eptr));

    if ((ims & PCRE_CASELESS) != 0)
      {
      c = md->lcc[c];
      for (i = 1; i <= min; i++)
        if (c != md->lcc[*eptr++]) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          if (i >= max || eptr >= md->end_subject ||
              c != md->lcc[*eptr++])
            return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        const uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c != md->lcc[*eptr]) break;
          eptr++;
          }
        while (eptr >= pp)
          if (match(eptr--, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
        return FALSE;
        }
      /* Control never gets here */
      }

    /* Caseful comparisons */

    else
      {
      for (i = 1; i <= min; i++) if (c != *eptr++) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          if (i >= max || eptr >= md->end_subject || c != *eptr++) return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        const uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c != *eptr) break;
          eptr++;
          }
        while (eptr >= pp)
         if (match(eptr--, ecode, offset_top, md, ims, eptrb, 0))
           return TRUE;
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a negated single character */

    case OP_NOT:
    if (eptr >= md->end_subject) return FALSE;
    ecode++;
    if ((ims & PCRE_CASELESS) != 0)
      {
      if (md->lcc[*ecode++] == md->lcc[*eptr++]) return FALSE;
      }
    else
      {
      if (*ecode++ == *eptr++) return FALSE;
      }
    break;

    /* Match a negated single character repeatedly. This is almost a repeat of
    the code for a repeated single character, but I haven't found a nice way of
    commoning these up that doesn't require a test of the positive/negative
    option for each character match. Maybe that wouldn't add very much to the
    time taken, but character matching *is* what this is all about... */

    case OP_NOTEXACT:
    min = max = GET2(ecode, 1);
    ecode += 3;
    goto REPEATNOTCHAR;

    case OP_NOTUPTO:
    case OP_NOTMINUPTO:
    min = 0;
    max = GET2(ecode, 1);
    minimize = *ecode == OP_NOTMINUPTO;
    ecode += 3;
    goto REPEATNOTCHAR;

    case OP_NOTSTAR:
    case OP_NOTMINSTAR:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTQUERY:
    case OP_NOTMINQUERY:
    c = *ecode++ - OP_NOTSTAR;
    minimize = (c & 1) != 0;
    min = rep_min[c];                 /* Pick up values from tables; */
    max = rep_max[c];                 /* zero for max => infinity */
    if (max == 0) max = INT_MAX;

    /* Common code for all repeated single-character matches. We can give
    up quickly if there are fewer than the minimum number of characters left in
    the subject. */

    REPEATNOTCHAR:
    if (min > md->end_subject - eptr) return FALSE;
    c = *ecode++;

    /* The code is duplicated for the caseless and caseful cases, for speed,
    since matching characters is likely to be quite common. First, ensure the
    minimum number of matches are present. If min = max, continue at the same
    level without recursing. Otherwise, if minimizing, keep trying the rest of
    the expression and advancing one matching character if failing, up to the
    maximum. Alternatively, if maximizing, find the maximum number of
    characters and work backwards. */

    DPRINTF(("negative matching %c{%d,%d} against subject %.*s\n", c, min, max,
      max, eptr));

    if ((ims & PCRE_CASELESS) != 0)
      {
      c = md->lcc[c];
      for (i = 1; i <= min; i++)
        if (c == md->lcc[*eptr++]) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          if (i >= max || eptr >= md->end_subject ||
              c == md->lcc[*eptr++])
            return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        const uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c == md->lcc[*eptr]) break;
          eptr++;
          }
        while (eptr >= pp)
          if (match(eptr--, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
        return FALSE;
        }
      /* Control never gets here */
      }

    /* Caseful comparisons */

    else
      {
      for (i = 1; i <= min; i++) if (c == *eptr++) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md, ims, eptrb, 0))
            return TRUE;
          if (i >= max || eptr >= md->end_subject || c == *eptr++) return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        const uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c == *eptr) break;
          eptr++;
          }
        while (eptr >= pp)
         if (match(eptr--, ecode, offset_top, md, ims, eptrb, 0))
           return TRUE;
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a single character type repeatedly; several different opcodes
    share code. This is very similar to the code for single characters, but we
    repeat it in the interests of efficiency. */

    case OP_TYPEEXACT:
    min = max = GET2(ecode, 1);
    minimize = TRUE;
    ecode += 3;
    goto REPEATTYPE;

    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    min = 0;
    max = GET2(ecode, 1);
    minimize = *ecode == OP_TYPEMINUPTO;
    ecode += 3;
    goto REPEATTYPE;

    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    c = *ecode++ - OP_TYPESTAR;
    minimize = (c & 1) != 0;
    min = rep_min[c];                 /* Pick up values from tables; */
    max = rep_max[c];                 /* zero for max => infinity */
    if (max == 0) max = INT_MAX;

    /* Common code for all repeated single character type matches */

    REPEATTYPE:
    ctype = *ecode++;      /* Code for the character type */

    /* First, ensure the minimum number of matches are present. Use inline
    code for maximizing the speed, and do the type test once at the start
    (i.e. keep it out of the loop). Also we can test that there are at least
    the minimum number of bytes before we start, except when doing '.' in
    UTF8 mode. Leave the test in in all cases; in the special case we have
    to test after each character. */

    if (min > md->end_subject - eptr) return FALSE;
    if (min > 0) switch(ctype)
      {
      case OP_ANY:
#ifdef SUPPORT_UTF8
      if (md->utf8)
        {
        for (i = 1; i <= min; i++)
          {
          if (eptr >= md->end_subject ||
             (*eptr++ == NEWLINE && (ims & PCRE_DOTALL) == 0))
            return FALSE;
          while (eptr < md->end_subject && (*eptr & 0xc0) == 0x80) eptr++;
          }
        break;
        }
#endif
      /* Non-UTF8 can be faster */
      if ((ims & PCRE_DOTALL) == 0)
        { for (i = 1; i <= min; i++) if (*eptr++ == NEWLINE) return FALSE; }
      else eptr += min;
      break;

      case OP_ANYBYTE:
      eptr += min;
      break;

      case OP_NOT_DIGIT:
      for (i = 1; i <= min; i++)
        if ((md->ctypes[*eptr++] & ctype_digit) != 0) return FALSE;
      break;

      case OP_DIGIT:
      for (i = 1; i <= min; i++)
        if ((md->ctypes[*eptr++] & ctype_digit) == 0) return FALSE;
      break;

      case OP_NOT_WHITESPACE:
      for (i = 1; i <= min; i++)
        if ((md->ctypes[*eptr++] & ctype_space) != 0) return FALSE;
      break;

      case OP_WHITESPACE:
      for (i = 1; i <= min; i++)
        if ((md->ctypes[*eptr++] & ctype_space) == 0) return FALSE;
      break;

      case OP_NOT_WORDCHAR:
      for (i = 1; i <= min; i++)
        if ((md->ctypes[*eptr++] & ctype_word) != 0)
          return FALSE;
      break;

      case OP_WORDCHAR:
      for (i = 1; i <= min; i++)
        if ((md->ctypes[*eptr++] & ctype_word) == 0)
          return FALSE;
      break;
      }

    /* If min = max, continue at the same level without recursing */

    if (min == max) continue;

    /* If minimizing, we have to test the rest of the pattern before each
    subsequent match. */

    if (minimize)
      {
      for (i = min;; i++)
        {
        if (match(eptr, ecode, offset_top, md, ims, eptrb, 0)) return TRUE;
        if (i >= max || eptr >= md->end_subject) return FALSE;

        c = *eptr++;
        switch(ctype)
          {
          case OP_ANY:
          if ((ims & PCRE_DOTALL) == 0 && c == NEWLINE) return FALSE;
#ifdef SUPPORT_UTF8
          if (md->utf8)
            while (eptr < md->end_subject && (*eptr & 0xc0) == 0x80) eptr++;
#endif
          break;

          case OP_ANYBYTE:
          break;

          case OP_NOT_DIGIT:
          if ((md->ctypes[c] & ctype_digit) != 0) return FALSE;
          break;

          case OP_DIGIT:
          if ((md->ctypes[c] & ctype_digit) == 0) return FALSE;
          break;

          case OP_NOT_WHITESPACE:
          if ((md->ctypes[c] & ctype_space) != 0) return FALSE;
          break;

          case OP_WHITESPACE:
          if  ((md->ctypes[c] & ctype_space) == 0) return FALSE;
          break;

          case OP_NOT_WORDCHAR:
          if ((md->ctypes[c] & ctype_word) != 0) return FALSE;
          break;

          case OP_WORDCHAR:
          if ((md->ctypes[c] & ctype_word) == 0) return FALSE;
          break;
          }
        }
      /* Control never gets here */
      }

    /* If maximizing it is worth using inline code for speed, doing the type
    test once at the start (i.e. keep it out of the loop). */

    else
      {
      const uschar *pp = eptr;
      switch(ctype)
        {
        case OP_ANY:

        /* Special code is required for UTF8, but when the maximum is unlimited
        we don't need it. */

#ifdef SUPPORT_UTF8
        if (md->utf8 && max < INT_MAX)
          {
          if ((ims & PCRE_DOTALL) == 0)
            {
            for (i = min; i < max; i++)
              {
              if (eptr >= md->end_subject || *eptr++ == NEWLINE) break;
              while (eptr < md->end_subject && (*eptr & 0xc0) == 0x80) eptr++;
              }
            }
          else
            {
            for (i = min; i < max; i++)
              {
              eptr++;
              while (eptr < md->end_subject && (*eptr & 0xc0) == 0x80) eptr++;
              }
            }
          break;
          }
#endif
        /* Non-UTF8 can be faster */
        if ((ims & PCRE_DOTALL) == 0)
          {
          for (i = min; i < max; i++)
            {
            if (eptr >= md->end_subject || *eptr == NEWLINE) break;
            eptr++;
            }
          break;
          }
        /* For non-UTF8 DOTALL case, fall through and treat as \C */

        case OP_ANYBYTE:
        c = max - min;
        if (c > md->end_subject - eptr) c = md->end_subject - eptr;
        eptr += c;
        break;

        case OP_NOT_DIGIT:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (md->ctypes[*eptr] & ctype_digit) != 0)
            break;
          eptr++;
          }
        break;

        case OP_DIGIT:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (md->ctypes[*eptr] & ctype_digit) == 0)
            break;
          eptr++;
          }
        break;

        case OP_NOT_WHITESPACE:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (md->ctypes[*eptr] & ctype_space) != 0)
            break;
          eptr++;
          }
        break;

        case OP_WHITESPACE:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (md->ctypes[*eptr] & ctype_space) == 0)
            break;
          eptr++;
          }
        break;

        case OP_NOT_WORDCHAR:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (md->ctypes[*eptr] & ctype_word) != 0)
            break;
          eptr++;
          }
        break;

        case OP_WORDCHAR:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (md->ctypes[*eptr] & ctype_word) == 0)
            break;
          eptr++;
          }
        break;
        }

      while (eptr >= pp)
        {
        if (match(eptr--, ecode, offset_top, md, ims, eptrb, 0))
          return TRUE;
#ifdef SUPPORT_UTF8
        if (md->utf8)
          while (eptr > pp && (*eptr & 0xc0) == 0x80) eptr--;
#endif
        }
      return FALSE;
      }
    /* Control never gets here */

    /* There's been some horrible disaster. */

    default:
    DPRINTF(("Unknown opcode %d\n", *ecode));
    md->errorcode = PCRE_ERROR_UNKNOWN_NODE;
    return FALSE;
    }

  /* Do not stick any code in here without much thought; it is assumed
  that "continue" in the code above comes out to here to repeat the main
  loop. */

  }             /* End of main loop */
/* Control never reaches here */
}




/*************************************************
*         Execute a Regular Expression           *
*************************************************/

/* This function applies a compiled re to a subject string and picks out
portions of the string if it matches. Two elements in the vector are set for
each substring: the offsets to the start and end of the substring.

Arguments:
  external_re     points to the compiled expression
  external_extra  points to "hints" from pcre_study() or is NULL
  subject         points to the subject string
  length          length of subject string (may contain binary zeros)
  start_offset    where to start in the subject string
  options         option bits
  offsets         points to a vector of ints to be filled in with offsets
  offsetcount     the number of elements in the vector

Returns:          > 0 => success; value is the number of elements filled in
                  = 0 => success, but offsets is not big enough
                   -1 => failed to match
                 < -1 => some kind of unexpected problem
*/

int
pcre_exec(const pcre *external_re, const pcre_extra *external_extra,
  const char *subject, int length, int start_offset, int options, int *offsets,
  int offsetcount)
{
int resetcount, ocount;
int first_char = -1;
int req_char = -1;
int req_char2 = -1;
unsigned long int ims = 0;
match_data match_block;
const uschar *start_bits = NULL;
const uschar *start_match = (const uschar *)subject + start_offset;
const uschar *end_subject;
const uschar *req_char_ptr = start_match - 1;
const real_pcre *re = (const real_pcre *)external_re;
const real_pcre_extra *extra = (const real_pcre_extra *)external_extra;
const uschar *codestart =
  (const uschar *)re + sizeof(real_pcre) + re->name_count * re->name_entry_size;
BOOL using_temporary_offsets = FALSE;
BOOL anchored;
BOOL startline;
BOOL first_char_caseless = FALSE;
BOOL req_char_caseless = FALSE;

if ((options & ~PUBLIC_EXEC_OPTIONS) != 0) return PCRE_ERROR_BADOPTION;

if (re == NULL || subject == NULL ||
   (offsets == NULL && offsetcount > 0)) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;

anchored = ((re->options | options) & PCRE_ANCHORED) != 0;
startline = (re->options & PCRE_STARTLINE) != 0;

match_block.start_code = codestart;
match_block.start_subject = (const uschar *)subject;
match_block.start_offset = start_offset;
match_block.end_subject = match_block.start_subject + length;
end_subject = match_block.end_subject;

match_block.endonly = (re->options & PCRE_DOLLAR_ENDONLY) != 0;
match_block.utf8 = (re->options & PCRE_UTF8) != 0;

match_block.notbol = (options & PCRE_NOTBOL) != 0;
match_block.noteol = (options & PCRE_NOTEOL) != 0;
match_block.notempty = (options & PCRE_NOTEMPTY) != 0;

match_block.errorcode = PCRE_ERROR_NOMATCH;     /* Default error */
match_block.recursive = NULL;                   /* No recursion */

match_block.lcc = re->tables + lcc_offset;
match_block.ctypes = re->tables + ctypes_offset;

/* The ims options can vary during the matching as a result of the presence
of (?ims) items in the pattern. They are kept in a local variable so that
restoring at the exit of a group is easy. */

ims = re->options & (PCRE_CASELESS|PCRE_MULTILINE|PCRE_DOTALL);

/* If the expression has got more back references than the offsets supplied can
hold, we get a temporary bit of working store to use during the matching.
Otherwise, we can use the vector supplied, rounding down its size to a multiple
of 3. */

ocount = offsetcount - (offsetcount % 3);

if (re->top_backref > 0 && re->top_backref >= ocount/3)
  {
  ocount = re->top_backref * 3 + 3;
  match_block.offset_vector = (int *)(pcre_malloc)(ocount * sizeof(int));
  if (match_block.offset_vector == NULL) return PCRE_ERROR_NOMEMORY;
  using_temporary_offsets = TRUE;
  DPRINTF(("Got memory to hold back references\n"));
  }
else match_block.offset_vector = offsets;

match_block.offset_end = ocount;
match_block.offset_max = (2*ocount)/3;
match_block.offset_overflow = FALSE;
match_block.capture_last = -1;

/* Compute the minimum number of offsets that we need to reset each time. Doing
this makes a huge difference to execution time when there aren't many brackets
in the pattern. */

resetcount = 2 + re->top_bracket * 2;
if (resetcount > offsetcount) resetcount = ocount;

/* Reset the working variable associated with each extraction. These should
never be used unless previously set, but they get saved and restored, and so we
initialize them to avoid reading uninitialized locations. */

if (match_block.offset_vector != NULL)
  {
  register int *iptr = match_block.offset_vector + ocount;
  register int *iend = iptr - resetcount/2 + 1;
  while (--iptr >= iend) *iptr = -1;
  }

/* Set up the first character to match, if available. The first_char value is
never set for an anchored regular expression, but the anchoring may be forced
at run time, so we have to test for anchoring. The first char may be unset for
an unanchored pattern, of course. If there's no first char and the pattern was
studied, there may be a bitmap of possible first characters. */

if (!anchored)
  {
  if ((re->options & PCRE_FIRSTSET) != 0)
    {
    first_char = re->first_char & 255;
    if ((first_char_caseless = ((re->first_char & REQ_CASELESS) != 0)) == TRUE)
      first_char = match_block.lcc[first_char];
    }
  else
    if (!startline && extra != NULL &&
      (extra->options & PCRE_STUDY_MAPPED) != 0)
        start_bits = extra->start_bits;
  }

/* For anchored or unanchored matches, there may be a "last known required
character" set. */

if ((re->options & PCRE_REQCHSET) != 0)
  {
  req_char = re->req_char & 255;
  req_char_caseless = (re->req_char & REQ_CASELESS) != 0;
  req_char2 = (re->tables + fcc_offset)[req_char];  /* case flipped */
  }

/* Loop for handling unanchored repeated matching attempts; for anchored regexs
the loop runs just once. */

do
  {
  int rc;
  register int *iptr = match_block.offset_vector;
  register int *iend = iptr + resetcount;

  /* Reset the maximum number of extractions we might see. */

  while (iptr < iend) *iptr++ = -1;

  /* Advance to a unique first char if possible */

  if (first_char >= 0)
    {
    if (first_char_caseless)
      while (start_match < end_subject &&
             match_block.lcc[*start_match] != first_char)
        start_match++;
    else
      while (start_match < end_subject && *start_match != first_char)
        start_match++;
    }

  /* Or to just after \n for a multiline match if possible */

  else if (startline)
    {
    if (start_match > match_block.start_subject + start_offset)
      {
      while (start_match < end_subject && start_match[-1] != NEWLINE)
        start_match++;
      }
    }

  /* Or to a non-unique first char after study */

  else if (start_bits != NULL)
    {
    while (start_match < end_subject)
      {
      register int c = *start_match;
      if ((start_bits[c/8] & (1 << (c&7))) == 0) start_match++; else break;
      }
    }

#ifdef DEBUG  /* Sigh. Some compilers never learn. */
  printf(">>>> Match against: ");
  pchars(start_match, end_subject - start_match, TRUE, &match_block);
  printf("\n");
#endif

  /* If req_char is set, we know that that character must appear in the subject
  for the match to succeed. If the first character is set, req_char must be
  later in the subject; otherwise the test starts at the match point. This
  optimization can save a huge amount of backtracking in patterns with nested
  unlimited repeats that aren't going to match. Writing separate code for
  cased/caseless versions makes it go faster, as does using an autoincrement
  and backing off on a match. */

  if (req_char >= 0)
    {
    register const uschar *p = start_match + ((first_char >= 0)? 1 : 0);

    /* We don't need to repeat the search if we haven't yet reached the
    place we found it at last time. */

    if (p > req_char_ptr)
      {
      if (req_char_caseless)
        {
        while (p < end_subject)
          {
          register int pp = *p++;
          if (pp == req_char || pp == req_char2) { p--; break; }
          }
        }
      else
        {
        while (p < end_subject)
          {
          if (*p++ == req_char) { p--; break; }
          }
        }

      /* If we can't find the required character, break the matching loop */

      if (p >= end_subject) break;

      /* If we have found the required character, save the point where we
      found it, so that we don't search again next time round the loop if
      the start hasn't passed this character yet. */

      req_char_ptr = p;
      }
    }

  /* When a match occurs, substrings will be set for all internal extractions;
  we just need to set up the whole thing as substring 0 before returning. If
  there were too many extractions, set the return code to zero. In the case
  where we had to get some local store to hold offsets for backreferences, copy
  those back references that we can. In this case there need not be overflow
  if certain parts of the pattern were not used. */

  match_block.start_match = start_match;
  if (!match(start_match, codestart, 2, &match_block, ims, NULL, match_isgroup))
    continue;

  /* Copy the offset information from temporary store if necessary */

  if (using_temporary_offsets)
    {
    if (offsetcount >= 4)
      {
      memcpy(offsets + 2, match_block.offset_vector + 2,
        (offsetcount - 2) * sizeof(int));
      DPRINTF(("Copied offsets from temporary memory\n"));
      }
    if (match_block.end_offset_top > offsetcount)
      match_block.offset_overflow = TRUE;

    DPRINTF(("Freeing temporary memory\n"));
    (pcre_free)(match_block.offset_vector);
    }

  rc = match_block.offset_overflow? 0 : match_block.end_offset_top/2;

  if (offsetcount < 2) rc = 0; else
    {
    offsets[0] = start_match - match_block.start_subject;
    offsets[1] = match_block.end_match_ptr - match_block.start_subject;
    }

  DPRINTF((">>>> returning %d\n", rc));
  return rc;
  }

/* This "while" is the end of the "do" above */

while (!anchored &&
       match_block.errorcode == PCRE_ERROR_NOMATCH &&
       start_match++ < end_subject);

if (using_temporary_offsets)
  {
  DPRINTF(("Freeing temporary memory\n"));
  (pcre_free)(match_block.offset_vector);
  }

DPRINTF((">>>> returning %d\n", match_block.errorcode));

return match_block.errorcode;
}

/* End of pcre.c */
