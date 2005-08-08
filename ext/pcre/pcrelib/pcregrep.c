/*************************************************
*               pcregrep program                 *
*************************************************/

/* This is a grep program that uses the PCRE regular expression library to do
its pattern matching. On a Unix or Win32 system it can recurse into
directories.

           Copyright (c) 1997-2005 University of Cambridge

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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "pcre.h"

#define FALSE 0
#define TRUE 1

typedef int BOOL;

#define VERSION "4.0 07-Jun-2005"
#define MAX_PATTERN_COUNT 100

#if BUFSIZ > 8192
#define MBUFTHIRD BUFSIZ
#else
#define MBUFTHIRD 8192
#endif



/*************************************************
*               Global variables                 *
*************************************************/

static char *pattern_filename = NULL;
static char *stdin_name = (char *)"(standard input)";
static int  pattern_count = 0;
static pcre **pattern_list;
static pcre_extra **hints_list;

static char *include_pattern = NULL;
static char *exclude_pattern = NULL;

static pcre *include_compiled = NULL;
static pcre *exclude_compiled = NULL;

static int after_context = 0;
static int before_context = 0;
static int both_context = 0;

static BOOL count_only = FALSE;
static BOOL filenames = TRUE;
static BOOL filenames_only = FALSE;
static BOOL filenames_nomatch_only = FALSE;
static BOOL hyphenpending = FALSE;
static BOOL invert = FALSE;
static BOOL multiline = FALSE;
static BOOL number = FALSE;
static BOOL quiet = FALSE;
static BOOL recurse = FALSE;
static BOOL silent = FALSE;
static BOOL whole_lines = FALSE;
static BOOL word_match = FALSE;

/* Structure for options and list of them */

enum { OP_NODATA, OP_STRING, OP_NUMBER };

typedef struct option_item {
  int type;
  int one_char;
  void *dataptr;
  const char *long_name;
  const char *help_text;
} option_item;

static option_item optionlist[] = {
  { OP_NODATA, -1,  NULL,              "",              "  terminate options" },
  { OP_NODATA, -1,  NULL,              "help",          "display this help and exit" },
  { OP_NUMBER, 'A', &after_context,    "after-context=number", "set number of following context lines" },
  { OP_NUMBER, 'B', &before_context,   "before-context=number", "set number of prior context lines" },
  { OP_NUMBER, 'C', &both_context,     "context=number", "set number of context lines, before & after" },
  { OP_NODATA, 'c', NULL,              "count",         "print only a count of matching lines per FILE" },
  { OP_STRING, 'f', &pattern_filename, "file=path",     "read patterns from file" },
  { OP_NODATA, 'h', NULL,              "no-filename",   "suppress the prefixing filename on output" },
  { OP_NODATA, 'i', NULL,              "ignore-case",   "ignore case distinctions" },
  { OP_NODATA, 'l', NULL,              "files-with-matches", "print only FILE names containing matches" },
  { OP_NODATA, 'L', NULL,              "files-without-match","print only FILE names not containing matches" },
  { OP_STRING, -1,  &stdin_name,       "label=name",    "set name for standard input" },
  { OP_NODATA, 'M', NULL,              "multiline",     "run in multiline mode" },
  { OP_NODATA, 'n', NULL,              "line-number",   "print line number with output lines" },
  { OP_NODATA, 'q', NULL,              "quiet",         "suppress output, just set return code" },
  { OP_NODATA, 'r', NULL,              "recursive",     "recursively scan sub-directories" },
  { OP_STRING, -1,  &exclude_pattern,  "exclude=pattern","exclude matching files when recursing" },
  { OP_STRING, -1,  &include_pattern,  "include=pattern","include matching files when recursing" },
  { OP_NODATA, 's', NULL,              "no-messages",   "suppress error messages" },
  { OP_NODATA, 'u', NULL,              "utf-8",         "use UTF-8 mode" },
  { OP_NODATA, 'V', NULL,              "version",       "print version information and exit" },
  { OP_NODATA, 'v', NULL,              "invert-match",  "select non-matching lines" },
  { OP_NODATA, 'w', NULL,              "word-regex(p)", "force PATTERN to match only as a word"  },
  { OP_NODATA, 'x', NULL,              "line-regex(p)", "force PATTERN to match only whole lines" },
  { OP_NODATA, 0,   NULL,               NULL,            NULL }
};


/*************************************************
*       Functions for directory scanning         *
*************************************************/

/* These functions are defined so that they can be made system specific,
although at present the only ones are for Unix, Win32, and for "no directory
recursion support". */


