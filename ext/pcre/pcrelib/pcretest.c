/*************************************************
*             PCRE testing program               *
*************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

/* Use the internal info for displaying the results of pcre_study(). */

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

#define LOOPREPEAT 20000


static FILE *outfile;
static int log_store = 0;



/* Debugging function to print the internal form of the regex. This is the same
code as contained in pcre.c under the DEBUG macro. */

static const char *OP_names[] = {
  "End", "\\A", "\\B", "\\b", "\\D", "\\d",
  "\\S", "\\s", "\\W", "\\w", "\\Z", "\\z",
  "Opt", "^", "$", "Any", "chars", "not",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{",
  "class", "Ref",
  "Alt", "Ket", "KetRmax", "KetRmin", "Assert", "Assert not",
  "AssertB", "AssertB not", "Reverse", "Once", "Cond", "Cref",
  "Brazero", "Braminzero", "Bra"
};


static void print_internals(pcre *re)
{
unsigned char *code = ((real_pcre *)re)->code;

fprintf(outfile, "------------------------------------------------------------------\n");

for(;;)
  {
  int c;
  int charlength;

  fprintf(outfile, "%3d ", (int)(code - ((real_pcre *)re)->code));

  if (*code >= OP_BRA)
    {
    fprintf(outfile, "%3d Bra %d", (code[1] << 8) + code[2], *code - OP_BRA);
    code += 2;
    }

  else switch(*code)
    {
    case OP_END:
    fprintf(outfile, "    %s\n", OP_names[*code]);
    fprintf(outfile, "------------------------------------------------------------------\n");
    return;

    case OP_OPT:
    fprintf(outfile, " %.2x %s", code[1], OP_names[*code]);
    code++;
    break;

    case OP_COND:
    fprintf(outfile, "%3d Cond", (code[1] << 8) + code[2]);
    code += 2;
    break;

    case OP_CREF:
    fprintf(outfile, " %.2d %s", code[1], OP_names[*code]);
    code++;
    break;

    case OP_CHARS:
    charlength = *(++code);
    fprintf(outfile, "%3d ", charlength);
    while (charlength-- > 0)
      if (isprint(c = *(++code))) fprintf(outfile, "%c", c);
        else fprintf(outfile, "\\x%02x", c);
    break;

    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_ALT:
    case OP_KET:
    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    case OP_ONCE:
    fprintf(outfile, "%3d %s", (code[1] << 8) + code[2], OP_names[*code]);
    code += 2;
    break;

    case OP_REVERSE:
    fprintf(outfile, "%3d %s", (code[1] << 8) + code[2], OP_names[*code]);
    code += 2;
    break;

    case OP_STAR:
    case OP_MINSTAR:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    if (*code >= OP_TYPESTAR)
      fprintf(outfile, "    %s", OP_names[code[1]]);
    else if (isprint(c = code[1])) fprintf(outfile, "    %c", c);
      else fprintf(outfile, "    \\x%02x", c);
    fprintf(outfile, "%s", OP_names[*code++]);
    break;

    case OP_EXACT:
    case OP_UPTO:
    case OP_MINUPTO:
    if (isprint(c = code[3])) fprintf(outfile, "    %c{", c);
      else fprintf(outfile, "    \\x%02x{", c);
    if (*code != OP_EXACT) fprintf(outfile, ",");
    fprintf(outfile, "%d}", (code[1] << 8) + code[2]);
    if (*code == OP_MINUPTO) fprintf(outfile, "?");
    code += 3;
    break;

    case OP_TYPEEXACT:
    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    fprintf(outfile, "    %s{", OP_names[code[3]]);
    if (*code != OP_TYPEEXACT) fprintf(outfile, "0,");
    fprintf(outfile, "%d}", (code[1] << 8) + code[2]);
    if (*code == OP_TYPEMINUPTO) fprintf(outfile, "?");
    code += 3;
    break;

    case OP_NOT:
    if (isprint(c = *(++code))) fprintf(outfile, "    [^%c]", c);
      else fprintf(outfile, "    [^\\x%02x]", c);
    break;

    case OP_NOTSTAR:
    case OP_NOTMINSTAR:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTQUERY:
    case OP_NOTMINQUERY:
    if (isprint(c = code[1])) fprintf(outfile, "    [^%c]", c);
      else fprintf(outfile, "    [^\\x%02x]", c);
    fprintf(outfile, "%s", OP_names[*code++]);
    break;

    case OP_NOTEXACT:
    case OP_NOTUPTO:
    case OP_NOTMINUPTO:
    if (isprint(c = code[3])) fprintf(outfile, "    [^%c]{", c);
      else fprintf(outfile, "    [^\\x%02x]{", c);
    if (*code != OP_NOTEXACT) fprintf(outfile, ",");
    fprintf(outfile, "%d}", (code[1] << 8) + code[2]);
    if (*code == OP_NOTMINUPTO) fprintf(outfile, "?");
    code += 3;
    break;

    case OP_REF:
    fprintf(outfile, "    \\%d", *(++code));
    code++;
    goto CLASS_REF_REPEAT;

    case OP_CLASS:
      {
      int i, min, max;
      code++;
      fprintf(outfile, "    [");

      for (i = 0; i < 256; i++)
        {
        if ((code[i/8] & (1 << (i&7))) != 0)
          {
          int j;
          for (j = i+1; j < 256; j++)
            if ((code[j/8] & (1 << (j&7))) == 0) break;
          if (i == '-' || i == ']') fprintf(outfile, "\\");
          if (isprint(i)) fprintf(outfile, "%c", i); else fprintf(outfile, "\\x%02x", i);
          if (--j > i)
            {
            fprintf(outfile, "-");
            if (j == '-' || j == ']') fprintf(outfile, "\\");
            if (isprint(j)) fprintf(outfile, "%c", j); else fprintf(outfile, "\\x%02x", j);
            }
          i = j;
          }
        }
      fprintf(outfile, "]");
      code += 32;

      CLASS_REF_REPEAT:

      switch(*code)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        fprintf(outfile, "%s", OP_names[*code]);
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        min = (code[1] << 8) + code[2];
        max = (code[3] << 8) + code[4];
        if (max == 0) fprintf(outfile, "{%d,}", min);
        else fprintf(outfile, "{%d,%d}", min, max);
        if (*code == OP_CRMINRANGE) fprintf(outfile, "?");
        code += 4;
        break;

        default:
        code--;
        }
      }
    break;

    /* Anything else is just a one-node item */

    default:
    fprintf(outfile, "    %s", OP_names[*code]);
    break;
    }

  code++;
  fprintf(outfile, "\n");
  }
}



