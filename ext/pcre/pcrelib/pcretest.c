/*************************************************
*             PCRE testing program               *
*************************************************/

/* This program was hacked up as a tester for PCRE. I really should have
written it more tidily in the first place. Will I ever learn? It has grown and
been extended and consequently is now rather untidy in places.

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
#include <time.h>
#include <locale.h>
#include <errno.h>

/* We need the internal info for displaying the results of pcre_study(). Also
for getting the opcodes for showing compiled code. */

#define PCRE_SPY        /* For Win32 build, import data, not export */
#include "internal.h"

/* It is possible to compile this test program without including support for
testing the POSIX interface, though this is not available via the standard
Makefile. */

#if !defined NOPOSIX
#include "pcreposix.h"
#endif

#ifndef CLOCKS_PER_SEC
#ifdef CLK_TCK
#define CLOCKS_PER_SEC CLK_TCK
#else
#define CLOCKS_PER_SEC 100
#endif
#endif

#define LOOPREPEAT 500000

#define BUFFER_SIZE 30000
#define PBUFFER_SIZE BUFFER_SIZE
#define DBUFFER_SIZE BUFFER_SIZE


static FILE *outfile;
static int log_store = 0;
static int callout_count;
static int callout_extra;
static int callout_fail_count;
static int callout_fail_id;
static int first_callout;
static int show_malloc;
static int use_utf8;
static size_t gotten_store;

static uschar *pbuffer = NULL;


static const int utf8_table1[] = {
  0x0000007f, 0x000007ff, 0x0000ffff, 0x001fffff, 0x03ffffff, 0x7fffffff};

static const int utf8_table2[] = {
  0, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc};

static const int utf8_table3[] = {
  0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};



/*************************************************
*         Print compiled regex                   *
*************************************************/

/* The code for doing this is held in a separate file that is also included in
pcre.c when it is compiled with the debug switch. It defines a function called
print_internals(), which uses a table of opcode lengths defined by the macro
OP_LENGTHS, whose name must be OP_lengths. It also uses a table that translates
Unicode property names to numbers; this is kept in a separate file. */

static uschar OP_lengths[] = { OP_LENGTHS };

#include "ucp.h"
#include "ucptypetable.c"
#include "printint.c"



/*************************************************
*          Read number from string               *
*************************************************/

/* We don't use strtoul() because SunOS4 doesn't have it. Rather than mess
around with conditional compilation, just do the job by hand. It is only used
for unpicking the -o argument, so just keep it simple.

Arguments:
  str           string to be converted
  endptr        where to put the end pointer

Returns:        the unsigned long
*/

static int
get_value(unsigned char *str, unsigned char **endptr)
{
int result = 0;
while(*str != 0 && isspace(*str)) str++;
while (isdigit(*str)) result = result * 10 + (int)(*str++ - '0');
*endptr = str;
return(result);
}



/*************************************************
*       Convert character value to UTF-8         *
*************************************************/

/* This function takes an integer value in the range 0 - 0x7fffffff
and encodes it as a UTF-8 character in 0 to 6 bytes.

Arguments:
  cvalue     the character value
  buffer     pointer to buffer for result - at least 6 bytes long

Returns:     number of characters placed in the buffer
             -1 if input character is negative
             0 if input character is positive but too big (only when
             int is longer than 32 bits)
*/

static int
ord2utf8(int cvalue, unsigned char *buffer)
{
register int i, j;
for (i = 0; i < sizeof(utf8_table1)/sizeof(int); i++)
  if (cvalue <= utf8_table1[i]) break;
if (i >= sizeof(utf8_table1)/sizeof(int)) return 0;
if (cvalue < 0) return -1;

buffer += i;
for (j = i; j > 0; j--)
 {
 *buffer-- = 0x80 | (cvalue & 0x3f);
 cvalue >>= 6;
 }
*buffer = utf8_table2[i] | cvalue;
return i + 1;
}


/*************************************************
*            Convert UTF-8 string to value       *
*************************************************/

/* This function takes one or more bytes that represents a UTF-8 character,
and returns the value of the character.

Argument:
  buffer   a pointer to the byte vector
  vptr     a pointer to an int to receive the value

Returns:   >  0 => the number of bytes consumed
           -6 to 0 => malformed UTF-8 character at offset = (-return)
*/

static int
utf82ord(unsigned char *buffer, int *vptr)
{
int c = *buffer++;
int d = c;
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
  c = *buffer++;
  if ((c & 0xc0) != 0x80) return -(j+1);
  s -= 6;
  d |= (c & 0x3f) << s;
  }

/* Check that encoding was the correct unique one */

for (j = 0; j < sizeof(utf8_table1)/sizeof(int); j++)
  if (d <= utf8_table1[j]) break;
if (j != i) return -(i+1);

/* Valid value */

*vptr = d;
return i+1;
}



/*************************************************
*             Print character string             *
*************************************************/

/* Character string printing function. Must handle UTF-8 strings in utf8
mode. Yields number of characters printed. If handed a NULL file, just counts
chars without printing. */

static int pchars(unsigned char *p, int length, FILE *f)
{
int c;
int yield = 0;

while (length-- > 0)
  {
  if (use_utf8)
    {
    int rc = utf82ord(p, &c);

    if (rc > 0 && rc <= length + 1)   /* Mustn't run over the end */
      {
      length -= rc - 1;
      p += rc;
      if (c < 256 && isprint(c))
        {
        if (f != NULL) fprintf(f, "%c", c);
        yield++;
        }
      else
        {
        int n;
        if (f != NULL) fprintf(f, "\\x{%02x}%n", c, &n);
        yield += n;
        }
      continue;
      }
    }

   /* Not UTF-8, or malformed UTF-8  */

  if (isprint(c = *(p++)))
    {
    if (f != NULL) fprintf(f, "%c", c);
    yield++;
    }
  else
    {
    if (f != NULL) fprintf(f, "\\x%02x", c);
    yield += 4;
    }
  }

return yield;
}



/*************************************************
*              Callout function                  *
*************************************************/

/* Called from PCRE as a result of the (?C) item. We print out where we are in
the match. Yield zero unless more callouts than the fail count, or the callout
data is not zero. */

