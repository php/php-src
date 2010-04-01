/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/


/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2010 University of Cambridge

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

/* This header contains definitions that are shared between the different
modules, but which are not relevant to the exported API. This includes some
functions whose names all begin with "_pcre_". */

#ifndef PCRE_INTERNAL_H
#define PCRE_INTERNAL_H

/* Define PCRE_DEBUG to get debugging output on stdout. */

#if 0
#define PCRE_DEBUG
#endif

/* We do not support both EBCDIC and UTF-8 at the same time. The "configure"
script prevents both being selected, but not everybody uses "configure". */

#if defined EBCDIC && defined SUPPORT_UTF8
#error The use of both EBCDIC and SUPPORT_UTF8 is not supported.
#endif

/* If SUPPORT_UCP is defined, SUPPORT_UTF8 must also be defined. The
"configure" script ensures this, but not everybody uses "configure". */

#if defined SUPPORT_UCP && !defined SUPPORT_UTF8
#define SUPPORT_UTF8 1
#endif

/* Use a macro for debugging printing, 'cause that eliminates the use of #ifdef
inline, and there are *still* stupid compilers about that don't like indented
pre-processor statements, or at least there were when I first wrote this. After
all, it had only been about 10 years then...

It turns out that the Mac Debugging.h header also defines the macro DPRINTF, so
be absolutely sure we get our version. */

#undef DPRINTF
#ifdef PCRE_DEBUG
#define DPRINTF(p) printf p
#else
#define DPRINTF(p) /* Nothing */
#endif


/* Standard C headers plus the external interface definition. The only time
setjmp and stdarg are used is when NO_RECURSE is set. */

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* When compiling a DLL for Windows, the exported symbols have to be declared
using some MS magic. I found some useful information on this web page:
http://msdn2.microsoft.com/en-us/library/y4h7bcy6(VS.80).aspx. According to the
information there, using __declspec(dllexport) without "extern" we have a
definition; with "extern" we have a declaration. The settings here override the
setting in pcre.h (which is included below); it defines only PCRE_EXP_DECL,
which is all that is needed for applications (they just import the symbols). We
use:

  PCRE_EXP_DECL       for declarations
  PCRE_EXP_DEFN       for definitions of exported functions
  PCRE_EXP_DATA_DEFN  for definitions of exported variables

The reason for the two DEFN macros is that in non-Windows environments, one
does not want to have "extern" before variable definitions because it leads to
compiler warnings. So we distinguish between functions and variables. In
Windows, the two should always be the same.

The reason for wrapping this in #ifndef PCRE_EXP_DECL is so that pcretest,
which is an application, but needs to import this file in order to "peek" at
internals, can #include pcre.h first to get an application's-eye view.

In principle, people compiling for non-Windows, non-Unix-like (i.e. uncommon,
special-purpose environments) might want to stick other stuff in front of
exported symbols. That's why, in the non-Windows case, we set PCRE_EXP_DEFN and
PCRE_EXP_DATA_DEFN only if they are not already set. */

#ifndef PCRE_EXP_DECL
#  ifdef _WIN32
#    ifndef PCRE_STATIC
#      define PCRE_EXP_DECL       extern __declspec(dllexport)
#      define PCRE_EXP_DEFN       __declspec(dllexport)
#      define PCRE_EXP_DATA_DEFN  __declspec(dllexport)
#    else
#      define PCRE_EXP_DECL       extern
#      define PCRE_EXP_DEFN
#      define PCRE_EXP_DATA_DEFN
#    endif
#  else
#    ifdef __cplusplus
#      define PCRE_EXP_DECL       extern "C"
#    else
#      define PCRE_EXP_DECL       extern
#    endif
#    ifndef PCRE_EXP_DEFN
#      define PCRE_EXP_DEFN       PCRE_EXP_DECL
#    endif
#    ifndef PCRE_EXP_DATA_DEFN
#      define PCRE_EXP_DATA_DEFN
#    endif
#  endif
#endif

/* When compiling with the MSVC compiler, it is sometimes necessary to include
a "calling convention" before exported function names. (This is secondhand
information; I know nothing about MSVC myself). For example, something like

  void __cdecl function(....)

might be needed. In order so make this easy, all the exported functions have
PCRE_CALL_CONVENTION just before their names. It is rarely needed; if not
set, we ensure here that it has no effect. */

#ifndef PCRE_CALL_CONVENTION
#define PCRE_CALL_CONVENTION
#endif

/* We need to have types that specify unsigned 16-bit and 32-bit integers. We
cannot determine these outside the compilation (e.g. by running a program as
part of "configure") because PCRE is often cross-compiled for use on other
systems. Instead we make use of the maximum sizes that are available at
preprocessor time in standard C environments. */

#if USHRT_MAX == 65535
  typedef unsigned short pcre_uint16;
  typedef short pcre_int16;
#elif UINT_MAX == 65535
  typedef unsigned int pcre_uint16;
  typedef int pcre_int16;
#else
  #error Cannot determine a type for 16-bit unsigned integers
#endif

#if UINT_MAX == 4294967295
  typedef unsigned int pcre_uint32;
  typedef int pcre_int32;
#elif ULONG_MAX == 4294967295
  typedef unsigned long int pcre_uint32;
  typedef long int pcre_int32;
#else
  #error Cannot determine a type for 32-bit unsigned integers
#endif

/* When checking for integer overflow in pcre_compile(), we need to handle
large integers. If a 64-bit integer type is available, we can use that.
Otherwise we have to cast to double, which of course requires floating point
arithmetic. Handle this by defining a macro for the appropriate type. If
stdint.h is available, include it; it may define INT64_MAX. Systems that do not
have stdint.h (e.g. Solaris) may have inttypes.h. The macro int64_t may be set
by "configure". */

#ifdef PHP_WIN32
#include "win32/php_stdint.h"
#elif HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#if defined INT64_MAX || defined int64_t
#define INT64_OR_DOUBLE int64_t
#else
#define INT64_OR_DOUBLE double
#endif

/* All character handling must be done as unsigned characters. Otherwise there
are problems with top-bit-set characters and functions such as isspace().
However, we leave the interface to the outside world as char *, because that
should make things easier for callers. We define a short type for unsigned char
to save lots of typing. I tried "uchar", but it causes problems on Digital
Unix, where it is defined in sys/types, so use "uschar" instead. */

typedef unsigned char uschar;

/* This is an unsigned int value that no character can ever have. UTF-8
characters only go up to 0x7fffffff (though Unicode doesn't go beyond
0x0010ffff). */

#define NOTACHAR 0xffffffff

/* PCRE is able to support several different kinds of newline (CR, LF, CRLF,
"any" and "anycrlf" at present). The following macros are used to package up
testing for newlines. NLBLOCK, PSSTART, and PSEND are defined in the various
modules to indicate in which datablock the parameters exist, and what the
start/end of string field names are. */

#define NLTYPE_FIXED    0     /* Newline is a fixed length string */
#define NLTYPE_ANY      1     /* Newline is any Unicode line ending */
#define NLTYPE_ANYCRLF  2     /* Newline is CR, LF, or CRLF */

/* This macro checks for a newline at the given position */

#define IS_NEWLINE(p) \
  ((NLBLOCK->nltype != NLTYPE_FIXED)? \
    ((p) < NLBLOCK->PSEND && \
     _pcre_is_newline((p), NLBLOCK->nltype, NLBLOCK->PSEND, &(NLBLOCK->nllen),\
       utf8)) \
    : \
    ((p) <= NLBLOCK->PSEND - NLBLOCK->nllen && \
     (p)[0] == NLBLOCK->nl[0] && \
     (NLBLOCK->nllen == 1 || (p)[1] == NLBLOCK->nl[1]) \
    ) \
  )

/* This macro checks for a newline immediately preceding the given position */

#define WAS_NEWLINE(p) \
  ((NLBLOCK->nltype != NLTYPE_FIXED)? \
    ((p) > NLBLOCK->PSSTART && \
     _pcre_was_newline((p), NLBLOCK->nltype, NLBLOCK->PSSTART, \
       &(NLBLOCK->nllen), utf8)) \
    : \
    ((p) >= NLBLOCK->PSSTART + NLBLOCK->nllen && \
     (p)[-NLBLOCK->nllen] == NLBLOCK->nl[0] && \
     (NLBLOCK->nllen == 1 || (p)[-NLBLOCK->nllen+1] == NLBLOCK->nl[1]) \
    ) \
  )

/* When PCRE is compiled as a C++ library, the subject pointer can be replaced
with a custom type. This makes it possible, for example, to allow pcre_exec()
to process subject strings that are discontinuous by using a smart pointer
class. It must always be possible to inspect all of the subject string in
pcre_exec() because of the way it backtracks. Two macros are required in the
normal case, for sign-unspecified and unsigned char pointers. The former is
used for the external interface and appears in pcre.h, which is why its name
must begin with PCRE_. */

