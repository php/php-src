/*************************************************
*               pcre2grep program                *
*************************************************/

/* This is a grep program that uses the 8-bit PCRE regular expression library
via the PCRE2 updated API to do its pattern matching. On Unix-like, Windows,
and native z/OS systems it can recurse into directories, and in z/OS it can
handle PDS files.

Note that for native z/OS, in addition to defining the NATIVE_ZOS macro, an
additional header is required. That header is not included in the main PCRE2
distribution because other apparatus is needed to compile pcre2grep for z/OS.
The header can be found in the special z/OS distribution, which is available
from www.zaconsultants.net or from www.cbttape.org.

           Copyright (c) 1997-2017 University of Cambridge

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#if (defined _WIN32 || (defined HAVE_WINDOWS_H && HAVE_WINDOWS_H)) \
  && !defined WIN32 && !defined(__CYGWIN__)
#define WIN32
#endif

/* Some cmake's define it still */
#if defined(__CYGWIN__) && !defined(WIN32)
#define WIN32
#endif

#ifdef WIN32
#include <io.h>                /* For _setmode() */
#include <fcntl.h>             /* For _O_BINARY */
#endif

#ifdef SUPPORT_PCRE2GREP_CALLOUT
#ifdef WIN32
#include <process.h>
#else
#include <sys/wait.h>
#endif
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef SUPPORT_LIBZ
#include <zlib.h>
#endif

#ifdef SUPPORT_LIBBZ2
#include <bzlib.h>
#endif

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#define FALSE 0
#define TRUE 1

typedef int BOOL;

#define OFFSET_SIZE 33

#if BUFSIZ > 8192
#define MAXPATLEN BUFSIZ
#else
#define MAXPATLEN 8192
#endif

#define FNBUFSIZ 1024
#define ERRBUFSIZ 256

/* Values for the "filenames" variable, which specifies options for file name
output. The order is important; it is assumed that a file name is wanted for
all values greater than FN_DEFAULT. */

enum { FN_NONE, FN_DEFAULT, FN_MATCH_ONLY, FN_NOMATCH_ONLY, FN_FORCE };

/* File reading styles */

enum { FR_PLAIN, FR_LIBZ, FR_LIBBZ2 };

/* Actions for the -d and -D options */

enum { dee_READ, dee_SKIP, dee_RECURSE };
enum { DEE_READ, DEE_SKIP };

/* Actions for special processing options (flag bits) */

#define PO_WORD_MATCH     0x0001
#define PO_LINE_MATCH     0x0002
#define PO_FIXED_STRINGS  0x0004

/* Binary file options */

enum { BIN_BINARY, BIN_NOMATCH, BIN_TEXT };

/* In newer versions of gcc, with FORTIFY_SOURCE set (the default in some
environments), a warning is issued if the value of fwrite() is ignored.
Unfortunately, casting to (void) does not suppress the warning. To get round
this, we use a macro that compiles a fudge. Oddly, this does not also seem to
apply to fprintf(). */

#define FWRITE_IGNORE(a,b,c,d) if (fwrite(a,b,c,d)) {}

/* Under Windows, we have to set stdout to be binary, so that it does not
convert \r\n at the ends of output lines to \r\r\n. However, that means that
any messages written to stdout must have \r\n as their line terminator. This is
handled by using STDOUT_NL as the newline string. We also use a normal double
quote for the example, as single quotes aren't usually available. */

#ifdef WIN32
#define STDOUT_NL  "\r\n"
#define QUOT       "\""
#else
#define STDOUT_NL  "\n"
#define QUOT       "'"
#endif



/*************************************************
*               Global variables                 *
*************************************************/

/* Jeffrey Friedl has some debugging requirements that are not part of the
regular code. */

#ifdef JFRIEDL_DEBUG
static int S_arg = -1;
static unsigned int jfriedl_XR = 0; /* repeat regex attempt this many times */
static unsigned int jfriedl_XT = 0; /* replicate text this many times */
static const char *jfriedl_prefix = "";
static const char *jfriedl_postfix = "";
#endif

static const char *colour_string = "1;31";
static const char *colour_option = NULL;
static const char *dee_option = NULL;
static const char *DEE_option = NULL;
static const char *locale = NULL;
static const char *newline_arg = NULL;
static const char *om_separator = NULL;
static const char *stdin_name = "(standard input)";
static const char *output_text = NULL;

static char *main_buffer = NULL;

static int after_context = 0;
static int before_context = 0;
static int binary_files = BIN_BINARY;
static int both_context = 0;
static int bufthird = PCRE2GREP_BUFSIZE;
static int max_bufthird = PCRE2GREP_MAX_BUFSIZE;
static int bufsize = 3*PCRE2GREP_BUFSIZE;
static int endlinetype;
static int total_count = 0;
static int counts_printed = 0;

#ifdef WIN32
static int dee_action = dee_SKIP;
#else
static int dee_action = dee_READ;
#endif

static int DEE_action = DEE_READ;
static int error_count = 0;
static int filenames = FN_DEFAULT;

#ifdef SUPPORT_PCRE2GREP_JIT
static BOOL use_jit = TRUE;
#else
static BOOL use_jit = FALSE;
#endif

static const uint8_t *character_tables = NULL;

static uint32_t pcre2_options = 0;
static uint32_t extra_options = 0;
static PCRE2_SIZE heap_limit = PCRE2_UNSET;
static uint32_t match_limit = 0;
static uint32_t depth_limit = 0;

static pcre2_compile_context *compile_context;
static pcre2_match_context *match_context;
static pcre2_match_data *match_data;
static PCRE2_SIZE *offsets;

static BOOL count_only = FALSE;
static BOOL do_colour = FALSE;
#ifdef WIN32
static BOOL do_ansi = FALSE;
#endif
static BOOL file_offsets = FALSE;
static BOOL hyphenpending = FALSE;
static BOOL invert = FALSE;
static BOOL line_buffered = FALSE;
static BOOL line_offsets = FALSE;
static BOOL multiline = FALSE;
static BOOL number = FALSE;
static BOOL omit_zero_count = FALSE;
static BOOL resource_error = FALSE;
static BOOL quiet = FALSE;
static BOOL show_total_count = FALSE;
static BOOL silent = FALSE;
static BOOL utf = FALSE;

/* Structure for list of --only-matching capturing numbers. */

typedef struct omstr {
  struct omstr *next;
  int groupnum;
} omstr;

static omstr *only_matching = NULL;
static omstr *only_matching_last = NULL;
static int only_matching_count;

/* Structure for holding the two variables that describe a number chain. */

typedef struct omdatastr {
  omstr **anchor;
  omstr **lastptr;
} omdatastr;

static omdatastr only_matching_data = { &only_matching, &only_matching_last };

/* Structure for list of file names (for -f and --{in,ex}clude-from) */

typedef struct fnstr {
  struct fnstr *next;
  char *name;
} fnstr;

static fnstr *exclude_from = NULL;
static fnstr *exclude_from_last = NULL;
static fnstr *include_from = NULL;
static fnstr *include_from_last = NULL;

static fnstr *file_lists = NULL;
static fnstr *file_lists_last = NULL;
static fnstr *pattern_files = NULL;
static fnstr *pattern_files_last = NULL;

/* Structure for holding the two variables that describe a file name chain. */

typedef struct fndatastr {
  fnstr **anchor;
  fnstr **lastptr;
} fndatastr;

static fndatastr exclude_from_data = { &exclude_from, &exclude_from_last };
static fndatastr include_from_data = { &include_from, &include_from_last };
static fndatastr file_lists_data = { &file_lists, &file_lists_last };
static fndatastr pattern_files_data = { &pattern_files, &pattern_files_last };

/* Structure for pattern and its compiled form; used for matching patterns and
also for include/exclude patterns. */

typedef struct patstr {
  struct patstr *next;
  char *string;
  pcre2_code *compiled;
} patstr;

static patstr *patterns = NULL;
static patstr *patterns_last = NULL;
static patstr *include_patterns = NULL;
static patstr *include_patterns_last = NULL;
static patstr *exclude_patterns = NULL;
static patstr *exclude_patterns_last = NULL;
static patstr *include_dir_patterns = NULL;
static patstr *include_dir_patterns_last = NULL;
static patstr *exclude_dir_patterns = NULL;
static patstr *exclude_dir_patterns_last = NULL;

/* Structure holding the two variables that describe a pattern chain. A pointer
to such structures is used for each appropriate option. */

typedef struct patdatastr {
  patstr **anchor;
  patstr **lastptr;
} patdatastr;

static patdatastr match_patdata = { &patterns, &patterns_last };
static patdatastr include_patdata = { &include_patterns, &include_patterns_last };
static patdatastr exclude_patdata = { &exclude_patterns, &exclude_patterns_last };
static patdatastr include_dir_patdata = { &include_dir_patterns, &include_dir_patterns_last };
static patdatastr exclude_dir_patdata = { &exclude_dir_patterns, &exclude_dir_patterns_last };

static patstr **incexlist[4] = { &include_patterns, &exclude_patterns,
                                 &include_dir_patterns, &exclude_dir_patterns };

static const char *incexname[4] = { "--include", "--exclude",
                                    "--include-dir", "--exclude-dir" };

/* Structure for options and list of them */

enum { OP_NODATA, OP_STRING, OP_OP_STRING, OP_NUMBER, OP_U32NUMBER, OP_SIZE,
       OP_OP_NUMBER, OP_OP_NUMBERS, OP_PATLIST, OP_FILELIST, OP_BINFILES };

typedef struct option_item {
  int type;
  int one_char;
  void *dataptr;
  const char *long_name;
  const char *help_text;
} option_item;

/* Options without a single-letter equivalent get a negative value. This can be
used to identify them. */

#define N_COLOUR       (-1)
#define N_EXCLUDE      (-2)
#define N_EXCLUDE_DIR  (-3)
#define N_HELP         (-4)
#define N_INCLUDE      (-5)
#define N_INCLUDE_DIR  (-6)
#define N_LABEL        (-7)
#define N_LOCALE       (-8)
#define N_NULL         (-9)
#define N_LOFFSETS     (-10)
#define N_FOFFSETS     (-11)
#define N_LBUFFER      (-12)
#define N_H_LIMIT      (-13)
#define N_M_LIMIT      (-14)
#define N_M_LIMIT_DEP  (-15)
#define N_BUFSIZE      (-16)
#define N_NOJIT        (-17)
#define N_FILE_LIST    (-18)
#define N_BINARY_FILES (-19)
#define N_EXCLUDE_FROM (-20)
#define N_INCLUDE_FROM (-21)
#define N_OM_SEPARATOR (-22)
#define N_MAX_BUFSIZE  (-23)

static option_item optionlist[] = {
  { OP_NODATA,     N_NULL,   NULL,              "",              "terminate options" },
  { OP_NODATA,     N_HELP,   NULL,              "help",          "display this help and exit" },
  { OP_NUMBER,     'A',      &after_context,    "after-context=number", "set number of following context lines" },
  { OP_NODATA,     'a',      NULL,              "text",          "treat binary files as text" },
  { OP_NUMBER,     'B',      &before_context,   "before-context=number", "set number of prior context lines" },
  { OP_BINFILES,   N_BINARY_FILES, NULL,        "binary-files=word", "set treatment of binary files" },
  { OP_NUMBER,     N_BUFSIZE,&bufthird,         "buffer-size=number", "set processing buffer starting size" },
  { OP_NUMBER,     N_MAX_BUFSIZE,&max_bufthird, "max-buffer-size=number",  "set processing buffer maximum size" },
  { OP_OP_STRING,  N_COLOUR, &colour_option,    "color=option",  "matched text color option" },
  { OP_OP_STRING,  N_COLOUR, &colour_option,    "colour=option", "matched text colour option" },
  { OP_NUMBER,     'C',      &both_context,     "context=number", "set number of context lines, before & after" },
  { OP_NODATA,     'c',      NULL,              "count",         "print only a count of matching lines per FILE" },
  { OP_STRING,     'D',      &DEE_option,       "devices=action","how to handle devices, FIFOs, and sockets" },
  { OP_STRING,     'd',      &dee_option,       "directories=action", "how to handle directories" },
  { OP_PATLIST,    'e',      &match_patdata,    "regex(p)=pattern", "specify pattern (may be used more than once)" },
  { OP_NODATA,     'F',      NULL,              "fixed-strings", "patterns are sets of newline-separated strings" },
  { OP_FILELIST,   'f',      &pattern_files_data, "file=path",   "read patterns from file" },
  { OP_FILELIST,   N_FILE_LIST, &file_lists_data, "file-list=path","read files to search from file" },
  { OP_NODATA,     N_FOFFSETS, NULL,            "file-offsets",  "output file offsets, not text" },
  { OP_NODATA,     'H',      NULL,              "with-filename", "force the prefixing filename on output" },
  { OP_NODATA,     'h',      NULL,              "no-filename",   "suppress the prefixing filename on output" },
  { OP_NODATA,     'I',      NULL,              "",              "treat binary files as not matching (ignore)" },
  { OP_NODATA,     'i',      NULL,              "ignore-case",   "ignore case distinctions" },
  { OP_NODATA,     'l',      NULL,              "files-with-matches", "print only FILE names containing matches" },
  { OP_NODATA,     'L',      NULL,              "files-without-match","print only FILE names not containing matches" },
  { OP_STRING,     N_LABEL,  &stdin_name,       "label=name",    "set name for standard input" },
  { OP_NODATA,     N_LBUFFER, NULL,             "line-buffered", "use line buffering" },
  { OP_NODATA,     N_LOFFSETS, NULL,            "line-offsets",  "output line numbers and offsets, not text" },
  { OP_STRING,     N_LOCALE, &locale,           "locale=locale", "use the named locale" },
  { OP_SIZE,       N_H_LIMIT, &heap_limit,      "heap-limit=number",  "set PCRE2 heap limit option (kilobytes)" },
  { OP_U32NUMBER,  N_M_LIMIT, &match_limit,     "match-limit=number", "set PCRE2 match limit option" },
  { OP_U32NUMBER,  N_M_LIMIT_DEP, &depth_limit, "depth-limit=number", "set PCRE2 depth limit option" },
  { OP_U32NUMBER,  N_M_LIMIT_DEP, &depth_limit, "recursion-limit=number", "obsolete synonym for depth-limit" },
  { OP_NODATA,     'M',      NULL,              "multiline",     "run in multiline mode" },
  { OP_STRING,     'N',      &newline_arg,      "newline=type",  "set newline type (CR, LF, CRLF, ANYCRLF, ANY, or NUL)" },
  { OP_NODATA,     'n',      NULL,              "line-number",   "print line number with output lines" },
#ifdef SUPPORT_PCRE2GREP_JIT
  { OP_NODATA,     N_NOJIT,  NULL,              "no-jit",        "do not use just-in-time compiler optimization" },
#else
  { OP_NODATA,     N_NOJIT,  NULL,              "no-jit",        "ignored: this pcre2grep does not support JIT" },
#endif
  { OP_STRING,     'O',      &output_text,       "output=text",   "show only this text (possibly expanded)" },
  { OP_OP_NUMBERS, 'o',      &only_matching_data, "only-matching=n", "show only the part of the line that matched" },
  { OP_STRING,     N_OM_SEPARATOR, &om_separator, "om-separator=text", "set separator for multiple -o output" },
  { OP_NODATA,     'q',      NULL,              "quiet",         "suppress output, just set return code" },
  { OP_NODATA,     'r',      NULL,              "recursive",     "recursively scan sub-directories" },
  { OP_PATLIST,    N_EXCLUDE,&exclude_patdata,  "exclude=pattern","exclude matching files when recursing" },
  { OP_PATLIST,    N_INCLUDE,&include_patdata,  "include=pattern","include matching files when recursing" },
  { OP_PATLIST,    N_EXCLUDE_DIR,&exclude_dir_patdata, "exclude-dir=pattern","exclude matching directories when recursing" },
  { OP_PATLIST,    N_INCLUDE_DIR,&include_dir_patdata, "include-dir=pattern","include matching directories when recursing" },
  { OP_FILELIST,   N_EXCLUDE_FROM,&exclude_from_data, "exclude-from=path", "read exclude list from file" },
  { OP_FILELIST,   N_INCLUDE_FROM,&include_from_data, "include-from=path", "read include list from file" },
#ifdef JFRIEDL_DEBUG
  { OP_OP_NUMBER, 'S',      &S_arg,            "jeffS",         "replace matched (sub)string with X" },
#endif
  { OP_NODATA,    's',      NULL,              "no-messages",   "suppress error messages" },
  { OP_NODATA,    't',      NULL,              "total-count",   "print total count of matching lines" },
  { OP_NODATA,    'u',      NULL,              "utf",           "use UTF mode" },
  { OP_NODATA,    'V',      NULL,              "version",       "print version information and exit" },
  { OP_NODATA,    'v',      NULL,              "invert-match",  "select non-matching lines" },
  { OP_NODATA,    'w',      NULL,              "word-regex(p)", "force patterns to match only as words"  },
  { OP_NODATA,    'x',      NULL,              "line-regex(p)", "force patterns to match only whole lines" },
  { OP_NODATA,    0,        NULL,               NULL,            NULL }
};

