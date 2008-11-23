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


/* This module contains an internal function that is used to match an extended
class (one that contains characters whose values are > 255). It is used by both
pcre_exec() and pcre_def_exec(). */


#include "config.h"

#include "pcre_internal.h"


/*************************************************
*       Match character against an XCLASS        *
*************************************************/

/* This function is called to match a character against an extended class that
might contain values > 255.

Arguments:
  c           the character
  data        points to the flag byte of the XCLASS data

Returns:      TRUE if character matches, else FALSE
*/

BOOL
_pcre_xclass(int c, const uschar *data)
{
int t;
BOOL negated = (*data & XCL_NOT) != 0;

/* Character values < 256 are matched against a bitmap, if one is present. If
not, we still carry on, because there may be ranges that start below 256 in the
additional data. */

if (c < 256)
  {
  if ((*data & XCL_MAP) != 0 && (data[1 + c/8] & (1 << (c&7))) != 0)
    return !negated;   /* char found */
  }

/* First skip the bit map if present. Then match against the list of Unicode
properties or large chars or ranges that end with a large char. We won't ever
encounter XCL_PROP or XCL_NOTPROP when UCP support is not compiled. */

if ((*data++ & XCL_MAP) != 0) data += 32;

while ((t = *data++) != XCL_END)
  {
  int x, y;
  if (t == XCL_SINGLE)
    {
    GETCHARINC(x, data);
    if (c == x) return !negated;
    }
  else if (t == XCL_RANGE)
    {
    GETCHARINC(x, data);
    GETCHARINC(y, data);
    if (c >= x && c <= y) return !negated;
    }

#ifdef SUPPORT_UCP
  else  /* XCL_PROP & XCL_NOTPROP */
    {
    const ucd_record * prop = GET_UCD(c);

    switch(*data)
      {
      case PT_ANY:
      if (t == XCL_PROP) return !negated;
      break;

      case PT_LAMP:
      if ((prop->chartype == ucp_Lu || prop->chartype == ucp_Ll || prop->chartype == ucp_Lt) ==
          (t == XCL_PROP)) return !negated;
      break;

      case PT_GC:
      if ((data[1] == _pcre_ucp_gentype[prop->chartype]) == (t == XCL_PROP)) return !negated;
      break;

      case PT_PC:
      if ((data[1] == prop->chartype) == (t == XCL_PROP)) return !negated;
      break;

      case PT_SC:
      if ((data[1] == prop->script) == (t == XCL_PROP)) return !negated;
      break;

      /* This should never occur, but compilers may mutter if there is no
      default. */

      default:
      return FALSE;
      }

    data += 2;
    }
#endif  /* SUPPORT_UCP */
  }

return negated;   /* char did not match */
}

/* End of pcre_xclass.c */