#ifdef CUSTOM_SUBJECT_PTR
#define PCRE_SPTR CUSTOM_SUBJECT_PTR
#define USPTR CUSTOM_SUBJECT_PTR
#else
#define PCRE_SPTR const char *
#define USPTR const unsigned char *
#endif



/* Include the public PCRE header and the definitions of UCP character property
values. */

#include "pcre.h"
#include "ucp.h"

/* When compiling for use with the Virtual Pascal compiler, these functions
need to have their names changed. PCRE must be compiled with the -DVPCOMPAT
option on the command line. */

#ifdef VPCOMPAT
#define strlen(s)        _strlen(s)
#define strncmp(s1,s2,m) _strncmp(s1,s2,m)
#define memcmp(s,c,n)    _memcmp(s,c,n)
#define memcpy(d,s,n)    _memcpy(d,s,n)
#define memmove(d,s,n)   _memmove(d,s,n)
#define memset(s,c,n)    _memset(s,c,n)
#else  /* VPCOMPAT */

/* To cope with SunOS4 and other systems that lack memmove() but have bcopy(),
define a macro for memmove() if HAVE_MEMMOVE is false, provided that HAVE_BCOPY
is set. Otherwise, include an emulating function for those systems that have
neither (there some non-Unix environments where this is the case). */

#ifndef HAVE_MEMMOVE
#undef  memmove        /* some systems may have a macro */
#ifdef HAVE_BCOPY
#define memmove(a, b, c) bcopy(b, a, c)
#else  /* HAVE_BCOPY */
static void *
pcre_memmove(void *d, const void *s, size_t n)
{
size_t i;
unsigned char *dest = (unsigned char *)d;
const unsigned char *src = (const unsigned char *)s;
if (dest > src)
  {
  dest += n;
  src += n;
  for (i = 0; i < n; ++i) *(--dest) = *(--src);
  return (void *)dest;
  }
else
  {
  for (i = 0; i < n; ++i) *dest++ = *src++;
  return (void *)(dest - n);
  }
}
#define memmove(a, b, c) pcre_memmove(a, b, c)
#endif   /* not HAVE_BCOPY */
#endif   /* not HAVE_MEMMOVE */
#endif   /* not VPCOMPAT */


/* PCRE keeps offsets in its compiled code as 2-byte quantities (always stored
in big-endian order) by default. These are used, for example, to link from the
start of a subpattern to its alternatives and its end. The use of 2 bytes per
offset limits the size of the compiled regex to around 64K, which is big enough
for almost everybody. However, I received a request for an even bigger limit.
For this reason, and also to make the code easier to maintain, the storing and
loading of offsets from the byte string is now handled by the macros that are
defined here.

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


/* When UTF-8 encoding is being used, a character is no longer just a single
byte. The macros for character handling generate simple sequences when used in
byte-mode, and more complicated ones for UTF-8 characters. BACKCHAR should
never be called in byte mode. To make sure it can never even appear when UTF-8
support is omitted, we don't even define it. */

#ifndef SUPPORT_UTF8
#define GETCHAR(c, eptr) c = *eptr;
#define GETCHARTEST(c, eptr) c = *eptr;
#define GETCHARINC(c, eptr) c = *eptr++;
#define GETCHARINCTEST(c, eptr) c = *eptr++;
#define GETCHARLEN(c, eptr, len) c = *eptr;
/* #define BACKCHAR(eptr) */

#else   /* SUPPORT_UTF8 */

/* Get the next UTF-8 character, not advancing the pointer. This is called when
we know we are in UTF-8 mode. */

#define GETCHAR(c, eptr) \
  c = *eptr; \
  if (c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    }

/* Get the next UTF-8 character, testing for UTF-8 mode, and not advancing the
pointer. */

#define GETCHARTEST(c, eptr) \
  c = *eptr; \
  if (utf8 && c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    }

/* Get the next UTF-8 character, advancing the pointer. This is called when we
know we are in UTF-8 mode. */

#define GETCHARINC(c, eptr) \
  c = *eptr++; \
  if (c >= 0xc0) \
    { \
    int gcaa = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    while (gcaa-- > 0) \
      { \
      gcss -= 6; \
      c |= (*eptr++ & 0x3f) << gcss; \
      } \
    }

/* Get the next character, testing for UTF-8 mode, and advancing the pointer */

#define GETCHARINCTEST(c, eptr) \
  c = *eptr++; \
  if (utf8 && c >= 0xc0) \
    { \
    int gcaa = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    while (gcaa-- > 0) \
      { \
      gcss -= 6; \
      c |= (*eptr++ & 0x3f) << gcss; \
      } \
    }

/* Get the next UTF-8 character, not advancing the pointer, incrementing length
if there are extra bytes. This is called when we know we are in UTF-8 mode. */

#define GETCHARLEN(c, eptr, len) \
  c = *eptr; \
  if (c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    len += gcaa; \
    }

/* Get the next UTF-8 character, testing for UTF-8 mode, not advancing the
pointer, incrementing length if there are extra bytes. This is called when we
know we are in UTF-8 mode. */

#define GETCHARLENTEST(c, eptr, len) \
  c = *eptr; \
  if (utf8 && c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    len += gcaa; \
    }

/* If the pointer is not at the start of a character, move it back until
it is. This is called only in UTF-8 mode - we don't put a test within the macro
because almost all calls are already within a block of UTF-8 only code. */

#define BACKCHAR(eptr) while((*eptr & 0xc0) == 0x80) eptr--

#endif


/* In case there is no definition of offsetof() provided - though any proper
Standard C system should have one. */

#ifndef offsetof
#define offsetof(p_type,field) ((size_t)&(((p_type *)0)->field))
#endif


/* These are the public options that can change during matching. */

#define PCRE_IMS (PCRE_CASELESS|PCRE_MULTILINE|PCRE_DOTALL)

/* Private flags containing information about the compiled regex. They used to
live at the top end of the options word, but that got almost full, so now they
are in a 16-bit flags word. From release 8.00, PCRE_NOPARTIAL is unused, as
the restrictions on partial matching have been lifted. It remains for backwards
compatibility. */

#define PCRE_NOPARTIAL     0x0001  /* can't use partial with this regex */
#define PCRE_FIRSTSET      0x0002  /* first_byte is set */
#define PCRE_REQCHSET      0x0004  /* req_byte is set */
#define PCRE_STARTLINE     0x0008  /* start after \n for multiline */
#define PCRE_JCHANGED      0x0010  /* j option used in regex */
#define PCRE_HASCRORLF     0x0020  /* explicit \r or \n in pattern */

/* Options for the "extra" block produced by pcre_study(). */

#define PCRE_STUDY_MAPPED   0x01     /* a map of starting chars exists */
#define PCRE_STUDY_MINLEN   0x02     /* a minimum length field exists */

/* Masks for identifying the public options that are permitted at compile
time, run time, or study time, respectively. */

#define PCRE_NEWLINE_BITS (PCRE_NEWLINE_CR|PCRE_NEWLINE_LF|PCRE_NEWLINE_ANY| \
                           PCRE_NEWLINE_ANYCRLF)

#define PUBLIC_COMPILE_OPTIONS \
  (PCRE_CASELESS|PCRE_EXTENDED|PCRE_ANCHORED|PCRE_MULTILINE| \
   PCRE_DOTALL|PCRE_DOLLAR_ENDONLY|PCRE_EXTRA|PCRE_UNGREEDY|PCRE_UTF8| \
   PCRE_NO_AUTO_CAPTURE|PCRE_NO_UTF8_CHECK|PCRE_AUTO_CALLOUT|PCRE_FIRSTLINE| \
   PCRE_DUPNAMES|PCRE_NEWLINE_BITS|PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE| \
   PCRE_JAVASCRIPT_COMPAT)

#define PUBLIC_EXEC_OPTIONS \
  (PCRE_ANCHORED|PCRE_NOTBOL|PCRE_NOTEOL|PCRE_NOTEMPTY|PCRE_NOTEMPTY_ATSTART| \
   PCRE_NO_UTF8_CHECK|PCRE_PARTIAL_HARD|PCRE_PARTIAL_SOFT|PCRE_NEWLINE_BITS| \
   PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE|PCRE_NO_START_OPTIMIZE)

#define PUBLIC_DFA_EXEC_OPTIONS \
  (PCRE_ANCHORED|PCRE_NOTBOL|PCRE_NOTEOL|PCRE_NOTEMPTY|PCRE_NOTEMPTY_ATSTART| \
   PCRE_NO_UTF8_CHECK|PCRE_PARTIAL_HARD|PCRE_PARTIAL_SOFT|PCRE_DFA_SHORTEST| \
   PCRE_DFA_RESTART|PCRE_NEWLINE_BITS|PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE| \
   PCRE_NO_START_OPTIMIZE)

#define PUBLIC_STUDY_OPTIONS 0   /* None defined */

/* Magic number to provide a small check against being handed junk. Also used
to detect whether a pattern was compiled on a host of different endianness. */

