/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-1999 University of Cambridge

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


/* This file is compiled on its own as part of the PCRE library. However,
it is also included in the compilation of dftables.c, in which case the macro
DFTABLES is defined. */

#ifndef DFTABLES
#include "internal.h"
#endif



/*************************************************
*           Create PCRE character tables         *
*************************************************/

/* This function builds a set of character tables for use by PCRE and returns
a pointer to them. They are build using the ctype functions, and consequently
their contents will depend upon the current locale setting. When compiled as
part of the library, the store is obtained via pcre_malloc(), but when compiled
inside dftables, use malloc().

Arguments:   none
Returns:     pointer to the contiguous block of data
*/

unsigned const char *
pcre_maketables(void)
{
unsigned char *yield, *p;
int i;

#ifndef DFTABLES
yield = (unsigned char*)(pcre_malloc)(tables_length);
#else
yield = (unsigned char*)malloc(tables_length);
#endif

if (yield == NULL) return NULL;
p = yield;

/* First comes the lower casing table */

for (i = 0; i < 256; i++) *p++ = tolower(i);

/* Next the case-flipping table */

for (i = 0; i < 256; i++) *p++ = islower(i)? toupper(i) : tolower(i);

/* Then the character class tables */

memset(p, 0, cbit_length);
for (i = 0; i < 256; i++)
  {
  if (isdigit(i)) p[cbit_digit  + i/8] |= 1 << (i&7);
  if (isalnum(i) || i == '_')
                  p[cbit_word   + i/8] |= 1 << (i&7);
  if (isspace(i)) p[cbit_space  + i/8] |= 1 << (i&7);
  }
p += cbit_length;

/* Finally, the character type table */

for (i = 0; i < 256; i++)
  {
  int x = 0;
  if (isspace(i)) x += ctype_space;
  if (isalpha(i)) x += ctype_letter;
  if (isdigit(i)) x += ctype_digit;
  if (isxdigit(i)) x += ctype_xdigit;
  if (isalnum(i) || i == '_') x += ctype_word;
  if (strchr("*+?{^.$|()[", i) != 0) x += ctype_meta;
  *p++ = x;
  }

return yield;
}

/* End of maketables.c */
