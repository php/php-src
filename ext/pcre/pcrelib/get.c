/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2001 University of Cambridge

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
*/

/* This module contains some convenience functions for extracting substrings
from the subject string after a regex match has succeeded. The original idea
for these functions came from Scott Wimer <scottw@cgibuilder.com>. */


/* Include the internals header, which itself includes Standard C headers plus
the external pcre header. */

#include "internal.h"



/*************************************************
*      Copy captured string to given buffer      *
*************************************************/

/* This function copies a single captured substring into a given buffer.
Note that we use memcpy() rather than strncpy() in case there are binary zeros
in the string.

Arguments:
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringnumber   the number of the required substring
  buffer         where to put the substring
  size           the size of the buffer

Returns:         if successful:
                   the length of the copied string, not including the zero
                   that is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) buffer too small
                   PCRE_ERROR_NOSUBSTRING (-7) no such captured substring
*/

int
pcre_copy_substring(const char *subject, int *ovector, int stringcount,
  int stringnumber, char *buffer, int size)
{
int yield;
if (stringnumber < 0 || stringnumber >= stringcount)
  return PCRE_ERROR_NOSUBSTRING;
stringnumber *= 2;
yield = ovector[stringnumber+1] - ovector[stringnumber];
if (size < yield + 1) return PCRE_ERROR_NOMEMORY;
memcpy(buffer, subject + ovector[stringnumber], yield);
buffer[yield] = 0;
return yield;
}



/*************************************************
*      Copy all captured strings to new store    *
*************************************************/

/* This function gets one chunk of store and builds a list of pointers and all
of the captured substrings in it. A NULL pointer is put on the end of the list.

Arguments:
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  listptr        set to point to the list of pointers

Returns:         if successful: 0
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) failed to get store
*/

int
pcre_get_substring_list(const char *subject, int *ovector, int stringcount,
  const char ***listptr)
{
int i;
int size = sizeof(char *);
int double_count = stringcount * 2;
char **stringlist;
char *p;

for (i = 0; i < double_count; i += 2)
  size += sizeof(char *) + ovector[i+1] - ovector[i] + 1;

stringlist = (char **)(pcre_malloc)(size);
if (stringlist == NULL) return PCRE_ERROR_NOMEMORY;

*listptr = (const char **)stringlist;
p = (char *)(stringlist + stringcount + 1);

for (i = 0; i < double_count; i += 2)
  {
  int len = ovector[i+1] - ovector[i];
  memcpy(p, subject + ovector[i], len);
  *stringlist++ = p;
  p += len;
  *p++ = 0;
  }

*stringlist = NULL;
return 0;
}



/*************************************************
*   Free store obtained by get_substring_list    *
*************************************************/

/* This function exists for the benefit of people calling PCRE from non-C
programs that can call its functions, but not free() or (pcre_free)() directly.

Argument:   the result of a previous pcre_get_substring_list()
Returns:    nothing
*/

void
pcre_free_substring_list(const char **pointer)
{
(pcre_free)((void *)pointer);
}



/*************************************************
*      Copy captured string to new store         *
*************************************************/

/* This function copies a single captured substring into a piece of new
store

Arguments:
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringnumber   the number of the required substring
  stringptr      where to put a pointer to the substring

Returns:         if successful:
                   the length of the string, not including the zero that
                   is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) failed to get store
                   PCRE_ERROR_NOSUBSTRING (-7) substring not present
*/

int
pcre_get_substring(const char *subject, int *ovector, int stringcount,
  int stringnumber, const char **stringptr)
{
int yield;
char *substring;
if (stringnumber < 0 || stringnumber >= stringcount)
  return PCRE_ERROR_NOSUBSTRING;
stringnumber *= 2;
yield = ovector[stringnumber+1] - ovector[stringnumber];
substring = (char *)(pcre_malloc)(yield + 1);
if (substring == NULL) return PCRE_ERROR_NOMEMORY;
memcpy(substring, subject + ovector[stringnumber], yield);
substring[yield] = 0;
*stringptr = substring;
return yield;
}



/*************************************************
*       Free store obtained by get_substring     *
*************************************************/

/* This function exists for the benefit of people calling PCRE from non-C
programs that can call its functions, but not free() or (pcre_free)() directly.

Argument:   the result of a previous pcre_get_substring()
Returns:    nothing
*/

void
pcre_free_substring(const char *pointer)
{
(pcre_free)((void *)pointer);
}

/* End of get.c */