#define MAGIC_NUMBER  0x50435245UL   /* 'PCRE' */

/* Negative values for the firstchar and reqchar variables */

#define REQ_UNSET (-2)
#define REQ_NONE  (-1)

/* The maximum remaining length of subject we are prepared to search for a
req_byte match. */

#define REQ_BYTE_MAX 1000

/* Flags added to firstbyte or reqbyte; a "non-literal" item is either a
variable-length repeat, or a anything other than literal characters. */

#define REQ_CASELESS 0x0100    /* indicates caselessness */
#define REQ_VARY     0x0200    /* reqbyte followed non-literal item */

/* Miscellaneous definitions. The #ifndef is to pacify compiler warnings in
environments where these macros are defined elsewhere. Unfortunately, there
is no way to do the same for the typedef. */

typedef int BOOL;

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

/* If PCRE is to support UTF-8 on EBCDIC platforms, we cannot use normal
character constants like '*' because the compiler would emit their EBCDIC code,
which is different from their ASCII/UTF-8 code. Instead we define macros for
the characters so that they always use the ASCII/UTF-8 code when UTF-8 support
is enabled. When UTF-8 support is not enabled, the definitions use character
literals. Both character and string versions of each character are needed, and
there are some longer strings as well.

This means that, on EBCDIC platforms, the PCRE library can handle either
EBCDIC, or UTF-8, but not both. To support both in the same compiled library
would need different lookups depending on whether PCRE_UTF8 was set or not.
This would make it impossible to use characters in switch/case statements,
which would reduce performance. For a theoretical use (which nobody has asked
for) in a minority area (EBCDIC platforms), this is not sensible. Any
application that did need both could compile two versions of the library, using
macros to give the functions distinct names. */

#ifndef SUPPORT_UTF8

/* UTF-8 support is not enabled; use the platform-dependent character literals
so that PCRE works on both ASCII and EBCDIC platforms, in non-UTF-mode only. */

#define CHAR_HT                     '\t'
#define CHAR_VT                     '\v'
#define CHAR_FF                     '\f'
#define CHAR_CR                     '\r'
#define CHAR_NL                     '\n'
#define CHAR_BS                     '\b'
#define CHAR_BEL                    '\a'
#ifdef EBCDIC
#define CHAR_ESC                    '\047'
#define CHAR_DEL                    '\007'
#else
#define CHAR_ESC                    '\033'
#define CHAR_DEL                    '\177'
#endif

#define CHAR_SPACE                  ' '
#define CHAR_EXCLAMATION_MARK       '!'
#define CHAR_QUOTATION_MARK         '"'
#define CHAR_NUMBER_SIGN            '#'
#define CHAR_DOLLAR_SIGN            '$'
#define CHAR_PERCENT_SIGN           '%'
#define CHAR_AMPERSAND              '&'
#define CHAR_APOSTROPHE             '\''
#define CHAR_LEFT_PARENTHESIS       '('
#define CHAR_RIGHT_PARENTHESIS      ')'
#define CHAR_ASTERISK               '*'
#define CHAR_PLUS                   '+'
#define CHAR_COMMA                  ','
#define CHAR_MINUS                  '-'
#define CHAR_DOT                    '.'
#define CHAR_SLASH                  '/'
#define CHAR_0                      '0'
#define CHAR_1                      '1'
#define CHAR_2                      '2'
#define CHAR_3                      '3'
#define CHAR_4                      '4'
#define CHAR_5                      '5'
#define CHAR_6                      '6'
#define CHAR_7                      '7'
#define CHAR_8                      '8'
#define CHAR_9                      '9'
#define CHAR_COLON                  ':'
#define CHAR_SEMICOLON              ';'
#define CHAR_LESS_THAN_SIGN         '<'
#define CHAR_EQUALS_SIGN            '='
#define CHAR_GREATER_THAN_SIGN      '>'
#define CHAR_QUESTION_MARK          '?'
#define CHAR_COMMERCIAL_AT          '@'
#define CHAR_A                      'A'
#define CHAR_B                      'B'
#define CHAR_C                      'C'
#define CHAR_D                      'D'
#define CHAR_E                      'E'
#define CHAR_F                      'F'
#define CHAR_G                      'G'
#define CHAR_H                      'H'
#define CHAR_I                      'I'
#define CHAR_J                      'J'
#define CHAR_K                      'K'
#define CHAR_L                      'L'
#define CHAR_M                      'M'
#define CHAR_N                      'N'
#define CHAR_O                      'O'
#define CHAR_P                      'P'
#define CHAR_Q                      'Q'
#define CHAR_R                      'R'
#define CHAR_S                      'S'
#define CHAR_T                      'T'
#define CHAR_U                      'U'
#define CHAR_V                      'V'
#define CHAR_W                      'W'
#define CHAR_X                      'X'
#define CHAR_Y                      'Y'
#define CHAR_Z                      'Z'
#define CHAR_LEFT_SQUARE_BRACKET    '['
#define CHAR_BACKSLASH              '\\'
#define CHAR_RIGHT_SQUARE_BRACKET   ']'
#define CHAR_CIRCUMFLEX_ACCENT      '^'
#define CHAR_UNDERSCORE             '_'
#define CHAR_GRAVE_ACCENT           '`'
#define CHAR_a                      'a'
#define CHAR_b                      'b'
#define CHAR_c                      'c'
#define CHAR_d                      'd'
#define CHAR_e                      'e'
#define CHAR_f                      'f'
#define CHAR_g                      'g'
#define CHAR_h                      'h'
#define CHAR_i                      'i'
#define CHAR_j                      'j'
#define CHAR_k                      'k'
#define CHAR_l                      'l'
#define CHAR_m                      'm'
#define CHAR_n                      'n'
#define CHAR_o                      'o'
#define CHAR_p                      'p'
#define CHAR_q                      'q'
#define CHAR_r                      'r'
#define CHAR_s                      's'
#define CHAR_t                      't'
#define CHAR_u                      'u'
#define CHAR_v                      'v'
#define CHAR_w                      'w'
#define CHAR_x                      'x'
#define CHAR_y                      'y'
#define CHAR_z                      'z'
#define CHAR_LEFT_CURLY_BRACKET     '{'
#define CHAR_VERTICAL_LINE          '|'
#define CHAR_RIGHT_CURLY_BRACKET    '}'
#define CHAR_TILDE                  '~'

#define STR_HT                      "\t"
#define STR_VT                      "\v"
#define STR_FF                      "\f"
#define STR_CR                      "\r"
#define STR_NL                      "\n"
#define STR_BS                      "\b"
#define STR_BEL                     "\a"
#ifdef EBCDIC
#define STR_ESC                     "\047"
#define STR_DEL                     "\007"
#else
#define STR_ESC                     "\033"
#define STR_DEL                     "\177"
#endif

#define STR_SPACE                   " "
#define STR_EXCLAMATION_MARK        "!"
#define STR_QUOTATION_MARK          "\""
#define STR_NUMBER_SIGN             "#"
#define STR_DOLLAR_SIGN             "$"
#define STR_PERCENT_SIGN            "%"
#define STR_AMPERSAND               "&"
#define STR_APOSTROPHE              "'"
#define STR_LEFT_PARENTHESIS        "("
#define STR_RIGHT_PARENTHESIS       ")"
#define STR_ASTERISK                "*"
#define STR_PLUS                    "+"
#define STR_COMMA                   ","
#define STR_MINUS                   "-"
#define STR_DOT                     "."
#define STR_SLASH                   "/"
#define STR_0                       "0"
#define STR_1                       "1"
#define STR_2                       "2"
#define STR_3                       "3"
#define STR_4                       "4"
#define STR_5                       "5"
#define STR_6                       "6"
#define STR_7                       "7"
#define STR_8                       "8"
#define STR_9                       "9"
#define STR_COLON                   ":"
#define STR_SEMICOLON               ";"
#define STR_LESS_THAN_SIGN          "<"
#define STR_EQUALS_SIGN             "="
#define STR_GREATER_THAN_SIGN       ">"
#define STR_QUESTION_MARK           "?"
#define STR_COMMERCIAL_AT           "@"
#define STR_A                       "A"
#define STR_B                       "B"
#define STR_C                       "C"
#define STR_D                       "D"
#define STR_E                       "E"
#define STR_F                       "F"
#define STR_G                       "G"
#define STR_H                       "H"
#define STR_I                       "I"
#define STR_J                       "J"
#define STR_K                       "K"
#define STR_L                       "L"
#define STR_M                       "M"
#define STR_N                       "N"
#define STR_O                       "O"
#define STR_P                       "P"
#define STR_Q                       "Q"
#define STR_R                       "R"
#define STR_S                       "S"
#define STR_T                       "T"
#define STR_U                       "U"
#define STR_V                       "V"
#define STR_W                       "W"
#define STR_X                       "X"
#define STR_Y                       "Y"
#define STR_Z                       "Z"
#define STR_LEFT_SQUARE_BRACKET     "["
#define STR_BACKSLASH               "\\"
#define STR_RIGHT_SQUARE_BRACKET    "]"
#define STR_CIRCUMFLEX_ACCENT       "^"
#define STR_UNDERSCORE              "_"
#define STR_GRAVE_ACCENT            "`"
#define STR_a                       "a"
#define STR_b                       "b"
#define STR_c                       "c"
#define STR_d                       "d"
#define STR_e                       "e"
#define STR_f                       "f"
#define STR_g                       "g"
#define STR_h                       "h"
#define STR_i                       "i"
#define STR_j                       "j"
#define STR_k                       "k"
#define STR_l                       "l"
#define STR_m                       "m"
#define STR_n                       "n"
#define STR_o                       "o"
#define STR_p                       "p"
#define STR_q                       "q"
#define STR_r                       "r"
#define STR_s                       "s"
#define STR_t                       "t"
#define STR_u                       "u"
#define STR_v                       "v"
#define STR_w                       "w"
#define STR_x                       "x"
#define STR_y                       "y"
#define STR_z                       "z"
#define STR_LEFT_CURLY_BRACKET      "{"
#define STR_VERTICAL_LINE           "|"
#define STR_RIGHT_CURLY_BRACKET     "}"
#define STR_TILDE                   "~"

