/*************************************************
*               PCRE grep program                *
*************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "pcre.h"


#define FALSE 0
#define TRUE 1

typedef int BOOL;



/*************************************************
*               Global variables                 *
*************************************************/

static pcre *pattern;
static pcre_extra *hints;

static BOOL count_only = FALSE;
static BOOL filenames_only = FALSE;
static BOOL invert = FALSE;
static BOOL number = FALSE;
static BOOL silent = FALSE;
static BOOL whole_lines = FALSE;



#ifdef STRERROR_FROM_ERRLIST
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
#endif /* STRERROR_FROM_ERRLIST */



/*************************************************
*              Grep an individual file           *
*************************************************/

static int
pgrep(FILE *in, char *name)
{
int rc = 1;
int linenumber = 0;
int count = 0;
int offsets[99];
char buffer[BUFSIZ];

while (fgets(buffer, sizeof(buffer), in) != NULL)
  {
  BOOL match;
  int length = (int)strlen(buffer);
  if (length > 0 && buffer[length-1] == '\n') buffer[--length] = 0;
  linenumber++;

  match = pcre_exec(pattern, hints, buffer, length, 0, 0, offsets, 99) >= 0;
  if (match && whole_lines && offsets[1] != length) match = FALSE;

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
*                Usage function                  *
*************************************************/

static int
usage(int rc)
{
fprintf(stderr, "Usage: pgrep [-Vchilnsvx] pattern [file] ...\n");
return rc;
}




/*************************************************
*                Main program                    *
*************************************************/

int
main(int argc, char **argv)
{
int i;
int rc = 1;
int options = 0;
int errptr;
const char *error;
BOOL filenames = TRUE;

/* Process the options */

for (i = 1; i < argc; i++)
  {
  char *s;
  if (argv[i][0] != '-') break;
  s = argv[i] + 1;
  while (*s != 0)
    {
    switch (*s++)
      {
      case 'c': count_only = TRUE; break;
      case 'h': filenames = FALSE; break;
      case 'i': options |= PCRE_CASELESS; break;
      case 'l': filenames_only = TRUE;
      case 'n': number = TRUE; break;
      case 's': silent = TRUE; break;
      case 'v': invert = TRUE; break;
      case 'x': whole_lines = TRUE; options |= PCRE_ANCHORED; break;

      case 'V':
      fprintf(stderr, "PCRE version %s\n", pcre_version());
      break;

      default:
      fprintf(stderr, "pgrep: unknown option %c\n", s[-1]);
      return usage(2);
      }
    }
  }

/* There must be at least a regexp argument */

if (i >= argc) return usage(0);

/* Compile the regular expression. */

pattern = pcre_compile(argv[i++], options, &error, &errptr, NULL);
if (pattern == NULL)
  {
  fprintf(stderr, "pgrep: error in regex at offset %d: %s\n", errptr, error);
  return 2;
  }

/* Study the regular expression, as we will be running it may times */

hints = pcre_study(pattern, 0, &error);
if (error != NULL)
  {
  fprintf(stderr, "pgrep: error while studing regex: %s\n", error);
  return 2;
  }

/* If there are no further arguments, do the business on stdin and exit */

if (i >= argc) return pgrep(stdin, NULL);

/* Otherwise, work through the remaining arguments as files. If there is only
one, don't give its name on the output. */

if (i == argc - 1) filenames = FALSE;
if (filenames_only) filenames = TRUE;

for (; i < argc; i++)
  {
  FILE *in = fopen(argv[i], "r");
  if (in == NULL)
    {
    fprintf(stderr, "%s: failed to open: %s\n", argv[i], strerror(errno));
    rc = 2;
    }
  else
    {
    int frc = pgrep(in, filenames? argv[i] : NULL);
    if (frc == 0 && rc == 1) rc = 0;
    fclose(in);
    }
  }

return rc;
}

/* End */
