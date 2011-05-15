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


/* This module contains a PCRE private debugging function for printing out the
internal form of a compiled regular expression, along with some supporting
local functions. This source file is used in two places:

(1) It is #included by pcre_compile.c when it is compiled in debugging mode
(PCRE_DEBUG defined in pcre_internal.h). It is not included in production
compiles.

(2) It is always #included by pcretest.c, which can be asked to print out a
compiled regex for debugging purposes. */


/* Macro that decides whether a character should be output as a literal or in
hexadecimal. We don't use isprint() because that can vary from system to system
(even without the use of locales) and we want the output always to be the same,
for testing purposes. This macro is used in pcretest as well as in this file. */

#ifdef EBCDIC
#define PRINTABLE(c) ((c) >= 64 && (c) < 255)
#else
#define PRINTABLE(c) ((c) >= 32 && (c) < 127)
#endif

/* The table of operator names. */

static const char *OP_names[] = { OP_NAME_LIST };



/*************************************************
*       Print single- or multi-byte character    *
*************************************************/

static int
print_char(FILE *f, uschar *ptr, BOOL utf8)
{
int c = *ptr;

#ifndef SUPPORT_UTF8
utf8 = utf8;  /* Avoid compiler warning */
if (PRINTABLE(c)) fprintf(f, "%c", c); else fprintf(f, "\\x%02x", c);
return 0;

#else
if (!utf8 || (c & 0xc0) != 0xc0)
  {
  if (PRINTABLE(c)) fprintf(f, "%c", c); else fprintf(f, "\\x%02x", c);
  return 0;
  }
else
  {
  int i;
  int a = _pcre_utf8_table4[c & 0x3f];  /* Number of additional bytes */
  int s = 6*a;
  c = (c & _pcre_utf8_table3[a]) << s;
  for (i = 1; i <= a; i++)
    {
    /* This is a check for malformed UTF-8; it should only occur if the sanity
    check has been turned off. Rather than swallow random bytes, just stop if
    we hit a bad one. Print it with \X instead of \x as an indication. */

    if ((ptr[i] & 0xc0) != 0x80)
      {
      fprintf(f, "\\X{%x}", c);
      return i - 1;
      }

    /* The byte is OK */

    s -= 6;
    c |= (ptr[i] & 0x3f) << s;
    }
  if (c < 128) fprintf(f, "\\x%02x", c); else fprintf(f, "\\x{%x}", c);
  return a;
  }
#endif
}



/*************************************************
*          Find Unicode property name            *
*************************************************/

static const char *
get_ucpname(int ptype, int pvalue)
{
#ifdef SUPPORT_UCP
int i;
for (i = _pcre_utt_size - 1; i >= 0; i--)
  {
  if (ptype == _pcre_utt[i].type && pvalue == _pcre_utt[i].value) break;
  }
return (i >= 0)? _pcre_utt_names + _pcre_utt[i].name_offset : "??";
#else
/* It gets harder and harder to shut off unwanted compiler warnings. */
ptype = ptype * pvalue;
return (ptype == pvalue)? "??" : "??";
#endif
}



/*************************************************
*         Print compiled regex                   *
*************************************************/

/* Make this function work for a regex with integers either byte order.
However, we assume that what we are passed is a compiled regex. The
print_lengths flag controls whether offsets and lengths of items are printed.
They can be turned off from pcretest so that automatic tests on bytecode can be
written that do not depend on the value of LINK_SIZE. */