#define STRING_ACCEPT0              "ACCEPT\0"
#define STRING_COMMIT0              "COMMIT\0"
#define STRING_F0                   "F\0"
#define STRING_FAIL0                "FAIL\0"
#define STRING_PRUNE0               "PRUNE\0"
#define STRING_SKIP0                "SKIP\0"
#define STRING_THEN                 "THEN"

#define STRING_alpha0               "alpha\0"
#define STRING_lower0               "lower\0"
#define STRING_upper0               "upper\0"
#define STRING_alnum0               "alnum\0"
#define STRING_ascii0               "ascii\0"
#define STRING_blank0               "blank\0"
#define STRING_cntrl0               "cntrl\0"
#define STRING_digit0               "digit\0"
#define STRING_graph0               "graph\0"
#define STRING_print0               "print\0"
#define STRING_punct0               "punct\0"
#define STRING_space0               "space\0"
#define STRING_word0                "word\0"
#define STRING_xdigit               "xdigit"

#define STRING_DEFINE               "DEFINE"

#define STRING_CR_RIGHTPAR          "CR)"
#define STRING_LF_RIGHTPAR          "LF)"
#define STRING_CRLF_RIGHTPAR        "CRLF)"
#define STRING_ANY_RIGHTPAR         "ANY)"
#define STRING_ANYCRLF_RIGHTPAR     "ANYCRLF)"
#define STRING_BSR_ANYCRLF_RIGHTPAR "BSR_ANYCRLF)"
#define STRING_BSR_UNICODE_RIGHTPAR "BSR_UNICODE)"
#define STRING_UTF8_RIGHTPAR        "UTF8)"

#else  /* SUPPORT_UTF8 */

/* UTF-8 support is enabled; always use UTF-8 (=ASCII) character codes. This
works in both modes non-EBCDIC platforms, and on EBCDIC platforms in UTF-8 mode
only. */

#define CHAR_HT                     '\011'
#define CHAR_VT                     '\013'
#define CHAR_FF                     '\014'
#define CHAR_CR                     '\015'
#define CHAR_NL                     '\012'
#define CHAR_BS                     '\010'
#define CHAR_BEL                    '\007'
#define CHAR_ESC                    '\033'
#define CHAR_DEL                    '\177'

#define CHAR_SPACE                  '\040'
#define CHAR_EXCLAMATION_MARK       '\041'
#define CHAR_QUOTATION_MARK         '\042'
#define CHAR_NUMBER_SIGN            '\043'
#define CHAR_DOLLAR_SIGN            '\044'
#define CHAR_PERCENT_SIGN           '\045'
#define CHAR_AMPERSAND              '\046'
#define CHAR_APOSTROPHE             '\047'
#define CHAR_LEFT_PARENTHESIS       '\050'
#define CHAR_RIGHT_PARENTHESIS      '\051'
#define CHAR_ASTERISK               '\052'
#define CHAR_PLUS                   '\053'
#define CHAR_COMMA                  '\054'
#define CHAR_MINUS                  '\055'
#define CHAR_DOT                    '\056'
#define CHAR_SLASH                  '\057'
#define CHAR_0                      '\060'
#define CHAR_1                      '\061'
#define CHAR_2                      '\062'
#define CHAR_3                      '\063'
#define CHAR_4                      '\064'
#define CHAR_5                      '\065'
#define CHAR_6                      '\066'
#define CHAR_7                      '\067'
#define CHAR_8                      '\070'
#define CHAR_9                      '\071'
#define CHAR_COLON                  '\072'
#define CHAR_SEMICOLON              '\073'
#define CHAR_LESS_THAN_SIGN         '\074'
#define CHAR_EQUALS_SIGN            '\075'
#define CHAR_GREATER_THAN_SIGN      '\076'
#define CHAR_QUESTION_MARK          '\077'
#define CHAR_COMMERCIAL_AT          '\100'
#define CHAR_A                      '\101'
#define CHAR_B                      '\102'
#define CHAR_C                      '\103'
#define CHAR_D                      '\104'
#define CHAR_E                      '\105'
#define CHAR_F                      '\106'
#define CHAR_G                      '\107'
#define CHAR_H                      '\110'
#define CHAR_I                      '\111'
#define CHAR_J                      '\112'
#define CHAR_K                      '\113'
#define CHAR_L                      '\114'
#define CHAR_M                      '\115'
#define CHAR_N                      '\116'
#define CHAR_O                      '\117'
#define CHAR_P                      '\120'
#define CHAR_Q                      '\121'
#define CHAR_R                      '\122'
#define CHAR_S                      '\123'
#define CHAR_T                      '\124'
#define CHAR_U                      '\125'
#define CHAR_V                      '\126'
#define CHAR_W                      '\127'
#define CHAR_X                      '\130'
#define CHAR_Y                      '\131'
#define CHAR_Z                      '\132'
#define CHAR_LEFT_SQUARE_BRACKET    '\133'
#define CHAR_BACKSLASH              '\134'
#define CHAR_RIGHT_SQUARE_BRACKET   '\135'
#define CHAR_CIRCUMFLEX_ACCENT      '\136'
#define CHAR_UNDERSCORE             '\137'
#define CHAR_GRAVE_ACCENT           '\140'
#define CHAR_a                      '\141'
#define CHAR_b                      '\142'
#define CHAR_c                      '\143'
#define CHAR_d                      '\144'
#define CHAR_e                      '\145'
#define CHAR_f                      '\146'
#define CHAR_g                      '\147'
#define CHAR_h                      '\150'
#define CHAR_i                      '\151'
#define CHAR_j                      '\152'
#define CHAR_k                      '\153'
#define CHAR_l                      '\154'
#define CHAR_m                      '\155'
#define CHAR_n                      '\156'
#define CHAR_o                      '\157'
#define CHAR_p                      '\160'
#define CHAR_q                      '\161'
#define CHAR_r                      '\162'
#define CHAR_s                      '\163'
#define CHAR_t                      '\164'
#define CHAR_u                      '\165'
#define CHAR_v                      '\166'
#define CHAR_w                      '\167'
#define CHAR_x                      '\170'
#define CHAR_y                      '\171'
#define CHAR_z                      '\172'
#define CHAR_LEFT_CURLY_BRACKET     '\173'
#define CHAR_VERTICAL_LINE          '\174'
#define CHAR_RIGHT_CURLY_BRACKET    '\175'
#define CHAR_TILDE                  '\176'

#define STR_HT                      "\011"
#define STR_VT                      "\013"
#define STR_FF                      "\014"
#define STR_CR                      "\015"
#define STR_NL                      "\012"
#define STR_BS                      "\010"
#define STR_BEL                     "\007"
#define STR_ESC                     "\033"
#define STR_DEL                     "\177"