static int callout(pcre_callout_block *cb)
{
FILE *f = (first_callout | callout_extra)? outfile : NULL;
int i, pre_start, post_start, subject_length;

if (callout_extra)
  {
  fprintf(f, "Callout %d: last capture = %d\n",
    cb->callout_number, cb->capture_last);

  for (i = 0; i < cb->capture_top * 2; i += 2)
    {
    if (cb->offset_vector[i] < 0)
      fprintf(f, "%2d: <unset>\n", i/2);
    else
      {
      fprintf(f, "%2d: ", i/2);
      (void)pchars((unsigned char *)cb->subject + cb->offset_vector[i],
        cb->offset_vector[i+1] - cb->offset_vector[i], f);
      fprintf(f, "\n");
      }
    }
  }

/* Re-print the subject in canonical form, the first time or if giving full
datails. On subsequent calls in the same match, we use pchars just to find the
printed lengths of the substrings. */

if (f != NULL) fprintf(f, "--->");

pre_start = pchars((unsigned char *)cb->subject, cb->start_match, f);
post_start = pchars((unsigned char *)(cb->subject + cb->start_match),
  cb->current_position - cb->start_match, f);

subject_length = pchars((unsigned char *)cb->subject, cb->subject_length, NULL);

(void)pchars((unsigned char *)(cb->subject + cb->current_position),
  cb->subject_length - cb->current_position, f);

if (f != NULL) fprintf(f, "\n");

/* Always print appropriate indicators, with callout number if not already
shown. For automatic callouts, show the pattern offset. */

if (cb->callout_number == 255)
  {
  fprintf(outfile, "%+3d ", cb->pattern_position);
  if (cb->pattern_position > 99) fprintf(outfile, "\n    ");
  }
else
  {
  if (callout_extra) fprintf(outfile, "    ");
    else fprintf(outfile, "%3d ", cb->callout_number);
  }

for (i = 0; i < pre_start; i++) fprintf(outfile, " ");
fprintf(outfile, "^");

if (post_start > 0)
  {
  for (i = 0; i < post_start - 1; i++) fprintf(outfile, " ");
  fprintf(outfile, "^");
  }

for (i = 0; i < subject_length - pre_start - post_start + 4; i++)
  fprintf(outfile, " ");

fprintf(outfile, "%.*s", (cb->next_item_length == 0)? 1 : cb->next_item_length,
  pbuffer + cb->pattern_position);

fprintf(outfile, "\n");
first_callout = 0;

if (cb->callout_data != NULL)
  {
  int callout_data = *((int *)(cb->callout_data));
  if (callout_data != 0)
    {
    fprintf(outfile, "Callout data = %d\n", callout_data);
    return callout_data;
    }
  }

return (cb->callout_number != callout_fail_id)? 0 :
       (++callout_count >= callout_fail_count)? 1 : 0;
}


/*************************************************
*            Local malloc functions              *
*************************************************/

/* Alternative malloc function, to test functionality and show the size of the
compiled re. */

static void *new_malloc(size_t size)
{
void *block = malloc(size);
gotten_store = size;
if (show_malloc)
  fprintf(outfile, "malloc       %3d %p\n", size, block);
return block;
}

static void new_free(void *block)
{
if (show_malloc)
  fprintf(outfile, "free             %p\n", block);
free(block);
}


/* For recursion malloc/free, to test stacking calls */

static void *stack_malloc(size_t size)
{
void *block = malloc(size);
if (show_malloc)
  fprintf(outfile, "stack_malloc %3d %p\n", size, block);
return block;
}

static void stack_free(void *block)
{
if (show_malloc)
  fprintf(outfile, "stack_free       %p\n", block);
free(block);
}


/*************************************************
*          Call pcre_fullinfo()                  *
*************************************************/

/* Get one piece of information from the pcre_fullinfo() function */

static void new_info(pcre *re, pcre_extra *study, int option, void *ptr)
{
int rc;
if ((rc = pcre_fullinfo(re, study, option, ptr)) < 0)
  fprintf(outfile, "Error %d from pcre_fullinfo(%d)\n", rc, option);
}



/*************************************************
*         Byte flipping function                 *
*************************************************/

static long int
byteflip(long int value, int n)
{
if (n == 2) return ((value & 0x00ff) << 8) | ((value & 0xff00) >> 8);
return ((value & 0x000000ff) << 24) |
       ((value & 0x0000ff00) <<  8) |
       ((value & 0x00ff0000) >>  8) |
       ((value & 0xff000000) >> 24);
}




/*************************************************
*                Main Program                    *
*************************************************/

/* Read lines from named file or stdin and write to named file or stdout; lines
consist of a regular expression, in delimiters and optionally followed by
options, followed by a set of test data, terminated by an empty line. */