/* Table of names for newline types. Must be kept in step with the definitions
of PCRE2_NEWLINE_xx in pcre2.h. */

static const char *newlines[] = {
  "DEFAULT", "CR", "LF", "CRLF", "ANY", "ANYCRLF", "NUL" };

/* UTF-8 tables - used only when the newline setting is "any". */

const int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

const char utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };



/*************************************************
*         Case-independent string compare        *
*************************************************/

static int
strcmpic(const char *str1, const char *str2)
{
unsigned int c1, c2;
while (*str1 != '\0' || *str2 != '\0')
  {
  c1 = tolower(*str1++);
  c2 = tolower(*str2++);
  if (c1 != c2) return ((c1 > c2) << 1) - 1;
  }
return 0;
}


/*************************************************
*         Parse GREP_COLORS                      *
*************************************************/

/* Extract ms or mt from GREP_COLORS.

Argument:  the string, possibly NULL
Returns:   the value of ms or mt, or NULL if neither present
*/

static char *
parse_grep_colors(const char *gc)
{
static char seq[16];
char *col;
uint32_t len;
if (gc == NULL) return NULL;
col = strstr(gc, "ms=");
if (col == NULL) col = strstr(gc, "mt=");
if (col == NULL) return NULL;
len = 0;
col += 3;
while (*col != ':' && *col != 0 && len < sizeof(seq)-1)
  seq[len++] = *col++;
seq[len] = 0;
return seq;
}


/*************************************************
*         Exit from the program                  *
*************************************************/

/* If there has been a resource error, give a suitable message.

Argument:  the return code
Returns:   does not return
*/

static void
pcre2grep_exit(int rc)
{
if (resource_error)
  {
  fprintf(stderr, "pcre2grep: Error %d, %d, %d or %d means that a resource "
    "limit was exceeded.\n", PCRE2_ERROR_JIT_STACKLIMIT, PCRE2_ERROR_MATCHLIMIT,
    PCRE2_ERROR_DEPTHLIMIT, PCRE2_ERROR_HEAPLIMIT);
  fprintf(stderr, "pcre2grep: Check your regex for nested unlimited loops.\n");
  }
exit(rc);
}


/*************************************************
*          Add item to chain of patterns         *
*************************************************/

/* Used to add an item onto a chain, or just return an unconnected item if the
"after" argument is NULL.

Arguments:
  s          pattern string to add
  after      if not NULL points to item to insert after

Returns:     new pattern block or NULL on error
*/

static patstr *
add_pattern(char *s, patstr *after)
{
patstr *p = (patstr *)malloc(sizeof(patstr));
if (p == NULL)
  {
  fprintf(stderr, "pcre2grep: malloc failed\n");
  pcre2grep_exit(2);
  }
if (strlen(s) > MAXPATLEN)
  {
  fprintf(stderr, "pcre2grep: pattern is too long (limit is %d bytes)\n",
    MAXPATLEN);
  free(p);
  return NULL;
  }
p->next = NULL;
p->string = s;
p->compiled = NULL;

if (after != NULL)
  {
  p->next = after->next;
  after->next = p;
  }
return p;
}


/*************************************************
*           Free chain of patterns               *
*************************************************/

/* Used for several chains of patterns.

Argument: pointer to start of chain
Returns:  nothing
*/

static void
free_pattern_chain(patstr *pc)
{
while (pc != NULL)
  {
  patstr *p = pc;
  pc = p->next;
  if (p->compiled != NULL) pcre2_code_free(p->compiled);
  free(p);
  }
}


/*************************************************
*           Free chain of file names             *
*************************************************/

/*
Argument: pointer to start of chain
Returns:  nothing
*/

static void
free_file_chain(fnstr *fn)
{
while (fn != NULL)
  {
  fnstr *f = fn;
  fn = f->next;
  free(f);
  }
}


/*************************************************
*            OS-specific functions               *
*************************************************/

/* These definitions are needed in all Windows environments, even those where
Unix-style directory scanning can be used (see below). */

#ifdef WIN32

#ifndef STRICT
# define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#define iswild(name) (strpbrk(name, "*?") != NULL)

/* Convert ANSI BGR format to RGB used by Windows */
#define BGR_RGB(x) ((x & 1 ? 4 : 0) | (x & 2) | (x & 4 ? 1 : 0))

static HANDLE hstdout;
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static WORD match_colour;

static WORD
decode_ANSI_colour(const char *cs)
{
WORD result = csbi.wAttributes;
while (*cs)
  {
  if (isdigit(*cs))
    {
    int code = atoi(cs);
    if (code == 1) result |= 0x08;
    else if (code == 4) result |= 0x8000;
    else if (code == 5) result |= 0x80;
    else if (code >= 30 && code <= 37) result = (result & 0xF8) | BGR_RGB(code - 30);
    else if (code == 39) result = (result & 0xF0) | (csbi.wAttributes & 0x0F);
    else if (code >= 40 && code <= 47) result = (result & 0x8F) | (BGR_RGB(code - 40) << 4);
    else if (code == 49) result = (result & 0x0F) | (csbi.wAttributes & 0xF0);
    /* aixterm high intensity colour codes */
    else if (code >= 90 && code <= 97) result = (result & 0xF0) | BGR_RGB(code - 90) | 0x08;
    else if (code >= 100 && code <= 107) result = (result & 0x0F) | (BGR_RGB(code - 100) << 4) | 0x80;

    while (isdigit(*cs)) cs++;
    }
  if (*cs) cs++;
  }
return result;
}


static void
init_colour_output()
{
if (do_colour)
  {
  hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
  /* This fails when redirected to con; try again if so. */
  if (!GetConsoleScreenBufferInfo(hstdout, &csbi) && !do_ansi)
    {
    HANDLE hcon = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    GetConsoleScreenBufferInfo(hcon, &csbi);
    CloseHandle(hcon);
    }
  match_colour = decode_ANSI_colour(colour_string);
  /* No valid colour found - turn off colouring */
  if (!match_colour) do_colour = FALSE;
  }
}

#endif  /* WIN32 */


/* The following sets of functions are defined so that they can be made system
specific. At present there are versions for Unix-style environments, Windows,
native z/OS, and "no support". */


/************* Directory scanning Unix-style and z/OS ***********/

#if (defined HAVE_SYS_STAT_H && defined HAVE_DIRENT_H && defined HAVE_SYS_TYPES_H) || defined NATIVE_ZOS
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#if defined NATIVE_ZOS
/************* Directory and PDS/E scanning for z/OS ***********/
/************* z/OS looks mostly like Unix with USS ************/
/* However, z/OS needs the #include statements in this header */
#include "pcrzosfs.h"
/* That header is not included in the main PCRE distribution because
   other apparatus is needed to compile pcre2grep for z/OS. The header
   can be found in the special z/OS distribution, which is available
   from www.zaconsultants.net or from www.cbttape.org. */
#endif

typedef DIR directory_type;
#define FILESEP '/'

static int
isdirectory(char *filename)
{
struct stat statbuf;
if (stat(filename, &statbuf) < 0)
  return 0;        /* In the expectation that opening as a file will fail */
return S_ISDIR(statbuf.st_mode);
}

static directory_type *
opendirectory(char *filename)
{
return opendir(filename);
}

static char *
readdirectory(directory_type *dir)
{
for (;;)
  {
  struct dirent *dent = readdir(dir);
  if (dent == NULL) return NULL;
  if (strcmp(dent->d_name, ".") != 0 && strcmp(dent->d_name, "..") != 0)
    return dent->d_name;
  }
/* Control never reaches here */
}

static void
closedirectory(directory_type *dir)
{
closedir(dir);
}


/************* Test for regular file, Unix-style **********/

static int
isregfile(char *filename)
{
struct stat statbuf;
if (stat(filename, &statbuf) < 0)
  return 1;        /* In the expectation that opening as a file will fail */
return S_ISREG(statbuf.st_mode);
}


#if defined NATIVE_ZOS
/************* Test for a terminal in z/OS **********/
/* isatty() does not work in a TSO environment, so always give FALSE.*/

static BOOL
is_stdout_tty(void)
{
return FALSE;
}

static BOOL
is_file_tty(FILE *f)
{
return FALSE;
}


/************* Test for a terminal, Unix-style **********/

#else
static BOOL
is_stdout_tty(void)
{
return isatty(fileno(stdout));
}

static BOOL
is_file_tty(FILE *f)
{
return isatty(fileno(f));
}
#endif


/************* Print optionally coloured match Unix-style and z/OS **********/

static void
print_match(const void *buf, int length)
{
if (length == 0) return;
if (do_colour) fprintf(stdout, "%c[%sm", 0x1b, colour_string);
FWRITE_IGNORE(buf, 1, length, stdout);
if (do_colour) fprintf(stdout, "%c[0m", 0x1b);
}

/* End of Unix-style or native z/OS environment functions. */


/************* Directory scanning in Windows ***********/

/* I (Philip Hazel) have no means of testing this code. It was contributed by
Lionel Fourquaux. David Burgess added a patch to define INVALID_FILE_ATTRIBUTES
when it did not exist. David Byron added a patch that moved the #include of
<windows.h> to before the INVALID_FILE_ATTRIBUTES definition rather than after.
*/

#elif defined WIN32

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF
#endif

typedef struct directory_type
{
HANDLE handle;
BOOL first;
WIN32_FIND_DATA data;
} directory_type;

#define FILESEP '/'

