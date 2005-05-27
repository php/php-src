/*************************************************
*     libucp - Unicode Property Table handler    *
*************************************************/

/* This function provides a fast way of obtaining the basic Unicode properties
of a character, using a compact binary tree that occupies less than 100K bytes.

           Copyright (c) 2004 University of Cambridge

-------------------------------------------------------------------------------
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
-------------------------------------------------------------------------------
*/


#include "ucp.h"               /* Exported interface */
#include "ucpinternal.h"       /* Internal table details */
#include "ucptable.c"          /* The table itself */



/*************************************************
*         Search table and return data           *
*************************************************/

/* Two values are returned: the category is ucp_C, ucp_L, etc. The detailed
character type is ucp_Lu, ucp_Nd, etc.

Arguments:
  c           the character value
  type_ptr    the detailed character type is returned here
  case_ptr    for letters, the opposite case is returned here, if there
                is one, else zero

Returns:      the character type category or -1 if not found
*/

static int
ucp_findchar(const int c, int *type_ptr, int *case_ptr)
{
cnode *node = ucp_table;
register int cc = c;
int case_offset;

for (;;)
  {
  register int d = node->f1 | ((node->f0 & f0_chhmask) << 16);
  if (cc == d) break;
  if (cc < d)
    {
    if ((node->f0 & f0_leftexists) == 0) return -1;
    node ++;
    }
  else
    {
    register int roffset = (node->f2 & f2_rightmask) >> f2_rightshift;
    if (roffset == 0) return -1;
    node += 1 << (roffset - 1);
    }
  }

switch ((*type_ptr = ((node->f0 & f0_typemask) >> f0_typeshift)))
  {
  case ucp_Cc:
  case ucp_Cf:
  case ucp_Cn:
  case ucp_Co:
  case ucp_Cs:
  return ucp_C;
  break;

  case ucp_Ll:
  case ucp_Lu:
  case_offset = node->f2 & f2_casemask;
  if ((case_offset & 0x0100) != 0) case_offset |= 0xfffff000;
  *case_ptr = (case_offset == 0)? 0 : cc + case_offset;
  return ucp_L;

  case ucp_Lm:
  case ucp_Lo:
  case ucp_Lt:
  *case_ptr = 0;
  return ucp_L;
  break;

  case ucp_Mc:
  case ucp_Me:
  case ucp_Mn:
  return ucp_M;
  break;

  case ucp_Nd:
  case ucp_Nl:
  case ucp_No:
  return ucp_N;
  break;

  case ucp_Pc:
  case ucp_Pd:
  case ucp_Pe:
  case ucp_Pf:
  case ucp_Pi:
  case ucp_Ps:
  case ucp_Po:
  return ucp_P;
  break;

  case ucp_Sc:
  case ucp_Sk:
  case ucp_Sm:
  case ucp_So:
  return ucp_S;
  break;

  case ucp_Zl:
  case ucp_Zp:
  case ucp_Zs:
  return ucp_Z;
  break;

  default:         /* "Should never happen" */
  return -1;
  break;
  }
}

/* End of ucp.c */
