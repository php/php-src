/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2005 University of Cambridge

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


/* This module contains some fixed tables that are used by more than one of the
PCRE code modules. */


#include "pcre_internal.h"


/* Table of sizes for the fixed-length opcodes. It's defined in a macro so that
the definition is next to the definition of the opcodes in internal.h. */

const uschar _pcre_OP_lengths[] = { OP_LENGTHS };



/*************************************************
*           Tables for UTF-8 support             *
*************************************************/

/* These are the breakpoints for different numbers of bytes in a UTF-8
character. */

const int _pcre_utf8_table1[] =
  { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};

const int _pcre_utf8_table1_size = sizeof(_pcre_utf8_table1)/sizeof(int);

/* These are the indicator bits and the mask for the data bits to set in the
first byte of a character, indexed by the number of additional bytes. */

const int _pcre_utf8_table2[] = { 0,    0xc0, 0xe0, 0xf0, 0xf8, 0xfc};
const int _pcre_utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

/* Table of the number of extra characters, indexed by the first character
masked with 0x3f. The highest number for a valid UTF-8 character is in fact
0x3d. */

const uschar _pcre_utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };

/* This table translates Unicode property names into code values for the
ucp_findchar() function. It is used by pcretest as well as by the library
functions. */

const ucp_type_table _pcre_utt[] = {
  { "C",  128 + ucp_C },
  { "Cc", ucp_Cc },
  { "Cf", ucp_Cf },
  { "Cn", ucp_Cn },
  { "Co", ucp_Co },
  { "Cs", ucp_Cs },
  { "L",  128 + ucp_L },
  { "Ll", ucp_Ll },
  { "Lm", ucp_Lm },
  { "Lo", ucp_Lo },
  { "Lt", ucp_Lt },
  { "Lu", ucp_Lu },
  { "M",  128 + ucp_M },
  { "Mc", ucp_Mc },
  { "Me", ucp_Me },
  { "Mn", ucp_Mn },
  { "N",  128 + ucp_N },
  { "Nd", ucp_Nd },
  { "Nl", ucp_Nl },
  { "No", ucp_No },
  { "P",  128 + ucp_P },
  { "Pc", ucp_Pc },
  { "Pd", ucp_Pd },
  { "Pe", ucp_Pe },
  { "Pf", ucp_Pf },
  { "Pi", ucp_Pi },
  { "Po", ucp_Po },
  { "Ps", ucp_Ps },
  { "S",  128 + ucp_S },
  { "Sc", ucp_Sc },
  { "Sk", ucp_Sk },
  { "Sm", ucp_Sm },
  { "So", ucp_So },
  { "Z",  128 + ucp_Z },
  { "Zl", ucp_Zl },
  { "Zp", ucp_Zp },
  { "Zs", ucp_Zs }
};

const int _pcre_utt_size = sizeof(_pcre_utt)/sizeof(ucp_type_table);

/* End of pcre_tables.c */