int
isdirectory(char *filename)
{
DWORD attr = GetFileAttributes(filename);
if (attr == INVALID_FILE_ATTRIBUTES)
  return 0;
return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

directory_type *
opendirectory(char *filename)
{
size_t len;
char *pattern;
directory_type *dir;
DWORD err;
len = strlen(filename);
pattern = (char *)malloc(len + 3);
dir = (directory_type *)malloc(sizeof(*dir));
if ((pattern == NULL) || (dir == NULL))
  {
  fprintf(stderr, "pcre2grep: malloc failed\n");
  pcre2grep_exit(2);
  }
memcpy(pattern, filename, len);
if (iswild(filename))
  pattern[len] = 0;
else
  memcpy(&(pattern[len]), "\\*", 3);
dir->handle = FindFirstFile(pattern, &(dir->data));
if (dir->handle != INVALID_HANDLE_VALUE)
  {
  free(pattern);
  dir->first = TRUE;
  return dir;
  }
err = GetLastError();
free(pattern);
free(dir);
errno = (err == ERROR_ACCESS_DENIED) ? EACCES : ENOENT;
return NULL;
}

char *
readdirectory(directory_type *dir)
{
for (;;)
  {
  if (!dir->first)
    {
    if (!FindNextFile(dir->handle, &(dir->data)))
      return NULL;
    }
  else
    {
    dir->first = FALSE;
    }
  if (strcmp(dir->data.cFileName, ".") != 0 && strcmp(dir->data.cFileName, "..") != 0)
    return dir->data.cFileName;
  }
#ifndef _MSC_VER
return NULL;   /* Keep compiler happy; never executed */
#endif
}

void
closedirectory(directory_type *dir)
{
FindClose(dir->handle);
free(dir);
}


/************* Test for regular file in Windows **********/

/* I don't know how to do this, or if it can be done; assume all paths are
regular if they are not directories. */

int isregfile(char *filename)
{
return !isdirectory(filename);
}


/************* Test for a terminal in Windows **********/

static BOOL
is_stdout_tty(void)
{
return _isatty(_fileno(stdout));
}

static BOOL
is_file_tty(FILE *f)
{
return _isatty(_fileno(f));
}


/************* Print optionally coloured match in Windows **********/

static void
print_match(const void *buf, int length)
{
if (length == 0) return;
if (do_colour)
  {
  if (do_ansi) fprintf(stdout, "%c[%sm", 0x1b, colour_string);
    else SetConsoleTextAttribute(hstdout, match_colour);
  }
FWRITE_IGNORE(buf, 1, length, stdout);
if (do_colour)
  {
  if (do_ansi) fprintf(stdout, "%c[0m", 0x1b);
    else SetConsoleTextAttribute(hstdout, csbi.wAttributes);
  }
}

/* End of Windows functions */


/************* Directory scanning when we can't do it ***********/

/* The type is void, and apart from isdirectory(), the functions do nothing. */

#else

#define FILESEP 0
typedef void directory_type;

int isdirectory(char *filename) { return 0; }
directory_type * opendirectory(char *filename) { return (directory_type*)0;}
char *readdirectory(directory_type *dir) { return (char*)0;}
void closedirectory(directory_type *dir) {}


/************* Test for regular file when we can't do it **********/

/* Assume all files are regular. */

int isregfile(char *filename) { return 1; }


/************* Test for a terminal when we can't do it **********/

static BOOL
is_stdout_tty(void)
{
return FALSE;
}

static BOOL
is_file_tty(FILE *f)
{
return FALSE;
}


/************* Print optionally coloured match when we can't do it **********/

static void
print_match(const void *buf, int length)
{
if (length == 0) return;
FWRITE_IGNORE(buf, 1, length, stdout);
}

#endif  /* End of system-specific functions */



#ifndef HAVE_STRERROR
/*************************************************
*     Provide strerror() for non-ANSI libraries  *
*************************************************/

/* Some old-fashioned systems still around (e.g. SunOS4) don't have strerror()
in their libraries, but can provide the same facility by this simple
alternative function. */

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
*                Usage function                  *
*************************************************/

static int
usage(int rc)
{
option_item *op;
fprintf(stderr, "Usage: pcre2grep [-");
for (op = optionlist; op->one_char != 0; op++)
  {
  if (op->one_char > 0) fprintf(stderr, "%c", op->one_char);
  }
fprintf(stderr, "] [long options] [pattern] [files]\n");
fprintf(stderr, "Type \"pcre2grep --help\" for more information and the long "
  "options.\n");
return rc;
}



/*************************************************
*                Help function                   *
*************************************************/

static void
help(void)
{
option_item *op;

printf("Usage: pcre2grep [OPTION]... [PATTERN] [FILE1 FILE2 ...]" STDOUT_NL);
printf("Search for PATTERN in each FILE or standard input." STDOUT_NL);
printf("PATTERN must be present if neither -e nor -f is used." STDOUT_NL);

#ifdef SUPPORT_PCRE2GREP_CALLOUT
printf("Callout scripts in patterns are supported." STDOUT_NL);
#else
printf("Callout scripts are not supported in this pcre2grep." STDOUT_NL);
#endif

printf("\"-\" can be used as a file name to mean STDIN." STDOUT_NL);

#ifdef SUPPORT_LIBZ
printf("Files whose names end in .gz are read using zlib." STDOUT_NL);
#endif

#ifdef SUPPORT_LIBBZ2
printf("Files whose names end in .bz2 are read using bzlib2." STDOUT_NL);
#endif

#if defined SUPPORT_LIBZ || defined SUPPORT_LIBBZ2
printf("Other files and the standard input are read as plain files." STDOUT_NL STDOUT_NL);
#else
printf("All files are read as plain files, without any interpretation." STDOUT_NL STDOUT_NL);
#endif

printf("Example: pcre2grep -i " QUOT "hello.*world" QUOT " menu.h main.c" STDOUT_NL STDOUT_NL);
printf("Options:" STDOUT_NL);

for (op = optionlist; op->one_char != 0; op++)
  {
  int n;
  char s[4];

  if (op->one_char > 0 && (op->long_name)[0] == 0)
    n = 31 - printf("  -%c", op->one_char);
  else
    {
    if (op->one_char > 0) sprintf(s, "-%c,", op->one_char);
      else strcpy(s, "   ");
    n = 31 - printf("  %s --%s", s, op->long_name);
    }

  if (n < 1) n = 1;
  printf("%.*s%s" STDOUT_NL, n, "                           ", op->help_text);
  }

printf(STDOUT_NL "Numbers may be followed by K or M, e.g. --max-buffer-size=100K." STDOUT_NL);
printf("The default value for --buffer-size is %d." STDOUT_NL, PCRE2GREP_BUFSIZE);
printf("The default value for --max-buffer-size is %d." STDOUT_NL, PCRE2GREP_MAX_BUFSIZE);
printf("When reading patterns or file names from a file, trailing white" STDOUT_NL);
printf("space is removed and blank lines are ignored." STDOUT_NL);
printf("The maximum size of any pattern is %d bytes." STDOUT_NL, MAXPATLEN);

printf(STDOUT_NL "With no FILEs, read standard input. If fewer than two FILEs given, assume -h." STDOUT_NL);
printf("Exit status is 0 if any matches, 1 if no matches, and 2 if trouble." STDOUT_NL);
}



/*************************************************
*            Test exclude/includes               *
*************************************************/

/* If any exclude pattern matches, the path is excluded. Otherwise, unless
there are no includes, the path must match an include pattern.

Arguments:
  path      the path to be matched
  ip        the chain of include patterns
  ep        the chain of exclude patterns

Returns:    TRUE if the path is not excluded
*/

static BOOL
test_incexc(char *path, patstr *ip, patstr *ep)
{
int plen = strlen((const char *)path);

for (; ep != NULL; ep = ep->next)
  {
  if (pcre2_match(ep->compiled, (PCRE2_SPTR)path, plen, 0, 0, match_data, NULL) >= 0)
    return FALSE;
  }

if (ip == NULL) return TRUE;

for (; ip != NULL; ip = ip->next)
  {
  if (pcre2_match(ip->compiled, (PCRE2_SPTR)path, plen, 0, 0, match_data, NULL) >= 0)
    return TRUE;
  }

return FALSE;
}



/*************************************************
*         Decode integer argument value          *
*************************************************/

/* Integer arguments can be followed by K or M. Avoid the use of strtoul()
because SunOS4 doesn't have it. This is used only for unpicking arguments, so
just keep it simple.

Arguments:
  option_data   the option data string
  op            the option item (for error messages)
  longop        TRUE if option given in long form

Returns:        a long integer
*/

static long int
decode_number(char *option_data, option_item *op, BOOL longop)
{
unsigned long int n = 0;
char *endptr = option_data;
while (*endptr != 0 && isspace((unsigned char)(*endptr))) endptr++;
while (isdigit((unsigned char)(*endptr)))
  n = n * 10 + (int)(*endptr++ - '0');
if (toupper(*endptr) == 'K')
  {
  n *= 1024;
  endptr++;
  }
else if (toupper(*endptr) == 'M')
  {
  n *= 1024*1024;
  endptr++;
  }

if (*endptr != 0)   /* Error */
  {
  if (longop)
    {
    char *equals = strchr(op->long_name, '=');
    int nlen = (equals == NULL)? (int)strlen(op->long_name) :
      (int)(equals - op->long_name);
    fprintf(stderr, "pcre2grep: Malformed number \"%s\" after --%.*s\n",
      option_data, nlen, op->long_name);
    }
  else
    fprintf(stderr, "pcre2grep: Malformed number \"%s\" after -%c\n",
      option_data, op->one_char);
  pcre2grep_exit(usage(2));
  }

return n;
}



/*************************************************
*       Add item to a chain of numbers           *
*************************************************/

/* Used to add an item onto a chain, or just return an unconnected item if the
"after" argument is NULL.

Arguments:
  n          the number to add
  after      if not NULL points to item to insert after

Returns:     new number block
*/

static omstr *
add_number(int n, omstr *after)
{
omstr *om = (omstr *)malloc(sizeof(omstr));

if (om == NULL)
  {
  fprintf(stderr, "pcre2grep: malloc failed\n");
  pcre2grep_exit(2);
  }
om->next = NULL;
om->groupnum = n;

if (after != NULL)
  {
  om->next = after->next;
  after->next = om;
  }
return om;
}



/*************************************************
*            Read one line of input              *
*************************************************/

/* Normally, input is read using fread() (or gzread, or BZ2_read) into a large
buffer, so many lines may be read at once. However, doing this for tty input
means that no output appears until a lot of input has been typed. Instead, tty
input is handled line by line. We cannot use fgets() for this, because it does
not stop at a binary zero, and therefore there is no way of telling how many
characters it has read, because there may be binary zeros embedded in the data.

Arguments:
  buffer     the buffer to read into
  length     the maximum number of characters to read
  f          the file

Returns:     the number of characters read, zero at end of file
*/

static unsigned int
read_one_line(char *buffer, int length, FILE *f)
{
int c;
int yield = 0;
while ((c = fgetc(f)) != EOF)
  {
  buffer[yield++] = c;
  if (c == '\n' || yield >= length) break;
  }
return yield;
}



/*************************************************
*             Find end of line                   *
*************************************************/

/* The length of the endline sequence that is found is set via lenptr. This may
be zero at the very end of the file if there is no line-ending sequence there.

Arguments:
  p         current position in line
  endptr    end of available data
  lenptr    where to put the length of the eol sequence

Returns:    pointer after the last byte of the line,
            including the newline byte(s)
*/

static char *
end_of_line(char *p, char *endptr, int *lenptr)
{
switch(endlinetype)
  {
  default:      /* Just in case */
  case PCRE2_NEWLINE_LF:
  while (p < endptr && *p != '\n') p++;
  if (p < endptr)
    {
    *lenptr = 1;
    return p + 1;
    }
  *lenptr = 0;
  return endptr;

  case PCRE2_NEWLINE_CR:
  while (p < endptr && *p != '\r') p++;
  if (p < endptr)
    {
    *lenptr = 1;
    return p + 1;
    }
  *lenptr = 0;
  return endptr;

  case PCRE2_NEWLINE_NUL:
  while (p < endptr && *p != '\0') p++;
  if (p < endptr)
    {
    *lenptr = 1;
    return p + 1;
    }
  *lenptr = 0;
  return endptr;

  case PCRE2_NEWLINE_CRLF:
  for (;;)
    {
    while (p < endptr && *p != '\r') p++;
    if (++p >= endptr)
      {
      *lenptr = 0;
      return endptr;
      }
    if (*p == '\n')
      {
      *lenptr = 2;
      return p + 1;
      }
    }
  break;

  case PCRE2_NEWLINE_ANYCRLF:
  while (p < endptr)
    {
    int extra = 0;
    int c = *((unsigned char *)p);

    if (utf && c >= 0xc0)
      {
      int gcii, gcss;
      extra = utf8_table4[c & 0x3f];  /* Number of additional bytes */
      gcss = 6*extra;
      c = (c & utf8_table3[extra]) << gcss;
      for (gcii = 1; gcii <= extra; gcii++)
        {
        gcss -= 6;
        c |= (p[gcii] & 0x3f) << gcss;
        }
      }

    p += 1 + extra;

    switch (c)
      {
      case '\n':
      *lenptr = 1;
      return p;

      case '\r':
      if (p < endptr && *p == '\n')
        {
        *lenptr = 2;
        p++;
        }
      else *lenptr = 1;
      return p;

      default:
      break;
      }
    }   /* End of loop for ANYCRLF case */

  *lenptr = 0;  /* Must have hit the end */
  return endptr;

  case PCRE2_NEWLINE_ANY:
  while (p < endptr)
    {
    int extra = 0;
    int c = *((unsigned char *)p);

    if (utf && c >= 0xc0)
      {
      int gcii, gcss;
      extra = utf8_table4[c & 0x3f];  /* Number of additional bytes */
      gcss = 6*extra;
      c = (c & utf8_table3[extra]) << gcss;
      for (gcii = 1; gcii <= extra; gcii++)
        {
        gcss -= 6;
        c |= (p[gcii] & 0x3f) << gcss;
        }
      }

    p += 1 + extra;

    switch (c)
      {
      case '\n':    /* LF */
      case '\v':    /* VT */
      case '\f':    /* FF */
      *lenptr = 1;
      return p;

      case '\r':    /* CR */
      if (p < endptr && *p == '\n')
        {
        *lenptr = 2;
        p++;
        }
      else *lenptr = 1;
      return p;

#ifndef EBCDIC
      case 0x85:    /* Unicode NEL */
      *lenptr = utf? 2 : 1;
      return p;

      case 0x2028:  /* Unicode LS */
      case 0x2029:  /* Unicode PS */
      *lenptr = 3;
      return p;
#endif  /* Not EBCDIC */

      default:
      break;
      }
    }   /* End of loop for ANY case */

  *lenptr = 0;  /* Must have hit the end */
  return endptr;
  }     /* End of overall switch */
}



/*************************************************
*         Find start of previous line            *
*************************************************/

/* This is called when looking back for before lines to print.

Arguments:
  p         start of the subsequent line
  startptr  start of available data

Returns:    pointer to the start of the previous line
*/

static char *
previous_line(char *p, char *startptr)
{
switch(endlinetype)
  {
  default:      /* Just in case */
  case PCRE2_NEWLINE_LF:
  p--;
  while (p > startptr && p[-1] != '\n') p--;
  return p;

  case PCRE2_NEWLINE_CR:
  p--;
  while (p > startptr && p[-1] != '\n') p--;
  return p;

  case PCRE2_NEWLINE_NUL:
  p--;
  while (p > startptr && p[-1] != '\0') p--;
  return p;

  case PCRE2_NEWLINE_CRLF:
  for (;;)
    {
    p -= 2;
    while (p > startptr && p[-1] != '\n') p--;
    if (p <= startptr + 1 || p[-2] == '\r') return p;
    }
  /* Control can never get here */

  case PCRE2_NEWLINE_ANY:
  case PCRE2_NEWLINE_ANYCRLF:
  if (*(--p) == '\n' && p > startptr && p[-1] == '\r') p--;
  if (utf) while ((*p & 0xc0) == 0x80) p--;

  while (p > startptr)
    {
    unsigned int c;
    char *pp = p - 1;

    if (utf)
      {
      int extra = 0;
      while ((*pp & 0xc0) == 0x80) pp--;
      c = *((unsigned char *)pp);
      if (c >= 0xc0)
        {
        int gcii, gcss;
        extra = utf8_table4[c & 0x3f];  /* Number of additional bytes */
        gcss = 6*extra;
        c = (c & utf8_table3[extra]) << gcss;
        for (gcii = 1; gcii <= extra; gcii++)
          {
          gcss -= 6;
          c |= (pp[gcii] & 0x3f) << gcss;
          }
        }
      }
    else c = *((unsigned char *)pp);

    if (endlinetype == PCRE2_NEWLINE_ANYCRLF) switch (c)
      {
      case '\n':    /* LF */
      case '\r':    /* CR */
      return p;

      default:
      break;
      }

    else switch (c)
      {
      case '\n':    /* LF */
      case '\v':    /* VT */
      case '\f':    /* FF */
      case '\r':    /* CR */
#ifndef EBCDIC
      case 0x85:    /* Unicode NEL */
      case 0x2028:  /* Unicode LS */
      case 0x2029:  /* Unicode PS */
#endif  /* Not EBCDIC */
      return p;

      default:
      break;
      }

    p = pp;  /* Back one character */
    }        /* End of loop for ANY case */

  return startptr;  /* Hit start of data */
  }     /* End of overall switch */
}



/*************************************************
*       Print the previous "after" lines         *
*************************************************/

/* This is called if we are about to lose said lines because of buffer filling,
and at the end of the file. The data in the line is written using fwrite() so
that a binary zero does not terminate it.

Arguments:
  lastmatchnumber   the number of the last matching line, plus one
  lastmatchrestart  where we restarted after the last match
  endptr            end of available data
  printname         filename for printing

Returns:            nothing
*/

static void
do_after_lines(int lastmatchnumber, char *lastmatchrestart, char *endptr,
  const char *printname)
{
if (after_context > 0 && lastmatchnumber > 0)
  {
  int count = 0;
  while (lastmatchrestart < endptr && count < after_context)
    {
    int ellength;
    char *pp = end_of_line(lastmatchrestart, endptr, &ellength);
    if (ellength == 0 && pp == main_buffer + bufsize) break;
    if (printname != NULL) fprintf(stdout, "%s-", printname);
    if (number) fprintf(stdout, "%d-", lastmatchnumber++);
    FWRITE_IGNORE(lastmatchrestart, 1, pp - lastmatchrestart, stdout);
    lastmatchrestart = pp;
    count++;
    }
  if (count > 0) hyphenpending = TRUE;
  }
}



/*************************************************
*   Apply patterns to subject till one matches   *
*************************************************/

/* This function is called to run through all patterns, looking for a match. It
is used multiple times for the same subject when colouring is enabled, in order
to find all possible matches.

Arguments:
  matchptr     the start of the subject
  length       the length of the subject to match
  options      options for pcre_exec
  startoffset  where to start matching
  mrc          address of where to put the result of pcre2_match()

Returns:      TRUE if there was a match
              FALSE if there was no match
              invert if there was a non-fatal error
*/

static BOOL
match_patterns(char *matchptr, size_t length, unsigned int options,
  size_t startoffset, int *mrc)
{
int i;
size_t slen = length;
patstr *p = patterns;
const char *msg = "this text:\n\n";

if (slen > 200)
  {
  slen = 200;
  msg = "text that starts:\n\n";
  }
for (i = 1; p != NULL; p = p->next, i++)
  {
  *mrc = pcre2_match(p->compiled, (PCRE2_SPTR)matchptr, (int)length,
    startoffset, options, match_data, match_context);
  if (*mrc >= 0) return TRUE;
  if (*mrc == PCRE2_ERROR_NOMATCH) continue;
  fprintf(stderr, "pcre2grep: pcre2_match() gave error %d while matching ", *mrc);
  if (patterns->next != NULL) fprintf(stderr, "pattern number %d to ", i);
  fprintf(stderr, "%s", msg);
  FWRITE_IGNORE(matchptr, 1, slen, stderr);   /* In case binary zero included */
  fprintf(stderr, "\n\n");
  if (*mrc == PCRE2_ERROR_MATCHLIMIT || *mrc == PCRE2_ERROR_DEPTHLIMIT ||
      *mrc == PCRE2_ERROR_HEAPLIMIT || *mrc == PCRE2_ERROR_JIT_STACKLIMIT)
    resource_error = TRUE;
  if (error_count++ > 20)
    {
    fprintf(stderr, "pcre2grep: Too many errors - abandoned.\n");
    pcre2grep_exit(2);
    }
  return invert;    /* No more matching; don't show the line again */
  }

return FALSE;  /* No match, no errors */
}


/*************************************************
*          Check output text for errors          *
*************************************************/

static BOOL
syntax_check_output_text(PCRE2_SPTR string, BOOL callout)
{
PCRE2_SPTR begin = string;
for (; *string != 0; string++)
  {
  if (*string == '$')
    {
    PCRE2_SIZE capture_id = 0;
    BOOL brace = FALSE;

    string++;

    /* Syntax error: a character must be present after $. */
    if (*string == 0)
      {
      if (!callout)
        fprintf(stderr, "pcre2grep: Error in output text at offset %d: %s\n",
          (int)(string - begin), "no character after $");
      return FALSE;
      }

    if (*string == '{')
      {
      /* Must be a decimal number in braces, e.g: {5} or {38} */
      string++;

      brace = TRUE;
      }

    if ((*string >= '1' && *string <= '9') || (!callout && *string == '0'))
      {
      do
        {
        /* Maximum capture id is 65535. */
        if (capture_id <= 65535)
          capture_id = capture_id * 10 + (*string - '0');

        string++;
        }
      while (*string >= '0' && *string <= '9');

      if (brace)
        {
        /* Syntax error: closing brace is missing. */
        if (*string != '}')
          {
          if (!callout)
            fprintf(stderr, "pcre2grep: Error in output text at offset %d: %s\n",
              (int)(string - begin), "missing closing brace");
          return FALSE;
          }
        }
      else
        {
        /* To negate the effect of the for. */
        string--;
        }
      }
    else if (brace)
      {
      /* Syntax error: a decimal number required. */
      if (!callout)
        fprintf(stderr, "pcre2grep: Error in output text at offset %d: %s\n",
          (int)(string - begin), "decimal number expected");
      return FALSE;
      }
    else if (*string == 'o')
      {
      string++;

      if (*string < '0' || *string > '7')
        {
        /* Syntax error: an octal number required. */
        if (!callout)
          fprintf(stderr, "pcre2grep: Error in output text at offset %d: %s\n",
            (int)(string - begin), "octal number expected");
        return FALSE;
        }
      }
    else if (*string == 'x')
      {
      string++;

      if (!isxdigit((unsigned char)*string))
        {
        /* Syntax error: a hexdecimal number required. */
        if (!callout)
          fprintf(stderr, "pcre2grep: Error in output text at offset %d: %s\n",
            (int)(string - begin), "hexadecimal number expected");
        return FALSE;
        }
      }
    }
  }

  return TRUE;
}


/*************************************************
*              Display output text               *
*************************************************/

/* Display the output text, which is assumed to have already been syntax
checked. Output may contain escape sequences started by the dollar sign. The
escape sequences are substituted as follows:

  $<digits> or ${<digits>} is replaced by the captured substring of the given
  decimal number; zero will substitute the whole match. If the number is
  greater than the number of capturing substrings, or if the capture is unset,
  the replacement is empty.

  $a is replaced by bell.
  $b is replaced by backspace.
  $e is replaced by escape.
  $f is replaced by form feed.
  $n is replaced by newline.
  $r is replaced by carriage return.
  $t is replaced by tab.
  $v is replaced by vertical tab.

  $o<digits> is replaced by the character represented by the given octal
  number; up to three digits are processed.

  $x<digits> is replaced by the character represented by the given hexadecimal
  number; up to two digits are processed.

  Any other character is substituted by itself. E.g: $$ is replaced by a single
  dollar.

Arguments:
  string:       the output text
  callout:      TRUE for the builtin callout, FALSE for --output
  subject       the start of the subject
  ovector:      capture offsets
  capture_top:  number of captures

Returns:        TRUE if something was output, other than newline
                FALSE if nothing was output, or newline was last output
*/

static BOOL
display_output_text(PCRE2_SPTR string, BOOL callout, PCRE2_SPTR subject,
  PCRE2_SIZE *ovector, PCRE2_SIZE capture_top)
{
BOOL printed = FALSE;

for (; *string != 0; string++)
  {
  int ch = EOF;
  if (*string == '$')
    {
    PCRE2_SIZE capture_id = 0;
    BOOL brace = FALSE;

    string++;

    if (*string == '{')
      {
      /* Must be a decimal number in braces, e.g: {5} or {38} */
      string++;

      brace = TRUE;
      }

    if ((*string >= '1' && *string <= '9') || (!callout && *string == '0'))
      {
      do
        {
        /* Maximum capture id is 65535. */
        if (capture_id <= 65535)
          capture_id = capture_id * 10 + (*string - '0');

        string++;
        }
      while (*string >= '0' && *string <= '9');

      if (!brace)
        {
        /* To negate the effect of the for. */
        string--;
        }

      if (capture_id < capture_top)
        {
        PCRE2_SIZE capturesize;
        capture_id *= 2;

        capturesize = ovector[capture_id + 1] - ovector[capture_id];
        if (capturesize > 0)
          {
          print_match(subject + ovector[capture_id], capturesize);
          printed = TRUE;
          }
        }
      }
    else if (*string == 'a') ch = '\a';
    else if (*string == 'b') ch = '\b';
#ifndef EBCDIC
    else if (*string == 'e') ch = '\033';
#else
    else if (*string == 'e') ch = '\047';
#endif
    else if (*string == 'f') ch = '\f';
    else if (*string == 'r') ch = '\r';
    else if (*string == 't') ch = '\t';
    else if (*string == 'v') ch = '\v';
    else if (*string == 'n')
      {
      fprintf(stdout, STDOUT_NL);
      printed = FALSE;
      }
    else if (*string == 'o')
      {
      string++;

      ch = *string - '0';
      if (string[1] >= '0' && string[1] <= '7')
        {
        string++;
        ch = ch * 8 + (*string - '0');
        }
      if (string[1] >= '0' && string[1] <= '7')
        {
        string++;
        ch = ch * 8 + (*string - '0');
        }
      }
    else if (*string == 'x')
      {
      string++;

      if (*string >= '0' && *string <= '9')
        ch = *string - '0';
      else
        ch = (*string | 0x20) - 'a' + 10;
      if (isxdigit((unsigned char)string[1]))
        {
        string++;
        ch *= 16;
        if (*string >= '0' && *string <= '9')
          ch += *string - '0';
        else
          ch += (*string | 0x20) - 'a' + 10;
        }
      }
    else
      {
      ch = *string;
      }
    }
  else
    {
    ch = *string;
    }
  if (ch != EOF)
    {
    fprintf(stdout, "%c", ch);
    printed = TRUE;
    }
  }

return printed;
}


#ifdef SUPPORT_PCRE2GREP_CALLOUT

/*************************************************
*        Parse and execute callout scripts       *
*************************************************/

/* This function parses a callout string block and executes the
program specified by the string. The string is a list of substrings
separated by pipe characters. The first substring represents the
executable name, and the following substrings specify the arguments:

  program_name|param1|param2|...

Any substring (including the program name) can contain escape sequences
started by the dollar character. The escape sequences are substituted as
follows:

  $<digits> or ${<digits>} is replaced by the captured substring of the given
  decimal number, which must be greater than zero. If the number is greater
  than the number of capturing substrings, or if the capture is unset, the
  replacement is empty.

  Any other character is substituted by itself. E.g: $$ is replaced by a single
  dollar or $| replaced by a pipe character.

Alternatively, if string starts with pipe, the remainder is taken as an output
string, same as --output. In this case, --om-separator is used to separate each
callout, defaulting to newline.

Example:

  echo -e "abcde\n12345" | pcre2grep \
    '(.)(..(.))(?C"/bin/echo|Arg1: [$1] [$2] [$3]|Arg2: $|${1}$| ($4)")()' -

  Output:

    Arg1: [a] [bcd] [d] Arg2: |a| ()
    abcde
    Arg1: [1] [234] [4] Arg2: |1| ()
    12345

Arguments:
  blockptr     the callout block

Returns:       currently it always returns with 0
*/

static int
pcre2grep_callout(pcre2_callout_block *calloutptr, void *unused)
{
PCRE2_SIZE length = calloutptr->callout_string_length;
PCRE2_SPTR string = calloutptr->callout_string;
PCRE2_SPTR subject = calloutptr->subject;
PCRE2_SIZE *ovector = calloutptr->offset_vector;
PCRE2_SIZE capture_top = calloutptr->capture_top;
PCRE2_SIZE argsvectorlen = 2;
PCRE2_SIZE argslen = 1;
char *args;
char *argsptr;
char **argsvector;
char **argsvectorptr;
#ifndef WIN32
pid_t pid;
#endif
int result = 0;

(void)unused;   /* Avoid compiler warning */

/* Only callout with strings are supported. */
if (string == NULL || length == 0) return 0;

/* If there's no command, output the remainder directly. */

if (*string == '|')
  {
  string++;
  if (!syntax_check_output_text(string, TRUE)) return 0;
  (void)display_output_text(string, TRUE, subject, ovector, capture_top);
  return 0;
  }

/* Checking syntax and compute the number of string fragments. Callout strings
are ignored in case of a syntax error. */

while (length > 0)
  {
  if (*string == '|')
    {
    argsvectorlen++;

    /* Maximum 10000 arguments allowed. */
    if (argsvectorlen > 10000) return 0;
    }
  else if (*string == '$')
    {
    PCRE2_SIZE capture_id = 0;

    string++;
    length--;

    /* Syntax error: a character must be present after $. */
    if (length == 0) return 0;

    if (*string >= '1' && *string <= '9')
      {
      do
        {
        /* Maximum capture id is 65535. */
        if (capture_id <= 65535)
          capture_id = capture_id * 10 + (*string - '0');

        string++;
        length--;
        }
      while (length > 0 && *string >= '0' && *string <= '9');

      /* To negate the effect of string++ below. */
      string--;
      length++;
      }
    else if (*string == '{')
      {
      /* Must be a decimal number in braces, e.g: {5} or {38} */
      string++;
      length--;

      /* Syntax error: a decimal number required. */
      if (length == 0) return 0;
      if (*string < '1' || *string > '9') return 0;

      do
        {
        /* Maximum capture id is 65535. */
        if (capture_id <= 65535)
          capture_id = capture_id * 10 + (*string - '0');

        string++;
        length--;

        /* Syntax error: no more characters */
        if (length == 0) return 0;
        }
      while (*string >= '0' && *string <= '9');

      /* Syntax error: closing brace is missing. */
      if (*string != '}') return 0;
      }

    if (capture_id > 0)
      {
      if (capture_id < capture_top)
        {
        capture_id *= 2;
        argslen += ovector[capture_id + 1] - ovector[capture_id];
        }

      /* To negate the effect of argslen++ below. */
      argslen--;
      }
    }

  string++;
  length--;
  argslen++;
  }

args = (char*)malloc(argslen);
if (args == NULL) return 0;

argsvector = (char**)malloc(argsvectorlen * sizeof(char*));
if (argsvector == NULL)
  {
  free(args);
  return 0;
  }

argsptr = args;
argsvectorptr = argsvector;

*argsvectorptr++ = argsptr;

length = calloutptr->callout_string_length;
string = calloutptr->callout_string;

while (length > 0)
  {
  if (*string == '|')
    {
    *argsptr++ = '\0';
    *argsvectorptr++ = argsptr;
    }
  else if (*string == '$')
    {
    string++;
    length--;

    if ((*string >= '1' && *string <= '9') || *string == '{')
      {
      PCRE2_SIZE capture_id = 0;

      if (*string != '{')
        {
        do
          {
          /* Maximum capture id is 65535. */
          if (capture_id <= 65535)
            capture_id = capture_id * 10 + (*string - '0');

          string++;
          length--;
          }
        while (length > 0 && *string >= '0' && *string <= '9');

        /* To negate the effect of string++ below. */
        string--;
        length++;
        }
      else
        {
        string++;
        length--;

        do
          {
          /* Maximum capture id is 65535. */
          if (capture_id <= 65535)
            capture_id = capture_id * 10 + (*string - '0');

          string++;
          length--;
          }
        while (*string != '}');
        }

        if (capture_id < capture_top)
          {
          PCRE2_SIZE capturesize;
          capture_id *= 2;

          capturesize = ovector[capture_id + 1] - ovector[capture_id];
          memcpy(argsptr, subject + ovector[capture_id], capturesize);
          argsptr += capturesize;
          }
      }
    else
      {
      *argsptr++ = *string;
      }
    }
  else
    {
    *argsptr++ = *string;
    }

  string++;
  length--;
  }

*argsptr++ = '\0';
*argsvectorptr = NULL;

#ifdef WIN32
result = _spawnvp(_P_WAIT, argsvector[0], (const char * const *)argsvector);
#else
pid = fork();

if (pid == 0)
  {
  (void)execv(argsvector[0], argsvector);
  /* Control gets here if there is an error, e.g. a non-existent program */
  exit(1);
  }
else if (pid > 0)
  (void)waitpid(pid, &result, 0);
#endif

free(args);
free(argsvector);

/* Currently negative return values are not supported, only zero (match
continues) or non-zero (match fails). */

return result != 0;
}

#endif



/*************************************************
*     Read a portion of the file into buffer     *
*************************************************/

static int
fill_buffer(void *handle, int frtype, char *buffer, int length,
  BOOL input_line_buffered)
{
(void)frtype;  /* Avoid warning when not used */

#ifdef SUPPORT_LIBZ
if (frtype == FR_LIBZ)
  return gzread((gzFile)handle, buffer, length);
else
#endif

#ifdef SUPPORT_LIBBZ2
if (frtype == FR_LIBBZ2)
  return BZ2_bzread((BZFILE *)handle, buffer, length);
else
#endif

return (input_line_buffered ?
  read_one_line(buffer, length, (FILE *)handle) :
  fread(buffer, 1, length, (FILE *)handle));
}



/*************************************************
*            Grep an individual file             *
*************************************************/

/* This is called from grep_or_recurse() below. It uses a buffer that is three
times the value of bufthird. The matching point is never allowed to stray into
the top third of the buffer, thus keeping more of the file available for
context printing or for multiline scanning. For large files, the pointer will
be in the middle third most of the time, so the bottom third is available for
"before" context printing.

Arguments:
  handle       the fopened FILE stream for a normal file
               the gzFile pointer when reading is via libz
               the BZFILE pointer when reading is via libbz2
  frtype       FR_PLAIN, FR_LIBZ, or FR_LIBBZ2
  filename     the file name or NULL (for errors)
  printname    the file name if it is to be printed for each match
               or NULL if the file name is not to be printed
               it cannot be NULL if filenames[_nomatch]_only is set

Returns:       0 if there was at least one match
               1 otherwise (no matches)
               2 if an overlong line is encountered
               3 if there is a read error on a .bz2 file
*/

static int
pcre2grep(void *handle, int frtype, const char *filename, const char *printname)
{
int rc = 1;
int linenumber = 1;
int lastmatchnumber = 0;
int count = 0;
int filepos = 0;
char *lastmatchrestart = NULL;
char *ptr = main_buffer;
char *endptr;
size_t bufflength;
BOOL binary = FALSE;
BOOL endhyphenpending = FALSE;
BOOL input_line_buffered = line_buffered;
FILE *in = NULL;                    /* Ensure initialized */

/* Do the first read into the start of the buffer and set up the pointer to end
of what we have. In the case of libz, a non-zipped .gz file will be read as a
plain file. However, if a .bz2 file isn't actually bzipped, the first read will
fail. */

if (frtype != FR_LIBZ && frtype != FR_LIBBZ2)
  {
  in = (FILE *)handle;
  if (is_file_tty(in)) input_line_buffered = TRUE;
  }

bufflength = fill_buffer(handle, frtype, main_buffer, bufsize,
  input_line_buffered);

#ifdef SUPPORT_LIBBZ2
if (frtype == FR_LIBBZ2 && (int)bufflength < 0) return 2;   /* Gotcha: bufflength is size_t; */
#endif

endptr = main_buffer + bufflength;

/* Unless binary-files=text, see if we have a binary file. This uses the same
rule as GNU grep, namely, a search for a binary zero byte near the start of the
file. However, when the newline convention is binary zero, we can't do this. */

if (binary_files != BIN_TEXT)
  {
  if (endlinetype != PCRE2_NEWLINE_NUL)
    binary = memchr(main_buffer, 0, (bufflength > 1024)? 1024 : bufflength)
      != NULL;
  if (binary && binary_files == BIN_NOMATCH) return 1;
  }

/* Loop while the current pointer is not at the end of the file. For large
files, endptr will be at the end of the buffer when we are in the middle of the
file, but ptr will never get there, because as soon as it gets over 2/3 of the
way, the buffer is shifted left and re-filled. */

while (ptr < endptr)
  {
  int endlinelength;
  int mrc = 0;
  unsigned int options = 0;
  BOOL match;
  char *matchptr = ptr;
  char *t = ptr;
  size_t length, linelength;
  size_t startoffset = 0;

  /* At this point, ptr is at the start of a line. We need to find the length
  of the subject string to pass to pcre2_match(). In multiline mode, it is the
  length remainder of the data in the buffer. Otherwise, it is the length of
  the next line, excluding the terminating newline. After matching, we always
  advance by the length of the next line. In multiline mode the PCRE2_FIRSTLINE
  option is used for compiling, so that any match is constrained to be in the
  first line. */

  t = end_of_line(t, endptr, &endlinelength);
  linelength = t - ptr - endlinelength;
  length = multiline? (size_t)(endptr - ptr) : linelength;

  /* Check to see if the line we are looking at extends right to the very end
  of the buffer without a line terminator. This means the line is too long to
  handle at the current buffer size. Until the buffer reaches its maximum size,
  try doubling it and reading more data. */

  if (endlinelength == 0 && t == main_buffer + bufsize)
    {
    if (bufthird < max_bufthird)
      {
      char *new_buffer;
      int new_bufthird = 2*bufthird;

      if (new_bufthird > max_bufthird) new_bufthird = max_bufthird;
      new_buffer = (char *)malloc(3*new_bufthird);

      if (new_buffer == NULL)
        {
        fprintf(stderr,
          "pcre2grep: line %d%s%s is too long for the internal buffer\n"
          "pcre2grep: not enough memory to increase the buffer size to %d\n",
          linenumber,
          (filename == NULL)? "" : " of file ",
          (filename == NULL)? "" : filename,
          new_bufthird);
        return 2;
        }

      /* Copy the data and adjust pointers to the new buffer location. */

      memcpy(new_buffer, main_buffer, bufsize);
      bufthird = new_bufthird;
      bufsize = 3*bufthird;
      ptr = new_buffer + (ptr - main_buffer);
      lastmatchrestart = new_buffer + (lastmatchrestart - main_buffer);
      free(main_buffer);
      main_buffer = new_buffer;

      /* Read more data into the buffer and then try to find the line ending
      again. */

      bufflength += fill_buffer(handle, frtype, main_buffer + bufflength,
        bufsize - bufflength, input_line_buffered);
      endptr = main_buffer + bufflength;
      continue;
      }
    else
      {
      fprintf(stderr,
        "pcre2grep: line %d%s%s is too long for the internal buffer\n"
        "pcre2grep: the maximum buffer size is %d\n"
        "pcre2grep: use the --max-buffer-size option to change it\n",
        linenumber,
        (filename == NULL)? "" : " of file ",
        (filename == NULL)? "" : filename,
        bufthird);
      return 2;
      }
    }

  /* Extra processing for Jeffrey Friedl's debugging. */

#ifdef JFRIEDL_DEBUG
  if (jfriedl_XT || jfriedl_XR)
  {
#     include <sys/time.h>
#     include <time.h>
      struct timeval start_time, end_time;
      struct timezone dummy;
      int i;

      if (jfriedl_XT)
      {
          unsigned long newlen = length * jfriedl_XT + strlen(jfriedl_prefix) + strlen(jfriedl_postfix);
          const char *orig = ptr;
          ptr = malloc(newlen + 1);
          if (!ptr) {
                  printf("out of memory");
                  pcre2grep_exit(2);
          }
          endptr = ptr;
          strcpy(endptr, jfriedl_prefix); endptr += strlen(jfriedl_prefix);
          for (i = 0; i < jfriedl_XT; i++) {
                  strncpy(endptr, orig,  length);
                  endptr += length;
          }
          strcpy(endptr, jfriedl_postfix); endptr += strlen(jfriedl_postfix);
          length = newlen;
      }

      if (gettimeofday(&start_time, &dummy) != 0)
              perror("bad gettimeofday");


      for (i = 0; i < jfriedl_XR; i++)
          match = (pcre_exec(patterns->compiled, patterns->hint, ptr, length, 0,
              PCRE2_NOTEMPTY, offsets, OFFSET_SIZE) >= 0);

      if (gettimeofday(&end_time, &dummy) != 0)
              perror("bad gettimeofday");

      double delta = ((end_time.tv_sec + (end_time.tv_usec / 1000000.0))
                      -
                      (start_time.tv_sec + (start_time.tv_usec / 1000000.0)));

      printf("%s TIMER[%.4f]\n", match ? "MATCH" : "FAIL", delta);
      return 0;
  }
#endif

  /* We come back here after a match when only_matching_count is non-zero, in
  order to find any further matches in the same line. This applies to
  --only-matching, --file-offsets, and --line-offsets. */

  ONLY_MATCHING_RESTART:

  /* Run through all the patterns until one matches or there is an error other
  than NOMATCH. This code is in a subroutine so that it can be re-used for
  finding subsequent matches when colouring matched lines. After finding one
  match, set PCRE2_NOTEMPTY to disable any further matches of null strings in
  this line. */

  match = match_patterns(matchptr, length, options, startoffset, &mrc);
  options = PCRE2_NOTEMPTY;

  /* If it's a match or a not-match (as required), do what's wanted. */

  if (match != invert)
    {
    BOOL hyphenprinted = FALSE;

    /* We've failed if we want a file that doesn't have any matches. */

    if (filenames == FN_NOMATCH_ONLY) return 1;

    /* If all we want is a yes/no answer, we can return immediately. */

    if (quiet) return 0;

    /* Just count if just counting is wanted. */

    else if (count_only || show_total_count) count++;

    /* When handling a binary file and binary-files==binary, the "binary"
    variable will be set true (it's false in all other cases). In this
    situation we just want to output the file name. No need to scan further. */

    else if (binary)
      {
      fprintf(stdout, "Binary file %s matches" STDOUT_NL, filename);
      return 0;
      }

    /* Likewise, if all we want is a file name, there is no need to scan any
    more lines in the file. */

    else if (filenames == FN_MATCH_ONLY)
      {
      fprintf(stdout, "%s" STDOUT_NL, printname);
      return 0;
      }

    /* The --only-matching option prints just the substring that matched,
    and/or one or more captured portions of it, as long as these strings are
    not empty. The --file-offsets and --line-offsets options output offsets for
    the matching substring (all three set only_matching_count non-zero). None
    of these mutually exclusive options prints any context. Afterwards, adjust
    the start and then jump back to look for further matches in the same line.
    If we are in invert mode, however, nothing is printed and we do not restart
    - this could still be useful because the return code is set. */

    else if (only_matching_count != 0)
      {
      if (!invert)
        {
        size_t oldstartoffset;

        if (printname != NULL) fprintf(stdout, "%s:", printname);
        if (number) fprintf(stdout, "%d:", linenumber);

        /* Handle --line-offsets */

        if (line_offsets)
          fprintf(stdout, "%d,%d" STDOUT_NL, (int)(matchptr + offsets[0] - ptr),
            (int)(offsets[1] - offsets[0]));

        /* Handle --file-offsets */

        else if (file_offsets)
          fprintf(stdout, "%d,%d" STDOUT_NL,
            (int)(filepos + matchptr + offsets[0] - ptr),
            (int)(offsets[1] - offsets[0]));

        /* Handle --output (which has already been syntax checked) */

        else if (output_text != NULL)
          {
          if (display_output_text((PCRE2_SPTR)output_text, FALSE,
              (PCRE2_SPTR)matchptr, offsets, mrc) || printname != NULL ||
              number)
            fprintf(stdout, STDOUT_NL);
          }

        /* Handle --only-matching, which may occur many times */

        else
          {
          BOOL printed = FALSE;
          omstr *om;

          for (om = only_matching; om != NULL; om = om->next)
            {
            int n = om->groupnum;
            if (n < mrc)
              {
              int plen = offsets[2*n + 1] - offsets[2*n];
              if (plen > 0)
                {
                if (printed && om_separator != NULL)
                  fprintf(stdout, "%s", om_separator);
                print_match(matchptr + offsets[n*2], plen);
                printed = TRUE;
                }
              }
            }

          if (printed || printname != NULL || number)
            fprintf(stdout, STDOUT_NL);
          }

        /* Prepare to repeat to find the next match in the line. */

        match = FALSE;
        if (line_buffered) fflush(stdout);
        rc = 0;                      /* Had some success */

        /* If the pattern contained a lookbehind that included \K, it is
        possible that the end of the match might be at or before the actual
        starting offset we have just used. In this case, start one character
        further on. */

        startoffset = offsets[1];    /* Restart after the match */
        oldstartoffset = pcre2_get_startchar(match_data);
        if (startoffset <= oldstartoffset)
          {
          if (startoffset >= length) goto END_ONE_MATCH;  /* Were at end */
          startoffset = oldstartoffset + 1;
          if (utf) while ((matchptr[startoffset] & 0xc0) == 0x80) startoffset++;
          }

        /* If the current match ended past the end of the line (only possible
        in multiline mode), we must move on to the line in which it did end
        before searching for more matches. */

        while (startoffset > linelength)
          {
          matchptr = ptr += linelength + endlinelength;
          filepos += (int)(linelength + endlinelength);
          linenumber++;
          startoffset -= (int)(linelength + endlinelength);
          t = end_of_line(ptr, endptr, &endlinelength);
          linelength = t - ptr - endlinelength;
          length = (size_t)(endptr - ptr);
          }

        goto ONLY_MATCHING_RESTART;
        }
      }

    /* This is the default case when none of the above options is set. We print
    the matching lines(s), possibly preceded and/or followed by other lines of
    context. */

    else
      {
      /* See if there is a requirement to print some "after" lines from a
      previous match. We never print any overlaps. */

      if (after_context > 0 && lastmatchnumber > 0)
        {
        int ellength;
        int linecount = 0;
        char *p = lastmatchrestart;

        while (p < ptr && linecount < after_context)
          {
          p = end_of_line(p, ptr, &ellength);
          linecount++;
          }

        /* It is important to advance lastmatchrestart during this printing so
        that it interacts correctly with any "before" printing below. Print
        each line's data using fwrite() in case there are binary zeroes. */

        while (lastmatchrestart < p)
          {
          char *pp = lastmatchrestart;
          if (printname != NULL) fprintf(stdout, "%s-", printname);
          if (number) fprintf(stdout, "%d-", lastmatchnumber++);
          pp = end_of_line(pp, endptr, &ellength);
          FWRITE_IGNORE(lastmatchrestart, 1, pp - lastmatchrestart, stdout);
          lastmatchrestart = pp;
          }
        if (lastmatchrestart != ptr) hyphenpending = TRUE;
        }

      /* If there were non-contiguous lines printed above, insert hyphens. */

      if (hyphenpending)
        {
        fprintf(stdout, "--" STDOUT_NL);
        hyphenpending = FALSE;
        hyphenprinted = TRUE;
        }

      /* See if there is a requirement to print some "before" lines for this
      match. Again, don't print overlaps. */

      if (before_context > 0)
        {
        int linecount = 0;
        char *p = ptr;

        while (p > main_buffer && (lastmatchnumber == 0 || p > lastmatchrestart) &&
               linecount < before_context)
          {
          linecount++;
          p = previous_line(p, main_buffer);
          }

        if (lastmatchnumber > 0 && p > lastmatchrestart && !hyphenprinted)
          fprintf(stdout, "--" STDOUT_NL);

        while (p < ptr)
          {
          int ellength;
          char *pp = p;
          if (printname != NULL) fprintf(stdout, "%s-", printname);
          if (number) fprintf(stdout, "%d-", linenumber - linecount--);
          pp = end_of_line(pp, endptr, &ellength);
          FWRITE_IGNORE(p, 1, pp - p, stdout);
          p = pp;
          }
        }

      /* Now print the matching line(s); ensure we set hyphenpending at the end
      of the file if any context lines are being output. */

      if (after_context > 0 || before_context > 0)
        endhyphenpending = TRUE;

      if (printname != NULL) fprintf(stdout, "%s:", printname);
      if (number) fprintf(stdout, "%d:", linenumber);

      /* In multiline mode, we want to print to the end of the line in which
      the end of the matched string is found, so we adjust linelength and the
      line number appropriately, but only when there actually was a match
      (invert not set). Because the PCRE2_FIRSTLINE option is set, the start of
      the match will always be before the first newline sequence. */

      if (multiline & !invert)
        {
        char *endmatch = ptr + offsets[1];
        t = ptr;
        while (t <= endmatch)
          {
          t = end_of_line(t, endptr, &endlinelength);
          if (t < endmatch) linenumber++; else break;
          }
        linelength = t - ptr - endlinelength;
        }

      /*** NOTE: Use only fwrite() to output the data line, so that binary
      zeroes are treated as just another data character. */

      /* This extra option, for Jeffrey Friedl's debugging requirements,
      replaces the matched string, or a specific captured string if it exists,
      with X. When this happens, colouring is ignored. */

#ifdef JFRIEDL_DEBUG
      if (S_arg >= 0 && S_arg < mrc)
        {
        int first = S_arg * 2;
        int last  = first + 1;
        FWRITE_IGNORE(ptr, 1, offsets[first], stdout);
        fprintf(stdout, "X");
        FWRITE_IGNORE(ptr + offsets[last], 1, linelength - offsets[last], stdout);
        }
      else
#endif

      /* We have to split the line(s) up if colouring, and search for further
      matches, but not of course if the line is a non-match. */

      if (do_colour && !invert)
        {
        int plength;
        FWRITE_IGNORE(ptr, 1, offsets[0], stdout);
        print_match(ptr + offsets[0], offsets[1] - offsets[0]);
        for (;;)
          {
          startoffset = offsets[1];
          if (startoffset >= linelength + endlinelength ||
              !match_patterns(matchptr, length, options, startoffset, &mrc))
            break;
          FWRITE_IGNORE(matchptr + startoffset, 1, offsets[0] - startoffset, stdout);
          print_match(matchptr + offsets[0], offsets[1] - offsets[0]);
          }

        /* In multiline mode, we may have already printed the complete line
        and its line-ending characters (if they matched the pattern), so there
        may be no more to print. */

        plength = (int)((linelength + endlinelength) - startoffset);
        if (plength > 0) FWRITE_IGNORE(ptr + startoffset, 1, plength, stdout);
        }

      /* Not colouring; no need to search for further matches */

      else FWRITE_IGNORE(ptr, 1, linelength + endlinelength, stdout);
      }

    /* End of doing what has to be done for a match. If --line-buffered was
    given, flush the output. */

    if (line_buffered) fflush(stdout);
    rc = 0;    /* Had some success */

    /* Remember where the last match happened for after_context. We remember
    where we are about to restart, and that line's number. */

    lastmatchrestart = ptr + linelength + endlinelength;
    lastmatchnumber = linenumber + 1;
    }

  /* For a match in multiline inverted mode (which of course did not cause
  anything to be printed), we have to move on to the end of the match before
  proceeding. */

  if (multiline && invert && match)
    {
    int ellength;
    char *endmatch = ptr + offsets[1];
    t = ptr;
    while (t < endmatch)
      {
      t = end_of_line(t, endptr, &ellength);
      if (t <= endmatch) linenumber++; else break;
      }
    endmatch = end_of_line(endmatch, endptr, &ellength);
    linelength = endmatch - ptr - ellength;
    }

  /* Advance to after the newline and increment the line number. The file
  offset to the current line is maintained in filepos. */

  END_ONE_MATCH:
  ptr += linelength + endlinelength;
  filepos += (int)(linelength + endlinelength);
  linenumber++;

  /* If input is line buffered, and the buffer is not yet full, read another
  line and add it into the buffer. */

  if (input_line_buffered && bufflength < (size_t)bufsize)
    {
    int add = read_one_line(ptr, bufsize - (int)(ptr - main_buffer), in);
    bufflength += add;
    endptr += add;
    }

  /* If we haven't yet reached the end of the file (the buffer is full), and
  the current point is in the top 1/3 of the buffer, slide the buffer down by
  1/3 and refill it. Before we do this, if some unprinted "after" lines are
  about to be lost, print them. */

  if (bufflength >= (size_t)bufsize && ptr > main_buffer + 2*bufthird)
    {
    if (after_context > 0 &&
        lastmatchnumber > 0 &&
        lastmatchrestart < main_buffer + bufthird)
      {
      do_after_lines(lastmatchnumber, lastmatchrestart, endptr, printname);
      lastmatchnumber = 0;  /* Indicates no after lines pending */
      }

    /* Now do the shuffle */

    memmove(main_buffer, main_buffer + bufthird, 2*bufthird);
    ptr -= bufthird;

    bufflength = 2*bufthird + fill_buffer(handle, frtype,
      main_buffer + 2*bufthird, bufthird, input_line_buffered);
    endptr = main_buffer + bufflength;

    /* Adjust any last match point */

    if (lastmatchnumber > 0) lastmatchrestart -= bufthird;
    }
  }     /* Loop through the whole file */

/* End of file; print final "after" lines if wanted; do_after_lines sets
hyphenpending if it prints something. */

if (only_matching_count == 0 && !(count_only|show_total_count))
  {
  do_after_lines(lastmatchnumber, lastmatchrestart, endptr, printname);
  hyphenpending |= endhyphenpending;
  }

/* Print the file name if we are looking for those without matches and there
were none. If we found a match, we won't have got this far. */

if (filenames == FN_NOMATCH_ONLY)
  {
  fprintf(stdout, "%s" STDOUT_NL, printname);
  return 0;
  }

/* Print the match count if wanted */

if (count_only && !quiet)
  {
  if (count > 0 || !omit_zero_count)
    {
    if (printname != NULL && filenames != FN_NONE)
      fprintf(stdout, "%s:", printname);
    fprintf(stdout, "%d" STDOUT_NL, count);
    counts_printed++;
    }
  }

total_count += count;   /* Can be set without count_only */
return rc;
}



/*************************************************
*     Grep a file or recurse into a directory    *
*************************************************/

/* Given a path name, if it's a directory, scan all the files if we are
recursing; if it's a file, grep it.

Arguments:
  pathname          the path to investigate
  dir_recurse       TRUE if recursing is wanted (-r or -drecurse)
  only_one_at_top   TRUE if the path is the only one at toplevel

Returns:  -1 the file/directory was skipped
           0 if there was at least one match
           1 if there were no matches
           2 there was some kind of error

However, file opening failures are suppressed if "silent" is set.
*/

static int
grep_or_recurse(char *pathname, BOOL dir_recurse, BOOL only_one_at_top)
{
int rc = 1;
int frtype;
void *handle;
char *lastcomp;
FILE *in = NULL;           /* Ensure initialized */

#ifdef SUPPORT_LIBZ
gzFile ingz = NULL;
#endif

#ifdef SUPPORT_LIBBZ2
BZFILE *inbz2 = NULL;
#endif

#if defined SUPPORT_LIBZ || defined SUPPORT_LIBBZ2
int pathlen;
#endif

#if defined NATIVE_ZOS
int zos_type;
FILE *zos_test_file;
#endif

/* If the file name is "-" we scan stdin */

if (strcmp(pathname, "-") == 0)
  {
  return pcre2grep(stdin, FR_PLAIN, stdin_name,
    (filenames > FN_DEFAULT || (filenames == FN_DEFAULT && !only_one_at_top))?
      stdin_name : NULL);
  }

/* Inclusion and exclusion: --include-dir and --exclude-dir apply only to
directories, whereas --include and --exclude apply to everything else. The test
is against the final component of the path. */

lastcomp = strrchr(pathname, FILESEP);
lastcomp = (lastcomp == NULL)? pathname : lastcomp + 1;

/* If the file is a directory, skip if not recursing or if explicitly excluded.
Otherwise, scan the directory and recurse for each path within it. The scanning
code is localized so it can be made system-specific. */


/* For z/OS, determine the file type. */

#if defined NATIVE_ZOS
zos_test_file =  fopen(pathname,"rb");

if (zos_test_file == NULL)
   {
   if (!silent) fprintf(stderr, "pcre2grep: failed to test next file %s\n",
     pathname, strerror(errno));
   return -1;
   }
zos_type = identifyzosfiletype (zos_test_file);
fclose (zos_test_file);

/* Handle a PDS in separate code */

if (zos_type == __ZOS_PDS || zos_type == __ZOS_PDSE)
   {
   return travelonpdsdir (pathname, only_one_at_top);
   }

/* Deal with regular files in the normal way below. These types are:
   zos_type == __ZOS_PDS_MEMBER
   zos_type == __ZOS_PS
   zos_type == __ZOS_VSAM_KSDS
   zos_type == __ZOS_VSAM_ESDS
   zos_type == __ZOS_VSAM_RRDS
*/

/* Handle a z/OS directory using common code. */

else if (zos_type == __ZOS_HFS)
 {
#endif  /* NATIVE_ZOS */


/* Handle directories: common code for all OS */

if (isdirectory(pathname))
  {
  if (dee_action == dee_SKIP ||
      !test_incexc(lastcomp, include_dir_patterns, exclude_dir_patterns))
    return -1;

  if (dee_action == dee_RECURSE)
    {
    char buffer[1024];
    char *nextfile;
    directory_type *dir = opendirectory(pathname);

    if (dir == NULL)
      {
      if (!silent)
        fprintf(stderr, "pcre2grep: Failed to open directory %s: %s\n", pathname,
          strerror(errno));
      return 2;
      }

    while ((nextfile = readdirectory(dir)) != NULL)
      {
      int frc;
      sprintf(buffer, "%.512s%c%.128s", pathname, FILESEP, nextfile);
      frc = grep_or_recurse(buffer, dir_recurse, FALSE);
      if (frc > 1) rc = frc;
       else if (frc == 0 && rc == 1) rc = 0;
      }

    closedirectory(dir);
    return rc;
    }
  }

#ifdef WIN32
if (iswild(pathname))
  {
  char buffer[1024];
  char *nextfile;
  char *name;
  directory_type *dir = opendirectory(pathname);

  if (dir == NULL)
    return 0;

  for (nextfile = name = pathname; *nextfile != 0; nextfile++)
    if (*nextfile == '/' || *nextfile == '\\')
      name = nextfile + 1;
  *name = 0;

  while ((nextfile = readdirectory(dir)) != NULL)
    {
    int frc;
    sprintf(buffer, "%.512s%.128s", pathname, nextfile);
    frc = grep_or_recurse(buffer, dir_recurse, FALSE);
    if (frc > 1) rc = frc;
     else if (frc == 0 && rc == 1) rc = 0;
    }

  closedirectory(dir);
  return rc;
  }
#endif

#if defined NATIVE_ZOS
 }
#endif

/* If the file is not a directory, check for a regular file, and if it is not,
skip it if that's been requested. Otherwise, check for an explicit inclusion or
exclusion. */

else if (
#if defined NATIVE_ZOS
        (zos_type == __ZOS_NOFILE && DEE_action == DEE_SKIP) ||
#else  /* all other OS */
        (!isregfile(pathname) && DEE_action == DEE_SKIP) ||
#endif
        !test_incexc(lastcomp, include_patterns, exclude_patterns))
  return -1;  /* File skipped */

/* Control reaches here if we have a regular file, or if we have a directory
and recursion or skipping was not requested, or if we have anything else and
skipping was not requested. The scan proceeds. If this is the first and only
argument at top level, we don't show the file name, unless we are only showing
the file name, or the filename was forced (-H). */

#if defined SUPPORT_LIBZ || defined SUPPORT_LIBBZ2
pathlen = (int)(strlen(pathname));
#endif

/* Open using zlib if it is supported and the file name ends with .gz. */

#ifdef SUPPORT_LIBZ
if (pathlen > 3 && strcmp(pathname + pathlen - 3, ".gz") == 0)
  {
  ingz = gzopen(pathname, "rb");
  if (ingz == NULL)
    {
    if (!silent)
      fprintf(stderr, "pcre2grep: Failed to open %s: %s\n", pathname,
        strerror(errno));
    return 2;
    }
  handle = (void *)ingz;
  frtype = FR_LIBZ;
  }
else
#endif

/* Otherwise open with bz2lib if it is supported and the name ends with .bz2. */

#ifdef SUPPORT_LIBBZ2
if (pathlen > 4 && strcmp(pathname + pathlen - 4, ".bz2") == 0)
  {
  inbz2 = BZ2_bzopen(pathname, "rb");
  handle = (void *)inbz2;
  frtype = FR_LIBBZ2;
  }
else
#endif

/* Otherwise use plain fopen(). The label is so that we can come back here if
an attempt to read a .bz2 file indicates that it really is a plain file. */

#ifdef SUPPORT_LIBBZ2
PLAIN_FILE:
#endif
  {
  in = fopen(pathname, "rb");
  handle = (void *)in;
  frtype = FR_PLAIN;
  }

/* All the opening methods return errno when they fail. */

if (handle == NULL)
  {
  if (!silent)
    fprintf(stderr, "pcre2grep: Failed to open %s: %s\n", pathname,
      strerror(errno));
  return 2;
  }

/* Now grep the file */

rc = pcre2grep(handle, frtype, pathname, (filenames > FN_DEFAULT ||
  (filenames == FN_DEFAULT && !only_one_at_top))? pathname : NULL);

/* Close in an appropriate manner. */

#ifdef SUPPORT_LIBZ
if (frtype == FR_LIBZ)
  gzclose(ingz);
else
#endif

/* If it is a .bz2 file and the result is 3, it means that the first attempt to
read failed. If the error indicates that the file isn't in fact bzipped, try
again as a normal file. */

#ifdef SUPPORT_LIBBZ2
if (frtype == FR_LIBBZ2)
  {
  if (rc == 3)
    {
    int errnum;
    const char *err = BZ2_bzerror(inbz2, &errnum);
    if (errnum == BZ_DATA_ERROR_MAGIC)
      {
      BZ2_bzclose(inbz2);
      goto PLAIN_FILE;
      }
    else if (!silent)
      fprintf(stderr, "pcre2grep: Failed to read %s using bzlib: %s\n",
        pathname, err);
    rc = 2;    /* The normal "something went wrong" code */
    }
  BZ2_bzclose(inbz2);
  }
else
#endif

/* Normal file close */

fclose(in);

/* Pass back the yield from pcre2grep(). */

return rc;
}



/*************************************************
*    Handle a single-letter, no data option      *
*************************************************/

static int
handle_option(int letter, int options)
{
switch(letter)
  {
  case N_FOFFSETS: file_offsets = TRUE; break;
  case N_HELP: help(); pcre2grep_exit(0); break; /* Stops compiler warning */
  case N_LBUFFER: line_buffered = TRUE; break;
  case N_LOFFSETS: line_offsets = number = TRUE; break;
  case N_NOJIT: use_jit = FALSE; break;
  case 'a': binary_files = BIN_TEXT; break;
  case 'c': count_only = TRUE; break;
  case 'F': options |= PCRE2_LITERAL; break;
  case 'H': filenames = FN_FORCE; break;
  case 'I': binary_files = BIN_NOMATCH; break;
  case 'h': filenames = FN_NONE; break;
  case 'i': options |= PCRE2_CASELESS; break;
  case 'l': omit_zero_count = TRUE; filenames = FN_MATCH_ONLY; break;
  case 'L': filenames = FN_NOMATCH_ONLY; break;
  case 'M': multiline = TRUE; options |= PCRE2_MULTILINE|PCRE2_FIRSTLINE; break;
  case 'n': number = TRUE; break;

  case 'o':
  only_matching_last = add_number(0, only_matching_last);
  if (only_matching == NULL) only_matching = only_matching_last;
  break;

  case 'q': quiet = TRUE; break;
  case 'r': dee_action = dee_RECURSE; break;
  case 's': silent = TRUE; break;
  case 't': show_total_count = TRUE; break;
  case 'u': options |= PCRE2_UTF; utf = TRUE; break;
  case 'v': invert = TRUE; break;
  case 'w': extra_options |= PCRE2_EXTRA_MATCH_WORD; break;
  case 'x': extra_options |= PCRE2_EXTRA_MATCH_LINE; break;

  case 'V':
    {
    unsigned char buffer[128];
    (void)pcre2_config(PCRE2_CONFIG_VERSION, buffer);
    fprintf(stdout, "pcre2grep version %s" STDOUT_NL, buffer);
    }
  pcre2grep_exit(0);
  break;

  default:
  fprintf(stderr, "pcre2grep: Unknown option -%c\n", letter);
  pcre2grep_exit(usage(2));
  }

return options;
}



/*************************************************
*          Construct printed ordinal             *
*************************************************/

/* This turns a number into "1st", "3rd", etc. */

static char *
ordin(int n)
{
static char buffer[14];
char *p = buffer;
sprintf(p, "%d", n);
while (*p != 0) p++;
n %= 100;
if (n >= 11 && n <= 13) n = 0;
switch (n%10)
  {
  case 1: strcpy(p, "st"); break;
  case 2: strcpy(p, "nd"); break;
  case 3: strcpy(p, "rd"); break;
  default: strcpy(p, "th"); break;
  }
return buffer;
}



/*************************************************
*          Compile a single pattern              *
*************************************************/

/* Do nothing if the pattern has already been compiled. This is the case for
include/exclude patterns read from a file.

When the -F option has been used, each "pattern" may be a list of strings,
separated by line breaks. They will be matched literally. We split such a
string and compile the first substring, inserting an additional block into the
pattern chain.

Arguments:
  p              points to the pattern block
  options        the PCRE options
  fromfile       TRUE if the pattern was read from a file
  fromtext       file name or identifying text (e.g. "include")
  count          0 if this is the only command line pattern, or
                 number of the command line pattern, or
                 linenumber for a pattern from a file

Returns:         TRUE on success, FALSE after an error
*/

static BOOL
compile_pattern(patstr *p, int options, int fromfile, const char *fromtext,
  int count)
{
char *ps;
int errcode;
PCRE2_SIZE patlen, erroffset;
PCRE2_UCHAR errmessbuffer[ERRBUFSIZ];

if (p->compiled != NULL) return TRUE;

ps = p->string;
patlen = strlen(ps);

if ((options & PCRE2_LITERAL) != 0)
  {
  int ellength;
  char *eop = ps + patlen;
  char *pe = end_of_line(ps, eop, &ellength);

  if (ellength != 0)
    {
    if (add_pattern(pe, p) == NULL) return FALSE;
    patlen = (int)(pe - ps - ellength);
    }
  }

p->compiled = pcre2_compile((PCRE2_SPTR)ps, patlen, options, &errcode,
  &erroffset, compile_context);

/* Handle successful compile. Try JIT-compiling if supported and enabled. We
ignore any JIT compiler errors, relying falling back to interpreting if
anything goes wrong with JIT. */

if (p->compiled != NULL)
  {
#ifdef SUPPORT_PCRE2GREP_JIT
  if (use_jit) (void)pcre2_jit_compile(p->compiled, PCRE2_JIT_COMPLETE);
#endif
  return TRUE;
  }

/* Handle compile errors */

if (erroffset > patlen) erroffset = patlen;
pcre2_get_error_message(errcode, errmessbuffer, sizeof(errmessbuffer));

if (fromfile)
  {
  fprintf(stderr, "pcre2grep: Error in regex in line %d of %s "
    "at offset %d: %s\n", count, fromtext, (int)erroffset, errmessbuffer);
  }
else
  {
  if (count == 0)
    fprintf(stderr, "pcre2grep: Error in %s regex at offset %d: %s\n",
      fromtext, (int)erroffset, errmessbuffer);
  else
    fprintf(stderr, "pcre2grep: Error in %s %s regex at offset %d: %s\n",
      ordin(count), fromtext, (int)erroffset, errmessbuffer);
  }

return FALSE;
}



/*************************************************
*     Read and compile a file of patterns        *
*************************************************/

/* This is used for --filelist, --include-from, and --exclude-from.

Arguments:
  name         the name of the file; "-" is stdin
  patptr       pointer to the pattern chain anchor
  patlastptr   pointer to the last pattern pointer

Returns:       TRUE if all went well
*/

static BOOL
read_pattern_file(char *name, patstr **patptr, patstr **patlastptr)
{
int linenumber = 0;
FILE *f;
const char *filename;
char buffer[MAXPATLEN+20];

if (strcmp(name, "-") == 0)
  {
  f = stdin;
  filename = stdin_name;
  }
else
  {
  f = fopen(name, "r");
  if (f == NULL)
    {
    fprintf(stderr, "pcre2grep: Failed to open %s: %s\n", name, strerror(errno));
    return FALSE;
    }
  filename = name;
  }

while (fgets(buffer, sizeof(buffer), f) != NULL)
  {
  char *s = buffer + (int)strlen(buffer);
  while (s > buffer && isspace((unsigned char)(s[-1]))) s--;
  *s = 0;
  linenumber++;
  if (buffer[0] == 0) continue;   /* Skip blank lines */

  /* Note: this call to add_pattern() puts a pointer to the local variable
  "buffer" into the pattern chain. However, that pointer is used only when
  compiling the pattern, which happens immediately below, so we flatten it
  afterwards, as a precaution against any later code trying to use it. */

  *patlastptr = add_pattern(buffer, *patlastptr);
  if (*patlastptr == NULL)
    {
    if (f != stdin) fclose(f);
    return FALSE;
    }
  if (*patptr == NULL) *patptr = *patlastptr;

  /* This loop is needed because compiling a "pattern" when -F is set may add
  on additional literal patterns if the original contains a newline. In the
  common case, it never will, because fgets() stops at a newline. However,
  the -N option can be used to give pcre2grep a different newline setting. */

  for(;;)
    {
    if (!compile_pattern(*patlastptr, pcre2_options, TRUE, filename,
        linenumber))
      {
      if (f != stdin) fclose(f);
      return FALSE;
      }
    (*patlastptr)->string = NULL;            /* Insurance */
    if ((*patlastptr)->next == NULL) break;
    *patlastptr = (*patlastptr)->next;
    }
  }

if (f != stdin) fclose(f);
return TRUE;
}



/*************************************************
*                Main program                    *
*************************************************/

/* Returns 0 if something matched, 1 if nothing matched, 2 after an error. */

int
main(int argc, char **argv)
{
int i, j;
int rc = 1;
BOOL only_one_at_top;
patstr *cp;
fnstr *fn;
const char *locale_from = "--locale";

#ifdef SUPPORT_PCRE2GREP_JIT
pcre2_jit_stack *jit_stack = NULL;
#endif

/* In Windows, stdout is set up as a text stream, which means that \n is
converted to \r\n. This causes output lines that are copied from the input to
change from ....\r\n to ....\r\r\n, which is not right. We therefore ensure
that stdout is a binary stream. Note that this means all other output to stdout
must use STDOUT_NL to terminate lines. */

#ifdef WIN32
_setmode(_fileno(stdout), _O_BINARY);
#endif

/* Set up a default compile and match contexts and a match data block. */

compile_context = pcre2_compile_context_create(NULL);
match_context = pcre2_match_context_create(NULL);
match_data = pcre2_match_data_create(OFFSET_SIZE, NULL);
offsets = pcre2_get_ovector_pointer(match_data);

/* If string (script) callouts are supported, set up the callout processing
function. */

#ifdef SUPPORT_PCRE2GREP_CALLOUT
pcre2_set_callout(match_context, pcre2grep_callout, NULL);
#endif

/* Process the options */

for (i = 1; i < argc; i++)
  {
  option_item *op = NULL;
  char *option_data = (char *)"";    /* default to keep compiler happy */
  BOOL longop;
  BOOL longopwasequals = FALSE;

  if (argv[i][0] != '-') break;

  /* If we hit an argument that is just "-", it may be a reference to STDIN,
  but only if we have previously had -e or -f to define the patterns. */

  if (argv[i][1] == 0)
    {
    if (pattern_files != NULL || patterns != NULL) break;
      else pcre2grep_exit(usage(2));
    }

  /* Handle a long name option, or -- to terminate the options */

  if (argv[i][1] == '-')
    {
    char *arg = argv[i] + 2;
    char *argequals = strchr(arg, '=');

    if (*arg == 0)    /* -- terminates options */
      {
      i++;
      break;                /* out of the options-handling loop */
      }

    longop = TRUE;

    /* Some long options have data that follows after =, for example file=name.
    Some options have variations in the long name spelling: specifically, we
    allow "regexp" because GNU grep allows it, though I personally go along
    with Jeffrey Friedl and Larry Wall in preferring "regex" without the "p".
    These options are entered in the table as "regex(p)". Options can be in
    both these categories. */

    for (op = optionlist; op->one_char != 0; op++)
      {
      char *opbra = strchr(op->long_name, '(');
      char *equals = strchr(op->long_name, '=');

      /* Handle options with only one spelling of the name */

      if (opbra == NULL)     /* Does not contain '(' */
        {
        if (equals == NULL)  /* Not thing=data case */
          {
          if (strcmp(arg, op->long_name) == 0) break;
          }
        else                 /* Special case xxx=data */
          {
          int oplen = (int)(equals - op->long_name);
          int arglen = (argequals == NULL)?
            (int)strlen(arg) : (int)(argequals - arg);
          if (oplen == arglen && strncmp(arg, op->long_name, oplen) == 0)
            {
            option_data = arg + arglen;
            if (*option_data == '=')
              {
              option_data++;
              longopwasequals = TRUE;
              }
            break;
            }
          }
        }

      /* Handle options with an alternate spelling of the name */

      else
        {
        char buff1[24];
        char buff2[24];

        int baselen = (int)(opbra - op->long_name);
        int fulllen = (int)(strchr(op->long_name, ')') - op->long_name + 1);
        int arglen = (argequals == NULL || equals == NULL)?
          (int)strlen(arg) : (int)(argequals - arg);

        sprintf(buff1, "%.*s", baselen, op->long_name);
        sprintf(buff2, "%s%.*s", buff1, fulllen - baselen - 2, opbra + 1);

        if (strncmp(arg, buff1, arglen) == 0 ||
           strncmp(arg, buff2, arglen) == 0)
          {
          if (equals != NULL && argequals != NULL)
            {
            option_data = argequals;
            if (*option_data == '=')
              {
              option_data++;
              longopwasequals = TRUE;
              }
            }
          break;
          }
        }
      }

    if (op->one_char == 0)
      {
      fprintf(stderr, "pcre2grep: Unknown option %s\n", argv[i]);
      pcre2grep_exit(usage(2));
      }
    }

  /* Jeffrey Friedl's debugging harness uses these additional options which
  are not in the right form for putting in the option table because they use
  only one hyphen, yet are more than one character long. By putting them
  separately here, they will not get displayed as part of the help() output,
  but I don't think Jeffrey will care about that. */

#ifdef JFRIEDL_DEBUG
  else if (strcmp(argv[i], "-pre") == 0) {
          jfriedl_prefix = argv[++i];
          continue;
  } else if (strcmp(argv[i], "-post") == 0) {
          jfriedl_postfix = argv[++i];
          continue;
  } else if (strcmp(argv[i], "-XT") == 0) {
          sscanf(argv[++i], "%d", &jfriedl_XT);
          continue;
  } else if (strcmp(argv[i], "-XR") == 0) {
          sscanf(argv[++i], "%d", &jfriedl_XR);
          continue;
  }
#endif


  /* One-char options; many that have no data may be in a single argument; we
  continue till we hit the last one or one that needs data. */

  else
    {
    char *s = argv[i] + 1;
    longop = FALSE;

    while (*s != 0)
      {
      for (op = optionlist; op->one_char != 0; op++)
        {
        if (*s == op->one_char) break;
        }
      if (op->one_char == 0)
        {
        fprintf(stderr, "pcre2grep: Unknown option letter '%c' in \"%s\"\n",
          *s, argv[i]);
        pcre2grep_exit(usage(2));
        }

      option_data = s+1;

      /* Break out if this is the last character in the string; it's handled
      below like a single multi-char option. */

      if (*option_data == 0) break;

      /* Check for a single-character option that has data: OP_OP_NUMBER(S)
      are used for ones that either have a numerical number or defaults, i.e.
      the data is optional. If a digit follows, there is data; if not, carry on
      with other single-character options in the same string. */

      if (op->type == OP_OP_NUMBER || op->type == OP_OP_NUMBERS)
        {
        if (isdigit((unsigned char)s[1])) break;
        }
      else   /* Check for an option with data */
        {
        if (op->type != OP_NODATA) break;
        }

      /* Handle a single-character option with no data, then loop for the
      next character in the string. */

      pcre2_options = handle_option(*s++, pcre2_options);
      }
    }

  /* At this point we should have op pointing to a matched option. If the type
  is NO_DATA, it means that there is no data, and the option might set
  something in the PCRE options. */

  if (op->type == OP_NODATA)
    {
    pcre2_options = handle_option(op->one_char, pcre2_options);
    continue;
    }

  /* If the option type is OP_OP_STRING or OP_OP_NUMBER(S), it's an option that
  either has a value or defaults to something. It cannot have data in a
  separate item. At the moment, the only such options are "colo(u)r",
  "only-matching", and Jeffrey Friedl's special -S debugging option. */

  if (*option_data == 0 &&
      (op->type == OP_OP_STRING || op->type == OP_OP_NUMBER ||
       op->type == OP_OP_NUMBERS))
    {
    switch (op->one_char)
      {
      case N_COLOUR:
      colour_option = "auto";
      break;

      case 'o':
      only_matching_last = add_number(0, only_matching_last);
      if (only_matching == NULL) only_matching = only_matching_last;
      break;

#ifdef JFRIEDL_DEBUG
      case 'S':
      S_arg = 0;
      break;
#endif
      }
    continue;
    }

  /* Otherwise, find the data string for the option. */

  if (*option_data == 0)
    {
    if (i >= argc - 1 || longopwasequals)
      {
      fprintf(stderr, "pcre2grep: Data missing after %s\n", argv[i]);
      pcre2grep_exit(usage(2));
      }
    option_data = argv[++i];
    }

  /* If the option type is OP_OP_NUMBERS, the value is a number that is to be
  added to a chain of numbers. */

  if (op->type == OP_OP_NUMBERS)
    {
    unsigned long int n = decode_number(option_data, op, longop);
    omdatastr *omd = (omdatastr *)op->dataptr;
    *(omd->lastptr) = add_number((int)n, *(omd->lastptr));
    if (*(omd->anchor) == NULL) *(omd->anchor) = *(omd->lastptr);
    }

  /* If the option type is OP_PATLIST, it's the -e option, or one of the
  include/exclude options, which can be called multiple times to create lists
  of patterns. */

  else if (op->type == OP_PATLIST)
    {
    patdatastr *pd = (patdatastr *)op->dataptr;
    *(pd->lastptr) = add_pattern(option_data, *(pd->lastptr));
    if (*(pd->lastptr) == NULL) goto EXIT2;
    if (*(pd->anchor) == NULL) *(pd->anchor) = *(pd->lastptr);
    }

  /* If the option type is OP_FILELIST, it's one of the options that names a
  file. */

  else if (op->type == OP_FILELIST)
    {
    fndatastr *fd = (fndatastr *)op->dataptr;
    fn = (fnstr *)malloc(sizeof(fnstr));
    if (fn == NULL)
      {
      fprintf(stderr, "pcre2grep: malloc failed\n");
      goto EXIT2;
      }
    fn->next = NULL;
    fn->name = option_data;
    if (*(fd->anchor) == NULL)
      *(fd->anchor) = fn;
    else
      (*(fd->lastptr))->next = fn;
    *(fd->lastptr) = fn;
    }

  /* Handle OP_BINARY_FILES */

  else if (op->type == OP_BINFILES)
    {
    if (strcmp(option_data, "binary") == 0)
      binary_files = BIN_BINARY;
    else if (strcmp(option_data, "without-match") == 0)
      binary_files = BIN_NOMATCH;
    else if (strcmp(option_data, "text") == 0)
      binary_files = BIN_TEXT;
    else
      {
      fprintf(stderr, "pcre2grep: unknown value \"%s\" for binary-files\n",
        option_data);
      pcre2grep_exit(usage(2));
      }
    }

  /* Otherwise, deal with a single string or numeric data value. */

  else if (op->type != OP_NUMBER && op->type != OP_U32NUMBER &&
           op->type != OP_OP_NUMBER && op->type != OP_SIZE)
    {
    *((char **)op->dataptr) = option_data;
    }
  else
    {
    unsigned long int n = decode_number(option_data, op, longop);
    if (op->type == OP_U32NUMBER) *((uint32_t *)op->dataptr) = n;
      else if (op->type == OP_SIZE) *((PCRE2_SIZE *)op->dataptr) = n;
      else *((int *)op->dataptr) = n;
    }
  }

/* Options have been decoded. If -C was used, its value is used as a default
for -A and -B. */

if (both_context > 0)
  {
  if (after_context == 0) after_context = both_context;
  if (before_context == 0) before_context = both_context;
  }

/* Only one of --only-matching, --output, --file-offsets, or --line-offsets is
permitted. They display, each in their own way, only the data that has matched.
*/

only_matching_count = (only_matching != NULL) + (output_text != NULL) +
  file_offsets + line_offsets;

if (only_matching_count > 1)
  {
  fprintf(stderr, "pcre2grep: Cannot mix --only-matching, --output, "
    "--file-offsets and/or --line-offsets\n");
  pcre2grep_exit(usage(2));
  }

/* Check the text supplied to --output for errors. */

if (output_text != NULL &&
    !syntax_check_output_text((PCRE2_SPTR)output_text, FALSE))
  goto EXIT2;

/* Put limits into the match data block. */

if (heap_limit != PCRE2_UNSET) pcre2_set_heap_limit(match_context, heap_limit);
if (match_limit > 0) pcre2_set_match_limit(match_context, match_limit);
if (depth_limit > 0) pcre2_set_depth_limit(match_context, depth_limit);

/* If a locale has not been provided as an option, see if the LC_CTYPE or
LC_ALL environment variable is set, and if so, use it. */

if (locale == NULL)
  {
  locale = getenv("LC_ALL");
  locale_from = "LC_ALL";
  }

if (locale == NULL)
  {
  locale = getenv("LC_CTYPE");
  locale_from = "LC_CTYPE";
  }

/* If a locale is set, use it to generate the tables the PCRE needs. Passing
NULL to pcre2_maketables() means that malloc() is used to get the memory. */

if (locale != NULL)
  {
  if (setlocale(LC_CTYPE, locale) == NULL)
    {
    fprintf(stderr, "pcre2grep: Failed to set locale %s (obtained from %s)\n",
      locale, locale_from);
    goto EXIT2;
    }
  character_tables = pcre2_maketables(NULL);
  pcre2_set_character_tables(compile_context, character_tables);
  }

/* Sort out colouring */

if (colour_option != NULL && strcmp(colour_option, "never") != 0)
  {
  if (strcmp(colour_option, "always") == 0)
#ifdef WIN32
    do_ansi = !is_stdout_tty(),
#endif
    do_colour = TRUE;
  else if (strcmp(colour_option, "auto") == 0) do_colour = is_stdout_tty();
  else
    {
    fprintf(stderr, "pcre2grep: Unknown colour setting \"%s\"\n",
      colour_option);
    goto EXIT2;
    }
  if (do_colour)
    {
    char *cs = getenv("PCRE2GREP_COLOUR");
    if (cs == NULL) cs = getenv("PCRE2GREP_COLOR");
    if (cs == NULL) cs = getenv("PCREGREP_COLOUR");
    if (cs == NULL) cs = getenv("PCREGREP_COLOR");
    if (cs == NULL) cs = parse_grep_colors(getenv("GREP_COLORS"));
    if (cs == NULL) cs = getenv("GREP_COLOR");
    if (cs != NULL)
      {
      if (strspn(cs, ";0123456789") == strlen(cs)) colour_string = cs;
      }
#ifdef WIN32
    init_colour_output();
#endif
    }
  }

/* Sort out a newline setting. */

if (newline_arg != NULL)
  {
  for (endlinetype = 1; endlinetype < (int)(sizeof(newlines)/sizeof(char *));
       endlinetype++)
    {
    if (strcmpic(newline_arg, newlines[endlinetype]) == 0) break;
    }
  if (endlinetype < (int)(sizeof(newlines)/sizeof(char *)))
    pcre2_set_newline(compile_context, endlinetype);
  else
    {
    fprintf(stderr, "pcre2grep: Invalid newline specifier \"%s\"\n",
      newline_arg);
    goto EXIT2;
    }
  }

/* Find default newline convention */

else
  {
  (void)pcre2_config(PCRE2_CONFIG_NEWLINE, &endlinetype);
  }

/* Interpret the text values for -d and -D */

if (dee_option != NULL)
  {
  if (strcmp(dee_option, "read") == 0) dee_action = dee_READ;
  else if (strcmp(dee_option, "recurse") == 0) dee_action = dee_RECURSE;
  else if (strcmp(dee_option, "skip") == 0) dee_action = dee_SKIP;
  else
    {
    fprintf(stderr, "pcre2grep: Invalid value \"%s\" for -d\n", dee_option);
    goto EXIT2;
    }
  }

if (DEE_option != NULL)
  {
  if (strcmp(DEE_option, "read") == 0) DEE_action = DEE_READ;
  else if (strcmp(DEE_option, "skip") == 0) DEE_action = DEE_SKIP;
  else
    {
    fprintf(stderr, "pcre2grep: Invalid value \"%s\" for -D\n", DEE_option);
    goto EXIT2;
    }
  }

/* Set the extra options */

(void)pcre2_set_compile_extra_options(compile_context, extra_options);

/* Check the values for Jeffrey Friedl's debugging options. */

#ifdef JFRIEDL_DEBUG
if (S_arg > 9)
  {
  fprintf(stderr, "pcre2grep: bad value for -S option\n");
  return 2;
  }
if (jfriedl_XT != 0 || jfriedl_XR != 0)
  {
  if (jfriedl_XT == 0) jfriedl_XT = 1;
  if (jfriedl_XR == 0) jfriedl_XR = 1;
  }
#endif

/* If use_jit is set, check whether JIT is available. If not, do not try
to use JIT. */

if (use_jit)
  {
  uint32_t answer;
  (void)pcre2_config(PCRE2_CONFIG_JIT, &answer);
  if (!answer) use_jit = FALSE;
  }

/* Get memory for the main buffer. */

if (bufthird <= 0)
  {
  fprintf(stderr, "pcre2grep: --buffer-size must be greater than zero\n");
  goto EXIT2;
  }

bufsize = 3*bufthird;
main_buffer = (char *)malloc(bufsize);

if (main_buffer == NULL)
  {
  fprintf(stderr, "pcre2grep: malloc failed\n");
  goto EXIT2;
  }

/* If no patterns were provided by -e, and there are no files provided by -f,
the first argument is the one and only pattern, and it must exist. */

if (patterns == NULL && pattern_files == NULL)
  {
  if (i >= argc) return usage(2);
  patterns = patterns_last = add_pattern(argv[i++], NULL);
  if (patterns == NULL) goto EXIT2;
  }

/* Compile the patterns that were provided on the command line, either by
multiple uses of -e or as a single unkeyed pattern. We cannot do this until
after all the command-line options are read so that we know which PCRE options
to use. When -F is used, compile_pattern() may add another block into the
chain, so we must not access the next pointer till after the compile. */

for (j = 1, cp = patterns; cp != NULL; j++, cp = cp->next)
  {
  if (!compile_pattern(cp, pcre2_options, FALSE, "command-line",
       (j == 1 && patterns->next == NULL)? 0 : j))
    goto EXIT2;
  }

/* Read and compile the regular expressions that are provided in files. */

for (fn = pattern_files; fn != NULL; fn = fn->next)
  {
  if (!read_pattern_file(fn->name, &patterns, &patterns_last)) goto EXIT2;
  }

/* Unless JIT has been explicitly disabled, arrange a stack for it to use. */

#ifdef SUPPORT_PCRE2GREP_JIT
if (use_jit)
  {
  jit_stack = pcre2_jit_stack_create(32*1024, 1024*1024, NULL);
  if (jit_stack != NULL                        )
    pcre2_jit_stack_assign(match_context, NULL, jit_stack);
  }
#endif

/* -F, -w, and -x do not apply to include or exclude patterns, so we must
adjust the options. */

pcre2_options &= ~PCRE2_LITERAL;
(void)pcre2_set_compile_extra_options(compile_context, 0);

/* If there are include or exclude patterns read from the command line, compile
them. */

for (j = 0; j < 4; j++)
  {
  int k;
  for (k = 1, cp = *(incexlist[j]); cp != NULL; k++, cp = cp->next)
    {
    if (!compile_pattern(cp, pcre2_options, FALSE, incexname[j],
         (k == 1 && cp->next == NULL)? 0 : k))
      goto EXIT2;
    }
  }

/* Read and compile include/exclude patterns from files. */

for (fn = include_from; fn != NULL; fn = fn->next)
  {
  if (!read_pattern_file(fn->name, &include_patterns, &include_patterns_last))
    goto EXIT2;
  }

for (fn = exclude_from; fn != NULL; fn = fn->next)
  {
  if (!read_pattern_file(fn->name, &exclude_patterns, &exclude_patterns_last))
    goto EXIT2;
  }

/* If there are no files that contain lists of files to search, and there are
no file arguments, search stdin, and then exit. */

if (file_lists == NULL && i >= argc)
  {
  rc = pcre2grep(stdin, FR_PLAIN, stdin_name,
    (filenames > FN_DEFAULT)? stdin_name : NULL);
  goto EXIT;
  }

/* If any files that contains a list of files to search have been specified,
read them line by line and search the given files. */

for (fn = file_lists; fn != NULL; fn = fn->next)
  {
  char buffer[FNBUFSIZ];
  FILE *fl;
  if (strcmp(fn->name, "-") == 0) fl = stdin; else
    {
    fl = fopen(fn->name, "rb");
    if (fl == NULL)
      {
      fprintf(stderr, "pcre2grep: Failed to open %s: %s\n", fn->name,
        strerror(errno));
      goto EXIT2;
      }
    }
  while (fgets(buffer, sizeof(buffer), fl) != NULL)
    {
    int frc;
    char *end = buffer + (int)strlen(buffer);
    while (end > buffer && isspace(end[-1])) end--;
    *end = 0;
    if (*buffer != 0)
      {
      frc = grep_or_recurse(buffer, dee_action == dee_RECURSE, FALSE);
      if (frc > 1) rc = frc;
        else if (frc == 0 && rc == 1) rc = 0;
      }
    }
  if (fl != stdin) fclose(fl);
  }

/* After handling file-list, work through remaining arguments. Pass in the fact
that there is only one argument at top level - this suppresses the file name if
the argument is not a directory and filenames are not otherwise forced. */

only_one_at_top = i == argc - 1 && file_lists == NULL;

for (; i < argc; i++)
  {
  int frc = grep_or_recurse(argv[i], dee_action == dee_RECURSE,
    only_one_at_top);
  if (frc > 1) rc = frc;
    else if (frc == 0 && rc == 1) rc = 0;
  }

#ifdef SUPPORT_PCRE2GREP_CALLOUT
/* If separating builtin echo callouts by implicit newline, add one more for
the final item. */

if (om_separator != NULL && strcmp(om_separator, STDOUT_NL) == 0)
  fprintf(stdout, STDOUT_NL);
#endif

/* Show the total number of matches if requested, but not if only one file's
count was printed. */

if (show_total_count && counts_printed != 1 && filenames != FN_NOMATCH_ONLY)
  {
  if (counts_printed != 0 && filenames >= FN_DEFAULT)
    fprintf(stdout, "TOTAL:");
  fprintf(stdout, "%d" STDOUT_NL, total_count);
  }

EXIT:
#ifdef SUPPORT_PCRE2GREP_JIT
if (jit_stack != NULL) pcre2_jit_stack_free(jit_stack);
#endif

free(main_buffer);
free((void *)character_tables);

pcre2_compile_context_free(compile_context);
pcre2_match_context_free(match_context);
pcre2_match_data_free(match_data);

free_pattern_chain(patterns);
free_pattern_chain(include_patterns);
free_pattern_chain(include_dir_patterns);
free_pattern_chain(exclude_patterns);
free_pattern_chain(exclude_dir_patterns);

free_file_chain(exclude_from);
free_file_chain(include_from);
free_file_chain(pattern_files);
free_file_chain(file_lists);

while (only_matching != NULL)
  {
  omstr *this = only_matching;
  only_matching = this->next;
  free(this);
  }

pcre2grep_exit(rc);

EXIT2:
rc = 2;
goto EXIT;
}

/* End of pcre2grep */
