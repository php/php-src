/*************************************************
*               pcregrep program                 *
*************************************************/

/* This is a grep program that uses the PCRE regular expression library to do
its pattern matching. On a Unix or Win32 system it can recurse into
directories.

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

#include "config.h"

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef SUPPORT_LIBZ
#include <zlib.h>
#endif

#ifdef SUPPORT_LIBBZ2
#include <bzlib.h>
#endif

#include "pcre.h"

#define FALSE 0
#define TRUE 1

typedef int BOOL;

#define MAX_PATTERN_COUNT 100

#if BUFSIZ > 8192
#define MBUFTHIRD BUFSIZ
#else
#define MBUFTHIRD 8192
#endif

/* Values for the "filenames" variable, which specifies options for file name
output. The order is important; it is assumed that a file name is wanted for
all values greater than FN_DEFAULT. */

enum { FN_NONE, FN_DEFAULT, FN_ONLY, FN_NOMATCH_ONLY, FN_FORCE };

/* File reading styles */

enum { FR_PLAIN, FR_LIBZ, FR_LIBBZ2 };

/* Actions for the -d and -D options */

enum { dee_READ, dee_SKIP, dee_RECURSE };
enum { DEE_READ, DEE_SKIP };

/* Actions for special processing options (flag bits) */

#define PO_WORD_MATCH     0x0001
#define PO_LINE_MATCH     0x0002
#define PO_FIXED_STRINGS  0x0004

/* Line ending types */

enum { EL_LF, EL_CR, EL_CRLF, EL_ANY, EL_ANYCRLF };



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

static int  endlinetype;

static char *colour_string = (char *)"1;31";
static char *colour_option = NULL;
static char *dee_option = NULL;
static char *DEE_option = NULL;
static char *newline = NULL;
static char *pattern_filename = NULL;
static char *stdin_name = (char *)"(standard input)";
static char *locale = NULL;

static const unsigned char *pcretables = NULL;

static int  pattern_count = 0;
static pcre **pattern_list = NULL;
static pcre_extra **hints_list = NULL;

static char *include_pattern = NULL;
static char *exclude_pattern = NULL;
static char *include_dir_pattern = NULL;
static char *exclude_dir_pattern = NULL;

static pcre *include_compiled = NULL;
static pcre *exclude_compiled = NULL;
static pcre *include_dir_compiled = NULL;
static pcre *exclude_dir_compiled = NULL;

static int after_context = 0;
static int before_context = 0;
static int both_context = 0;
static int dee_action = dee_READ;
static int DEE_action = DEE_READ;
static int error_count = 0;
static int filenames = FN_DEFAULT;
static int process_options = 0;

static BOOL count_only = FALSE;
static BOOL do_colour = FALSE;
static BOOL file_offsets = FALSE;
static BOOL hyphenpending = FALSE;
static BOOL invert = FALSE;
static BOOL line_offsets = FALSE;
static BOOL multiline = FALSE;
static BOOL number = FALSE;
static BOOL only_matching = FALSE;
static BOOL quiet = FALSE;
static BOOL silent = FALSE;
static BOOL utf8 = FALSE;

/* Structure for options and list of them */

enum { OP_NODATA, OP_STRING, OP_OP_STRING, OP_NUMBER, OP_OP_NUMBER,
       OP_PATLIST };

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

static option_item optionlist[] = {
  { OP_NODATA,    N_NULL,   NULL,              "",              "  terminate options" },
  { OP_NODATA,    N_HELP,   NULL,              "help",          "display this help and exit" },
  { OP_NUMBER,    'A',      &after_context,    "after-context=number", "set number of following context lines" },
  { OP_NUMBER,    'B',      &before_context,   "before-context=number", "set number of prior context lines" },
  { OP_OP_STRING, N_COLOUR, &colour_option,    "color=option",  "matched text color option" },
  { OP_NUMBER,    'C',      &both_context,     "context=number", "set number of context lines, before & after" },
  { OP_NODATA,    'c',      NULL,              "count",         "print only a count of matching lines per FILE" },
  { OP_OP_STRING, N_COLOUR, &colour_option,    "colour=option", "matched text colour option" },
  { OP_STRING,    'D',      &DEE_option,       "devices=action","how to handle devices, FIFOs, and sockets" },
  { OP_STRING,    'd',      &dee_option,       "directories=action", "how to handle directories" },
  { OP_PATLIST,   'e',      NULL,              "regex(p)",      "specify pattern (may be used more than once)" },
  { OP_NODATA,    'F',      NULL,              "fixed_strings", "patterns are sets of newline-separated strings" },
  { OP_STRING,    'f',      &pattern_filename, "file=path",     "read patterns from file" },
  { OP_NODATA,    N_FOFFSETS, NULL,            "file-offsets",  "output file offsets, not text" },
  { OP_NODATA,    'H',      NULL,              "with-filename", "force the prefixing filename on output" },
  { OP_NODATA,    'h',      NULL,              "no-filename",   "suppress the prefixing filename on output" },
  { OP_NODATA,    'i',      NULL,              "ignore-case",   "ignore case distinctions" },
  { OP_NODATA,    'l',      NULL,              "files-with-matches", "print only FILE names containing matches" },
  { OP_NODATA,    'L',      NULL,              "files-without-match","print only FILE names not containing matches" },
  { OP_STRING,    N_LABEL,  &stdin_name,       "label=name",    "set name for standard input" },
  { OP_NODATA,    N_LOFFSETS, NULL,            "line-offsets",  "output line numbers and offsets, not text" },
  { OP_STRING,    N_LOCALE, &locale,           "locale=locale", "use the named locale" },
  { OP_NODATA,    'M',      NULL,              "multiline",     "run in multiline mode" },
  { OP_STRING,    'N',      &newline,          "newline=type",  "set newline type (CR, LF, CRLF, ANYCRLF or ANY)" },
  { OP_NODATA,    'n',      NULL,              "line-number",   "print line number with output lines" },
  { OP_NODATA,    'o',      NULL,              "only-matching", "show only the part of the line that matched" },
  { OP_NODATA,    'q',      NULL,              "quiet",         "suppress output, just set return code" },
  { OP_NODATA,    'r',      NULL,              "recursive",     "recursively scan sub-directories" },
  { OP_STRING,    N_EXCLUDE,&exclude_pattern,  "exclude=pattern","exclude matching files when recursing" },
  { OP_STRING,    N_INCLUDE,&include_pattern,  "include=pattern","include matching files when recursing" },
  { OP_STRING,    N_EXCLUDE_DIR,&exclude_dir_pattern, "exclude_dir=pattern","exclude matching directories when recursing" },
  { OP_STRING,    N_INCLUDE_DIR,&include_dir_pattern, "include_dir=pattern","include matching directories when recursing" },
#ifdef JFRIEDL_DEBUG
  { OP_OP_NUMBER, 'S',      &S_arg,            "jeffS",         "replace matched (sub)string with X" },
#endif
  { OP_NODATA,    's',      NULL,              "no-messages",   "suppress error messages" },
  { OP_NODATA,    'u',      NULL,              "utf-8",         "use UTF-8 mode" },
  { OP_NODATA,    'V',      NULL,              "version",       "print version information and exit" },
  { OP_NODATA,    'v',      NULL,              "invert-match",  "select non-matching lines" },
  { OP_NODATA,    'w',      NULL,              "word-regex(p)", "force patterns to match only as words"  },
  { OP_NODATA,    'x',      NULL,              "line-regex(p)", "force patterns to match only whole lines" },
  { OP_NODATA,    0,        NULL,               NULL,            NULL }
};

