/*************************************************
*               pcregrep program                 *
*************************************************/

/* This is a grep program that uses the PCRE regular expression library to do
its pattern matching. On a Unix or Win32 system it can recurse into
directories. */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "config.h"
#include "pcre.h"

#define FALSE 0
#define TRUE 1

typedef int BOOL;

#define VERSION "2.2 10-Sep-2002"
#define MAX_PATTERN_COUNT 100


/*************************************************
*               Global variables                 *
*************************************************/

static char *pattern_filename = NULL;
static int  pattern_count = 0;
static pcre **pattern_list;
static pcre_extra **hints_list;

static BOOL count_only = FALSE;
static BOOL filenames = TRUE;
static BOOL filenames_only = FALSE;
static BOOL invert = FALSE;
static BOOL number = FALSE;
static BOOL recurse = FALSE;
static BOOL silent = FALSE;
static BOOL whole_lines = FALSE;

/* Structure for options and list of them */

typedef struct option_item {
  int one_char;
  char *long_name;
  char *help_text;
} option_item;

static option_item optionlist[] = {
  { -1,  "help",         "display this help and exit" },
  { 'c', "count",        "print only a count of matching lines per FILE" },
  { 'h', "no-filename",  "suppress the prefixing filename on output" },
  { 'i', "ignore-case",  "ignore case distinctions" },
  { 'l', "files-with-matches", "print only FILE names containing matches" },
  { 'n', "line-number",  "print line number with output lines" },
  { 'r', "recursive",    "recursively scan sub-directories" },
  { 's', "no-messages",  "suppress error messages" },
  { 'V', "version",      "print version information and exit" },
  { 'v', "invert-match", "select non-matching lines" },
  { 'x', "line-regex",   "force PATTERN to match only whole lines" },
  { 'x', "line-regexp",  "force PATTERN to match only whole lines" },
  { 0,    NULL,           NULL }
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

int
isdirectory(char *filename)
{
struct stat statbuf;
if (stat(filename, &statbuf) < 0)
  return 0;        /* In the expectation that opening as a file will fail */
return ((statbuf.st_mode & S_IFMT) == S_IFDIR)? '/' : 0;
}

directory_type *
opendirectory(char *filename)
{
return opendir(filename);
}

char *
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

void
closedirectory(directory_type *dir)
{
closedir(dir);
}


/************* Directory scanning in Win32 ***********/

/* I (Philip Hazel) have no means of testing this code. It was contributed by
Lionel Fourquaux. */


#elif HAVE_WIN32API

#ifndef STRICT
# define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
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
*              Grep an individual file           *
*************************************************/

static int
pcregrep(FILE *in, char *name)
{
int rc = 1;
int linenumber = 0;
int count = 0;
int offsets[99];
char buffer[BUFSIZ];

while (fgets(buffer, sizeof(buffer), in) != NULL)
  {
  BOOL match = FALSE;
  int i;
  int length = (int)strlen(buffer);
  if (length > 0 && buffer[length-1] == '\n') buffer[--length] = 0;
  linenumber++;

  for (i = 0; !match && i < pattern_count; i++)
    {
    match = pcre_exec(pattern_list[i], hints_list[i], buffer, length, 0, 0,
      offsets, 99) >= 0;
    if (match && whole_lines && offsets[1] != length) match = FALSE;
    }

  if (match != invert)
    {
    if (count_only) count++;

    else if (filenames_only)
      {
      fprintf(stdout, "%s\n", (name == NULL)? "<stdin>" : name);
      return 0;
      }

    else if (silent) return 0;

    else
      {
      if (name != NULL) fprintf(stdout, "%s:", name);
      if (number) fprintf(stdout, "%d:", linenumber);
      fprintf(stdout, "%s\n", buffer);
      }

    rc = 0;
    }
  }

if (count_only)
  {
  if (name != NULL) fprintf(stdout, "%s:", name);
  fprintf(stdout, "%d\n", count);
  }

return rc;
}




/*************************************************
*     Grep a file or recurse into a directory    *
*************************************************/

static int
grep_or_recurse(char *filename, BOOL recurse, BOOL show_filenames,
  BOOL only_one_at_top)
{
int rc = 1;
int sep;
FILE *in;

/* If the file is a directory and we are recursing, scan each file within it.
The scanning code is localized so it can be made system-specific. */

if ((sep = isdirectory(filename)) != 0 && recurse)
  {
  char buffer[1024];
  char *nextfile;
  directory_type *dir = opendirectory(filename);

  if (dir == NULL)
    {
    fprintf(stderr, "pcregrep: Failed to open directory %s: %s\n", filename,
      strerror(errno));
    return 2;
    }

  while ((nextfile = readdirectory(dir)) != NULL)
    {
    int frc;
    sprintf(buffer, "%.512s%c%.128s", filename, sep, nextfile);
    frc = grep_or_recurse(buffer, recurse, TRUE, FALSE);
    if (frc == 0 && rc == 1) rc = 0;
    }

  closedirectory(dir);
  return rc;
  }

/* If the file is not a directory, or we are not recursing, scan it. If this is
the first and only argument at top level, we don't show the file name (unless
we are only showing the file name). Otherwise, control is via the
show_filenames variable. */

in = fopen(filename, "r");
if (in == NULL)
  {
  fprintf(stderr, "pcregrep: Failed to open %s: %s\n", filename, strerror(errno));
  return 2;
  }

rc = pcregrep(in, (filenames_only || (show_filenames && !only_one_at_top))?
  filename : NULL);
fclose(in);
return rc;
}




/*************************************************
*                Usage function                  *
*************************************************/

static int
usage(int rc)
{
fprintf(stderr, "Usage: pcregrep [-Vcfhilnrsvx] [long-options] [pattern] [file1 file2 ...]\n");
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

printf("\n  -f<filename>  or  --file=<filename>\n");
printf("    Read patterns from <filename> instead of using a command line option.\n");
printf("    Trailing white space is removed; blanks lines are ignored.\n");
printf("    There is a maximum of %d patterns.\n", MAX_PATTERN_COUNT);

printf("\nWith no FILE, read standard input. If fewer than two FILEs given, assume -h.\n");
printf("Exit status is 0 if any matches, 1 if no matches, and 2 if trouble.\n");
}




/*************************************************
*                Handle an option                *
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
  case 'l': filenames_only = TRUE;
  case 'n': number = TRUE; break;
  case 'r': recurse = TRUE; break;
  case 's': silent = TRUE; break;
  case 'v': invert = TRUE; break;
  case 'x': whole_lines = TRUE; options |= PCRE_ANCHORED; break;

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
  if (argv[i][0] != '-') break;

  /* Missing options */

  if (argv[i][1] == 0) exit(usage(2));

  /* Long name options */

  if (argv[i][1] == '-')
    {
    option_item *op;

    if (strncmp(argv[i]+2, "file=", 5) == 0)
      {
      pattern_filename = argv[i] + 7;
      continue;
      }

    for (op = optionlist; op->one_char != 0; op++)
      {
      if (strcmp(argv[i]+2, op->long_name) == 0)
        {
        options = handle_option(op->one_char, options);
        break;
        }
      }
    if (op->one_char == 0)
      {
      fprintf(stderr, "pcregrep: Unknown option %s\n", argv[i]);
      exit(usage(2));
      }
    }

  /* One-char options */

  else
    {
    char *s = argv[i] + 1;
    while (*s != 0)
      {
      if (*s == 'f')
        {
        pattern_filename = s + 1;
        if (pattern_filename[0] == 0)
          {
          if (i >= argc - 1)
            {
            fprintf(stderr, "pcregrep: File name missing after -f\n");
            exit(usage(2));
            }
          pattern_filename = argv[++i];
          }
        break;
        }
      else options = handle_option(*s++, options);
      }
    }
  }

