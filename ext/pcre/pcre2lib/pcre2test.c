/*************************************************
*             PCRE2 testing program              *
*************************************************/

/* PCRE2 is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. In 2014
the API was completely revised and '2' was added to the name, because the old
API, which had lasted for 16 years, could not accommodate new requirements. At
the same time, this testing program was re-designed because its original
hacked-up (non-) design had also run out of steam.

                       Written by Philip Hazel
     Original code Copyright (c) 1997-2012 University of Cambridge
    Rewritten code Copyright (c) 2016-2017 University of Cambridge

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


/* This program supports testing of the 8-bit, 16-bit, and 32-bit PCRE2
libraries in a single program, though its input and output are always 8-bit.
It is different from modules such as pcre2_compile.c in the library itself,
which are compiled separately for each code unit width. If two widths are
enabled, for example, pcre2_compile.c is compiled twice. In contrast,
pcre2test.c is compiled only once, and linked with all the enabled libraries.
Therefore, it must not make use of any of the macros from pcre2.h or
pcre2_internal.h that depend on PCRE2_CODE_UNIT_WIDTH. It does, however, make
use of SUPPORT_PCRE2_8, SUPPORT_PCRE2_16, and SUPPORT_PCRE2_32, to ensure that
it references only the enabled library functions. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <errno.h>

#if defined NATIVE_ZOS
#include "pcrzoscs.h"
/* That header is not included in the main PCRE2 distribution because other
apparatus is needed to compile pcre2test for z/OS. The header can be found in
the special z/OS distribution, which is available from www.zaconsultants.net or
from www.cbttape.org. */
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Debugging code enabler */

// #define DEBUG_SHOW_MALLOC_ADDRESSES

/* Both libreadline and libedit are optionally supported. The user-supplied
original patch uses readline/readline.h for libedit, but in at least one system
it is installed as editline/readline.h, so the configuration code now looks for
that first, falling back to readline/readline.h. */

#if defined(SUPPORT_LIBREADLINE) || defined(SUPPORT_LIBEDIT)
#if defined(SUPPORT_LIBREADLINE)
#include <readline/readline.h>
#include <readline/history.h>
#else
#if defined(HAVE_EDITLINE_READLINE_H)
#include <editline/readline.h>
#else
#include <readline/readline.h>
#endif
#endif
#endif

/* Put the test for interactive input into a macro so that it can be changed if
required for different environments. */

#define INTERACTIVE(f) isatty(fileno(f))


/* ---------------------- System-specific definitions ---------------------- */

/* A number of things vary for Windows builds. Originally, pcretest opened its
input and output without "b"; then I was told that "b" was needed in some
environments, so it was added for release 5.0 to both the input and output. (It
makes no difference on Unix-like systems.) Later I was told that it is wrong
for the input on Windows. I've now abstracted the modes into macros that are
set here, to make it easier to fiddle with them, and removed "b" from the input
mode under Windows. The BINARY versions are used when saving/restoring compiled
patterns. */

#if defined(_WIN32) || defined(WIN32)
#include <io.h>                /* For _setmode() */
#include <fcntl.h>             /* For _O_BINARY */
#define INPUT_MODE          "r"
#define OUTPUT_MODE         "wb"
#define BINARY_INPUT_MODE   "rb"
#define BINARY_OUTPUT_MODE  "wb"

#ifndef isatty
#define isatty _isatty         /* This is what Windows calls them, I'm told, */
#endif                         /* though in some environments they seem to   */
                               /* be already defined, hence the #ifndefs.    */
#ifndef fileno
#define fileno _fileno
#endif

/* A user sent this fix for Borland Builder 5 under Windows. */

#ifdef __BORLANDC__
#define _setmode(handle, mode) setmode(handle, mode)
#endif

/* Not Windows */

#else
#include <sys/time.h>          /* These two includes are needed */
#include <sys/resource.h>      /* for setrlimit(). */
#if defined NATIVE_ZOS         /* z/OS uses non-binary I/O */
#define INPUT_MODE   "r"
#define OUTPUT_MODE  "w"
#define BINARY_INPUT_MODE   "rb"
#define BINARY_OUTPUT_MODE  "wb"
#else
#define INPUT_MODE          "rb"
#define OUTPUT_MODE         "wb"
#define BINARY_INPUT_MODE   "rb"
#define BINARY_OUTPUT_MODE  "wb"
#endif
#endif

#ifdef __VMS
#include <ssdef.h>
void vms_setsymbol( char *, char *, int );
#endif

/* VC doesn't support "%td". */
#ifdef _MSC_VER
#define PTR_SPEC "%lu"
#else
#define PTR_SPEC "%td"
#endif

/* ------------------End of system-specific definitions -------------------- */

/* Glueing macros that are used in several places below. */

#define glue(a,b) a##b
#define G(a,b) glue(a,b)

/* Miscellaneous parameters and manifests */

#ifndef CLOCKS_PER_SEC
#ifdef CLK_TCK
#define CLOCKS_PER_SEC CLK_TCK
#else
#define CLOCKS_PER_SEC 100
#endif
#endif

#define CFORE_UNSET UINT32_MAX    /* Unset value for startend/cfail/cerror fields */
#define CONVERT_UNSET UINT32_MAX  /* Unset value for convert_type field */
#define DFA_WS_DIMENSION 1000     /* Size of DFA workspace */
#define DEFAULT_OVECCOUNT 15      /* Default ovector count */
#define JUNK_OFFSET 0xdeadbeef    /* For initializing ovector */
#define LOCALESIZE 32             /* Size of locale name */
#define LOOPREPEAT 500000         /* Default loop count for timing */
#define MALLOCLISTSIZE 20         /* For remembering mallocs */
#define PARENS_NEST_DEFAULT 220   /* Default parentheses nest limit */
#define PATSTACKSIZE 20           /* Pattern stack for save/restore testing */
#define REPLACE_MODSIZE 100       /* Field for reading 8-bit replacement */
#define VERSION_SIZE 64           /* Size of buffer for the version strings */

/* Make sure the buffer into which replacement strings are copied is big enough
to hold them as 32-bit code units. */

#define REPLACE_BUFFSIZE 1024   /* This is a byte value */

/* Execution modes */

#define PCRE8_MODE   8
#define PCRE16_MODE 16
#define PCRE32_MODE 32

/* Processing returns */

enum { PR_OK, PR_SKIP, PR_ABEND };

/* The macro PRINTABLE determines whether to print an output character as-is or
as a hex value when showing compiled patterns. is We use it in cases when the
locale has not been explicitly changed, so as to get consistent output from
systems that differ in their output from isprint() even in the "C" locale. */

#ifdef EBCDIC
#define PRINTABLE(c) ((c) >= 64 && (c) < 255)
#else
#define PRINTABLE(c) ((c) >= 32 && (c) < 127)
#endif

#define PRINTOK(c) ((use_tables != NULL && c < 256)? isprint(c) : PRINTABLE(c))

/* We have to include some of the library source files because we need
to use some of the macros, internal structure definitions, and other internal
values - pcre2test has "inside information" compared to an application program
that strictly follows the PCRE2 API.

Before including pcre2_internal.h we define PRIV so that it does not get
defined therein. This ensures that PRIV names in the included files do not
clash with those in the libraries. Also, although pcre2_internal.h does itself
include pcre2.h, we explicitly include it beforehand, along with pcre2posix.h,
so that the PCRE2_EXP_xxx macros get set appropriately for an application, not
for building the library. */

#define PRIV(name) name
#define PCRE2_CODE_UNIT_WIDTH 0
#include "pcre2.h"
#include "pcre2posix.h"
#include "pcre2_internal.h"

/* We need access to some of the data tables that PCRE2 uses. Defining
PCRE2_PCRETEST makes some minor changes in the files. The previous definition
of PRIV avoids name clashes. */

#define PCRE2_PCRE2TEST
#include "pcre2_tables.c"
#include "pcre2_ucd.c"

/* 32-bit integer values in the input are read by strtoul() or strtol(). The
check needed for overflow depends on whether long ints are in fact longer than
ints. They are defined not to be shorter. */

#if ULONG_MAX > UINT32_MAX
#define U32OVERFLOW(x) (x > UINT32_MAX)
#else
#define U32OVERFLOW(x) (x == UINT32_MAX)
#endif

#if LONG_MAX > INT32_MAX
#define S32OVERFLOW(x) (x > INT32_MAX || x < INT32_MIN)
#else
#define S32OVERFLOW(x) (x == INT32_MAX || x == INT32_MIN)
#endif

/* When PCRE2_CODE_UNIT_WIDTH is zero, pcre2_internal.h does not include
pcre2_intmodedep.h, which is where mode-dependent macros and structures are
defined. We can now include it for each supported code unit width. Because
PCRE2_CODE_UNIT_WIDTH was defined as zero before including pcre2.h, it will
have left PCRE2_SUFFIX defined as a no-op. We must re-define it appropriately
while including these files, and then restore it to a no-op. Because LINK_SIZE
may be changed in 16-bit mode and forced to 1 in 32-bit mode, the order of
these inclusions should not be changed. */

#undef PCRE2_SUFFIX
#undef PCRE2_CODE_UNIT_WIDTH

#ifdef   SUPPORT_PCRE2_8
#define  PCRE2_CODE_UNIT_WIDTH 8
#define  PCRE2_SUFFIX(a) G(a,8)
#include "pcre2_intmodedep.h"
#include "pcre2_printint.c"
#undef   PCRE2_CODE_UNIT_WIDTH
#undef   PCRE2_SUFFIX
#endif   /* SUPPORT_PCRE2_8 */

#ifdef   SUPPORT_PCRE2_16
#define  PCRE2_CODE_UNIT_WIDTH 16
#define  PCRE2_SUFFIX(a) G(a,16)
#include "pcre2_intmodedep.h"
#include "pcre2_printint.c"
#undef   PCRE2_CODE_UNIT_WIDTH
#undef   PCRE2_SUFFIX
#endif   /* SUPPORT_PCRE2_16 */

#ifdef   SUPPORT_PCRE2_32
#define  PCRE2_CODE_UNIT_WIDTH 32
#define  PCRE2_SUFFIX(a) G(a,32)
#include "pcre2_intmodedep.h"
#include "pcre2_printint.c"
#undef   PCRE2_CODE_UNIT_WIDTH
#undef   PCRE2_SUFFIX
#endif   /* SUPPORT_PCRE2_32 */

#define PCRE2_SUFFIX(a) a

/* We need to be able to check input text for UTF-8 validity, whatever code
widths are actually available, because the input to pcre2test is always in
8-bit code units. So we include the UTF validity checking function for 8-bit
code units. */

extern int valid_utf(PCRE2_SPTR8, PCRE2_SIZE, PCRE2_SIZE *);

#define  PCRE2_CODE_UNIT_WIDTH 8
#undef   PCRE2_SPTR
#define  PCRE2_SPTR PCRE2_SPTR8
#include "pcre2_valid_utf.c"
#undef   PCRE2_CODE_UNIT_WIDTH
#undef   PCRE2_SPTR

/* If we have 8-bit support, default to it; if there is also 16-or 32-bit
support, it can be selected by a command-line option. If there is no 8-bit
support, there must be 16-bit or 32-bit support, so default to one of them. The
config function, JIT stack, contexts, and version string are the same in all
modes, so use the form of the first that is available. */

#if defined SUPPORT_PCRE2_8
#define DEFAULT_TEST_MODE PCRE8_MODE
#define VERSION_TYPE PCRE2_UCHAR8
#define PCRE2_CONFIG pcre2_config_8
#define PCRE2_JIT_STACK pcre2_jit_stack_8
#define PCRE2_REAL_GENERAL_CONTEXT pcre2_real_general_context_8
#define PCRE2_REAL_COMPILE_CONTEXT pcre2_real_compile_context_8
#define PCRE2_REAL_CONVERT_CONTEXT pcre2_real_convert_context_8
#define PCRE2_REAL_MATCH_CONTEXT pcre2_real_match_context_8

#elif defined SUPPORT_PCRE2_16
#define DEFAULT_TEST_MODE PCRE16_MODE
#define VERSION_TYPE PCRE2_UCHAR16
#define PCRE2_CONFIG pcre2_config_16
#define PCRE2_JIT_STACK pcre2_jit_stack_16
#define PCRE2_REAL_GENERAL_CONTEXT pcre2_real_general_context_16
#define PCRE2_REAL_COMPILE_CONTEXT pcre2_real_compile_context_16
#define PCRE2_REAL_CONVERT_CONTEXT pcre2_real_convert_context_16
#define PCRE2_REAL_MATCH_CONTEXT pcre2_real_match_context_16

#elif defined SUPPORT_PCRE2_32
#define DEFAULT_TEST_MODE PCRE32_MODE
#define VERSION_TYPE PCRE2_UCHAR32
#define PCRE2_CONFIG pcre2_config_32
#define PCRE2_JIT_STACK pcre2_jit_stack_32
#define PCRE2_REAL_GENERAL_CONTEXT pcre2_real_general_context_32
#define PCRE2_REAL_COMPILE_CONTEXT pcre2_real_compile_context_32
#define PCRE2_REAL_CONVERT_CONTEXT pcre2_real_convert_context_32
#define PCRE2_REAL_MATCH_CONTEXT pcre2_real_match_context_32
#endif

/* ------------- Structure and table for handling #-commands ------------- */

typedef struct cmdstruct {
  const char *name;
  int  value;
} cmdstruct;

enum { CMD_FORBID_UTF, CMD_LOAD, CMD_NEWLINE_DEFAULT, CMD_PATTERN,
  CMD_PERLTEST, CMD_POP, CMD_POPCOPY, CMD_SAVE, CMD_SUBJECT, CMD_UNKNOWN };

static cmdstruct cmdlist[] = {
  { "forbid_utf",      CMD_FORBID_UTF },
  { "load",            CMD_LOAD },
  { "newline_default", CMD_NEWLINE_DEFAULT },
  { "pattern",         CMD_PATTERN },
  { "perltest",        CMD_PERLTEST },
  { "pop",             CMD_POP },
  { "popcopy",         CMD_POPCOPY },
  { "save",            CMD_SAVE },
  { "subject",         CMD_SUBJECT }};

#define cmdlistcount (sizeof(cmdlist)/sizeof(cmdstruct))

/* ------------- Structures and tables for handling modifiers -------------- */

/* Table of names for newline types. Must be kept in step with the definitions
of PCRE2_NEWLINE_xx in pcre2.h. */

static const char *newlines[] = {
  "DEFAULT", "CR", "LF", "CRLF", "ANY", "ANYCRLF", "NUL" };

/* Structure and table for handling pattern conversion types. */

typedef struct convertstruct {
  const char *name;
  uint32_t option;
} convertstruct;

static convertstruct convertlist[] = {
  { "glob",                   PCRE2_CONVERT_GLOB },
  { "glob_no_starstar",       PCRE2_CONVERT_GLOB_NO_STARSTAR },
  { "glob_no_wild_separator", PCRE2_CONVERT_GLOB_NO_WILD_SEPARATOR },
  { "posix_basic",            PCRE2_CONVERT_POSIX_BASIC },
  { "posix_extended",         PCRE2_CONVERT_POSIX_EXTENDED },
  { "unset",                  CONVERT_UNSET }};

#define convertlistcount (sizeof(convertlist)/sizeof(convertstruct))

/* Modifier types and applicability */

enum { MOD_CTC,    /* Applies to a compile context */
       MOD_CTM,    /* Applies to a match context */
       MOD_PAT,    /* Applies to a pattern */
       MOD_PATP,   /* Ditto, OK for Perl test */
       MOD_DAT,    /* Applies to a data line */
       MOD_PD,     /* Applies to a pattern or a data line */
       MOD_PDP,    /* As MOD_PD, OK for Perl test */
       MOD_PND,    /* As MOD_PD, but not for a default pattern */
       MOD_PNDP,   /* As MOD_PND, OK for Perl test */
       MOD_CHR,    /* Is a single character */
       MOD_CON,    /* Is a "convert" type/options list */
       MOD_CTL,    /* Is a control bit */
       MOD_BSR,    /* Is a BSR value */
       MOD_IN2,    /* Is one or two unsigned integers */
       MOD_INS,    /* Is a signed integer */
       MOD_INT,    /* Is an unsigned integer */
       MOD_IND,    /* Is an unsigned integer, but no value => default */
       MOD_NL,     /* Is a newline value */
       MOD_NN,     /* Is a number or a name; more than one may occur */
       MOD_OPT,    /* Is an option bit */
       MOD_SIZ,    /* Is a PCRE2_SIZE value */
       MOD_STR };  /* Is a string */

/* Control bits. Some apply to compiling, some to matching, but some can be set
either on a pattern or a data line, so they must all be distinct. There are now
so many of them that they are split into two fields. */

#define CTL_AFTERTEXT                    0x00000001u
#define CTL_ALLAFTERTEXT                 0x00000002u
#define CTL_ALLCAPTURES                  0x00000004u
#define CTL_ALLUSEDTEXT                  0x00000008u
#define CTL_ALTGLOBAL                    0x00000010u
#define CTL_BINCODE                      0x00000020u
#define CTL_CALLOUT_CAPTURE              0x00000040u
#define CTL_CALLOUT_INFO                 0x00000080u
#define CTL_CALLOUT_NONE                 0x00000100u
#define CTL_DFA                          0x00000200u
#define CTL_EXPAND                       0x00000400u
#define CTL_FINDLIMITS                   0x00000800u
#define CTL_FRAMESIZE                    0x00001000u
#define CTL_FULLBINCODE                  0x00002000u
#define CTL_GETALL                       0x00004000u
#define CTL_GLOBAL                       0x00008000u
#define CTL_HEXPAT                       0x00010000u  /* Same word as USE_LENGTH */
#define CTL_INFO                         0x00020000u
#define CTL_JITFAST                      0x00040000u
#define CTL_JITVERIFY                    0x00080000u
#define CTL_MARK                         0x00100000u
#define CTL_MEMORY                       0x00200000u
#define CTL_NULLCONTEXT                  0x00400000u
#define CTL_POSIX                        0x00800000u
#define CTL_POSIX_NOSUB                  0x01000000u
#define CTL_PUSH                         0x02000000u  /* These three must be */
#define CTL_PUSHCOPY                     0x04000000u  /*   all in the same */
#define CTL_PUSHTABLESCOPY               0x08000000u  /*     word. */
#define CTL_STARTCHAR                    0x10000000u
#define CTL_USE_LENGTH                   0x20000000u  /* Same word as HEXPAT */
#define CTL_UTF8_INPUT                   0x40000000u
#define CTL_ZERO_TERMINATE               0x80000000u

/* Combinations */

#define CTL_DEBUG            (CTL_FULLBINCODE|CTL_INFO)  /* For setting */
#define CTL_ANYINFO          (CTL_DEBUG|CTL_BINCODE|CTL_CALLOUT_INFO)
#define CTL_ANYGLOB          (CTL_ALTGLOBAL|CTL_GLOBAL)

/* Second control word */

#define CTL2_SUBSTITUTE_EXTENDED         0x00000001u
#define CTL2_SUBSTITUTE_OVERFLOW_LENGTH  0x00000002u
#define CTL2_SUBSTITUTE_UNKNOWN_UNSET    0x00000004u
#define CTL2_SUBSTITUTE_UNSET_EMPTY      0x00000008u
#define CTL2_SUBJECT_LITERAL             0x00000010u
#define CTL2_CALLOUT_NO_WHERE            0x00000020u

#define CTL2_NL_SET                      0x40000000u  /* Informational */
#define CTL2_BSR_SET                     0x80000000u  /* Informational */

/* These are the matching controls that may be set either on a pattern or on a
data line. They are copied from the pattern controls as initial settings for
data line controls Note that CTL_MEMORY is not included here, because it does
different things in the two cases. */

#define CTL_ALLPD  (CTL_AFTERTEXT|\
                    CTL_ALLAFTERTEXT|\
                    CTL_ALLCAPTURES|\
                    CTL_ALLUSEDTEXT|\
                    CTL_ALTGLOBAL|\
                    CTL_GLOBAL|\
                    CTL_MARK|\
                    CTL_STARTCHAR|\
                    CTL_UTF8_INPUT)

#define CTL2_ALLPD (CTL2_SUBSTITUTE_EXTENDED|\
                    CTL2_SUBSTITUTE_OVERFLOW_LENGTH|\
                    CTL2_SUBSTITUTE_UNKNOWN_UNSET|\
                    CTL2_SUBSTITUTE_UNSET_EMPTY)

/* Structures for holding modifier information for patterns and subject strings
(data). Fields containing modifiers that can be set either for a pattern or a
subject must be at the start and in the same order in both cases so that the
same offset in the big table below works for both. */

typedef struct patctl {    /* Structure for pattern modifiers. */
  uint32_t  options;       /* Must be in same position as datctl */
  uint32_t  control;       /* Must be in same position as datctl */
  uint32_t  control2;      /* Must be in same position as datctl */
  uint32_t  jitstack;      /* Must be in same position as datctl */
   uint8_t  replacement[REPLACE_MODSIZE];  /* So must this */
  uint32_t  jit;
  uint32_t  stackguard_test;
  uint32_t  tables_id;
  uint32_t  convert_type;
  uint32_t  convert_length;
  uint32_t  convert_glob_escape;
  uint32_t  convert_glob_separator;
  uint32_t  regerror_buffsize;
   uint8_t  locale[LOCALESIZE];
} patctl;

#define MAXCPYGET 10
#define LENCPYGET 64

typedef struct datctl {    /* Structure for data line modifiers. */
  uint32_t  options;       /* Must be in same position as patctl */
  uint32_t  control;       /* Must be in same position as patctl */
  uint32_t  control2;      /* Must be in same position as patctl */
  uint32_t  jitstack;      /* Must be in same position as patctl */
   uint8_t  replacement[REPLACE_MODSIZE];  /* So must this */
  uint32_t  startend[2];
  uint32_t  cerror[2];
  uint32_t  cfail[2];
   int32_t  callout_data;
   int32_t  copy_numbers[MAXCPYGET];
   int32_t  get_numbers[MAXCPYGET];
  uint32_t  oveccount;
  uint32_t  offset;
  uint8_t   copy_names[LENCPYGET];
  uint8_t   get_names[LENCPYGET];
} datctl;

/* Ids for which context to modify. */

enum { CTX_PAT,            /* Active pattern context */
       CTX_POPPAT,         /* Ditto, for a popped pattern */
       CTX_DEFPAT,         /* Default pattern context */
       CTX_DAT,            /* Active data (match) context */
       CTX_DEFDAT };       /* Default data (match) context */

/* Macros to simplify the big table below. */

#define CO(name) offsetof(PCRE2_REAL_COMPILE_CONTEXT, name)
#define MO(name) offsetof(PCRE2_REAL_MATCH_CONTEXT, name)
#define PO(name) offsetof(patctl, name)
#define PD(name) PO(name)
#define DO(name) offsetof(datctl, name)

/* Table of all long-form modifiers. Must be in collating sequence of modifier
name because it is searched by binary chop. */

typedef struct modstruct {
  const char   *name;
  uint16_t      which;
  uint16_t      type;
  uint32_t      value;
  PCRE2_SIZE    offset;
} modstruct;

static modstruct modlist[] = {
  { "aftertext",                  MOD_PNDP, MOD_CTL, CTL_AFTERTEXT,              PO(control) },
  { "allaftertext",               MOD_PNDP, MOD_CTL, CTL_ALLAFTERTEXT,           PO(control) },
  { "allcaptures",                MOD_PND,  MOD_CTL, CTL_ALLCAPTURES,            PO(control) },
  { "allow_empty_class",          MOD_PAT,  MOD_OPT, PCRE2_ALLOW_EMPTY_CLASS,    PO(options) },
  { "allow_surrogate_escapes",    MOD_CTC,  MOD_OPT, PCRE2_EXTRA_ALLOW_SURROGATE_ESCAPES, CO(extra_options) },
  { "allusedtext",                MOD_PNDP, MOD_CTL, CTL_ALLUSEDTEXT,            PO(control) },
  { "alt_bsux",                   MOD_PAT,  MOD_OPT, PCRE2_ALT_BSUX,             PO(options) },
  { "alt_circumflex",             MOD_PAT,  MOD_OPT, PCRE2_ALT_CIRCUMFLEX,       PO(options) },
  { "alt_verbnames",              MOD_PAT,  MOD_OPT, PCRE2_ALT_VERBNAMES,        PO(options) },
  { "altglobal",                  MOD_PND,  MOD_CTL, CTL_ALTGLOBAL,              PO(control) },
  { "anchored",                   MOD_PD,   MOD_OPT, PCRE2_ANCHORED,             PD(options) },
  { "auto_callout",               MOD_PAT,  MOD_OPT, PCRE2_AUTO_CALLOUT,         PO(options) },
  { "bad_escape_is_literal",      MOD_CTC,  MOD_OPT, PCRE2_EXTRA_BAD_ESCAPE_IS_LITERAL, CO(extra_options) },
  { "bincode",                    MOD_PAT,  MOD_CTL, CTL_BINCODE,                PO(control) },
  { "bsr",                        MOD_CTC,  MOD_BSR, 0,                          CO(bsr_convention) },
  { "callout_capture",            MOD_DAT,  MOD_CTL, CTL_CALLOUT_CAPTURE,        DO(control) },
  { "callout_data",               MOD_DAT,  MOD_INS, 0,                          DO(callout_data) },
  { "callout_error",              MOD_DAT,  MOD_IN2, 0,                          DO(cerror) },
  { "callout_fail",               MOD_DAT,  MOD_IN2, 0,                          DO(cfail) },
  { "callout_info",               MOD_PAT,  MOD_CTL, CTL_CALLOUT_INFO,           PO(control) },
  { "callout_no_where",           MOD_DAT,  MOD_CTL, CTL2_CALLOUT_NO_WHERE,      DO(control2) },
  { "callout_none",               MOD_DAT,  MOD_CTL, CTL_CALLOUT_NONE,           DO(control) },
  { "caseless",                   MOD_PATP, MOD_OPT, PCRE2_CASELESS,             PO(options) },
  { "convert",                    MOD_PAT,  MOD_CON, 0,                          PO(convert_type) },
  { "convert_glob_escape",        MOD_PAT,  MOD_CHR, 0,                          PO(convert_glob_escape) },
  { "convert_glob_separator",     MOD_PAT,  MOD_CHR, 0,                          PO(convert_glob_separator) },
  { "convert_length",             MOD_PAT,  MOD_INT, 0,                          PO(convert_length) },
  { "copy",                       MOD_DAT,  MOD_NN,  DO(copy_numbers),           DO(copy_names) },
  { "debug",                      MOD_PAT,  MOD_CTL, CTL_DEBUG,                  PO(control) },
  { "depth_limit",                MOD_CTM,  MOD_INT, 0,                          MO(depth_limit) },
  { "dfa",                        MOD_DAT,  MOD_CTL, CTL_DFA,                    DO(control) },
  { "dfa_restart",                MOD_DAT,  MOD_OPT, PCRE2_DFA_RESTART,          DO(options) },
  { "dfa_shortest",               MOD_DAT,  MOD_OPT, PCRE2_DFA_SHORTEST,         DO(options) },
  { "dollar_endonly",             MOD_PAT,  MOD_OPT, PCRE2_DOLLAR_ENDONLY,       PO(options) },
  { "dotall",                     MOD_PATP, MOD_OPT, PCRE2_DOTALL,               PO(options) },
  { "dupnames",                   MOD_PATP, MOD_OPT, PCRE2_DUPNAMES,             PO(options) },
  { "endanchored",                MOD_PD,   MOD_OPT, PCRE2_ENDANCHORED,          PD(options) },
  { "expand",                     MOD_PAT,  MOD_CTL, CTL_EXPAND,                 PO(control) },
  { "extended",                   MOD_PATP, MOD_OPT, PCRE2_EXTENDED,             PO(options) },
  { "extended_more",              MOD_PATP, MOD_OPT, PCRE2_EXTENDED_MORE,        PO(options) },
  { "find_limits",                MOD_DAT,  MOD_CTL, CTL_FINDLIMITS,             DO(control) },
  { "firstline",                  MOD_PAT,  MOD_OPT, PCRE2_FIRSTLINE,            PO(options) },
  { "framesize",                  MOD_PAT,  MOD_CTL, CTL_FRAMESIZE,              PO(control) },
  { "fullbincode",                MOD_PAT,  MOD_CTL, CTL_FULLBINCODE,            PO(control) },
  { "get",                        MOD_DAT,  MOD_NN,  DO(get_numbers),            DO(get_names) },
  { "getall",                     MOD_DAT,  MOD_CTL, CTL_GETALL,                 DO(control) },
  { "global",                     MOD_PNDP, MOD_CTL, CTL_GLOBAL,                 PO(control) },
  { "heap_limit",                 MOD_CTM,  MOD_INT, 0,                          MO(heap_limit) },
  { "hex",                        MOD_PAT,  MOD_CTL, CTL_HEXPAT,                 PO(control) },
  { "info",                       MOD_PAT,  MOD_CTL, CTL_INFO,                   PO(control) },
  { "jit",                        MOD_PAT,  MOD_IND, 7,                          PO(jit) },
  { "jitfast",                    MOD_PAT,  MOD_CTL, CTL_JITFAST,                PO(control) },
  { "jitstack",                   MOD_PNDP, MOD_INT, 0,                          PO(jitstack) },
  { "jitverify",                  MOD_PAT,  MOD_CTL, CTL_JITVERIFY,              PO(control) },
  { "literal",                    MOD_PAT,  MOD_OPT, PCRE2_LITERAL,              PO(options) },
  { "locale",                     MOD_PAT,  MOD_STR, LOCALESIZE,                 PO(locale) },
  { "mark",                       MOD_PNDP, MOD_CTL, CTL_MARK,                   PO(control) },
  { "match_limit",                MOD_CTM,  MOD_INT, 0,                          MO(match_limit) },
  { "match_line",                 MOD_CTC,  MOD_OPT, PCRE2_EXTRA_MATCH_LINE,     CO(extra_options) },
  { "match_unset_backref",        MOD_PAT,  MOD_OPT, PCRE2_MATCH_UNSET_BACKREF,  PO(options) },
  { "match_word",                 MOD_CTC,  MOD_OPT, PCRE2_EXTRA_MATCH_WORD,     CO(extra_options) },
  { "max_pattern_length",         MOD_CTC,  MOD_SIZ, 0,                          CO(max_pattern_length) },
  { "memory",                     MOD_PD,   MOD_CTL, CTL_MEMORY,                 PD(control) },
  { "multiline",                  MOD_PATP, MOD_OPT, PCRE2_MULTILINE,            PO(options) },
  { "never_backslash_c",          MOD_PAT,  MOD_OPT, PCRE2_NEVER_BACKSLASH_C,    PO(options) },
  { "never_ucp",                  MOD_PAT,  MOD_OPT, PCRE2_NEVER_UCP,            PO(options) },
  { "never_utf",                  MOD_PAT,  MOD_OPT, PCRE2_NEVER_UTF,            PO(options) },
  { "newline",                    MOD_CTC,  MOD_NL,  0,                          CO(newline_convention) },
  { "no_auto_capture",            MOD_PAT,  MOD_OPT, PCRE2_NO_AUTO_CAPTURE,      PO(options) },
  { "no_auto_possess",            MOD_PATP, MOD_OPT, PCRE2_NO_AUTO_POSSESS,      PO(options) },
  { "no_dotstar_anchor",          MOD_PAT,  MOD_OPT, PCRE2_NO_DOTSTAR_ANCHOR,    PO(options) },
  { "no_jit",                     MOD_DAT,  MOD_OPT, PCRE2_NO_JIT,               DO(options) },
  { "no_start_optimize",          MOD_PATP, MOD_OPT, PCRE2_NO_START_OPTIMIZE,    PO(options) },
  { "no_utf_check",               MOD_PD,   MOD_OPT, PCRE2_NO_UTF_CHECK,         PD(options) },
  { "notbol",                     MOD_DAT,  MOD_OPT, PCRE2_NOTBOL,               DO(options) },
  { "notempty",                   MOD_DAT,  MOD_OPT, PCRE2_NOTEMPTY,             DO(options) },
  { "notempty_atstart",           MOD_DAT,  MOD_OPT, PCRE2_NOTEMPTY_ATSTART,     DO(options) },
  { "noteol",                     MOD_DAT,  MOD_OPT, PCRE2_NOTEOL,               DO(options) },
  { "null_context",               MOD_PD,   MOD_CTL, CTL_NULLCONTEXT,            PO(control) },
  { "offset",                     MOD_DAT,  MOD_INT, 0,                          DO(offset) },
  { "offset_limit",               MOD_CTM,  MOD_SIZ, 0,                          MO(offset_limit)},
  { "ovector",                    MOD_DAT,  MOD_INT, 0,                          DO(oveccount) },
  { "parens_nest_limit",          MOD_CTC,  MOD_INT, 0,                          CO(parens_nest_limit) },
  { "partial_hard",               MOD_DAT,  MOD_OPT, PCRE2_PARTIAL_HARD,         DO(options) },
  { "partial_soft",               MOD_DAT,  MOD_OPT, PCRE2_PARTIAL_SOFT,         DO(options) },
  { "ph",                         MOD_DAT,  MOD_OPT, PCRE2_PARTIAL_HARD,         DO(options) },
  { "posix",                      MOD_PAT,  MOD_CTL, CTL_POSIX,                  PO(control) },
  { "posix_nosub",                MOD_PAT,  MOD_CTL, CTL_POSIX|CTL_POSIX_NOSUB,  PO(control) },
  { "posix_startend",             MOD_DAT,  MOD_IN2, 0,                          DO(startend) },
  { "ps",                         MOD_DAT,  MOD_OPT, PCRE2_PARTIAL_SOFT,         DO(options) },
  { "push",                       MOD_PAT,  MOD_CTL, CTL_PUSH,                   PO(control) },
  { "pushcopy",                   MOD_PAT,  MOD_CTL, CTL_PUSHCOPY,               PO(control) },
  { "pushtablescopy",             MOD_PAT,  MOD_CTL, CTL_PUSHTABLESCOPY,         PO(control) },
  { "recursion_limit",            MOD_CTM,  MOD_INT, 0,                          MO(depth_limit) },  /* Obsolete synonym */
  { "regerror_buffsize",          MOD_PAT,  MOD_INT, 0,                          PO(regerror_buffsize) },
  { "replace",                    MOD_PND,  MOD_STR, REPLACE_MODSIZE,            PO(replacement) },
  { "stackguard",                 MOD_PAT,  MOD_INT, 0,                          PO(stackguard_test) },
  { "startchar",                  MOD_PND,  MOD_CTL, CTL_STARTCHAR,              PO(control) },
  { "startoffset",                MOD_DAT,  MOD_INT, 0,                          DO(offset) },
  { "subject_literal",            MOD_PATP, MOD_CTL, CTL2_SUBJECT_LITERAL,       PO(control2) },
  { "substitute_extended",        MOD_PND,  MOD_CTL, CTL2_SUBSTITUTE_EXTENDED,   PO(control2) },
  { "substitute_overflow_length", MOD_PND,  MOD_CTL, CTL2_SUBSTITUTE_OVERFLOW_LENGTH, PO(control2) },
  { "substitute_unknown_unset",   MOD_PND,  MOD_CTL, CTL2_SUBSTITUTE_UNKNOWN_UNSET, PO(control2) },
  { "substitute_unset_empty",     MOD_PND,  MOD_CTL, CTL2_SUBSTITUTE_UNSET_EMPTY, PO(control2) },
  { "tables",                     MOD_PAT,  MOD_INT, 0,                          PO(tables_id) },
  { "ucp",                        MOD_PATP, MOD_OPT, PCRE2_UCP,                  PO(options) },
  { "ungreedy",                   MOD_PAT,  MOD_OPT, PCRE2_UNGREEDY,             PO(options) },
  { "use_length",                 MOD_PAT,  MOD_CTL, CTL_USE_LENGTH,             PO(control) },
  { "use_offset_limit",           MOD_PAT,  MOD_OPT, PCRE2_USE_OFFSET_LIMIT,     PO(options) },
  { "utf",                        MOD_PATP, MOD_OPT, PCRE2_UTF,                  PO(options) },
  { "utf8_input",                 MOD_PAT,  MOD_CTL, CTL_UTF8_INPUT,             PO(control) },
  { "zero_terminate",             MOD_DAT,  MOD_CTL, CTL_ZERO_TERMINATE,         DO(control) }
};

#define MODLISTCOUNT sizeof(modlist)/sizeof(modstruct)

/* Controls and options that are supported for use with the POSIX interface. */

#define POSIX_SUPPORTED_COMPILE_OPTIONS ( \
  PCRE2_CASELESS|PCRE2_DOTALL|PCRE2_LITERAL|PCRE2_MULTILINE|PCRE2_UCP| \
  PCRE2_UTF|PCRE2_UNGREEDY)

#define POSIX_SUPPORTED_COMPILE_EXTRA_OPTIONS (0)

#define POSIX_SUPPORTED_COMPILE_CONTROLS ( \
  CTL_AFTERTEXT|CTL_ALLAFTERTEXT|CTL_EXPAND|CTL_HEXPAT|CTL_POSIX| \
  CTL_POSIX_NOSUB|CTL_USE_LENGTH)

#define POSIX_SUPPORTED_COMPILE_CONTROLS2 (0)

#define POSIX_SUPPORTED_MATCH_OPTIONS ( \
  PCRE2_NOTBOL|PCRE2_NOTEMPTY|PCRE2_NOTEOL)

#define POSIX_SUPPORTED_MATCH_CONTROLS  (CTL_AFTERTEXT|CTL_ALLAFTERTEXT)
#define POSIX_SUPPORTED_MATCH_CONTROLS2 (0)

/* Control bits that are not ignored with 'push'. */

#define PUSH_SUPPORTED_COMPILE_CONTROLS ( \
  CTL_BINCODE|CTL_CALLOUT_INFO|CTL_FULLBINCODE|CTL_HEXPAT|CTL_INFO| \
  CTL_JITVERIFY|CTL_MEMORY|CTL_FRAMESIZE|CTL_PUSH|CTL_PUSHCOPY| \
  CTL_PUSHTABLESCOPY|CTL_USE_LENGTH)

#define PUSH_SUPPORTED_COMPILE_CONTROLS2 (CTL2_BSR_SET|CTL2_NL_SET)

/* Controls that apply only at compile time with 'push'. */

#define PUSH_COMPILE_ONLY_CONTROLS   CTL_JITVERIFY
#define PUSH_COMPILE_ONLY_CONTROLS2  (0)

/* Controls that are forbidden with #pop or #popcopy. */

#define NOTPOP_CONTROLS (CTL_HEXPAT|CTL_POSIX|CTL_POSIX_NOSUB|CTL_PUSH| \
  CTL_PUSHCOPY|CTL_PUSHTABLESCOPY|CTL_USE_LENGTH)

/* Pattern controls that are mutually exclusive. At present these are all in
the first control word. Note that CTL_POSIX_NOSUB is always accompanied by
CTL_POSIX, so it doesn't need its own entries. */

static uint32_t exclusive_pat_controls[] = {
  CTL_POSIX    | CTL_PUSH,
  CTL_POSIX    | CTL_PUSHCOPY,
  CTL_POSIX    | CTL_PUSHTABLESCOPY,
  CTL_PUSH     | CTL_PUSHCOPY,
  CTL_PUSH     | CTL_PUSHTABLESCOPY,
  CTL_PUSHCOPY | CTL_PUSHTABLESCOPY,
  CTL_EXPAND   | CTL_HEXPAT };

/* Data controls that are mutually exclusive. At present these are all in the
first control word. */

static uint32_t exclusive_dat_controls[] = {
  CTL_ALLUSEDTEXT | CTL_STARTCHAR,
  CTL_FINDLIMITS  | CTL_NULLCONTEXT };

/* Table of single-character abbreviated modifiers. The index field is
initialized to -1, but the first time the modifier is encountered, it is filled
in with the index of the full entry in modlist, to save repeated searching when
processing multiple test items. This short list is searched serially, so its
order does not matter. */

typedef struct c1modstruct {
  const char *fullname;
  uint32_t    onechar;
  int         index;
} c1modstruct;

static c1modstruct c1modlist[] = {
  { "bincode",         'B',           -1 },
  { "info",            'I',           -1 },
  { "global",          'g',           -1 },
  { "caseless",        'i',           -1 },
  { "multiline",       'm',           -1 },
  { "no_auto_capture", 'n',           -1 },
  { "dotall",          's',           -1 },
  { "extended",        'x',           -1 }
};

#define C1MODLISTCOUNT sizeof(c1modlist)/sizeof(c1modstruct)

/* Table of arguments for the -C command line option. Use macros to make the
table itself easier to read. */

#if defined SUPPORT_PCRE2_8
#define SUPPORT_8 1
#endif
#if defined SUPPORT_PCRE2_16
#define SUPPORT_16 1
#endif
#if defined SUPPORT_PCRE2_32
#define SUPPORT_32 1
#endif

#ifndef SUPPORT_8
#define SUPPORT_8 0
#endif
#ifndef SUPPORT_16
#define SUPPORT_16 0
#endif
#ifndef SUPPORT_32
#define SUPPORT_32 0
#endif

#ifdef EBCDIC
#define SUPPORT_EBCDIC 1
#define EBCDIC_NL CHAR_LF
#else
#define SUPPORT_EBCDIC 0
#define EBCDIC_NL 0
#endif

#ifdef NEVER_BACKSLASH_C
#define BACKSLASH_C 0
#else
#define BACKSLASH_C 1
#endif

typedef struct coptstruct {
  const char *name;
  uint32_t    type;
  uint32_t    value;
} coptstruct;

enum { CONF_BSR,
       CONF_FIX,
       CONF_FIZ,
       CONF_INT,
       CONF_NL
};

static coptstruct coptlist[] = {
  { "backslash-C", CONF_FIX, BACKSLASH_C },
  { "bsr",         CONF_BSR, PCRE2_CONFIG_BSR },
  { "ebcdic",      CONF_FIX, SUPPORT_EBCDIC },
  { "ebcdic-nl",   CONF_FIZ, EBCDIC_NL },
  { "jit",         CONF_INT, PCRE2_CONFIG_JIT },
  { "linksize",    CONF_INT, PCRE2_CONFIG_LINKSIZE },
  { "newline",     CONF_NL,  PCRE2_CONFIG_NEWLINE },
  { "pcre2-16",    CONF_FIX, SUPPORT_16 },
  { "pcre2-32",    CONF_FIX, SUPPORT_32 },
  { "pcre2-8",     CONF_FIX, SUPPORT_8 },
  { "unicode",     CONF_INT, PCRE2_CONFIG_UNICODE }
};

#define COPTLISTCOUNT sizeof(coptlist)/sizeof(coptstruct)

#undef SUPPORT_8
#undef SUPPORT_16
#undef SUPPORT_32
#undef SUPPORT_EBCDIC


/* ----------------------- Static variables ------------------------ */

static FILE *infile;
static FILE *outfile;

static const void *last_callout_mark;
static PCRE2_JIT_STACK *jit_stack = NULL;
static size_t jit_stack_size = 0;

static BOOL first_callout;
static BOOL jit_was_used;
static BOOL restrict_for_perl_test = FALSE;
static BOOL show_memory = FALSE;

static int code_unit_size;                    /* Bytes */
static int jitrc;                             /* Return from JIT compile */
static int test_mode = DEFAULT_TEST_MODE;
static int timeit = 0;
static int timeitm = 0;

clock_t total_compile_time = 0;
clock_t total_jit_compile_time = 0;
clock_t total_match_time = 0;

static uint32_t dfa_matched;
static uint32_t forbid_utf = 0;
static uint32_t maxlookbehind;
static uint32_t max_oveccount;
static uint32_t callout_count;

static uint16_t local_newline_default = 0;

static VERSION_TYPE jittarget[VERSION_SIZE];
static VERSION_TYPE version[VERSION_SIZE];
static VERSION_TYPE uversion[VERSION_SIZE];

static patctl def_patctl;
static patctl pat_patctl;
static datctl def_datctl;
static datctl dat_datctl;

static void *patstack[PATSTACKSIZE];
static int patstacknext = 0;

static void *malloclist[MALLOCLISTSIZE];
static PCRE2_SIZE malloclistlength[MALLOCLISTSIZE];
static uint32_t malloclistptr = 0;

#ifdef SUPPORT_PCRE2_8
static regex_t preg = { NULL, NULL, 0, 0, 0, 0 };
#endif

static int *dfa_workspace = NULL;
static const uint8_t *locale_tables = NULL;
static const uint8_t *use_tables = NULL;
static uint8_t locale_name[32];

/* We need buffers for building 16/32-bit strings; 8-bit strings don't need
rebuilding, but set up the same naming scheme for use in macros. The "buffer"
buffer is where all input lines are read. Its size is the same as pbuffer8.
Pattern lines are always copied to pbuffer8 for use in callouts, even if they
are actually compiled from pbuffer16 or pbuffer32. */

static size_t    pbuffer8_size  = 50000;        /* Initial size, bytes */
static uint8_t  *pbuffer8 = NULL;
static uint8_t  *buffer = NULL;

/* The dbuffer is where all processed data lines are put. In non-8-bit modes it
is cast as needed. For long data lines it grows as necessary. */

static size_t dbuffer_size = 1u << 14;    /* Initial size, bytes */
static uint8_t *dbuffer = NULL;


/* ---------------- Mode-dependent variables -------------------*/

#ifdef SUPPORT_PCRE2_8
static pcre2_code_8             *compiled_code8;
static pcre2_general_context_8  *general_context8, *general_context_copy8;
static pcre2_compile_context_8  *pat_context8, *default_pat_context8;
static pcre2_convert_context_8  *con_context8, *default_con_context8;
static pcre2_match_context_8    *dat_context8, *default_dat_context8;
static pcre2_match_data_8       *match_data8;
#endif

#ifdef SUPPORT_PCRE2_16
static pcre2_code_16            *compiled_code16;
static pcre2_general_context_16 *general_context16, *general_context_copy16;
static pcre2_compile_context_16 *pat_context16, *default_pat_context16;
static pcre2_convert_context_16 *con_context16, *default_con_context16;
static pcre2_match_context_16   *dat_context16, *default_dat_context16;
static pcre2_match_data_16      *match_data16;
static PCRE2_SIZE pbuffer16_size = 0;   /* Set only when needed */
static uint16_t *pbuffer16 = NULL;
#endif

#ifdef SUPPORT_PCRE2_32
static pcre2_code_32            *compiled_code32;
static pcre2_general_context_32 *general_context32, *general_context_copy32;
static pcre2_compile_context_32 *pat_context32, *default_pat_context32;
static pcre2_convert_context_32 *con_context32, *default_con_context32;
static pcre2_match_context_32   *dat_context32, *default_dat_context32;
static pcre2_match_data_32      *match_data32;
static PCRE2_SIZE pbuffer32_size = 0;   /* Set only when needed */
static uint32_t *pbuffer32 = NULL;
#endif


/* ---------------- Macros that work in all modes ----------------- */

#define CAST8VAR(x) CASTVAR(uint8_t *, x)
#define SET(x,y) SETOP(x,y,=)
#define SETPLUS(x,y) SETOP(x,y,+=)
#define strlen8(x) strlen((char *)x)


/* ---------------- Mode-dependent, runtime-testing macros ------------------*/

/* Define macros for variables and functions that must be selected dynamically
depending on the mode setting (8, 16, 32). These are dependent on which modes
are supported. */

#if (defined (SUPPORT_PCRE2_8) + defined (SUPPORT_PCRE2_16) + \
     defined (SUPPORT_PCRE2_32)) >= 2

/* ----- All three modes supported ----- */

#if defined(SUPPORT_PCRE2_8) && defined(SUPPORT_PCRE2_16) && defined(SUPPORT_PCRE2_32)

#define CASTFLD(t,a,b) ((test_mode == PCRE8_MODE)? (t)(G(a,8)->b) : \
  (test_mode == PCRE16_MODE)? (t)(G(a,16)->b) : (t)(G(a,32)->b))

#define CASTVAR(t,x) ( \
  (test_mode == PCRE8_MODE)? (t)G(x,8) : \
  (test_mode == PCRE16_MODE)? (t)G(x,16) : (t)G(x,32))

#define CODE_UNIT(a,b) ( \
  (test_mode == PCRE8_MODE)? (uint32_t)(((PCRE2_SPTR8)(a))[b]) : \
  (test_mode == PCRE16_MODE)? (uint32_t)(((PCRE2_SPTR16)(a))[b]) : \
  (uint32_t)(((PCRE2_SPTR32)(a))[b]))

#define CONCTXCPY(a,b) \
  if (test_mode == PCRE8_MODE) \
    memcpy(G(a,8),G(b,8),sizeof(pcre2_convert_context_8)); \
  else if (test_mode == PCRE16_MODE) \
    memcpy(G(a,16),G(b,16),sizeof(pcre2_convert_context_16)); \
  else memcpy(G(a,32),G(b,32),sizeof(pcre2_convert_context_32))

#define CONVERT_COPY(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    memcpy(G(a,8),(char *)b,c); \
  else if (test_mode == PCRE16_MODE) \
    memcpy(G(a,16),(char *)b,(c)*2); \
  else if (test_mode == PCRE32_MODE) \
    memcpy(G(a,32),(char *)b,(c)*4)

#define DATCTXCPY(a,b) \
  if (test_mode == PCRE8_MODE) \
    memcpy(G(a,8),G(b,8),sizeof(pcre2_match_context_8)); \
  else if (test_mode == PCRE16_MODE) \
    memcpy(G(a,16),G(b,16),sizeof(pcre2_match_context_16)); \
  else memcpy(G(a,32),G(b,32),sizeof(pcre2_match_context_32))

#define FLD(a,b) ((test_mode == PCRE8_MODE)? G(a,8)->b : \
  (test_mode == PCRE16_MODE)? G(a,16)->b : G(a,32)->b)

#define PATCTXCPY(a,b) \
  if (test_mode == PCRE8_MODE) \
    memcpy(G(a,8),G(b,8),sizeof(pcre2_compile_context_8)); \
  else if (test_mode == PCRE16_MODE) \
    memcpy(G(a,16),G(b,16),sizeof(pcre2_compile_context_16)); \
  else memcpy(G(a,32),G(b,32),sizeof(pcre2_compile_context_32))

#define PCHARS(lv, p, offset, len, utf, f) \
  if (test_mode == PCRE32_MODE) \
    lv = pchars32((PCRE2_SPTR32)(p)+offset, len, utf, f); \
  else if (test_mode == PCRE16_MODE) \
    lv = pchars16((PCRE2_SPTR16)(p)+offset, len, utf, f); \
  else \
    lv = pchars8((PCRE2_SPTR8)(p)+offset, len, utf, f)

#define PCHARSV(p, offset, len, utf, f) \
  if (test_mode == PCRE32_MODE) \
    (void)pchars32((PCRE2_SPTR32)(p)+offset, len, utf, f); \
  else if (test_mode == PCRE16_MODE) \
    (void)pchars16((PCRE2_SPTR16)(p)+offset, len, utf, f); \
  else \
    (void)pchars8((PCRE2_SPTR8)(p)+offset, len, utf, f)

#define PCRE2_CALLOUT_ENUMERATE(a,b,c) \
  if (test_mode == PCRE8_MODE) \
     a = pcre2_callout_enumerate_8(compiled_code8, \
       (int (*)(struct pcre2_callout_enumerate_block_8 *, void *))b,c); \
  else if (test_mode == PCRE16_MODE) \
     a = pcre2_callout_enumerate_16(compiled_code16, \
       (int(*)(struct pcre2_callout_enumerate_block_16 *, void *))b,c); \
  else \
     a = pcre2_callout_enumerate_32(compiled_code32, \
       (int (*)(struct pcre2_callout_enumerate_block_32 *, void *))b,c)

#define PCRE2_CODE_COPY_FROM_VOID(a,b) \
  if (test_mode == PCRE8_MODE) \
    G(a,8) = pcre2_code_copy_8(b); \
  else if (test_mode == PCRE16_MODE) \
    G(a,16) = pcre2_code_copy_16(b); \
  else \
    G(a,32) = pcre2_code_copy_32(b)

#define PCRE2_CODE_COPY_TO_VOID(a,b) \
  if (test_mode == PCRE8_MODE) \
    a = (void *)pcre2_code_copy_8(G(b,8)); \
  else if (test_mode == PCRE16_MODE) \
    a = (void *)pcre2_code_copy_16(G(b,16)); \
  else \
    a = (void *)pcre2_code_copy_32(G(b,32))

#define PCRE2_CODE_COPY_WITH_TABLES_TO_VOID(a,b) \
  if (test_mode == PCRE8_MODE) \
    a = (void *)pcre2_code_copy_with_tables_8(G(b,8)); \
  else if (test_mode == PCRE16_MODE) \
    a = (void *)pcre2_code_copy_with_tables_16(G(b,16)); \
  else \
    a = (void *)pcre2_code_copy_with_tables_32(G(b,32))

#define PCRE2_COMPILE(a,b,c,d,e,f,g) \
  if (test_mode == PCRE8_MODE) \
    G(a,8) = pcre2_compile_8(G(b,8),c,d,e,f,g); \
  else if (test_mode == PCRE16_MODE) \
    G(a,16) = pcre2_compile_16(G(b,16),c,d,e,f,g); \
  else \
    G(a,32) = pcre2_compile_32(G(b,32),c,d,e,f,g)

#define PCRE2_CONVERTED_PATTERN_FREE(a) \
  if (test_mode == PCRE8_MODE) pcre2_converted_pattern_free_8((PCRE2_UCHAR8 *)a); \
  else if (test_mode == PCRE16_MODE) pcre2_converted_pattern_free_16((PCRE2_UCHAR16 *)a); \
  else pcre2_converted_pattern_free_32((PCRE2_UCHAR32 *)a)

#define PCRE2_DFA_MATCH(a,b,c,d,e,f,g,h,i,j) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_dfa_match_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),h,i,j); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_dfa_match_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),h,i,j); \
  else \
    a = pcre2_dfa_match_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),h,i,j)

#define PCRE2_GET_ERROR_MESSAGE(r,a,b) \
  if (test_mode == PCRE8_MODE) \
    r = pcre2_get_error_message_8(a,G(b,8),G(G(b,8),_size)); \
  else if (test_mode == PCRE16_MODE) \
    r = pcre2_get_error_message_16(a,G(b,16),G(G(b,16),_size/2)); \
  else \
    r = pcre2_get_error_message_32(a,G(b,32),G(G(b,32),_size/4))

#define PCRE2_GET_OVECTOR_COUNT(a,b) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_get_ovector_count_8(G(b,8)); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_get_ovector_count_16(G(b,16)); \
  else \
    a = pcre2_get_ovector_count_32(G(b,32))

#define PCRE2_GET_STARTCHAR(a,b) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_get_startchar_8(G(b,8)); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_get_startchar_16(G(b,16)); \
  else \
    a = pcre2_get_startchar_32(G(b,32))

#define PCRE2_JIT_COMPILE(r,a,b) \
  if (test_mode == PCRE8_MODE) r = pcre2_jit_compile_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) r = pcre2_jit_compile_16(G(a,16),b); \
  else r = pcre2_jit_compile_32(G(a,32),b)

#define PCRE2_JIT_FREE_UNUSED_MEMORY(a) \
  if (test_mode == PCRE8_MODE) pcre2_jit_free_unused_memory_8(G(a,8)); \
  else if (test_mode == PCRE16_MODE) pcre2_jit_free_unused_memory_16(G(a,16)); \
  else pcre2_jit_free_unused_memory_32(G(a,32))

#define PCRE2_JIT_MATCH(a,b,c,d,e,f,g,h) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_jit_match_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),h); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_jit_match_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),h); \
  else \
    a = pcre2_jit_match_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),h)

#define PCRE2_JIT_STACK_CREATE(a,b,c,d) \
  if (test_mode == PCRE8_MODE) \
    a = (PCRE2_JIT_STACK *)pcre2_jit_stack_create_8(b,c,d); \
  else if (test_mode == PCRE16_MODE) \
    a = (PCRE2_JIT_STACK *)pcre2_jit_stack_create_16(b,c,d); \
  else \
    a = (PCRE2_JIT_STACK *)pcre2_jit_stack_create_32(b,c,d);

#define PCRE2_JIT_STACK_ASSIGN(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    pcre2_jit_stack_assign_8(G(a,8),(pcre2_jit_callback_8)b,c); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_jit_stack_assign_16(G(a,16),(pcre2_jit_callback_16)b,c); \
  else \
    pcre2_jit_stack_assign_32(G(a,32),(pcre2_jit_callback_32)b,c);

#define PCRE2_JIT_STACK_FREE(a) \
  if (test_mode == PCRE8_MODE) \
    pcre2_jit_stack_free_8((pcre2_jit_stack_8 *)a); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_jit_stack_free_16((pcre2_jit_stack_16 *)a); \
  else \
    pcre2_jit_stack_free_32((pcre2_jit_stack_32 *)a);

#define PCRE2_MAKETABLES(a) \
  if (test_mode == PCRE8_MODE) a = pcre2_maketables_8(NULL); \
  else if (test_mode == PCRE16_MODE) a = pcre2_maketables_16(NULL); \
  else a = pcre2_maketables_32(NULL)

#define PCRE2_MATCH(a,b,c,d,e,f,g,h) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_match_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),h); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_match_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),h); \
  else \
    a = pcre2_match_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),h)

#define PCRE2_MATCH_DATA_CREATE(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    G(a,8) = pcre2_match_data_create_8(b,c); \
  else if (test_mode == PCRE16_MODE) \
    G(a,16) = pcre2_match_data_create_16(b,c); \
  else \
    G(a,32) = pcre2_match_data_create_32(b,c)

#define PCRE2_MATCH_DATA_CREATE_FROM_PATTERN(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    G(a,8) = pcre2_match_data_create_from_pattern_8(G(b,8),c); \
  else if (test_mode == PCRE16_MODE) \
    G(a,16) = pcre2_match_data_create_from_pattern_16(G(b,16),c); \
  else \
    G(a,32) = pcre2_match_data_create_from_pattern_32(G(b,32),c)

#define PCRE2_MATCH_DATA_FREE(a) \
  if (test_mode == PCRE8_MODE) \
    pcre2_match_data_free_8(G(a,8)); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_match_data_free_16(G(a,16)); \
  else \
    pcre2_match_data_free_32(G(a,32))

#define PCRE2_PATTERN_CONVERT(a,b,c,d,e,f,g) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_pattern_convert_8(G(b,8),c,d,(PCRE2_UCHAR8 **)e,f,G(g,8)); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_pattern_convert_16(G(b,16),c,d,(PCRE2_UCHAR16 **)e,f,G(g,16)); \
  else \
    a = pcre2_pattern_convert_32(G(b,32),c,d,(PCRE2_UCHAR32 **)e,f,G(g,32))

#define PCRE2_PATTERN_INFO(a,b,c,d) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_pattern_info_8(G(b,8),c,d); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_pattern_info_16(G(b,16),c,d); \
  else \
    a = pcre2_pattern_info_32(G(b,32),c,d)

#define PCRE2_PRINTINT(a) \
  if (test_mode == PCRE8_MODE) \
    pcre2_printint_8(compiled_code8,outfile,a); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_printint_16(compiled_code16,outfile,a); \
  else \
    pcre2_printint_32(compiled_code32,outfile,a)

#define PCRE2_SERIALIZE_DECODE(r,a,b,c,d) \
  if (test_mode == PCRE8_MODE) \
    r = pcre2_serialize_decode_8((pcre2_code_8 **)a,b,c,G(d,8)); \
  else if (test_mode == PCRE16_MODE) \
    r = pcre2_serialize_decode_16((pcre2_code_16 **)a,b,c,G(d,16)); \
  else \
    r = pcre2_serialize_decode_32((pcre2_code_32 **)a,b,c,G(d,32))

#define PCRE2_SERIALIZE_ENCODE(r,a,b,c,d,e) \
  if (test_mode == PCRE8_MODE) \
    r = pcre2_serialize_encode_8((const pcre2_code_8 **)a,b,c,d,G(e,8)); \
  else if (test_mode == PCRE16_MODE) \
    r = pcre2_serialize_encode_16((const pcre2_code_16 **)a,b,c,d,G(e,16)); \
  else \
    r = pcre2_serialize_encode_32((const pcre2_code_32 **)a,b,c,d,G(e,32))

#define PCRE2_SERIALIZE_FREE(a) \
  if (test_mode == PCRE8_MODE) \
    pcre2_serialize_free_8(a); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_serialize_free_16(a); \
  else \
    pcre2_serialize_free_32(a)

#define PCRE2_SERIALIZE_GET_NUMBER_OF_CODES(r,a) \
  if (test_mode == PCRE8_MODE) \
    r = pcre2_serialize_get_number_of_codes_8(a); \
  else if (test_mode == PCRE16_MODE) \
    r = pcre2_serialize_get_number_of_codes_16(a); \
  else \
    r = pcre2_serialize_get_number_of_codes_32(a); \

#define PCRE2_SET_CALLOUT(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_callout_8(G(a,8),(int (*)(pcre2_callout_block_8 *, void *))b,c); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_callout_16(G(a,16),(int (*)(pcre2_callout_block_16 *, void *))b,c); \
  else \
    pcre2_set_callout_32(G(a,32),(int (*)(pcre2_callout_block_32 *, void *))b,c);

#define PCRE2_SET_CHARACTER_TABLES(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_character_tables_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_character_tables_16(G(a,16),b); \
  else \
    pcre2_set_character_tables_32(G(a,32),b)

#define PCRE2_SET_COMPILE_RECURSION_GUARD(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_compile_recursion_guard_8(G(a,8),b,c); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_compile_recursion_guard_16(G(a,16),b,c); \
  else \
    pcre2_set_compile_recursion_guard_32(G(a,32),b,c)

#define PCRE2_SET_DEPTH_LIMIT(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_depth_limit_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_depth_limit_16(G(a,16),b); \
  else \
    pcre2_set_depth_limit_32(G(a,32),b)

#define PCRE2_SET_GLOB_SEPARATOR(r,a,b) \
  if (test_mode == PCRE8_MODE) \
    r = pcre2_set_glob_separator_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    r = pcre2_set_glob_separator_16(G(a,16),b); \
  else \
    r = pcre2_set_glob_separator_32(G(a,32),b)

#define PCRE2_SET_GLOB_ESCAPE(r,a,b) \
  if (test_mode == PCRE8_MODE) \
    r = pcre2_set_glob_escape_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    r = pcre2_set_glob_escape_16(G(a,16),b); \
  else \
    r = pcre2_set_glob_escape_32(G(a,32),b)

#define PCRE2_SET_HEAP_LIMIT(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_heap_limit_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_heap_limit_16(G(a,16),b); \
  else \
    pcre2_set_heap_limit_32(G(a,32),b)

#define PCRE2_SET_MATCH_LIMIT(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_match_limit_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_match_limit_16(G(a,16),b); \
  else \
    pcre2_set_match_limit_32(G(a,32),b)

#define PCRE2_SET_MAX_PATTERN_LENGTH(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_max_pattern_length_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_max_pattern_length_16(G(a,16),b); \
  else \
    pcre2_set_max_pattern_length_32(G(a,32),b)

#define PCRE2_SET_OFFSET_LIMIT(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_offset_limit_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_offset_limit_16(G(a,16),b); \
  else \
    pcre2_set_offset_limit_32(G(a,32),b)

#define PCRE2_SET_PARENS_NEST_LIMIT(a,b) \
  if (test_mode == PCRE8_MODE) \
    pcre2_set_parens_nest_limit_8(G(a,8),b); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_set_parens_nest_limit_16(G(a,16),b); \
  else \
    pcre2_set_parens_nest_limit_32(G(a,32),b)

#define PCRE2_SUBSTITUTE(a,b,c,d,e,f,g,h,i,j,k,l) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substitute_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),G(h,8), \
      (PCRE2_SPTR8)i,j,(PCRE2_UCHAR8 *)k,l); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substitute_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),G(h,16), \
      (PCRE2_SPTR16)i,j,(PCRE2_UCHAR16 *)k,l); \
  else \
    a = pcre2_substitute_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),G(h,32), \
      (PCRE2_SPTR32)i,j,(PCRE2_UCHAR32 *)k,l)

#define PCRE2_SUBSTRING_COPY_BYNAME(a,b,c,d,e) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_copy_byname_8(G(b,8),G(c,8),(PCRE2_UCHAR8 *)d,e); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_copy_byname_16(G(b,16),G(c,16),(PCRE2_UCHAR16 *)d,e); \
  else \
    a = pcre2_substring_copy_byname_32(G(b,32),G(c,32),(PCRE2_UCHAR32 *)d,e)

#define PCRE2_SUBSTRING_COPY_BYNUMBER(a,b,c,d,e) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_copy_bynumber_8(G(b,8),c,(PCRE2_UCHAR8 *)d,e); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_copy_bynumber_16(G(b,16),c,(PCRE2_UCHAR16 *)d,e); \
  else \
    a = pcre2_substring_copy_bynumber_32(G(b,32),c,(PCRE2_UCHAR32 *)d,e)

#define PCRE2_SUBSTRING_FREE(a) \
  if (test_mode == PCRE8_MODE) pcre2_substring_free_8((PCRE2_UCHAR8 *)a); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_substring_free_16((PCRE2_UCHAR16 *)a); \
  else pcre2_substring_free_32((PCRE2_UCHAR32 *)a)

#define PCRE2_SUBSTRING_GET_BYNAME(a,b,c,d,e) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_get_byname_8(G(b,8),G(c,8),(PCRE2_UCHAR8 **)d,e); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_get_byname_16(G(b,16),G(c,16),(PCRE2_UCHAR16 **)d,e); \
  else \
    a = pcre2_substring_get_byname_32(G(b,32),G(c,32),(PCRE2_UCHAR32 **)d,e)

#define PCRE2_SUBSTRING_GET_BYNUMBER(a,b,c,d,e) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_get_bynumber_8(G(b,8),c,(PCRE2_UCHAR8 **)d,e); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_get_bynumber_16(G(b,16),c,(PCRE2_UCHAR16 **)d,e); \
  else \
    a = pcre2_substring_get_bynumber_32(G(b,32),c,(PCRE2_UCHAR32 **)d,e)

#define PCRE2_SUBSTRING_LENGTH_BYNAME(a,b,c,d) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_length_byname_8(G(b,8),G(c,8),d); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_length_byname_16(G(b,16),G(c,16),d); \
  else \
    a = pcre2_substring_length_byname_32(G(b,32),G(c,32),d)

#define PCRE2_SUBSTRING_LENGTH_BYNUMBER(a,b,c,d) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_length_bynumber_8(G(b,8),c,d); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_length_bynumber_16(G(b,16),c,d); \
  else \
    a = pcre2_substring_length_bynumber_32(G(b,32),c,d)

#define PCRE2_SUBSTRING_LIST_GET(a,b,c,d) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_list_get_8(G(b,8),(PCRE2_UCHAR8 ***)c,d); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_list_get_16(G(b,16),(PCRE2_UCHAR16 ***)c,d); \
  else \
    a = pcre2_substring_list_get_32(G(b,32),(PCRE2_UCHAR32 ***)c,d)

#define PCRE2_SUBSTRING_LIST_FREE(a) \
  if (test_mode == PCRE8_MODE) \
    pcre2_substring_list_free_8((PCRE2_SPTR8 *)a); \
  else if (test_mode == PCRE16_MODE) \
    pcre2_substring_list_free_16((PCRE2_SPTR16 *)a); \
  else \
    pcre2_substring_list_free_32((PCRE2_SPTR32 *)a)

#define PCRE2_SUBSTRING_NUMBER_FROM_NAME(a,b,c) \
  if (test_mode == PCRE8_MODE) \
    a = pcre2_substring_number_from_name_8(G(b,8),G(c,8)); \
  else if (test_mode == PCRE16_MODE) \
    a = pcre2_substring_number_from_name_16(G(b,16),G(c,16)); \
  else \
    a = pcre2_substring_number_from_name_32(G(b,32),G(c,32))

#define PTR(x) ( \
  (test_mode == PCRE8_MODE)? (void *)G(x,8) : \
  (test_mode == PCRE16_MODE)? (void *)G(x,16) : \
  (void *)G(x,32))

#define SETFLD(x,y,z) \
  if (test_mode == PCRE8_MODE) G(x,8)->y = z; \
  else if (test_mode == PCRE16_MODE) G(x,16)->y = z; \
  else G(x,32)->y = z

#define SETFLDVEC(x,y,v,z) \
  if (test_mode == PCRE8_MODE) G(x,8)->y[v] = z; \
  else if (test_mode == PCRE16_MODE) G(x,16)->y[v] = z; \
  else G(x,32)->y[v] = z

#define SETOP(x,y,z) \
  if (test_mode == PCRE8_MODE) G(x,8) z y; \
  else if (test_mode == PCRE16_MODE) G(x,16) z y; \
  else G(x,32) z y

#define SETCASTPTR(x,y) \
  if (test_mode == PCRE8_MODE) \
    G(x,8) = (uint8_t *)(y); \
  else if (test_mode == PCRE16_MODE) \
    G(x,16) = (uint16_t *)(y); \
  else \
    G(x,32) = (uint32_t *)(y)

#define STRLEN(p) ((test_mode == PCRE8_MODE)? ((int)strlen((char *)p)) : \
  (test_mode == PCRE16_MODE)? ((int)strlen16((PCRE2_SPTR16)p)) : \
  ((int)strlen32((PCRE2_SPTR32)p)))

#define SUB1(a,b) \
  if (test_mode == PCRE8_MODE) G(a,8)(G(b,8)); \
  else if (test_mode == PCRE16_MODE) G(a,16)(G(b,16)); \
  else G(a,32)(G(b,32))

#define SUB2(a,b,c) \
  if (test_mode == PCRE8_MODE) G(a,8)(G(b,8),G(c,8)); \
  else if (test_mode == PCRE16_MODE) G(a,16)(G(b,16),G(c,16)); \
  else G(a,32)(G(b,32),G(c,32))

#define TEST(x,r,y) ( \
  (test_mode == PCRE8_MODE && G(x,8) r (y)) || \
  (test_mode == PCRE16_MODE && G(x,16) r (y)) || \
  (test_mode == PCRE32_MODE && G(x,32) r (y)))

#define TESTFLD(x,f,r,y) ( \
  (test_mode == PCRE8_MODE && G(x,8)->f r (y)) || \
  (test_mode == PCRE16_MODE && G(x,16)->f r (y)) || \
  (test_mode == PCRE32_MODE && G(x,32)->f r (y)))


/* ----- Two out of three modes are supported ----- */

#else

/* We can use some macro trickery to make a single set of definitions work in
the three different cases. */

/* ----- 32-bit and 16-bit but not 8-bit supported ----- */

#if defined(SUPPORT_PCRE2_32) && defined(SUPPORT_PCRE2_16)
#define BITONE 32
#define BITTWO 16

/* ----- 32-bit and 8-bit but not 16-bit supported ----- */

#elif defined(SUPPORT_PCRE2_32) && defined(SUPPORT_PCRE2_8)
#define BITONE 32
#define BITTWO 8

/* ----- 16-bit and 8-bit but not 32-bit supported ----- */

#else
#define BITONE 16
#define BITTWO 8
#endif


/* ----- Common macros for two-mode cases ----- */

#define BYTEONE (BITONE/8)
#define BYTETWO (BITTWO/8)

#define CASTFLD(t,a,b) \
  ((test_mode == G(G(PCRE,BITONE),_MODE))? (t)(G(a,BITONE)->b) : \
    (t)(G(a,BITTWO)->b))

#define CASTVAR(t,x) ( \
  (test_mode == G(G(PCRE,BITONE),_MODE))? \
    (t)G(x,BITONE) : (t)G(x,BITTWO))

#define CODE_UNIT(a,b) ( \
  (test_mode == G(G(PCRE,BITONE),_MODE))? \
  (uint32_t)(((G(PCRE2_SPTR,BITONE))(a))[b]) : \
  (uint32_t)(((G(PCRE2_SPTR,BITTWO))(a))[b]))

#define CONCTXCPY(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    memcpy(G(a,BITONE),G(b,BITONE),sizeof(G(pcre2_convert_context_,BITONE))); \
  else \
    memcpy(G(a,BITTWO),G(b,BITTWO),sizeof(G(pcre2_convert_context_,BITTWO)))

#define CONVERT_COPY(a,b,c) \
  (test_mode == G(G(PCRE,BITONE),_MODE))? \
  memcpy(G(a,BITONE),(char *)b,(c)*BYTEONE) : \
  memcpy(G(a,BITTWO),(char *)b,(c)*BYTETWO)

#define DATCTXCPY(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    memcpy(G(a,BITONE),G(b,BITONE),sizeof(G(pcre2_match_context_,BITONE))); \
  else \
    memcpy(G(a,BITTWO),G(b,BITTWO),sizeof(G(pcre2_match_context_,BITTWO)))

#define FLD(a,b) \
  ((test_mode == G(G(PCRE,BITONE),_MODE))? G(a,BITONE)->b : G(a,BITTWO)->b)

#define PATCTXCPY(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    memcpy(G(a,BITONE),G(b,BITONE),sizeof(G(pcre2_compile_context_,BITONE))); \
  else \
    memcpy(G(a,BITTWO),G(b,BITTWO),sizeof(G(pcre2_compile_context_,BITTWO)))

#define PCHARS(lv, p, offset, len, utf, f) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    lv = G(pchars,BITONE)((G(PCRE2_SPTR,BITONE))(p)+offset, len, utf, f); \
  else \
    lv = G(pchars,BITTWO)((G(PCRE2_SPTR,BITTWO))(p)+offset, len, utf, f)

#define PCHARSV(p, offset, len, utf, f) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    (void)G(pchars,BITONE)((G(PCRE2_SPTR,BITONE))(p)+offset, len, utf, f); \
  else \
    (void)G(pchars,BITTWO)((G(PCRE2_SPTR,BITTWO))(p)+offset, len, utf, f)

#define PCRE2_CALLOUT_ENUMERATE(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
     a = G(pcre2_callout_enumerate,BITONE)(G(compiled_code,BITONE), \
       (int (*)(struct G(pcre2_callout_enumerate_block_,BITONE) *, void *))b,c); \
  else \
     a = G(pcre2_callout_enumerate,BITTWO)(G(compiled_code,BITTWO), \
       (int (*)(struct G(pcre2_callout_enumerate_block_,BITTWO) *, void *))b,c)

#define PCRE2_CODE_COPY_FROM_VOID(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(a,BITONE) = G(pcre2_code_copy_,BITONE)(b); \
  else \
    G(a,BITTWO) = G(pcre2_code_copy_,BITTWO)(b)

#define PCRE2_CODE_COPY_TO_VOID(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = (void *)G(pcre2_code_copy_,BITONE)(G(b,BITONE)); \
  else \
    a = (void *)G(pcre2_code_copy_,BITTWO)(G(b,BITTWO))

#define PCRE2_CODE_COPY_WITH_TABLES_TO_VOID(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = (void *)G(pcre2_code_copy_with_tables_,BITONE)(G(b,BITONE)); \
  else \
    a = (void *)G(pcre2_code_copy_with_tables_,BITTWO)(G(b,BITTWO))

#define PCRE2_COMPILE(a,b,c,d,e,f,g) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(a,BITONE) = G(pcre2_compile_,BITONE)(G(b,BITONE),c,d,e,f,g); \
  else \
    G(a,BITTWO) = G(pcre2_compile_,BITTWO)(G(b,BITTWO),c,d,e,f,g)

#define PCRE2_CONVERTED_PATTERN_FREE(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_converted_pattern_free_,BITONE)((G(PCRE2_UCHAR,BITONE) *)a); \
  else \
    G(pcre2_converted_pattern_free_,BITTWO)((G(PCRE2_UCHAR,BITTWO) *)a)

#define PCRE2_DFA_MATCH(a,b,c,d,e,f,g,h,i,j) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_dfa_match_,BITONE)(G(b,BITONE),(G(PCRE2_SPTR,BITONE))c,d,e,f, \
      G(g,BITONE),h,i,j); \
  else \
    a = G(pcre2_dfa_match_,BITTWO)(G(b,BITTWO),(G(PCRE2_SPTR,BITTWO))c,d,e,f, \
      G(g,BITTWO),h,i,j)

#define PCRE2_GET_ERROR_MESSAGE(r,a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_get_error_message_,BITONE)(a,G(b,BITONE),G(G(b,BITONE),_size/BYTEONE)); \
  else \
    r = G(pcre2_get_error_message_,BITTWO)(a,G(b,BITTWO),G(G(b,BITTWO),_size/BYTETWO))

#define PCRE2_GET_OVECTOR_COUNT(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_get_ovector_count_,BITONE)(G(b,BITONE)); \
  else \
    a = G(pcre2_get_ovector_count_,BITTWO)(G(b,BITTWO))

#define PCRE2_GET_STARTCHAR(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_get_startchar_,BITONE)(G(b,BITONE)); \
  else \
    a = G(pcre2_get_startchar_,BITTWO)(G(b,BITTWO))

#define PCRE2_JIT_COMPILE(r,a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_jit_compile_,BITONE)(G(a,BITONE),b); \
  else \
    r = G(pcre2_jit_compile_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_JIT_FREE_UNUSED_MEMORY(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_jit_free_unused_memory_,BITONE)(G(a,BITONE)); \
  else \
    G(pcre2_jit_free_unused_memory_,BITTWO)(G(a,BITTWO))

#define PCRE2_JIT_MATCH(a,b,c,d,e,f,g,h) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_jit_match_,BITONE)(G(b,BITONE),(G(PCRE2_SPTR,BITONE))c,d,e,f, \
      G(g,BITONE),h); \
  else \
    a = G(pcre2_jit_match_,BITTWO)(G(b,BITTWO),(G(PCRE2_SPTR,BITTWO))c,d,e,f, \
      G(g,BITTWO),h)

#define PCRE2_JIT_STACK_CREATE(a,b,c,d) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = (PCRE2_JIT_STACK *)G(pcre2_jit_stack_create_,BITONE)(b,c,d); \
  else \
    a = (PCRE2_JIT_STACK *)G(pcre2_jit_stack_create_,BITTWO)(b,c,d); \

#define PCRE2_JIT_STACK_ASSIGN(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_jit_stack_assign_,BITONE)(G(a,BITONE),(G(pcre2_jit_callback_,BITONE))b,c); \
  else \
    G(pcre2_jit_stack_assign_,BITTWO)(G(a,BITTWO),(G(pcre2_jit_callback_,BITTWO))b,c);

#define PCRE2_JIT_STACK_FREE(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_jit_stack_free_,BITONE)((G(pcre2_jit_stack_,BITONE) *)a); \
  else \
    G(pcre2_jit_stack_free_,BITTWO)((G(pcre2_jit_stack_,BITTWO) *)a);

#define PCRE2_MAKETABLES(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_maketables_,BITONE)(NULL); \
  else \
    a = G(pcre2_maketables_,BITTWO)(NULL)

#define PCRE2_MATCH(a,b,c,d,e,f,g,h) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_match_,BITONE)(G(b,BITONE),(G(PCRE2_SPTR,BITONE))c,d,e,f, \
      G(g,BITONE),h); \
  else \
    a = G(pcre2_match_,BITTWO)(G(b,BITTWO),(G(PCRE2_SPTR,BITTWO))c,d,e,f, \
      G(g,BITTWO),h)

#define PCRE2_MATCH_DATA_CREATE(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(a,BITONE) = G(pcre2_match_data_create_,BITONE)(b,c); \
  else \
    G(a,BITTWO) = G(pcre2_match_data_create_,BITTWO)(b,c)

#define PCRE2_MATCH_DATA_CREATE_FROM_PATTERN(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(a,BITONE) = G(pcre2_match_data_create_from_pattern_,BITONE)(G(b,BITONE),c); \
  else \
    G(a,BITTWO) = G(pcre2_match_data_create_from_pattern_,BITTWO)(G(b,BITTWO),c)

#define PCRE2_MATCH_DATA_FREE(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_match_data_free_,BITONE)(G(a,BITONE)); \
  else \
    G(pcre2_match_data_free_,BITTWO)(G(a,BITTWO))

#define PCRE2_PATTERN_CONVERT(a,b,c,d,e,f,g) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_pattern_convert_,BITONE)(G(b,BITONE),c,d,(G(PCRE2_UCHAR,BITONE) **)e,f,G(g,BITONE)); \
  else \
    a = G(pcre2_pattern_convert_,BITTWO)(G(b,BITTWO),c,d,(G(PCRE2_UCHAR,BITTWO) **)e,f,G(g,BITTWO))

#define PCRE2_PATTERN_INFO(a,b,c,d) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_pattern_info_,BITONE)(G(b,BITONE),c,d); \
  else \
    a = G(pcre2_pattern_info_,BITTWO)(G(b,BITTWO),c,d)

#define PCRE2_PRINTINT(a) \
 if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_printint_,BITONE)(G(compiled_code,BITONE),outfile,a); \
  else \
    G(pcre2_printint_,BITTWO)(G(compiled_code,BITTWO),outfile,a)

#define PCRE2_SERIALIZE_DECODE(r,a,b,c,d) \
 if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_serialize_decode_,BITONE)((G(pcre2_code_,BITONE) **)a,b,c,G(d,BITONE)); \
  else \
    r = G(pcre2_serialize_decode_,BITTWO)((G(pcre2_code_,BITTWO) **)a,b,c,G(d,BITTWO))

#define PCRE2_SERIALIZE_ENCODE(r,a,b,c,d,e) \
 if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_serialize_encode_,BITONE)((G(const pcre2_code_,BITONE) **)a,b,c,d,G(e,BITONE)); \
  else \
    r = G(pcre2_serialize_encode_,BITTWO)((G(const pcre2_code_,BITTWO) **)a,b,c,d,G(e,BITTWO))

#define PCRE2_SERIALIZE_FREE(a) \
 if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_serialize_free_,BITONE)(a); \
  else \
    G(pcre2_serialize_free_,BITTWO)(a)

#define PCRE2_SERIALIZE_GET_NUMBER_OF_CODES(r,a) \
 if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_serialize_get_number_of_codes_,BITONE)(a); \
  else \
    r = G(pcre2_serialize_get_number_of_codes_,BITTWO)(a)

#define PCRE2_SET_CALLOUT(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_callout_,BITONE)(G(a,BITONE), \
      (int (*)(G(pcre2_callout_block_,BITONE) *, void *))b,c); \
  else \
    G(pcre2_set_callout_,BITTWO)(G(a,BITTWO), \
      (int (*)(G(pcre2_callout_block_,BITTWO) *, void *))b,c);

#define PCRE2_SET_CHARACTER_TABLES(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_character_tables_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_character_tables_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_COMPILE_RECURSION_GUARD(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_compile_recursion_guard_,BITONE)(G(a,BITONE),b,c); \
  else \
    G(pcre2_set_compile_recursion_guard_,BITTWO)(G(a,BITTWO),b,c)

#define PCRE2_SET_DEPTH_LIMIT(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_depth_limit_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_depth_limit_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_GLOB_ESCAPE(r,a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_set_glob_escape_,BITONE)(G(a,BITONE),b); \
  else \
    r = G(pcre2_set_glob_escape_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_GLOB_SEPARATOR(r,a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    r = G(pcre2_set_glob_separator_,BITONE)(G(a,BITONE),b); \
  else \
    r = G(pcre2_set_glob_separator_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_HEAP_LIMIT(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_heap_limit_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_heap_limit_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_MATCH_LIMIT(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_match_limit_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_match_limit_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_MAX_PATTERN_LENGTH(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_max_pattern_length_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_max_pattern_length_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_OFFSET_LIMIT(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_offset_limit_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_offset_limit_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SET_PARENS_NEST_LIMIT(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_set_parens_nest_limit_,BITONE)(G(a,BITONE),b); \
  else \
    G(pcre2_set_parens_nest_limit_,BITTWO)(G(a,BITTWO),b)

#define PCRE2_SUBSTITUTE(a,b,c,d,e,f,g,h,i,j,k,l) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substitute_,BITONE)(G(b,BITONE),(G(PCRE2_SPTR,BITONE))c,d,e,f, \
      G(g,BITONE),G(h,BITONE),(G(PCRE2_SPTR,BITONE))i,j, \
      (G(PCRE2_UCHAR,BITONE) *)k,l); \
  else \
    a = G(pcre2_substitute_,BITTWO)(G(b,BITTWO),(G(PCRE2_SPTR,BITTWO))c,d,e,f, \
      G(g,BITTWO),G(h,BITTWO),(G(PCRE2_SPTR,BITTWO))i,j, \
      (G(PCRE2_UCHAR,BITTWO) *)k,l)

#define PCRE2_SUBSTRING_COPY_BYNAME(a,b,c,d,e) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_copy_byname_,BITONE)(G(b,BITONE),G(c,BITONE),\
      (G(PCRE2_UCHAR,BITONE) *)d,e); \
  else \
    a = G(pcre2_substring_copy_byname_,BITTWO)(G(b,BITTWO),G(c,BITTWO),\
      (G(PCRE2_UCHAR,BITTWO) *)d,e)

#define PCRE2_SUBSTRING_COPY_BYNUMBER(a,b,c,d,e) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_copy_bynumber_,BITONE)(G(b,BITONE),c,\
      (G(PCRE2_UCHAR,BITONE) *)d,e); \
  else \
    a = G(pcre2_substring_copy_bynumber_,BITTWO)(G(b,BITTWO),c,\
      (G(PCRE2_UCHAR,BITTWO) *)d,e)

#define PCRE2_SUBSTRING_FREE(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_substring_free_,BITONE)((G(PCRE2_UCHAR,BITONE) *)a); \
  else G(pcre2_substring_free_,BITTWO)((G(PCRE2_UCHAR,BITTWO) *)a)

#define PCRE2_SUBSTRING_GET_BYNAME(a,b,c,d,e) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_get_byname_,BITONE)(G(b,BITONE),G(c,BITONE),\
      (G(PCRE2_UCHAR,BITONE) **)d,e); \
  else \
    a = G(pcre2_substring_get_byname_,BITTWO)(G(b,BITTWO),G(c,BITTWO),\
      (G(PCRE2_UCHAR,BITTWO) **)d,e)

#define PCRE2_SUBSTRING_GET_BYNUMBER(a,b,c,d,e) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_get_bynumber_,BITONE)(G(b,BITONE),c,\
      (G(PCRE2_UCHAR,BITONE) **)d,e); \
  else \
    a = G(pcre2_substring_get_bynumber_,BITTWO)(G(b,BITTWO),c,\
      (G(PCRE2_UCHAR,BITTWO) **)d,e)

#define PCRE2_SUBSTRING_LENGTH_BYNAME(a,b,c,d) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_length_byname_,BITONE)(G(b,BITONE),G(c,BITONE),d); \
  else \
    a = G(pcre2_substring_length_byname_,BITTWO)(G(b,BITTWO),G(c,BITTWO),d)

#define PCRE2_SUBSTRING_LENGTH_BYNUMBER(a,b,c,d) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_length_bynumber_,BITONE)(G(b,BITONE),c,d); \
  else \
    a = G(pcre2_substring_length_bynumber_,BITTWO)(G(b,BITTWO),c,d)

#define PCRE2_SUBSTRING_LIST_GET(a,b,c,d) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_list_get_,BITONE)(G(b,BITONE), \
      (G(PCRE2_UCHAR,BITONE) ***)c,d); \
  else \
    a = G(pcre2_substring_list_get_,BITTWO)(G(b,BITTWO), \
      (G(PCRE2_UCHAR,BITTWO) ***)c,d)

#define PCRE2_SUBSTRING_LIST_FREE(a) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(pcre2_substring_list_free_,BITONE)((G(PCRE2_SPTR,BITONE) *)a); \
  else \
    G(pcre2_substring_list_free_,BITTWO)((G(PCRE2_SPTR,BITTWO) *)a)

#define PCRE2_SUBSTRING_NUMBER_FROM_NAME(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    a = G(pcre2_substring_number_from_name_,BITONE)(G(b,BITONE),G(c,BITONE)); \
  else \
    a = G(pcre2_substring_number_from_name_,BITTWO)(G(b,BITTWO),G(c,BITTWO))

#define PTR(x) ( \
  (test_mode == G(G(PCRE,BITONE),_MODE))? (void *)G(x,BITONE) : \
  (void *)G(x,BITTWO))

#define SETFLD(x,y,z) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) G(x,BITONE)->y = z; \
  else G(x,BITTWO)->y = z

#define SETFLDVEC(x,y,v,z) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) G(x,BITONE)->y[v] = z; \
  else G(x,BITTWO)->y[v] = z

#define SETOP(x,y,z) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) G(x,BITONE) z y; \
  else G(x,BITTWO) z y

#define SETCASTPTR(x,y) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(x,BITONE) = (G(G(uint,BITONE),_t) *)(y); \
  else \
    G(x,BITTWO) = (G(G(uint,BITTWO),_t) *)(y)

#define STRLEN(p) ((test_mode == G(G(PCRE,BITONE),_MODE))? \
  G(strlen,BITONE)((G(PCRE2_SPTR,BITONE))p) : \
  G(strlen,BITTWO)((G(PCRE2_SPTR,BITTWO))p))

#define SUB1(a,b) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(a,BITONE)(G(b,BITONE)); \
  else \
    G(a,BITTWO)(G(b,BITTWO))

#define SUB2(a,b,c) \
  if (test_mode == G(G(PCRE,BITONE),_MODE)) \
    G(a,BITONE))(G(b,BITONE),G(c,BITONE)); \
  else \
    G(a,BITTWO))(G(b,BITTWO),G(c,BITTWO))

#define TEST(x,r,y) ( \
  (test_mode == G(G(PCRE,BITONE),_MODE) && G(x,BITONE) r (y)) || \
  (test_mode == G(G(PCRE,BITTWO),_MODE) && G(x,BITTWO) r (y)))

#define TESTFLD(x,f,r,y) ( \
  (test_mode == G(G(PCRE,BITONE),_MODE) && G(x,BITONE)->f r (y)) || \
  (test_mode == G(G(PCRE,BITTWO),_MODE) && G(x,BITTWO)->f r (y)))


#endif  /* Two out of three modes */

/* ----- End of cases where more than one mode is supported ----- */


/* ----- Only 8-bit mode is supported ----- */

#elif defined SUPPORT_PCRE2_8
#define CASTFLD(t,a,b) (t)(G(a,8)->b)
#define CASTVAR(t,x) (t)G(x,8)
#define CODE_UNIT(a,b) (uint32_t)(((PCRE2_SPTR8)(a))[b])
#define CONCTXCPY(a,b) memcpy(G(a,8),G(b,8),sizeof(pcre2_convert_context_8))
#define CONVERT_COPY(a,b,c) memcpy(G(a,8),(char *)b, c)
#define DATCTXCPY(a,b) memcpy(G(a,8),G(b,8),sizeof(pcre2_match_context_8))
#define FLD(a,b) G(a,8)->b
#define PATCTXCPY(a,b) memcpy(G(a,8),G(b,8),sizeof(pcre2_compile_context_8))
#define PCHARS(lv, p, offset, len, utf, f) \
  lv = pchars8((PCRE2_SPTR8)(p)+offset, len, utf, f)
#define PCHARSV(p, offset, len, utf, f) \
  (void)pchars8((PCRE2_SPTR8)(p)+offset, len, utf, f)
#define PCRE2_CALLOUT_ENUMERATE(a,b,c) \
   a = pcre2_callout_enumerate_8(compiled_code8, \
     (int (*)(struct pcre2_callout_enumerate_block_8 *, void *))b,c)
#define PCRE2_CODE_COPY_FROM_VOID(a,b) G(a,8) = pcre2_code_copy_8(b)
#define PCRE2_CODE_COPY_TO_VOID(a,b) a = (void *)pcre2_code_copy_8(G(b,8))
#define PCRE2_CODE_COPY_WITH_TABLES_TO_VOID(a,b) a = (void *)pcre2_code_copy_with_tables_8(G(b,8))
#define PCRE2_COMPILE(a,b,c,d,e,f,g) \
  G(a,8) = pcre2_compile_8(G(b,8),c,d,e,f,g)
#define PCRE2_CONVERTED_PATTERN_FREE(a) \
  pcre2_converted_pattern_free_8((PCRE2_UCHAR8 *)a)
#define PCRE2_DFA_MATCH(a,b,c,d,e,f,g,h,i,j) \
  a = pcre2_dfa_match_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),h,i,j)
#define PCRE2_GET_ERROR_MESSAGE(r,a,b) \
  r = pcre2_get_error_message_8(a,G(b,8),G(G(b,8),_size))
#define PCRE2_GET_OVECTOR_COUNT(a,b) a = pcre2_get_ovector_count_8(G(b,8))
#define PCRE2_GET_STARTCHAR(a,b) a = pcre2_get_startchar_8(G(b,8))
#define PCRE2_JIT_COMPILE(r,a,b) r = pcre2_jit_compile_8(G(a,8),b)
#define PCRE2_JIT_FREE_UNUSED_MEMORY(a) pcre2_jit_free_unused_memory_8(G(a,8))
#define PCRE2_JIT_MATCH(a,b,c,d,e,f,g,h) \
  a = pcre2_jit_match_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),h)
#define PCRE2_JIT_STACK_CREATE(a,b,c,d) \
  a = (PCRE2_JIT_STACK *)pcre2_jit_stack_create_8(b,c,d);
#define PCRE2_JIT_STACK_ASSIGN(a,b,c) \
  pcre2_jit_stack_assign_8(G(a,8),(pcre2_jit_callback_8)b,c);
#define PCRE2_JIT_STACK_FREE(a) pcre2_jit_stack_free_8((pcre2_jit_stack_8 *)a);
#define PCRE2_MAKETABLES(a) a = pcre2_maketables_8(NULL)
#define PCRE2_MATCH(a,b,c,d,e,f,g,h) \
  a = pcre2_match_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),h)
#define PCRE2_MATCH_DATA_CREATE(a,b,c) G(a,8) = pcre2_match_data_create_8(b,c)
#define PCRE2_MATCH_DATA_CREATE_FROM_PATTERN(a,b,c) \
  G(a,8) = pcre2_match_data_create_from_pattern_8(G(b,8),c)
#define PCRE2_MATCH_DATA_FREE(a) pcre2_match_data_free_8(G(a,8))
#define PCRE2_PATTERN_CONVERT(a,b,c,d,e,f,g) a = pcre2_pattern_convert_8(G(b,8),c,d,(PCRE2_UCHAR8 **)e,f,G(g,8))
#define PCRE2_PATTERN_INFO(a,b,c,d) a = pcre2_pattern_info_8(G(b,8),c,d)
#define PCRE2_PRINTINT(a) pcre2_printint_8(compiled_code8,outfile,a)
#define PCRE2_SERIALIZE_DECODE(r,a,b,c,d) \
  r = pcre2_serialize_decode_8((pcre2_code_8 **)a,b,c,G(d,8))
#define PCRE2_SERIALIZE_ENCODE(r,a,b,c,d,e) \
  r = pcre2_serialize_encode_8((const pcre2_code_8 **)a,b,c,d,G(e,8))
#define PCRE2_SERIALIZE_FREE(a) pcre2_serialize_free_8(a)
#define PCRE2_SERIALIZE_GET_NUMBER_OF_CODES(r,a) \
  r = pcre2_serialize_get_number_of_codes_8(a)
#define PCRE2_SET_CALLOUT(a,b,c) \
  pcre2_set_callout_8(G(a,8),(int (*)(pcre2_callout_block_8 *, void *))b,c)
#define PCRE2_SET_CHARACTER_TABLES(a,b) pcre2_set_character_tables_8(G(a,8),b)
#define PCRE2_SET_COMPILE_RECURSION_GUARD(a,b,c) \
  pcre2_set_compile_recursion_guard_8(G(a,8),b,c)
#define PCRE2_SET_DEPTH_LIMIT(a,b) pcre2_set_depth_limit_8(G(a,8),b)
#define PCRE2_SET_GLOB_ESCAPE(r,a,b) r = pcre2_set_glob_escape_8(G(a,8),b)
#define PCRE2_SET_GLOB_SEPARATOR(r,a,b) r = pcre2_set_glob_separator_8(G(a,8),b)
#define PCRE2_SET_HEAP_LIMIT(a,b) pcre2_set_heap_limit_8(G(a,8),b)
#define PCRE2_SET_MATCH_LIMIT(a,b) pcre2_set_match_limit_8(G(a,8),b)
#define PCRE2_SET_MAX_PATTERN_LENGTH(a,b) pcre2_set_max_pattern_length_8(G(a,8),b)
#define PCRE2_SET_OFFSET_LIMIT(a,b) pcre2_set_offset_limit_8(G(a,8),b)
#define PCRE2_SET_PARENS_NEST_LIMIT(a,b) pcre2_set_parens_nest_limit_8(G(a,8),b)
#define PCRE2_SUBSTITUTE(a,b,c,d,e,f,g,h,i,j,k,l) \
  a = pcre2_substitute_8(G(b,8),(PCRE2_SPTR8)c,d,e,f,G(g,8),G(h,8), \
    (PCRE2_SPTR8)i,j,(PCRE2_UCHAR8 *)k,l)
#define PCRE2_SUBSTRING_COPY_BYNAME(a,b,c,d,e) \
  a = pcre2_substring_copy_byname_8(G(b,8),G(c,8),(PCRE2_UCHAR8 *)d,e)
#define PCRE2_SUBSTRING_COPY_BYNUMBER(a,b,c,d,e) \
  a = pcre2_substring_copy_bynumber_8(G(b,8),c,(PCRE2_UCHAR8 *)d,e)
#define PCRE2_SUBSTRING_FREE(a) pcre2_substring_free_8((PCRE2_UCHAR8 *)a)
#define PCRE2_SUBSTRING_GET_BYNAME(a,b,c,d,e) \
  a = pcre2_substring_get_byname_8(G(b,8),G(c,8),(PCRE2_UCHAR8 **)d,e)
#define PCRE2_SUBSTRING_GET_BYNUMBER(a,b,c,d,e) \
  a = pcre2_substring_get_bynumber_8(G(b,8),c,(PCRE2_UCHAR8 **)d,e)
#define PCRE2_SUBSTRING_LENGTH_BYNAME(a,b,c,d) \
    a = pcre2_substring_length_byname_8(G(b,8),G(c,8),d)
#define PCRE2_SUBSTRING_LENGTH_BYNUMBER(a,b,c,d) \
    a = pcre2_substring_length_bynumber_8(G(b,8),c,d)
#define PCRE2_SUBSTRING_LIST_GET(a,b,c,d) \
  a = pcre2_substring_list_get_8(G(b,8),(PCRE2_UCHAR8 ***)c,d)
#define PCRE2_SUBSTRING_LIST_FREE(a) \
  pcre2_substring_list_free_8((PCRE2_SPTR8 *)a)
#define PCRE2_SUBSTRING_NUMBER_FROM_NAME(a,b,c) \
  a = pcre2_substring_number_from_name_8(G(b,8),G(c,8));
#define PTR(x) (void *)G(x,8)
#define SETFLD(x,y,z) G(x,8)->y = z
#define SETFLDVEC(x,y,v,z) G(x,8)->y[v] = z
#define SETOP(x,y,z) G(x,8) z y
#define SETCASTPTR(x,y) G(x,8) = (uint8_t *)(y)
#define STRLEN(p) (int)strlen((char *)p)
#define SUB1(a,b) G(a,8)(G(b,8))
#define SUB2(a,b,c) G(a,8)(G(b,8),G(c,8))
#define TEST(x,r,y) (G(x,8) r (y))
#define TESTFLD(x,f,r,y) (G(x,8)->f r (y))


/* ----- Only 16-bit mode is supported ----- */

#elif defined SUPPORT_PCRE2_16
#define CASTFLD(t,a,b) (t)(G(a,16)->b)
#define CASTVAR(t,x) (t)G(x,16)
#define CODE_UNIT(a,b) (uint32_t)(((PCRE2_SPTR16)(a))[b])
#define CONCTXCPY(a,b) memcpy(G(a,16),G(b,16),sizeof(pcre2_convert_context_16))
#define CONVERT_COPY(a,b,c) memcpy(G(a,16),(char *)b, (c)*2)
#define DATCTXCPY(a,b) memcpy(G(a,16),G(b,16),sizeof(pcre2_match_context_16))
#define FLD(a,b) G(a,16)->b
#define PATCTXCPY(a,b) memcpy(G(a,16),G(b,16),sizeof(pcre2_compile_context_16))
#define PCHARS(lv, p, offset, len, utf, f) \
  lv = pchars16((PCRE2_SPTR16)(p)+offset, len, utf, f)
#define PCHARSV(p, offset, len, utf, f) \
  (void)pchars16((PCRE2_SPTR16)(p)+offset, len, utf, f)
#define PCRE2_CALLOUT_ENUMERATE(a,b,c) \
   a = pcre2_callout_enumerate_16(compiled_code16, \
     (int (*)(struct pcre2_callout_enumerate_block_16 *, void *))b,c)
#define PCRE2_CODE_COPY_FROM_VOID(a,b) G(a,16) = pcre2_code_copy_16(b)
#define PCRE2_CODE_COPY_TO_VOID(a,b) a = (void *)pcre2_code_copy_16(G(b,16))
#define PCRE2_CODE_COPY_WITH_TABLES_TO_VOID(a,b) a = (void *)pcre2_code_copy_with_tables_16(G(b,16))
#define PCRE2_COMPILE(a,b,c,d,e,f,g) \
  G(a,16) = pcre2_compile_16(G(b,16),c,d,e,f,g)
#define PCRE2_CONVERTED_PATTERN_FREE(a) \
  pcre2_converted_pattern_free_16((PCRE2_UCHAR16 *)a)
#define PCRE2_DFA_MATCH(a,b,c,d,e,f,g,h,i,j) \
  a = pcre2_dfa_match_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),h,i,j)
#define PCRE2_GET_ERROR_MESSAGE(r,a,b) \
  r = pcre2_get_error_message_16(a,G(b,16),G(G(b,16),_size/2))
#define PCRE2_GET_OVECTOR_COUNT(a,b) a = pcre2_get_ovector_count_16(G(b,16))
#define PCRE2_GET_STARTCHAR(a,b) a = pcre2_get_startchar_16(G(b,16))
#define PCRE2_JIT_COMPILE(r,a,b) r = pcre2_jit_compile_16(G(a,16),b)
#define PCRE2_JIT_FREE_UNUSED_MEMORY(a) pcre2_jit_free_unused_memory_16(G(a,16))
#define PCRE2_JIT_MATCH(a,b,c,d,e,f,g,h) \
  a = pcre2_jit_match_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),h)
#define PCRE2_JIT_STACK_CREATE(a,b,c,d) \
  a = (PCRE2_JIT_STACK *)pcre2_jit_stack_create_16(b,c,d);
#define PCRE2_JIT_STACK_ASSIGN(a,b,c) \
  pcre2_jit_stack_assign_16(G(a,16),(pcre2_jit_callback_16)b,c);
#define PCRE2_JIT_STACK_FREE(a) pcre2_jit_stack_free_16((pcre2_jit_stack_16 *)a);
#define PCRE2_MAKETABLES(a) a = pcre2_maketables_16(NULL)
#define PCRE2_MATCH(a,b,c,d,e,f,g,h) \
  a = pcre2_match_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),h)
#define PCRE2_MATCH_DATA_CREATE(a,b,c) G(a,16) = pcre2_match_data_create_16(b,c)
#define PCRE2_MATCH_DATA_CREATE_FROM_PATTERN(a,b,c) \
  G(a,16) = pcre2_match_data_create_from_pattern_16(G(b,16),c)
#define PCRE2_MATCH_DATA_FREE(a) pcre2_match_data_free_16(G(a,16))
#define PCRE2_PATTERN_CONVERT(a,b,c,d,e,f,g) a = pcre2_pattern_convert_16(G(b,16),c,d,(PCRE2_UCHAR16 **)e,f,G(g,16))
#define PCRE2_PATTERN_INFO(a,b,c,d) a = pcre2_pattern_info_16(G(b,16),c,d)
#define PCRE2_PRINTINT(a) pcre2_printint_16(compiled_code16,outfile,a)
#define PCRE2_SERIALIZE_DECODE(r,a,b,c,d) \
  r = pcre2_serialize_decode_16((pcre2_code_16 **)a,b,c,G(d,16))
#define PCRE2_SERIALIZE_ENCODE(r,a,b,c,d,e) \
  r = pcre2_serialize_encode_16((const pcre2_code_16 **)a,b,c,d,G(e,16))
#define PCRE2_SERIALIZE_FREE(a) pcre2_serialize_free_16(a)
#define PCRE2_SERIALIZE_GET_NUMBER_OF_CODES(r,a) \
  r = pcre2_serialize_get_number_of_codes_16(a)
#define PCRE2_SET_CALLOUT(a,b,c) \
  pcre2_set_callout_16(G(a,16),(int (*)(pcre2_callout_block_16 *, void *))b,c);
#define PCRE2_SET_CHARACTER_TABLES(a,b) pcre2_set_character_tables_16(G(a,16),b)
#define PCRE2_SET_COMPILE_RECURSION_GUARD(a,b,c) \
  pcre2_set_compile_recursion_guard_16(G(a,16),b,c)
#define PCRE2_SET_DEPTH_LIMIT(a,b) pcre2_set_depth_limit_16(G(a,16),b)
#define PCRE2_SET_GLOB_ESCAPE(r,a,b) r = pcre2_set_glob_escape_16(G(a,16),b)
#define PCRE2_SET_GLOB_SEPARATOR(r,a,b) r = pcre2_set_glob_separator_16(G(a,16),b)
#define PCRE2_SET_HEAP_LIMIT(a,b) pcre2_set_heap_limit_16(G(a,16),b)
#define PCRE2_SET_MATCH_LIMIT(a,b) pcre2_set_match_limit_16(G(a,16),b)
#define PCRE2_SET_MAX_PATTERN_LENGTH(a,b) pcre2_set_max_pattern_length_16(G(a,16),b)
#define PCRE2_SET_OFFSET_LIMIT(a,b) pcre2_set_offset_limit_16(G(a,16),b)
#define PCRE2_SET_PARENS_NEST_LIMIT(a,b) pcre2_set_parens_nest_limit_16(G(a,16),b)
#define PCRE2_SUBSTITUTE(a,b,c,d,e,f,g,h,i,j,k,l) \
  a = pcre2_substitute_16(G(b,16),(PCRE2_SPTR16)c,d,e,f,G(g,16),G(h,16), \
    (PCRE2_SPTR16)i,j,(PCRE2_UCHAR16 *)k,l)
#define PCRE2_SUBSTRING_COPY_BYNAME(a,b,c,d,e) \
  a = pcre2_substring_copy_byname_16(G(b,16),G(c,16),(PCRE2_UCHAR16 *)d,e)
#define PCRE2_SUBSTRING_COPY_BYNUMBER(a,b,c,d,e) \
  a = pcre2_substring_copy_bynumber_16(G(b,16),c,(PCRE2_UCHAR16 *)d,e)
#define PCRE2_SUBSTRING_FREE(a) pcre2_substring_free_16((PCRE2_UCHAR16 *)a)
#define PCRE2_SUBSTRING_GET_BYNAME(a,b,c,d,e) \
  a = pcre2_substring_get_byname_16(G(b,16),G(c,16),(PCRE2_UCHAR16 **)d,e)
#define PCRE2_SUBSTRING_GET_BYNUMBER(a,b,c,d,e) \
  a = pcre2_substring_get_bynumber_16(G(b,16),c,(PCRE2_UCHAR16 **)d,e)
#define PCRE2_SUBSTRING_LENGTH_BYNAME(a,b,c,d) \
    a = pcre2_substring_length_byname_16(G(b,16),G(c,16),d)
#define PCRE2_SUBSTRING_LENGTH_BYNUMBER(a,b,c,d) \
    a = pcre2_substring_length_bynumber_16(G(b,16),c,d)
#define PCRE2_SUBSTRING_LIST_GET(a,b,c,d) \
  a = pcre2_substring_list_get_16(G(b,16),(PCRE2_UCHAR16 ***)c,d)
#define PCRE2_SUBSTRING_LIST_FREE(a) \
  pcre2_substring_list_free_16((PCRE2_SPTR16 *)a)
#define PCRE2_SUBSTRING_NUMBER_FROM_NAME(a,b,c) \
  a = pcre2_substring_number_from_name_16(G(b,16),G(c,16));
#define PTR(x) (void *)G(x,16)
#define SETFLD(x,y,z) G(x,16)->y = z
#define SETFLDVEC(x,y,v,z) G(x,16)->y[v] = z
#define SETOP(x,y,z) G(x,16) z y
#define SETCASTPTR(x,y) G(x,16) = (uint16_t *)(y)
#define STRLEN(p) (int)strlen16((PCRE2_SPTR16)p)
#define SUB1(a,b) G(a,16)(G(b,16))
#define SUB2(a,b,c) G(a,16)(G(b,16),G(c,16))
#define TEST(x,r,y) (G(x,16) r (y))
#define TESTFLD(x,f,r,y) (G(x,16)->f r (y))


/* ----- Only 32-bit mode is supported ----- */

#elif defined SUPPORT_PCRE2_32
#define CASTFLD(t,a,b) (t)(G(a,32)->b)
#define CASTVAR(t,x) (t)G(x,32)
#define CODE_UNIT(a,b) (uint32_t)(((PCRE2_SPTR32)(a))[b])
#define CONCTXCPY(a,b) memcpy(G(a,32),G(b,32),sizeof(pcre2_convert_context_32))
#define CONVERT_COPY(a,b,c) memcpy(G(a,32),(char *)b, (c)*4)
#define DATCTXCPY(a,b) memcpy(G(a,32),G(b,32),sizeof(pcre2_match_context_32))
#define FLD(a,b) G(a,32)->b
#define PATCTXCPY(a,b) memcpy(G(a,32),G(b,32),sizeof(pcre2_compile_context_32))
#define PCHARS(lv, p, offset, len, utf, f) \
  lv = pchars32((PCRE2_SPTR32)(p)+offset, len, utf, f)
#define PCHARSV(p, offset, len, utf, f) \
  (void)pchars32((PCRE2_SPTR32)(p)+offset, len, utf, f)
#define PCRE2_CALLOUT_ENUMERATE(a,b,c) \
   a = pcre2_callout_enumerate_32(compiled_code32, \
     (int (*)(struct pcre2_callout_enumerate_block_32 *, void *))b,c)
#define PCRE2_CODE_COPY_FROM_VOID(a,b) G(a,32) = pcre2_code_copy_32(b)
#define PCRE2_CODE_COPY_TO_VOID(a,b) a = (void *)pcre2_code_copy_32(G(b,32))
#define PCRE2_CODE_COPY_WITH_TABLES_TO_VOID(a,b) a = (void *)pcre2_code_copy_with_tables_32(G(b,32))
#define PCRE2_COMPILE(a,b,c,d,e,f,g) \
  G(a,32) = pcre2_compile_32(G(b,32),c,d,e,f,g)
#define PCRE2_CONVERTED_PATTERN_FREE(a) \
  pcre2_converted_pattern_free_32((PCRE2_UCHAR32 *)a)
#define PCRE2_DFA_MATCH(a,b,c,d,e,f,g,h,i,j) \
  a = pcre2_dfa_match_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),h,i,j)
#define PCRE2_GET_ERROR_MESSAGE(r,a,b) \
  r = pcre2_get_error_message_32(a,G(b,32),G(G(b,32),_size/4))
#define PCRE2_GET_OVECTOR_COUNT(a,b) a = pcre2_get_ovector_count_32(G(b,32))
#define PCRE2_GET_STARTCHAR(a,b) a = pcre2_get_startchar_32(G(b,32))
#define PCRE2_JIT_COMPILE(r,a,b) r = pcre2_jit_compile_32(G(a,32),b)
#define PCRE2_JIT_FREE_UNUSED_MEMORY(a) pcre2_jit_free_unused_memory_32(G(a,32))
#define PCRE2_JIT_MATCH(a,b,c,d,e,f,g,h) \
  a = pcre2_jit_match_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),h)
#define PCRE2_JIT_STACK_CREATE(a,b,c,d) \
  a = (PCRE2_JIT_STACK *)pcre2_jit_stack_create_32(b,c,d);
#define PCRE2_JIT_STACK_ASSIGN(a,b,c) \
  pcre2_jit_stack_assign_32(G(a,32),(pcre2_jit_callback_32)b,c);
#define PCRE2_JIT_STACK_FREE(a) pcre2_jit_stack_free_32((pcre2_jit_stack_32 *)a);
#define PCRE2_MAKETABLES(a) a = pcre2_maketables_32(NULL)
#define PCRE2_MATCH(a,b,c,d,e,f,g,h) \
  a = pcre2_match_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),h)
#define PCRE2_MATCH_DATA_CREATE(a,b,c) G(a,32) = pcre2_match_data_create_32(b,c)
#define PCRE2_MATCH_DATA_CREATE_FROM_PATTERN(a,b,c) \
  G(a,32) = pcre2_match_data_create_from_pattern_32(G(b,32),c)
#define PCRE2_MATCH_DATA_FREE(a) pcre2_match_data_free_32(G(a,32))
#define PCRE2_PATTERN_CONVERT(a,b,c,d,e,f,g) a = pcre2_pattern_convert_32(G(b,32),c,d,(PCRE2_UCHAR32 **)e,f,G(g,32))
#define PCRE2_PATTERN_INFO(a,b,c,d) a = pcre2_pattern_info_32(G(b,32),c,d)
#define PCRE2_PRINTINT(a) pcre2_printint_32(compiled_code32,outfile,a)
#define PCRE2_SERIALIZE_DECODE(r,a,b,c,d) \
  r = pcre2_serialize_decode_32((pcre2_code_32 **)a,b,c,G(d,32))
#define PCRE2_SERIALIZE_ENCODE(r,a,b,c,d,e) \
  r = pcre2_serialize_encode_32((const pcre2_code_32 **)a,b,c,d,G(e,32))
#define PCRE2_SERIALIZE_FREE(a) pcre2_serialize_free_32(a)
#define PCRE2_SERIALIZE_GET_NUMBER_OF_CODES(r,a) \
  r = pcre2_serialize_get_number_of_codes_32(a)
#define PCRE2_SET_CALLOUT(a,b,c) \
  pcre2_set_callout_32(G(a,32),(int (*)(pcre2_callout_block_32 *, void *))b,c);
#define PCRE2_SET_CHARACTER_TABLES(a,b) pcre2_set_character_tables_32(G(a,32),b)
#define PCRE2_SET_COMPILE_RECURSION_GUARD(a,b,c) \
  pcre2_set_compile_recursion_guard_32(G(a,32),b,c)
#define PCRE2_SET_DEPTH_LIMIT(a,b) pcre2_set_depth_limit_32(G(a,32),b)
#define PCRE2_SET_GLOB_ESCAPE(r,a,b) r = pcre2_set_glob_escape_32(G(a,32),b)
#define PCRE2_SET_GLOB_SEPARATOR(r,a,b) r = pcre2_set_glob_separator_32(G(a,32),b)
#define PCRE2_SET_HEAP_LIMIT(a,b) pcre2_set_heap_limit_32(G(a,32),b)
#define PCRE2_SET_MATCH_LIMIT(a,b) pcre2_set_match_limit_32(G(a,32),b)
#define PCRE2_SET_MAX_PATTERN_LENGTH(a,b) pcre2_set_max_pattern_length_32(G(a,32),b)
#define PCRE2_SET_OFFSET_LIMIT(a,b) pcre2_set_offset_limit_32(G(a,32),b)
#define PCRE2_SET_PARENS_NEST_LIMIT(a,b) pcre2_set_parens_nest_limit_32(G(a,32),b)
#define PCRE2_SUBSTITUTE(a,b,c,d,e,f,g,h,i,j,k,l) \
  a = pcre2_substitute_32(G(b,32),(PCRE2_SPTR32)c,d,e,f,G(g,32),G(h,32), \
    (PCRE2_SPTR32)i,j,(PCRE2_UCHAR32 *)k,l)
#define PCRE2_SUBSTRING_COPY_BYNAME(a,b,c,d,e) \
  a = pcre2_substring_copy_byname_32(G(b,32),G(c,32),(PCRE2_UCHAR32 *)d,e)
#define PCRE2_SUBSTRING_COPY_BYNUMBER(a,b,c,d,e) \
  a = pcre2_substring_copy_bynumber_32(G(b,32),c,(PCRE2_UCHAR32 *)d,e);
#define PCRE2_SUBSTRING_FREE(a) pcre2_substring_free_32((PCRE2_UCHAR32 *)a)
#define PCRE2_SUBSTRING_GET_BYNAME(a,b,c,d,e) \
  a = pcre2_substring_get_byname_32(G(b,32),G(c,32),(PCRE2_UCHAR32 **)d,e)
#define PCRE2_SUBSTRING_GET_BYNUMBER(a,b,c,d,e) \
  a = pcre2_substring_get_bynumber_32(G(b,32),c,(PCRE2_UCHAR32 **)d,e)
#define PCRE2_SUBSTRING_LENGTH_BYNAME(a,b,c,d) \
    a = pcre2_substring_length_byname_32(G(b,32),G(c,32),d)
#define PCRE2_SUBSTRING_LENGTH_BYNUMBER(a,b,c,d) \
    a = pcre2_substring_length_bynumber_32(G(b,32),c,d)
#define PCRE2_SUBSTRING_LIST_GET(a,b,c,d) \
  a = pcre2_substring_list_get_32(G(b,32),(PCRE2_UCHAR32 ***)c,d)
#define PCRE2_SUBSTRING_LIST_FREE(a) \
  pcre2_substring_list_free_32((PCRE2_SPTR32 *)a)
#define PCRE2_SUBSTRING_NUMBER_FROM_NAME(a,b,c) \
  a = pcre2_substring_number_from_name_32(G(b,32),G(c,32));
#define PTR(x) (void *)G(x,32)
#define SETFLD(x,y,z) G(x,32)->y = z
#define SETFLDVEC(x,y,v,z) G(x,32)->y[v] = z
#define SETOP(x,y,z) G(x,32) z y
#define SETCASTPTR(x,y) G(x,32) = (uint32_t *)(y)
#define STRLEN(p) (int)strlen32((PCRE2_SPTR32)p)
#define SUB1(a,b) G(a,32)(G(b,32))
#define SUB2(a,b,c) G(a,32)(G(b,32),G(c,32))
#define TEST(x,r,y) (G(x,32) r (y))
#define TESTFLD(x,f,r,y) (G(x,32)->f r (y))

#endif

/* ----- End of mode-specific function call macros ----- */




/*************************************************
*         Alternate character tables             *
*************************************************/

/* By default, the "tables" pointer in the compile context when calling
pcre2_compile() is not set (= NULL), thereby using the default tables of the
library. However, the tables modifier can be used to select alternate sets of
tables, for different kinds of testing. Note that the locale modifier also
adjusts the tables. */

/* This is the set of tables distributed as default with PCRE2. It recognizes
only ASCII characters. */

static const uint8_t tables1[] = {

/* This table is a lower casing table. */

    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23,
   24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39,
   40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55,
   56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,
  104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,
  120,121,122, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,
  104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,
  120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,
  136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,
  152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,
  168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,
  184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,
  200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,
  216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,
  232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,
  248,249,250,251,252,253,254,255,

/* This table is a case flipping table. */

    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23,
   24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39,
   40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55,
   56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,
  104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,
  120,121,122, 91, 92, 93, 94, 95,
   96, 65, 66, 67, 68, 69, 70, 71,
   72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87,
   88, 89, 90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,
  136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,
  152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,
  168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,
  184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,
  200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,
  216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,
  232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,
  248,249,250,251,252,253,254,255,

/* This table contains bit maps for various character classes. Each map is 32
bytes long and the bits run from the least significant end of each byte. The
classes that have their own maps are: space, xdigit, digit, upper, lower, word,
graph, print, punct, and cntrl. Other classes are built from combinations. */

  0x00,0x3e,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,
  0x7e,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfe,0xff,0xff,0x07,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xfe,0xff,0xff,0x07,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,
  0xfe,0xff,0xff,0x87,0xfe,0xff,0xff,0x07,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0xfe,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0xfe,0xff,0x00,0xfc,
  0x01,0x00,0x00,0xf8,0x01,0x00,0x00,0x78,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* This table identifies various classes of character by individual bits:
  0x01   white space character
  0x02   letter
  0x04   decimal digit
  0x08   hexadecimal digit
  0x10   alphanumeric or '_'
  0x80   regular expression metacharacter or binary zero
*/

  0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   0-  7 */
  0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x00, /*   8- 15 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  16- 23 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31 */
  0x01,0x00,0x00,0x00,0x80,0x00,0x00,0x00, /*    - '  */
  0x80,0x80,0x80,0x80,0x00,0x00,0x80,0x00, /*  ( - /  */
  0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c, /*  0 - 7  */
  0x1c,0x1c,0x00,0x00,0x00,0x00,0x00,0x80, /*  8 - ?  */
  0x00,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /*  @ - G  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  H - O  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  P - W  */
  0x12,0x12,0x12,0x80,0x80,0x00,0x80,0x10, /*  X - _  */
  0x00,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /*  ` - g  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  h - o  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  p - w  */
  0x12,0x12,0x12,0x80,0x80,0x00,0x00,0x00, /*  x -127 */
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

/* This is a set of tables that came originally from a Windows user. It seems
to be at least an approximation of ISO 8859. In particular, there are
characters greater than 128 that are marked as spaces, letters, etc. */

static const uint8_t tables2[] = {
0,1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,
56,57,58,59,60,61,62,63,
64,97,98,99,100,101,102,103,
104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,
120,121,122,91,92,93,94,95,
96,97,98,99,100,101,102,103,
104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,
120,121,122,123,124,125,126,127,
128,129,130,131,132,133,134,135,
136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,
152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,
168,169,170,171,172,173,174,175,
176,177,178,179,180,181,182,183,
184,185,186,187,188,189,190,191,
224,225,226,227,228,229,230,231,
232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,215,
248,249,250,251,252,253,254,223,
224,225,226,227,228,229,230,231,
232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,
248,249,250,251,252,253,254,255,
0,1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,
56,57,58,59,60,61,62,63,
64,97,98,99,100,101,102,103,
104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,
120,121,122,91,92,93,94,95,
96,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,
88,89,90,123,124,125,126,127,
128,129,130,131,132,133,134,135,
136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,
152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,
168,169,170,171,172,173,174,175,
176,177,178,179,180,181,182,183,
184,185,186,187,188,189,190,191,
224,225,226,227,228,229,230,231,
232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,215,
248,249,250,251,252,253,254,223,
192,193,194,195,196,197,198,199,
200,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,247,
216,217,218,219,220,221,222,255,
0,62,0,0,1,0,0,0,
0,0,0,0,0,0,0,0,
32,0,0,0,1,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,255,3,
126,0,0,0,126,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,255,3,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,12,2,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
254,255,255,7,0,0,0,0,
0,0,0,0,0,0,0,0,
255,255,127,127,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,254,255,255,7,
0,0,0,0,0,4,32,4,
0,0,0,128,255,255,127,255,
0,0,0,0,0,0,255,3,
254,255,255,135,254,255,255,7,
0,0,0,0,0,4,44,6,
255,255,127,255,255,255,127,255,
0,0,0,0,254,255,255,255,
255,255,255,255,255,255,255,127,
0,0,0,0,254,255,255,255,
255,255,255,255,255,255,255,255,
0,2,0,0,255,255,255,255,
255,255,255,255,255,255,255,127,
0,0,0,0,255,255,255,255,
255,255,255,255,255,255,255,255,
0,0,0,0,254,255,0,252,
1,0,0,248,1,0,0,120,
0,0,0,0,254,255,255,255,
0,0,128,0,0,0,128,0,
255,255,255,255,0,0,0,0,
0,0,0,0,0,0,0,128,
255,255,255,255,0,0,0,0,
0,0,0,0,0,0,0,0,
128,0,0,0,0,0,0,0,
0,1,1,0,1,1,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
1,0,0,0,128,0,0,0,
128,128,128,128,0,0,128,0,
28,28,28,28,28,28,28,28,
28,28,0,0,0,0,0,128,
0,26,26,26,26,26,26,18,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,
18,18,18,128,128,0,128,16,
0,26,26,26,26,26,26,18,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,
18,18,18,128,128,0,0,0,
0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,
0,0,18,0,0,0,0,0,
0,0,20,20,0,18,0,0,
0,20,18,0,0,0,0,0,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,0,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,0,
18,18,18,18,18,18,18,18
};


#ifndef HAVE_STRERROR
/*************************************************
*     Provide strerror() for non-ANSI libraries  *
*************************************************/

/* Some old-fashioned systems (e.g. SunOS4) didn't have strerror() in their
libraries. They may no longer be around, but just in case, we can try to
provide the same facility by this simple alternative function. */

extern int   sys_nerr;
extern char *sys_errlist[];

char *
strerror(int n)
{
if (n < 0 || n >= sys_nerr) return "unknown error number";
return sys_errlist[n];
}
#endif /* HAVE_STRERROR */



/*************************************************
*            Local memory functions              *
*************************************************/

/* Alternative memory functions, to test functionality. */

static void *my_malloc(PCRE2_SIZE size, void *data)
{
void *block = malloc(size);
(void)data;
if (show_memory)
  {
  if (block == NULL)
    {
    fprintf(outfile, "** malloc() failed for %zd\n", size);
    }
  else
    {
    fprintf(outfile, "malloc  %5zd", size);
#ifdef DEBUG_SHOW_MALLOC_ADDRESSES
    fprintf(outfile, " %p", block);   /* Not portable */
#endif
    if (malloclistptr < MALLOCLISTSIZE)
      {
      malloclist[malloclistptr] = block;
      malloclistlength[malloclistptr++] = size;
      }
    else
      fprintf(outfile, " (not remembered)");
    fprintf(outfile, "\n");
    }
  }
return block;
}

static void my_free(void *block, void *data)
{
(void)data;
if (show_memory)
  {
  uint32_t i, j;
  BOOL found = FALSE;

  fprintf(outfile, "free");
  for (i = 0; i < malloclistptr; i++)
    {
    if (block == malloclist[i])
      {
      fprintf(outfile, "    %5zd", malloclistlength[i]);
      malloclistptr--;
      for (j = i; j < malloclistptr; j++)
        {
        malloclist[j] = malloclist[j+1];
        malloclistlength[j] = malloclistlength[j+1];
        }
      found = TRUE;
      break;
      }
    }
  if (!found) fprintf(outfile, " unremembered block");
#ifdef DEBUG_SHOW_MALLOC_ADDRESSES
  fprintf(outfile, " %p", block);  /* Not portable */
#endif
  fprintf(outfile, "\n");
  }
free(block);
}



/*************************************************
*       Callback function for stack guard        *
*************************************************/

/* This is set up to be called from pcre2_compile() when the stackguard=n
modifier sets a value greater than zero. The test we do is whether the
parenthesis nesting depth is greater than the value set by the modifier.

Argument:  the current parenthesis nesting depth
Returns:   non-zero to kill the compilation
*/

static int
stack_guard(uint32_t depth, void *user_data)
{
(void)user_data;
return depth > pat_patctl.stackguard_test;
}


/*************************************************
*         JIT memory callback                    *
*************************************************/

static PCRE2_JIT_STACK*
jit_callback(void *arg)
{
jit_was_used = TRUE;
return (PCRE2_JIT_STACK *)arg;
}


/*************************************************
*      Convert UTF-8 character to code point     *
*************************************************/

/* This function reads one or more bytes that represent a UTF-8 character,
and returns the codepoint of that character. Note that the function supports
the original UTF-8 definition of RFC 2279, allowing for values in the range 0
to 0x7fffffff, up to 6 bytes long. This makes it possible to generate
codepoints greater than 0x10ffff which are useful for testing PCRE2's error
checking, and also for generating 32-bit non-UTF data values above the UTF
limit.

Argument:
  utf8bytes   a pointer to the byte vector
  vptr        a pointer to an int to receive the value

Returns:      >  0 => the number of bytes consumed
              -6 to 0 => malformed UTF-8 character at offset = (-return)
*/

static int
utf82ord(PCRE2_SPTR8 utf8bytes, uint32_t *vptr)
{
uint32_t c = *utf8bytes++;
uint32_t d = c;
int i, j, s;

for (i = -1; i < 6; i++)               /* i is number of additional bytes */
  {
  if ((d & 0x80) == 0) break;
  d <<= 1;
  }

if (i == -1) { *vptr = c; return 1; }  /* ascii character */
if (i == 0 || i == 6) return 0;        /* invalid UTF-8 */

/* i now has a value in the range 1-5 */

s = 6*i;
d = (c & utf8_table3[i]) << s;

for (j = 0; j < i; j++)
  {
  c = *utf8bytes++;
  if ((c & 0xc0) != 0x80) return -(j+1);
  s -= 6;
  d |= (c & 0x3f) << s;
  }

/* Check that encoding was the correct unique one */

for (j = 0; j < utf8_table1_size; j++)
  if (d <= (uint32_t)utf8_table1[j]) break;
if (j != i) return -(i+1);

/* Valid value */

*vptr = d;
return i+1;
}



/*************************************************
*             Print one character                *
*************************************************/

/* Print a single character either literally, or as a hex escape, and count how
many printed characters are used.

Arguments:
  c            the character
  utf          TRUE in UTF mode
  f            the FILE to print to, or NULL just to count characters

Returns:       number of characters written
*/

static int
pchar(uint32_t c, BOOL utf, FILE *f)
{
int n = 0;
char tempbuffer[16];

if (PRINTOK(c))
  {
  if (f != NULL) fprintf(f, "%c", c);
  return 1;
  }

if (c < 0x100)
  {
  if (utf)
    {
    if (f != NULL) fprintf(f, "\\x{%02x}", c);
    return 6;
    }
  else
    {
    if (f != NULL) fprintf(f, "\\x%02x", c);
    return 4;
    }
  }

if (f != NULL) n = fprintf(f, "\\x{%02x}", c);
  else n = sprintf(tempbuffer, "\\x{%02x}", c);

return n >= 0 ? n : 0;
}



#ifdef SUPPORT_PCRE2_16
/*************************************************
*    Find length of 0-terminated 16-bit string   *
*************************************************/

static size_t strlen16(PCRE2_SPTR16 p)
{
PCRE2_SPTR16 pp = p;
while (*pp != 0) pp++;
return (int)(pp - p);
}
#endif  /* SUPPORT_PCRE2_16 */



#ifdef SUPPORT_PCRE2_32
/*************************************************
*    Find length of 0-terminated 32-bit string   *
*************************************************/

static size_t strlen32(PCRE2_SPTR32 p)
{
PCRE2_SPTR32 pp = p;
while (*pp != 0) pp++;
return (int)(pp - p);
}
#endif  /* SUPPORT_PCRE2_32 */


#ifdef SUPPORT_PCRE2_8
/*************************************************
*         Print 8-bit character string           *
*************************************************/

/* Must handle UTF-8 strings in utf8 mode. Yields number of characters printed.
For printing *MARK strings, a negative length is given. If handed a NULL file,
just counts chars without printing (because pchar() does that). */

static int pchars8(PCRE2_SPTR8 p, int length, BOOL utf, FILE *f)
{
uint32_t c = 0;
int yield = 0;

if (length < 0) length = p[-1];
while (length-- > 0)
  {
  if (utf)
    {
    int rc = utf82ord(p, &c);
    if (rc > 0 && rc <= length + 1)   /* Mustn't run over the end */
      {
      length -= rc - 1;
      p += rc;
      yield += pchar(c, utf, f);
      continue;
      }
    }
  c = *p++;
  yield += pchar(c, utf, f);
  }

return yield;
}
#endif


#ifdef SUPPORT_PCRE2_16
/*************************************************
*           Print 16-bit character string        *
*************************************************/

/* Must handle UTF-16 strings in utf mode. Yields number of characters printed.
For printing *MARK strings, a negative length is given. If handed a NULL file,
just counts chars without printing. */

static int pchars16(PCRE2_SPTR16 p, int length, BOOL utf, FILE *f)
{
int yield = 0;
if (length < 0) length = p[-1];
while (length-- > 0)
  {
  uint32_t c = *p++ & 0xffff;
  if (utf && c >= 0xD800 && c < 0xDC00 && length > 0)
    {
    int d = *p & 0xffff;
    if (d >= 0xDC00 && d <= 0xDFFF)
      {
      c = ((c & 0x3ff) << 10) + (d & 0x3ff) + 0x10000;
      length--;
      p++;
      }
    }
  yield += pchar(c, utf, f);
  }
return yield;
}
#endif  /* SUPPORT_PCRE2_16 */



#ifdef SUPPORT_PCRE2_32
/*************************************************
*           Print 32-bit character string        *
*************************************************/

/* Must handle UTF-32 strings in utf mode. Yields number of characters printed.
For printing *MARK strings, a negative length is given. If handed a NULL file,
just counts chars without printing. */

static int pchars32(PCRE2_SPTR32 p, int length, BOOL utf, FILE *f)
{
int yield = 0;
(void)(utf);  /* Avoid compiler warning */

if (length < 0) length = p[-1];
while (length-- > 0)
  {
  uint32_t c = *p++;
  yield += pchar(c, utf, f);
  }
return yield;
}
#endif  /* SUPPORT_PCRE2_32 */




#ifdef SUPPORT_PCRE2_8
/*************************************************
*       Convert character value to UTF-8         *
*************************************************/

/* This function takes an integer value in the range 0 - 0x7fffffff
and encodes it as a UTF-8 character in 0 to 6 bytes.

Arguments:
  cvalue     the character value
  utf8bytes  pointer to buffer for result - at least 6 bytes long

Returns:     number of characters placed in the buffer
*/

static int
ord2utf8(uint32_t cvalue, uint8_t *utf8bytes)
{
int i, j;
if (cvalue > 0x7fffffffu)
  return -1;
for (i = 0; i < utf8_table1_size; i++)
  if (cvalue <= (uint32_t)utf8_table1[i]) break;
utf8bytes += i;
for (j = i; j > 0; j--)
 {
 *utf8bytes-- = 0x80 | (cvalue & 0x3f);
 cvalue >>= 6;
 }
*utf8bytes = utf8_table2[i] | cvalue;
return i + 1;
}
#endif  /* SUPPORT_PCRE2_8 */



#ifdef SUPPORT_PCRE2_16
/*************************************************
*           Convert string to 16-bit             *
*************************************************/

/* In UTF mode the input is always interpreted as a string of UTF-8 bytes using
the original UTF-8 definition of RFC 2279, which allows for up to 6 bytes, and
code values from 0 to 0x7fffffff. However, values greater than the later UTF
limit of 0x10ffff cause an error. In non-UTF mode the input is interpreted as
UTF-8 if the utf8_input modifier is set, but an error is generated for values
greater than 0xffff.

If all the input bytes are ASCII, the space needed for a 16-bit string is
exactly double the 8-bit size. Otherwise, the size needed for a 16-bit string
is no more than double, because up to 0xffff uses no more than 3 bytes in UTF-8
but possibly 4 in UTF-16. Higher values use 4 bytes in UTF-8 and up to 4 bytes
in UTF-16. The result is always left in pbuffer16. Impose a minimum size to
save repeated re-sizing.

Note that this function does not object to surrogate values. This is
deliberate; it makes it possible to construct UTF-16 strings that are invalid,
for the purpose of testing that they are correctly faulted.

Arguments:
  p          points to a byte string
  utf        true in UTF mode
  lenptr     points to number of bytes in the string (excluding trailing zero)

Returns:     0 on success, with the length updated to the number of 16-bit
               data items used (excluding the trailing zero)
             OR -1 if a UTF-8 string is malformed
             OR -2 if a value > 0x10ffff is encountered in UTF mode
             OR -3 if a value > 0xffff is encountered when not in UTF mode
*/

static PCRE2_SIZE
to16(uint8_t *p, int utf, PCRE2_SIZE *lenptr)
{
uint16_t *pp;
PCRE2_SIZE len = *lenptr;

if (pbuffer16_size < 2*len + 2)
  {
  if (pbuffer16 != NULL) free(pbuffer16);
  pbuffer16_size = 2*len + 2;
  if (pbuffer16_size < 4096) pbuffer16_size = 4096;
  pbuffer16 = (uint16_t *)malloc(pbuffer16_size);
  if (pbuffer16 == NULL)
    {
    fprintf(stderr, "pcre2test: malloc(%lu) failed for pbuffer16\n",
      (unsigned long int)pbuffer16_size);
    exit(1);
    }
  }

pp = pbuffer16;
if (!utf && (pat_patctl.control & CTL_UTF8_INPUT) == 0)
  {
  for (; len > 0; len--) *pp++ = *p++;
  }
else while (len > 0)
  {
  uint32_t c;
  int chlen = utf82ord(p, &c);
  if (chlen <= 0) return -1;
  if (!utf && c > 0xffff) return -3;
  if (c > 0x10ffff) return -2;
  p += chlen;
  len -= chlen;
  if (c < 0x10000) *pp++ = c; else
    {
    c -= 0x10000;
    *pp++ = 0xD800 | (c >> 10);
    *pp++ = 0xDC00 | (c & 0x3ff);
    }
  }

*pp = 0;
*lenptr = pp - pbuffer16;
return 0;
}
#endif



#ifdef SUPPORT_PCRE2_32
/*************************************************
*           Convert string to 32-bit             *
*************************************************/

/* In UTF mode the input is always interpreted as a string of UTF-8 bytes using
the original UTF-8 definition of RFC 2279, which allows for up to 6 bytes, and
code values from 0 to 0x7fffffff. However, values greater than the later UTF
limit of 0x10ffff cause an error.

In non-UTF mode the input is interpreted as UTF-8 if the utf8_input modifier
is set, and no limit is imposed. There is special interpretation of the 0xff
byte (which is illegal in UTF-8) in this case: it causes the top bit of the
next character to be set. This provides a way of generating 32-bit characters
greater than 0x7fffffff.

If all the input bytes are ASCII, the space needed for a 32-bit string is
exactly four times the 8-bit size. Otherwise, the size needed for a 32-bit
string is no more than four times, because the number of characters must be
less than the number of bytes. The result is always left in pbuffer32. Impose a
minimum size to save repeated re-sizing.

Note that this function does not object to surrogate values. This is
deliberate; it makes it possible to construct UTF-32 strings that are invalid,
for the purpose of testing that they are correctly faulted.

Arguments:
  p          points to a byte string
  utf        true in UTF mode
  lenptr     points to number of bytes in the string (excluding trailing zero)

Returns:     0 on success, with the length updated to the number of 32-bit
               data items used (excluding the trailing zero)
             OR -1 if a UTF-8 string is malformed
             OR -2 if a value > 0x10ffff is encountered in UTF mode
*/

static PCRE2_SIZE
to32(uint8_t *p, int utf, PCRE2_SIZE *lenptr)
{
uint32_t *pp;
PCRE2_SIZE len = *lenptr;

if (pbuffer32_size < 4*len + 4)
  {
  if (pbuffer32 != NULL) free(pbuffer32);
  pbuffer32_size = 4*len + 4;
  if (pbuffer32_size < 8192) pbuffer32_size = 8192;
  pbuffer32 = (uint32_t *)malloc(pbuffer32_size);
  if (pbuffer32 == NULL)
    {
    fprintf(stderr, "pcre2test: malloc(%lu) failed for pbuffer32\n",
      (unsigned long int)pbuffer32_size);
    exit(1);
    }
  }

pp = pbuffer32;

if (!utf && (pat_patctl.control & CTL_UTF8_INPUT) == 0)
  {
  for (; len > 0; len--) *pp++ = *p++;
  }

else while (len > 0)
  {
  int chlen;
  uint32_t c;
  uint32_t topbit = 0;
  if (!utf && *p == 0xff && len > 1)
    {
    topbit = 0x80000000u;
    p++;
    len--;
    }
  chlen = utf82ord(p, &c);
  if (chlen <= 0) return -1;
  if (utf && c > 0x10ffff) return -2;
  p += chlen;
  len -= chlen;
  *pp++ = c | topbit;
  }

*pp = 0;
*lenptr = pp - pbuffer32;
return 0;
}
#endif /* SUPPORT_PCRE2_32 */



/*************************************************
*         Move back by so many characters        *
*************************************************/

/* Given a code unit offset in a subject string, move backwards by a number of
characters, and return the resulting offset.

Arguments:
  subject   pointer to the string
  offset    start offset
  count     count to move back by
  utf       TRUE if in UTF mode

Returns:   a possibly changed offset
*/

static PCRE2_SIZE
backchars(uint8_t *subject, PCRE2_SIZE offset, uint32_t count, BOOL utf)
{
if (!utf || test_mode == PCRE32_MODE)
  return (count >= offset)? 0 : (offset - count);

else if (test_mode == PCRE8_MODE)
  {
  PCRE2_SPTR8 pp = (PCRE2_SPTR8)subject + offset;
  for (; count > 0 && pp > (PCRE2_SPTR8)subject; count--)
    {
    pp--;
    while ((*pp & 0xc0) == 0x80) pp--;
    }
  return pp - (PCRE2_SPTR8)subject;
  }

else  /* 16-bit mode */
  {
  PCRE2_SPTR16 pp = (PCRE2_SPTR16)subject + offset;
  for (; count > 0 && pp > (PCRE2_SPTR16)subject; count--)
    {
    pp--;
    if ((*pp & 0xfc00) == 0xdc00) pp--;
    }
  return pp - (PCRE2_SPTR16)subject;
  }
}



/*************************************************
*           Expand input buffers                 *
*************************************************/

/* This function doubles the size of the input buffer and the buffer for
keeping an 8-bit copy of patterns (pbuffer8), and copies the current buffers to
the new ones.

Arguments: none
Returns:   nothing (aborts if malloc() fails)
*/

static void
expand_input_buffers(void)
{
int new_pbuffer8_size = 2*pbuffer8_size;
uint8_t *new_buffer = (uint8_t *)malloc(new_pbuffer8_size);
uint8_t *new_pbuffer8 = (uint8_t *)malloc(new_pbuffer8_size);

if (new_buffer == NULL || new_pbuffer8 == NULL)
  {
  fprintf(stderr, "pcre2test: malloc(%d) failed\n", new_pbuffer8_size);
  exit(1);
  }

memcpy(new_buffer, buffer, pbuffer8_size);
memcpy(new_pbuffer8, pbuffer8, pbuffer8_size);

pbuffer8_size = new_pbuffer8_size;

free(buffer);
free(pbuffer8);

buffer = new_buffer;
pbuffer8 = new_pbuffer8;
}



/*************************************************
*        Read or extend an input line            *
*************************************************/

/* Input lines are read into buffer, but both patterns and data lines can be
continued over multiple input lines. In addition, if the buffer fills up, we
want to automatically expand it so as to be able to handle extremely large
lines that are needed for certain stress tests, although this is less likely
now that there are repetition features for both patterns and data. When the
input buffer is expanded, the other two buffers must also be expanded likewise,
and the contents of pbuffer, which are a copy of the input for callouts, must
be preserved (for when expansion happens for a data line). This is not the most
optimal way of handling this, but hey, this is just a test program!

Arguments:
  f            the file to read
  start        where in buffer to start (this *must* be within buffer)
  prompt       for stdin or readline()

Returns:       pointer to the start of new data
               could be a copy of start, or could be moved
               NULL if no data read and EOF reached
*/

static uint8_t *
extend_inputline(FILE *f, uint8_t *start, const char *prompt)
{
uint8_t *here = start;

for (;;)
  {
  size_t rlen = (size_t)(pbuffer8_size - (here - buffer));

  if (rlen > 1000)
    {
    size_t dlen;

    /* If libreadline or libedit support is required, use readline() to read a
    line if the input is a terminal. Note that readline() removes the trailing
    newline, so we must put it back again, to be compatible with fgets(). */

#if defined(SUPPORT_LIBREADLINE) || defined(SUPPORT_LIBEDIT)
    if (INTERACTIVE(f))
      {
      size_t len;
      char *s = readline(prompt);
      if (s == NULL) return (here == start)? NULL : start;
      len = strlen(s);
      if (len > 0) add_history(s);
      if (len > rlen - 1) len = rlen - 1;
      memcpy(here, s, len);
      here[len] = '\n';
      here[len+1] = 0;
      free(s);
      }
    else
#endif

    /* Read the next line by normal means, prompting if the file is a tty. */

      {
      if (INTERACTIVE(f)) printf("%s", prompt);
      if (fgets((char *)here, rlen,  f) == NULL)
        return (here == start)? NULL : start;
      }

    dlen = strlen((char *)here);
    here += dlen;

    /* Check for end of line reached. Take care not to read data from before
    start (dlen will be zero for a file starting with a binary zero). */

    if (here > start && here[-1] == '\n') return start;

    /* If we have not read a newline when reading a file, we have either filled
    the buffer or reached the end of the file. We can detect the former by
    checking that the string fills the buffer, and the latter by feof(). If
    neither of these is true, it means we read a binary zero which has caused
    strlen() to give a short length. This is a hard error because pcre2test
    expects to work with C strings. */

    if (!INTERACTIVE(f) && dlen < rlen - 1 && !feof(f))
      {
      fprintf(outfile, "** Binary zero encountered in input\n");
      fprintf(outfile, "** pcre2test run abandoned\n");
      exit(1);
      }
    }

  else
    {
    size_t start_offset = start - buffer;
    size_t here_offset = here - buffer;
    expand_input_buffers();
    start = buffer + start_offset;
    here = buffer + here_offset;
    }
  }

/* Control never gets here */
}



/*************************************************
*         Case-independent strncmp() function    *
*************************************************/

/*
Arguments:
  s         first string
  t         second string
  n         number of characters to compare

Returns:    < 0, = 0, or > 0, according to the comparison
*/

static int
strncmpic(const uint8_t *s, const uint8_t *t, int n)
{
while (n--)
  {
  int c = tolower(*s++) - tolower(*t++);
  if (c != 0) return c;
  }
return 0;
}



/*************************************************
*          Scan the main modifier list           *
*************************************************/

/* This function searches the modifier list for a long modifier name.

Argument:
  p         start of the name
  lenp      length of the name

Returns:    an index in the modifier list, or -1 on failure
*/

static int
scan_modifiers(const uint8_t *p, unsigned int len)
{
int bot = 0;
int top = MODLISTCOUNT;

while (top > bot)
  {
  int mid = (bot + top)/2;
  unsigned int mlen = strlen(modlist[mid].name);
  int c = strncmp((char *)p, modlist[mid].name, (len < mlen)? len : mlen);
  if (c == 0)
    {
    if (len == mlen) return mid;
    c = (int)len - (int)mlen;
    }
  if (c > 0) bot = mid + 1; else top = mid;
  }

return -1;

}



/*************************************************
*        Check a modifer and find its field      *
*************************************************/

/* This function is called when a modifier has been identified. We check that
it is allowed here and find the field that is to be changed.

Arguments:
  m          the modifier list entry
  ctx        CTX_PAT     => pattern context
             CTX_POPPAT  => pattern context for popped pattern
             CTX_DEFPAT  => default pattern context
             CTX_DAT     => data context
             CTX_DEFDAT  => default data context
  pctl       point to pattern control block
  dctl       point to data control block
  c          a single character or 0

Returns:     a field pointer or NULL
*/

static void *
check_modifier(modstruct *m, int ctx, patctl *pctl, datctl *dctl, uint32_t c)
{
void *field = NULL;
PCRE2_SIZE offset = m->offset;

if (restrict_for_perl_test) switch(m->which)
  {
  case MOD_PNDP:
  case MOD_PATP:
  case MOD_PDP:
  break;

  default:
  fprintf(outfile, "** '%s' is not allowed in a Perl-compatible test\n",
    m->name);
  return NULL;
  }

switch (m->which)
  {
  case MOD_CTC:  /* Compile context modifier */
  if (ctx == CTX_DEFPAT) field = PTR(default_pat_context);
    else if (ctx == CTX_PAT) field = PTR(pat_context);
  break;

  case MOD_CTM:  /* Match context modifier */
  if (ctx == CTX_DEFDAT) field = PTR(default_dat_context);
    else if (ctx == CTX_DAT) field = PTR(dat_context);
  break;

  case MOD_DAT:  /* Data line modifier */
  if (dctl != NULL) field = dctl;
  break;

  case MOD_PAT:    /* Pattern modifier */
  case MOD_PATP:   /* Allowed for Perl test */
  if (pctl != NULL) field = pctl;
  break;

  case MOD_PD:   /* Pattern or data line modifier */
  case MOD_PDP:  /* Ditto, allowed for Perl test */
  case MOD_PND:  /* Ditto, but not default pattern */
  case MOD_PNDP: /* Ditto, allowed for Perl test */
  if (dctl != NULL) field = dctl;
    else if (pctl != NULL && (m->which == MOD_PD || m->which == MOD_PDP ||
             ctx != CTX_DEFPAT))
      field = pctl;
  break;
  }

if (field == NULL)
  {
  if (c == 0)
    fprintf(outfile, "** '%s' is not valid here\n", m->name);
  else
    fprintf(outfile, "** /%c is not valid here\n", c);
  return NULL;
  }

return (char *)field + offset;
}



/*************************************************
*            Decode a modifier list              *
*************************************************/

/* A pointer to a control block is NULL when called in cases when that block is
not relevant. They are never all relevant in one call. At least one of patctl
and datctl is NULL. The second argument specifies which context to use for
modifiers that apply to contexts.

Arguments:
  p          point to modifier string
  ctx        CTX_PAT     => pattern context
             CTX_POPPAT  => pattern context for popped pattern
             CTX_DEFPAT  => default pattern context
             CTX_DAT     => data context
             CTX_DEFDAT  => default data context
  pctl       point to pattern control block
  dctl       point to data control block

Returns: TRUE if successful decode, FALSE otherwise
*/

static BOOL
decode_modifiers(uint8_t *p, int ctx, patctl *pctl, datctl *dctl)
{
uint8_t *ep, *pp;
long li;
unsigned long uli;
BOOL first = TRUE;

for (;;)
  {
  void *field;
  modstruct *m;
  BOOL off = FALSE;
  unsigned int i, len;
  int index;
  char *endptr;

  /* Skip white space and commas. */

  while (isspace(*p) || *p == ',') p++;
  if (*p == 0) break;

  /* Find the end of the item; lose trailing whitespace at end of line. */

  for (ep = p; *ep != 0 && *ep != ','; ep++);
  if (*ep == 0)
    {
    while (ep > p && isspace(ep[-1])) ep--;
    *ep = 0;
    }

  /* Remember if the first character is '-'. */

  if (*p == '-')
    {
    off = TRUE;
    p++;
    }

  /* Find the length of a full-length modifier name, and scan for it. */

  pp = p;
  while (pp < ep && *pp != '=') pp++;
  index = scan_modifiers(p, pp - p);

  /* If the first modifier is unrecognized, try to interpret it as a sequence
  of single-character abbreviated modifiers. None of these modifiers have any
  associated data. They just set options or control bits. */

  if (index < 0)
    {
    uint32_t cc;
    uint8_t *mp = p;

    if (!first)
      {
      fprintf(outfile, "** Unrecognized modifier '%.*s'\n", (int)(ep-p), p);
      if (ep - p == 1)
        fprintf(outfile, "** Single-character modifiers must come first\n");
      return FALSE;
      }

    for (cc = *p; cc != ',' && cc != '\n' && cc != 0; cc = *(++p))
      {
      for (i = 0; i < C1MODLISTCOUNT; i++)
        if (cc == c1modlist[i].onechar) break;

      if (i >= C1MODLISTCOUNT)
        {
        fprintf(outfile, "** Unrecognized modifier '%c' in '%.*s'\n",
          *p, (int)(ep-mp), mp);
        return FALSE;
        }

      if (c1modlist[i].index >= 0)
        {
        index = c1modlist[i].index;
        }

      else
        {
        index = scan_modifiers((uint8_t *)(c1modlist[i].fullname),
          strlen(c1modlist[i].fullname));
        if (index < 0)
          {
          fprintf(outfile, "** Internal error: single-character equivalent "
            "modifier '%s' not found\n", c1modlist[i].fullname);
          return FALSE;
          }
        c1modlist[i].index = index;     /* Cache for next time */
        }

      field = check_modifier(modlist + index, ctx, pctl, dctl, *p);
      if (field == NULL) return FALSE;

      /* /x is a special case; a second appearance changes PCRE2_EXTENDED to
      PCRE2_EXTENDED_MORE. */

      if (cc == 'x' && (*((uint32_t *)field) & PCRE2_EXTENDED) != 0)
        {
        *((uint32_t *)field) &= ~PCRE2_EXTENDED;
        *((uint32_t *)field) |= PCRE2_EXTENDED_MORE;
        }
      else
        *((uint32_t *)field) |= modlist[index].value;
      }

    continue;    /* With tne next (fullname) modifier */
    }

  /* We have a match on a full-name modifier. Check for the existence of data
  when needed. */

  m = modlist + index;      /* Save typing */
  if (m->type != MOD_CTL && m->type != MOD_OPT &&
      (m->type != MOD_IND || *pp == '='))
    {
    if (*pp++ != '=')
      {
      fprintf(outfile, "** '=' expected after '%s'\n", m->name);
      return FALSE;
      }
    if (off)
      {
      fprintf(outfile, "** '-' is not valid for '%s'\n", m->name);
      return FALSE;
      }
    }

  /* These on/off types have no data. */

  else if (*pp != ',' && *pp != '\n' && *pp != ' ' && *pp != 0)
    {
    fprintf(outfile, "** Unrecognized modifier '%.*s'\n", (int)(ep-p), p);
    return FALSE;
    }

  /* Set the data length for those types that have data. Then find the field
  that is to be set. If check_modifier() returns NULL, it has already output an
  error message. */

  len = ep - pp;
  field = check_modifier(m, ctx, pctl, dctl, 0);
  if (field == NULL) return FALSE;

  /* Process according to data type. */

  switch (m->type)
    {
    case MOD_CTL:
    case MOD_OPT:
    if (off) *((uint32_t *)field) &= ~m->value;
      else *((uint32_t *)field) |= m->value;
    break;

    case MOD_BSR:
    if (len == 7 && strncmpic(pp, (const uint8_t *)"default", 7) == 0)
      {
#ifdef BSR_ANYCRLF
      *((uint16_t *)field) = PCRE2_BSR_ANYCRLF;
#else
      *((uint16_t *)field) = PCRE2_BSR_UNICODE;
#endif
      if (ctx == CTX_PAT || ctx == CTX_DEFPAT) pctl->control2 &= ~CTL2_BSR_SET;
        else dctl->control2 &= ~CTL2_BSR_SET;
      }
    else
      {
      if (len == 7 && strncmpic(pp, (const uint8_t *)"anycrlf", 7) == 0)
        *((uint16_t *)field) = PCRE2_BSR_ANYCRLF;
      else if (len == 7 && strncmpic(pp, (const uint8_t *)"unicode", 7) == 0)
        *((uint16_t *)field) = PCRE2_BSR_UNICODE;
      else goto INVALID_VALUE;
      if (ctx == CTX_PAT || ctx == CTX_DEFPAT) pctl->control2 |= CTL2_BSR_SET;
        else dctl->control2 |= CTL2_BSR_SET;
      }
    pp = ep;
    break;

    case MOD_CHR:  /* A single character */
    *((uint32_t *)field) = *pp++;
    break;

    case MOD_CON:  /* A convert type/options list */
    for (;; pp++)
      {
      uint8_t *colon = (uint8_t *)strchr((const char *)pp, ':');
      len = ((colon != NULL && colon < ep)? colon:ep) - pp;
      for (i = 0; i < convertlistcount; i++)
        {
        if (strncmpic(pp, (const uint8_t *)convertlist[i].name, len) == 0)
          {
          if (*((uint32_t *)field) == CONVERT_UNSET)
            *((uint32_t *)field) = convertlist[i].option;
          else
            *((uint32_t *)field) |= convertlist[i].option;
          break;
          }
        }
      if (i >= convertlistcount) goto INVALID_VALUE;
      pp += len;
      if (*pp != ':') break;
      }
    break;

    case MOD_IN2:    /* One or two unsigned integers */
    if (!isdigit(*pp)) goto INVALID_VALUE;
    uli = strtoul((const char *)pp, &endptr, 10);
    if (U32OVERFLOW(uli)) goto INVALID_VALUE;
    ((uint32_t *)field)[0] = (uint32_t)uli;
    if (*endptr == ':')
      {
      uli = strtoul((const char *)endptr+1, &endptr, 10);
      if (U32OVERFLOW(uli)) goto INVALID_VALUE;
      ((uint32_t *)field)[1] = (uint32_t)uli;
      }
    else ((uint32_t *)field)[1] = 0;
    pp = (uint8_t *)endptr;
    break;

    /* PCRE2_SIZE_MAX is usually SIZE_MAX, which may be greater, equal to, or
    less than ULONG_MAX. So first test for overflowing the long int, and then
    test for overflowing PCRE2_SIZE_MAX if it is smaller than ULONG_MAX. */

    case MOD_SIZ:    /* PCRE2_SIZE value */
    if (!isdigit(*pp)) goto INVALID_VALUE;
    uli = strtoul((const char *)pp, &endptr, 10);
    if (uli == ULONG_MAX) goto INVALID_VALUE;
#if ULONG_MAX > PCRE2_SIZE_MAX
    if (uli > PCRE2_SIZE_MAX) goto INVALID_VALUE;
#endif
    *((PCRE2_SIZE *)field) = (PCRE2_SIZE)uli;
    pp = (uint8_t *)endptr;
    break;

    case MOD_IND:    /* Unsigned integer with default */
    if (len == 0)
      {
      *((uint32_t *)field) = (uint32_t)(m->value);
      break;
      }
    /* Fall through */

    case MOD_INT:    /* Unsigned integer */
    if (!isdigit(*pp)) goto INVALID_VALUE;
    uli = strtoul((const char *)pp, &endptr, 10);
    if (U32OVERFLOW(uli)) goto INVALID_VALUE;
    *((uint32_t *)field) = (uint32_t)uli;
    pp = (uint8_t *)endptr;
    break;

    case MOD_INS:   /* Signed integer */
    if (!isdigit(*pp) && *pp != '-') goto INVALID_VALUE;
    li = strtol((const char *)pp, &endptr, 10);
    if (S32OVERFLOW(li)) goto INVALID_VALUE;
    *((int32_t *)field) = (int32_t)li;
    pp = (uint8_t *)endptr;
    break;

    case MOD_NL:
    for (i = 0; i < sizeof(newlines)/sizeof(char *); i++)
      if (len == strlen(newlines[i]) &&
        strncmpic(pp, (const uint8_t *)newlines[i], len) == 0) break;
    if (i >= sizeof(newlines)/sizeof(char *)) goto INVALID_VALUE;
    if (i == 0)
      {
      *((uint16_t *)field) = NEWLINE_DEFAULT;
      if (ctx == CTX_PAT || ctx == CTX_DEFPAT) pctl->control2 &= ~CTL2_NL_SET;
        else dctl->control2 &= ~CTL2_NL_SET;
      }
    else
      {
      *((uint16_t *)field) = i;
      if (ctx == CTX_PAT || ctx == CTX_DEFPAT) pctl->control2 |= CTL2_NL_SET;
        else dctl->control2 |= CTL2_NL_SET;
      }
    pp = ep;
    break;

    case MOD_NN:              /* Name or (signed) number; may be several */
    if (isdigit(*pp) || *pp == '-')
      {
      int ct = MAXCPYGET - 1;
      int32_t value;
      li = strtol((const char *)pp, &endptr, 10);
      if (S32OVERFLOW(li)) goto INVALID_VALUE;
      value = (int32_t)li;
      field = (char *)field - m->offset + m->value;      /* Adjust field ptr */
      if (value >= 0)                                    /* Add new number */
        {
        while (*((int32_t *)field) >= 0 && ct-- > 0)   /* Skip previous */
          field = (char *)field + sizeof(int32_t);
        if (ct <= 0)
          {
          fprintf(outfile, "** Too many numeric '%s' modifiers\n", m->name);
          return FALSE;
          }
        }
      *((int32_t *)field) = value;
      if (ct > 0) ((int32_t *)field)[1] = -1;
      pp = (uint8_t *)endptr;
      }

    /* Multiple strings are put end to end. */

    else
      {
      char *nn = (char *)field;
      if (len > 0)                    /* Add new name */
        {
        if (len > MAX_NAME_SIZE)
          {
          fprintf(outfile, "** Group name in '%s' is too long\n", m->name);
          return FALSE;
          }
        while (*nn != 0) nn += strlen(nn) + 1;
        if (nn + len + 2 - (char *)field > LENCPYGET)
          {
          fprintf(outfile, "** Too many characters in named '%s' modifiers\n",
            m->name);
          return FALSE;
          }
        memcpy(nn, pp, len);
        }
      nn[len] = 0 ;
      nn[len+1] = 0;
      pp = ep;
      }
    break;

    case MOD_STR:
    if (len + 1 > m->value)
      {
      fprintf(outfile, "** Overlong value for '%s' (max %d code units)\n",
        m->name, m->value - 1);
      return FALSE;
      }
    memcpy(field, pp, len);
    ((uint8_t *)field)[len] = 0;
    pp = ep;
    break;
    }

  if (*pp != ',' && *pp != '\n' && *pp != ' ' && *pp != 0)
    {
    fprintf(outfile, "** Comma expected after modifier item '%s'\n", m->name);
    return FALSE;
    }

  p = pp;
  first = FALSE;

  if (ctx == CTX_POPPAT &&
     (pctl->options != 0 ||
      pctl->tables_id != 0 ||
      pctl->locale[0] != 0 ||
      (pctl->control & NOTPOP_CONTROLS) != 0))
    {
    fprintf(outfile, "** '%s' is not valid here\n", m->name);
    return FALSE;
    }
  }

return TRUE;

INVALID_VALUE:
fprintf(outfile, "** Invalid value in '%.*s'\n", (int)(ep-p), p);
return FALSE;
}


/*************************************************
*             Get info from a pattern            *
*************************************************/

/* A wrapped call to pcre2_pattern_info(), applied to the current compiled
pattern.

Arguments:
  what        code for the required information
  where       where to put the answer
  unsetok     PCRE2_ERROR_UNSET is an "expected" result

Returns:      the return from pcre2_pattern_info()
*/

static int
pattern_info(int what, void *where, BOOL unsetok)
{
int rc;
PCRE2_PATTERN_INFO(rc, compiled_code, what, NULL);  /* Exercise the code */
PCRE2_PATTERN_INFO(rc, compiled_code, what, where);
if (rc >= 0) return 0;
if (rc != PCRE2_ERROR_UNSET || !unsetok)
  {
  fprintf(outfile, "Error %d from pcre2_pattern_info_%d(%d)\n", rc, test_mode,
    what);
  if (rc == PCRE2_ERROR_BADMODE)
    fprintf(outfile, "Running in %d-bit mode but pattern was compiled in "
      "%d-bit mode\n", test_mode,
      8 * (FLD(compiled_code, flags) & PCRE2_MODE_MASK));
  }
return rc;
}



#ifdef SUPPORT_PCRE2_8
/*************************************************
*             Show something in a list           *
*************************************************/

/* This function just helps to keep the code that uses it tidier. It's used for
various lists of things where there needs to be introductory text before the
first item. As these calls are all in the POSIX-support code, they happen only
when 8-bit mode is supported. */

static void
prmsg(const char **msg, const char *s)
{
fprintf(outfile, "%s %s", *msg, s);
*msg = "";
}
#endif  /* SUPPORT_PCRE2_8 */



/*************************************************
*                Show control bits               *
*************************************************/

/* Called for mutually exclusive controls and for unsupported POSIX controls.
Because the bits are unique, this can be used for both pattern and data control
words.

Arguments:
  controls    control bits
  controls2   more control bits
  before      text to print before

Returns:      nothing
*/

static void
show_controls(uint32_t controls, uint32_t controls2, const char *before)
{
fprintf(outfile, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
  before,
  ((controls & CTL_AFTERTEXT) != 0)? " aftertext" : "",
  ((controls & CTL_ALLAFTERTEXT) != 0)? " allaftertext" : "",
  ((controls & CTL_ALLCAPTURES) != 0)? " allcaptures" : "",
  ((controls & CTL_ALLUSEDTEXT) != 0)? " allusedtext" : "",
  ((controls & CTL_ALTGLOBAL) != 0)? " altglobal" : "",
  ((controls & CTL_BINCODE) != 0)? " bincode" : "",
  ((controls2 & CTL2_BSR_SET) != 0)? " bsr" : "",
  ((controls & CTL_CALLOUT_CAPTURE) != 0)? " callout_capture" : "",
  ((controls & CTL_CALLOUT_INFO) != 0)? " callout_info" : "",
  ((controls & CTL_CALLOUT_NONE) != 0)? " callout_none" : "",
  ((controls2 & CTL2_CALLOUT_NO_WHERE) != 0)? " callout_no_where" : "",
  ((controls & CTL_DFA) != 0)? " dfa" : "",
  ((controls & CTL_EXPAND) != 0)? " expand" : "",
  ((controls & CTL_FINDLIMITS) != 0)? " find_limits" : "",
  ((controls & CTL_FRAMESIZE) != 0)? " framesize" : "",
  ((controls & CTL_FULLBINCODE) != 0)? " fullbincode" : "",
  ((controls & CTL_GETALL) != 0)? " getall" : "",
  ((controls & CTL_GLOBAL) != 0)? " global" : "",
  ((controls & CTL_HEXPAT) != 0)? " hex" : "",
  ((controls & CTL_INFO) != 0)? " info" : "",
  ((controls & CTL_JITFAST) != 0)? " jitfast" : "",
  ((controls & CTL_JITVERIFY) != 0)? " jitverify" : "",
  ((controls & CTL_MARK) != 0)? " mark" : "",
  ((controls & CTL_MEMORY) != 0)? " memory" : "",
  ((controls2 & CTL2_NL_SET) != 0)? " newline" : "",
  ((controls & CTL_NULLCONTEXT) != 0)? " null_context" : "",
  ((controls & CTL_POSIX) != 0)? " posix" : "",
  ((controls & CTL_POSIX_NOSUB) != 0)? " posix_nosub" : "",
  ((controls & CTL_PUSH) != 0)? " push" : "",
  ((controls & CTL_PUSHCOPY) != 0)? " pushcopy" : "",
  ((controls & CTL_PUSHTABLESCOPY) != 0)? " pushtablescopy" : "",
  ((controls & CTL_STARTCHAR) != 0)? " startchar" : "",
  ((controls2 & CTL2_SUBSTITUTE_EXTENDED) != 0)? " substitute_extended" : "",
  ((controls2 & CTL2_SUBSTITUTE_OVERFLOW_LENGTH) != 0)? " substitute_overflow_length" : "",
  ((controls2 & CTL2_SUBSTITUTE_UNKNOWN_UNSET) != 0)? " substitute_unknown_unset" : "",
  ((controls2 & CTL2_SUBSTITUTE_UNSET_EMPTY) != 0)? " substitute_unset_empty" : "",
  ((controls & CTL_USE_LENGTH) != 0)? " use_length" : "",
  ((controls & CTL_UTF8_INPUT) != 0)? " utf8_input" : "",
  ((controls & CTL_ZERO_TERMINATE) != 0)? " zero_terminate" : "");
}



/*************************************************
*                Show compile options            *
*************************************************/

/* Called from show_pattern_info() and for unsupported POSIX options.

Arguments:
  options     an options word
  before      text to print before
  after       text to print after

Returns:      nothing
*/

static void
show_compile_options(uint32_t options, const char *before, const char *after)
{
if (options == 0) fprintf(outfile, "%s <none>%s", before, after);
else fprintf(outfile, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
  before,
  ((options & PCRE2_ALT_BSUX) != 0)? " alt_bsux" : "",
  ((options & PCRE2_ALT_CIRCUMFLEX) != 0)? " alt_circumflex" : "",
  ((options & PCRE2_ALT_VERBNAMES) != 0)? " alt_verbnames" : "",
  ((options & PCRE2_ALLOW_EMPTY_CLASS) != 0)? " allow_empty_class" : "",
  ((options & PCRE2_ANCHORED) != 0)? " anchored" : "",
  ((options & PCRE2_AUTO_CALLOUT) != 0)? " auto_callout" : "",
  ((options & PCRE2_CASELESS) != 0)? " caseless" : "",
  ((options & PCRE2_DOLLAR_ENDONLY) != 0)? " dollar_endonly" : "",
  ((options & PCRE2_DOTALL) != 0)? " dotall" : "",
  ((options & PCRE2_DUPNAMES) != 0)? " dupnames" : "",
  ((options & PCRE2_ENDANCHORED) != 0)? " endanchored" : "",
  ((options & PCRE2_EXTENDED) != 0)? " extended" : "",
  ((options & PCRE2_EXTENDED_MORE) != 0)? " extended_more" : "",
  ((options & PCRE2_FIRSTLINE) != 0)? " firstline" : "",
  ((options & PCRE2_LITERAL) != 0)? " literal" : "",
  ((options & PCRE2_MATCH_UNSET_BACKREF) != 0)? " match_unset_backref" : "",
  ((options & PCRE2_MULTILINE) != 0)? " multiline" : "",
  ((options & PCRE2_NEVER_BACKSLASH_C) != 0)? " never_backslash_c" : "",
  ((options & PCRE2_NEVER_UCP) != 0)? " never_ucp" : "",
  ((options & PCRE2_NEVER_UTF) != 0)? " never_utf" : "",
  ((options & PCRE2_NO_AUTO_CAPTURE) != 0)? " no_auto_capture" : "",
  ((options & PCRE2_NO_AUTO_POSSESS) != 0)? " no_auto_possess" : "",
  ((options & PCRE2_NO_DOTSTAR_ANCHOR) != 0)? " no_dotstar_anchor" : "",
  ((options & PCRE2_NO_UTF_CHECK) != 0)? " no_utf_check" : "",
  ((options & PCRE2_NO_START_OPTIMIZE) != 0)? " no_start_optimize" : "",
  ((options & PCRE2_UCP) != 0)? " ucp" : "",
  ((options & PCRE2_UNGREEDY) != 0)? " ungreedy" : "",
  ((options & PCRE2_USE_OFFSET_LIMIT) != 0)? " use_offset_limit" : "",
  ((options & PCRE2_UTF) != 0)? " utf" : "",
  after);
}


/*************************************************
*           Show compile extra options           *
*************************************************/

/* Called only for unsupported POSIX options at present, and therefore needed
only when the 8-bit library is being compiled.

Arguments:
  options     an options word
  before      text to print before
  after       text to print after

Returns:      nothing
*/

#ifdef SUPPORT_PCRE2_8
static void
show_compile_extra_options(uint32_t options, const char *before,
  const char *after)
{
if (options == 0) fprintf(outfile, "%s <none>%s", before, after);
else fprintf(outfile, "%s%s%s%s%s%s",
  before,
  ((options & PCRE2_EXTRA_ALLOW_SURROGATE_ESCAPES) != 0)? " allow_surrogate_escapes" : "",
  ((options & PCRE2_EXTRA_BAD_ESCAPE_IS_LITERAL) != 0)? " bad_escape_is_literal" : "",
  ((options & PCRE2_EXTRA_MATCH_WORD) != 0)? " match_word" : "",
  ((options & PCRE2_EXTRA_MATCH_LINE) != 0)? " match_line" : "",
  after);
}
#endif



#ifdef SUPPORT_PCRE2_8
/*************************************************
*                Show match options              *
*************************************************/

/* Called for unsupported POSIX options. */

static void
show_match_options(uint32_t options)
{
fprintf(outfile, "%s%s%s%s%s%s%s%s%s%s%s",
  ((options & PCRE2_ANCHORED) != 0)? " anchored" : "",
  ((options & PCRE2_DFA_RESTART) != 0)? " dfa_restart" : "",
  ((options & PCRE2_DFA_SHORTEST) != 0)? " dfa_shortest" : "",
  ((options & PCRE2_ENDANCHORED) != 0)? " endanchored" : "",
  ((options & PCRE2_NO_UTF_CHECK) != 0)? " no_utf_check" : "",
  ((options & PCRE2_NOTBOL) != 0)? " notbol" : "",
  ((options & PCRE2_NOTEMPTY) != 0)? " notempty" : "",
  ((options & PCRE2_NOTEMPTY_ATSTART) != 0)? " notempty_atstart" : "",
  ((options & PCRE2_NOTEOL) != 0)? " noteol" : "",
  ((options & PCRE2_PARTIAL_HARD) != 0)? " partial_hard" : "",
  ((options & PCRE2_PARTIAL_SOFT) != 0)? " partial_soft" : "");
}
#endif  /* SUPPORT_PCRE2_8 */



/*************************************************
*      Show memory usage info for a pattern      *
*************************************************/

static void
show_memory_info(void)
{
uint32_t name_count, name_entry_size;
size_t size, cblock_size;

/* One of the test_mode values will always be true, but to stop a compiler
warning we must initialize cblock_size. */

cblock_size = 0;
#ifdef SUPPORT_PCRE2_8
if (test_mode == PCRE8_MODE) cblock_size = sizeof(pcre2_real_code_8);
#endif
#ifdef SUPPORT_PCRE2_16
if (test_mode == PCRE16_MODE) cblock_size = sizeof(pcre2_real_code_16);
#endif
#ifdef SUPPORT_PCRE2_32
if (test_mode == PCRE32_MODE) cblock_size = sizeof(pcre2_real_code_32);
#endif

(void)pattern_info(PCRE2_INFO_SIZE, &size, FALSE);
(void)pattern_info(PCRE2_INFO_NAMECOUNT, &name_count, FALSE);
(void)pattern_info(PCRE2_INFO_NAMEENTRYSIZE, &name_entry_size, FALSE);
fprintf(outfile, "Memory allocation (code space): %d\n",
  (int)(size - name_count*name_entry_size*code_unit_size - cblock_size));
if (pat_patctl.jit != 0)
  {
  (void)pattern_info(PCRE2_INFO_JITSIZE, &size, FALSE);
  fprintf(outfile, "Memory allocation (JIT code): %d\n", (int)size);
  }
}



/*************************************************
*       Show frame size info for a pattern       *
*************************************************/

static void
show_framesize(void)
{
size_t frame_size;
(void)pattern_info(PCRE2_INFO_FRAMESIZE, &frame_size, FALSE);
fprintf(outfile, "Frame size for pcre2_match(): %d\n", (int)frame_size);
}



/*************************************************
*         Get and output an error message        *
*************************************************/

static BOOL
print_error_message(int errorcode, const char *before, const char *after)
{
int len;
PCRE2_GET_ERROR_MESSAGE(len, errorcode, pbuffer);
if (len < 0)
  {
  fprintf(outfile, "\n** pcre2test internal error: cannot interpret error "
    "number\n** Unexpected return (%d) from pcre2_get_error_message()\n", len);
  }
else
  {
  fprintf(outfile, "%s", before);
  PCHARSV(CASTVAR(void *, pbuffer), 0, len, FALSE, outfile);
  fprintf(outfile, "%s", after);
  }
return len >= 0;
}


/*************************************************
*     Callback function for callout enumeration  *
*************************************************/

/* The only differences in the callout emumeration block for different code
unit widths are that the pointers to the subject, the most recent MARK, and a
callout argument string point to strings of the appropriate width. Casts can be
used to deal with this.

Argument:
  cb            pointer to enumerate block
  callout_data  user data

Returns:    0
*/

static int callout_callback(pcre2_callout_enumerate_block_8 *cb,
  void *callout_data)
{
uint32_t i;
BOOL utf = (FLD(compiled_code, overall_options) & PCRE2_UTF) != 0;

(void)callout_data;  /* Not currently displayed */

fprintf(outfile, "Callout ");
if (cb->callout_string != NULL)
  {
  uint32_t delimiter = CODE_UNIT(cb->callout_string, -1);
  fprintf(outfile, "%c", delimiter);
  PCHARSV(cb->callout_string, 0,
    cb->callout_string_length, utf, outfile);
  for (i = 0; callout_start_delims[i] != 0; i++)
    if (delimiter == callout_start_delims[i])
      {
      delimiter = callout_end_delims[i];
      break;
      }
  fprintf(outfile, "%c  ", delimiter);
  }
else fprintf(outfile, "%d  ", cb->callout_number);

fprintf(outfile, "%.*s\n",
  (int)((cb->next_item_length == 0)? 1 : cb->next_item_length),
  pbuffer8 + cb->pattern_position);

return 0;
}



/*************************************************
*        Show information about a pattern        *
*************************************************/

/* This function is called after a pattern has been compiled if any of the
information-requesting controls have been set.

Arguments:  none

Returns:    PR_OK     continue processing next line
            PR_SKIP   skip to a blank line
            PR_ABEND  abort the pcre2test run
*/

static int
show_pattern_info(void)
{
uint32_t compile_options, overall_options;

if ((pat_patctl.control & (CTL_BINCODE|CTL_FULLBINCODE)) != 0)
  {
  fprintf(outfile, "------------------------------------------------------------------\n");
  PCRE2_PRINTINT((pat_patctl.control & CTL_FULLBINCODE) != 0);
  }

if ((pat_patctl.control & CTL_INFO) != 0)
  {
  int rc;
  void *nametable;
  uint8_t *start_bits;
  BOOL heap_limit_set, match_limit_set, depth_limit_set;
  uint32_t backrefmax, bsr_convention, capture_count, first_ctype, first_cunit,
    hasbackslashc, hascrorlf, jchanged, last_ctype, last_cunit, match_empty,
    depth_limit, heap_limit, match_limit, minlength, nameentrysize, namecount,
    newline_convention;

  /* Exercise the error route. */

  PCRE2_PATTERN_INFO(rc, compiled_code, 999, NULL);
  (void)rc;

  /* These info requests may return PCRE2_ERROR_UNSET. */

  switch(pattern_info(PCRE2_INFO_HEAPLIMIT, &heap_limit, TRUE))
    {
    case 0:
    heap_limit_set = TRUE;
    break;

    case PCRE2_ERROR_UNSET:
    heap_limit_set = FALSE;
    break;

    default:
    return PR_ABEND;
    }

  switch(pattern_info(PCRE2_INFO_MATCHLIMIT, &match_limit, TRUE))
    {
    case 0:
    match_limit_set = TRUE;
    break;

    case PCRE2_ERROR_UNSET:
    match_limit_set = FALSE;
    break;

    default:
    return PR_ABEND;
    }

  switch(pattern_info(PCRE2_INFO_DEPTHLIMIT, &depth_limit, TRUE))
    {
    case 0:
    depth_limit_set = TRUE;
    break;

    case PCRE2_ERROR_UNSET:
    depth_limit_set = FALSE;
    break;

    default:
    return PR_ABEND;
    }

  /* These info requests should always succeed. */

  if (pattern_info(PCRE2_INFO_BACKREFMAX, &backrefmax, FALSE) +
      pattern_info(PCRE2_INFO_BSR, &bsr_convention, FALSE) +
      pattern_info(PCRE2_INFO_CAPTURECOUNT, &capture_count, FALSE) +
      pattern_info(PCRE2_INFO_FIRSTBITMAP, &start_bits, FALSE) +
      pattern_info(PCRE2_INFO_FIRSTCODEUNIT, &first_cunit, FALSE) +
      pattern_info(PCRE2_INFO_FIRSTCODETYPE, &first_ctype, FALSE) +
      pattern_info(PCRE2_INFO_HASBACKSLASHC, &hasbackslashc, FALSE) +
      pattern_info(PCRE2_INFO_HASCRORLF, &hascrorlf, FALSE) +
      pattern_info(PCRE2_INFO_JCHANGED, &jchanged, FALSE) +
      pattern_info(PCRE2_INFO_LASTCODEUNIT, &last_cunit, FALSE) +
      pattern_info(PCRE2_INFO_LASTCODETYPE, &last_ctype, FALSE) +
      pattern_info(PCRE2_INFO_MATCHEMPTY, &match_empty, FALSE) +
      pattern_info(PCRE2_INFO_MINLENGTH, &minlength, FALSE) +
      pattern_info(PCRE2_INFO_NAMECOUNT, &namecount, FALSE) +
      pattern_info(PCRE2_INFO_NAMEENTRYSIZE, &nameentrysize, FALSE) +
      pattern_info(PCRE2_INFO_NAMETABLE, &nametable, FALSE) +
      pattern_info(PCRE2_INFO_NEWLINE, &newline_convention, FALSE)
      != 0)
    return PR_ABEND;

  fprintf(outfile, "Capturing subpattern count = %d\n", capture_count);

  if (backrefmax > 0)
    fprintf(outfile, "Max back reference = %d\n", backrefmax);

  if (maxlookbehind > 0)
    fprintf(outfile, "Max lookbehind = %d\n", maxlookbehind);

  if (heap_limit_set)
    fprintf(outfile, "Heap limit = %u\n", heap_limit);

  if (match_limit_set)
    fprintf(outfile, "Match limit = %u\n", match_limit);

  if (depth_limit_set)
    fprintf(outfile, "Depth limit = %u\n", depth_limit);

  if (namecount > 0)
    {
    fprintf(outfile, "Named capturing subpatterns:\n");
    for (; namecount > 0; namecount--)
      {
      int imm2_size = test_mode == PCRE8_MODE ? 2 : 1;
      uint32_t length = (uint32_t)STRLEN(nametable + imm2_size);
      fprintf(outfile, "  ");
      PCHARSV(nametable, imm2_size, length, FALSE, outfile);
      while (length++ < nameentrysize - imm2_size) putc(' ', outfile);
#ifdef SUPPORT_PCRE2_32
      if (test_mode == PCRE32_MODE)
        fprintf(outfile, "%3d\n", (int)(((PCRE2_SPTR32)nametable)[0]));
#endif
#ifdef SUPPORT_PCRE2_16
      if (test_mode == PCRE16_MODE)
        fprintf(outfile, "%3d\n", (int)(((PCRE2_SPTR16)nametable)[0]));
#endif
#ifdef SUPPORT_PCRE2_8
      if (test_mode == PCRE8_MODE)
        fprintf(outfile, "%3d\n", (int)(
        ((((PCRE2_SPTR8)nametable)[0]) << 8) | ((PCRE2_SPTR8)nametable)[1]));
#endif
      nametable = (void*)((PCRE2_SPTR8)nametable + nameentrysize * code_unit_size);
      }
    }

  if (hascrorlf)     fprintf(outfile, "Contains explicit CR or LF match\n");
  if (hasbackslashc) fprintf(outfile, "Contains \\C\n");
  if (match_empty)   fprintf(outfile, "May match empty string\n");

  pattern_info(PCRE2_INFO_ARGOPTIONS, &compile_options, FALSE);
  pattern_info(PCRE2_INFO_ALLOPTIONS, &overall_options, FALSE);

  /* Remove UTF/UCP if they were there only because of forbid_utf. This saves
  cluttering up the verification output of non-UTF test files. */

  if ((pat_patctl.options & PCRE2_NEVER_UTF) == 0)
    {
    compile_options &= ~PCRE2_NEVER_UTF;
    overall_options &= ~PCRE2_NEVER_UTF;
    }

  if ((pat_patctl.options & PCRE2_NEVER_UCP) == 0)
    {
    compile_options &= ~PCRE2_NEVER_UCP;
    overall_options &= ~PCRE2_NEVER_UCP;
    }

  if ((compile_options|overall_options) != 0)
    {
    if (compile_options == overall_options)
      show_compile_options(compile_options, "Options:", "\n");
    else
      {
      show_compile_options(compile_options, "Compile options:", "\n");
      show_compile_options(overall_options, "Overall options:", "\n");
      }
    }

  if (jchanged) fprintf(outfile, "Duplicate name status changes\n");

  if ((pat_patctl.control2 & CTL2_BSR_SET) != 0 ||
      (FLD(compiled_code, flags) & PCRE2_BSR_SET) != 0)
    fprintf(outfile, "\\R matches %s\n", (bsr_convention == PCRE2_BSR_UNICODE)?
      "any Unicode newline" : "CR, LF, or CRLF");

  if ((FLD(compiled_code, flags) & PCRE2_NL_SET) != 0)
    {
    switch (newline_convention)
      {
      case PCRE2_NEWLINE_CR:
      fprintf(outfile, "Forced newline is CR\n");
      break;

      case PCRE2_NEWLINE_LF:
      fprintf(outfile, "Forced newline is LF\n");
      break;

      case PCRE2_NEWLINE_CRLF:
      fprintf(outfile, "Forced newline is CRLF\n");
      break;

      case PCRE2_NEWLINE_ANYCRLF:
      fprintf(outfile, "Forced newline is CR, LF, or CRLF\n");
      break;

      case PCRE2_NEWLINE_ANY:
      fprintf(outfile, "Forced newline is any Unicode newline\n");
      break;

      case PCRE2_NEWLINE_NUL:
      fprintf(outfile, "Forced newline is NUL\n");
      break;

      default:
      break;
      }
    }

  if (first_ctype == 2)
    {
    fprintf(outfile, "First code unit at start or follows newline\n");
    }
  else if (first_ctype == 1)
    {
    const char *caseless =
      ((FLD(compiled_code, flags) & PCRE2_FIRSTCASELESS) == 0)?
      "" : " (caseless)";
    if (PRINTOK(first_cunit))
      fprintf(outfile, "First code unit = \'%c\'%s\n", first_cunit, caseless);
    else
      {
      fprintf(outfile, "First code unit = ");
      pchar(first_cunit, FALSE, outfile);
      fprintf(outfile, "%s\n", caseless);
      }
    }
  else if (start_bits != NULL)
    {
    int i;
    int c = 24;
    fprintf(outfile, "Starting code units: ");
    for (i = 0; i < 256; i++)
      {
      if ((start_bits[i/8] & (1<<(i&7))) != 0)
        {
        if (c > 75)
          {
          fprintf(outfile, "\n  ");
          c = 2;
          }
        if (PRINTOK(i) && i != ' ')
          {
          fprintf(outfile, "%c ", i);
          c += 2;
          }
        else
          {
          fprintf(outfile, "\\x%02x ", i);
          c += 5;
          }
        }
      }
    fprintf(outfile, "\n");
    }

  if (last_ctype != 0)
    {
    const char *caseless =
      ((FLD(compiled_code, flags) & PCRE2_LASTCASELESS) == 0)?
      "" : " (caseless)";
    if (PRINTOK(last_cunit))
      fprintf(outfile, "Last code unit = \'%c\'%s\n", last_cunit, caseless);
    else
      {
      fprintf(outfile, "Last code unit = ");
      pchar(last_cunit, FALSE, outfile);
      fprintf(outfile, "%s\n", caseless);
      }
    }

  fprintf(outfile, "Subject length lower bound = %d\n", minlength);

  if (pat_patctl.jit != 0 && (pat_patctl.control & CTL_JITVERIFY) != 0)
    {
    if (FLD(compiled_code, executable_jit) != NULL)
      fprintf(outfile, "JIT compilation was successful\n");
    else
      {
#ifdef SUPPORT_JIT
      fprintf(outfile, "JIT compilation was not successful");
      if (jitrc != 0 && !print_error_message(jitrc, " (", ")"))
        return PR_ABEND;
      fprintf(outfile, "\n");
#else
      fprintf(outfile, "JIT support is not available in this version of PCRE2\n");
#endif
      }
    }
  }

if ((pat_patctl.control & CTL_CALLOUT_INFO) != 0)
  {
  int errorcode;
  PCRE2_CALLOUT_ENUMERATE(errorcode, callout_callback, 0);
  if (errorcode != 0)
    {
    fprintf(outfile, "Callout enumerate failed: error %d: ", errorcode);
    if (errorcode < 0 && !print_error_message(errorcode, "", "\n"))
      return PR_ABEND;
    return PR_SKIP;
    }
  }

return PR_OK;
}



/*************************************************
*              Handle serialization error        *
*************************************************/

/* Print an error message after a serialization failure.

Arguments:
  rc         the error code
  msg        an initial message for what failed

Returns:     FALSE if print_error_message() fails
*/

static BOOL
serial_error(int rc, const char *msg)
{
fprintf(outfile, "%s failed: error %d: ", msg, rc);
return print_error_message(rc, "", "\n");
}



/*************************************************
*        Open file for save/load commands        *
*************************************************/

/* This function decodes the file name and opens the file.

Arguments:
  buffptr     point after the #command
  mode        open mode
  fptr        points to the FILE variable

Returns:      PR_OK or PR_ABEND
*/

static int
open_file(uint8_t *buffptr, const char *mode, FILE **fptr)
{
char *endf;
char *filename = (char *)buffptr;
while (isspace(*filename)) filename++;
endf = filename + strlen8(filename);
while (endf > filename && isspace(endf[-1])) endf--;

if (endf == filename)
  {
  fprintf(outfile, "** File name expected after #save\n");
  return PR_ABEND;
  }

*endf = 0;
*fptr = fopen((const char *)filename, mode);
if (*fptr == NULL)
  {
  fprintf(outfile, "** Failed to open '%s': %s\n", filename, strerror(errno));
  return PR_ABEND;
  }

return PR_OK;
}



/*************************************************
*               Process command line             *
*************************************************/

/* This function is called for lines beginning with # and a character that is
not ! or whitespace, when encountered between tests, which means that there is
no compiled pattern (compiled_code is NULL). The line is in buffer.

Arguments:  none

Returns:    PR_OK     continue processing next line
            PR_SKIP   skip to a blank line
            PR_ABEND  abort the pcre2test run
*/

static int
process_command(void)
{
FILE *f;
PCRE2_SIZE serial_size;
size_t i;
int rc, cmd, cmdlen, yield;
uint16_t first_listed_newline;
const char *cmdname;
uint8_t *argptr, *serial;

if (restrict_for_perl_test)
  {
  fprintf(outfile, "** #-commands are not allowed after #perltest\n");
  return PR_ABEND;
  }

yield = PR_OK;
cmd = CMD_UNKNOWN;
cmdlen = 0;

for (i = 0; i < cmdlistcount; i++)
  {
  cmdname = cmdlist[i].name;
  cmdlen = strlen(cmdname);
  if (strncmp((char *)(buffer+1), cmdname, cmdlen) == 0 &&
      isspace(buffer[cmdlen+1]))
    {
    cmd = cmdlist[i].value;
    break;
    }
  }

argptr = buffer + cmdlen + 1;

switch(cmd)
  {
  case CMD_UNKNOWN:
  fprintf(outfile, "** Unknown command: %s", buffer);
  break;

  case CMD_FORBID_UTF:
  forbid_utf = PCRE2_NEVER_UTF|PCRE2_NEVER_UCP;
  break;

  case CMD_PERLTEST:
  restrict_for_perl_test = TRUE;
  break;

  /* Set default pattern modifiers */

  case CMD_PATTERN:
  (void)decode_modifiers(argptr, CTX_DEFPAT, &def_patctl, NULL);
  if (def_patctl.jit == 0 && (def_patctl.control & CTL_JITVERIFY) != 0)
    def_patctl.jit = 7;
  break;

  /* Set default subject modifiers */

  case CMD_SUBJECT:
  (void)decode_modifiers(argptr, CTX_DEFDAT, NULL, &def_datctl);
  break;

  /* Check the default newline, and if not one of those listed, set up the
  first one to be forced. An empty list unsets. */

  case CMD_NEWLINE_DEFAULT:
  local_newline_default = 0;   /* Unset */
  first_listed_newline = 0;
  for (;;)
    {
    while (isspace(*argptr)) argptr++;
    if (*argptr == 0) break;
    for (i = 1; i < sizeof(newlines)/sizeof(char *); i++)
      {
      size_t nlen = strlen(newlines[i]);
      if (strncmpic(argptr, (const uint8_t *)newlines[i], nlen) == 0 &&
          isspace(argptr[nlen]))
        {
        if (i == NEWLINE_DEFAULT) return PR_OK;  /* Default is valid */
        if (first_listed_newline == 0) first_listed_newline = i;
        }
      }
    while (*argptr != 0 && !isspace(*argptr)) argptr++;
    }
  local_newline_default = first_listed_newline;
  break;

  /* Pop or copy a compiled pattern off the stack. Modifiers that do not affect
  the compiled pattern (e.g. to give information) are permitted. The default
  pattern modifiers are ignored. */

  case CMD_POP:
  case CMD_POPCOPY:
  if (patstacknext <= 0)
    {
    fprintf(outfile, "** Can't pop off an empty stack\n");
    return PR_SKIP;
    }
  memset(&pat_patctl, 0, sizeof(patctl));   /* Completely unset */
  if (!decode_modifiers(argptr, CTX_POPPAT, &pat_patctl, NULL))
    return PR_SKIP;

  if (cmd == CMD_POP)
    {
    SET(compiled_code, patstack[--patstacknext]);
    }
  else
    {
    PCRE2_CODE_COPY_FROM_VOID(compiled_code, patstack[patstacknext - 1]);
    }

  if (pat_patctl.jit != 0)
    {
    PCRE2_JIT_COMPILE(jitrc, compiled_code, pat_patctl.jit);
    }
  if ((pat_patctl.control & CTL_MEMORY) != 0) show_memory_info();
  if ((pat_patctl.control & CTL_FRAMESIZE) != 0) show_framesize();
  if ((pat_patctl.control & CTL_ANYINFO) != 0)
    {
    rc = show_pattern_info();
    if (rc != PR_OK) return rc;
    }
  break;

  /* Save the stack of compiled patterns to a file, then empty the stack. */

  case CMD_SAVE:
  if (patstacknext <= 0)
    {
    fprintf(outfile, "** No stacked patterns to save\n");
    return PR_OK;
    }

  rc = open_file(argptr+1, BINARY_OUTPUT_MODE, &f);
  if (rc != PR_OK) return rc;

  PCRE2_SERIALIZE_ENCODE(rc, patstack, patstacknext, &serial, &serial_size,
    general_context);
  if (rc < 0)
    {
    fclose(f);
    if (!serial_error(rc, "Serialization")) return PR_ABEND;
    break;
    }

  /* Write the length at the start of the file to make it straightforward to
  get the right memory when re-loading. This saves having to read the file size
  in different operating systems. To allow for different endianness (even
  though reloading with the opposite endianness does not work), write the
  length byte-by-byte. */

  for (i = 0; i < 4; i++) fputc((serial_size >> (i*8)) & 255, f);
  if (fwrite(serial, 1, serial_size, f) != serial_size)
    {
    fprintf(outfile, "** Wrong return from fwrite()\n");
    fclose(f);
    return PR_ABEND;
    }

  fclose(f);
  PCRE2_SERIALIZE_FREE(serial);
  while(patstacknext > 0)
    {
    SET(compiled_code, patstack[--patstacknext]);
    SUB1(pcre2_code_free, compiled_code);
    }
  SET(compiled_code, NULL);
  break;

  /* Load a set of compiled patterns from a file onto the stack */

  case CMD_LOAD:
  rc = open_file(argptr+1, BINARY_INPUT_MODE, &f);
  if (rc != PR_OK) return rc;

  serial_size = 0;
  for (i = 0; i < 4; i++) serial_size |= fgetc(f) << (i*8);

  serial = malloc(serial_size);
  if (serial == NULL)
    {
    fprintf(outfile, "** Failed to get memory (size %lu) for #load\n",
      (unsigned long int)serial_size);
    fclose(f);
    return PR_ABEND;
    }

  i = fread(serial, 1, serial_size, f);
  fclose(f);

  if (i != serial_size)
    {
    fprintf(outfile, "** Wrong return from fread()\n");
    yield = PR_ABEND;
    }
  else
    {
    PCRE2_SERIALIZE_GET_NUMBER_OF_CODES(rc, serial);
    if (rc < 0)
      {
      if (!serial_error(rc, "Get number of codes")) yield = PR_ABEND;
      }
    else
      {
      if (rc + patstacknext > PATSTACKSIZE)
        {
        fprintf(outfile, "** Not enough space on pattern stack for %d pattern%s\n",
          rc, (rc == 1)? "" : "s");
        rc = PATSTACKSIZE - patstacknext;
        fprintf(outfile, "** Decoding %d pattern%s\n", rc,
          (rc == 1)? "" : "s");
        }
      PCRE2_SERIALIZE_DECODE(rc, patstack + patstacknext, rc, serial,
        general_context);
      if (rc < 0)
        {
        if (!serial_error(rc, "Deserialization")) yield = PR_ABEND;
        }
      else patstacknext += rc;
      }
    }

  free(serial);
  break;
  }

return yield;
}



/*************************************************
*               Process pattern line             *
*************************************************/

/* This function is called when the input buffer contains the start of a
pattern. The first character is known to be a valid delimiter. The pattern is
read, modifiers are interpreted, and a suitable local context is set up for
this test. The pattern is then compiled.

Arguments:  none

Returns:    PR_OK     continue processing next line
            PR_SKIP   skip to a blank line
            PR_ABEND  abort the pcre2test run
*/

static int
process_pattern(void)
{
BOOL utf;
uint32_t k;
uint8_t *p = buffer;
unsigned int delimiter = *p++;
int errorcode;
void *use_pat_context;
uint32_t use_forbid_utf = forbid_utf;
PCRE2_SIZE patlen;
PCRE2_SIZE valgrind_access_length;
PCRE2_SIZE erroroffset;

/* Initialize the context and pattern/data controls for this test from the
defaults. */

PATCTXCPY(pat_context, default_pat_context);
memcpy(&pat_patctl, &def_patctl, sizeof(patctl));

/* Find the end of the pattern, reading more lines if necessary. */

for(;;)
  {
  while (*p != 0)
    {
    if (*p == '\\' && p[1] != 0) p++;
      else if (*p == delimiter) break;
    p++;
    }
  if (*p != 0) break;
  if ((p = extend_inputline(infile, p, "    > ")) == NULL)
    {
    fprintf(outfile, "** Unexpected EOF\n");
    return PR_ABEND;
    }
  if (!INTERACTIVE(infile)) fprintf(outfile, "%s", (char *)p);
  }

/* If the first character after the delimiter is backslash, make the pattern
end with backslash. This is purely to provide a way of testing for the error
message when a pattern ends with backslash. */

if (p[1] == '\\') *p++ = '\\';

/* Terminate the pattern at the delimiter, and compute the length. */

*p++ = 0;
patlen = p - buffer - 2;

/* Look for modifiers and options after the final delimiter. */

if (!decode_modifiers(p, CTX_PAT, &pat_patctl, NULL)) return PR_SKIP;
utf = (pat_patctl.options & PCRE2_UTF) != 0;

/* The utf8_input modifier is not allowed in 8-bit mode, and is mutually
exclusive with the utf modifier. */

if ((pat_patctl.control & CTL_UTF8_INPUT) != 0)
  {
  if (test_mode == PCRE8_MODE)
    {
    fprintf(outfile, "** The utf8_input modifier is not allowed in 8-bit mode\n");
    return PR_SKIP;
    }
  if (utf)
    {
    fprintf(outfile, "** The utf and utf8_input modifiers are mutually exclusive\n");
    return PR_SKIP;
    }
  }

/* The convert and posix modifiers are mutually exclusive. */

if (pat_patctl.convert_type != CONVERT_UNSET &&
    (pat_patctl.control & CTL_POSIX) != 0)
  {
  fprintf(outfile, "** The convert and posix modifiers are mutually exclusive\n");
  return PR_SKIP;
  }

/* Check for mutually exclusive control modifiers. At present, these are all in
the first control word. */

for (k = 0; k < sizeof(exclusive_pat_controls)/sizeof(uint32_t); k++)
  {
  uint32_t c = pat_patctl.control & exclusive_pat_controls[k];
  if (c != 0 && c != (c & (~c+1)))
    {
    show_controls(c, 0, "** Not allowed together:");
    fprintf(outfile, "\n");
    return PR_SKIP;
    }
  }

/* Assume full JIT compile for jitverify and/or jitfast if nothing else was
specified. */

if (pat_patctl.jit == 0 &&
    (pat_patctl.control & (CTL_JITVERIFY|CTL_JITFAST)) != 0)
  pat_patctl.jit = 7;

/* Now copy the pattern to pbuffer8 for use in 8-bit testing and for reflecting
in callouts. Convert from hex if requested (literal strings in quotes may be
present within the hexadecimal pairs). The result must necessarily be fewer
characters so will always fit in pbuffer8. */

if ((pat_patctl.control & CTL_HEXPAT) != 0)
  {
  uint8_t *pp, *pt;
  uint32_t c, d;

  pt = pbuffer8;
  for (pp = buffer + 1; *pp != 0; pp++)
    {
    if (isspace(*pp)) continue;
    c = *pp++;

    /* Handle a literal substring */

    if (c == '\'' || c == '"')
      {
      uint8_t *pq = pp;
      for (;; pp++)
        {
        d = *pp;
        if (d == 0)
          {
          fprintf(outfile, "** Missing closing quote in hex pattern: "
            "opening quote is at offset " PTR_SPEC ".\n", pq - buffer - 2);
          return PR_SKIP;
          }
        if (d == c) break;
        *pt++ = d;
        }
      }

    /* Expect a hex pair */

    else
      {
      if (!isxdigit(c))
        {
        fprintf(outfile, "** Unexpected non-hex-digit '%c' at offset "
          PTR_SPEC " in hex pattern: quote missing?\n", c, pp - buffer - 2);
        return PR_SKIP;
        }
      if (*pp == 0)
        {
        fprintf(outfile, "** Odd number of digits in hex pattern\n");
        return PR_SKIP;
        }
      d = *pp;
      if (!isxdigit(d))
        {
        fprintf(outfile, "** Unexpected non-hex-digit '%c' at offset "
          PTR_SPEC " in hex pattern: quote missing?\n", d, pp - buffer - 1);
        return PR_SKIP;
        }
      c = toupper(c);
      d = toupper(d);
      *pt++ = ((isdigit(c)? (c - '0') : (c - 'A' + 10)) << 4) +
               (isdigit(d)? (d - '0') : (d - 'A' + 10));
      }
    }
  *pt = 0;
  patlen = pt - pbuffer8;
  }

/* If not a hex string, process for repetition expansion if requested. */

else if ((pat_patctl.control & CTL_EXPAND) != 0)
  {
  uint8_t *pp, *pt;

  pt = pbuffer8;
  for (pp = buffer + 1; *pp != 0; pp++)
    {
    uint8_t *pc = pp;
    uint32_t count = 1;
    size_t length = 1;

    /* Check for replication syntax; if not found, the defaults just set will
    prevail and one character will be copied. */

    if (pp[0] == '\\' && pp[1] == '[')
      {
      uint8_t *pe;
      for (pe = pp + 2; *pe != 0; pe++)
        {
        if (pe[0] == ']' && pe[1] == '{')
          {
          uint32_t clen = pe - pc - 2;
          uint32_t i = 0;
          unsigned long uli;
          char *endptr;

          pe += 2;
          uli = strtoul((const char *)pe, &endptr, 10);
          if (U32OVERFLOW(uli))
            {
            fprintf(outfile, "** Pattern repeat count too large\n");
            return PR_SKIP;
            }

          i = (uint32_t)uli;
          pe = (uint8_t *)endptr;
          if (*pe == '}')
            {
            if (i == 0)
              {
              fprintf(outfile, "** Zero repeat not allowed\n");
              return PR_SKIP;
              }
            pc += 2;
            count = i;
            length = clen;
            pp = pe;
            break;
            }
          }
        }
      }

    /* Add to output. If the buffer is too small expand it. The function for
    expanding buffers always keeps buffer and pbuffer8 in step as far as their
    size goes. */

    while (pt + count * length > pbuffer8 + pbuffer8_size)
      {
      size_t pc_offset = pc - buffer;
      size_t pp_offset = pp - buffer;
      size_t pt_offset = pt - pbuffer8;
      expand_input_buffers();
      pc = buffer + pc_offset;
      pp = buffer + pp_offset;
      pt = pbuffer8 + pt_offset;
      }

    for (; count > 0; count--)
      {
      memcpy(pt, pc, length);
      pt += length;
      }
    }

  *pt = 0;
  patlen = pt - pbuffer8;

  if ((pat_patctl.control & CTL_INFO) != 0)
    fprintf(outfile, "Expanded: %s\n", pbuffer8);
  }

/* Neither hex nor expanded, just copy the input verbatim. */

else
  {
  strncpy((char *)pbuffer8, (char *)(buffer+1), patlen + 1);
  }

/* Sort out character tables */

if (pat_patctl.locale[0] != 0)
  {
  if (pat_patctl.tables_id != 0)
    {
    fprintf(outfile, "** 'Locale' and 'tables' must not both be set\n");
    return PR_SKIP;
    }
  if (setlocale(LC_CTYPE, (const char *)pat_patctl.locale) == NULL)
    {
    fprintf(outfile, "** Failed to set locale '%s'\n", pat_patctl.locale);
    return PR_SKIP;
    }
  if (strcmp((const char *)pat_patctl.locale, (const char *)locale_name) != 0)
    {
    strcpy((char *)locale_name, (char *)pat_patctl.locale);
    if (locale_tables != NULL) free((void *)locale_tables);
    PCRE2_MAKETABLES(locale_tables);
    }
  use_tables = locale_tables;
  }

else switch (pat_patctl.tables_id)
  {
  case 0: use_tables = NULL; break;
  case 1: use_tables = tables1; break;
  case 2: use_tables = tables2; break;
  default:
  fprintf(outfile, "** 'Tables' must specify 0, 1, or 2.\n");
  return PR_SKIP;
  }

PCRE2_SET_CHARACTER_TABLES(pat_context, use_tables);

/* Set up for the stackguard test. */

if (pat_patctl.stackguard_test != 0)
  {
  PCRE2_SET_COMPILE_RECURSION_GUARD(pat_context, stack_guard, NULL);
  }

/* Handle compiling via the POSIX interface, which doesn't support the
timing, showing, or debugging options, nor the ability to pass over
local character tables. Neither does it have 16-bit or 32-bit support. */

if ((pat_patctl.control & CTL_POSIX) != 0)
  {
#ifdef SUPPORT_PCRE2_8
  int rc;
  int cflags = 0;
  const char *msg = "** Ignored with POSIX interface:";
#endif

  if (test_mode != PCRE8_MODE)
    {
    fprintf(outfile, "** The POSIX interface is available only in 8-bit mode\n");
    return PR_SKIP;
    }

#ifdef SUPPORT_PCRE2_8
  /* Check for features that the POSIX interface does not support. */

  if (pat_patctl.locale[0] != 0) prmsg(&msg, "locale");
  if (pat_patctl.replacement[0] != 0) prmsg(&msg, "replace");
  if (pat_patctl.tables_id != 0) prmsg(&msg, "tables");
  if (pat_patctl.stackguard_test != 0) prmsg(&msg, "stackguard");
  if (timeit > 0) prmsg(&msg, "timing");
  if (pat_patctl.jit != 0) prmsg(&msg, "JIT");

  if ((pat_patctl.options & ~POSIX_SUPPORTED_COMPILE_OPTIONS) != 0)
    {
    show_compile_options(
      pat_patctl.options & ~POSIX_SUPPORTED_COMPILE_OPTIONS, msg, "");
    msg = "";
    }

  if ((FLD(pat_context, extra_options) &
       ~POSIX_SUPPORTED_COMPILE_EXTRA_OPTIONS) != 0)
    {
    show_compile_extra_options(
      FLD(pat_context, extra_options) & ~POSIX_SUPPORTED_COMPILE_EXTRA_OPTIONS,
        msg, "");
    msg = "";
    }

  if ((pat_patctl.control & ~POSIX_SUPPORTED_COMPILE_CONTROLS) != 0 ||
      (pat_patctl.control2 & ~POSIX_SUPPORTED_COMPILE_CONTROLS2) != 0)
    {
    show_controls(pat_patctl.control & ~POSIX_SUPPORTED_COMPILE_CONTROLS,
      pat_patctl.control2 & ~POSIX_SUPPORTED_COMPILE_CONTROLS2, msg);
    msg = "";
    }

  if (local_newline_default != 0) prmsg(&msg, "#newline_default");
  if (FLD(pat_context, max_pattern_length) != PCRE2_UNSET)
    prmsg(&msg, "max_pattern_length");
  if (FLD(pat_context, parens_nest_limit) != PARENS_NEST_DEFAULT)
    prmsg(&msg, "parens_nest_limit");

  if (msg[0] == 0) fprintf(outfile, "\n");

  /* Translate PCRE2 options to POSIX options and then compile. */

  if (utf) cflags |= REG_UTF;
  if ((pat_patctl.control & CTL_POSIX_NOSUB) != 0) cflags |= REG_NOSUB;
  if ((pat_patctl.options & PCRE2_UCP) != 0) cflags |= REG_UCP;
  if ((pat_patctl.options & PCRE2_CASELESS) != 0) cflags |= REG_ICASE;
  if ((pat_patctl.options & PCRE2_LITERAL) != 0) cflags |= REG_NOSPEC;
  if ((pat_patctl.options & PCRE2_MULTILINE) != 0) cflags |= REG_NEWLINE;
  if ((pat_patctl.options & PCRE2_DOTALL) != 0) cflags |= REG_DOTALL;
  if ((pat_patctl.options & PCRE2_UNGREEDY) != 0) cflags |= REG_UNGREEDY;

  if ((pat_patctl.control & (CTL_HEXPAT|CTL_USE_LENGTH)) != 0)
    {
    preg.re_endp = (char *)pbuffer8 + patlen;
    cflags |= REG_PEND;
    }

  rc = regcomp(&preg, (char *)pbuffer8, cflags);

  /* Compiling failed */

  if (rc != 0)
    {
    size_t bsize, usize;
    int psize;

    preg.re_pcre2_code = NULL;     /* In case something was left in there */
    preg.re_match_data = NULL;

    bsize = (pat_patctl.regerror_buffsize != 0)?
      pat_patctl.regerror_buffsize : pbuffer8_size;
    if (bsize + 8 < pbuffer8_size)
      memcpy(pbuffer8 + bsize, "DEADBEEF", 8);
    usize = regerror(rc, &preg, (char *)pbuffer8, bsize);

    /* Inside regerror(), snprintf() is used. If the buffer is too small, some
    versions of snprintf() put a zero byte at the end, but others do not.
    Therefore, we print a maximum of one less than the size of the buffer. */

    psize = (int)bsize - 1;
    fprintf(outfile, "Failed: POSIX code %d: %.*s\n", rc, psize, pbuffer8);
    if (usize > bsize)
      {
      fprintf(outfile, "** regerror() message truncated\n");
      if (memcmp(pbuffer8 + bsize, "DEADBEEF", 8) != 0)
        fprintf(outfile, "** regerror() buffer overflow\n");
      }
    return PR_SKIP;
    }

  /* Compiling succeeded. Check that the values in the preg block are sensible.
  It can happen that pcre2test is accidentally linked with a different POSIX
  library which succeeds, but of course puts different things into preg. In
  this situation, calling regfree() may cause a segfault (or invalid free() in
  valgrind), so ensure that preg.re_pcre2_code is NULL, which suppresses the
  calling of regfree() on exit. */

  if (preg.re_pcre2_code == NULL ||
      ((pcre2_real_code_8 *)preg.re_pcre2_code)->magic_number != MAGIC_NUMBER ||
      ((pcre2_real_code_8 *)preg.re_pcre2_code)->top_bracket != preg.re_nsub ||
      preg.re_match_data == NULL ||
      preg.re_cflags != cflags)
    {
    fprintf(outfile,
      "** The regcomp() function returned zero (success), but the values set\n"
      "** in the preg block are not valid for PCRE2. Check that pcre2test is\n"
      "** linked with PCRE2's pcre2posix module (-lpcre2-posix) and not with\n"
      "** some other POSIX regex library.\n**\n");
    preg.re_pcre2_code = NULL;
    return PR_ABEND;
    }

  return PR_OK;
#endif  /* SUPPORT_PCRE2_8 */
  }

/* Handle compiling via the native interface. Controls that act later are
ignored with "push". Replacements are locked out. */

if ((pat_patctl.control & (CTL_PUSH|CTL_PUSHCOPY|CTL_PUSHTABLESCOPY)) != 0)
  {
  if (pat_patctl.replacement[0] != 0)
    {
    fprintf(outfile, "** Replacement text is not supported with 'push'.\n");
    return PR_OK;
    }
  if ((pat_patctl.control & ~PUSH_SUPPORTED_COMPILE_CONTROLS) != 0 ||
      (pat_patctl.control2 & ~PUSH_SUPPORTED_COMPILE_CONTROLS2) != 0)
    {
    show_controls(pat_patctl.control & ~PUSH_SUPPORTED_COMPILE_CONTROLS,
                  pat_patctl.control2 & ~PUSH_SUPPORTED_COMPILE_CONTROLS2,
      "** Ignored when compiled pattern is stacked with 'push':");
    fprintf(outfile, "\n");
    }
  if ((pat_patctl.control & PUSH_COMPILE_ONLY_CONTROLS) != 0 ||
      (pat_patctl.control2 & PUSH_COMPILE_ONLY_CONTROLS2) != 0)
    {
    show_controls(pat_patctl.control & PUSH_COMPILE_ONLY_CONTROLS,
                  pat_patctl.control2 & PUSH_COMPILE_ONLY_CONTROLS2,
      "** Applies only to compile when pattern is stacked with 'push':");
    fprintf(outfile, "\n");
    }
  }

/* Convert the input in non-8-bit modes. */

errorcode = 0;

#ifdef SUPPORT_PCRE2_16
if (test_mode == PCRE16_MODE) errorcode = to16(pbuffer8, utf, &patlen);
#endif

#ifdef SUPPORT_PCRE2_32
if (test_mode == PCRE32_MODE) errorcode = to32(pbuffer8, utf, &patlen);
#endif

switch(errorcode)
  {
  case -1:
  fprintf(outfile, "** Failed: invalid UTF-8 string cannot be "
    "converted to %d-bit string\n", (test_mode == PCRE16_MODE)? 16:32);
  return PR_SKIP;

  case -2:
  fprintf(outfile, "** Failed: character value greater than 0x10ffff "
    "cannot be converted to UTF\n");
  return PR_SKIP;

  case -3:
  fprintf(outfile, "** Failed: character value greater than 0xffff "
    "cannot be converted to 16-bit in non-UTF mode\n");
  return PR_SKIP;

  default:
  break;
  }

/* The pattern is now in pbuffer[8|16|32], with the length in code units in
patlen. If it is to be converted, copy the result back afterwards so that it
it ends up back in the usual place. */

if (pat_patctl.convert_type != CONVERT_UNSET)
  {
  int rc;
  int convert_return = PR_OK;
  uint32_t convert_options = pat_patctl.convert_type;
  void *converted_pattern;
  PCRE2_SIZE converted_length;

  if (pat_patctl.convert_length != 0)
    {
    converted_length = pat_patctl.convert_length;
    converted_pattern = malloc(converted_length * code_unit_size);
    if (converted_pattern == NULL)
      {
      fprintf(outfile, "** Failed: malloc failed for converted pattern\n");
      return PR_SKIP;
      }
    }
  else converted_pattern = NULL;  /* Let the library allocate */

  if (utf) convert_options |= PCRE2_CONVERT_UTF;
  if ((pat_patctl.options & PCRE2_NO_UTF_CHECK) != 0)
    convert_options |= PCRE2_CONVERT_NO_UTF_CHECK;

  CONCTXCPY(con_context, default_con_context);

  if (pat_patctl.convert_glob_escape != 0)
    {
    uint32_t escape = (pat_patctl.convert_glob_escape == '0')? 0 :
      pat_patctl.convert_glob_escape;
    PCRE2_SET_GLOB_ESCAPE(rc, con_context, escape);
    if (rc != 0)
      {
      fprintf(outfile, "** Invalid glob escape '%c'\n",
        pat_patctl.convert_glob_escape);
      convert_return = PR_SKIP;
      goto CONVERT_FINISH;
      }
    }

  if (pat_patctl.convert_glob_separator != 0)
    {
    PCRE2_SET_GLOB_SEPARATOR(rc, con_context, pat_patctl.convert_glob_separator);
    if (rc != 0)
      {
      fprintf(outfile, "** Invalid glob separator '%c'\n",
        pat_patctl.convert_glob_separator);
      convert_return = PR_SKIP;
      goto CONVERT_FINISH;
      }
    }

  PCRE2_PATTERN_CONVERT(rc, pbuffer, patlen, convert_options,
    &converted_pattern, &converted_length, con_context);

  if (rc != 0)
    {
    fprintf(outfile, "** Pattern conversion error at offset %zu: ",
      converted_length);
    convert_return = print_error_message(rc, "", "\n")? PR_SKIP:PR_ABEND;
    }

  /* Output the converted pattern, then copy it. */

  else
    {
    PCHARSV(converted_pattern, 0, converted_length, utf, outfile);
    fprintf(outfile, "\n");
    patlen = converted_length;
    CONVERT_COPY(pbuffer, converted_pattern, converted_length + 1);
    }

  /* Free the converted pattern. */

  CONVERT_FINISH:
  if (pat_patctl.convert_length != 0)
    free(converted_pattern);
  else
    PCRE2_CONVERTED_PATTERN_FREE(converted_pattern);

  /* Return if conversion was unsuccessful. */

  if (convert_return != PR_OK) return convert_return;
  }

/* By default we pass a zero-terminated pattern, but a length is passed if
"use_length" was specified or this is a hex pattern (which might contain binary
zeros). When valgrind is supported, arrange for the unused part of the buffer
to be marked as no access. */

valgrind_access_length = patlen;
if ((pat_patctl.control & (CTL_HEXPAT|CTL_USE_LENGTH)) == 0)
  {
  patlen = PCRE2_ZERO_TERMINATED;
  valgrind_access_length += 1;  /* For the terminating zero */
  }

#ifdef SUPPORT_VALGRIND
#ifdef SUPPORT_PCRE2_8
if (test_mode == PCRE8_MODE && pbuffer8 != NULL)
  {
  VALGRIND_MAKE_MEM_NOACCESS(pbuffer8 + valgrind_access_length,
    pbuffer8_size - valgrind_access_length);
  }
#endif
#ifdef SUPPORT_PCRE2_16
if (test_mode == PCRE16_MODE && pbuffer16 != NULL)
  {
  VALGRIND_MAKE_MEM_NOACCESS(pbuffer16 + valgrind_access_length,
    pbuffer16_size - valgrind_access_length*sizeof(uint16_t));
  }
#endif
#ifdef SUPPORT_PCRE2_32
if (test_mode == PCRE32_MODE && pbuffer32 != NULL)
  {
  VALGRIND_MAKE_MEM_NOACCESS(pbuffer32 + valgrind_access_length,
    pbuffer32_size - valgrind_access_length*sizeof(uint32_t));
  }
#endif
#else  /* Valgrind not supported */
(void)valgrind_access_length;  /* Avoid compiler warning */
#endif

/* If #newline_default has been used and the library was not compiled with an
appropriate default newline setting, local_newline_default will be non-zero. We
use this if there is no explicit newline modifier. */

if ((pat_patctl.control2 & CTL2_NL_SET) == 0 && local_newline_default != 0)
  {
  SETFLD(pat_context, newline_convention, local_newline_default);
  }

/* The null_context modifier is used to test calling pcre2_compile() with a
NULL context. */

use_pat_context = ((pat_patctl.control & CTL_NULLCONTEXT) != 0)?
  NULL : PTR(pat_context);

/* If PCRE2_LITERAL is set, set use_forbid_utf zero because PCRE2_NEVER_UTF
and PCRE2_NEVER_UCP are invalid with it. */

if ((pat_patctl.options & PCRE2_LITERAL) != 0) use_forbid_utf = 0;

/* Compile many times when timing. */

if (timeit > 0)
  {
  int i;
  clock_t time_taken = 0;
  for (i = 0; i < timeit; i++)
    {
    clock_t start_time = clock();
    PCRE2_COMPILE(compiled_code, pbuffer, patlen,
      pat_patctl.options|use_forbid_utf, &errorcode, &erroroffset,
        use_pat_context);
    time_taken += clock() - start_time;
    if (TEST(compiled_code, !=, NULL))
      { SUB1(pcre2_code_free, compiled_code); }
    }
  total_compile_time += time_taken;
  fprintf(outfile, "Compile time %.4f milliseconds\n",
    (((double)time_taken * 1000.0) / (double)timeit) /
      (double)CLOCKS_PER_SEC);
  }

/* A final compile that is used "for real". */

PCRE2_COMPILE(compiled_code, pbuffer, patlen, pat_patctl.options|use_forbid_utf,
  &errorcode, &erroroffset, use_pat_context);

/* Call the JIT compiler if requested. When timing, we must free and recompile
the pattern each time because that is the only way to free the JIT compiled
code. We know that compilation will always succeed. */

if (TEST(compiled_code, !=, NULL) && pat_patctl.jit != 0)
  {
  if (timeit > 0)
    {
    int i;
    clock_t time_taken = 0;
    for (i = 0; i < timeit; i++)
      {
      clock_t start_time;
      SUB1(pcre2_code_free, compiled_code);
      PCRE2_COMPILE(compiled_code, pbuffer, patlen,
        pat_patctl.options|use_forbid_utf, &errorcode, &erroroffset,
        use_pat_context);
      start_time = clock();
      PCRE2_JIT_COMPILE(jitrc,compiled_code, pat_patctl.jit);
      time_taken += clock() - start_time;
      }
    total_jit_compile_time += time_taken;
    fprintf(outfile, "JIT compile  %.4f milliseconds\n",
      (((double)time_taken * 1000.0) / (double)timeit) /
        (double)CLOCKS_PER_SEC);
    }
  else
    {
    PCRE2_JIT_COMPILE(jitrc, compiled_code, pat_patctl.jit);
    }
  }

/* If valgrind is supported, mark the pbuffer as accessible again. The 16-bit
and 32-bit buffers can be marked completely undefined, but we must leave the
pattern in the 8-bit buffer defined because it may be read from a callout
during matching. */

#ifdef SUPPORT_VALGRIND
#ifdef SUPPORT_PCRE2_8
if (test_mode == PCRE8_MODE)
  {
  VALGRIND_MAKE_MEM_UNDEFINED(pbuffer8 + valgrind_access_length,
    pbuffer8_size - valgrind_access_length);
  }
#endif
#ifdef SUPPORT_PCRE2_16
if (test_mode == PCRE16_MODE)
  {
  VALGRIND_MAKE_MEM_UNDEFINED(pbuffer16, pbuffer16_size);
  }
#endif
#ifdef SUPPORT_PCRE2_32
if (test_mode == PCRE32_MODE)
  {
  VALGRIND_MAKE_MEM_UNDEFINED(pbuffer32, pbuffer32_size);
  }
#endif
#endif

/* Compilation failed; go back for another re, skipping to blank line
if non-interactive. */

if (TEST(compiled_code, ==, NULL))
  {
  fprintf(outfile, "Failed: error %d at offset %d: ", errorcode,
    (int)erroroffset);
  if (!print_error_message(errorcode, "", "\n")) return PR_ABEND;
  return PR_SKIP;
  }

/* If forbid_utf is non-zero, we are running a non-UTF test. UTF and UCP are
locked out at compile time, but we must also check for occurrences of \P, \p,
and \X, which are only supported when Unicode is supported. */

if (forbid_utf != 0)
  {
  if ((FLD(compiled_code, flags) & PCRE2_HASBKPORX) != 0)
    {
    fprintf(outfile, "** \\P, \\p, and \\X are not allowed after the "
      "#forbid_utf command\n");
    return PR_SKIP;
    }
  }

/* Remember the maximum lookbehind, for partial matching. */

if (pattern_info(PCRE2_INFO_MAXLOOKBEHIND, &maxlookbehind, FALSE) != 0)
  return PR_ABEND;

/* If an explicit newline modifier was given, set the information flag in the
pattern so that it is preserved over push/pop. */

if ((pat_patctl.control2 & CTL2_NL_SET) != 0)
  {
  SETFLD(compiled_code, flags, FLD(compiled_code, flags) | PCRE2_NL_SET);
  }

/* Output code size and other information if requested. */

if ((pat_patctl.control & CTL_MEMORY) != 0) show_memory_info();
if ((pat_patctl.control & CTL_FRAMESIZE) != 0) show_framesize();
if ((pat_patctl.control & CTL_ANYINFO) != 0)
  {
  int rc = show_pattern_info();
  if (rc != PR_OK) return rc;
  }

/* The "push" control requests that the compiled pattern be remembered on a
stack. This is mainly for testing the serialization functionality. */

if ((pat_patctl.control & CTL_PUSH) != 0)
  {
  if (patstacknext >= PATSTACKSIZE)
    {
    fprintf(outfile, "** Too many pushed patterns (max %d)\n", PATSTACKSIZE);
    return PR_ABEND;
    }
  patstack[patstacknext++] = PTR(compiled_code);
  SET(compiled_code, NULL);
  }

/* The "pushcopy" and "pushtablescopy" controls are similar, but push a
copy of the pattern, the latter with a copy of its character tables. This tests
the pcre2_code_copy() and pcre2_code_copy_with_tables() functions. */

if ((pat_patctl.control & (CTL_PUSHCOPY|CTL_PUSHTABLESCOPY)) != 0)
  {
  if (patstacknext >= PATSTACKSIZE)
    {
    fprintf(outfile, "** Too many pushed patterns (max %d)\n", PATSTACKSIZE);
    return PR_ABEND;
    }
  if ((pat_patctl.control & CTL_PUSHCOPY) != 0)
    {
    PCRE2_CODE_COPY_TO_VOID(patstack[patstacknext++], compiled_code);
    }
  else
    {
    PCRE2_CODE_COPY_WITH_TABLES_TO_VOID(patstack[patstacknext++],
      compiled_code); }
  }

return PR_OK;
}



/*************************************************
*          Check heap, match or depth limit      *
*************************************************/

/* This is used for DFA, normal, and JIT fast matching. For DFA matching it
should only called with the third argument set to PCRE2_ERROR_DEPTHLIMIT.

Arguments:
  pp        the subject string
  ulen      length of subject or PCRE2_ZERO_TERMINATED
  errnumber defines which limit to test
  msg       string to include in final message

Returns:    the return from the final match function call
*/

static int
check_match_limit(uint8_t *pp, PCRE2_SIZE ulen, int errnumber, const char *msg)
{
int capcount;
uint32_t min = 0;
uint32_t mid = 64;
uint32_t max = UINT32_MAX;

PCRE2_SET_MATCH_LIMIT(dat_context, max);
PCRE2_SET_DEPTH_LIMIT(dat_context, max);
PCRE2_SET_HEAP_LIMIT(dat_context, max);

for (;;)
  {
  if (errnumber == PCRE2_ERROR_HEAPLIMIT)
    {
    PCRE2_SET_HEAP_LIMIT(dat_context, mid);
    }
  else if (errnumber == PCRE2_ERROR_MATCHLIMIT)
    {
    PCRE2_SET_MATCH_LIMIT(dat_context, mid);
    }
  else
    {
    PCRE2_SET_DEPTH_LIMIT(dat_context, mid);
    }

  if ((dat_datctl.control & CTL_DFA) != 0)
    {
    if (dfa_workspace == NULL)
      dfa_workspace = (int *)malloc(DFA_WS_DIMENSION*sizeof(int));
    if (dfa_matched++ == 0)
      dfa_workspace[0] = -1;  /* To catch bad restart */
    PCRE2_DFA_MATCH(capcount, compiled_code, pp, ulen, dat_datctl.offset,
      dat_datctl.options, match_data,
      PTR(dat_context), dfa_workspace, DFA_WS_DIMENSION);
    }

  else if ((pat_patctl.control & CTL_JITFAST) != 0)
    PCRE2_JIT_MATCH(capcount, compiled_code, pp, ulen, dat_datctl.offset,
      dat_datctl.options, match_data, PTR(dat_context));

  else
    PCRE2_MATCH(capcount, compiled_code, pp, ulen, dat_datctl.offset,
      dat_datctl.options, match_data, PTR(dat_context));

  if (capcount == errnumber)
    {
    min = mid;
    mid = (mid == max - 1)? max : (max != UINT32_MAX)? (min + max)/2 : mid*2;
    }
  else if (capcount >= 0 ||
           capcount == PCRE2_ERROR_NOMATCH ||
           capcount == PCRE2_ERROR_PARTIAL)
    {
    /* If we've not hit the error with a heap limit less than the size of the
    initial stack frame vector, the heap is not being used, so the minimum
    limit is zero; there's no need to go on. The other limits are always
    greater than zero. */

    if (errnumber == PCRE2_ERROR_HEAPLIMIT && mid < START_FRAMES_SIZE/1024)
      {
      fprintf(outfile, "Minimum %s limit = 0\n", msg);
      break;
      }
    if (mid == min + 1)
      {
      fprintf(outfile, "Minimum %s limit = %d\n", msg, mid);
      break;
      }
    max = mid;
    mid = (min + max)/2;
    }
  else break;    /* Some other error */
  }

return capcount;
}



/*************************************************
*              Callout function                  *
*************************************************/

/* Called from a PCRE2 library as a result of the (?C) item. We print out where
we are in the match (unless suppressed). Yield zero unless more callouts than
the fail count, or the callout data is not zero. The only differences in the
callout block for different code unit widths are that the pointers to the
subject, the most recent MARK, and a callout argument string point to strings
of the appropriate width. Casts can be used to deal with this.

Argument:  a pointer to a callout block
Return:
*/

static int
callout_function(pcre2_callout_block_8 *cb, void *callout_data_ptr)
{
uint32_t i, pre_start, post_start, subject_length;
PCRE2_SIZE current_position;
BOOL utf = (FLD(compiled_code, overall_options) & PCRE2_UTF) != 0;
BOOL callout_capture = (dat_datctl.control & CTL_CALLOUT_CAPTURE) != 0;
BOOL callout_where = (dat_datctl.control2 & CTL2_CALLOUT_NO_WHERE) == 0;

/* This FILE is used for echoing the subject. This is done only once in simple
cases. */

FILE *f = (first_callout || callout_capture || cb->callout_string != NULL)?
  outfile : NULL;

/* For a callout with a string argument, show the string first because there
isn't a tidy way to fit it in the rest of the data. */

if (cb->callout_string != NULL)
  {
  uint32_t delimiter = CODE_UNIT(cb->callout_string, -1);
  fprintf(outfile, "Callout (%lu): %c",
    (unsigned long int)cb->callout_string_offset, delimiter);
  PCHARSV(cb->callout_string, 0,
    cb->callout_string_length, utf, outfile);
  for (i = 0; callout_start_delims[i] != 0; i++)
    if (delimiter == callout_start_delims[i])
      {
      delimiter = callout_end_delims[i];
      break;
      }
  fprintf(outfile, "%c", delimiter);
  if (!callout_capture) fprintf(outfile, "\n");
  }

/* Show captured strings if required */

if (callout_capture)
  {
  if (cb->callout_string == NULL)
    fprintf(outfile, "Callout %d:", cb->callout_number);
  fprintf(outfile, " last capture = %d\n", cb->capture_last);
  for (i = 2; i < cb->capture_top * 2; i += 2)
    {
    fprintf(outfile, "%2d: ", i/2);
    if (cb->offset_vector[i] == PCRE2_UNSET)
      fprintf(outfile, "<unset>");
    else
      {
      PCHARSV(cb->subject, cb->offset_vector[i],
        cb->offset_vector[i+1] - cb->offset_vector[i], utf, f);
      }
    fprintf(outfile, "\n");
    }
  }

/* Unless suppressed, re-print the subject in canonical form (with escapes for
non-printing characters), the first time, or if giving full details. On
subsequent calls in the same match, we use PCHARS() just to find the printed
lengths of the substrings. */

if (callout_where)
  {

  if (f != NULL) fprintf(f, "--->");

  /* The subject before the match start. */

  PCHARS(pre_start, cb->subject, 0, cb->start_match, utf, f);

  /* If a lookbehind is involved, the current position may be earlier than the
  match start. If so, use the match start instead. */

  current_position = (cb->current_position >= cb->start_match)?
    cb->current_position : cb->start_match;

  /* The subject between the match start and the current position. */

  PCHARS(post_start, cb->subject, cb->start_match,
    current_position - cb->start_match, utf, f);

  /* Print from the current position to the end. */

  PCHARSV(cb->subject, current_position, cb->subject_length - current_position,
    utf, f);

  /* Calculate the total subject printed length (no print). */

  PCHARS(subject_length, cb->subject, 0, cb->subject_length, utf, NULL);

  if (f != NULL) fprintf(f, "\n");

  /* For automatic callouts, show the pattern offset. Otherwise, for a numerical
  callout whose number has not already been shown with captured strings, show the
  number here. A callout with a string argument has been displayed above. */

  if (cb->callout_number == 255)
    {
    fprintf(outfile, "%+3d ", (int)cb->pattern_position);
    if (cb->pattern_position > 99) fprintf(outfile, "\n    ");
    }
  else
    {
    if (callout_capture || cb->callout_string != NULL) fprintf(outfile, "    ");
      else fprintf(outfile, "%3d ", cb->callout_number);
    }

  /* Now show position indicators */

  for (i = 0; i < pre_start; i++) fprintf(outfile, " ");
  fprintf(outfile, "^");

  if (post_start > 0)
    {
    for (i = 0; i < post_start - 1; i++) fprintf(outfile, " ");
    fprintf(outfile, "^");
    }

  for (i = 0; i < subject_length - pre_start - post_start + 4; i++)
    fprintf(outfile, " ");

  if (cb->next_item_length != 0)
    fprintf(outfile, "%.*s", (int)(cb->next_item_length),
      pbuffer8 + cb->pattern_position);

  fprintf(outfile, "\n");
  }

first_callout = FALSE;

/* Show any mark info */

if (cb->mark != last_callout_mark)
  {
  if (cb->mark == NULL)
    fprintf(outfile, "Latest Mark: <unset>\n");
  else
    {
    fprintf(outfile, "Latest Mark: ");
    PCHARSV(cb->mark, 0, -1, utf, outfile);
    putc('\n', outfile);
    }
  last_callout_mark = cb->mark;
  }

/* Show callout data */

if (callout_data_ptr != NULL)
  {
  int callout_data = *((int32_t *)callout_data_ptr);
  if (callout_data != 0)
    {
    fprintf(outfile, "Callout data = %d\n", callout_data);
    return callout_data;
    }
  }

/* Keep count and give the appropriate return code */

callout_count++;

if (cb->callout_number == dat_datctl.cerror[0] &&
    callout_count >= dat_datctl.cerror[1])
  return PCRE2_ERROR_CALLOUT;

if (cb->callout_number == dat_datctl.cfail[0] &&
    callout_count >= dat_datctl.cfail[1])
  return 1;

return 0;
}



/*************************************************
*       Handle *MARK and copy/get tests          *
*************************************************/

/* This function is called after complete and partial matches. It runs the
tests for substring extraction.

Arguments:
  utf       TRUE for utf
  capcount  return from pcre2_match()

Returns:    FALSE if print_error_message() fails
*/

static BOOL
copy_and_get(BOOL utf, int capcount)
{
int i;
uint8_t *nptr;

/* Test copy strings by number */

for (i = 0; i < MAXCPYGET && dat_datctl.copy_numbers[i] >= 0; i++)
  {
  int rc;
  PCRE2_SIZE length, length2;
  uint32_t copybuffer[256];
  uint32_t n = (uint32_t)(dat_datctl.copy_numbers[i]);
  length = sizeof(copybuffer)/code_unit_size;
  PCRE2_SUBSTRING_COPY_BYNUMBER(rc, match_data, n, copybuffer, &length);
  if (rc < 0)
    {
    fprintf(outfile, "Copy substring %d failed (%d): ", n, rc);
    if (!print_error_message(rc, "", "\n")) return FALSE;
    }
  else
    {
    PCRE2_SUBSTRING_LENGTH_BYNUMBER(rc, match_data, n, &length2);
    if (rc < 0)
      {
      fprintf(outfile, "Get substring %d length failed (%d): ", n, rc);
      if (!print_error_message(rc, "", "\n")) return FALSE;
      }
    else if (length2 != length)
      {
      fprintf(outfile, "Mismatched substring lengths: %lu %lu\n",
        (unsigned long int)length, (unsigned long int)length2);
      }
    fprintf(outfile, "%2dC ", n);
    PCHARSV(copybuffer, 0, length, utf, outfile);
    fprintf(outfile, " (%lu)\n", (unsigned long)length);
    }
  }

/* Test copy strings by name */

nptr = dat_datctl.copy_names;
for (;;)
  {
  int rc;
  int groupnumber;
  PCRE2_SIZE length, length2;
  uint32_t copybuffer[256];
  int namelen = strlen((const char *)nptr);
#if defined SUPPORT_PCRE2_16 || defined SUPPORT_PCRE2_32
  PCRE2_SIZE cnl = namelen;
#endif
  if (namelen == 0) break;

#ifdef SUPPORT_PCRE2_8
  if (test_mode == PCRE8_MODE) strcpy((char *)pbuffer8, (char *)nptr);
#endif
#ifdef SUPPORT_PCRE2_16
  if (test_mode == PCRE16_MODE)(void)to16(nptr, utf, &cnl);
#endif
#ifdef SUPPORT_PCRE2_32
  if (test_mode == PCRE32_MODE)(void)to32(nptr, utf, &cnl);
#endif

  PCRE2_SUBSTRING_NUMBER_FROM_NAME(groupnumber, compiled_code, pbuffer);
  if (groupnumber < 0 && groupnumber != PCRE2_ERROR_NOUNIQUESUBSTRING)
    fprintf(outfile, "Number not found for group '%s'\n", nptr);

  length = sizeof(copybuffer)/code_unit_size;
  PCRE2_SUBSTRING_COPY_BYNAME(rc, match_data, pbuffer, copybuffer, &length);
  if (rc < 0)
    {
    fprintf(outfile, "Copy substring '%s' failed (%d): ", nptr, rc);
    if (!print_error_message(rc, "", "\n")) return FALSE;
    }
  else
    {
    PCRE2_SUBSTRING_LENGTH_BYNAME(rc, match_data, pbuffer, &length2);
    if (rc < 0)
      {
      fprintf(outfile, "Get substring '%s' length failed (%d): ", nptr, rc);
      if (!print_error_message(rc, "", "\n")) return FALSE;
      }
    else if (length2 != length)
      {
      fprintf(outfile, "Mismatched substring lengths: %lu %lu\n",
        (unsigned long int)length, (unsigned long int)length2);
      }
    fprintf(outfile, "  C ");
    PCHARSV(copybuffer, 0, length, utf, outfile);
    fprintf(outfile, " (%lu) %s", (unsigned long)length, nptr);
    if (groupnumber >= 0) fprintf(outfile, " (group %d)\n", groupnumber);
      else fprintf(outfile, " (non-unique)\n");
    }
  nptr += namelen + 1;
  }

/* Test get strings by number */

for (i = 0; i < MAXCPYGET && dat_datctl.get_numbers[i] >= 0; i++)
  {
  int rc;
  PCRE2_SIZE length;
  void *gotbuffer;
  uint32_t n = (uint32_t)(dat_datctl.get_numbers[i]);
  PCRE2_SUBSTRING_GET_BYNUMBER(rc, match_data, n, &gotbuffer, &length);
  if (rc < 0)
    {
    fprintf(outfile, "Get substring %d failed (%d): ", n, rc);
    if (!print_error_message(rc, "", "\n")) return FALSE;
    }
  else
    {
    fprintf(outfile, "%2dG ", n);
    PCHARSV(gotbuffer, 0, length, utf, outfile);
    fprintf(outfile, " (%lu)\n", (unsigned long)length);
    PCRE2_SUBSTRING_FREE(gotbuffer);
    }
  }

/* Test get strings by name */

nptr = dat_datctl.get_names;
for (;;)
  {
  PCRE2_SIZE length;
  void *gotbuffer;
  int rc;
  int groupnumber;
  int namelen = strlen((const char *)nptr);
#if defined SUPPORT_PCRE2_16 || defined SUPPORT_PCRE2_32
  PCRE2_SIZE cnl = namelen;
#endif
  if (namelen == 0) break;

#ifdef SUPPORT_PCRE2_8
  if (test_mode == PCRE8_MODE) strcpy((char *)pbuffer8, (char *)nptr);
#endif
#ifdef SUPPORT_PCRE2_16
  if (test_mode == PCRE16_MODE)(void)to16(nptr, utf, &cnl);
#endif
#ifdef SUPPORT_PCRE2_32
  if (test_mode == PCRE32_MODE)(void)to32(nptr, utf, &cnl);
#endif

  PCRE2_SUBSTRING_NUMBER_FROM_NAME(groupnumber, compiled_code, pbuffer);
  if (groupnumber < 0 && groupnumber != PCRE2_ERROR_NOUNIQUESUBSTRING)
    fprintf(outfile, "Number not found for group '%s'\n", nptr);

  PCRE2_SUBSTRING_GET_BYNAME(rc, match_data, pbuffer, &gotbuffer, &length);
  if (rc < 0)
    {
    fprintf(outfile, "Get substring '%s' failed (%d): ", nptr, rc);
    if (!print_error_message(rc, "", "\n")) return FALSE;
    }
  else
    {
    fprintf(outfile, "  G ");
    PCHARSV(gotbuffer, 0, length, utf, outfile);
    fprintf(outfile, " (%lu) %s", (unsigned long)length, nptr);
    if (groupnumber >= 0) fprintf(outfile, " (group %d)\n", groupnumber);
      else fprintf(outfile, " (non-unique)\n");
    PCRE2_SUBSTRING_FREE(gotbuffer);
    }
  nptr += namelen + 1;
  }

/* Test getting the complete list of captured strings. */

if ((dat_datctl.control & CTL_GETALL) != 0)
  {
  int rc;
  void **stringlist;
  PCRE2_SIZE *lengths;
  PCRE2_SUBSTRING_LIST_GET(rc, match_data, &stringlist, &lengths);
  if (rc < 0)
    {
    fprintf(outfile, "get substring list failed (%d): ", rc);
    if (!print_error_message(rc, "", "\n")) return FALSE;
    }
  else
    {
    for (i = 0; i < capcount; i++)
      {
      fprintf(outfile, "%2dL ", i);
      PCHARSV(stringlist[i], 0, lengths[i], utf, outfile);
      putc('\n', outfile);
      }
    if (stringlist[i] != NULL)
      fprintf(outfile, "string list not terminated by NULL\n");
    PCRE2_SUBSTRING_LIST_FREE(stringlist);
    }
  }

return TRUE;
}



/*************************************************
*               Process a data line              *
*************************************************/

/* The line is in buffer; it will not be empty.

Arguments:  none

Returns:    PR_OK     continue processing next line
            PR_SKIP   skip to a blank line
            PR_ABEND  abort the pcre2test run
*/

static int
process_data(void)
{
PCRE2_SIZE len, ulen, arg_ulen;
uint32_t gmatched;
uint32_t c, k;
uint32_t g_notempty = 0;
uint8_t *p, *pp, *start_rep;
size_t needlen;
void *use_dat_context;
BOOL utf;
BOOL subject_literal;

#ifdef SUPPORT_PCRE2_8
uint8_t *q8 = NULL;
#endif
#ifdef SUPPORT_PCRE2_16
uint16_t *q16 = NULL;
#endif
#ifdef SUPPORT_PCRE2_32
uint32_t *q32 = NULL;
#endif

subject_literal = (pat_patctl.control2 & CTL2_SUBJECT_LITERAL) != 0;

/* Copy the default context and data control blocks to the active ones. Then
copy from the pattern the controls that can be set in either the pattern or the
data. This allows them to be overridden in the data line. We do not do this for
options because those that are common apply separately to compiling and
matching. */

DATCTXCPY(dat_context, default_dat_context);
memcpy(&dat_datctl, &def_datctl, sizeof(datctl));
dat_datctl.control |= (pat_patctl.control & CTL_ALLPD);
dat_datctl.control2 |= (pat_patctl.control2 & CTL2_ALLPD);
strcpy((char *)dat_datctl.replacement, (char *)pat_patctl.replacement);
if (dat_datctl.jitstack == 0) dat_datctl.jitstack = pat_patctl.jitstack;

/* Initialize for scanning the data line. */

#ifdef SUPPORT_PCRE2_8
utf = ((((pat_patctl.control & CTL_POSIX) != 0)?
  ((pcre2_real_code_8 *)preg.re_pcre2_code)->overall_options :
  FLD(compiled_code, overall_options)) & PCRE2_UTF) != 0;
#else
utf = (FLD(compiled_code, overall_options) & PCRE2_UTF) != 0;
#endif

start_rep = NULL;
len = strlen((const char *)buffer);
while (len > 0 && isspace(buffer[len-1])) len--;
buffer[len] = 0;
p = buffer;
while (isspace(*p)) p++;

/* Check that the data is well-formed UTF-8 if we're in UTF mode. To create
invalid input to pcre2_match(), you must use \x?? or \x{} sequences. */

if (utf)
  {
  uint8_t *q;
  uint32_t cc;
  int n = 1;
  for (q = p; n > 0 && *q; q += n) n = utf82ord(q, &cc);
  if (n <= 0)
    {
    fprintf(outfile, "** Failed: invalid UTF-8 string cannot be used as input "
      "in UTF mode\n");
    return PR_OK;
    }
  }

#ifdef SUPPORT_VALGRIND
/* Mark the dbuffer as addressable but undefined again. */
if (dbuffer != NULL)
  {
  VALGRIND_MAKE_MEM_UNDEFINED(dbuffer, dbuffer_size);
  }
#endif

/* Allocate a buffer to hold the data line; len+1 is an upper bound on
the number of code units that will be needed (though the buffer may have to be
extended if replication is involved). */

needlen = (size_t)((len+1) * code_unit_size);
if (dbuffer == NULL || needlen >= dbuffer_size)
  {
  while (needlen >= dbuffer_size) dbuffer_size *= 2;
  dbuffer = (uint8_t *)realloc(dbuffer, dbuffer_size);
  if (dbuffer == NULL)
    {
    fprintf(stderr, "pcre2test: realloc(%d) failed\n", (int)dbuffer_size);
    exit(1);
    }
  }
SETCASTPTR(q, dbuffer);  /* Sets q8, q16, or q32, as appropriate. */

/* Scan the data line, interpreting data escapes, and put the result into a
buffer of the appropriate width. In UTF mode, input is always UTF-8; otherwise,
in 16- and 32-bit modes, it can be forced to UTF-8 by the utf8_input modifier.
*/

while ((c = *p++) != 0)
  {
  int32_t i = 0;
  size_t replen;

  /* ] may mark the end of a replicated sequence */

  if (c == ']' && start_rep != NULL)
    {
    long li;
    char *endptr;
    size_t qoffset = CAST8VAR(q) - dbuffer;
    size_t rep_offset = start_rep - dbuffer;

    if (*p++ != '{')
      {
      fprintf(outfile, "** Expected '{' after \\[....]\n");
      return PR_OK;
      }

    li = strtol((const char *)p, &endptr, 10);
    if (S32OVERFLOW(li))
      {
      fprintf(outfile, "** Repeat count too large\n");
      return PR_OK;
      }

    p = (uint8_t *)endptr;
    if (*p++ != '}')
      {
      fprintf(outfile, "** Expected '}' after \\[...]{...\n");
      return PR_OK;
      }

    i = (int32_t)li;
    if (i-- == 0)
      {
      fprintf(outfile, "** Zero repeat not allowed\n");
      return PR_OK;
      }

    replen = CAST8VAR(q) - start_rep;
    needlen += replen * i;

    if (needlen >= dbuffer_size)
      {
      while (needlen >= dbuffer_size) dbuffer_size *= 2;
      dbuffer = (uint8_t *)realloc(dbuffer, dbuffer_size);
      if (dbuffer == NULL)
        {
        fprintf(stderr, "pcre2test: realloc(%d) failed\n", (int)dbuffer_size);
        exit(1);
        }
      SETCASTPTR(q, dbuffer + qoffset);
      start_rep = dbuffer + rep_offset;
      }

    while (i-- > 0)
      {
      memcpy(CAST8VAR(q), start_rep, replen);
      SETPLUS(q, replen/code_unit_size);
      }

    start_rep = NULL;
    continue;
    }

  /* Handle a non-escaped character. In non-UTF 32-bit mode with utf8_input
  set, do the fudge for setting the top bit. */

  if (c != '\\' || subject_literal)
    {
    uint32_t topbit = 0;
    if (test_mode == PCRE32_MODE && c == 0xff && *p != 0)
      {
      topbit = 0x80000000;
      c = *p++;
      }
    if ((utf || (pat_patctl.control & CTL_UTF8_INPUT) != 0) &&
      HASUTF8EXTRALEN(c)) { GETUTF8INC(c, p); }
    c |= topbit;
    }

  /* Handle backslash escapes */

  else switch ((c = *p++))
    {
    case '\\': break;
    case 'a': c = CHAR_BEL; break;
    case 'b': c = '\b'; break;
    case 'e': c = CHAR_ESC; break;
    case 'f': c = '\f'; break;
    case 'n': c = '\n'; break;
    case 'r': c = '\r'; break;
    case 't': c = '\t'; break;
    case 'v': c = '\v'; break;

    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    c -= '0';
    while (i++ < 2 && isdigit(*p) && *p != '8' && *p != '9')
      c = c * 8 + *p++ - '0';
    break;

    case 'o':
    if (*p == '{')
      {
      uint8_t *pt = p;
      c = 0;
      for (pt++; isdigit(*pt) && *pt != '8' && *pt != '9'; pt++)
        {
        if (++i == 12)
          fprintf(outfile, "** Too many octal digits in \\o{...} item; "
                           "using only the first twelve.\n");
        else c = c * 8 + *pt - '0';
        }
      if (*pt == '}') p = pt + 1;
        else fprintf(outfile, "** Missing } after \\o{ (assumed)\n");
      }
    break;

    case 'x':
    if (*p == '{')
      {
      uint8_t *pt = p;
      c = 0;

      /* We used to have "while (isxdigit(*(++pt)))" here, but it fails
      when isxdigit() is a macro that refers to its argument more than
      once. This is banned by the C Standard, but apparently happens in at
      least one MacOS environment. */

      for (pt++; isxdigit(*pt); pt++)
        {
        if (++i == 9)
          fprintf(outfile, "** Too many hex digits in \\x{...} item; "
                           "using only the first eight.\n");
        else c = c * 16 + tolower(*pt) - ((isdigit(*pt))? '0' : 'a' - 10);
        }
      if (*pt == '}')
        {
        p = pt + 1;
        break;
        }
      /* Not correct form for \x{...}; fall through */
      }

    /* \x without {} always defines just one byte in 8-bit mode. This
    allows UTF-8 characters to be constructed byte by byte, and also allows
    invalid UTF-8 sequences to be made. Just copy the byte in UTF-8 mode.
    Otherwise, pass it down as data. */

    c = 0;
    while (i++ < 2 && isxdigit(*p))
      {
      c = c * 16 + tolower(*p) - ((isdigit(*p))? '0' : 'a' - 10);
      p++;
      }
#if defined SUPPORT_PCRE2_8
    if (utf && (test_mode == PCRE8_MODE))
      {
      *q8++ = c;
      continue;
      }
#endif
    break;

    case 0:     /* \ followed by EOF allows for an empty line */
    p--;
    continue;

    case '=':   /* \= terminates the data, starts modifiers */
    goto ENDSTRING;

    case '[':   /* \[ introduces a replicated character sequence */
    if (start_rep != NULL)
      {
      fprintf(outfile, "** Nested replication is not supported\n");
      return PR_OK;
      }
    start_rep = CAST8VAR(q);
    continue;

    default:
    if (isalnum(c))
      {
      fprintf(outfile, "** Unrecognized escape sequence \"\\%c\"\n", c);
      return PR_OK;
      }
    }

  /* We now have a character value in c that may be greater than 255.
  In 8-bit mode we convert to UTF-8 if we are in UTF mode. Values greater
  than 127 in UTF mode must have come from \x{...} or octal constructs
  because values from \x.. get this far only in non-UTF mode. */

#ifdef SUPPORT_PCRE2_8
  if (test_mode == PCRE8_MODE)
    {
    if (utf)
      {
      if (c > 0x7fffffff)
        {
        fprintf(outfile, "** Character \\x{%x} is greater than 0x7fffffff "
          "and so cannot be converted to UTF-8\n", c);
        return PR_OK;
        }
      q8 += ord2utf8(c, q8);
      }
    else
      {
      if (c > 0xffu)
        {
        fprintf(outfile, "** Character \\x{%x} is greater than 255 "
          "and UTF-8 mode is not enabled.\n", c);
        fprintf(outfile, "** Truncation will probably give the wrong "
          "result.\n");
        }
      *q8++ = c;
      }
    }
#endif
#ifdef SUPPORT_PCRE2_16
  if (test_mode == PCRE16_MODE)
    {
    if (utf)
      {
      if (c > 0x10ffffu)
        {
        fprintf(outfile, "** Failed: character \\x{%x} is greater than "
          "0x10ffff and so cannot be converted to UTF-16\n", c);
        return PR_OK;
        }
      else if (c >= 0x10000u)
        {
        c-= 0x10000u;
        *q16++ = 0xD800 | (c >> 10);
        *q16++ = 0xDC00 | (c & 0x3ff);
        }
      else
        *q16++ = c;
      }
    else
      {
      if (c > 0xffffu)
        {
        fprintf(outfile, "** Character \\x{%x} is greater than 0xffff "
          "and UTF-16 mode is not enabled.\n", c);
        fprintf(outfile, "** Truncation will probably give the wrong "
          "result.\n");
        }

      *q16++ = c;
      }
    }
#endif
#ifdef SUPPORT_PCRE2_32
  if (test_mode == PCRE32_MODE)
    {
    *q32++ = c;
    }
#endif
  }

ENDSTRING:
SET(*q, 0);
len = CASTVAR(uint8_t *, q) - dbuffer;    /* Length in bytes */
ulen = len/code_unit_size;                /* Length in code units */
arg_ulen = ulen;                          /* Value to use in match arg */

/* If the string was terminated by \= we must now interpret modifiers. */

if (p[-1] != 0 && !decode_modifiers(p, CTX_DAT, NULL, &dat_datctl))
  return PR_OK;

/* Check for mutually exclusive modifiers. At present, these are all in the
first control word. */

for (k = 0; k < sizeof(exclusive_dat_controls)/sizeof(uint32_t); k++)
  {
  c = dat_datctl.control & exclusive_dat_controls[k];
  if (c != 0 && c != (c & (~c+1)))
    {
    show_controls(c, 0, "** Not allowed together:");
    fprintf(outfile, "\n");
    return PR_OK;
    }
  }

if (pat_patctl.replacement[0] != 0 &&
    (dat_datctl.control & CTL_NULLCONTEXT) != 0)
  {
  fprintf(outfile, "** Replacement text is not supported with null_context.\n");
  return PR_OK;
  }

/* We now have the subject in dbuffer, with len containing the byte length, and
ulen containing the code unit length, with a copy in arg_ulen for use in match
function arguments (this gets changed to PCRE2_ZERO_TERMINATED when the
zero_terminate modifier is present).

Move the data to the end of the buffer so that a read over the end can be
caught by valgrind or other means. If we have explicit valgrind support, mark
the unused start of the buffer unaddressable. If we are using the POSIX
interface, or testing zero-termination, we must include the terminating zero in
the usable data. */

c = code_unit_size * (((pat_patctl.control & CTL_POSIX) +
                       (dat_datctl.control & CTL_ZERO_TERMINATE) != 0)? 1:0);
pp = memmove(dbuffer + dbuffer_size - len - c, dbuffer, len + c);
#ifdef SUPPORT_VALGRIND
  VALGRIND_MAKE_MEM_NOACCESS(dbuffer, dbuffer_size - (len + c));
#endif

/* Now pp points to the subject string. POSIX matching is only possible in
8-bit mode, and it does not support timing or other fancy features. Some were
checked at compile time, but we need to check the match-time settings here. */

#ifdef SUPPORT_PCRE2_8
if ((pat_patctl.control & CTL_POSIX) != 0)
  {
  int rc;
  int eflags = 0;
  regmatch_t *pmatch = NULL;
  const char *msg = "** Ignored with POSIX interface:";

  if (dat_datctl.cerror[0] != CFORE_UNSET || dat_datctl.cerror[1] != CFORE_UNSET)
    prmsg(&msg, "callout_error");
  if (dat_datctl.cfail[0] != CFORE_UNSET || dat_datctl.cfail[1] != CFORE_UNSET)
    prmsg(&msg, "callout_fail");
  if (dat_datctl.copy_numbers[0] >= 0 || dat_datctl.copy_names[0] != 0)
    prmsg(&msg, "copy");
  if (dat_datctl.get_numbers[0] >= 0 || dat_datctl.get_names[0] != 0)
    prmsg(&msg, "get");
  if (dat_datctl.jitstack != 0) prmsg(&msg, "jitstack");
  if (dat_datctl.offset != 0) prmsg(&msg, "offset");

  if ((dat_datctl.options & ~POSIX_SUPPORTED_MATCH_OPTIONS) != 0)
    {
    fprintf(outfile, "%s", msg);
    show_match_options(dat_datctl.options & ~POSIX_SUPPORTED_MATCH_OPTIONS);
    msg = "";
    }
  if ((dat_datctl.control & ~POSIX_SUPPORTED_MATCH_CONTROLS) != 0 ||
      (dat_datctl.control2 & ~POSIX_SUPPORTED_MATCH_CONTROLS2) != 0)
    {
    show_controls(dat_datctl.control & ~POSIX_SUPPORTED_MATCH_CONTROLS,
                  dat_datctl.control2 & ~POSIX_SUPPORTED_MATCH_CONTROLS2, msg);
    msg = "";
    }

  if (msg[0] == 0) fprintf(outfile, "\n");

  if (dat_datctl.oveccount > 0)
    {
    pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * dat_datctl.oveccount);
    if (pmatch == NULL)
      {
      fprintf(outfile, "** Failed to get memory for recording matching "
        "information (size set = %du)\n", dat_datctl.oveccount);
      return PR_OK;
      }
    }

  if (dat_datctl.startend[0] != CFORE_UNSET)
    {
    pmatch[0].rm_so = dat_datctl.startend[0];
    pmatch[0].rm_eo = (dat_datctl.startend[1] != 0)?
      dat_datctl.startend[1] : len;
    eflags |= REG_STARTEND;
    }

  if ((dat_datctl.options & PCRE2_NOTBOL) != 0) eflags |= REG_NOTBOL;
  if ((dat_datctl.options & PCRE2_NOTEOL) != 0) eflags |= REG_NOTEOL;
  if ((dat_datctl.options & PCRE2_NOTEMPTY) != 0) eflags |= REG_NOTEMPTY;

  rc = regexec(&preg, (const char *)pp, dat_datctl.oveccount, pmatch, eflags);
  if (rc != 0)
    {
    (void)regerror(rc, &preg, (char *)pbuffer8, pbuffer8_size);
    fprintf(outfile, "No match: POSIX code %d: %s\n", rc, pbuffer8);
    }
  else if ((pat_patctl.control & CTL_POSIX_NOSUB) != 0)
    fprintf(outfile, "Matched with REG_NOSUB\n");
  else if (dat_datctl.oveccount == 0)
    fprintf(outfile, "Matched without capture\n");
  else
    {
    size_t i;
    for (i = 0; i < (size_t)dat_datctl.oveccount; i++)
      {
      if (pmatch[i].rm_so >= 0)
        {
        PCRE2_SIZE start = pmatch[i].rm_so;
        PCRE2_SIZE end = pmatch[i].rm_eo;
        if (start > end)
          {
          start = pmatch[i].rm_eo;
          end = pmatch[i].rm_so;
          fprintf(outfile, "Start of matched string is beyond its end - "
            "displaying from end to start.\n");
          }
        fprintf(outfile, "%2d: ", (int)i);
        PCHARSV(pp, start, end - start, utf, outfile);
        fprintf(outfile, "\n");

        if ((i == 0 && (dat_datctl.control & CTL_AFTERTEXT) != 0) ||
            (dat_datctl.control & CTL_ALLAFTERTEXT) != 0)
          {
          fprintf(outfile, "%2d+ ", (int)i);
          /* Note: don't use the start/end variables here because we want to
          show the text from what is reported as the end. */
          PCHARSV(pp, pmatch[i].rm_eo, len - pmatch[i].rm_eo, utf, outfile);
          fprintf(outfile, "\n"); }
        }
      }
    }
  free(pmatch);
  return PR_OK;
  }
#endif  /* SUPPORT_PCRE2_8 */

 /* Handle matching via the native interface. Check for consistency of
modifiers. */

if (dat_datctl.startend[0] != CFORE_UNSET)
  fprintf(outfile, "** \\=posix_startend ignored for non-POSIX matching\n");

/* ALLUSEDTEXT is not supported with JIT, but JIT is not used with DFA
matching, even if the JIT compiler was used. */

if ((dat_datctl.control & (CTL_ALLUSEDTEXT|CTL_DFA)) == CTL_ALLUSEDTEXT &&
    FLD(compiled_code, executable_jit) != NULL)
  {
  fprintf(outfile, "** Showing all consulted text is not supported by JIT: ignored\n");
  dat_datctl.control &= ~CTL_ALLUSEDTEXT;
  }

/* Handle passing the subject as zero-terminated. */

if ((dat_datctl.control & CTL_ZERO_TERMINATE) != 0)
  arg_ulen = PCRE2_ZERO_TERMINATED;

/* The nullcontext modifier is used to test calling pcre2_[jit_]match() with a
NULL context. */

use_dat_context = ((dat_datctl.control & CTL_NULLCONTEXT) != 0)?
  NULL : PTR(dat_context);

/* Enable display of malloc/free if wanted. We can do this only if either the
pattern or the subject is processed with a context. */

show_memory = (dat_datctl.control & CTL_MEMORY) != 0;

if (show_memory &&
    (pat_patctl.control & dat_datctl.control & CTL_NULLCONTEXT) != 0)
  fprintf(outfile, "** \\=memory requires either a pattern or a subject "
    "context: ignored\n");

/* Create and assign a JIT stack if requested. */

if (dat_datctl.jitstack != 0)
  {
  if (dat_datctl.jitstack != jit_stack_size)
    {
    PCRE2_JIT_STACK_FREE(jit_stack);
    PCRE2_JIT_STACK_CREATE(jit_stack, 1, dat_datctl.jitstack * 1024, NULL);
    jit_stack_size = dat_datctl.jitstack;
    }
  PCRE2_JIT_STACK_ASSIGN(dat_context, jit_callback, jit_stack);
  }

/* Or de-assign */

else if (jit_stack != NULL)
  {
  PCRE2_JIT_STACK_ASSIGN(dat_context, NULL, NULL);
  PCRE2_JIT_STACK_FREE(jit_stack);
  jit_stack = NULL;
  jit_stack_size = 0;
  }

/* When no JIT stack is assigned, we must ensure that there is a JIT callback
if we want to verify that JIT was actually used. */

if ((pat_patctl.control & CTL_JITVERIFY) != 0 && jit_stack == NULL)
   {
   PCRE2_JIT_STACK_ASSIGN(dat_context, jit_callback, NULL);
   }

/* Adjust match_data according to size of offsets required. A size of zero
causes a new match data block to be obtained that exactly fits the pattern. */

if (dat_datctl.oveccount == 0)
  {
  PCRE2_MATCH_DATA_FREE(match_data);
  PCRE2_MATCH_DATA_CREATE_FROM_PATTERN(match_data, compiled_code, NULL);
  PCRE2_GET_OVECTOR_COUNT(max_oveccount, match_data);
  }
else if (dat_datctl.oveccount <= max_oveccount)
  {
  SETFLD(match_data, oveccount, dat_datctl.oveccount);
  }
else
  {
  max_oveccount = dat_datctl.oveccount;
  PCRE2_MATCH_DATA_FREE(match_data);
  PCRE2_MATCH_DATA_CREATE(match_data, max_oveccount, NULL);
  }

if (CASTVAR(void *, match_data) == NULL)
  {
  fprintf(outfile, "** Failed to get memory for recording matching "
    "information (size requested: %d)\n", dat_datctl.oveccount);
  max_oveccount = 0;
  return PR_OK;
  }

/* Replacement processing is ignored for DFA matching. */

if (dat_datctl.replacement[0] != 0 && (dat_datctl.control & CTL_DFA) != 0)
  {
  fprintf(outfile, "** Ignored for DFA matching: replace\n");
  dat_datctl.replacement[0] = 0;
  }

/* If a replacement string is provided, call pcre2_substitute() instead of one
of the matching functions. First we have to convert the replacement string to
the appropriate width. */

if (dat_datctl.replacement[0] != 0)
  {
  int rc;
  uint8_t *pr;
  uint8_t rbuffer[REPLACE_BUFFSIZE];
  uint8_t nbuffer[REPLACE_BUFFSIZE];
  uint32_t xoptions;
  PCRE2_SIZE rlen, nsize, erroroffset;
  BOOL badutf = FALSE;

#ifdef SUPPORT_PCRE2_8
  uint8_t *r8 = NULL;
#endif
#ifdef SUPPORT_PCRE2_16
  uint16_t *r16 = NULL;
#endif
#ifdef SUPPORT_PCRE2_32
  uint32_t *r32 = NULL;
#endif

  if (timeitm)
    fprintf(outfile, "** Timing is not supported with replace: ignored\n");

  xoptions = (((dat_datctl.control & CTL_GLOBAL) == 0)? 0 :
                PCRE2_SUBSTITUTE_GLOBAL) |
             (((dat_datctl.control2 & CTL2_SUBSTITUTE_EXTENDED) == 0)? 0 :
                PCRE2_SUBSTITUTE_EXTENDED) |
             (((dat_datctl.control2 & CTL2_SUBSTITUTE_OVERFLOW_LENGTH) == 0)? 0 :
                PCRE2_SUBSTITUTE_OVERFLOW_LENGTH) |
             (((dat_datctl.control2 & CTL2_SUBSTITUTE_UNKNOWN_UNSET) == 0)? 0 :
                PCRE2_SUBSTITUTE_UNKNOWN_UNSET) |
             (((dat_datctl.control2 & CTL2_SUBSTITUTE_UNSET_EMPTY) == 0)? 0 :
                PCRE2_SUBSTITUTE_UNSET_EMPTY);

  SETCASTPTR(r, rbuffer);  /* Sets r8, r16, or r32, as appropriate. */
  pr = dat_datctl.replacement;

  /* If the replacement starts with '[<number>]' we interpret that as length
  value for the replacement buffer. */

  nsize = REPLACE_BUFFSIZE/code_unit_size;
  if (*pr == '[')
    {
    PCRE2_SIZE n = 0;
    while ((c = *(++pr)) >= CHAR_0 && c <= CHAR_9) n = n * 10 + c - CHAR_0;
    if (*pr++ != ']')
      {
      fprintf(outfile, "Bad buffer size in replacement string\n");
      return PR_OK;
      }
    if (n > nsize)
      {
      fprintf(outfile, "Replacement buffer setting (%lu) is too large "
        "(max %lu)\n", (unsigned long int)n, (unsigned long int)nsize);
      return PR_OK;
      }
    nsize = n;
    }

  /* Now copy the replacement string to a buffer of the appropriate width. No
  escape processing is done for replacements. In UTF mode, check for an invalid
  UTF-8 input string, and if it is invalid, just copy its code units without
  UTF interpretation. This provides a means of checking that an invalid string
  is detected. Otherwise, UTF-8 can be used to include wide characters in a
  replacement. */

  if (utf) badutf = valid_utf(pr, strlen((const char *)pr), &erroroffset);

  /* Not UTF or invalid UTF-8: just copy the code units. */

  if (!utf || badutf)
    {
    while ((c = *pr++) != 0)
      {
#ifdef SUPPORT_PCRE2_8
      if (test_mode == PCRE8_MODE) *r8++ = c;
#endif
#ifdef SUPPORT_PCRE2_16
      if (test_mode == PCRE16_MODE) *r16++ = c;
#endif
#ifdef SUPPORT_PCRE2_32
      if (test_mode == PCRE32_MODE) *r32++ = c;
#endif
      }
    }

  /* Valid UTF-8 replacement string */

  else while ((c = *pr++) != 0)
    {
    if (HASUTF8EXTRALEN(c)) { GETUTF8INC(c, pr); }

#ifdef SUPPORT_PCRE2_8
    if (test_mode == PCRE8_MODE) r8 += ord2utf8(c, r8);
#endif

#ifdef SUPPORT_PCRE2_16
    if (test_mode == PCRE16_MODE)
      {
      if (c >= 0x10000u)
        {
        c-= 0x10000u;
        *r16++ = 0xD800 | (c >> 10);
        *r16++ = 0xDC00 | (c & 0x3ff);
        }
      else *r16++ = c;
      }
#endif

#ifdef SUPPORT_PCRE2_32
    if (test_mode == PCRE32_MODE) *r32++ = c;
#endif
    }

  SET(*r, 0);
  if ((dat_datctl.control & CTL_ZERO_TERMINATE) != 0)
    rlen = PCRE2_ZERO_TERMINATED;
  else
    rlen = (CASTVAR(uint8_t *, r) - rbuffer)/code_unit_size;
  PCRE2_SUBSTITUTE(rc, compiled_code, pp, arg_ulen, dat_datctl.offset,
    dat_datctl.options|xoptions, match_data, dat_context,
    rbuffer, rlen, nbuffer, &nsize);

  if (rc < 0)
    {
    fprintf(outfile, "Failed: error %d", rc);
    if (rc != PCRE2_ERROR_NOMEMORY && nsize != PCRE2_UNSET)
      fprintf(outfile, " at offset %ld in replacement", (long int)nsize);
    fprintf(outfile, ": ");
    if (!print_error_message(rc, "", "")) return PR_ABEND;
    if (rc == PCRE2_ERROR_NOMEMORY &&
        (xoptions & PCRE2_SUBSTITUTE_OVERFLOW_LENGTH) != 0)
      fprintf(outfile, ": %ld code units are needed", (long int)nsize);
    }
  else
    {
    fprintf(outfile, "%2d: ", rc);
    PCHARSV(nbuffer, 0, nsize, utf, outfile);
    }

  fprintf(outfile, "\n");
  }   /* End of substitution handling */

/* When a replacement string is not provided, run a loop for global matching
with one of the basic matching functions. */

else for (gmatched = 0;; gmatched++)
  {
  PCRE2_SIZE j;
  int capcount;
  PCRE2_SIZE *ovector;
  PCRE2_SIZE ovecsave[2];

  ovector = FLD(match_data, ovector);

  /* After the first time round a global loop, for a normal global (/g)
  iteration, save the current ovector[0,1] so that we can check that they do
  change each time. Otherwise a matching bug that returns the same string
  causes an infinite loop. It has happened! */

  if (gmatched > 0 && (dat_datctl.control & CTL_GLOBAL) != 0)
    {
    ovecsave[0] = ovector[0];
    ovecsave[1] = ovector[1];
    }

  /* For altglobal (or first time round the loop), set an "unset" value. */

  else ovecsave[0] = ovecsave[1] = PCRE2_UNSET;

  /* Fill the ovector with junk to detect elements that do not get set
  when they should be. */

  for (j = 0; j < 2*dat_datctl.oveccount; j++) ovector[j] = JUNK_OFFSET;

  /* When matching is via pcre2_match(), we will detect the use of JIT via the
  stack callback function. */

  jit_was_used = (pat_patctl.control & CTL_JITFAST) != 0;

  /* Do timing if required. */

  if (timeitm > 0)
    {
    int i;
    clock_t start_time, time_taken;

    if ((dat_datctl.control & CTL_DFA) != 0)
      {
      if ((dat_datctl.options & PCRE2_DFA_RESTART) != 0)
        {
        fprintf(outfile, "Timing DFA restarts is not supported\n");
        return PR_OK;
        }
      if (dfa_workspace == NULL)
        dfa_workspace = (int *)malloc(DFA_WS_DIMENSION*sizeof(int));
      start_time = clock();
      for (i = 0; i < timeitm; i++)
        {
        PCRE2_DFA_MATCH(capcount, compiled_code, pp, arg_ulen,
          dat_datctl.offset, dat_datctl.options | g_notempty, match_data,
          use_dat_context, dfa_workspace, DFA_WS_DIMENSION);
        }
      }

    else if ((pat_patctl.control & CTL_JITFAST) != 0)
      {
      start_time = clock();
      for (i = 0; i < timeitm; i++)
        {
        PCRE2_JIT_MATCH(capcount, compiled_code, pp, arg_ulen,
          dat_datctl.offset, dat_datctl.options | g_notempty, match_data,
          use_dat_context);
        }
      }

    else
      {
      start_time = clock();
      for (i = 0; i < timeitm; i++)
        {
        PCRE2_MATCH(capcount, compiled_code, pp, arg_ulen,
          dat_datctl.offset, dat_datctl.options | g_notempty, match_data,
          use_dat_context);
        }
      }
    total_match_time += (time_taken = clock() - start_time);
    fprintf(outfile, "Match time %.4f milliseconds\n",
      (((double)time_taken * 1000.0) / (double)timeitm) /
        (double)CLOCKS_PER_SEC);
    }

  /* Find the heap, match and depth limits if requested. The match and heap
  limits are not relevant for DFA matching and the depth and heap limits are
  not relevant for JIT. The return from check_match_limit() is the return from
  the final call to pcre2_match() or pcre2_dfa_match(). */

  if ((dat_datctl.control & CTL_FINDLIMITS) != 0)
    {
    capcount = 0;  /* This stops compiler warnings */

    if ((dat_datctl.control & CTL_DFA) == 0 &&
        (FLD(compiled_code, executable_jit) == NULL ||
          (dat_datctl.options & PCRE2_NO_JIT) != 0))
      {
      (void)check_match_limit(pp, arg_ulen, PCRE2_ERROR_HEAPLIMIT, "heap");
      }

    capcount = check_match_limit(pp, arg_ulen, PCRE2_ERROR_MATCHLIMIT,
      "match");

    if (FLD(compiled_code, executable_jit) == NULL ||
        (dat_datctl.options & PCRE2_NO_JIT) != 0 ||
        (dat_datctl.control & CTL_DFA) != 0)
      {
      capcount = check_match_limit(pp, arg_ulen, PCRE2_ERROR_DEPTHLIMIT,
        "depth");
      }
    }

  /* Otherwise just run a single match, setting up a callout if required (the
  default). There is a copy of the pattern in pbuffer8 for use by callouts. */

  else
    {
    if ((dat_datctl.control & CTL_CALLOUT_NONE) == 0)
      {
      PCRE2_SET_CALLOUT(dat_context, callout_function,
        (void *)(&dat_datctl.callout_data));
      first_callout = TRUE;
      last_callout_mark = NULL;
      callout_count = 0;
      }
    else
      {
      PCRE2_SET_CALLOUT(dat_context, NULL, NULL);  /* No callout */
      }

    /* Run a single DFA or NFA match. */

    if ((dat_datctl.control & CTL_DFA) != 0)
      {
      if (dfa_workspace == NULL)
        dfa_workspace = (int *)malloc(DFA_WS_DIMENSION*sizeof(int));
      if (dfa_matched++ == 0)
        dfa_workspace[0] = -1;  /* To catch bad restart */
      PCRE2_DFA_MATCH(capcount, compiled_code, pp, arg_ulen,
        dat_datctl.offset, dat_datctl.options | g_notempty, match_data,
        use_dat_context, dfa_workspace, DFA_WS_DIMENSION);
      if (capcount == 0)
        {
        fprintf(outfile, "Matched, but offsets vector is too small to show all matches\n");
        capcount = dat_datctl.oveccount;
        }
      }
    else
      {
      if ((pat_patctl.control & CTL_JITFAST) != 0)
        PCRE2_JIT_MATCH(capcount, compiled_code, pp, arg_ulen, dat_datctl.offset,
          dat_datctl.options | g_notempty, match_data, use_dat_context);
      else
        PCRE2_MATCH(capcount, compiled_code, pp, arg_ulen, dat_datctl.offset,
          dat_datctl.options | g_notempty, match_data, use_dat_context);
      if (capcount == 0)
        {
        fprintf(outfile, "Matched, but too many substrings\n");
        capcount = dat_datctl.oveccount;
        }
      }
    }

  /* The result of the match is now in capcount. First handle a successful
  match. */

  if (capcount >= 0)
    {
    int i;
    uint32_t oveccount;

    /* This is a check against a lunatic return value. */

    PCRE2_GET_OVECTOR_COUNT(oveccount, match_data);
    if (capcount > (int)oveccount)
      {
      fprintf(outfile,
        "** PCRE2 error: returned count %d is too big for ovector count %d\n",
        capcount, oveccount);
      capcount = oveccount;
      if ((dat_datctl.control & CTL_ANYGLOB) != 0)
        {
        fprintf(outfile, "** Global loop abandoned\n");
        dat_datctl.control &= ~CTL_ANYGLOB;        /* Break g/G loop */
        }
      }

    /* If this is not the first time round a global loop, check that the
    returned string has changed. If not, there is a bug somewhere and we must
    break the loop because it will go on for ever. We know that there are
    always at least two elements in the ovector. */

    if (gmatched > 0 && ovecsave[0] == ovector[0] && ovecsave[1] == ovector[1])
      {
      fprintf(outfile,
        "** PCRE2 error: global repeat returned the same string as previous\n");
      fprintf(outfile, "** Global loop abandoned\n");
      dat_datctl.control &= ~CTL_ANYGLOB;        /* Break g/G loop */
      }

    /* "allcaptures" requests showing of all captures in the pattern, to check
    unset ones at the end. It may be set on the pattern or the data. Implement
    by setting capcount to the maximum. This is not relevant for DFA matching,
    so ignore it. */

    if ((dat_datctl.control & CTL_ALLCAPTURES) != 0)
      {
      uint32_t maxcapcount;
      if ((dat_datctl.control & CTL_DFA) != 0)
        {
        fprintf(outfile, "** Ignored after DFA matching: allcaptures\n");
        }
      else
        {
        if (pattern_info(PCRE2_INFO_CAPTURECOUNT, &maxcapcount, FALSE) < 0)
          return PR_SKIP;
        capcount = maxcapcount + 1;   /* Allow for full match */
        if (capcount > (int)oveccount) capcount = oveccount;
        }
      }

    /* Output the captured substrings. Note that, for the matched string,
    the use of \K in an assertion can make the start later than the end. */

    for (i = 0; i < 2*capcount; i += 2)
      {
      PCRE2_SIZE lleft, lmiddle, lright;
      PCRE2_SIZE start = ovector[i];
      PCRE2_SIZE end = ovector[i+1];

      if (start > end)
        {
        start = ovector[i+1];
        end = ovector[i];
        fprintf(outfile, "Start of matched string is beyond its end - "
          "displaying from end to start.\n");
        }

      fprintf(outfile, "%2d: ", i/2);

      /* Check for an unset group */

      if (start == PCRE2_UNSET)
        {
        fprintf(outfile, "<unset>\n");
        continue;
        }

      /* Check for silly offsets, in particular, values that have not been
      set when they should have been. */

      if (start > ulen || end > ulen)
        {
        fprintf(outfile, "ERROR: bad value(s) for offset(s): 0x%lx 0x%lx\n",
          (unsigned long int)start, (unsigned long int)end);
        continue;
        }

      /* When JIT is not being used, ALLUSEDTEXT may be set. (It if is set with
      JIT, it is disabled above, with a comment.) When the match is done by the
      interpreter, leftchar and rightchar are available, and if ALLUSEDTEXT is
      set, and if the leftmost consulted character is before the start of the
      match or the rightmost consulted character is past the end of the match,
      we want to show all consulted characters for the main matched string, and
      indicate which were lookarounds. */

      if (i == 0)
        {
        BOOL showallused;
        PCRE2_SIZE leftchar, rightchar;

        if ((dat_datctl.control & CTL_ALLUSEDTEXT) != 0)
          {
          leftchar = FLD(match_data, leftchar);
          rightchar = FLD(match_data, rightchar);
          showallused = i == 0 && (leftchar < start || rightchar > end);
          }
        else showallused = FALSE;

        if (showallused)
          {
          PCHARS(lleft, pp, leftchar, start - leftchar, utf, outfile);
          PCHARS(lmiddle, pp, start, end - start, utf, outfile);
          PCHARS(lright, pp, end, rightchar - end, utf, outfile);
          if ((pat_patctl.control & CTL_JITVERIFY) != 0 && jit_was_used)
            fprintf(outfile, " (JIT)");
          fprintf(outfile, "\n    ");
          for (j = 0; j < lleft; j++) fprintf(outfile, "<");
          for (j = 0; j < lmiddle; j++) fprintf(outfile, " ");
          for (j = 0; j < lright; j++) fprintf(outfile, ">");
          }

        /* When a pattern contains \K, the start of match position may be
        different to the start of the matched string. When this is the case,
        show it when requested. */

        else if ((dat_datctl.control & CTL_STARTCHAR) != 0)
          {
          PCRE2_SIZE startchar;
          PCRE2_GET_STARTCHAR(startchar, match_data);
          PCHARS(lleft, pp, startchar, start - startchar, utf, outfile);
          PCHARSV(pp, start, end - start, utf, outfile);
          if ((pat_patctl.control & CTL_JITVERIFY) != 0 && jit_was_used)
            fprintf(outfile, " (JIT)");
          if (startchar != start)
            {
            fprintf(outfile, "\n    ");
            for (j = 0; j < lleft; j++) fprintf(outfile, "^");
            }
          }

        /* Otherwise, just show the matched string. */

        else
          {
          PCHARSV(pp, start, end - start, utf, outfile);
          if ((pat_patctl.control & CTL_JITVERIFY) != 0 && jit_was_used)
            fprintf(outfile, " (JIT)");
          }
        }

      /* Not the main matched string. Just show it unadorned. */

      else
        {
        PCHARSV(pp, start, end - start, utf, outfile);
        }

      fprintf(outfile, "\n");

      /* Note: don't use the start/end variables here because we want to
      show the text from what is reported as the end. */

      if ((dat_datctl.control & CTL_ALLAFTERTEXT) != 0 ||
          (i == 0 && (dat_datctl.control & CTL_AFTERTEXT) != 0))
        {
        fprintf(outfile, "%2d+ ", i/2);
        PCHARSV(pp, ovector[i+1], ulen - ovector[i+1], utf, outfile);
        fprintf(outfile, "\n");
        }
      }

    /* Output (*MARK) data if requested */

    if ((dat_datctl.control & CTL_MARK) != 0 &&
         TESTFLD(match_data, mark, !=, NULL))
      {
      fprintf(outfile, "MK: ");
      PCHARSV(CASTFLD(void *, match_data, mark), 0, -1, utf, outfile);
      fprintf(outfile, "\n");
      }

    /* Process copy/get strings */

    if (!copy_and_get(utf, capcount)) return PR_ABEND;

    }    /* End of handling a successful match */

  /* There was a partial match. The value of ovector[0] is the bumpalong point,
  that is, startchar, not any \K point that might have been passed. */

  else if (capcount == PCRE2_ERROR_PARTIAL)
    {
    PCRE2_SIZE poffset;
    int backlength;
    int rubriclength = 0;

    fprintf(outfile, "Partial match");
    if ((dat_datctl.control & CTL_MARK) != 0 &&
         TESTFLD(match_data, mark, !=, NULL))
      {
      fprintf(outfile, ", mark=");
      PCHARS(rubriclength, CASTFLD(void *, match_data, mark), 0, -1, utf,
        outfile);
      rubriclength += 7;
      }
    fprintf(outfile, ": ");
    rubriclength += 15;

    poffset = backchars(pp, ovector[0], maxlookbehind, utf);
    PCHARS(backlength, pp, poffset, ovector[0] - poffset, utf, outfile);
    PCHARSV(pp, ovector[0], ulen - ovector[0], utf, outfile);

    if ((pat_patctl.control & CTL_JITVERIFY) != 0 && jit_was_used)
      fprintf(outfile, " (JIT)");
    fprintf(outfile, "\n");

    if (backlength != 0)
      {
      int i;
      for (i = 0; i < rubriclength; i++) fprintf(outfile, " ");
      for (i = 0; i < backlength; i++) fprintf(outfile, "<");
      fprintf(outfile, "\n");
      }

    /* Process copy/get strings */

    if (!copy_and_get(utf, 1)) return PR_ABEND;

    break;  /* Out of the /g loop */
    }       /* End of handling partial match */

  /* Failed to match. If this is a /g or /G loop, we might previously have
  set g_notempty (to PCRE2_NOTEMPTY_ATSTART|PCRE2_ANCHORED) after a null match.
  If that is the case, this is not necessarily the end. We want to advance the
  start offset, and continue. We won't be at the end of the string - that was
  checked before setting g_notempty. We achieve the effect by pretending that a
  single character was matched.

  Complication arises in the case when the newline convention is "any", "crlf",
  or "anycrlf". If the previous match was at the end of a line terminated by
  CRLF, an advance of one character just passes the CR, whereas we should
  prefer the longer newline sequence, as does the code in pcre2_match().

  Otherwise, in the case of UTF-8 or UTF-16 matching, the advance must be one
  character, not one byte. */

  else if (g_notempty != 0)   /* There was a previous null match */
    {
    uint16_t nl = FLD(compiled_code, newline_convention);
    PCRE2_SIZE start_offset = dat_datctl.offset;    /* Where the match was */
    PCRE2_SIZE end_offset = start_offset + 1;

    if ((nl == PCRE2_NEWLINE_CRLF || nl == PCRE2_NEWLINE_ANY ||
         nl == PCRE2_NEWLINE_ANYCRLF) &&
        start_offset < ulen - 1 &&
        CODE_UNIT(pp, start_offset) == '\r' &&
        CODE_UNIT(pp, end_offset) == '\n')
      end_offset++;

    else if (utf && test_mode != PCRE32_MODE)
      {
      if (test_mode == PCRE8_MODE)
        {
        for (; end_offset < ulen; end_offset++)
          if ((((PCRE2_SPTR8)pp)[end_offset] & 0xc0) != 0x80) break;
        }
      else  /* 16-bit mode */
        {
        for (; end_offset < ulen; end_offset++)
          if ((((PCRE2_SPTR16)pp)[end_offset] & 0xfc00) != 0xdc00) break;
        }
      }

    SETFLDVEC(match_data, ovector, 0, start_offset);
    SETFLDVEC(match_data, ovector, 1, end_offset);
    }  /* End of handling null match in a global loop */

  /* A "normal" match failure. There will be a negative error number in
  capcount. */

  else
    {
    switch(capcount)
      {
      case PCRE2_ERROR_NOMATCH:
      if (gmatched == 0)
        {
        fprintf(outfile, "No match");
        if ((dat_datctl.control & CTL_MARK) != 0 &&
             TESTFLD(match_data, mark, !=, NULL))
          {
          fprintf(outfile, ", mark = ");
          PCHARSV(CASTFLD(void *, match_data, mark), 0, -1, utf, outfile);
          }
        if ((pat_patctl.control & CTL_JITVERIFY) != 0 && jit_was_used)
          fprintf(outfile, " (JIT)");
        fprintf(outfile, "\n");
        }
      break;

      case PCRE2_ERROR_BADUTFOFFSET:
      fprintf(outfile, "Error %d (bad UTF-%d offset)\n", capcount, test_mode);
      break;

      default:
      fprintf(outfile, "Failed: error %d: ", capcount);
      if (!print_error_message(capcount, "", "")) return PR_ABEND;
      if (capcount <= PCRE2_ERROR_UTF8_ERR1 &&
          capcount >= PCRE2_ERROR_UTF32_ERR2)
        {
        PCRE2_SIZE startchar;
        PCRE2_GET_STARTCHAR(startchar, match_data);
        fprintf(outfile, " at offset %lu", (unsigned long int)startchar);
        }
      fprintf(outfile, "\n");
      break;
      }

    break;  /* Out of the /g loop */
    }       /* End of failed match handling */

  /* Control reaches here in two circumstances: (a) after a match, and (b)
  after a non-match that immediately followed a match on an empty string when
  doing a global search. Such a match is done with PCRE2_NOTEMPTY_ATSTART and
  PCRE2_ANCHORED set in g_notempty. The code above turns it into a fake match
  of one character. So effectively we get here only after a match. If we
  are not doing a global search, we are done. */

  if ((dat_datctl.control & CTL_ANYGLOB) == 0) break; else
    {
    PCRE2_SIZE end_offset = FLD(match_data, ovector)[1];

    /* We must now set up for the next iteration of a global search. If we have
    matched an empty string, first check to see if we are at the end of the
    subject. If so, the loop is over. Otherwise, mimic what Perl's /g option
    does. Set PCRE2_NOTEMPTY_ATSTART and PCRE2_ANCHORED and try the match again
    at the same point. If this fails it will be picked up above, where a fake
    match is set up so that at this point we advance to the next character. */

    if (FLD(match_data, ovector)[0] == end_offset)
      {
      if (end_offset == ulen) break;      /* End of subject */
      g_notempty = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
      }

    /* However, even after matching a non-empty string, there is still one
    tricky case. If a pattern contains \K within a lookbehind assertion at the
    start, the end of the matched string can be at the offset where the match
    started. In the case of a normal /g iteration without special action, this
    leads to a loop that keeps on returning the same substring. The loop would
    be caught above, but we really want to move on to the next match. */

    else
      {
      g_notempty = 0;   /* Set for a "normal" repeat */
      if ((dat_datctl.control & CTL_GLOBAL) != 0)
        {
        PCRE2_SIZE startchar;
        PCRE2_GET_STARTCHAR(startchar, match_data);
        if (end_offset <= startchar)
          {
          if (startchar >= ulen) break;       /* End of subject */
          end_offset = startchar + 1;
          if (utf && test_mode != PCRE32_MODE)
            {
            if (test_mode == PCRE8_MODE)
              {
              for (; end_offset < ulen; end_offset++)
                if ((((PCRE2_SPTR8)pp)[end_offset] & 0xc0) != 0x80) break;
              }
            else  /* 16-bit mode */
              {
              for (; end_offset < ulen; end_offset++)
                if ((((PCRE2_SPTR16)pp)[end_offset] & 0xfc00) != 0xdc00) break;
              }
            }
          }
        }
      }

    /* For /g (global), update the start offset, leaving the rest alone. */

    if ((dat_datctl.control & CTL_GLOBAL) != 0)
      dat_datctl.offset = end_offset;

    /* For altglobal, just update the pointer and length. */

    else
      {
      pp += end_offset * code_unit_size;
      len -= end_offset * code_unit_size;
      ulen -= end_offset;
      if (arg_ulen != PCRE2_ZERO_TERMINATED) arg_ulen -= end_offset;
      }
    }
  }  /* End of global loop */

show_memory = FALSE;
return PR_OK;
}




/*************************************************
*               Print PCRE2 version              *
*************************************************/

static void
print_version(FILE *f)
{
VERSION_TYPE *vp;
fprintf(f, "PCRE2 version ");
for (vp = version; *vp != 0; vp++) fprintf(f, "%c", *vp);
fprintf(f, "\n");
}



/*************************************************
*               Print Unicode version            *
*************************************************/

static void
print_unicode_version(FILE *f)
{
VERSION_TYPE *vp;
fprintf(f, "Unicode version ");
for (vp = uversion; *vp != 0; vp++) fprintf(f, "%c", *vp);
}



/*************************************************
*               Print JIT target                 *
*************************************************/

static void
print_jit_target(FILE *f)
{
VERSION_TYPE *vp;
for (vp = jittarget; *vp != 0; vp++) fprintf(f, "%c", *vp);
}



/*************************************************
*       Print newline configuration              *
*************************************************/

/* Output is always to stdout.

Arguments:
  rc         the return code from PCRE2_CONFIG_NEWLINE
  isc        TRUE if called from "-C newline"
Returns:     nothing
*/

static void
print_newline_config(uint32_t optval, BOOL isc)
{
if (!isc) printf("  Newline sequence is ");
if (optval < sizeof(newlines)/sizeof(char *))
  printf("%s\n", newlines[optval]);
else
  printf("a non-standard value: %d\n", optval);
}



/*************************************************
*             Usage function                     *
*************************************************/

static void
usage(void)
{
printf("Usage:     pcre2test [options] [<input file> [<output file>]]\n\n");
printf("Input and output default to stdin and stdout.\n");
#if defined(SUPPORT_LIBREADLINE) || defined(SUPPORT_LIBEDIT)
printf("If input is a terminal, readline() is used to read from it.\n");
#else
printf("This version of pcre2test is not linked with readline().\n");
#endif
printf("\nOptions:\n");
#ifdef SUPPORT_PCRE2_8
printf("  -8            use the 8-bit library\n");
#endif
#ifdef SUPPORT_PCRE2_16
printf("  -16           use the 16-bit library\n");
#endif
#ifdef SUPPORT_PCRE2_32
printf("  -32           use the 32-bit library\n");
#endif
printf("  -ac           set default pattern option PCRE2_AUTO_CALLOUT\n");
printf("  -b            set default pattern modifier 'fullbincode'\n");
printf("  -C            show PCRE2 compile-time options and exit\n");
printf("  -C arg        show a specific compile-time option and exit with its\n");
printf("                  value if numeric (else 0). The arg can be:\n");
printf("     backslash-C    use of \\C is enabled [0, 1]\n");
printf("     bsr            \\R type [ANYCRLF, ANY]\n");
printf("     ebcdic         compiled for EBCDIC character code [0,1]\n");
printf("     ebcdic-nl      NL code if compiled for EBCDIC\n");
printf("     jit            just-in-time compiler supported [0, 1]\n");
printf("     linksize       internal link size [2, 3, 4]\n");
printf("     newline        newline type [CR, LF, CRLF, ANYCRLF, ANY, NUL]\n");
printf("     pcre2-8        8 bit library support enabled [0, 1]\n");
printf("     pcre2-16       16 bit library support enabled [0, 1]\n");
printf("     pcre2-32       32 bit library support enabled [0, 1]\n");
printf("     unicode        Unicode and UTF support enabled [0, 1]\n");
printf("  -d            set default pattern modifier 'debug'\n");
printf("  -dfa          set default subject modifier 'dfa'\n");
printf("  -error <n,m,..>  show messages for error numbers, then exit\n");
printf("  -help         show usage information\n");
printf("  -i            set default pattern modifier 'info'\n");
printf("  -jit          set default pattern modifier 'jit'\n");
printf("  -jitverify    set default pattern modifier 'jitverify'\n");
printf("  -q            quiet: do not output PCRE2 version number at start\n");
printf("  -pattern <s>  set default pattern modifier fields\n");
printf("  -subject <s>  set default subject modifier fields\n");
printf("  -S <n>        set stack size to <n> megabytes\n");
printf("  -t [<n>]      time compilation and execution, repeating <n> times\n");
printf("  -tm [<n>]     time execution (matching) only, repeating <n> times\n");
printf("  -T            same as -t, but show total times at the end\n");
printf("  -TM           same as -tm, but show total time at the end\n");
printf("  -version      show PCRE2 version and exit\n");
}



/*************************************************
*             Handle -C option                   *
*************************************************/

/* This option outputs configuration options and sets an appropriate return
code when asked for a single option. The code is abstracted into a separate
function because of its size. Use whichever pcre2_config() function is
available.

Argument:   an option name or NULL
Returns:    the return code
*/

static int
c_option(const char *arg)
{
uint32_t optval;
int yield = 0;

if (arg != NULL)
  {
  unsigned int i;

  for (i = 0; i < COPTLISTCOUNT; i++)
    if (strcmp(arg, coptlist[i].name) == 0) break;

  if (i >= COPTLISTCOUNT)
    {
    fprintf(stderr, "** Unknown -C option '%s'\n", arg);
    return -1;
    }

  switch (coptlist[i].type)
    {
    case CONF_BSR:
    (void)PCRE2_CONFIG(coptlist[i].value, &optval);
    printf("%s\n", optval? "ANYCRLF" : "ANY");
    break;

    case CONF_FIX:
    yield = coptlist[i].value;
    printf("%d\n", yield);
    break;

    case CONF_FIZ:
    optval = coptlist[i].value;
    printf("%d\n", optval);
    break;

    case CONF_INT:
    (void)PCRE2_CONFIG(coptlist[i].value, &yield);
    printf("%d\n", yield);
    break;

    case CONF_NL:
    (void)PCRE2_CONFIG(coptlist[i].value, &optval);
    print_newline_config(optval, TRUE);
    break;
    }

/* For VMS, return the value by setting a symbol, for certain values only. */

#ifdef __VMS
  if (copytlist[i].type == CONF_FIX || coptlist[i].type == CONF_INT)
    {
    char ucname[16];
    strcpy(ucname, coptlist[i].name);
    for (i = 0; ucname[i] != 0; i++) ucname[i] = toupper[ucname[i];
    vms_setsymbol(ucname, 0, optval);
    }
#endif

  return yield;
  }

/* No argument for -C: output all configuration information. */

print_version(stdout);
printf("Compiled with\n");

#ifdef EBCDIC
printf("  EBCDIC code support: LF is 0x%02x\n", CHAR_LF);
#if defined NATIVE_ZOS
printf("  EBCDIC code page %s or similar\n", pcrz_cpversion());
#endif
#endif

#ifdef SUPPORT_PCRE2_8
printf("  8-bit support\n");
#endif
#ifdef SUPPORT_PCRE2_16
printf("  16-bit support\n");
#endif
#ifdef SUPPORT_PCRE2_32
printf("  32-bit support\n");
#endif
#ifdef SUPPORT_VALGRIND
printf("  Valgrind support\n");
#endif

(void)PCRE2_CONFIG(PCRE2_CONFIG_UNICODE, &optval);
if (optval != 0)
  {
  printf("  UTF and UCP support (");
  print_unicode_version(stdout);
  printf(")\n");
  }
else printf("  No Unicode support\n");

(void)PCRE2_CONFIG(PCRE2_CONFIG_JIT, &optval);
if (optval != 0)
  {
  printf("  Just-in-time compiler support: ");
  print_jit_target(stdout);
  printf("\n");
  }
else
  {
  printf("  No just-in-time compiler support\n");
  }

(void)PCRE2_CONFIG(PCRE2_CONFIG_NEWLINE, &optval);
print_newline_config(optval, FALSE);
(void)PCRE2_CONFIG(PCRE2_CONFIG_BSR, &optval);
printf("  \\R matches %s\n", optval? "CR, LF, or CRLF only" :
                                 "all Unicode newlines");
#ifdef NEVER_BACKSLASH_C
printf("  \\C is not supported\n");
#else
printf("  \\C is supported\n");
#endif
(void)PCRE2_CONFIG(PCRE2_CONFIG_LINKSIZE, &optval);
printf("  Internal link size = %d\n", optval);
(void)PCRE2_CONFIG(PCRE2_CONFIG_PARENSLIMIT, &optval);
printf("  Parentheses nest limit = %d\n", optval);
(void)PCRE2_CONFIG(PCRE2_CONFIG_HEAPLIMIT, &optval);
printf("  Default heap limit = %d\n", optval);
(void)PCRE2_CONFIG(PCRE2_CONFIG_MATCHLIMIT, &optval);
printf("  Default match limit = %d\n", optval);
(void)PCRE2_CONFIG(PCRE2_CONFIG_DEPTHLIMIT, &optval);
printf("  Default depth limit = %d\n", optval);
return 0;
}



/*************************************************
*                Main Program                    *
*************************************************/

int
main(int argc, char **argv)
{
uint32_t temp;
uint32_t yield = 0;
uint32_t op = 1;
BOOL notdone = TRUE;
BOOL quiet = FALSE;
BOOL showtotaltimes = FALSE;
BOOL skipping = FALSE;
char *arg_subject = NULL;
char *arg_pattern = NULL;
char *arg_error = NULL;

/* The offsets to the options and control bits fields of the pattern and data
control blocks must be the same so that common options and controls such as
"anchored" or "memory" can work for either of them from a single table entry.
We cannot test this till runtime because "offsetof" does not work in the
preprocessor. */

if (PO(options) != DO(options) || PO(control) != DO(control) ||
    PO(control2) != DO(control2))
  {
  fprintf(stderr, "** Coding error: "
    "options and control offsets for pattern and data must be the same.\n");
  return 1;
  }

/* Get the PCRE2 and Unicode version number and JIT target information, at the
same time checking that a request for the length gives the same answer. Also
check lengths for non-string items. */

if (PCRE2_CONFIG(PCRE2_CONFIG_VERSION, NULL) !=
    PCRE2_CONFIG(PCRE2_CONFIG_VERSION, version) ||

    PCRE2_CONFIG(PCRE2_CONFIG_UNICODE_VERSION, NULL) !=
    PCRE2_CONFIG(PCRE2_CONFIG_UNICODE_VERSION, uversion) ||

    PCRE2_CONFIG(PCRE2_CONFIG_JITTARGET, NULL) !=
    PCRE2_CONFIG(PCRE2_CONFIG_JITTARGET, jittarget) ||

    PCRE2_CONFIG(PCRE2_CONFIG_UNICODE, NULL) != sizeof(uint32_t) ||
    PCRE2_CONFIG(PCRE2_CONFIG_MATCHLIMIT, NULL) != sizeof(uint32_t))
  {
  fprintf(stderr, "** Error in pcre2_config(): bad length\n");
  return 1;
  }

/* Check that bad options are diagnosed. */

if (PCRE2_CONFIG(999, NULL) != PCRE2_ERROR_BADOPTION ||
    PCRE2_CONFIG(999, &temp) != PCRE2_ERROR_BADOPTION)
  {
  fprintf(stderr, "** Error in pcre2_config(): bad option not diagnosed\n");
  return 1;
  }

/* This configuration option is now obsolete, but running a quick check ensures
that its code is covered. */

(void)PCRE2_CONFIG(PCRE2_CONFIG_STACKRECURSE, &temp);

/* Get buffers from malloc() so that valgrind will check their misuse when
debugging. They grow automatically when very long lines are read. The 16-
and 32-bit buffers (pbuffer16, pbuffer32) are obtained only if needed. */

buffer = (uint8_t *)malloc(pbuffer8_size);
pbuffer8 = (uint8_t *)malloc(pbuffer8_size);

/* The following  _setmode() stuff is some Windows magic that tells its runtime
library to translate CRLF into a single LF character. At least, that's what
I've been told: never having used Windows I take this all on trust. Originally
it set 0x8000, but then I was advised that _O_BINARY was better. */

#if defined(_WIN32) || defined(WIN32)
_setmode( _fileno( stdout ), _O_BINARY );
#endif

/* Initialization that does not depend on the running mode. */

locale_name[0] = 0;

memset(&def_patctl, 0, sizeof(patctl));
def_patctl.convert_type = CONVERT_UNSET;

memset(&def_datctl, 0, sizeof(datctl));
def_datctl.oveccount = DEFAULT_OVECCOUNT;
def_datctl.copy_numbers[0] = -1;
def_datctl.get_numbers[0] = -1;
def_datctl.startend[0] = def_datctl.startend[1] = CFORE_UNSET;
def_datctl.cerror[0] = def_datctl.cerror[1] = CFORE_UNSET;
def_datctl.cfail[0] = def_datctl.cfail[1] = CFORE_UNSET;

/* Scan command line options. */

while (argc > 1 && argv[op][0] == '-' && argv[op][1] != 0)
  {
  char *endptr;
  char *arg = argv[op];
  unsigned long uli;

  /* Display and/or set return code for configuration options. */

  if (strcmp(arg, "-C") == 0)
    {
    yield = c_option(argv[op + 1]);
    goto EXIT;
    }

  /* Select operating mode. Ensure that pcre2_config() is called in 16-bit
  and 32-bit modes because that won't happen naturally when 8-bit is also
  configured. Also call some other functions that are not otherwise used. This
  means that a coverage report won't claim there are uncalled functions. */

  if (strcmp(arg, "-8") == 0)
    {
#ifdef SUPPORT_PCRE2_8
    test_mode = PCRE8_MODE;
    (void)pcre2_set_bsr_8(pat_context8, 999);
    (void)pcre2_set_newline_8(pat_context8, 999);
#else
    fprintf(stderr,
      "** This version of PCRE2 was built without 8-bit support\n");
    exit(1);
#endif
    }

  else if (strcmp(arg, "-16") == 0)
    {
#ifdef SUPPORT_PCRE2_16
    test_mode = PCRE16_MODE;
    (void)pcre2_config_16(PCRE2_CONFIG_VERSION, NULL);
    (void)pcre2_set_bsr_16(pat_context16, 999);
    (void)pcre2_set_newline_16(pat_context16, 999);
#else
    fprintf(stderr,
      "** This version of PCRE2 was built without 16-bit support\n");
    exit(1);
#endif
    }

  else if (strcmp(arg, "-32") == 0)
    {
#ifdef SUPPORT_PCRE2_32
    test_mode = PCRE32_MODE;
    (void)pcre2_config_32(PCRE2_CONFIG_VERSION, NULL);
    (void)pcre2_set_bsr_32(pat_context32, 999);
    (void)pcre2_set_newline_32(pat_context32, 999);
#else
    fprintf(stderr,
      "** This version of PCRE2 was built without 32-bit support\n");
    exit(1);
#endif
    }

  /* Set quiet (no version verification) */

  else if (strcmp(arg, "-q") == 0) quiet = TRUE;

  /* Set system stack size */

  else if (strcmp(arg, "-S") == 0 && argc > 2 &&
      ((uli = strtoul(argv[op+1], &endptr, 10)), *endptr == 0))
    {
#if defined(_WIN32) || defined(WIN32) || defined(__minix) || defined(NATIVE_ZOS) || defined(__VMS)
    fprintf(stderr, "pcre2test: -S is not supported on this OS\n");
    exit(1);
#else
    int rc;
    uint32_t stack_size;
    struct rlimit rlim;
    if (U32OVERFLOW(uli))
      {
      fprintf(stderr, "** Argument for -S is too big\n");
      exit(1);
      }
    stack_size = (uint32_t)uli;
    getrlimit(RLIMIT_STACK, &rlim);
    rlim.rlim_cur = stack_size * 1024 * 1024;
    if (rlim.rlim_cur > rlim.rlim_max)
      {
      fprintf(stderr,
        "pcre2test: requested stack size %luM is greater than hard limit %lu\n",
        (unsigned long int)stack_size,
        (unsigned long int)(rlim.rlim_max));
      exit(1);
      }
    rc = setrlimit(RLIMIT_STACK, &rlim);
    if (rc != 0)
      {
      fprintf(stderr, "pcre2test: setting stack size %luM failed: %s\n",
        (unsigned long int)stack_size, strerror(errno));
      exit(1);
      }
    op++;
    argc--;
#endif
    }

  /* Set some common pattern and subject controls */

  else if (strcmp(arg, "-ac") == 0)  def_patctl.options |= PCRE2_AUTO_CALLOUT;
  else if (strcmp(arg, "-b") == 0)   def_patctl.control |= CTL_FULLBINCODE;
  else if (strcmp(arg, "-d") == 0)   def_patctl.control |= CTL_DEBUG;
  else if (strcmp(arg, "-dfa") == 0) def_datctl.control |= CTL_DFA;
  else if (strcmp(arg, "-i") == 0)   def_patctl.control |= CTL_INFO;
  else if (strcmp(arg, "-jit") == 0 || strcmp(arg, "-jitverify") == 0)
    {
    if (arg[4] != 0) def_patctl.control |= CTL_JITVERIFY;
    def_patctl.jit = 7;  /* full & partial */
#ifndef SUPPORT_JIT
    fprintf(stderr, "** Warning: JIT support is not available: "
                    "-jit[verify] calls functions that do nothing.\n");
#endif
    }

  /* Set timing parameters */

  else if (strcmp(arg, "-t") == 0 || strcmp(arg, "-tm") == 0 ||
           strcmp(arg, "-T") == 0 || strcmp(arg, "-TM") == 0)
    {
    int both = arg[2] == 0;
    showtotaltimes = arg[1] == 'T';
    if (argc > 2 && (uli = strtoul(argv[op+1], &endptr, 10), *endptr == 0))
      {
      if (U32OVERFLOW(uli))
        {
        fprintf(stderr, "** Argument for %s is too big\n", arg);
        exit(1);
        }
      timeitm = (int)uli;
      op++;
      argc--;
      }
    else timeitm = LOOPREPEAT;
    if (both) timeit = timeitm;
    }

  /* Give help */

  else if (strcmp(arg, "-help") == 0 ||
           strcmp(arg, "--help") == 0)
    {
    usage();
    goto EXIT;
    }

  /* Show version */

  else if (strcmp(arg, "-version") == 0 ||
           strcmp(arg, "--version") == 0)
    {
    print_version(stdout);
    goto EXIT;
    }

  /* The following options save their data for processing once we know what
  the running mode is. */

  else if (strcmp(arg, "-error") == 0)
    {
    arg_error = argv[op+1];
    goto CHECK_VALUE_EXISTS;
    }

  else if (strcmp(arg, "-subject") == 0)
    {
    arg_subject = argv[op+1];
    goto CHECK_VALUE_EXISTS;
    }

  else if (strcmp(arg, "-pattern") == 0)
    {
    arg_pattern = argv[op+1];
    CHECK_VALUE_EXISTS:
    if (argc <= 2)
      {
      fprintf(stderr, "** Missing value for %s\n", arg);
      yield = 1;
      goto EXIT;
      }
    op++;
    argc--;
    }

  /* Unrecognized option */

  else
    {
    fprintf(stderr, "** Unknown or malformed option '%s'\n", arg);
    usage();
    yield = 1;
    goto EXIT;
    }
  op++;
  argc--;
  }

/* If -error was present, get the error numbers, show the messages, and exit.
We wait to do this until we know which mode we are in. */

if (arg_error != NULL)
  {
  int len;
  int errcode;
  char *endptr;

/* Ensure the relevant non-8-bit buffer is available. Ensure that it is at
least 128 code units, because it is used for retrieving error messages. */

#ifdef SUPPORT_PCRE2_16
  if (test_mode == PCRE16_MODE)
    {
    pbuffer16_size = 256;
    pbuffer16 = (uint16_t *)malloc(pbuffer16_size);
    if (pbuffer16 == NULL)
      {
      fprintf(stderr, "pcre2test: malloc(%lu) failed for pbuffer16\n",
        (unsigned long int)pbuffer16_size);
      yield = 1;
      goto EXIT;
      }
    }
#endif

#ifdef SUPPORT_PCRE2_32
  if (test_mode == PCRE32_MODE)
    {
    pbuffer32_size = 512;
    pbuffer32 = (uint32_t *)malloc(pbuffer32_size);
    if (pbuffer32 == NULL)
      {
      fprintf(stderr, "pcre2test: malloc(%lu) failed for pbuffer32\n",
        (unsigned long int)pbuffer32_size);
      yield = 1;
      goto EXIT;
      }
    }
#endif

  /* Loop along a list of error numbers. */

  for (;;)
    {
    errcode = strtol(arg_error, &endptr, 10);
    if (*endptr != 0 && *endptr != CHAR_COMMA)
      {
      fprintf(stderr, "** '%s' is not a valid error number list\n", arg_error);
      yield = 1;
      goto EXIT;
      }
    printf("Error %d: ", errcode);
    PCRE2_GET_ERROR_MESSAGE(len, errcode, pbuffer);
    if (len < 0)
      {
      switch (len)
        {
        case PCRE2_ERROR_BADDATA:
        printf("PCRE2_ERROR_BADDATA (unknown error number)");
        break;

        case PCRE2_ERROR_NOMEMORY:
        printf("PCRE2_ERROR_NOMEMORY (buffer too small)");
        break;

        default:
        printf("Unexpected return (%d) from pcre2_get_error_message()", len);
        break;
        }
      }
    else
      {
      PCHARSV(CASTVAR(void *, pbuffer), 0, len, FALSE, stdout);
      }
    printf("\n");
    if (*endptr == 0) goto EXIT;
    arg_error = endptr + 1;
    }
  /* Control never reaches here */
  }  /* End of -error handling */

/* Initialize things that cannot be done until we know which test mode we are
running in. Exercise the general context copying function, which is not
otherwise used. */

code_unit_size = test_mode/8;
max_oveccount = DEFAULT_OVECCOUNT;

/* Use macros to save a lot of duplication. */

#define CREATECONTEXTS \
  G(general_context,BITS) = G(pcre2_general_context_create_,BITS)(&my_malloc, &my_free, NULL); \
  G(general_context_copy,BITS) = G(pcre2_general_context_copy_,BITS)(G(general_context,BITS)); \
  G(default_pat_context,BITS) = G(pcre2_compile_context_create_,BITS)(G(general_context,BITS)); \
  G(pat_context,BITS) = G(pcre2_compile_context_copy_,BITS)(G(default_pat_context,BITS)); \
  G(default_dat_context,BITS) = G(pcre2_match_context_create_,BITS)(G(general_context,BITS)); \
  G(dat_context,BITS) = G(pcre2_match_context_copy_,BITS)(G(default_dat_context,BITS)); \
  G(default_con_context,BITS) = G(pcre2_convert_context_create_,BITS)(G(general_context,BITS)); \
  G(con_context,BITS) = G(pcre2_convert_context_copy_,BITS)(G(default_con_context,BITS)); \
  G(match_data,BITS) = G(pcre2_match_data_create_,BITS)(max_oveccount, G(general_context,BITS))

#define CONTEXTTESTS \
  (void)G(pcre2_set_compile_extra_options_,BITS)(G(pat_context,BITS), 0); \
  (void)G(pcre2_set_max_pattern_length_,BITS)(G(pat_context,BITS), 0); \
  (void)G(pcre2_set_offset_limit_,BITS)(G(dat_context,BITS), 0); \
  (void)G(pcre2_set_recursion_memory_management_,BITS)(G(dat_context,BITS), my_malloc, my_free, NULL)

/* Call the appropriate functions for the current mode, and exercise some
functions that are not otherwise called. */

#ifdef SUPPORT_PCRE2_8
#undef BITS
#define BITS 8
if (test_mode == PCRE8_MODE)
  {
  CREATECONTEXTS;
  CONTEXTTESTS;
  }
#endif

#ifdef SUPPORT_PCRE2_16
#undef BITS
#define BITS 16
if (test_mode == PCRE16_MODE)
  {
  CREATECONTEXTS;
  CONTEXTTESTS;
  }
#endif

#ifdef SUPPORT_PCRE2_32
#undef BITS
#define BITS 32
if (test_mode == PCRE32_MODE)
  {
  CREATECONTEXTS;
  CONTEXTTESTS;
  }
#endif

/* Set a default parentheses nest limit that is large enough to run the
standard tests (this also exercises the function). */

PCRE2_SET_PARENS_NEST_LIMIT(default_pat_context, PARENS_NEST_DEFAULT);

/* Handle command line modifier settings, sending any error messages to
stderr. We need to know the mode before modifying the context, and it is tidier
to do them all in the same way. */

outfile = stderr;
if ((arg_pattern != NULL &&
    !decode_modifiers((uint8_t *)arg_pattern, CTX_DEFPAT, &def_patctl, NULL)) ||
    (arg_subject != NULL &&
    !decode_modifiers((uint8_t *)arg_subject, CTX_DEFDAT, NULL, &def_datctl)))
  {
  yield = 1;
  goto EXIT;
  }

/* Sort out the input and output files, defaulting to stdin/stdout. */

infile = stdin;
outfile = stdout;

if (argc > 1 && strcmp(argv[op], "-") != 0)
  {
  infile = fopen(argv[op], INPUT_MODE);
  if (infile == NULL)
    {
    printf("** Failed to open '%s': %s\n", argv[op], strerror(errno));
    yield = 1;
    goto EXIT;
    }
  }

#if defined(SUPPORT_LIBREADLINE) || defined(SUPPORT_LIBEDIT)
if (INTERACTIVE(infile)) using_history();
#endif

if (argc > 2)
  {
  outfile = fopen(argv[op+1], OUTPUT_MODE);
  if (outfile == NULL)
    {
    printf("** Failed to open '%s': %s\n", argv[op+1], strerror(errno));
    yield = 1;
    goto EXIT;
    }
  }

/* Output a heading line unless quiet, then process input lines. */

if (!quiet) print_version(outfile);

SET(compiled_code, NULL);

#ifdef SUPPORT_PCRE2_8
preg.re_pcre2_code = NULL;
preg.re_match_data = NULL;
#endif

while (notdone)
  {
  uint8_t *p;
  int rc = PR_OK;
  BOOL expectdata = TEST(compiled_code, !=, NULL);
#ifdef SUPPORT_PCRE2_8
  expectdata |= preg.re_pcre2_code != NULL;
#endif

  if (extend_inputline(infile, buffer, expectdata? "data> " : "  re> ") == NULL)
    break;
  if (!INTERACTIVE(infile)) fprintf(outfile, "%s", (char *)buffer);
  fflush(outfile);
  p = buffer;

  /* If we have a pattern set up for testing, or we are skipping after a
  compile failure, a blank line terminates this test. */

  if (expectdata || skipping)
    {
    while (isspace(*p)) p++;
    if (*p == 0)
      {
#ifdef SUPPORT_PCRE2_8
      if (preg.re_pcre2_code != NULL)
        {
        regfree(&preg);
        preg.re_pcre2_code = NULL;
        preg.re_match_data = NULL;
        }
#endif  /* SUPPORT_PCRE2_8 */
      if (TEST(compiled_code, !=, NULL))
        {
        SUB1(pcre2_code_free, compiled_code);
        SET(compiled_code, NULL);
        }
      skipping = FALSE;
      setlocale(LC_CTYPE, "C");
      }

    /* Otherwise, if we are not skipping, and the line is not a data comment
    line starting with "\=", process a data line. */

    else if (!skipping && !(p[0] == '\\' && p[1] == '=' && isspace(p[2])))
      {
      rc = process_data();
      }
    }

  /* We do not have a pattern set up for testing. Lines starting with # are
  either comments or special commands. Blank lines are ignored. Otherwise, the
  line must start with a valid delimiter. It is then processed as a pattern
  line. A copy of the pattern is left in pbuffer8 for use by callouts. Under
  valgrind, make the unused part of the buffer undefined, to catch overruns. */

  else if (*p == '#')
    {
    if (isspace(p[1]) || p[1] == '!' || p[1] == 0) continue;
    rc = process_command();
    }

  else if (strchr("/!\"'`%&-=_:;,@~", *p) != NULL)
    {
    rc = process_pattern();
    dfa_matched = 0;
    }

  else
    {
    while (isspace(*p)) p++;
    if (*p != 0)
      {
      fprintf(outfile, "** Invalid pattern delimiter '%c' (x%x).\n", *buffer,
        *buffer);
      rc = PR_SKIP;
      }
    }

  if (rc == PR_SKIP && !INTERACTIVE(infile)) skipping = TRUE;
  else if (rc == PR_ABEND)
    {
    fprintf(outfile, "** pcre2test run abandoned\n");
    yield = 1;
    goto EXIT;
    }
  }

/* Finish off a normal run. */

if (INTERACTIVE(infile)) fprintf(outfile, "\n");

if (showtotaltimes)
  {
  const char *pad = "";
  fprintf(outfile, "--------------------------------------\n");
  if (timeit > 0)
    {
    fprintf(outfile, "Total compile time %.4f milliseconds\n",
      (((double)total_compile_time * 1000.0) / (double)timeit) /
        (double)CLOCKS_PER_SEC);
    if (total_jit_compile_time > 0)
      fprintf(outfile, "Total JIT compile  %.4f milliseconds\n",
        (((double)total_jit_compile_time * 1000.0) / (double)timeit) /
          (double)CLOCKS_PER_SEC);
    pad = "  ";
    }
  fprintf(outfile, "Total match time %s%.4f milliseconds\n", pad,
    (((double)total_match_time * 1000.0) / (double)timeitm) /
      (double)CLOCKS_PER_SEC);
  }


EXIT:

#if defined(SUPPORT_LIBREADLINE) || defined(SUPPORT_LIBEDIT)
if (infile != NULL && INTERACTIVE(infile)) clear_history();
#endif

if (infile != NULL && infile != stdin) fclose(infile);
if (outfile != NULL && outfile != stdout) fclose(outfile);

free(buffer);
free(dbuffer);
free(pbuffer8);
free(dfa_workspace);
free((void *)locale_tables);
PCRE2_MATCH_DATA_FREE(match_data);
SUB1(pcre2_code_free, compiled_code);

while(patstacknext-- > 0)
  {
  SET(compiled_code, patstack[patstacknext]);
  SUB1(pcre2_code_free, compiled_code);
  }

PCRE2_JIT_FREE_UNUSED_MEMORY(general_context);
if (jit_stack != NULL)
  {
  PCRE2_JIT_STACK_FREE(jit_stack);
  }

#define FREECONTEXTS \
  G(pcre2_general_context_free_,BITS)(G(general_context,BITS)); \
  G(pcre2_general_context_free_,BITS)(G(general_context_copy,BITS)); \
  G(pcre2_compile_context_free_,BITS)(G(pat_context,BITS)); \
  G(pcre2_compile_context_free_,BITS)(G(default_pat_context,BITS)); \
  G(pcre2_match_context_free_,BITS)(G(dat_context,BITS)); \
  G(pcre2_match_context_free_,BITS)(G(default_dat_context,BITS))

#ifdef SUPPORT_PCRE2_8
#undef BITS
#define BITS 8
if (preg.re_pcre2_code != NULL) regfree(&preg);
FREECONTEXTS;
#endif

#ifdef SUPPORT_PCRE2_16
#undef BITS
#define BITS 16
free(pbuffer16);
FREECONTEXTS;
#endif

#ifdef SUPPORT_PCRE2_32
#undef BITS
#define BITS 32
free(pbuffer32);
FREECONTEXTS;
#endif

#if defined(__VMS)
  yield = SS$_NORMAL;  /* Return values via DCL symbols */
#endif

return yield;
}

/* End of pcre2test.c */