/* Character string printing function. */

static void pchars(unsigned char *p, int length)
{
int c;
while (length-- > 0)
  if (isprint(c = *(p++))) fprintf(outfile, "%c", c);
    else fprintf(outfile, "\\x%02x", c);
}



/* Alternative malloc function, to test functionality and show the size of the
compiled re. */

static void *new_malloc(size_t size)
{
if (log_store)
  fprintf(outfile, "Memory allocation (code space): %d\n",
    (int)((int)size - offsetof(real_pcre, code[0])));
return malloc(size);
}



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
int posix = 0;
int debug = 0;
int done = 0;
unsigned char buffer[30000];
unsigned char dbuffer[1024];

/* Static so that new_malloc can use it. */

outfile = stdout;

/* Scan options */

while (argc > 1 && argv[op][0] == '-')
  {
  if (strcmp(argv[op], "-s") == 0 || strcmp(argv[op], "-m") == 0)
    showstore = 1;
  else if (strcmp(argv[op], "-t") == 0) timeit = 1;
  else if (strcmp(argv[op], "-i") == 0) showinfo = 1;
  else if (strcmp(argv[op], "-d") == 0) showinfo = debug = 1;
  else if (strcmp(argv[op], "-p") == 0) posix = 1;
  else
    {
    printf("*** Unknown option %s\n", argv[op]);
    printf("Usage: pcretest [-d] [-i] [-p] [-s] [-t] [<input> [<output>]]\n");
    printf("  -d   debug: show compiled code; implies -i\n"
           "  -i   show information about compiled pattern\n"
           "  -p   use POSIX interface\n"
           "  -s   output store information\n"
           "  -t   time compilation and execution\n");
    return 1;
    }
  op++;
  argc--;
  }