pattern_list = malloc(MAX_PATTERN_COUNT * sizeof(pcre *));
hints_list = malloc(MAX_PATTERN_COUNT * sizeof(pcre_extra *));

if (pattern_list == NULL || hints_list == NULL)
  {
  fprintf(stderr, "pcregrep: malloc failed\n");
  return 2;
  }

/* Compile the regular expression(s). */

if (pattern_filename != NULL)
  {
  FILE *f = fopen(pattern_filename, "r");
  char buffer[BUFSIZ];
  if (f == NULL)
    {
    fprintf(stderr, "pcregrep: Failed to open %s: %s\n", pattern_filename,
      strerror(errno));
    return 2;
    }
  while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
    char *s = buffer + (int)strlen(buffer);
    if (pattern_count >= MAX_PATTERN_COUNT)
      {
      fprintf(stderr, "pcregrep: Too many patterns in file (max %d)\n",
        MAX_PATTERN_COUNT);
      return 2;
      }
    while (s > buffer && isspace((unsigned char)(s[-1]))) s--;
    if (s == buffer) continue;
    *s = 0;
    pattern_list[pattern_count] = pcre_compile(buffer, options, &error,
      &errptr, NULL);
    if (pattern_list[pattern_count++] == NULL)
      {
      fprintf(stderr, "pcregrep: Error in regex number %d at offset %d: %s\n",
        pattern_count, errptr, error);
      return 2;
      }
    }
  fclose(f);
  }

/* If no file name, a single regex must be given inline */

else
  {
  if (i >= argc) return usage(2);
  pattern_list[0] = pcre_compile(argv[i++], options, &error, &errptr, NULL);
  if (pattern_list[0] == NULL)
    {
    fprintf(stderr, "pcregrep: Error in regex at offset %d: %s\n", errptr,
      error);
    return 2;
    }
  pattern_count++;
  }

/* Study the regular expressions, as we will be running them may times */

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

/* If there are no further arguments, do the business on stdin and exit */

if (i >= argc) return pcregrep(stdin, NULL);

/* Otherwise, work through the remaining arguments as files or directories.
Pass in the fact that there is only one argument at top level - this suppresses
the file name if the argument is not a directory. */

only_one_at_top = (i == argc - 1);
if (filenames_only) filenames = TRUE;

for (; i < argc; i++)
  {
  int frc = grep_or_recurse(argv[i], recurse, filenames, only_one_at_top);
  if (frc == 0 && rc == 1) rc = 0;
  }

return rc;
}

/* End */