/************* Directory scanning in Unix ***********/

#if IS_UNIX
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
return NULL;   /* Keep compiler happy; never executed */
}

static void
closedirectory(directory_type *dir)
{
closedir(dir);
}


/************* Directory scanning in Win32 ***********/

/* I (Philip Hazel) have no means of testing this code. It was contributed by
Lionel Fourquaux. David Burgess added a patch to define INVALID_FILE_ATTRIBUTES
when it did not exist. */


#elif HAVE_WIN32API

#ifndef STRICT
# define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF
#endif

#include <windows.h>

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


/************* Directory scanning when we can't do it ***********/

/* The type is void, and apart from isdirectory(), the functions do nothing. */

#else

typedef void directory_type;

int isdirectory(char *filename) { return FALSE; }
directory_type * opendirectory(char *filename) {}
char *readdirectory(directory_type *dir) {}
void closedirectory(directory_type *dir) {}

#endif



#if ! HAVE_STRERROR
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
*       Print the previous "after" lines         *
*************************************************/

/* This is called if we are about to lose said lines because of buffer filling,
and at the end of the file.

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
    char *pp = lastmatchrestart;
    if (printname != NULL) fprintf(stdout, "%s-", printname);
    if (number) fprintf(stdout, "%d-", lastmatchnumber++);
    while (*pp != '\n') pp++;
    fprintf(stdout, "%.*s", pp - lastmatchrestart + 1, lastmatchrestart);
    lastmatchrestart = pp + 1;
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
  in           the fopened FILE stream
  printname    the file name if it is to be printed for each match
               or NULL if the file name is not to be printed
               it cannot be NULL if filenames[_nomatch]_only is set

Returns:       0 if there was at least one match
               1 otherwise (no matches)
*/

static int
pcregrep(FILE *in, char *printname)
{
int rc = 1;
int linenumber = 1;
int lastmatchnumber = 0;
int count = 0;
int offsets[99];
char *lastmatchrestart = NULL;
char buffer[3*MBUFTHIRD];
char *ptr = buffer;
char *endptr;
size_t bufflength;
BOOL endhyphenpending = FALSE;

/* Do the first read into the start of the buffer and set up the pointer to
end of what we have. */

bufflength = fread(buffer, 1, 3*MBUFTHIRD, in);
endptr = buffer + bufflength;

/* Loop while the current pointer is not at the end of the file. For large
files, endptr will be at the end of the buffer when we are in the middle of the
file, but ptr will never get there, because as soon as it gets over 2/3 of the
way, the buffer is shifted left and re-filled. */

while (ptr < endptr)
  {
  int i;
  BOOL match = FALSE;
  char *t = ptr;
  size_t length, linelength;

  /* At this point, ptr is at the start of a line. We need to find the length
  of the subject string to pass to pcre_exec(). In multiline mode, it is the
  length remainder of the data in the buffer. Otherwise, it is the length of
  the next line. After matching, we always advance by the length of the next
  line. In multiline mode the PCRE_FIRSTLINE option is used for compiling, so
  that any match is constrained to be in the first line. */

  linelength = 0;
  while (t < endptr && *t++ != '\n') linelength++;
  length = multiline? endptr - ptr : linelength;

  /* Run through all the patterns until one matches. Note that we don't include
  the final newline in the subject string. */

  for (i = 0; !match && i < pattern_count; i++)
    {
    match = pcre_exec(pattern_list[i], hints_list[i], ptr, length, 0, 0,
      offsets, 99) >= 0;
    }

  /* If it's a match or a not-match (as required), print what's wanted. */

  if (match != invert)
    {
    BOOL hyphenprinted = FALSE;

    if (filenames_nomatch_only) return 1;

    if (count_only) count++;

    else if (filenames_only)
      {
      fprintf(stdout, "%s\n", printname);
      return 0;
      }

    else if (quiet) return 0;

    else
      {
      /* See if there is a requirement to print some "after" lines from a
      previous match. We never print any overlaps. */

      if (after_context > 0 && lastmatchnumber > 0)
        {
        int linecount = 0;
        char *p = lastmatchrestart;

        while (p < ptr && linecount < after_context)
          {
          while (*p != '\n') p++;
          p++;
          linecount++;
          }

        /* It is important to advance lastmatchrestart during this printing so
        that it interacts correctly with any "before" printing below. */

        while (lastmatchrestart < p)
          {
          char *pp = lastmatchrestart;
          if (printname != NULL) fprintf(stdout, "%s-", printname);
          if (number) fprintf(stdout, "%d-", lastmatchnumber++);
          while (*pp != '\n') pp++;
          fprintf(stdout, "%.*s", pp - lastmatchrestart + 1, lastmatchrestart);
          lastmatchrestart = pp + 1;
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
               linecount++ < before_context)
          {
          p--;
          while (p > buffer && p[-1] != '\n') p--;
          }

        if (lastmatchnumber > 0 && p > lastmatchrestart && !hyphenprinted)
          fprintf(stdout, "--\n");

        while (p < ptr)
          {
          char *pp = p;
          if (printname != NULL) fprintf(stdout, "%s-", printname);
          if (number) fprintf(stdout, "%d-", linenumber - linecount--);
          while (*pp != '\n') pp++;
          fprintf(stdout, "%.*s", pp - p + 1, p);
          p = pp + 1;
          }
        }

      /* Now print the matching line(s); ensure we set hyphenpending at the end
      of the file. */

      endhyphenpending = TRUE;
      if (printname != NULL) fprintf(stdout, "%s:", printname);
      if (number) fprintf(stdout, "%d:", linenumber);

      /* In multiline mode, we want to print to the end of the line in which
      the end of the matched string is found, so we adjust linelength and the
      line number appropriately. Because the PCRE_FIRSTLINE option is set, the
      start of the match will always be before the first \n character. */

      if (multiline)
        {
        char *endmatch = ptr + offsets[1];
        t = ptr;
        while (t < endmatch) { if (*t++ == '\n') linenumber++; }
        while (endmatch < endptr && *endmatch != '\n') endmatch++;
        linelength = endmatch - ptr;
        }

      fprintf(stdout, "%.*s\n", linelength, ptr);
      }

    rc = 0;    /* Had some success */

    /* Remember where the last match happened for after_context. We remember
    where we are about to restart, and that line's number. */

    lastmatchrestart = ptr + linelength + 1;
    lastmatchnumber = linenumber + 1;
    }

  /* Advance to after the newline and increment the line number. */

  ptr += linelength + 1;
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
    bufflength = 2*MBUFTHIRD + fread(buffer + 2*MBUFTHIRD, 1, MBUFTHIRD, in);
    endptr = buffer + bufflength;

    /* Adjust any last match point */

    if (lastmatchnumber > 0) lastmatchrestart -= MBUFTHIRD;
    }
  }     /* Loop through the whole file */