#define STR_SPACE                   "\040"
#define STR_EXCLAMATION_MARK        "\041"
#define STR_QUOTATION_MARK          "\042"
#define STR_NUMBER_SIGN             "\043"
#define STR_DOLLAR_SIGN             "\044"
#define STR_PERCENT_SIGN            "\045"
#define STR_AMPERSAND               "\046"
#define STR_APOSTROPHE              "\047"
#define STR_LEFT_PARENTHESIS        "\050"
#define STR_RIGHT_PARENTHESIS       "\051"
#define STR_ASTERISK                "\052"
#define STR_PLUS                    "\053"
#define STR_COMMA                   "\054"
#define STR_MINUS                   "\055"
#define STR_DOT                     "\056"
#define STR_SLASH                   "\057"
#define STR_0                       "\060"
#define STR_1                       "\061"
#define STR_2                       "\062"
#define STR_3                       "\063"
#define STR_4                       "\064"
#define STR_5                       "\065"
#define STR_6                       "\066"
#define STR_7                       "\067"
#define STR_8                       "\070"
#define STR_9                       "\071"
#define STR_COLON                   "\072"
#define STR_SEMICOLON               "\073"
#define STR_LESS_THAN_SIGN          "\074"
#define STR_EQUALS_SIGN             "\075"
#define STR_GREATER_THAN_SIGN       "\076"
#define STR_QUESTION_MARK           "\077"
#define STR_COMMERCIAL_AT           "\100"
#define STR_A                       "\101"
#define STR_B                       "\102"
#define STR_C                       "\103"
#define STR_D                       "\104"
#define STR_E                       "\105"
#define STR_F                       "\106"
#define STR_G                       "\107"
#define STR_H                       "\110"
#define STR_I                       "\111"
#define STR_J                       "\112"
#define STR_K                       "\113"
#define STR_L                       "\114"
#define STR_M                       "\115"
#define STR_N                       "\116"
#define STR_O                       "\117"
#define STR_P                       "\120"
#define STR_Q                       "\121"
#define STR_R                       "\122"
#define STR_S                       "\123"
#define STR_T                       "\124"
#define STR_U                       "\125"
#define STR_V                       "\126"
#define STR_W                       "\127"
#define STR_X                       "\130"
#define STR_Y                       "\131"
#define STR_Z                       "\132"
#define STR_LEFT_SQUARE_BRACKET     "\133"
#define STR_BACKSLASH               "\134"
#define STR_RIGHT_SQUARE_BRACKET    "\135"
#define STR_CIRCUMFLEX_ACCENT       "\136"
#define STR_UNDERSCORE              "\137"
#define STR_GRAVE_ACCENT            "\140"
#define STR_a                       "\141"
#define STR_b                       "\142"
#define STR_c                       "\143"
#define STR_d                       "\144"
#define STR_e                       "\145"
#define STR_f                       "\146"
#define STR_g                       "\147"
#define STR_h                       "\150"
#define STR_i                       "\151"
#define STR_j                       "\152"
#define STR_k                       "\153"
#define STR_l                       "\154"
#define STR_m                       "\155"
#define STR_n                       "\156"
#define STR_o                       "\157"
#define STR_p                       "\160"
#define STR_q                       "\161"
#define STR_r                       "\162"
#define STR_s                       "\163"
#define STR_t                       "\164"
#define STR_u                       "\165"
#define STR_v                       "\166"
#define STR_w                       "\167"
#define STR_x                       "\170"
#define STR_y                       "\171"
#define STR_z                       "\172"
#define STR_LEFT_CURLY_BRACKET      "\173"
#define STR_VERTICAL_LINE           "\174"
#define STR_RIGHT_CURLY_BRACKET     "\175"
#define STR_TILDE                   "\176"

#define STRING_ACCEPT0              STR_A STR_C STR_C STR_E STR_P STR_T "\0"
#define STRING_COMMIT0              STR_C STR_O STR_M STR_M STR_I STR_T "\0"
#define STRING_F0                   STR_F "\0"
#define STRING_FAIL0                STR_F STR_A STR_I STR_L "\0"
#define STRING_PRUNE0               STR_P STR_R STR_U STR_N STR_E "\0"
#define STRING_SKIP0                STR_S STR_K STR_I STR_P "\0"
#define STRING_THEN                 STR_T STR_H STR_E STR_N

#define STRING_alpha0               STR_a STR_l STR_p STR_h STR_a "\0"
#define STRING_lower0               STR_l STR_o STR_w STR_e STR_r "\0"
#define STRING_upper0               STR_u STR_p STR_p STR_e STR_r "\0"
#define STRING_alnum0               STR_a STR_l STR_n STR_u STR_m "\0"
#define STRING_ascii0               STR_a STR_s STR_c STR_i STR_i "\0"
#define STRING_blank0               STR_b STR_l STR_a STR_n STR_k "\0"
#define STRING_cntrl0               STR_c STR_n STR_t STR_r STR_l "\0"
#define STRING_digit0               STR_d STR_i STR_g STR_i STR_t "\0"
#define STRING_graph0               STR_g STR_r STR_a STR_p STR_h "\0"
#define STRING_print0               STR_p STR_r STR_i STR_n STR_t "\0"
#define STRING_punct0               STR_p STR_u STR_n STR_c STR_t "\0"
#define STRING_space0               STR_s STR_p STR_a STR_c STR_e "\0"
#define STRING_word0                STR_w STR_o STR_r STR_d       "\0"
#define STRING_xdigit               STR_x STR_d STR_i STR_g STR_i STR_t

#define STRING_DEFINE               STR_D STR_E STR_F STR_I STR_N STR_E

#define STRING_CR_RIGHTPAR          STR_C STR_R STR_RIGHT_PARENTHESIS
#define STRING_LF_RIGHTPAR          STR_L STR_F STR_RIGHT_PARENTHESIS
#define STRING_CRLF_RIGHTPAR        STR_C STR_R STR_L STR_F STR_RIGHT_PARENTHESIS
#define STRING_ANY_RIGHTPAR         STR_A STR_N STR_Y STR_RIGHT_PARENTHESIS
#define STRING_ANYCRLF_RIGHTPAR     STR_A STR_N STR_Y STR_C STR_R STR_L STR_F STR_RIGHT_PARENTHESIS
#define STRING_BSR_ANYCRLF_RIGHTPAR STR_B STR_S STR_R STR_UNDERSCORE STR_A STR_N STR_Y STR_C STR_R STR_L STR_F STR_RIGHT_PARENTHESIS
#define STRING_BSR_UNICODE_RIGHTPAR STR_B STR_S STR_R STR_UNDERSCORE STR_U STR_N STR_I STR_C STR_O STR_D STR_E STR_RIGHT_PARENTHESIS
#define STRING_UTF8_RIGHTPAR        STR_U STR_T STR_F STR_8 STR_RIGHT_PARENTHESIS

#endif  /* SUPPORT_UTF8 */

/* Escape items that are just an encoding of a particular data value. */

#ifndef ESC_e
#define ESC_e CHAR_ESC
#endif

#ifndef ESC_f
#define ESC_f CHAR_FF
#endif

#ifndef ESC_n
#define ESC_n CHAR_NL
#endif

#ifndef ESC_r
#define ESC_r CHAR_CR
#endif

/* We can't officially use ESC_t because it is a POSIX reserved identifier
(presumably because of all the others like size_t). */

#ifndef ESC_tee
#define ESC_tee CHAR_HT
#endif

/* Codes for different types of Unicode property */

#define PT_ANY        0    /* Any property - matches all chars */
#define PT_LAMP       1    /* L& - the union of Lu, Ll, Lt */
#define PT_GC         2    /* General characteristic (e.g. L) */
#define PT_PC         3    /* Particular characteristic (e.g. Lu) */
#define PT_SC         4    /* Script (e.g. Han) */

/* Flag bits and data types for the extended class (OP_XCLASS) for classes that
contain UTF-8 characters with values greater than 255. */

#define XCL_NOT    0x01    /* Flag: this is a negative class */
#define XCL_MAP    0x02    /* Flag: a 32-byte map is present */

#define XCL_END       0    /* Marks end of individual items */
#define XCL_SINGLE    1    /* Single item (one multibyte char) follows */
#define XCL_RANGE     2    /* A range (two multibyte chars) follows */
#define XCL_PROP      3    /* Unicode property (2-byte property code follows) */
#define XCL_NOTPROP   4    /* Unicode inverted property (ditto) */

/* These are escaped items that aren't just an encoding of a particular data
value such as \n. They must have non-zero values, as check_escape() returns
their negation. Also, they must appear in the same order as in the opcode
definitions below, up to ESC_z. There's a dummy for OP_ANY because it
corresponds to "." rather than an escape sequence, and another for OP_ALLANY
(which is used for [^] in JavaScript compatibility mode).

The final escape must be ESC_REF as subsequent values are used for
backreferences (\1, \2, \3, etc). There are two tests in the code for an escape
greater than ESC_b and less than ESC_Z to detect the types that may be
repeated. These are the types that consume characters. If any new escapes are
put in between that don't consume a character, that code will have to change.
*/

enum { ESC_A = 1, ESC_G, ESC_K, ESC_B, ESC_b, ESC_D, ESC_d, ESC_S, ESC_s,
       ESC_W, ESC_w, ESC_dum1, ESC_dum2, ESC_C, ESC_P, ESC_p, ESC_R, ESC_H,
       ESC_h, ESC_V, ESC_v, ESC_X, ESC_Z, ESC_z, ESC_E, ESC_Q, ESC_g, ESC_k,
       ESC_REF };


/* Opcode table: Starting from 1 (i.e. after OP_END), the values up to
OP_EOD must correspond in order to the list of escapes immediately above.

*** NOTE NOTE NOTE *** Whenever this list is updated, the two macro definitions
that follow must also be updated to match. There are also tables called
"coptable" and "poptable" in pcre_dfa_exec.c that must be updated. */

