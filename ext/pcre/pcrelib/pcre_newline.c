/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2012 University of Cambridge

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


/* This module contains internal functions for testing newlines when more than
one kind of newline is to be recognized. When a newline is found, its length is
returned. In principle, we could implement several newline "types", each
referring to a different set of newline characters. At present, PCRE supports
only NLTYPE_FIXED, which gets handled without these functions, NLTYPE_ANYCRLF,
and NLTYPE_ANY. The full list of Unicode newline characters is taken from
http://unicode.org/unicode/reports/tr18/. */


#include "config.h"

#include "pcre_internal.h"



/*************************************************
*      Check for newline at given position       *
*************************************************/

/* It is guaranteed that the initial value of ptr is less than the end of the
string that is being processed.

Arguments:
  ptr          pointer to possible newline
  type         the newline type
  endptr       pointer to the end of the string
  lenptr       where to return the length
  utf          TRUE if in utf mode

Returns:       TRUE or FALSE
*/

BOOL
PRIV(is_newline)(PCRE_PUCHAR ptr, int type, PCRE_PUCHAR endptr, int *lenptr,
  BOOL utf)
{
int c;
(void)utf;
#ifdef SUPPORT_UTF
if (utf)
  {
  GETCHAR(c, ptr);
  }
else
#endif  /* SUPPORT_UTF */
  c = *ptr;

if (type == NLTYPE_ANYCRLF) switch(c)
  {
  case 0x000a: *lenptr = 1; return TRUE;             /* LF */
  case 0x000d: *lenptr = (ptr < endptr - 1 && ptr[1] == 0x0a)? 2 : 1;
               return TRUE;                          /* CR */
  default: return FALSE;
  }

/* NLTYPE_ANY */

else switch(c)
  {
  case 0x000a:                                       /* LF */
  case 0x000b:                                       /* VT */
  case 0x000c: *lenptr = 1; return TRUE;             /* FF */
  case 0x000d: *lenptr = (ptr < endptr - 1 && ptr[1] == 0x0a)? 2 : 1;
               return TRUE;                          /* CR */
#ifdef COMPILE_PCRE8
  case 0x0085: *lenptr = utf? 2 : 1; return TRUE;    /* NEL */
  case 0x2028:                                       /* LS */
  case 0x2029: *lenptr = 3; return TRUE;             /* PS */
#else
  case 0x0085:                                       /* NEL */
  case 0x2028:                                       /* LS */
  case 0x2029: *lenptr = 1; return TRUE;             /* PS */
#endif /* COMPILE_PCRE8 */
  default: return FALSE;
  }
}



/*************************************************
*     Check for newline at previous position     *
*************************************************/

/* It is guaranteed that the initial value of ptr is greater than the start of
the string that is being processed.

Arguments:
  ptr          pointer to possible newline
  type         the newline type
  startptr     pointer to the start of the string
  lenptr       where to return the length
  utf          TRUE if in utf mode

Returns:       TRUE or FALSE
*/

BOOL
PRIV(was_newline)(PCRE_PUCHAR ptr, int type, PCRE_PUCHAR startptr, int *lenptr,
  BOOL utf)
{
int c;
(void)utf;
ptr--;
#ifdef SUPPORT_UTF
if (utf)
  {
  BACKCHAR(ptr);
  GETCHAR(c, ptr);
  }
else
#endif  /* SUPPORT_UTF */
  c = *ptr;

if (type == NLTYPE_ANYCRLF) switch(c)
  {
  case 0x000a: *lenptr = (ptr > startptr && ptr[-1] == 0x0d)? 2 : 1;
               return TRUE;                         /* LF */
  case 0x000d: *lenptr = 1; return TRUE;            /* CR */
  default: return FALSE;
  }

else switch(c)
  {
  case 0x000a: *lenptr = (ptr > startptr && ptr[-1] == 0x0d)? 2 : 1;
               return TRUE;                         /* LF */
  case 0x000b:                                      /* VT */
  case 0x000c:                                      /* FF */
  case 0x000d: *lenptr = 1; return TRUE;            /* CR */
#ifdef COMPILE_PCRE8
  case 0x0085: *lenptr = utf? 2 : 1; return TRUE;   /* NEL */
  case 0x2028:                                      /* LS */
  case 0x2029: *lenptr = 3; return TRUE;            /* PS */
#else
  case 0x0085:                                       /* NEL */
  case 0x2028:                                       /* LS */
  case 0x2029: *lenptr = 1; return TRUE;             /* PS */
#endif /* COMPILE_PCRE8 */
  default: return FALSE;
  }
}

/* End of pcre_newline.c */
