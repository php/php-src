/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2003 University of Cambridge

-----------------------------------------------------------------------------
Permission is granted to anyone to use this software for any purpose on any
computer system, and to redistribute it freely, subject to the following
restrictions:

1. This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

4. If PCRE is embedded in any software that is released under the GNU
   General Purpose Licence (GPL), then the terms of that licence shall
   supersede any condition above with which it is incompatible.
-----------------------------------------------------------------------------

See the file Tech.Notes for some information on the internals.
*/


/* This is a support program to generate the file chartables.c, containing
character tables of various kinds. They are built according to the default C
locale and used as the default tables by PCRE. Now that pcre_maketables is
a function visible to the outside world, we make use of its code from here in
order to be consistent. */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "internal.h"

#define DFTABLES          /* maketables.c notices this */
#include "maketables.c"


int main(int argc, char **argv)
{
int i;
FILE *f;
const unsigned char *tables = pcre_maketables();

if (argc != 2)
  {
  fprintf(stderr, "dftables: one filename argument is required\n");
  return 1;
  }

f = fopen(argv[1], "w");
if (f == NULL)
  {
  fprintf(stderr, "dftables: failed to open %s for writing\n", argv[1]);
  return 1;
  }

/* There are two fprintf() calls here, because gcc in pedantic mode complains
about the very long string otherwise. */

fprintf(f,
  "/*************************************************\n"
  "*      Perl-Compatible Regular Expressions       *\n"
  "*************************************************/\n\n"
  "/* This file is automatically written by the dftables auxiliary \n"
  "program. If you edit it by hand, you might like to edit the Makefile to \n"
  "prevent its ever being regenerated.\n\n");
fprintf(f,
  "This file is #included in the compilation of pcre.c to build the default\n"
  "character tables which are used when no tables are passed to the compile\n"
  "function. */\n\n"
  "static unsigned char pcre_default_tables[] = {\n\n"
  "/* This table is a lower casing table. */\n\n");

fprintf(f, "  ");
for (i = 0; i < 256; i++)
  {
  if ((i & 7) == 0 && i != 0) fprintf(f, "\n  ");
  fprintf(f, "%3d", *tables++);
  if (i != 255) fprintf(f, ",");
  }
fprintf(f, ",\n\n");

fprintf(f, "/* This table is a case flipping table. */\n\n");

fprintf(f, "  ");
for (i = 0; i < 256; i++)
  {
  if ((i & 7) == 0 && i != 0) fprintf(f, "\n  ");
  fprintf(f, "%3d", *tables++);
  if (i != 255) fprintf(f, ",");
  }
fprintf(f, ",\n\n");

fprintf(f,
  "/* This table contains bit maps for various character classes.\n"
  "Each map is 32 bytes long and the bits run from the least\n"
  "significant end of each byte. The classes that have their own\n"
  "maps are: space, xdigit, digit, upper, lower, word, graph\n"
  "print, punct, and cntrl. Other classes are built from combinations. */\n\n");

fprintf(f, "  ");
for (i = 0; i < cbit_length; i++)
  {
  if ((i & 7) == 0 && i != 0)
    {
    if ((i & 31) == 0) fprintf(f, "\n");
    fprintf(f, "\n  ");
    }
  fprintf(f, "0x%02x", *tables++);
  if (i != cbit_length - 1) fprintf(f, ",");
  }
fprintf(f, ",\n\n");

fprintf(f,
  "/* This table identifies various classes of character by individual bits:\n"
  "  0x%02x   white space character\n"
  "  0x%02x   letter\n"
  "  0x%02x   decimal digit\n"
  "  0x%02x   hexadecimal digit\n"
  "  0x%02x   alphanumeric or '_'\n"
  "  0x%02x   regular expression metacharacter or binary zero\n*/\n\n",
  ctype_space, ctype_letter, ctype_digit, ctype_xdigit, ctype_word,
  ctype_meta);

fprintf(f, "  ");
for (i = 0; i < 256; i++)
  {
  if ((i & 7) == 0 && i != 0)
    {
    fprintf(f, " /* ");
    if (isprint(i-8)) fprintf(f, " %c -", i-8);
      else fprintf(f, "%3d-", i-8);
    if (isprint(i-1)) fprintf(f, " %c ", i-1);
      else fprintf(f, "%3d", i-1);
    fprintf(f, " */\n  ");
    }
  fprintf(f, "0x%02x", *tables++);
  if (i != 255) fprintf(f, ",");
  }

fprintf(f, "};/* ");
if (isprint(i-8)) fprintf(f, " %c -", i-8);
  else fprintf(f, "%3d-", i-8);
if (isprint(i-1)) fprintf(f, " %c ", i-1);
  else fprintf(f, "%3d", i-1);
fprintf(f, " */\n\n/* End of chartables.c */\n");

fclose(f);
return 0;
}

/* End of dftables.c */