enum {
  OP_END,            /* 0 End of pattern */

  /* Values corresponding to backslashed metacharacters */

  OP_SOD,            /* 1 Start of data: \A */
  OP_SOM,            /* 2 Start of match (subject + offset): \G */
  OP_SET_SOM,        /* 3 Set start of match (\K) */
  OP_NOT_WORD_BOUNDARY,  /*  4 \B */
  OP_WORD_BOUNDARY,      /*  5 \b */
  OP_NOT_DIGIT,          /*  6 \D */
  OP_DIGIT,              /*  7 \d */
  OP_NOT_WHITESPACE,     /*  8 \S */
  OP_WHITESPACE,         /*  9 \s */
  OP_NOT_WORDCHAR,       /* 10 \W */
  OP_WORDCHAR,           /* 11 \w */
  OP_ANY,            /* 12 Match any character (subject to DOTALL) */
  OP_ALLANY,         /* 13 Match any character (not subject to DOTALL) */
  OP_ANYBYTE,        /* 14 Match any byte (\C); different to OP_ANY for UTF-8 */
  OP_NOTPROP,        /* 15 \P (not Unicode property) */
  OP_PROP,           /* 16 \p (Unicode property) */
  OP_ANYNL,          /* 17 \R (any newline sequence) */
  OP_NOT_HSPACE,     /* 18 \H (not horizontal whitespace) */
  OP_HSPACE,         /* 19 \h (horizontal whitespace) */
  OP_NOT_VSPACE,     /* 20 \V (not vertical whitespace) */
  OP_VSPACE,         /* 21 \v (vertical whitespace) */
  OP_EXTUNI,         /* 22 \X (extended Unicode sequence */
  OP_EODN,           /* 23 End of data or \n at end of data: \Z. */
  OP_EOD,            /* 24 End of data: \z */

  OP_OPT,            /* 25 Set runtime options */
  OP_CIRC,           /* 26 Start of line - varies with multiline switch */
  OP_DOLL,           /* 27 End of line - varies with multiline switch */
  OP_CHAR,           /* 28 Match one character, casefully */
  OP_CHARNC,         /* 29 Match one character, caselessly */
  OP_NOT,            /* 30 Match one character, not the following one */

  OP_STAR,           /* 31 The maximizing and minimizing versions of */
  OP_MINSTAR,        /* 32 these six opcodes must come in pairs, with */
  OP_PLUS,           /* 33 the minimizing one second. */
  OP_MINPLUS,        /* 34 This first set applies to single characters.*/
  OP_QUERY,          /* 35 */
  OP_MINQUERY,       /* 36 */

  OP_UPTO,           /* 37 From 0 to n matches */
  OP_MINUPTO,        /* 38 */
  OP_EXACT,          /* 39 Exactly n matches */

  OP_POSSTAR,        /* 40 Possessified star */
  OP_POSPLUS,        /* 41 Possessified plus */
  OP_POSQUERY,       /* 42 Posesssified query */
  OP_POSUPTO,        /* 43 Possessified upto */

  OP_NOTSTAR,        /* 44 The maximizing and minimizing versions of */
  OP_NOTMINSTAR,     /* 45 these six opcodes must come in pairs, with */
  OP_NOTPLUS,        /* 46 the minimizing one second. They must be in */
  OP_NOTMINPLUS,     /* 47 exactly the same order as those above. */
  OP_NOTQUERY,       /* 48 This set applies to "not" single characters. */
  OP_NOTMINQUERY,    /* 49 */

  OP_NOTUPTO,        /* 50 From 0 to n matches */
  OP_NOTMINUPTO,     /* 51 */
  OP_NOTEXACT,       /* 52 Exactly n matches */

  OP_NOTPOSSTAR,     /* 53 Possessified versions */
  OP_NOTPOSPLUS,     /* 54 */
  OP_NOTPOSQUERY,    /* 55 */
  OP_NOTPOSUPTO,     /* 56 */

  OP_TYPESTAR,       /* 57 The maximizing and minimizing versions of */
  OP_TYPEMINSTAR,    /* 58 these six opcodes must come in pairs, with */
  OP_TYPEPLUS,       /* 59 the minimizing one second. These codes must */
  OP_TYPEMINPLUS,    /* 60 be in exactly the same order as those above. */
  OP_TYPEQUERY,      /* 61 This set applies to character types such as \d */
  OP_TYPEMINQUERY,   /* 62 */

  OP_TYPEUPTO,       /* 63 From 0 to n matches */
  OP_TYPEMINUPTO,    /* 64 */
  OP_TYPEEXACT,      /* 65 Exactly n matches */

  OP_TYPEPOSSTAR,    /* 66 Possessified versions */
  OP_TYPEPOSPLUS,    /* 67 */
  OP_TYPEPOSQUERY,   /* 68 */
  OP_TYPEPOSUPTO,    /* 69 */

  OP_CRSTAR,         /* 70 The maximizing and minimizing versions of */
  OP_CRMINSTAR,      /* 71 all these opcodes must come in pairs, with */
  OP_CRPLUS,         /* 72 the minimizing one second. These codes must */
  OP_CRMINPLUS,      /* 73 be in exactly the same order as those above. */
  OP_CRQUERY,        /* 74 These are for character classes and back refs */
  OP_CRMINQUERY,     /* 75 */
  OP_CRRANGE,        /* 76 These are different to the three sets above. */
  OP_CRMINRANGE,     /* 77 */

  OP_CLASS,          /* 78 Match a character class, chars < 256 only */
  OP_NCLASS,         /* 79 Same, but the bitmap was created from a negative
                           class - the difference is relevant only when a UTF-8
                           character > 255 is encountered. */

  OP_XCLASS,         /* 80 Extended class for handling UTF-8 chars within the
                           class. This does both positive and negative. */

  OP_REF,            /* 81 Match a back reference */
  OP_RECURSE,        /* 82 Match a numbered subpattern (possibly recursive) */
  OP_CALLOUT,        /* 83 Call out to external function if provided */

  OP_ALT,            /* 84 Start of alternation */
  OP_KET,            /* 85 End of group that doesn't have an unbounded repeat */
  OP_KETRMAX,        /* 86 These two must remain together and in this */
  OP_KETRMIN,        /* 87 order. They are for groups the repeat for ever. */

  /* The assertions must come before BRA, CBRA, ONCE, and COND.*/

  OP_ASSERT,         /* 88 Positive lookahead */
  OP_ASSERT_NOT,     /* 89 Negative lookahead */
  OP_ASSERTBACK,     /* 90 Positive lookbehind */
  OP_ASSERTBACK_NOT, /* 91 Negative lookbehind */
  OP_REVERSE,        /* 92 Move pointer back - used in lookbehind assertions */

  /* ONCE, BRA, CBRA, and COND must come after the assertions, with ONCE first,
  as there's a test for >= ONCE for a subpattern that isn't an assertion. */

  OP_ONCE,           /* 93 Atomic group */
  OP_BRA,            /* 94 Start of non-capturing bracket */
  OP_CBRA,           /* 95 Start of capturing bracket */
  OP_COND,           /* 96 Conditional group */

  /* These three must follow the previous three, in the same order. There's a
  check for >= SBRA to distinguish the two sets. */

  OP_SBRA,           /* 97 Start of non-capturing bracket, check empty  */
  OP_SCBRA,          /* 98 Start of capturing bracket, check empty */
  OP_SCOND,          /* 99 Conditional group, check empty */

  /* The next two pairs must (respectively) be kept together. */

  OP_CREF,           /* 100 Used to hold a capture number as condition */
  OP_NCREF,          /* 101 Same, but generaged by a name reference*/
  OP_RREF,           /* 102 Used to hold a recursion number as condition */
  OP_NRREF,          /* 103 Same, but generaged by a name reference*/
  OP_DEF,            /* 104 The DEFINE condition */

  OP_BRAZERO,        /* 105 These two must remain together and in this */
  OP_BRAMINZERO,     /* 106 order. */

  /* These are backtracking control verbs */

  OP_PRUNE,          /* 107 */
  OP_SKIP,           /* 108 */
  OP_THEN,           /* 109 */
  OP_COMMIT,         /* 110 */

  /* These are forced failure and success verbs */

  OP_FAIL,           /* 111 */
  OP_ACCEPT,         /* 112 */
  OP_CLOSE,          /* 113 Used before OP_ACCEPT to close open captures */

  /* This is used to skip a subpattern with a {0} quantifier */

  OP_SKIPZERO,       /* 114 */

  /* This is not an opcode, but is used to check that tables indexed by opcode
  are the correct length, in order to catch updating errors - there have been
  some in the past. */

  OP_TABLE_LENGTH
};

/* *** NOTE NOTE NOTE *** Whenever the list above is updated, the two macro
definitions that follow must also be updated to match. There are also tables
called "coptable" cna "poptable" in pcre_dfa_exec.c that must be updated. */