/* End of file; print final "after" lines if wanted; do_after_lines sets
hyphenpending if it prints something. */

do_after_lines(lastmatchnumber, lastmatchrestart, endptr, printname);
hyphenpending |= endhyphenpending;

/* Print the file name if we are looking for those without matches and there
were none. If we found a match, we won't have got this far. */

if (filenames_nomatch_only)
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
  dir_recurse       TRUE if recursing is wanted (-r)
  show_filenames    TRUE if file names are wanted for multiple files, except
                      for the only file at top level when not filenames_only
  only_one_at_top   TRUE if the path is the only one at toplevel

Returns:   0 if there was at least one match
           1 if there were no matches
           2 there was some kind of error

However, file opening failures are suppressed if "silent" is set.
*/

static int
grep_or_recurse(char *pathname, BOOL dir_recurse, BOOL show_filenames,
  BOOL only_one_at_top)
{
int rc = 1;
int sep;
FILE *in;
char *printname;

/* If the file name is "-" we scan stdin */

if (strcmp(pathname, "-") == 0)
  {
  return pcregrep(stdin,
    (filenames_only || filenames_nomatch_only ||
    (show_filenames && !only_one_at_top))?
      stdin_name : NULL);
  }

/* If the file is a directory and we are recursing, scan each file within it,
subject to any include or exclude patterns that were set. The scanning code is
localized so it can be made system-specific. */

if ((sep = isdirectory(pathname)) != 0 && dir_recurse)
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
    int frc, blen;
    sprintf(buffer, "%.512s%c%.128s", pathname, sep, nextfile);
    blen = strlen(buffer);

    if (exclude_compiled != NULL &&
        pcre_exec(exclude_compiled, NULL, buffer, blen, 0, 0, NULL, 0) >= 0)
      continue;

    if (include_compiled != NULL &&
        pcre_exec(include_compiled, NULL, buffer, blen, 0, 0, NULL, 0) < 0)
      continue;

    frc = grep_or_recurse(buffer, dir_recurse, TRUE, FALSE);
    if (frc > 1) rc = frc;
     else if (frc == 0 && rc == 1) rc = 0;
    }

  closedirectory(dir);
  return rc;
  }