/* Tables for prefixing and suffixing patterns, according to the -w, -x, and -F
options. These set the 1, 2, and 4 bits in process_options, respectively. Note
that the combination of -w and -x has the same effect as -x on its own, so we
can treat them as the same. */

static const char *prefix[] = {
  "", "\\b", "^(?:", "^(?:", "\\Q", "\\b\\Q", "^(?:\\Q", "^(?:\\Q" };

static const char *suffix[] = {
  "", "\\b", ")$",   ")$",   "\\E", "\\E\\b", "\\E)$",   "\\E)$" };

/* UTF-8 tables - used only when the newline setting is "any". */

const int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

const char utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };



/*************************************************
*            OS-specific functions               *
*************************************************/

/* These functions are defined so that they can be made system specific,
although at present the only ones are for Unix, Win32, and for "no support". */


/************* Directory scanning in Unix ***********/

#if defined HAVE_SYS_STAT_H && defined HAVE_DIRENT_H && defined HAVE_SYS_TYPES_H
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef DIR directory_type;

static int
isdirectory(char *filename)
{
struct stat statbuf;
if (stat(filename, &statbuf) < 0)
  return 0;        /* In the expectation that opening as a file will fail */
return ((statbuf.st_mode & S_IFMT) == S_IFDIR)? '/' : 0;
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


/************* Test for regular file in Unix **********/

static int
isregfile(char *filename)
{
struct stat statbuf;
if (stat(filename, &statbuf) < 0)
  return 1;        /* In the expectation that opening as a file will fail */
return (statbuf.st_mode & S_IFMT) == S_IFREG;
}


/************* Test stdout for being a terminal in Unix **********/

static BOOL
is_stdout_tty(void)
{
return isatty(fileno(stdout));
}


/************* Directory scanning in Win32 ***********/

/* I (Philip Hazel) have no means of testing this code. It was contributed by
Lionel Fourquaux. David Burgess added a patch to define INVALID_FILE_ATTRIBUTES
when it did not exist. David Byron added a patch that moved the #include of
<windows.h> to before the INVALID_FILE_ATTRIBUTES definition rather than after.
*/

#elif HAVE_WINDOWS_H

#ifndef STRICT
# define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF
#endif

typedef struct directory_type
{
HANDLE handle;
BOOL first;
WIN32_FIND_DATA data;
} directory_type;

int
isdirectory(char *filename)
{
DWORD attr = GetFileAttributes(filename);
if (attr == INVALID_FILE_ATTRIBUTES)
  return 0;
return ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0) ? '/' : 0;
}