/* This macro defines textual names for all the opcodes. These are used only
for debugging. The macro is referenced only in pcre_printint.c. */

#define OP_NAME_LIST \
  "End", "\\A", "\\G", "\\K", "\\B", "\\b", "\\D", "\\d",         \
  "\\S", "\\s", "\\W", "\\w", "Any", "AllAny", "Anybyte",         \
  "notprop", "prop", "\\R", "\\H", "\\h", "\\V", "\\v",           \
  "extuni",  "\\Z", "\\z",                                        \
  "Opt", "^", "$", "char", "charnc", "not",                       \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*+","++", "?+", "{",                                           \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*+","++", "?+", "{",                                           \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*+","++", "?+", "{",                                           \
  "*", "*?", "+", "+?", "?", "??", "{", "{",                      \
  "class", "nclass", "xclass", "Ref", "Recurse", "Callout",       \
  "Alt", "Ket", "KetRmax", "KetRmin", "Assert", "Assert not",     \
  "AssertB", "AssertB not", "Reverse",                            \
  "Once", "Bra", "CBra", "Cond", "SBra", "SCBra", "SCond",        \
  "Cond ref", "Cond nref", "Cond rec", "Cond nrec", "Cond def",   \
  "Brazero", "Braminzero",                                        \
  "*PRUNE", "*SKIP", "*THEN", "*COMMIT", "*FAIL", "*ACCEPT",      \
  "Close", "Skip zero"


/* This macro defines the length of fixed length operations in the compiled
regex. The lengths are used when searching for specific things, and also in the
debugging printing of a compiled regex. We use a macro so that it can be
defined close to the definitions of the opcodes themselves.

As things have been extended, some of these are no longer fixed lenths, but are
minima instead. For example, the length of a single-character repeat may vary
in UTF-8 mode. The code that uses this table must know about such things. */

#define OP_LENGTHS \
  1,                             /* End                                    */ \
  1, 1, 1, 1, 1,                 /* \A, \G, \K, \B, \b                     */ \
  1, 1, 1, 1, 1, 1,              /* \D, \d, \S, \s, \W, \w                 */ \
  1, 1, 1,                       /* Any, AllAny, Anybyte                   */ \
  3, 3,                          /* \P, \p                                 */ \
  1, 1, 1, 1, 1,                 /* \R, \H, \h, \V, \v                     */ \
  1,                             /* \X                                     */ \
  1, 1, 2, 1, 1,                 /* \Z, \z, Opt, ^, $                      */ \
  2,                             /* Char  - the minimum length             */ \
  2,                             /* Charnc  - the minimum length           */ \
  2,                             /* not                                    */ \
  /* Positive single-char repeats                            ** These are  */ \
  2, 2, 2, 2, 2, 2,              /* *, *?, +, +?, ?, ??      ** minima in  */ \
  4, 4, 4,                       /* upto, minupto, exact     ** UTF-8 mode */ \
  2, 2, 2, 4,                    /* *+, ++, ?+, upto+                      */ \
  /* Negative single-char repeats - only for chars < 256                   */ \
  2, 2, 2, 2, 2, 2,              /* NOT *, *?, +, +?, ?, ??                */ \
  4, 4, 4,                       /* NOT upto, minupto, exact               */ \
  2, 2, 2, 4,                    /* Possessive *, +, ?, upto               */ \
  /* Positive type repeats                                                 */ \
  2, 2, 2, 2, 2, 2,              /* Type *, *?, +, +?, ?, ??               */ \
  4, 4, 4,                       /* Type upto, minupto, exact              */ \
  2, 2, 2, 4,                    /* Possessive *+, ++, ?+, upto+           */ \
  /* Character class & ref repeats                                         */ \
  1, 1, 1, 1, 1, 1,              /* *, *?, +, +?, ?, ??                    */ \
  5, 5,                          /* CRRANGE, CRMINRANGE                    */ \
 33,                             /* CLASS                                  */ \
 33,                             /* NCLASS                                 */ \
  0,                             /* XCLASS - variable length               */ \
  3,                             /* REF                                    */ \
  1+LINK_SIZE,                   /* RECURSE                                */ \
  2+2*LINK_SIZE,                 /* CALLOUT                                */ \
  1+LINK_SIZE,                   /* Alt                                    */ \
  1+LINK_SIZE,                   /* Ket                                    */ \
  1+LINK_SIZE,                   /* KetRmax                                */ \
  1+LINK_SIZE,                   /* KetRmin                                */ \
  1+LINK_SIZE,                   /* Assert                                 */ \
  1+LINK_SIZE,                   /* Assert not                             */ \
  1+LINK_SIZE,                   /* Assert behind                          */ \
  1+LINK_SIZE,                   /* Assert behind not                      */ \
  1+LINK_SIZE,                   /* Reverse                                */ \
  1+LINK_SIZE,                   /* ONCE                                   */ \
  1+LINK_SIZE,                   /* BRA                                    */ \
  3+LINK_SIZE,                   /* CBRA                                   */ \
  1+LINK_SIZE,                   /* COND                                   */ \
  1+LINK_SIZE,                   /* SBRA                                   */ \
  3+LINK_SIZE,                   /* SCBRA                                  */ \
  1+LINK_SIZE,                   /* SCOND                                  */ \
  3, 3,                          /* CREF, NCREF                            */ \
  3, 3,                          /* RREF, NRREF                            */ \
  1,                             /* DEF                                    */ \
  1, 1,                          /* BRAZERO, BRAMINZERO                    */ \
  1, 1, 1, 1,                    /* PRUNE, SKIP, THEN, COMMIT,             */ \
  1, 1, 3, 1                     /* FAIL, ACCEPT, CLOSE, SKIPZERO          */


/* A magic value for OP_RREF and OP_NRREF to indicate the "any recursion"
condition. */

#define RREF_ANY  0xffff

/* Compile time error code numbers. They are given names so that they can more
easily be tracked. When a new number is added, the table called eint in
pcreposix.c must be updated. */

enum { ERR0,  ERR1,  ERR2,  ERR3,  ERR4,  ERR5,  ERR6,  ERR7,  ERR8,  ERR9,
       ERR10, ERR11, ERR12, ERR13, ERR14, ERR15, ERR16, ERR17, ERR18, ERR19,
       ERR20, ERR21, ERR22, ERR23, ERR24, ERR25, ERR26, ERR27, ERR28, ERR29,
       ERR30, ERR31, ERR32, ERR33, ERR34, ERR35, ERR36, ERR37, ERR38, ERR39,
       ERR40, ERR41, ERR42, ERR43, ERR44, ERR45, ERR46, ERR47, ERR48, ERR49,
       ERR50, ERR51, ERR52, ERR53, ERR54, ERR55, ERR56, ERR57, ERR58, ERR59,
       ERR60, ERR61, ERR62, ERR63, ERR64, ERR65, ERRCOUNT };

/* The real format of the start of the pcre block; the index of names and the
code vector run on as long as necessary after the end. We store an explicit
offset to the name table so that if a regex is compiled on one host, saved, and
then run on another where the size of pointers is different, all might still
be well. For the case of compiled-on-4 and run-on-8, we include an extra
pointer that is always NULL. For future-proofing, a few dummy fields were
originally included - even though you can never get this planning right - but
there is only one left now.

NOTE NOTE NOTE:
Because people can now save and re-use compiled patterns, any additions to this
structure should be made at the end, and something earlier (e.g. a new
flag in the options or one of the dummy fields) should indicate that the new
fields are present. Currently PCRE always sets the dummy fields to zero.
NOTE NOTE NOTE
*/

typedef struct real_pcre {
  pcre_uint32 magic_number;
  pcre_uint32 size;               /* Total that was malloced */
  pcre_uint32 options;            /* Public options */
  pcre_uint16 flags;              /* Private flags */
  pcre_uint16 dummy1;             /* For future use */
  pcre_uint16 top_bracket;
  pcre_uint16 top_backref;
  pcre_uint16 first_byte;
  pcre_uint16 req_byte;
  pcre_uint16 name_table_offset;  /* Offset to name table that follows */
  pcre_uint16 name_entry_size;    /* Size of any name items */
  pcre_uint16 name_count;         /* Number of name items */
  pcre_uint16 ref_count;          /* Reference count */

  const unsigned char *tables;    /* Pointer to tables or NULL for std */
  const unsigned char *nullpad;   /* NULL padding */
} real_pcre;

/* The format of the block used to store data from pcre_study(). The same
remark (see NOTE above) about extending this structure applies. */

typedef struct pcre_study_data {
  pcre_uint32 size;               /* Total that was malloced */
  pcre_uint32 flags;              /* Private flags */
  uschar start_bits[32];          /* Starting char bits */
  pcre_uint32 minlength;          /* Minimum subject length */
} pcre_study_data;

/* Structure for building a chain of open capturing subpatterns during
compiling, so that instructions to close them can be compiled when (*ACCEPT) is
encountered. This is also used to identify subpatterns that contain recursive
back references to themselves, so that they can be made atomic. */

typedef struct open_capitem {
  struct open_capitem *next;    /* Chain link */
  pcre_uint16 number;           /* Capture number */
  pcre_uint16 flag;             /* Set TRUE if recursive back ref */
} open_capitem;

/* Structure for passing "static" information around between the functions
doing the compiling, so that they are thread-safe. */

typedef struct compile_data {
  const uschar *lcc;            /* Points to lower casing table */
  const uschar *fcc;            /* Points to case-flipping table */
  const uschar *cbits;          /* Points to character type table */
  const uschar *ctypes;         /* Points to table of type maps */
  const uschar *start_workspace;/* The start of working space */
  const uschar *start_code;     /* The start of the compiled code */
  const uschar *start_pattern;  /* The start of the pattern */
  const uschar *end_pattern;    /* The end of the pattern */
  open_capitem *open_caps;      /* Chain of open capture items */
  uschar *hwm;                  /* High watermark of workspace */
  uschar *name_table;           /* The name/number table */
  int  names_found;             /* Number of entries so far */
  int  name_entry_size;         /* Size of each entry */
  int  bracount;                /* Count of capturing parens as we compile */
  int  final_bracount;          /* Saved value after first pass */
  int  top_backref;             /* Maximum back reference */
  unsigned int backref_map;     /* Bitmap of low back refs */
  int  external_options;        /* External (initial) options */
  int  external_flags;          /* External flag bits to be set */
  int  req_varyopt;             /* "After variable item" flag for reqbyte */
  BOOL had_accept;              /* (*ACCEPT) encountered */
  BOOL check_lookbehind;        /* Lookbehinds need later checking */
  int  nltype;                  /* Newline type */
  int  nllen;                   /* Newline string length */
  uschar nl[4];                 /* Newline string when fixed length */
} compile_data;

/* Structure for maintaining a chain of pointers to the currently incomplete
branches, for testing for left recursion. */

typedef struct branch_chain {
  struct branch_chain *outer;
  uschar *current_branch;
} branch_chain;

/* Structure for items in a linked list that represents an explicit recursive
call within the pattern. */

typedef struct recursion_info {
  struct recursion_info *prevrec; /* Previous recursion record (or NULL) */
  int group_num;                /* Number of group that was called */
  const uschar *after_call;     /* "Return value": points after the call in the expr */
  int *offset_save;             /* Pointer to start of saved offsets */
  int saved_max;                /* Number of saved offsets */
  int save_offset_top;          /* Current value of offset_top */
} recursion_info;

/* Structure for building a chain of data for holding the values of the subject
pointer at the start of each subpattern, so as to detect when an empty string
has been matched by a subpattern - to break infinite loops. */

typedef struct eptrblock {
  struct eptrblock *epb_prev;
  USPTR epb_saved_eptr;
} eptrblock;


/* Structure for passing "static" information around between the functions
doing traditional NFA matching, so that they are thread-safe. */

typedef struct match_data {
  unsigned long int match_call_count;      /* As it says */
  unsigned long int match_limit;           /* As it says */
  unsigned long int match_limit_recursion; /* As it says */
  int   *offset_vector;         /* Offset vector */
  int    offset_end;            /* One past the end */
  int    offset_max;            /* The maximum usable for return data */
  int    nltype;                /* Newline type */
  int    nllen;                 /* Newline string length */
  int    name_count;            /* Number of names in name table */
  int    name_entry_size;       /* Size of entry in names table */
  uschar *name_table;           /* Table of names */
  uschar nl[4];                 /* Newline string when fixed */
  const uschar *lcc;            /* Points to lower casing table */
  const uschar *ctypes;         /* Points to table of type maps */
  BOOL   offset_overflow;       /* Set if too many extractions */
  BOOL   notbol;                /* NOTBOL flag */
  BOOL   noteol;                /* NOTEOL flag */
  BOOL   utf8;                  /* UTF8 flag */
  BOOL   jscript_compat;        /* JAVASCRIPT_COMPAT flag */
  BOOL   endonly;               /* Dollar not before final \n */
  BOOL   notempty;              /* Empty string match not wanted */
  BOOL   notempty_atstart;      /* Empty string match at start not wanted */
  BOOL   hitend;                /* Hit the end of the subject at some point */
  BOOL   bsr_anycrlf;           /* \R is just any CRLF, not full Unicode */
  const uschar *start_code;     /* For use when recursing */
  USPTR  start_subject;         /* Start of the subject string */
  USPTR  end_subject;           /* End of the subject string */
  USPTR  start_match_ptr;       /* Start of matched string */
  USPTR  end_match_ptr;         /* Subject position at end match */
  USPTR  start_used_ptr;        /* Earliest consulted character */
  int    partial;               /* PARTIAL options */
  int    end_offset_top;        /* Highwater mark at end of match */
  int    capture_last;          /* Most recent capture number */
  int    start_offset;          /* The start offset value */
  eptrblock *eptrchain;         /* Chain of eptrblocks for tail recursions */
  int    eptrn;                 /* Next free eptrblock */
  recursion_info *recursive;    /* Linked list of recursion data */
  void  *callout_data;          /* To pass back to callouts */
} match_data;

/* A similar structure is used for the same purpose by the DFA matching
functions. */

typedef struct dfa_match_data {
  const uschar *start_code;     /* Start of the compiled pattern */
  const uschar *start_subject;  /* Start of the subject string */
  const uschar *end_subject;    /* End of subject string */
  const uschar *start_used_ptr; /* Earliest consulted character */
  const uschar *tables;         /* Character tables */
  int   start_offset;           /* The start offset value */
  int   moptions;               /* Match options */
  int   poptions;               /* Pattern options */
  int    nltype;                /* Newline type */
  int    nllen;                 /* Newline string length */
  uschar nl[4];                 /* Newline string when fixed */
  void  *callout_data;          /* To pass back to callouts */
} dfa_match_data;

/* Bit definitions for entries in the pcre_ctypes table. */

#define ctype_space   0x01
#define ctype_letter  0x02
#define ctype_digit   0x04
#define ctype_xdigit  0x08
#define ctype_word    0x10   /* alphanumeric or '_' */
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

/* Layout of the UCP type table that translates property names into types and
codes. Each entry used to point directly to a name, but to reduce the number of
relocations in shared libraries, it now has an offset into a single string
instead. */

typedef struct {
  pcre_uint16 name_offset;
  pcre_uint16 type;
  pcre_uint16 value;
} ucp_type_table;


/* Internal shared data tables. These are tables that are used by more than one
of the exported public functions. They have to be "external" in the C sense,
but are not part of the PCRE public API. The data for these tables is in the
pcre_tables.c module. */

extern const int    _pcre_utf8_table1[];
extern const int    _pcre_utf8_table2[];
extern const int    _pcre_utf8_table3[];
extern const uschar _pcre_utf8_table4[];

extern const int    _pcre_utf8_table1_size;

extern const char   _pcre_utt_names[];
extern const ucp_type_table _pcre_utt[];
extern const int _pcre_utt_size;

extern const uschar _pcre_default_tables[];

extern const uschar _pcre_OP_lengths[];


/* Internal shared functions. These are functions that are used by more than
one of the exported public functions. They have to be "external" in the C
sense, but are not part of the PCRE public API. */

extern const uschar *_pcre_find_bracket(const uschar *, BOOL, int);
extern BOOL          _pcre_is_newline(USPTR, int, USPTR, int *, BOOL);
extern int           _pcre_ord2utf8(int, uschar *);
extern real_pcre    *_pcre_try_flipped(const real_pcre *, real_pcre *,
                       const pcre_study_data *, pcre_study_data *);
extern int           _pcre_valid_utf8(USPTR, int);
extern BOOL          _pcre_was_newline(USPTR, int, USPTR, int *, BOOL);
extern BOOL          _pcre_xclass(int, const uschar *);


/* Unicode character database (UCD) */

typedef struct {
  uschar script;
  uschar chartype;
  pcre_int32 other_case;
} ucd_record;

extern const ucd_record  _pcre_ucd_records[];
extern const uschar      _pcre_ucd_stage1[];
extern const pcre_uint16 _pcre_ucd_stage2[];
extern const int         _pcre_ucp_gentype[];


/* UCD access macros */

#define UCD_BLOCK_SIZE 128
#define GET_UCD(ch) (_pcre_ucd_records + \
        _pcre_ucd_stage2[_pcre_ucd_stage1[(ch) / UCD_BLOCK_SIZE] * \
        UCD_BLOCK_SIZE + ch % UCD_BLOCK_SIZE])

#define UCD_CHARTYPE(ch)  GET_UCD(ch)->chartype
#define UCD_SCRIPT(ch)    GET_UCD(ch)->script
#define UCD_CATEGORY(ch)  _pcre_ucp_gentype[UCD_CHARTYPE(ch)]
#define UCD_OTHERCASE(ch) (ch + GET_UCD(ch)->other_case)

#endif

/* End of pcre_internal.h */
