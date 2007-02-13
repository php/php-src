/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2006 University of Cambridge

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


/* This module contains some convenience functions for extracting substrings
from the subject string after a regex match has succeeded. The original idea
for these functions came from Scott Wimer. */


#include "pcre_internal.h"


/*************************************************
*           Find number for named string         *
*************************************************/

/* This function is used by the get_first_set() function below, as well
as being generally available. It assumes that names are unique.

Arguments:
  code        the compiled regex
  stringname  the name whose number is required

Returns:      the number of the named parentheses, or a negative number
                (PCRE_ERROR_NOSUBSTRING) if not found
*/

int
pcre_get_stringnumber(const pcre *code, const char *stringname)
{
int rc;
int entrysize;
int top, bot;
uschar *nametable;

if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMECOUNT, &top)) != 0)
  return rc;
if (top <= 0) return PCRE_ERROR_NOSUBSTRING;

if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMEENTRYSIZE, &entrysize)) != 0)
  return rc;
if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMETABLE, &nametable)) != 0)
  return rc;

bot = 0;
while (top > bot)
  {
  int mid = (top + bot) / 2;
  uschar *entry = nametable + entrysize*mid;
  int c = strcmp(stringname, (char *)(entry + 2));
  if (c == 0) return (entry[0] << 8) + entry[1];
  if (c > 0) bot = mid + 1; else top = mid;
  }

return PCRE_ERROR_NOSUBSTRING;
}



/*************************************************
*     Find (multiple) entries for named string   *
*************************************************/

/* This is used by the get_first_set() function below, as well as being
generally available. It is used when duplicated names are permitted.

Arguments:
  code        the compiled regex
  stringname  the name whose entries required
  firstptr    where to put the pointer to the first entry
  lastptr     where to put the pointer to the last entry

Returns:      the length of each entry, or a negative number
                (PCRE_ERROR_NOSUBSTRING) if not found
*/

int
pcre_get_stringtable_entries(const pcre *code, const char *stringname,
  char **firstptr, char **lastptr)
{
int rc;
int entrysize;
int top, bot;
uschar *nametable, *lastentry;

if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMECOUNT, &top)) != 0)
  return rc;
if (top <= 0) return PCRE_ERROR_NOSUBSTRING;

if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMEENTRYSIZE, &entrysize)) != 0)
  return rc;
if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMETABLE, &nametable)) != 0)
  return rc;

lastentry = nametable + entrysize * (top - 1);
bot = 0;
while (top > bot)
  {
  int mid = (top + bot) / 2;
  uschar *entry = nametable + entrysize*mid;
  int c = strcmp(stringname, (char *)(entry + 2));
  if (c == 0)
    {
    uschar *first = entry;
    uschar *last = entry;
    while (first > nametable)
      {
      if (strcmp(stringname, (char *)(first - entrysize + 2)) != 0) break;
      first -= entrysize;
      }
    while (last < lastentry)
      {
      if (strcmp(stringname, (char *)(last + entrysize + 2)) != 0) break;
      last += entrysize;
      }
    *firstptr = (char *)first;
    *lastptr = (char *)last;
    return entrysize;
    }
  if (c > 0) bot = mid + 1; else top = mid;
  }

return PCRE_ERROR_NOSUBSTRING;
}



/*************************************************
*    Find first set of multiple named strings    *
*************************************************/

/* This function allows for duplicate names in the table of named substrings.
It returns the number of the first one that was set in a pattern match.

Arguments:
  code         the compiled regex
  stringname   the name of the capturing substring
  ovector      the vector of matched substrings

Returns:       the number of the first that is set,
               or the number of the last one if none are set,
               or a negative number on error
*/

static int
get_first_set(const pcre *code, const char *stringname, int *ovector)
{
const real_pcre *re = (const real_pcre *)code;
int entrysize;
char *first, *last;
uschar *entry;
if ((re->options & (PCRE_DUPNAMES | PCRE_JCHANGED)) == 0)
  return pcre_get_stringnumber(code, stringname);
entrysize = pcre_get_stringtable_entries(code, stringname, &first, &last);
if (entrysize <= 0) return entrysize;
for (entry = (uschar *)first; entry <= (uschar *)last; entry += entrysize)
  {
  int n = (entry[0] << 8) + entry[1];
  if (ovector[n*2] >= 0) return n;
  }
return (first[0] << 8) + first[1];
}




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
*   Copy named captured string to given buffer   *
*************************************************/

/* This function copies a single captured substring into a given buffer,
identifying it by name. If the regex permits duplicate names, the first
substring that is set is chosen.

Arguments:
  code           the compiled regex
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringname     the name of the required substring
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
pcre_copy_named_substring(const pcre *code, const char *subject, int *ovector,
  int stringcount, const char *stringname, char *buffer, int size)
{
int n = get_first_set(code, stringname, ovector);
if (n <= 0) return n;
return pcre_copy_substring(subject, ovector, stringcount, n, buffer, size);
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
*   Copy named captured string to new store      *
*************************************************/

/* This function copies a single captured substring, identified by name, into
new store. If the regex permits duplicate names, the first substring that is
set is chosen.

Arguments:
  code           the compiled regex
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringname     the name of the required substring
  stringptr      where to put the pointer

Returns:         if successful:
                   the length of the copied string, not including the zero
                   that is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) couldn't get memory
                   PCRE_ERROR_NOSUBSTRING (-7) no such captured substring
*/

int
pcre_get_named_substring(const pcre *code, const char *subject, int *ovector,
  int stringcount, const char *stringname, const char **stringptr)
{
int n = get_first_set(code, stringname, ovector);
if (n <= 0) return n;
return pcre_get_substring(subject, ovector, stringcount, n, stringptr);
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

/* End of pcre_get.c */