directory_type *
opendirectory(char *filename)
{
size_t len;
char *pattern;
directory_type *dir;
DWORD err;
len = strlen(filename);
pattern = (char *) malloc(len + 3);
dir = (directory_type *) malloc(sizeof(*dir));
if ((pattern == NULL) || (dir == NULL))
  {
  fprintf(stderr, "pcregrep: malloc failed\n");
  exit(2);
  }
memcpy(pattern, filename, len);
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


/************* Test for regular file in Win32 **********/

/* I don't know how to do this, or if it can be done; assume all paths are
regular if they are not directories. */

int isregfile(char *filename)
{
return !isdirectory(filename);
}


/************* Test stdout for being a terminal in Win32 **********/

/* I don't know how to do this; assume never */

static BOOL
is_stdout_tty(void)
{
return FALSE;
}


/************* Directory scanning when we can't do it ***********/

/* The type is void, and apart from isdirectory(), the functions do nothing. */

#else

typedef void directory_type;

int isdirectory(char *filename) { return 0; }
directory_type * opendirectory(char *filename) { return (directory_type*)0;}
char *readdirectory(directory_type *dir) { return (char*)0;}
void closedirectory(directory_type *dir) {}


/************* Test for regular when we can't do it **********/

/* Assume all files are regular. */

int isregfile(char *filename) { return 1; }


/************* Test stdout for being a terminal when we can't do it **********/

static BOOL
is_stdout_tty(void)
{
return FALSE;
}


#endif



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
*             Find end of line                   *
*************************************************/

/* The length of the endline sequence that is found is set via lenptr. This may
be zero at the very end of the file if there is no line-ending sequence there.

Arguments:
  p         current position in line
  endptr    end of available data
  lenptr    where to put the length of the eol sequence

Returns:    pointer to the last byte of the line
*/

static char *
end_of_line(char *p, char *endptr, int *lenptr)
{
switch(endlinetype)
  {
  default:      /* Just in case */
  case EL_LF:
  while (p < endptr && *p != '\n') p++;
  if (p < endptr)
    {
    *lenptr = 1;
    return p + 1;
    }
  *lenptr = 0;
  return endptr;

  case EL_CR:
  while (p < endptr && *p != '\r') p++;
  if (p < endptr)
    {
    *lenptr = 1;
    return p + 1;
    }
  *lenptr = 0;
  return endptr;

  case EL_CRLF:
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

  case EL_ANYCRLF:
  while (p < endptr)
    {
    int extra = 0;
    register int c = *((unsigned char *)p);

    if (utf8 && c >= 0xc0)
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
      case 0x0a:    /* LF */
      *lenptr = 1;
      return p;

      case 0x0d:    /* CR */
      if (p < endptr && *p == 0x0a)
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

  case EL_ANY:
  while (p < endptr)
    {
    int extra = 0;
    register int c = *((unsigned char *)p);

    if (utf8 && c >= 0xc0)
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
      case 0x0a:    /* LF */
      case 0x0b:    /* VT */
      case 0x0c:    /* FF */
      *lenptr = 1;
      return p;

      case 0x0d:    /* CR */
      if (p < endptr && *p == 0x0a)
        {
        *lenptr = 2;
        p++;
        }
      else *lenptr = 1;
      return p;

      case 0x85:    /* NEL */
      *lenptr = utf8? 2 : 1;
      return p;

      case 0x2028:  /* LS */
      case 0x2029:  /* PS */
      *lenptr = 3;
      return p;

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
  case EL_LF:
  p--;
  while (p > startptr && p[-1] != '\n') p--;
  return p;

  case EL_CR:
  p--;
  while (p > startptr && p[-1] != '\n') p--;
  return p;

  case EL_CRLF:
  for (;;)
    {
    p -= 2;
    while (p > startptr && p[-1] != '\n') p--;
    if (p <= startptr + 1 || p[-2] == '\r') return p;
    }
  return p;   /* But control should never get here */

  case EL_ANY:
  case EL_ANYCRLF:
  if (*(--p) == '\n' && p > startptr && p[-1] == '\r') p--;
  if (utf8) while ((*p & 0xc0) == 0x80) p--;

  while (p > startptr)
    {
    register int c;
    char *pp = p - 1;

    if (utf8)
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

    if (endlinetype == EL_ANYCRLF) switch (c)
      {
      case 0x0a:    /* LF */
      case 0x0d:    /* CR */
      return p;

      default:
      break;
      }

    else switch (c)
      {
      case 0x0a:    /* LF */
      case 0x0b:    /* VT */
      case 0x0c:    /* FF */
      case 0x0d:    /* CR */
      case 0x85:    /* NEL */
      case 0x2028:  /* LS */
      case 0x2029:  /* PS */
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

static void do_after_lines(int lastmatchnumber, char *lastmatchrestart,
  char *endptr, char *printname)
{
if (after_context > 0 && lastmatchnumber > 0)
  {
  int count = 0;
  while (lastmatchrestart < endptr && count++ < after_context)
    {
    int ellength;
    char *pp = lastmatchrestart;
    if (printname != NULL) fprintf(stdout, "%s-", printname);
    if (number) fprintf(stdout, "%d-", lastmatchnumber++);
    pp = end_of_line(pp, endptr, &ellength);
    fwrite(lastmatchrestart, 1, pp - lastmatchrestart, stdout);
    lastmatchrestart = pp;
    }
  hyphenpending = TRUE;
  }
}



/*************************************************
*            Grep an individual file             *
*************************************************/

/* This is called from grep_or_recurse() below. It uses a buffer that is three
times the value of MBUFTHIRD. The matching point is never allowed to stray into
the top third of the buffer, thus keeping more of the file available for
context printing or for multiline scanning. For large files, the pointer will
be in the middle third most of the time, so the bottom third is available for
"before" context printing.

Arguments:
  handle       the fopened FILE stream for a normal file
               the gzFile pointer when reading is via libz
               the BZFILE pointer when reading is via libbz2
  frtype       FR_PLAIN, FR_LIBZ, or FR_LIBBZ2
  printname    the file name if it is to be printed for each match
               or NULL if the file name is not to be printed
               it cannot be NULL if filenames[_nomatch]_only is set

Returns:       0 if there was at least one match
               1 otherwise (no matches)
               2 if there is a read error on a .bz2 file
*/

static int
pcregrep(void *handle, int frtype, char *printname)
{
int rc = 1;
int linenumber = 1;
int lastmatchnumber = 0;
int count = 0;
int filepos = 0;
int offsets[99];
char *lastmatchrestart = NULL;
char buffer[3*MBUFTHIRD];
char *ptr = buffer;
char *endptr;
size_t bufflength;
BOOL endhyphenpending = FALSE;
FILE *in = NULL;                    /* Ensure initialized */

#ifdef SUPPORT_LIBZ
gzFile ingz = NULL;
#endif

#ifdef SUPPORT_LIBBZ2
BZFILE *inbz2 = NULL;
#endif


/* Do the first read into the start of the buffer and set up the pointer to end
of what we have. In the case of libz, a non-zipped .gz file will be read as a
plain file. However, if a .bz2 file isn't actually bzipped, the first read will
fail. */

#ifdef SUPPORT_LIBZ
if (frtype == FR_LIBZ)
  {
  ingz = (gzFile)handle;
  bufflength = gzread (ingz, buffer, 3*MBUFTHIRD);
  }
else
#endif

#ifdef SUPPORT_LIBBZ2
if (frtype == FR_LIBBZ2)
  {
  inbz2 = (BZFILE *)handle;
  bufflength = BZ2_bzread(inbz2, buffer, 3*MBUFTHIRD);
  if ((int)bufflength < 0) return 2;   /* Gotcha: bufflength is size_t; */
  }                                    /* without the cast it is unsigned. */
else
#endif

  {
  in = (FILE *)handle;
  bufflength = fread(buffer, 1, 3*MBUFTHIRD, in);
  }

endptr = buffer + bufflength;

/* Loop while the current pointer is not at the end of the file. For large
files, endptr will be at the end of the buffer when we are in the middle of the
file, but ptr will never get there, because as soon as it gets over 2/3 of the
way, the buffer is shifted left and re-filled. */

while (ptr < endptr)
  {
  int i, endlinelength;
  int mrc = 0;
  BOOL match = FALSE;
  char *matchptr = ptr;
  char *t = ptr;
  size_t length, linelength;

  /* At this point, ptr is at the start of a line. We need to find the length
  of the subject string to pass to pcre_exec(). In multiline mode, it is the
  length remainder of the data in the buffer. Otherwise, it is the length of
  the next line. After matching, we always advance by the length of the next
  line. In multiline mode the PCRE_FIRSTLINE option is used for compiling, so
  that any match is constrained to be in the first line. */

  t = end_of_line(t, endptr, &endlinelength);
  linelength = t - ptr - endlinelength;
  length = multiline? (size_t)(endptr - ptr) : linelength;

  /* Extra processing for Jeffrey Friedl's debugging. */

#ifdef JFRIEDL_DEBUG
  if (jfriedl_XT || jfriedl_XR)
  {
      #include <sys/time.h>
      #include <time.h>
      struct timeval start_time, end_time;
      struct timezone dummy;

      if (jfriedl_XT)
      {
          unsigned long newlen = length * jfriedl_XT + strlen(jfriedl_prefix) + strlen(jfriedl_postfix);
          const char *orig = ptr;
          ptr = malloc(newlen + 1);
          if (!ptr) {
                  printf("out of memory");
                  exit(2);
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
          match = (pcre_exec(pattern_list[0], hints_list[0], ptr, length, 0, 0, offsets, 99) >= 0);

      if (gettimeofday(&end_time, &dummy) != 0)
              perror("bad gettimeofday");

      double delta = ((end_time.tv_sec + (end_time.tv_usec / 1000000.0))
                      -
                      (start_time.tv_sec + (start_time.tv_usec / 1000000.0)));

      printf("%s TIMER[%.4f]\n", match ? "MATCH" : "FAIL", delta);
      return 0;
  }
#endif

  /* We come back here after a match when the -o option (only_matching) is set,
  in order to find any further matches in the same line. */

  ONLY_MATCHING_RESTART:

  /* Run through all the patterns until one matches. Note that we don't include
  the final newline in the subject string. */

  for (i = 0; i < pattern_count; i++)
    {
    mrc = pcre_exec(pattern_list[i], hints_list[i], matchptr, length, 0, 0,
      offsets, 99);
    if (mrc >= 0) { match = TRUE; break; }
    if (mrc != PCRE_ERROR_NOMATCH)
      {
      fprintf(stderr, "pcregrep: pcre_exec() error %d while matching ", mrc);
      if (pattern_count > 1) fprintf(stderr, "pattern number %d to ", i+1);
      fprintf(stderr, "this line:\n");
      fwrite(matchptr, 1, linelength, stderr);  /* In case binary zero included */
      fprintf(stderr, "\n");
      if (error_count == 0 &&
          (mrc == PCRE_ERROR_MATCHLIMIT || mrc == PCRE_ERROR_RECURSIONLIMIT))
        {
        fprintf(stderr, "pcregrep: error %d means that a resource limit "
          "was exceeded\n", mrc);
        fprintf(stderr, "pcregrep: check your regex for nested unlimited loops\n");
        }
      if (error_count++ > 20)
        {
        fprintf(stderr, "pcregrep: too many errors - abandoned\n");
        exit(2);
        }
      match = invert;    /* No more matching; don't show the line again */
      break;
      }
    }

  /* If it's a match or a not-match (as required), do what's wanted. */

  if (match != invert)
    {
    BOOL hyphenprinted = FALSE;

    /* We've failed if we want a file that doesn't have any matches. */

    if (filenames == FN_NOMATCH_ONLY) return 1;

    /* Just count if just counting is wanted. */

    if (count_only) count++;

    /* If all we want is a file name, there is no need to scan any more lines
    in the file. */

    else if (filenames == FN_ONLY)
      {
      fprintf(stdout, "%s\n", printname);
      return 0;
      }

    /* Likewise, if all we want is a yes/no answer. */

    else if (quiet) return 0;

    /* The --only-matching option prints just the substring that matched, and
    the --file-offsets and --line-offsets options output offsets for the
    matching substring (they both force --only-matching). None of these options
    prints any context. Afterwards, adjust the start and length, and then jump
    back to look for further matches in the same line. If we are in invert
    mode, however, nothing is printed - this could be still useful because the
    return code is set. */

    else if (only_matching)
      {
      if (!invert)
        {
        if (printname != NULL) fprintf(stdout, "%s:", printname);
        if (number) fprintf(stdout, "%d:", linenumber);
        if (line_offsets)
          fprintf(stdout, "%d,%d", (int)(matchptr + offsets[0] - ptr),
            offsets[1] - offsets[0]);
        else if (file_offsets)
          fprintf(stdout, "%d,%d", (int)(filepos + matchptr + offsets[0] - ptr),
            offsets[1] - offsets[0]);
        else
          fwrite(matchptr + offsets[0], 1, offsets[1] - offsets[0], stdout);
        fprintf(stdout, "\n");
        matchptr += offsets[1];
        length -= offsets[1];
        match = FALSE;
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
          fwrite(lastmatchrestart, 1, pp - lastmatchrestart, stdout);
          lastmatchrestart = pp;
          }
        if (lastmatchrestart != ptr) hyphenpending = TRUE;
        }

      /* If there were non-contiguous lines printed above, insert hyphens. */

      if (hyphenpending)
        {
        fprintf(stdout, "--\n");
        hyphenpending = FALSE;
        hyphenprinted = TRUE;
        }

      /* See if there is a requirement to print some "before" lines for this
      match. Again, don't print overlaps. */

      if (before_context > 0)
        {
        int linecount = 0;
        char *p = ptr;

        while (p > buffer && (lastmatchnumber == 0 || p > lastmatchrestart) &&
               linecount < before_context)
          {
          linecount++;
          p = previous_line(p, buffer);
          }

        if (lastmatchnumber > 0 && p > lastmatchrestart && !hyphenprinted)
          fprintf(stdout, "--\n");

        while (p < ptr)
          {
          int ellength;
          char *pp = p;
          if (printname != NULL) fprintf(stdout, "%s-", printname);
          if (number) fprintf(stdout, "%d-", linenumber - linecount--);
          pp = end_of_line(pp, endptr, &ellength);
          fwrite(p, 1, pp - p, stdout);
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
      (invert not set). Because the PCRE_FIRSTLINE option is set, the start of
      the match will always be before the first newline sequence. */

      if (multiline)
        {
        int ellength;
        char *endmatch = ptr;
        if (!invert)
          {
          endmatch += offsets[1];
          t = ptr;
          while (t < endmatch)
            {
            t = end_of_line(t, endptr, &ellength);
            if (t <= endmatch) linenumber++; else break;
            }
          }
        endmatch = end_of_line(endmatch, endptr, &ellength);
        linelength = endmatch - ptr - ellength;
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
        fwrite(ptr, 1, offsets[first], stdout);
        fprintf(stdout, "X");
        fwrite(ptr + offsets[last], 1, linelength - offsets[last], stdout);
        }
      else
#endif

      /* We have to split the line(s) up if colouring. */

      if (do_colour)
        {
        fwrite(ptr, 1, offsets[0], stdout);
        fprintf(stdout, "%c[%sm", 0x1b, colour_string);
        fwrite(ptr + offsets[0], 1, offsets[1] - offsets[0], stdout);
        fprintf(stdout, "%c[00m", 0x1b);
        fwrite(ptr + offsets[1], 1, (linelength + endlinelength) - offsets[1],
          stdout);
        }
      else fwrite(ptr, 1, linelength + endlinelength, stdout);
      }

    /* End of doing what has to be done for a match */

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

  ptr += linelength + endlinelength;
  filepos += linelength + endlinelength;
  linenumber++;

  /* If we haven't yet reached the end of the file (the buffer is full), and
  the current point is in the top 1/3 of the buffer, slide the buffer down by
  1/3 and refill it. Before we do this, if some unprinted "after" lines are
  about to be lost, print them. */

  if (bufflength >= sizeof(buffer) && ptr > buffer + 2*MBUFTHIRD)
    {
    if (after_context > 0 &&
        lastmatchnumber > 0 &&
        lastmatchrestart < buffer + MBUFTHIRD)
      {
      do_after_lines(lastmatchnumber, lastmatchrestart, endptr, printname);
      lastmatchnumber = 0;
      }

    /* Now do the shuffle */

    memmove(buffer, buffer + MBUFTHIRD, 2*MBUFTHIRD);
    ptr -= MBUFTHIRD;

#ifdef SUPPORT_LIBZ
    if (frtype == FR_LIBZ)
      bufflength = 2*MBUFTHIRD +
        gzread (ingz, buffer + 2*MBUFTHIRD, MBUFTHIRD);
    else
#endif

#ifdef SUPPORT_LIBBZ2
    if (frtype == FR_LIBBZ2)
      bufflength = 2*MBUFTHIRD +
        BZ2_bzread(inbz2, buffer + 2*MBUFTHIRD, MBUFTHIRD);
    else
#endif

    bufflength = 2*MBUFTHIRD + fread(buffer + 2*MBUFTHIRD, 1, MBUFTHIRD, in);

    endptr = buffer + bufflength;

    /* Adjust any last match point */

    if (lastmatchnumber > 0) lastmatchrestart -= MBUFTHIRD;
    }
  }     /* Loop through the whole file */

/* End of file; print final "after" lines if wanted; do_after_lines sets
hyphenpending if it prints something. */

if (!only_matching && !count_only)
  {
  do_after_lines(lastmatchnumber, lastmatchrestart, endptr, printname);
  hyphenpending |= endhyphenpending;
  }

/* Print the file name if we are looking for those without matches and there
were none. If we found a match, we won't have got this far. */

if (filenames == FN_NOMATCH_ONLY)
  {
  fprintf(stdout, "%s\n", printname);
  return 0;
  }

/* Print the match count if wanted */

if (count_only)
  {
  if (printname != NULL) fprintf(stdout, "%s:", printname);
  fprintf(stdout, "%d\n", count);
  }

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

Returns:   0 if there was at least one match
           1 if there were no matches
           2 there was some kind of error

However, file opening failures are suppressed if "silent" is set.
*/

static int
grep_or_recurse(char *pathname, BOOL dir_recurse, BOOL only_one_at_top)
{
int rc = 1;
int sep;
int frtype;
int pathlen;
void *handle;
FILE *in = NULL;           /* Ensure initialized */

#ifdef SUPPORT_LIBZ
gzFile ingz = NULL;
#endif

#ifdef SUPPORT_LIBBZ2
BZFILE *inbz2 = NULL;
#endif

/* If the file name is "-" we scan stdin */

if (strcmp(pathname, "-") == 0)
  {
  return pcregrep(stdin, FR_PLAIN,
    (filenames > FN_DEFAULT || (filenames == FN_DEFAULT && !only_one_at_top))?
      stdin_name : NULL);
  }

/* If the file is a directory, skip if skipping or if we are recursing, scan
each file and directory within it, subject to any include or exclude patterns
that were set. The scanning code is localized so it can be made
system-specific. */

if ((sep = isdirectory(pathname)) != 0)
  {
  if (dee_action == dee_SKIP) return 1;
  if (dee_action == dee_RECURSE)
    {
    char buffer[1024];
    char *nextfile;
    directory_type *dir = opendirectory(pathname);

    if (dir == NULL)
      {
      if (!silent)
        fprintf(stderr, "pcregrep: Failed to open directory %s: %s\n", pathname,
          strerror(errno));
      return 2;
      }

    while ((nextfile = readdirectory(dir)) != NULL)
      {
      int frc, nflen;
      sprintf(buffer, "%.512s%c%.128s", pathname, sep, nextfile);
      nflen = strlen(nextfile);

      if (isdirectory(buffer))
        {
        if (exclude_dir_compiled != NULL &&
            pcre_exec(exclude_dir_compiled, NULL, nextfile, nflen, 0, 0, NULL, 0) >= 0)
          continue;

        if (include_dir_compiled != NULL &&
            pcre_exec(include_dir_compiled, NULL, nextfile, nflen, 0, 0, NULL, 0) < 0)
          continue;
        }
      else
        {
        if (exclude_compiled != NULL &&
            pcre_exec(exclude_compiled, NULL, nextfile, nflen, 0, 0, NULL, 0) >= 0)
          continue;

        if (include_compiled != NULL &&
            pcre_exec(include_compiled, NULL, nextfile, nflen, 0, 0, NULL, 0) < 0)
          continue;
        }

      frc = grep_or_recurse(buffer, dir_recurse, FALSE);
      if (frc > 1) rc = frc;
       else if (frc == 0 && rc == 1) rc = 0;
      }

    closedirectory(dir);
    return rc;
    }
  }

/* If the file is not a directory and not a regular file, skip it if that's
been requested. */

else if (!isregfile(pathname) && DEE_action == DEE_SKIP) return 1;

/* Control reaches here if we have a regular file, or if we have a directory
and recursion or skipping was not requested, or if we have anything else and
skipping was not requested. The scan proceeds. If this is the first and only
argument at top level, we don't show the file name, unless we are only showing
the file name, or the filename was forced (-H). */

pathlen = strlen(pathname);

/* Open using zlib if it is supported and the file name ends with .gz. */

#ifdef SUPPORT_LIBZ
if (pathlen > 3 && strcmp(pathname + pathlen - 3, ".gz") == 0)
  {
  ingz = gzopen(pathname, "rb");
  if (ingz == NULL)
    {
    if (!silent)
      fprintf(stderr, "pcregrep: Failed to open %s: %s\n", pathname,
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
  in = fopen(pathname, "r");
  handle = (void *)in;
  frtype = FR_PLAIN;
  }

/* All the opening methods return errno when they fail. */

if (handle == NULL)
  {
  if (!silent)
    fprintf(stderr, "pcregrep: Failed to open %s: %s\n", pathname,
      strerror(errno));
  return 2;
  }

/* Now grep the file */

rc = pcregrep(handle, frtype, (filenames > FN_DEFAULT ||
  (filenames == FN_DEFAULT && !only_one_at_top))? pathname : NULL);

/* Close in an appropriate manner. */

#ifdef SUPPORT_LIBZ
if (frtype == FR_LIBZ)
  gzclose(ingz);
else
#endif

/* If it is a .bz2 file and the result is 2, it means that the first attempt to
read failed. If the error indicates that the file isn't in fact bzipped, try
again as a normal file. */

#ifdef SUPPORT_LIBBZ2
if (frtype == FR_LIBBZ2)
  {
  if (rc == 2)
    {
    int errnum;
    const char *err = BZ2_bzerror(inbz2, &errnum);
    if (errnum == BZ_DATA_ERROR_MAGIC)
      {
      BZ2_bzclose(inbz2);
      goto PLAIN_FILE;
      }
    else if (!silent)
      fprintf(stderr, "pcregrep: Failed to read %s using bzlib: %s\n",
        pathname, err);
    }
  BZ2_bzclose(inbz2);
  }
else
#endif

/* Normal file close */

fclose(in);

/* Pass back the yield from pcregrep(). */

return rc;
}




/*************************************************
*                Usage function                  *
*************************************************/

static int
usage(int rc)
{
option_item *op;
fprintf(stderr, "Usage: pcregrep [-");
for (op = optionlist; op->one_char != 0; op++)
  {
  if (op->one_char > 0) fprintf(stderr, "%c", op->one_char);
  }
fprintf(stderr, "] [long options] [pattern] [files]\n");
fprintf(stderr, "Type `pcregrep --help' for more information and the long "
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

printf("Usage: pcregrep [OPTION]... [PATTERN] [FILE1 FILE2 ...]\n");
printf("Search for PATTERN in each FILE or standard input.\n");
printf("PATTERN must be present if neither -e nor -f is used.\n");
printf("\"-\" can be used as a file name to mean STDIN.\n");

#ifdef SUPPORT_LIBZ
printf("Files whose names end in .gz are read using zlib.\n");
#endif

#ifdef SUPPORT_LIBBZ2
printf("Files whose names end in .bz2 are read using bzlib2.\n");
#endif

#if defined SUPPORT_LIBZ || defined SUPPORT_LIBBZ2
printf("Other files and the standard input are read as plain files.\n\n");
#else
printf("All files are read as plain files, without any interpretation.\n\n");
#endif

printf("Example: pcregrep -i 'hello.*world' menu.h main.c\n\n");
printf("Options:\n");

for (op = optionlist; op->one_char != 0; op++)
  {
  int n;
  char s[4];
  if (op->one_char > 0) sprintf(s, "-%c,", op->one_char); else strcpy(s, "   ");
  n = 30 - printf("  %s --%s", s, op->long_name);
  if (n < 1) n = 1;
  printf("%.*s%s\n", n, "                    ", op->help_text);
  }

printf("\nWhen reading patterns from a file instead of using a command line option,\n");
printf("trailing white space is removed and blank lines are ignored.\n");
printf("There is a maximum of %d patterns.\n", MAX_PATTERN_COUNT);

printf("\nWith no FILEs, read standard input. If fewer than two FILEs given, assume -h.\n");
printf("Exit status is 0 if any matches, 1 if no matches, and 2 if trouble.\n");
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
  case N_HELP: help(); exit(0);
  case N_LOFFSETS: line_offsets = number = TRUE; break;
  case 'c': count_only = TRUE; break;
  case 'F': process_options |= PO_FIXED_STRINGS; break;
  case 'H': filenames = FN_FORCE; break;
  case 'h': filenames = FN_NONE; break;
  case 'i': options |= PCRE_CASELESS; break;
  case 'l': filenames = FN_ONLY; break;
  case 'L': filenames = FN_NOMATCH_ONLY; break;
  case 'M': multiline = TRUE; options |= PCRE_MULTILINE|PCRE_FIRSTLINE; break;
  case 'n': number = TRUE; break;
  case 'o': only_matching = TRUE; break;
  case 'q': quiet = TRUE; break;
  case 'r': dee_action = dee_RECURSE; break;
  case 's': silent = TRUE; break;
  case 'u': options |= PCRE_UTF8; utf8 = TRUE; break;
  case 'v': invert = TRUE; break;
  case 'w': process_options |= PO_WORD_MATCH; break;
  case 'x': process_options |= PO_LINE_MATCH; break;

  case 'V':
  fprintf(stderr, "pcregrep version %s\n", pcre_version());
  exit(0);
  break;

  default:
  fprintf(stderr, "pcregrep: Unknown option -%c\n", letter);
  exit(usage(2));
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
static char buffer[8];
char *p = buffer;
sprintf(p, "%d", n);
while (*p != 0) p++;
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

/* When the -F option has been used, this is called for each substring.
Otherwise it's called for each supplied pattern.

Arguments:
  pattern        the pattern string
  options        the PCRE options
  filename       the file name, or NULL for a command-line pattern
  count          0 if this is the only command line pattern, or
                 number of the command line pattern, or
                 linenumber for a pattern from a file

Returns:         TRUE on success, FALSE after an error
*/

static BOOL
compile_single_pattern(char *pattern, int options, char *filename, int count)
{
char buffer[MBUFTHIRD + 16];
const char *error;
int errptr;

if (pattern_count >= MAX_PATTERN_COUNT)
  {
  fprintf(stderr, "pcregrep: Too many %spatterns (max %d)\n",
    (filename == NULL)? "command-line " : "", MAX_PATTERN_COUNT);
  return FALSE;
  }

sprintf(buffer, "%s%.*s%s", prefix[process_options], MBUFTHIRD, pattern,
  suffix[process_options]);
pattern_list[pattern_count] =
  pcre_compile(buffer, options, &error, &errptr, pcretables);
if (pattern_list[pattern_count] != NULL)
  {
  pattern_count++;
  return TRUE;
  }

/* Handle compile errors */

errptr -= (int)strlen(prefix[process_options]);
if (errptr > (int)strlen(pattern)) errptr = (int)strlen(pattern);

if (filename == NULL)
  {
  if (count == 0)
    fprintf(stderr, "pcregrep: Error in command-line regex "
      "at offset %d: %s\n", errptr, error);
  else
    fprintf(stderr, "pcregrep: Error in %s command-line regex "
      "at offset %d: %s\n", ordin(count), errptr, error);
  }
else
  {
  fprintf(stderr, "pcregrep: Error in regex in line %d of %s "
    "at offset %d: %s\n", count, filename, errptr, error);
  }

return FALSE;
}



/*************************************************
*           Compile one supplied pattern         *
*************************************************/

/* When the -F option has been used, each string may be a list of strings,
separated by line breaks. They will be matched literally.

Arguments:
  pattern        the pattern string
  options        the PCRE options
  filename       the file name, or NULL for a command-line pattern
  count          0 if this is the only command line pattern, or
                 number of the command line pattern, or
                 linenumber for a pattern from a file

Returns:         TRUE on success, FALSE after an error
*/

static BOOL
compile_pattern(char *pattern, int options, char *filename, int count)
{
if ((process_options & PO_FIXED_STRINGS) != 0)
  {
  char *eop = pattern + strlen(pattern);
  char buffer[MBUFTHIRD];
  for(;;)
    {
    int ellength;
    char *p = end_of_line(pattern, eop, &ellength);
    if (ellength == 0)
      return compile_single_pattern(pattern, options, filename, count);
    sprintf(buffer, "%.*s", (int)(p - pattern - ellength), pattern);
    pattern = p;
    if (!compile_single_pattern(buffer, options, filename, count))
      return FALSE;
    }
  }
else return compile_single_pattern(pattern, options, filename, count);
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
int pcre_options = 0;
int cmd_pattern_count = 0;
int hint_count = 0;
int errptr;
BOOL only_one_at_top;
char *patterns[MAX_PATTERN_COUNT];
const char *locale_from = "--locale";
const char *error;

/* Set the default line ending value from the default in the PCRE library;
"lf", "cr", "crlf", and "any" are supported. Anything else is treated as "lf".
*/

(void)pcre_config(PCRE_CONFIG_NEWLINE, &i);
switch(i)
  {
  default:                 newline = (char *)"lf"; break;
  case '\r':               newline = (char *)"cr"; break;
  case ('\r' << 8) | '\n': newline = (char *)"crlf"; break;
  case -1:                 newline = (char *)"any"; break;
  case -2:                 newline = (char *)"anycrlf"; break;
  }

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
    if (pattern_filename != NULL || pattern_count > 0) break;
      else exit(usage(2));
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
    These options are entered in the table as "regex(p)". No option is in both
    these categories, fortunately. */

    for (op = optionlist; op->one_char != 0; op++)
      {
      char *opbra = strchr(op->long_name, '(');
      char *equals = strchr(op->long_name, '=');
      if (opbra == NULL)     /* Not a (p) case */
        {
        if (equals == NULL)  /* Not thing=data case */
          {
          if (strcmp(arg, op->long_name) == 0) break;
          }
        else                 /* Special case xxx=data */
          {
          int oplen = equals - op->long_name;
          int arglen = (argequals == NULL)? (int)strlen(arg) : argequals - arg;
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
      else                   /* Special case xxxx(p) */
        {
        char buff1[24];
        char buff2[24];
        int baselen = opbra - op->long_name;
        sprintf(buff1, "%.*s", baselen, op->long_name);
        sprintf(buff2, "%s%.*s", buff1,
          (int)strlen(op->long_name) - baselen - 2, opbra + 1);
        if (strcmp(arg, buff1) == 0 || strcmp(arg, buff2) == 0)
          break;
        }
      }

    if (op->one_char == 0)
      {
      fprintf(stderr, "pcregrep: Unknown option %s\n", argv[i]);
      exit(usage(2));
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
        { if (*s == op->one_char) break; }
      if (op->one_char == 0)
        {
        fprintf(stderr, "pcregrep: Unknown option letter '%c' in \"%s\"\n",
          *s, argv[i]);
        exit(usage(2));
        }
      if (op->type != OP_NODATA || s[1] == 0)
        {
        option_data = s+1;
        break;
        }
      pcre_options = handle_option(*s++, pcre_options);
      }
    }

  /* At this point we should have op pointing to a matched option. If the type
  is NO_DATA, it means that there is no data, and the option might set
  something in the PCRE options. */

  if (op->type == OP_NODATA)
    {
    pcre_options = handle_option(op->one_char, pcre_options);
    continue;
    }

  /* If the option type is OP_OP_STRING or OP_OP_NUMBER, it's an option that
  either has a value or defaults to something. It cannot have data in a
  separate item. At the moment, the only such options are "colo(u)r" and
  Jeffrey Friedl's special -S debugging option. */

  if (*option_data == 0 &&
      (op->type == OP_OP_STRING || op->type == OP_OP_NUMBER))
    {
    switch (op->one_char)
      {
      case N_COLOUR:
      colour_option = (char *)"auto";
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
      fprintf(stderr, "pcregrep: Data missing after %s\n", argv[i]);
      exit(usage(2));
      }
    option_data = argv[++i];
    }

  /* If the option type is OP_PATLIST, it's the -e option, which can be called
  multiple times to create a list of patterns. */

  if (op->type == OP_PATLIST)
    {
    if (cmd_pattern_count >= MAX_PATTERN_COUNT)
      {
      fprintf(stderr, "pcregrep: Too many command-line patterns (max %d)\n",
        MAX_PATTERN_COUNT);
      return 2;
      }
    patterns[cmd_pattern_count++] = option_data;
    }

  /* Otherwise, deal with single string or numeric data values. */

  else if (op->type != OP_NUMBER && op->type != OP_OP_NUMBER)
    {
    *((char **)op->dataptr) = option_data;
    }
  else
    {
    char *endptr;
    int n = strtoul(option_data, &endptr, 10);
    if (*endptr != 0)
      {
      if (longop)
        {
        char *equals = strchr(op->long_name, '=');
        int nlen = (equals == NULL)? (int)strlen(op->long_name) :
          equals - op->long_name;
        fprintf(stderr, "pcregrep: Malformed number \"%s\" after --%.*s\n",
          option_data, nlen, op->long_name);
        }
      else
        fprintf(stderr, "pcregrep: Malformed number \"%s\" after -%c\n",
          option_data, op->one_char);
      exit(usage(2));
      }
    *((int *)op->dataptr) = n;
    }
  }

/* Options have been decoded. If -C was used, its value is used as a default
for -A and -B. */

if (both_context > 0)
  {
  if (after_context == 0) after_context = both_context;
  if (before_context == 0) before_context = both_context;
  }

/* Only one of --only-matching, --file-offsets, or --line-offsets is permitted.
However, the latter two set the only_matching flag. */

if ((only_matching && (file_offsets || line_offsets)) ||
    (file_offsets && line_offsets))
  {
  fprintf(stderr, "pcregrep: Cannot mix --only-matching, --file-offsets "
    "and/or --line-offsets\n");
  exit(usage(2));
  }

if (file_offsets || line_offsets) only_matching = TRUE;

/* If a locale has not been provided as an option, see if the LC_CTYPE or
LC_ALL environment variable is set, and if so, use it. */

if (locale == NULL)
  {
  locale = getenv("LC_ALL");
  locale_from = "LCC_ALL";
  }

if (locale == NULL)
  {
  locale = getenv("LC_CTYPE");
  locale_from = "LC_CTYPE";
  }

/* If a locale has been provided, set it, and generate the tables the PCRE
needs. Otherwise, pcretables==NULL, which causes the use of default tables. */

if (locale != NULL)
  {
  if (setlocale(LC_CTYPE, locale) == NULL)
    {
    fprintf(stderr, "pcregrep: Failed to set locale %s (obtained from %s)\n",
      locale, locale_from);
    return 2;
    }
  pcretables = pcre_maketables();
  }

/* Sort out colouring */

if (colour_option != NULL && strcmp(colour_option, "never") != 0)
  {
  if (strcmp(colour_option, "always") == 0) do_colour = TRUE;
  else if (strcmp(colour_option, "auto") == 0) do_colour = is_stdout_tty();
  else
    {
    fprintf(stderr, "pcregrep: Unknown colour setting \"%s\"\n",
      colour_option);
    return 2;
    }
  if (do_colour)
    {
    char *cs = getenv("PCREGREP_COLOUR");
    if (cs == NULL) cs = getenv("PCREGREP_COLOR");
    if (cs != NULL) colour_string = cs;
    }
  }

/* Interpret the newline type; the default settings are Unix-like. */

if (strcmp(newline, "cr") == 0 || strcmp(newline, "CR") == 0)
  {
  pcre_options |= PCRE_NEWLINE_CR;
  endlinetype = EL_CR;
  }
else if (strcmp(newline, "lf") == 0 || strcmp(newline, "LF") == 0)
  {
  pcre_options |= PCRE_NEWLINE_LF;
  endlinetype = EL_LF;
  }
else if (strcmp(newline, "crlf") == 0 || strcmp(newline, "CRLF") == 0)
  {
  pcre_options |= PCRE_NEWLINE_CRLF;
  endlinetype = EL_CRLF;
  }
else if (strcmp(newline, "any") == 0 || strcmp(newline, "ANY") == 0)
  {
  pcre_options |= PCRE_NEWLINE_ANY;
  endlinetype = EL_ANY;
  }
else if (strcmp(newline, "anycrlf") == 0 || strcmp(newline, "ANYCRLF") == 0)
  {
  pcre_options |= PCRE_NEWLINE_ANYCRLF;
  endlinetype = EL_ANYCRLF;
  }
else
  {
  fprintf(stderr, "pcregrep: Invalid newline specifier \"%s\"\n", newline);
  return 2;
  }

/* Interpret the text values for -d and -D */

if (dee_option != NULL)
  {
  if (strcmp(dee_option, "read") == 0) dee_action = dee_READ;
  else if (strcmp(dee_option, "recurse") == 0) dee_action = dee_RECURSE;
  else if (strcmp(dee_option, "skip") == 0) dee_action = dee_SKIP;
  else
    {
    fprintf(stderr, "pcregrep: Invalid value \"%s\" for -d\n", dee_option);
    return 2;
    }
  }

if (DEE_option != NULL)
  {
  if (strcmp(DEE_option, "read") == 0) DEE_action = DEE_READ;
  else if (strcmp(DEE_option, "skip") == 0) DEE_action = DEE_SKIP;
  else
    {
    fprintf(stderr, "pcregrep: Invalid value \"%s\" for -D\n", DEE_option);
    return 2;
    }
  }

/* Check the values for Jeffrey Friedl's debugging options. */

#ifdef JFRIEDL_DEBUG
if (S_arg > 9)
  {
  fprintf(stderr, "pcregrep: bad value for -S option\n");
  return 2;
  }
if (jfriedl_XT != 0 || jfriedl_XR != 0)
  {
  if (jfriedl_XT == 0) jfriedl_XT = 1;
  if (jfriedl_XR == 0) jfriedl_XR = 1;
  }
#endif

/* Get memory to store the pattern and hints lists. */

pattern_list = (pcre **)malloc(MAX_PATTERN_COUNT * sizeof(pcre *));
hints_list = (pcre_extra **)malloc(MAX_PATTERN_COUNT * sizeof(pcre_extra *));

if (pattern_list == NULL || hints_list == NULL)
  {
  fprintf(stderr, "pcregrep: malloc failed\n");
  goto EXIT2;
  }

/* If no patterns were provided by -e, and there is no file provided by -f,
the first argument is the one and only pattern, and it must exist. */

if (cmd_pattern_count == 0 && pattern_filename == NULL)
  {
  if (i >= argc) return usage(2);
  patterns[cmd_pattern_count++] = argv[i++];
  }

/* Compile the patterns that were provided on the command line, either by
multiple uses of -e or as a single unkeyed pattern. */

for (j = 0; j < cmd_pattern_count; j++)
  {
  if (!compile_pattern(patterns[j], pcre_options, NULL,
       (j == 0 && cmd_pattern_count == 1)? 0 : j + 1))
    goto EXIT2;
  }

/* Compile the regular expressions that are provided in a file. */

if (pattern_filename != NULL)
  {
  int linenumber = 0;
  FILE *f;
  char *filename;
  char buffer[MBUFTHIRD];

  if (strcmp(pattern_filename, "-") == 0)
    {
    f = stdin;
    filename = stdin_name;
    }
  else
    {
    f = fopen(pattern_filename, "r");
    if (f == NULL)
      {
      fprintf(stderr, "pcregrep: Failed to open %s: %s\n", pattern_filename,
        strerror(errno));
      goto EXIT2;
      }
    filename = pattern_filename;
    }

  while (fgets(buffer, MBUFTHIRD, f) != NULL)
    {
    char *s = buffer + (int)strlen(buffer);
    while (s > buffer && isspace((unsigned char)(s[-1]))) s--;
    *s = 0;
    linenumber++;
    if (buffer[0] == 0) continue;   /* Skip blank lines */
    if (!compile_pattern(buffer, pcre_options, filename, linenumber))
      goto EXIT2;
    }

  if (f != stdin) fclose(f);
  }

/* Study the regular expressions, as we will be running them many times */

for (j = 0; j < pattern_count; j++)
  {
  hints_list[j] = pcre_study(pattern_list[j], 0, &error);
  if (error != NULL)
    {
    char s[16];
    if (pattern_count == 1) s[0] = 0; else sprintf(s, " number %d", j);
    fprintf(stderr, "pcregrep: Error while studying regex%s: %s\n", s, error);
    goto EXIT2;
    }
  hint_count++;
  }

/* If there are include or exclude patterns, compile them. */

if (exclude_pattern != NULL)
  {
  exclude_compiled = pcre_compile(exclude_pattern, 0, &error, &errptr,
    pcretables);
  if (exclude_compiled == NULL)
    {
    fprintf(stderr, "pcregrep: Error in 'exclude' regex at offset %d: %s\n",
      errptr, error);
    goto EXIT2;
    }
  }

if (include_pattern != NULL)
  {
  include_compiled = pcre_compile(include_pattern, 0, &error, &errptr,
    pcretables);
  if (include_compiled == NULL)
    {
    fprintf(stderr, "pcregrep: Error in 'include' regex at offset %d: %s\n",
      errptr, error);
    goto EXIT2;
    }
  }

if (exclude_dir_pattern != NULL)
  {
  exclude_dir_compiled = pcre_compile(exclude_dir_pattern, 0, &error, &errptr,
    pcretables);
  if (exclude_dir_compiled == NULL)
    {
    fprintf(stderr, "pcregrep: Error in 'exclude_dir' regex at offset %d: %s\n",
      errptr, error);
    goto EXIT2;
    }
  }

if (include_dir_pattern != NULL)
  {
  include_dir_compiled = pcre_compile(include_dir_pattern, 0, &error, &errptr,
    pcretables);
  if (include_dir_compiled == NULL)
    {
    fprintf(stderr, "pcregrep: Error in 'include_dir' regex at offset %d: %s\n",
      errptr, error);
    goto EXIT2;
    }
  }

/* If there are no further arguments, do the business on stdin and exit. */

if (i >= argc)
  {
  rc = pcregrep(stdin, FR_PLAIN, (filenames > FN_DEFAULT)? stdin_name : NULL);
  goto EXIT;
  }

/* Otherwise, work through the remaining arguments as files or directories.
Pass in the fact that there is only one argument at top level - this suppresses
the file name if the argument is not a directory and filenames are not
otherwise forced. */

only_one_at_top = i == argc - 1;   /* Catch initial value of i */

for (; i < argc; i++)
  {
  int frc = grep_or_recurse(argv[i], dee_action == dee_RECURSE,
    only_one_at_top);
  if (frc > 1) rc = frc;
    else if (frc == 0 && rc == 1) rc = 0;
  }

EXIT:
if (pattern_list != NULL)
  {
  for (i = 0; i < pattern_count; i++) free(pattern_list[i]);
  free(pattern_list);
  }
if (hints_list != NULL)
  {
  for (i = 0; i < hint_count; i++) free(hints_list[i]);
  free(hints_list);
  }
return rc;

EXIT2:
rc = 2;
goto EXIT;
}

/* End of pcregrep */
