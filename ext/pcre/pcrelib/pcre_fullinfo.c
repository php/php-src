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


/* This module contains the external function pcre_fullinfo(), which returns
information about a compiled pattern. */


#include "config.h"

#include "pcre_internal.h"


/*************************************************
*        Return info about compiled pattern      *
*************************************************/

/* This is a newer "info" function which has an extensible interface so
that additional items can be added compatibly.

Arguments:
  argument_re      points to compiled code
  extra_data       points extra data, or NULL
  what             what information is required
  where            where to put the information

Returns:           0 if data returned, negative on error
*/

PCRE_EXP_DEFN int PCRE_CALL_CONVENTION
pcre_fullinfo(const pcre *argument_re, const pcre_extra *extra_data, int what,
  void *where)
{
real_pcre internal_re;
pcre_study_data internal_study;
const real_pcre *re = (const real_pcre *)argument_re;
const pcre_study_data *study = NULL;

if (re == NULL || where == NULL) return PCRE_ERROR_NULL;

if (extra_data != NULL && (extra_data->flags & PCRE_EXTRA_STUDY_DATA) != 0)
  study = (const pcre_study_data *)extra_data->study_data;

if (re->magic_number != MAGIC_NUMBER)
  {
  re = _pcre_try_flipped(re, &internal_re, study, &internal_study);
  if (re == NULL) return PCRE_ERROR_BADMAGIC;
  if (study != NULL) study = &internal_study;
  }

switch (what)
  {
  case PCRE_INFO_OPTIONS:
  *((unsigned long int *)where) = re->options & PUBLIC_OPTIONS;
  break;

  case PCRE_INFO_SIZE:
  *((size_t *)where) = re->size;
  break;

  case PCRE_INFO_STUDYSIZE:
  *((size_t *)where) = (study == NULL)? 0 : study->size;
  break;

  case PCRE_INFO_CAPTURECOUNT:
  *((int *)where) = re->top_bracket;
  break;

  case PCRE_INFO_BACKREFMAX:
  *((int *)where) = re->top_backref;
  break;

  case PCRE_INFO_FIRSTBYTE:
  *((int *)where) =
    ((re->flags & PCRE_FIRSTSET) != 0)? re->first_byte :
    ((re->flags & PCRE_STARTLINE) != 0)? -1 : -2;
  break;

  /* Make sure we pass back the pointer to the bit vector in the external
  block, not the internal copy (with flipped integer fields). */

  case PCRE_INFO_FIRSTTABLE:
  *((const uschar **)where) =
    (study != NULL && (study->options & PCRE_STUDY_MAPPED) != 0)?
      ((const pcre_study_data *)extra_data->study_data)->start_bits : NULL;
  break;

  case PCRE_INFO_LASTLITERAL:
  *((int *)where) =
    ((re->flags & PCRE_REQCHSET) != 0)? re->req_byte : -1;
  break;

  case PCRE_INFO_NAMEENTRYSIZE:
  *((int *)where) = re->name_entry_size;
  break;

  case PCRE_INFO_NAMECOUNT:
  *((int *)where) = re->name_count;
  break;

  case PCRE_INFO_NAMETABLE:
  *((const uschar **)where) = (const uschar *)re + re->name_table_offset;
  break;

  case PCRE_INFO_DEFAULT_TABLES:
  *((const uschar **)where) = (const uschar *)(_pcre_default_tables);
  break;

  case PCRE_INFO_OKPARTIAL:
  *((int *)where) = (re->flags & PCRE_NOPARTIAL) == 0;
  break;

  case PCRE_INFO_JCHANGED:
  *((int *)where) = (re->flags & PCRE_JCHANGED) != 0;
  break;

  case PCRE_INFO_HASCRORLF:
  *((int *)where) = (re->flags & PCRE_HASCRORLF) != 0;
  break;

  default: return PCRE_ERROR_BADOPTION;
  }

return 0;
}

/* End of pcre_fullinfo.c */