/* If the file is not a directory, or we are not recursing, scan it. If this is
the first and only argument at top level, we don't show the file name (unless
we are only showing the file name). Otherwise, control is via the
show_filenames variable. */

in = fopen(pathname, "r");
if (in == NULL)
  {
  if (!silent)
    fprintf(stderr, "pcregrep: Failed to open %s: %s\n", pathname,
      strerror(errno));
  return 2;
  }

printname =  (filenames_only || filenames_nomatch_only ||
  (show_filenames && !only_one_at_top))? pathname : NULL;

rc = pcregrep(in, printname);

fclose(in);
return rc;
}




/*************************************************
*                Usage function                  *
*************************************************/

static int
usage(int rc)
{
fprintf(stderr, "Usage: pcregrep [-LMVcfhilnqrsvwx] [long-options] [pattern] [file1 file2 ...]\n");
fprintf(stderr, "Type `pcregrep --help' for more information.\n");
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
printf("PATTERN must be present if -f is not used.\n");
printf("\"-\" can be used as a file name to mean STDIN.\n");
printf("Example: pcregrep -i 'hello.*world' menu.h main.c\n\n");

printf("Options:\n");

for (op = optionlist; op->one_char != 0; op++)
  {
  int n;
  char s[4];
  if (op->one_char > 0) sprintf(s, "-%c,", op->one_char); else strcpy(s, "   ");
  printf("  %s --%s%n", s, op->long_name, &n);
  n = 30 - n;
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
  case -1:  help(); exit(0);
  case 'c': count_only = TRUE; break;
  case 'h': filenames = FALSE; break;
  case 'i': options |= PCRE_CASELESS; break;
  case 'l': filenames_only = TRUE; break;
  case 'L': filenames_nomatch_only = TRUE; break;
  case 'M': multiline = TRUE; options |= PCRE_MULTILINE|PCRE_FIRSTLINE; break;
  case 'n': number = TRUE; break;
  case 'q': quiet = TRUE; break;
  case 'r': recurse = TRUE; break;
  case 's': silent = TRUE; break;
  case 'u': options |= PCRE_UTF8; break;
  case 'v': invert = TRUE; break;
  case 'w': word_match = TRUE; break;
  case 'x': whole_lines = TRUE; break;

  case 'V':
  fprintf(stderr, "pcregrep version %s using ", VERSION);
  fprintf(stderr, "PCRE version %s\n", pcre_version());
  exit(0);
  break;

  default:
  fprintf(stderr, "pcregrep: Unknown option -%c\n", letter);
  exit(usage(2));
  }

return options;
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
int options = 0;
int errptr;
const char *error;
BOOL only_one_at_top;

/* Process the options */

for (i = 1; i < argc; i++)
  {
  option_item *op = NULL;
  char *option_data = (char *)"";    /* default to keep compiler happy */
  BOOL longop;
  BOOL longopwasequals = FALSE;

  if (argv[i][0] != '-') break;

  /* If we hit an argument that is just "-", it may be a reference to STDIN,
  but only if we have previously had -f to define the patterns. */

  if (argv[i][1] == 0)
    {
    if (pattern_filename != NULL) break;
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
    with Jeff Friedl in preferring "regex" without the "p". These options are
    entered in the table as "regex(p)". No option is in both these categories,
    fortunately. */

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
          int arglen = (argequals == NULL)? strlen(arg) : argequals - arg;
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
        sprintf(buff2, "%s%.*s", buff1, strlen(op->long_name) - baselen - 2,
          opbra + 1);
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
      options = handle_option(*s++, options);
      }
    }

  /* At this point we should have op pointing to a matched option */

  if (op->type == OP_NODATA)
    options = handle_option(op->one_char, options);
  else
    {
    if (*option_data == 0)
      {
      if (i >= argc - 1 || longopwasequals)
        {
        fprintf(stderr, "pcregrep: Data missing after %s\n", argv[i]);
        exit(usage(2));
        }
      option_data = argv[++i];
      }

    if (op->type == OP_STRING) *((char **)op->dataptr) = option_data; else
      {
      char *endptr;
      int n = strtoul(option_data, &endptr, 10);
      if (*endptr != 0)
        {
        if (longop)
          fprintf(stderr, "pcregrep: Malformed number \"%s\" after --%s\n",
            option_data, op->long_name);
        else
          fprintf(stderr, "pcregrep: Malformed number \"%s\" after -%c\n",
            option_data, op->one_char);
        exit(usage(2));
        }
      *((int *)op->dataptr) = n;
      }
    }
  }

