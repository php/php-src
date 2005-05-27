/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2004 University of Cambridge

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

/* This module contains a table for translating Unicode property names into
code values for the ucp_findchar function. It is in a separate module so that
it can be included both in the main pcre library, and into pcretest (for
printing out internals). */

typedef struct {
  const char *name;
  int value;
} ucp_type_table;

static ucp_type_table utt[] = {
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

/* End of ucptypetable.c */