int main(int argc, char **argv)
{
FILE *infile = stdin;
int options = 0;
int study_options = 0;
int op = 1;
int timeit = 0;
int showinfo = 0;
int showstore = 0;
int size_offsets = 45;
int size_offsets_max;
int *offsets;
#if !defined NOPOSIX
int posix = 0;
#endif
int debug = 0;
int done = 0;

unsigned char *buffer;
unsigned char *dbuffer;

/* Get buffers from malloc() so that Electric Fence will check their misuse
when I am debugging. */

buffer = (unsigned char *)malloc(BUFFER_SIZE);
dbuffer = (unsigned char *)malloc(DBUFFER_SIZE);
pbuffer = (unsigned char *)malloc(PBUFFER_SIZE);

/* The outfile variable is static so that new_malloc can use it. The _setmode()
stuff is some magic that I don't understand, but which apparently does good
things in Windows. It's related to line terminations.  */

#if defined(_WIN32) || defined(WIN32)
_setmode( _fileno( stdout ), 0x8000 );
#endif  /* defined(_WIN32) || defined(WIN32) */

outfile = stdout;

/* Scan options */

while (argc > 1 && argv[op][0] == '-')
  {
  unsigned char *endptr;

  if (strcmp(argv[op], "-s") == 0 || strcmp(argv[op], "-m") == 0)
    showstore = 1;
  else if (strcmp(argv[op], "-t") == 0) timeit = 1;
  else if (strcmp(argv[op], "-i") == 0) showinfo = 1;
  else if (strcmp(argv[op], "-d") == 0) showinfo = debug = 1;
  else if (strcmp(argv[op], "-o") == 0 && argc > 2 &&
      ((size_offsets = get_value((unsigned char *)argv[op+1], &endptr)),
        *endptr == 0))
    {
    op++;
    argc--;
    }
#if !defined NOPOSIX
  else if (strcmp(argv[op], "-p") == 0) posix = 1;
#endif
  else if (strcmp(argv[op], "-C") == 0)
    {
    int rc;
    printf("PCRE version %s\n", pcre_version());
    printf("Compiled with\n");
    (void)pcre_config(PCRE_CONFIG_UTF8, &rc);
    printf("  %sUTF-8 support\n", rc? "" : "No ");
    (void)pcre_config(PCRE_CONFIG_UNICODE_PROPERTIES, &rc);
    printf("  %sUnicode properties support\n", rc? "" : "No ");
    (void)pcre_config(PCRE_CONFIG_NEWLINE, &rc);
    printf("  Newline character is %s\n", (rc == '\r')? "CR" : "LF");
    (void)pcre_config(PCRE_CONFIG_LINK_SIZE, &rc);
    printf("  Internal link size = %d\n", rc);
    (void)pcre_config(PCRE_CONFIG_POSIX_MALLOC_THRESHOLD, &rc);
    printf("  POSIX malloc threshold = %d\n", rc);
    (void)pcre_config(PCRE_CONFIG_MATCH_LIMIT, &rc);
    printf("  Default match limit = %d\n", rc);
    (void)pcre_config(PCRE_CONFIG_STACKRECURSE, &rc);
    printf("  Match recursion uses %s\n", rc? "stack" : "heap");
    exit(0);
    }
  else
    {
    printf("** Unknown or malformed option %s\n", argv[op]);
    printf("Usage:   pcretest [-d] [-i] [-o <n>] [-p] [-s] [-t] [<input> [<output>]]\n");
    printf("  -C     show PCRE compile-time options and exit\n");
    printf("  -d     debug: show compiled code; implies -i\n"
           "  -i     show information about compiled pattern\n"
           "  -m     output memory used information\n"
           "  -o <n> set size of offsets vector to <n>\n");
#if !defined NOPOSIX
    printf("  -p     use POSIX interface\n");
#endif
    printf("  -s     output store (memory) used information\n"
           "  -t     time compilation and execution\n");
    return 1;
    }
  op++;
  argc--;
  }

/* Get the store for the offsets vector, and remember what it was */

size_offsets_max = size_offsets;
offsets = (int *)malloc(size_offsets_max * sizeof(int));
if (offsets == NULL)
  {
  printf("** Failed to get %d bytes of memory for offsets vector\n",
    size_offsets_max * sizeof(int));
  return 1;
  }

/* Sort out the input and output files */

if (argc > 1)
  {
  infile = fopen(argv[op], "rb");
  if (infile == NULL)
    {
    printf("** Failed to open %s\n", argv[op]);
    return 1;
    }
  }

if (argc > 2)
  {
  outfile = fopen(argv[op+1], "wb");
  if (outfile == NULL)
    {
    printf("** Failed to open %s\n", argv[op+1]);
    return 1;
    }
  }

/* Set alternative malloc function */

pcre_malloc = new_malloc;
pcre_free = new_free;
pcre_stack_malloc = stack_malloc;
pcre_stack_free = stack_free;

/* Heading line, then prompt for first regex if stdin */

fprintf(outfile, "PCRE version %s\n\n", pcre_version());

/* Main loop */

while (!done)
  {
  pcre *re = NULL;
  pcre_extra *extra = NULL;

#if !defined NOPOSIX  /* There are still compilers that require no indent */
  regex_t preg;
  int do_posix = 0;
#endif

  const char *error;
  unsigned char *p, *pp, *ppp;
  unsigned char *to_file = NULL;
  const unsigned char *tables = NULL;
  unsigned long int true_size, true_study_size = 0;
  size_t size, regex_gotten_store;
  int do_study = 0;
  int do_debug = debug;
  int do_G = 0;
  int do_g = 0;
  int do_showinfo = showinfo;
  int do_showrest = 0;
  int do_flip = 0;
  int erroroffset, len, delimiter;

  use_utf8 = 0;

  if (infile == stdin) printf("  re> ");
  if (fgets((char *)buffer, BUFFER_SIZE, infile) == NULL) break;
  if (infile != stdin) fprintf(outfile, "%s", (char *)buffer);
  fflush(outfile);

  p = buffer;
  while (isspace(*p)) p++;
  if (*p == 0) continue;

  /* See if the pattern is to be loaded pre-compiled from a file. */

  if (*p == '<' && strchr((char *)(p+1), '<') == NULL)
    {
    unsigned long int magic;
    uschar sbuf[8];
    FILE *f;

    p++;
    pp = p + (int)strlen((char *)p);
    while (isspace(pp[-1])) pp--;
    *pp = 0;

    f = fopen((char *)p, "rb");
    if (f == NULL)
      {
      fprintf(outfile, "Failed to open %s: %s\n", p, strerror(errno));
      continue;
      }

    if (fread(sbuf, 1, 8, f) != 8) goto FAIL_READ;

    true_size =
      (sbuf[0] << 24) | (sbuf[1] << 16) | (sbuf[2] << 8) | sbuf[3];
    true_study_size =
      (sbuf[4] << 24) | (sbuf[5] << 16) | (sbuf[6] << 8) | sbuf[7];

    re = (real_pcre *)new_malloc(true_size);
    regex_gotten_store = gotten_store;

    if (fread(re, 1, true_size, f) != true_size) goto FAIL_READ;

    magic = ((real_pcre *)re)->magic_number;
    if (magic != MAGIC_NUMBER)
      {
      if (byteflip(magic, sizeof(magic)) == MAGIC_NUMBER)
        {
        do_flip = 1;
        }
      else
        {
        fprintf(outfile, "Data in %s is not a compiled PCRE regex\n", p);
        fclose(f);
        continue;
        }
      }

    fprintf(outfile, "Compiled regex%s loaded from %s\n",
      do_flip? " (byte-inverted)" : "", p);

    /* Need to know if UTF-8 for printing data strings */

    new_info(re, NULL, PCRE_INFO_OPTIONS, &options);
    use_utf8 = (options & PCRE_UTF8) != 0;

    /* Now see if there is any following study data */

    if (true_study_size != 0)
      {
      pcre_study_data *psd;

      extra = (pcre_extra *)new_malloc(sizeof(pcre_extra) + true_study_size);
      extra->flags = PCRE_EXTRA_STUDY_DATA;

      psd = (pcre_study_data *)(((char *)extra) + sizeof(pcre_extra));
      extra->study_data = psd;

      if (fread(psd, 1, true_study_size, f) != true_study_size)
        {
        FAIL_READ:
        fprintf(outfile, "Failed to read data from %s\n", p);
        if (extra != NULL) new_free(extra);
        if (re != NULL) new_free(re);
        fclose(f);
        continue;
        }
      fprintf(outfile, "Study data loaded from %s\n", p);
      do_study = 1;     /* To get the data output if requested */
      }
    else fprintf(outfile, "No study data\n");

    fclose(f);
    goto SHOW_INFO;
    }

  /* In-line pattern (the usual case). Get the delimiter and seek the end of
  the pattern; if is isn't complete, read more. */

  delimiter = *p++;

  if (isalnum(delimiter) || delimiter == '\\')
    {
    fprintf(outfile, "** Delimiter must not be alphameric or \\\n");
    goto SKIP_DATA;
    }

  pp = p;

  for(;;)
    {
    while (*pp != 0)
      {
      if (*pp == '\\' && pp[1] != 0) pp++;
        else if (*pp == delimiter) break;
      pp++;
      }
    if (*pp != 0) break;

    len = BUFFER_SIZE - (pp - buffer);
    if (len < 256)
      {
      fprintf(outfile, "** Expression too long - missing delimiter?\n");
      goto SKIP_DATA;
      }

    if (infile == stdin) printf("    > ");
    if (fgets((char *)pp, len, infile) == NULL)
      {
      fprintf(outfile, "** Unexpected EOF\n");
      done = 1;
      goto CONTINUE;
      }
    if (infile != stdin) fprintf(outfile, "%s", (char *)pp);
    }

  /* If the first character after the delimiter is backslash, make
  the pattern end with backslash. This is purely to provide a way
  of testing for the error message when a pattern ends with backslash. */

  if (pp[1] == '\\') *pp++ = '\\';

  /* Terminate the pattern at the delimiter, and save a copy of the pattern
  for callouts. */

  *pp++ = 0;
  strcpy((char *)pbuffer, (char *)p);

  /* Look for options after final delimiter */

  options = 0;
  study_options = 0;
  log_store = showstore;  /* default from command line */

  while (*pp != 0)
    {
    switch (*pp++)
      {
      case 'g': do_g = 1; break;
      case 'i': options |= PCRE_CASELESS; break;
      case 'm': options |= PCRE_MULTILINE; break;
      case 's': options |= PCRE_DOTALL; break;
      case 'x': options |= PCRE_EXTENDED; break;

      case '+': do_showrest = 1; break;
      case 'A': options |= PCRE_ANCHORED; break;
      case 'C': options |= PCRE_AUTO_CALLOUT; break;
      case 'D': do_debug = do_showinfo = 1; break;
      case 'E': options |= PCRE_DOLLAR_ENDONLY; break;
      case 'F': do_flip = 1; break;
      case 'G': do_G = 1; break;
      case 'I': do_showinfo = 1; break;
      case 'M': log_store = 1; break;
      case 'N': options |= PCRE_NO_AUTO_CAPTURE; break;

#if !defined NOPOSIX
      case 'P': do_posix = 1; break;
#endif

      case 'S': do_study = 1; break;
      case 'U': options |= PCRE_UNGREEDY; break;
      case 'X': options |= PCRE_EXTRA; break;
      case '8': options |= PCRE_UTF8; use_utf8 = 1; break;
      case '?': options |= PCRE_NO_UTF8_CHECK; break;

      case 'L':
      ppp = pp;
      while (*ppp != '\n' && *ppp != ' ') ppp++;
      *ppp = 0;
      if (setlocale(LC_CTYPE, (const char *)pp) == NULL)
        {
        fprintf(outfile, "** Failed to set locale \"%s\"\n", pp);
        goto SKIP_DATA;
        }
      tables = pcre_maketables();
      pp = ppp;
      break;

      case '>':
      to_file = pp;
      while (*pp != 0) pp++;
      while (isspace(pp[-1])) pp--;
      *pp = 0;
      break;

      case '\n': case ' ': break;

      default:
      fprintf(outfile, "** Unknown option '%c'\n", pp[-1]);
      goto SKIP_DATA;
      }
    }

  /* Handle compiling via the POSIX interface, which doesn't support the
  timing, showing, or debugging options, nor the ability to pass over
  local character tables. */

#if !defined NOPOSIX
  if (posix || do_posix)
    {
    int rc;
    int cflags = 0;

    if ((options & PCRE_CASELESS) != 0) cflags |= REG_ICASE;
    if ((options & PCRE_MULTILINE) != 0) cflags |= REG_NEWLINE;
    rc = regcomp(&preg, (char *)p, cflags);

    /* Compilation failed; go back for another re, skipping to blank line
    if non-interactive. */

    if (rc != 0)
      {
      (void)regerror(rc, &preg, (char *)buffer, BUFFER_SIZE);
      fprintf(outfile, "Failed: POSIX code %d: %s\n", rc, buffer);
      goto SKIP_DATA;
      }
    }

  /* Handle compiling via the native interface */

  else
#endif  /* !defined NOPOSIX */

    {
    if (timeit)
      {
      register int i;
      clock_t time_taken;
      clock_t start_time = clock();
      for (i = 0; i < LOOPREPEAT; i++)
        {
        re = pcre_compile((char *)p, options, &error, &erroroffset, tables);
        if (re != NULL) free(re);
        }
      time_taken = clock() - start_time;
      fprintf(outfile, "Compile time %.3f milliseconds\n",
        (((double)time_taken * 1000.0) / (double)LOOPREPEAT) /
          (double)CLOCKS_PER_SEC);
      }

    re = pcre_compile((char *)p, options, &error, &erroroffset, tables);

    /* Compilation failed; go back for another re, skipping to blank line
    if non-interactive. */

    if (re == NULL)
      {
      fprintf(outfile, "Failed: %s at offset %d\n", error, erroroffset);
      SKIP_DATA:
      if (infile != stdin)
        {
        for (;;)
          {
          if (fgets((char *)buffer, BUFFER_SIZE, infile) == NULL)
            {
            done = 1;
            goto CONTINUE;
            }
          len = (int)strlen((char *)buffer);
          while (len > 0 && isspace(buffer[len-1])) len--;
          if (len == 0) break;
          }
        fprintf(outfile, "\n");
        }
      goto CONTINUE;
      }

    /* Compilation succeeded; print data if required. There are now two
    info-returning functions. The old one has a limited interface and
    returns only limited data. Check that it agrees with the newer one. */

    if (log_store)
      fprintf(outfile, "Memory allocation (code space): %d\n",
        (int)(gotten_store -
              sizeof(real_pcre) -
              ((real_pcre *)re)->name_count * ((real_pcre *)re)->name_entry_size));

    /* Extract the size for possible writing before possibly flipping it,
    and remember the store that was got. */

    true_size = ((real_pcre *)re)->size;
    regex_gotten_store = gotten_store;

    /* If /S was present, study the regexp to generate additional info to
    help with the matching. */

    if (do_study)
      {
      if (timeit)
        {
        register int i;
        clock_t time_taken;
        clock_t start_time = clock();
        for (i = 0; i < LOOPREPEAT; i++)
          extra = pcre_study(re, study_options, &error);
        time_taken = clock() - start_time;
        if (extra != NULL) free(extra);
        fprintf(outfile, "  Study time %.3f milliseconds\n",
          (((double)time_taken * 1000.0) / (double)LOOPREPEAT) /
            (double)CLOCKS_PER_SEC);
        }
      extra = pcre_study(re, study_options, &error);
      if (error != NULL)
        fprintf(outfile, "Failed to study: %s\n", error);
      else if (extra != NULL)
        true_study_size = ((pcre_study_data *)(extra->study_data))->size;
      }

    /* If the 'F' option was present, we flip the bytes of all the integer
    fields in the regex data block and the study block. This is to make it
    possible to test PCRE's handling of byte-flipped patterns, e.g. those
    compiled on a different architecture. */

    if (do_flip)
      {
      real_pcre *rre = (real_pcre *)re;
      rre->magic_number = byteflip(rre->magic_number, sizeof(rre->magic_number));
      rre->size = byteflip(rre->size, sizeof(rre->size));
      rre->options = byteflip(rre->options, sizeof(rre->options));
      rre->top_bracket = byteflip(rre->top_bracket, sizeof(rre->top_bracket));
      rre->top_backref = byteflip(rre->top_backref, sizeof(rre->top_backref));
      rre->first_byte = byteflip(rre->first_byte, sizeof(rre->first_byte));
      rre->req_byte = byteflip(rre->req_byte, sizeof(rre->req_byte));
      rre->name_table_offset = byteflip(rre->name_table_offset,
        sizeof(rre->name_table_offset));
      rre->name_entry_size = byteflip(rre->name_entry_size,
        sizeof(rre->name_entry_size));
      rre->name_count = byteflip(rre->name_count, sizeof(rre->name_count));

      if (extra != NULL)
        {
        pcre_study_data *rsd = (pcre_study_data *)(extra->study_data);
        rsd->size = byteflip(rsd->size, sizeof(rsd->size));
        rsd->options = byteflip(rsd->options, sizeof(rsd->options));
        }
      }

    /* Extract information from the compiled data if required */

    SHOW_INFO:

    if (do_showinfo)
      {
      unsigned long int get_options, all_options;
      int old_first_char, old_options, old_count;
      int count, backrefmax, first_char, need_char;
      int nameentrysize, namecount;
      const uschar *nametable;

      if (do_debug)
        {
        fprintf(outfile, "------------------------------------------------------------------\n");
        print_internals(re, outfile);
        }

      new_info(re, NULL, PCRE_INFO_OPTIONS, &get_options);
      new_info(re, NULL, PCRE_INFO_SIZE, &size);
      new_info(re, NULL, PCRE_INFO_CAPTURECOUNT, &count);
      new_info(re, NULL, PCRE_INFO_BACKREFMAX, &backrefmax);
      new_info(re, NULL, PCRE_INFO_FIRSTBYTE, &first_char);
      new_info(re, NULL, PCRE_INFO_LASTLITERAL, &need_char);
      new_info(re, NULL, PCRE_INFO_NAMEENTRYSIZE, &nameentrysize);
      new_info(re, NULL, PCRE_INFO_NAMECOUNT, &namecount);
      new_info(re, NULL, PCRE_INFO_NAMETABLE, (void *)&nametable);

      old_count = pcre_info(re, &old_options, &old_first_char);
      if (count < 0) fprintf(outfile,
        "Error %d from pcre_info()\n", count);
      else
        {
        if (old_count != count) fprintf(outfile,
          "Count disagreement: pcre_fullinfo=%d pcre_info=%d\n", count,
            old_count);

        if (old_first_char != first_char) fprintf(outfile,
          "First char disagreement: pcre_fullinfo=%d pcre_info=%d\n",
            first_char, old_first_char);

        if (old_options != (int)get_options) fprintf(outfile,
          "Options disagreement: pcre_fullinfo=%ld pcre_info=%d\n",
            get_options, old_options);
        }

      if (size != regex_gotten_store) fprintf(outfile,
        "Size disagreement: pcre_fullinfo=%d call to malloc for %d\n",
        size, regex_gotten_store);

      fprintf(outfile, "Capturing subpattern count = %d\n", count);
      if (backrefmax > 0)
        fprintf(outfile, "Max back reference = %d\n", backrefmax);

      if (namecount > 0)
        {
        fprintf(outfile, "Named capturing subpatterns:\n");
        while (namecount-- > 0)
          {
          fprintf(outfile, "  %s %*s%3d\n", nametable + 2,
            nameentrysize - 3 - (int)strlen((char *)nametable + 2), "",
            GET2(nametable, 0));
          nametable += nameentrysize;
          }
        }

      /* The NOPARTIAL bit is a private bit in the options, so we have
      to fish it out via out back door */

      all_options = ((real_pcre *)re)->options;
      if (do_flip)
        {
        all_options = byteflip(all_options, sizeof(all_options));
        }

      if ((all_options & PCRE_NOPARTIAL) != 0)
        fprintf(outfile, "Partial matching not supported\n");

      if (get_options == 0) fprintf(outfile, "No options\n");
        else fprintf(outfile, "Options:%s%s%s%s%s%s%s%s%s%s\n",
          ((get_options & PCRE_ANCHORED) != 0)? " anchored" : "",
          ((get_options & PCRE_CASELESS) != 0)? " caseless" : "",
          ((get_options & PCRE_EXTENDED) != 0)? " extended" : "",
          ((get_options & PCRE_MULTILINE) != 0)? " multiline" : "",
          ((get_options & PCRE_DOTALL) != 0)? " dotall" : "",
          ((get_options & PCRE_DOLLAR_ENDONLY) != 0)? " dollar_endonly" : "",
          ((get_options & PCRE_EXTRA) != 0)? " extra" : "",
          ((get_options & PCRE_UNGREEDY) != 0)? " ungreedy" : "",
          ((get_options & PCRE_UTF8) != 0)? " utf8" : "",
          ((get_options & PCRE_NO_UTF8_CHECK) != 0)? " no_utf8_check" : "");

      if (((((real_pcre *)re)->options) & PCRE_ICHANGED) != 0)
        fprintf(outfile, "Case state changes\n");

      if (first_char == -1)
        {
        fprintf(outfile, "First char at start or follows \\n\n");
        }
      else if (first_char < 0)
        {
        fprintf(outfile, "No first char\n");
        }
      else
        {
        int ch = first_char & 255;
        const char *caseless = ((first_char & REQ_CASELESS) == 0)?
          "" : " (caseless)";
        if (isprint(ch))
          fprintf(outfile, "First char = \'%c\'%s\n", ch, caseless);
        else
          fprintf(outfile, "First char = %d%s\n", ch, caseless);
        }

      if (need_char < 0)
        {
        fprintf(outfile, "No need char\n");
        }
      else
        {
        int ch = need_char & 255;
        const char *caseless = ((need_char & REQ_CASELESS) == 0)?
          "" : " (caseless)";
        if (isprint(ch))
          fprintf(outfile, "Need char = \'%c\'%s\n", ch, caseless);
        else
          fprintf(outfile, "Need char = %d%s\n", ch, caseless);
        }

      /* Don't output study size; at present it is in any case a fixed
      value, but it varies, depending on the computer architecture, and
      so messes up the test suite. (And with the /F option, it might be
      flipped.) */

      if (do_study)
        {
        if (extra == NULL)
          fprintf(outfile, "Study returned NULL\n");
        else
          {
          uschar *start_bits = NULL;
          new_info(re, extra, PCRE_INFO_FIRSTTABLE, &start_bits);

          if (start_bits == NULL)
            fprintf(outfile, "No starting byte set\n");
          else
            {
            int i;
            int c = 24;
            fprintf(outfile, "Starting byte set: ");
            for (i = 0; i < 256; i++)
              {
              if ((start_bits[i/8] & (1<<(i&7))) != 0)
                {
                if (c > 75)
                  {
                  fprintf(outfile, "\n  ");
                  c = 2;
                  }
                if (isprint(i) && i != ' ')
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
          }
        }
      }

    /* If the '>' option was present, we write out the regex to a file, and
    that is all. The first 8 bytes of the file are the regex length and then
    the study length, in big-endian order. */

    if (to_file != NULL)
      {
      FILE *f = fopen((char *)to_file, "wb");
      if (f == NULL)
        {
        fprintf(outfile, "Unable to open %s: %s\n", to_file, strerror(errno));
        }
      else
        {
        uschar sbuf[8];
        sbuf[0] = (true_size >> 24)  & 255;
        sbuf[1] = (true_size >> 16)  & 255;
        sbuf[2] = (true_size >>  8)  & 255;
        sbuf[3] = (true_size)  & 255;

        sbuf[4] = (true_study_size >> 24)  & 255;
        sbuf[5] = (true_study_size >> 16)  & 255;
        sbuf[6] = (true_study_size >>  8)  & 255;
        sbuf[7] = (true_study_size)  & 255;

        if (fwrite(sbuf, 1, 8, f) < 8 ||
            fwrite(re, 1, true_size, f) < true_size)
          {
          fprintf(outfile, "Write error on %s: %s\n", to_file, strerror(errno));
          }
        else
          {
          fprintf(outfile, "Compiled regex written to %s\n", to_file);
          if (extra != NULL)
            {
            if (fwrite(extra->study_data, 1, true_study_size, f) <
                true_study_size)
              {
              fprintf(outfile, "Write error on %s: %s\n", to_file,
                strerror(errno));
              }
            else fprintf(outfile, "Study data written to %s\n", to_file);
            }
          }
        fclose(f);
        }
      continue;  /* With next regex */
      }
    }        /* End of non-POSIX compile */

  /* Read data lines and test them */

  for (;;)
    {
    unsigned char *q;
    unsigned char *bptr = dbuffer;
    int *use_offsets = offsets;
    int use_size_offsets = size_offsets;
    int callout_data = 0;
    int callout_data_set = 0;
    int count, c;
    int copystrings = 0;
    int find_match_limit = 0;
    int getstrings = 0;
    int getlist = 0;
    int gmatched = 0;
    int start_offset = 0;
    int g_notempty = 0;

    options = 0;

    pcre_callout = callout;
    first_callout = 1;
    callout_extra = 0;
    callout_count = 0;
    callout_fail_count = 999999;
    callout_fail_id = -1;
    show_malloc = 0;

    if (infile == stdin) printf("data> ");
    if (fgets((char *)buffer, BUFFER_SIZE, infile) == NULL)
      {
      done = 1;
      goto CONTINUE;
      }
    if (infile != stdin) fprintf(outfile, "%s", (char *)buffer);

    len = (int)strlen((char *)buffer);
    while (len > 0 && isspace(buffer[len-1])) len--;
    buffer[len] = 0;
    if (len == 0) break;

    p = buffer;
    while (isspace(*p)) p++;

    q = dbuffer;
    while ((c = *p++) != 0)
      {
      int i = 0;
      int n = 0;

      if (c == '\\') switch ((c = *p++))
        {
        case 'a': c =    7; break;
        case 'b': c = '\b'; break;
        case 'e': c =   27; break;
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

        case 'x':

        /* Handle \x{..} specially - new Perl thing for utf8 */

        if (*p == '{')
          {
          unsigned char *pt = p;
          c = 0;
          while (isxdigit(*(++pt)))
            c = c * 16 + tolower(*pt) - ((isdigit(*pt))? '0' : 'W');
          if (*pt == '}')
            {
            unsigned char buff8[8];
            int ii, utn;
            utn = ord2utf8(c, buff8);
            for (ii = 0; ii < utn - 1; ii++) *q++ = buff8[ii];
            c = buff8[ii];   /* Last byte */
            p = pt + 1;
            break;
            }
          /* Not correct form; fall through */
          }

        /* Ordinary \x */

        c = 0;
        while (i++ < 2 && isxdigit(*p))
          {
          c = c * 16 + tolower(*p) - ((isdigit(*p))? '0' : 'W');
          p++;
          }
        break;

        case 0:   /* \ followed by EOF allows for an empty line */
        p--;
        continue;

        case '>':
        while(isdigit(*p)) start_offset = start_offset * 10 + *p++ - '0';
        continue;

        case 'A':  /* Option setting */
        options |= PCRE_ANCHORED;
        continue;

        case 'B':
        options |= PCRE_NOTBOL;
        continue;

        case 'C':
        if (isdigit(*p))    /* Set copy string */
          {
          while(isdigit(*p)) n = n * 10 + *p++ - '0';
          copystrings |= 1 << n;
          }
        else if (isalnum(*p))
          {
          uschar name[256];
          uschar *npp = name;
          while (isalnum(*p)) *npp++ = *p++;
          *npp = 0;
          n = pcre_get_stringnumber(re, (char *)name);
          if (n < 0)
            fprintf(outfile, "no parentheses with name \"%s\"\n", name);
          else copystrings |= 1 << n;
          }
        else if (*p == '+')
          {
          callout_extra = 1;
          p++;
          }
        else if (*p == '-')
          {
          pcre_callout = NULL;
          p++;
          }
        else if (*p == '!')
          {
          callout_fail_id = 0;
          p++;
          while(isdigit(*p))
            callout_fail_id = callout_fail_id * 10 + *p++ - '0';
          callout_fail_count = 0;
          if (*p == '!')
            {
            p++;
            while(isdigit(*p))
              callout_fail_count = callout_fail_count * 10 + *p++ - '0';
            }
          }
        else if (*p == '*')
          {
          int sign = 1;
          callout_data = 0;
          if (*(++p) == '-') { sign = -1; p++; }
          while(isdigit(*p))
            callout_data = callout_data * 10 + *p++ - '0';
          callout_data *= sign;
          callout_data_set = 1;
          }
        continue;

        case 'G':
        if (isdigit(*p))
          {
          while(isdigit(*p)) n = n * 10 + *p++ - '0';
          getstrings |= 1 << n;
          }
        else if (isalnum(*p))
          {
          uschar name[256];
          uschar *npp = name;
          while (isalnum(*p)) *npp++ = *p++;
          *npp = 0;
          n = pcre_get_stringnumber(re, (char *)name);
          if (n < 0)
            fprintf(outfile, "no parentheses with name \"%s\"\n", name);
          else getstrings |= 1 << n;
          }
        continue;

        case 'L':
        getlist = 1;
        continue;

        case 'M':
        find_match_limit = 1;
        continue;

        case 'N':
        options |= PCRE_NOTEMPTY;
        continue;

        case 'O':
        while(isdigit(*p)) n = n * 10 + *p++ - '0';
        if (n > size_offsets_max)
          {
          size_offsets_max = n;
          free(offsets);
          use_offsets = offsets = (int *)malloc(size_offsets_max * sizeof(int));
          if (offsets == NULL)
            {
            printf("** Failed to get %d bytes of memory for offsets vector\n",
              size_offsets_max * sizeof(int));
            return 1;
            }
          }
        use_size_offsets = n;
        if (n == 0) use_offsets = NULL;   /* Ensures it can't write to it */
        continue;

        case 'P':
        options |= PCRE_PARTIAL;
        continue;

        case 'S':
        show_malloc = 1;
        continue;

        case 'Z':
        options |= PCRE_NOTEOL;
        continue;

        case '?':
        options |= PCRE_NO_UTF8_CHECK;
        continue;
        }
      *q++ = c;
      }
    *q = 0;
    len = q - dbuffer;

    /* Handle matching via the POSIX interface, which does not
    support timing or playing with the match limit or callout data. */

#if !defined NOPOSIX
    if (posix || do_posix)
      {
      int rc;
      int eflags = 0;
      regmatch_t *pmatch = NULL;
      if (use_size_offsets > 0)
        pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * use_size_offsets);
      if ((options & PCRE_NOTBOL) != 0) eflags |= REG_NOTBOL;
      if ((options & PCRE_NOTEOL) != 0) eflags |= REG_NOTEOL;

      rc = regexec(&preg, (const char *)bptr, use_size_offsets, pmatch, eflags);

      if (rc != 0)
        {
        (void)regerror(rc, &preg, (char *)buffer, BUFFER_SIZE);
        fprintf(outfile, "No match: POSIX code %d: %s\n", rc, buffer);
        }
      else
        {
        size_t i;
        for (i = 0; i < (size_t)use_size_offsets; i++)
          {
          if (pmatch[i].rm_so >= 0)
            {
            fprintf(outfile, "%2d: ", (int)i);
            (void)pchars(dbuffer + pmatch[i].rm_so,
              pmatch[i].rm_eo - pmatch[i].rm_so, outfile);
            fprintf(outfile, "\n");
            if (i == 0 && do_showrest)
              {
              fprintf(outfile, " 0+ ");
              (void)pchars(dbuffer + pmatch[i].rm_eo, len - pmatch[i].rm_eo,
                outfile);
              fprintf(outfile, "\n");
              }
            }
          }
        }
      free(pmatch);
      }

    /* Handle matching via the native interface - repeats for /g and /G */

    else
#endif  /* !defined NOPOSIX */

    for (;; gmatched++)    /* Loop for /g or /G */
      {
      if (timeit)
        {
        register int i;
        clock_t time_taken;
        clock_t start_time = clock();
        for (i = 0; i < LOOPREPEAT; i++)
          count = pcre_exec(re, extra, (char *)bptr, len,
            start_offset, options | g_notempty, use_offsets, use_size_offsets);
        time_taken = clock() - start_time;
        fprintf(outfile, "Execute time %.3f milliseconds\n",
          (((double)time_taken * 1000.0) / (double)LOOPREPEAT) /
            (double)CLOCKS_PER_SEC);
        }

      /* If find_match_limit is set, we want to do repeated matches with
      varying limits in order to find the minimum value. */

      if (find_match_limit)
        {
        int min = 0;
        int mid = 64;
        int max = -1;

        if (extra == NULL)
          {
          extra = (pcre_extra *)malloc(sizeof(pcre_extra));
          extra->flags = 0;
          }
        extra->flags |= PCRE_EXTRA_MATCH_LIMIT;

        for (;;)
          {
          extra->match_limit = mid;
          count = pcre_exec(re, extra, (char *)bptr, len, start_offset,
            options | g_notempty, use_offsets, use_size_offsets);
          if (count == PCRE_ERROR_MATCHLIMIT)
            {
            /* fprintf(outfile, "Testing match limit = %d\n", mid); */
            min = mid;
            mid = (mid == max - 1)? max : (max > 0)? (min + max)/2 : mid*2;
            }
          else if (count >= 0 || count == PCRE_ERROR_NOMATCH ||
                                 count == PCRE_ERROR_PARTIAL)
            {
            if (mid == min + 1)
              {
              fprintf(outfile, "Minimum match limit = %d\n", mid);
              break;
              }
            /* fprintf(outfile, "Testing match limit = %d\n", mid); */
            max = mid;
            mid = (min + mid)/2;
            }
          else break;    /* Some other error */
          }

        extra->flags &= ~PCRE_EXTRA_MATCH_LIMIT;
        }

      /* If callout_data is set, use the interface with additional data */

      else if (callout_data_set)
        {
        if (extra == NULL)
          {
          extra = (pcre_extra *)malloc(sizeof(pcre_extra));
          extra->flags = 0;
          }
        extra->flags |= PCRE_EXTRA_CALLOUT_DATA;
        extra->callout_data = &callout_data;
        count = pcre_exec(re, extra, (char *)bptr, len, start_offset,
          options | g_notempty, use_offsets, use_size_offsets);
        extra->flags &= ~PCRE_EXTRA_CALLOUT_DATA;
        }

      /* The normal case is just to do the match once, with the default
      value of match_limit. */

      else
        {
        count = pcre_exec(re, extra, (char *)bptr, len,
          start_offset, options | g_notempty, use_offsets, use_size_offsets);
        }

      if (count == 0)
        {
        fprintf(outfile, "Matched, but too many substrings\n");
        count = use_size_offsets/3;
        }

      /* Matched */

      if (count >= 0)
        {
        int i;
        for (i = 0; i < count * 2; i += 2)
          {
          if (use_offsets[i] < 0)
            fprintf(outfile, "%2d: <unset>\n", i/2);
          else
            {
            fprintf(outfile, "%2d: ", i/2);
            (void)pchars(bptr + use_offsets[i],
              use_offsets[i+1] - use_offsets[i], outfile);
            fprintf(outfile, "\n");
            if (i == 0)
              {
              if (do_showrest)
                {
                fprintf(outfile, " 0+ ");
                (void)pchars(bptr + use_offsets[i+1], len - use_offsets[i+1],
                  outfile);
                fprintf(outfile, "\n");
                }
              }
            }
          }

        for (i = 0; i < 32; i++)
          {
          if ((copystrings & (1 << i)) != 0)
            {
            char copybuffer[16];
            int rc = pcre_copy_substring((char *)bptr, use_offsets, count,
              i, copybuffer, sizeof(copybuffer));
            if (rc < 0)
              fprintf(outfile, "copy substring %d failed %d\n", i, rc);
            else
              fprintf(outfile, "%2dC %s (%d)\n", i, copybuffer, rc);
            }
          }

        for (i = 0; i < 32; i++)
          {
          if ((getstrings & (1 << i)) != 0)
            {
            const char *substring;
            int rc = pcre_get_substring((char *)bptr, use_offsets, count,
              i, &substring);
            if (rc < 0)
              fprintf(outfile, "get substring %d failed %d\n", i, rc);
            else
              {
              fprintf(outfile, "%2dG %s (%d)\n", i, substring, rc);
              /* free((void *)substring); */
              pcre_free_substring(substring);
              }
            }
          }

        if (getlist)
          {
          const char **stringlist;
          int rc = pcre_get_substring_list((char *)bptr, use_offsets, count,
            &stringlist);
          if (rc < 0)
            fprintf(outfile, "get substring list failed %d\n", rc);
          else
            {
            for (i = 0; i < count; i++)
              fprintf(outfile, "%2dL %s\n", i, stringlist[i]);
            if (stringlist[i] != NULL)
              fprintf(outfile, "string list not terminated by NULL\n");
            /* free((void *)stringlist); */
            pcre_free_substring_list(stringlist);
            }
          }
        }

      /* There was a partial match */

      else if (count == PCRE_ERROR_PARTIAL)
        {
        fprintf(outfile, "Partial match\n");
        break;  /* Out of the /g loop */
        }

      /* Failed to match. If this is a /g or /G loop and we previously set
      g_notempty after a null match, this is not necessarily the end.
      We want to advance the start offset, and continue. In the case of UTF-8
      matching, the advance must be one character, not one byte. Fudge the
      offset values to achieve this. We won't be at the end of the string -
      that was checked before setting g_notempty. */

      else
        {
        if (g_notempty != 0)
          {
          int onechar = 1;
          use_offsets[0] = start_offset;
          if (use_utf8)
            {
            while (start_offset + onechar < len)
              {
              int tb = bptr[start_offset+onechar];
              if (tb <= 127) break;
              tb &= 0xc0;
              if (tb != 0 && tb != 0xc0) onechar++;
              }
            }
          use_offsets[1] = start_offset + onechar;
          }
        else
          {
          if (count == PCRE_ERROR_NOMATCH)
            {
            if (gmatched == 0) fprintf(outfile, "No match\n");
            }
          else fprintf(outfile, "Error %d\n", count);
          break;  /* Out of the /g loop */
          }
        }

      /* If not /g or /G we are done */

      if (!do_g && !do_G) break;

      /* If we have matched an empty string, first check to see if we are at
      the end of the subject. If so, the /g loop is over. Otherwise, mimic
      what Perl's /g options does. This turns out to be rather cunning. First
      we set PCRE_NOTEMPTY and PCRE_ANCHORED and try the match again at the
      same point. If this fails (picked up above) we advance to the next
      character. */

      g_notempty = 0;
      if (use_offsets[0] == use_offsets[1])
        {
        if (use_offsets[0] == len) break;
        g_notempty = PCRE_NOTEMPTY | PCRE_ANCHORED;
        }

      /* For /g, update the start offset, leaving the rest alone */

      if (do_g) start_offset = use_offsets[1];

      /* For /G, update the pointer and length */

      else
        {
        bptr += use_offsets[1];
        len -= use_offsets[1];
        }
      }  /* End of loop for /g and /G */
    }    /* End of loop for data lines */

  CONTINUE:

#if !defined NOPOSIX
  if (posix || do_posix) regfree(&preg);
#endif

  if (re != NULL) free(re);
  if (extra != NULL) free(extra);
  if (tables != NULL)
    {
    free((void *)tables);
    setlocale(LC_CTYPE, "C");
    }
  }

if (infile == stdin) fprintf(outfile, "\n");
return 0;
}

/* End */