/* Options have been decoded. If -C was used, its value is used as a default
for -A and -B. */

if (both_context > 0)
  {
  if (after_context == 0) after_context = both_context;
  if (before_context == 0) before_context = both_context;
  }

pattern_list = (pcre **)malloc(MAX_PATTERN_COUNT * sizeof(pcre *));
hints_list = (pcre_extra **)malloc(MAX_PATTERN_COUNT * sizeof(pcre_extra *));

if (pattern_list == NULL || hints_list == NULL)
  {
  fprintf(stderr, "pcregrep: malloc failed\n");
  return 2;
  }

/* Compile the regular expression(s). */

if (pattern_filename != NULL)
  {
  FILE *f = fopen(pattern_filename, "r");
  char buffer[MBUFTHIRD + 16];
  char *rdstart;
  int adjust = 0;

  if (f == NULL)
    {
    fprintf(stderr, "pcregrep: Failed to open %s: %s\n", pattern_filename,
      strerror(errno));
    return 2;
    }

  if (whole_lines)
    {
    strcpy(buffer, "^(?:");
    adjust = 4;
    }
  else if (word_match)
    {
    strcpy(buffer, "\\b");
    adjust = 2;
    }

  rdstart = buffer + adjust;
  while (fgets(rdstart, MBUFTHIRD, f) != NULL)
    {
    char *s = rdstart + (int)strlen(rdstart);
    if (pattern_count >= MAX_PATTERN_COUNT)
      {
      fprintf(stderr, "pcregrep: Too many patterns in file (max %d)\n",
        MAX_PATTERN_COUNT);
      return 2;
      }
    while (s > rdstart && isspace((unsigned char)(s[-1]))) s--;
    if (s == rdstart) continue;
    if (whole_lines) strcpy(s, ")$");
      else if (word_match)strcpy(s, "\\b");
        else *s = 0;
    pattern_list[pattern_count] = pcre_compile(buffer, options, &error,
      &errptr, NULL);
    if (pattern_list[pattern_count++] == NULL)
      {
      fprintf(stderr, "pcregrep: Error in regex number %d at offset %d: %s\n",
        pattern_count, errptr - adjust, error);
      return 2;
      }
    }
  fclose(f);
  }

/* If no file name, a single regex must be given inline. */

else
  {
  char buffer[MBUFTHIRD + 16];
  char *pat;
  int adjust = 0;

  if (i >= argc) return usage(2);

  if (whole_lines)
    {
    sprintf(buffer, "^(?:%.*s)$", MBUFTHIRD, argv[i++]);
    pat = buffer;
    adjust = 4;
    }
  else if (word_match)
    {
    sprintf(buffer, "\\b%.*s\\b", MBUFTHIRD, argv[i++]);
    pat = buffer;
    adjust = 2;
    }
  else pat = argv[i++];

  pattern_list[0] = pcre_compile(pat, options, &error, &errptr, NULL);

  if (pattern_list[0] == NULL)
    {
    fprintf(stderr, "pcregrep: Error in regex at offset %d: %s\n",
      errptr - adjust, error);
    return 2;
    }
  pattern_count++;
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
    return 2;
    }
  }

/* If there are include or exclude patterns, compile them. */

if (exclude_pattern != NULL)
  {
  exclude_compiled = pcre_compile(exclude_pattern, 0, &error, &errptr, NULL);
  if (exclude_compiled == NULL)
    {
    fprintf(stderr, "pcregrep: Error in 'exclude' regex at offset %d: %s\n",
      errptr, error);
    return 2;
    }
  }

if (include_pattern != NULL)
  {
  include_compiled = pcre_compile(include_pattern, 0, &error, &errptr, NULL);
  if (include_compiled == NULL)
    {
    fprintf(stderr, "pcregrep: Error in 'include' regex at offset %d: %s\n",
      errptr, error);
    return 2;
    }
  }

/* If there are no further arguments, do the business on stdin and exit */

if (i >= argc) return pcregrep(stdin,
  (filenames_only || filenames_nomatch_only)? stdin_name : NULL);

/* Otherwise, work through the remaining arguments as files or directories.
Pass in the fact that there is only one argument at top level - this suppresses
the file name if the argument is not a directory and filenames_only is not set.
*/

only_one_at_top = (i == argc - 1);

for (; i < argc; i++)
  {
  int frc = grep_or_recurse(argv[i], recurse, filenames, only_one_at_top);
  if (frc > 1) rc = frc;
    else if (frc == 0 && rc == 1) rc = 0;
  }

return rc;
}

/* End of pcregrep */
