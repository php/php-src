/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2008 University of Cambridge

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


/* This module contains the external function pcre_info(), which gives some
information about a compiled pattern. However, use of this function is now
deprecated, as it has been superseded by pcre_fullinfo(). */


#include "config.h"

#include "pcre_internal.h"


/*************************************************
* (Obsolete) Return info about compiled pattern  *
*************************************************/

/* This is the original "info" function. It picks potentially useful data out
of the private structure, but its interface was too rigid. It remains for
backwards compatibility. The public options are passed back in an int - though
the re->options field has been expanded to a long int, all the public options
at the low end of it, and so even on 16-bit systems this will still be OK.
Therefore, I haven't changed the API for pcre_info().

Arguments:
  argument_re   points to compiled code
  optptr        where to pass back the options
  first_byte    where to pass back the first character,
                or -1 if multiline and all branches start ^,
                or -2 otherwise

Returns:        number of capturing subpatterns
                or negative values on error
*/

PCRE_EXP_DEFN int
pcre_info(const pcre *argument_re, int *optptr, int *first_byte)
{
real_pcre internal_re;
const real_pcre *re = (const real_pcre *)argument_re;
if (re == NULL) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER)
  {
  re = _pcre_try_flipped(re, &internal_re, NULL, NULL);
  if (re == NULL) return PCRE_ERROR_BADMAGIC;
  }
if (optptr != NULL) *optptr = (int)(re->options & PUBLIC_OPTIONS);
if (first_byte != NULL)
  *first_byte = ((re->flags & PCRE_FIRSTSET) != 0)? re->first_byte :
     ((re->flags & PCRE_STARTLINE) != 0)? -1 : -2;
return re->top_bracket;
}

/* End of pcre_info.c */