static void
pcre_printint(pcre *external_re, FILE *f, BOOL print_lengths)
{
real_pcre *re = (real_pcre *)external_re;
uschar *codestart, *code;
BOOL utf8;

unsigned int options = re->options;
int offset = re->name_table_offset;
int count = re->name_count;
int size = re->name_entry_size;

if (re->magic_number != MAGIC_NUMBER)
  {
  offset = ((offset << 8) & 0xff00) | ((offset >> 8) & 0xff);
  count = ((count << 8) & 0xff00) | ((count >> 8) & 0xff);
  size = ((size << 8) & 0xff00) | ((size >> 8) & 0xff);
  options = ((options << 24) & 0xff000000) |
            ((options <<  8) & 0x00ff0000) |
            ((options >>  8) & 0x0000ff00) |
            ((options >> 24) & 0x000000ff);
  }

code = codestart = (uschar *)re + offset + count * size;
utf8 = (options & PCRE_UTF8) != 0;

for(;;)
  {
  uschar *ccode;
  int c;
  int extra = 0;

  if (print_lengths)
    fprintf(f, "%3d ", (int)(code - codestart));
  else
    fprintf(f, "    ");

  switch(*code)
    {
/* ========================================================================== */
      /* These cases are never obeyed. This is a fudge that causes a compile-
      time error if the vectors OP_names or _pcre_OP_lengths, which are indexed
      by opcode, are not the correct length. It seems to be the only way to do
      such a check at compile time, as the sizeof() operator does not work in
      the C preprocessor. We do this while compiling pcretest, because that
      #includes pcre_tables.c, which holds _pcre_OP_lengths. We can't do this
      when building pcre_compile.c with PCRE_DEBUG set, because it doesn't then
      know the size of _pcre_OP_lengths. */

#ifdef COMPILING_PCRETEST
      case OP_TABLE_LENGTH:
      case OP_TABLE_LENGTH +
        ((sizeof(OP_names)/sizeof(const char *) == OP_TABLE_LENGTH) &&
        (sizeof(_pcre_OP_lengths) == OP_TABLE_LENGTH)):
      break;
#endif
/* ========================================================================== */

    case OP_END:
    fprintf(f, "    %s\n", OP_names[*code]);
    fprintf(f, "------------------------------------------------------------------\n");
    return;

    case OP_OPT:
    fprintf(f, " %.2x %s", code[1], OP_names[*code]);
    break;

    case OP_CHAR:
    fprintf(f, "    ");
    do
      {
      code++;
      code += 1 + print_char(f, code, utf8);
      }
    while (*code == OP_CHAR);
    fprintf(f, "\n");
    continue;

    case OP_CHARNC:
    fprintf(f, " NC ");
    do
      {
      code++;
      code += 1 + print_char(f, code, utf8);
      }
    while (*code == OP_CHARNC);
    fprintf(f, "\n");
    continue;

    case OP_CBRA:
    case OP_SCBRA:
    if (print_lengths) fprintf(f, "%3d ", GET(code, 1));
      else fprintf(f, "    ");
    fprintf(f, "%s %d", OP_names[*code], GET2(code, 1+LINK_SIZE));
    break;

    case OP_BRA:
    case OP_SBRA:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_ALT:
    case OP_KET:
    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    case OP_ONCE:
    case OP_COND:
    case OP_SCOND:
    case OP_REVERSE:
    if (print_lengths) fprintf(f, "%3d ", GET(code, 1));
      else fprintf(f, "    ");
    fprintf(f, "%s", OP_names[*code]);
    break;

    case OP_CLOSE:
    fprintf(f, "    %s %d", OP_names[*code], GET2(code, 1));
    break;

    case OP_CREF:
    case OP_NCREF:
    fprintf(f, "%3d %s", GET2(code,1), OP_names[*code]);
    break;

    case OP_RREF:
    c = GET2(code, 1);
    if (c == RREF_ANY)
      fprintf(f, "    Cond recurse any");
    else
      fprintf(f, "    Cond recurse %d", c);
    break;

    case OP_NRREF:
    c = GET2(code, 1);
    if (c == RREF_ANY)
      fprintf(f, "    Cond nrecurse any");
    else
      fprintf(f, "    Cond nrecurse %d", c);
    break;

    case OP_DEF:
    fprintf(f, "    Cond def");
    break;

    case OP_STAR:
    case OP_MINSTAR:
    case OP_POSSTAR:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_POSPLUS:
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_POSQUERY:
    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPOSSTAR:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEPOSPLUS:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    case OP_TYPEPOSQUERY:
    fprintf(f, "    ");
    if (*code >= OP_TYPESTAR)
      {
      fprintf(f, "%s", OP_names[code[1]]);
      if (code[1] == OP_PROP || code[1] == OP_NOTPROP)
        {
        fprintf(f, " %s ", get_ucpname(code[2], code[3]));
        extra = 2;
        }
      }
    else extra = print_char(f, code+1, utf8);
    fprintf(f, "%s", OP_names[*code]);
    break;

    case OP_EXACT:
    case OP_UPTO:
    case OP_MINUPTO:
    case OP_POSUPTO:
    fprintf(f, "    ");
    extra = print_char(f, code+3, utf8);
    fprintf(f, "{");
    if (*code != OP_EXACT) fprintf(f, "0,");
    fprintf(f, "%d}", GET2(code,1));
    if (*code == OP_MINUPTO) fprintf(f, "?");
      else if (*code == OP_POSUPTO) fprintf(f, "+");
    break;

    case OP_TYPEEXACT:
    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    case OP_TYPEPOSUPTO:
    fprintf(f, "    %s", OP_names[code[3]]);
    if (code[3] == OP_PROP || code[3] == OP_NOTPROP)
      {
      fprintf(f, " %s ", get_ucpname(code[4], code[5]));
      extra = 2;
      }
    fprintf(f, "{");
    if (*code != OP_TYPEEXACT) fprintf(f, "0,");
    fprintf(f, "%d}", GET2(code,1));
    if (*code == OP_TYPEMINUPTO) fprintf(f, "?");
      else if (*code == OP_TYPEPOSUPTO) fprintf(f, "+");
    break;

    case OP_NOT:
    c = code[1];
    if (PRINTABLE(c)) fprintf(f, "    [^%c]", c);
      else fprintf(f, "    [^\\x%02x]", c);
    break;

    case OP_NOTSTAR:
    case OP_NOTMINSTAR:
    case OP_NOTPOSSTAR:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTPOSPLUS:
    case OP_NOTQUERY:
    case OP_NOTMINQUERY:
    case OP_NOTPOSQUERY:
    c = code[1];
    if (PRINTABLE(c)) fprintf(f, "    [^%c]", c);
      else fprintf(f, "    [^\\x%02x]", c);
    fprintf(f, "%s", OP_names[*code]);
    break;

    case OP_NOTEXACT:
    case OP_NOTUPTO:
    case OP_NOTMINUPTO:
    case OP_NOTPOSUPTO:
    c = code[3];
    if (PRINTABLE(c)) fprintf(f, "    [^%c]{", c);
      else fprintf(f, "    [^\\x%02x]{", c);
    if (*code != OP_NOTEXACT) fprintf(f, "0,");
    fprintf(f, "%d}", GET2(code,1));
    if (*code == OP_NOTMINUPTO) fprintf(f, "?");
      else if (*code == OP_NOTPOSUPTO) fprintf(f, "+");
    break;

    case OP_RECURSE:
    if (print_lengths) fprintf(f, "%3d ", GET(code, 1));
      else fprintf(f, "    ");
    fprintf(f, "%s", OP_names[*code]);
    break;

    case OP_REF:
    fprintf(f, "    \\%d", GET2(code,1));
    ccode = code + _pcre_OP_lengths[*code];
    goto CLASS_REF_REPEAT;

    case OP_CALLOUT:
    fprintf(f, "    %s %d %d %d", OP_names[*code], code[1], GET(code,2),
      GET(code, 2 + LINK_SIZE));
    break;

    case OP_PROP:
    case OP_NOTPROP:
    fprintf(f, "    %s %s", OP_names[*code], get_ucpname(code[1], code[2]));
    break;

    /* OP_XCLASS can only occur in UTF-8 mode. However, there's no harm in
    having this code always here, and it makes it less messy without all those
    #ifdefs. */

    case OP_CLASS:
    case OP_NCLASS:
    case OP_XCLASS:
      {
      int i, min, max;
      BOOL printmap;

      fprintf(f, "    [");

      if (*code == OP_XCLASS)
        {
        extra = GET(code, 1);
        ccode = code + LINK_SIZE + 1;
        printmap = (*ccode & XCL_MAP) != 0;
        if ((*ccode++ & XCL_NOT) != 0) fprintf(f, "^");
        }
      else
        {
        printmap = TRUE;
        ccode = code + 1;
        }

      /* Print a bit map */

      if (printmap)
        {
        for (i = 0; i < 256; i++)
          {
          if ((ccode[i/8] & (1 << (i&7))) != 0)
            {
            int j;
            for (j = i+1; j < 256; j++)
              if ((ccode[j/8] & (1 << (j&7))) == 0) break;
            if (i == '-' || i == ']') fprintf(f, "\\");
            if (PRINTABLE(i)) fprintf(f, "%c", i);
              else fprintf(f, "\\x%02x", i);
            if (--j > i)
              {
              if (j != i + 1) fprintf(f, "-");
              if (j == '-' || j == ']') fprintf(f, "\\");
              if (PRINTABLE(j)) fprintf(f, "%c", j);
                else fprintf(f, "\\x%02x", j);
              }
            i = j;
            }
          }
        ccode += 32;
        }

      /* For an XCLASS there is always some additional data */

      if (*code == OP_XCLASS)
        {
        int ch;
        while ((ch = *ccode++) != XCL_END)
          {
          if (ch == XCL_PROP)
            {
            int ptype = *ccode++;
            int pvalue = *ccode++;
            fprintf(f, "\\p{%s}", get_ucpname(ptype, pvalue));
            }
          else if (ch == XCL_NOTPROP)
            {
            int ptype = *ccode++;
            int pvalue = *ccode++;
            fprintf(f, "\\P{%s}", get_ucpname(ptype, pvalue));
            }
          else
            {
            ccode += 1 + print_char(f, ccode, TRUE);
            if (ch == XCL_RANGE)
              {
              fprintf(f, "-");
              ccode += 1 + print_char(f, ccode, TRUE);
              }
            }
          }
        }

      /* Indicate a non-UTF8 class which was created by negation */

      fprintf(f, "]%s", (*code == OP_NCLASS)? " (neg)" : "");

      /* Handle repeats after a class or a back reference */

      CLASS_REF_REPEAT:
      switch(*ccode)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        fprintf(f, "%s", OP_names[*ccode]);
        extra += _pcre_OP_lengths[*ccode];
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        min = GET2(ccode,1);
        max = GET2(ccode,3);
        if (max == 0) fprintf(f, "{%d,}", min);
        else fprintf(f, "{%d,%d}", min, max);
        if (*ccode == OP_CRMINRANGE) fprintf(f, "?");
        extra += _pcre_OP_lengths[*ccode];
        break;

        /* Do nothing if it's not a repeat; this code stops picky compilers
        warning about the lack of a default code path. */

        default:
        break;
        }
      }
    break;

    case OP_MARK:
    case OP_PRUNE_ARG:
    case OP_SKIP_ARG:
    fprintf(f, "    %s %s", OP_names[*code], code + 2);
    extra += code[1];
    break;

    case OP_THEN:
    if (print_lengths)
      fprintf(f, "    %s %d", OP_names[*code], GET(code, 1));
    else
      fprintf(f, "    %s", OP_names[*code]);
    break;

    case OP_THEN_ARG:
    if (print_lengths)
      fprintf(f, "    %s %d %s", OP_names[*code], GET(code, 1),
        code + 2 + LINK_SIZE);
    else
      fprintf(f, "    %s %s", OP_names[*code], code + 2 + LINK_SIZE);
    extra += code[1+LINK_SIZE];
    break;

    /* Anything else is just an item with no data*/

    default:
    fprintf(f, "    %s", OP_names[*code]);
    break;
    }

  code += _pcre_OP_lengths[*code] + extra;
  fprintf(f, "\n");
  }
}

/* End of pcre_printint.src */