/* Sort out the input and output files */

if (argc > 1)
  {
  infile = fopen(argv[op], "r");
  if (infile == NULL)
    {
    printf("** Failed to open %s\n", argv[op]);
    return 1;
    }
  }

if (argc > 2)
  {
  outfile = fopen(argv[op+1], "w");
  if (outfile == NULL)
    {
    printf("** Failed to open %s\n", argv[op+1]);
    return 1;
    }
  }

/* Set alternative malloc function */

pcre_malloc = new_malloc;

/* Heading line, then prompt for first regex if stdin */

fprintf(outfile, "PCRE version %s\n\n", pcre_version());

/* Main loop */

while (!done)
  {
  pcre *re = NULL;
  pcre_extra *extra = NULL;

#if !defined NOPOSIX  /* There are still compilers that require no indent */
  regex_t preg;
#endif

  const char *error;
  unsigned char *p, *pp, *ppp;
  unsigned const char *tables = NULL;
  int do_study = 0;
  int do_debug = debug;
  int do_G = 0;
  int do_g = 0;
  int do_showinfo = showinfo;
  int do_showrest = 0;
  int do_posix = 0;
  int erroroffset, len, delimiter;

  if (infile == stdin) printf("  re> ");
  if (fgets((char *)buffer, sizeof(buffer), infile) == NULL) break;
  if (infile != stdin) fprintf(outfile, "%s", (char *)buffer);

  p = buffer;
  while (isspace(*p)) p++;
  if (*p == 0) continue;

  /* Get the delimiter and seek the end of the pattern; if is isn't
  complete, read more. */

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

    len = sizeof(buffer) - (pp - buffer);
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

  /* Terminate the pattern at the delimiter */

  *pp++ = 0;

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
      case 'D': do_debug = do_showinfo = 1; break;
      case 'E': options |= PCRE_DOLLAR_ENDONLY; break;
      case 'G': do_G = 1; break;
      case 'I': do_showinfo = 1; break;
      case 'M': log_store = 1; break;

#if !defined NOPOSIX
      case 'P': do_posix = 1; break;
#endif

      case 'S': do_study = 1; break;
      case 'U': options |= PCRE_UNGREEDY; break;
      case 'X': options |= PCRE_EXTRA; break;

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
      (void)regerror(rc, &preg, (char *)buffer, sizeof(buffer));
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
        ((double)time_taken * 1000.0) /
        ((double)LOOPREPEAT * (double)CLOCKS_PER_SEC));
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
          if (fgets((char *)buffer, sizeof(buffer), infile) == NULL)
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

    /* Compilation succeeded; print data if required */

    if (do_showinfo)
      {
      int first_char, count;

      if (do_debug) print_internals(re);

      count = pcre_info(re, &options, &first_char);
      if (count < 0) fprintf(outfile,
        "Error %d while reading info\n", count);
      else
        {
        fprintf(outfile, "Identifying subpattern count = %d\n", count);
        if (options == 0) fprintf(outfile, "No options\n");
          else fprintf(outfile, "Options:%s%s%s%s%s%s%s%s\n",
            ((options & PCRE_ANCHORED) != 0)? " anchored" : "",
            ((options & PCRE_CASELESS) != 0)? " caseless" : "",
            ((options & PCRE_EXTENDED) != 0)? " extended" : "",
            ((options & PCRE_MULTILINE) != 0)? " multiline" : "",
            ((options & PCRE_DOTALL) != 0)? " dotall" : "",
            ((options & PCRE_DOLLAR_ENDONLY) != 0)? " dollar_endonly" : "",
            ((options & PCRE_EXTRA) != 0)? " extra" : "",
            ((options & PCRE_UNGREEDY) != 0)? " ungreedy" : "");

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
          if (isprint(first_char))
            fprintf(outfile, "First char = \'%c\'\n", first_char);
          else
            fprintf(outfile, "First char = %d\n", first_char);
          }

        if (((((real_pcre *)re)->options) & PCRE_REQCHSET) != 0)
          {
          int req_char = ((real_pcre *)re)->req_char;
          if (isprint(req_char))
            fprintf(outfile, "Req char = \'%c\'\n", req_char);
          else
            fprintf(outfile, "Req char = %d\n", req_char);
          }
        else fprintf(outfile, "No req char\n");
        }
      }

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
          ((double)time_taken * 1000.0)/
          ((double)LOOPREPEAT * (double)CLOCKS_PER_SEC));
        }

      extra = pcre_study(re, study_options, &error);
      if (error != NULL)
        fprintf(outfile, "Failed to study: %s\n", error);
      else if (extra == NULL)
        fprintf(outfile, "Study returned NULL\n");

      /* This looks at internal information. A bit kludgy to do it this
      way, but it is useful for testing. */

      else if (do_showinfo)
        {
        real_pcre_extra *xx = (real_pcre_extra *)extra;
        if ((xx->options & PCRE_STUDY_MAPPED) == 0)
          fprintf(outfile, "No starting character set\n");
        else
          {
          int i;
          int c = 24;
          fprintf(outfile, "Starting character set: ");
          for (i = 0; i < 256; i++)
            {
            if ((xx->start_bits[i/8] & (1<<(i%8))) != 0)
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

  /* Read data lines and test them */

  for (;;)
    {
    unsigned char *q;
    unsigned char *bptr = dbuffer;
    int count, c;
    int copystrings = 0;
    int getstrings = 0;
    int getlist = 0;
    int gmatched = 0;
    int start_offset = 0;
    int offsets[45];
    int size_offsets = sizeof(offsets)/sizeof(int);

    options = 0;

    if (infile == stdin) printf("data> ");
    if (fgets((char *)buffer, sizeof(buffer), infile) == NULL)
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
        c = 0;
        while (i++ < 2 && isxdigit(*p))
          {
          c = c * 16 + tolower(*p) - ((isdigit(*p))? '0' : 'W');
          p++;
          }
        break;

        case 0:   /* Allows for an empty line */
        p--;
        continue;

        case 'A':  /* Option setting */
        options |= PCRE_ANCHORED;
        continue;

        case 'B':
        options |= PCRE_NOTBOL;
        continue;

        case 'C':
        while(isdigit(*p)) n = n * 10 + *p++ - '0';
        copystrings |= 1 << n;
        continue;

        case 'G':
        while(isdigit(*p)) n = n * 10 + *p++ - '0';
        getstrings |= 1 << n;
        continue;

        case 'L':
        getlist = 1;
        continue;

        case 'N':
        options |= PCRE_NOTEMPTY;
        continue;

        case 'O':
        while(isdigit(*p)) n = n * 10 + *p++ - '0';
        if (n <= (int)(sizeof(offsets)/sizeof(int))) size_offsets = n;
        continue;

        case 'Z':
        options |= PCRE_NOTEOL;
        continue;
        }
      *q++ = c;
      }
    *q = 0;
    len = q - dbuffer;

    /* Handle matching via the POSIX interface, which does not
    support timing. */

#if !defined NOPOSIX
    if (posix || do_posix)
      {
      int rc;
      int eflags = 0;
      regmatch_t pmatch[30];
      if ((options & PCRE_NOTBOL) != 0) eflags |= REG_NOTBOL;
      if ((options & PCRE_NOTEOL) != 0) eflags |= REG_NOTEOL;

      rc = regexec(&preg, (const char *)bptr,
        sizeof(pmatch)/sizeof(regmatch_t), pmatch, eflags);

      if (rc != 0)
        {
        (void)regerror(rc, &preg, (char *)buffer, sizeof(buffer));
        fprintf(outfile, "No match: POSIX code %d: %s\n", rc, buffer);
        }
      else
        {
        size_t i;
        for (i = 0; i < sizeof(pmatch)/sizeof(regmatch_t); i++)
          {
          if (pmatch[i].rm_so >= 0)
            {
            fprintf(outfile, "%2d: ", (int)i);
            pchars(dbuffer + pmatch[i].rm_so,
              pmatch[i].rm_eo - pmatch[i].rm_so);
            fprintf(outfile, "\n");
            if (i == 0 && do_showrest)
              {
              fprintf(outfile, " 0+ ");
              pchars(dbuffer + pmatch[i].rm_eo, len - pmatch[i].rm_eo);
              fprintf(outfile, "\n");
              }
            }
          }
        }
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
            start_offset, options, offsets, size_offsets);
        time_taken = clock() - start_time;
        fprintf(outfile, "Execute time %.3f milliseconds\n",
          ((double)time_taken * 1000.0)/
          ((double)LOOPREPEAT * (double)CLOCKS_PER_SEC));
        }

      count = pcre_exec(re, extra, (char *)bptr, len,
        start_offset, options, offsets, size_offsets);

      if (count == 0)
        {
        fprintf(outfile, "Matched, but too many substrings\n");
        count = size_offsets/3;
        }

      /* Matched */

      if (count >= 0)
        {
        int i;
        for (i = 0; i < count * 2; i += 2)
          {
          if (offsets[i] < 0)
            fprintf(outfile, "%2d: <unset>\n", i/2);
          else
            {
            fprintf(outfile, "%2d: ", i/2);
            pchars(bptr + offsets[i], offsets[i+1] - offsets[i]);
            fprintf(outfile, "\n");
            if (i == 0)
              {
              if (do_showrest)
                {
                fprintf(outfile, " 0+ ");
                pchars(bptr + offsets[i+1], len - offsets[i+1]);
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
            int rc = pcre_copy_substring((char *)bptr, offsets, count,
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
            int rc = pcre_get_substring((char *)bptr, offsets, count,
              i, &substring);
            if (rc < 0)
              fprintf(outfile, "get substring %d failed %d\n", i, rc);
            else
              {
              fprintf(outfile, "%2dG %s (%d)\n", i, substring, rc);
              free((void *)substring);
              }
            }
          }

        if (getlist)
          {
          const char **stringlist;
          int rc = pcre_get_substring_list((char *)bptr, offsets, count,
            &stringlist);
          if (rc < 0)
            fprintf(outfile, "get substring list failed %d\n", rc);
          else
            {
            for (i = 0; i < count; i++)
              fprintf(outfile, "%2dL %s\n", i, stringlist[i]);
            if (stringlist[i] != NULL)
              fprintf(outfile, "string list not terminated by NULL\n");
            free((void *)stringlist);
            }
          }
        }

      /* Failed to match */

      else
        {
        if (gmatched == 0)
          {
          if (count == -1) fprintf(outfile, "No match\n");
            else fprintf(outfile, "Error %d\n", count);
          }
        break;  /* Out of the /g loop */
        }

      /* If not /g or /G we are done */

      if (!do_g && !do_G) break;

      /* If we have matched an empty string, set PCRE_NOTEMPTY for the next
      match. This mimics what Perl's /g option does. */

      if (offsets[1] == offsets[0])
        options |= PCRE_NOTEMPTY;
      else
        options &= ~PCRE_NOTEMPTY;

      /* For /g, update the start offset, leaving the rest alone */

      if (do_g) start_offset = offsets[1];

      /* For /G, update the pointer and length */

      else
        {
        bptr += offsets[1];
        len -= offsets[1];
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

fprintf(outfile, "\n");
return 0;
}

/* End */
