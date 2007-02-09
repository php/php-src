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


/* This module contains some fixed tables that are used by more than one of the
PCRE code modules. The tables are also #included by the pcretest program, which
uses macros to change their names from _pcre_xxx to xxxx, thereby avoiding name
clashes with the library. */


#include "pcre_internal.h"


/* Table of sizes for the fixed-length opcodes. It's defined in a macro so that
the definition is next to the definition of the opcodes in pcre_internal.h. */

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

/* Table of the number of extra bytes, indexed by the first byte masked with
0x3f. The highest number for a valid UTF-8 first byte is in fact 0x3d. */

const uschar _pcre_utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };

/* This table translates Unicode property names into type and code values. It
is searched by binary chop, so must be in collating sequence of name. */

const ucp_type_table _pcre_utt[] = {
  { "Any",                 PT_ANY,  0 },
  { "Arabic",              PT_SC,   ucp_Arabic },
  { "Armenian",            PT_SC,   ucp_Armenian },
  { "Balinese",            PT_SC,   ucp_Balinese },
  { "Bengali",             PT_SC,   ucp_Bengali },
  { "Bopomofo",            PT_SC,   ucp_Bopomofo },
  { "Braille",             PT_SC,   ucp_Braille },
  { "Buginese",            PT_SC,   ucp_Buginese },
  { "Buhid",               PT_SC,   ucp_Buhid },
  { "C",                   PT_GC,   ucp_C },
  { "Canadian_Aboriginal", PT_SC,   ucp_Canadian_Aboriginal },
  { "Cc",                  PT_PC,   ucp_Cc },
  { "Cf",                  PT_PC,   ucp_Cf },
  { "Cherokee",            PT_SC,   ucp_Cherokee },
  { "Cn",                  PT_PC,   ucp_Cn },
  { "Co",                  PT_PC,   ucp_Co },
  { "Common",              PT_SC,   ucp_Common },
  { "Coptic",              PT_SC,   ucp_Coptic },
  { "Cs",                  PT_PC,   ucp_Cs },
  { "Cuneiform",           PT_SC,   ucp_Cuneiform },
  { "Cypriot",             PT_SC,   ucp_Cypriot },
  { "Cyrillic",            PT_SC,   ucp_Cyrillic },
  { "Deseret",             PT_SC,   ucp_Deseret },
  { "Devanagari",          PT_SC,   ucp_Devanagari },
  { "Ethiopic",            PT_SC,   ucp_Ethiopic },
  { "Georgian",            PT_SC,   ucp_Georgian },
  { "Glagolitic",          PT_SC,   ucp_Glagolitic },
  { "Gothic",              PT_SC,   ucp_Gothic },
  { "Greek",               PT_SC,   ucp_Greek },
  { "Gujarati",            PT_SC,   ucp_Gujarati },
  { "Gurmukhi",            PT_SC,   ucp_Gurmukhi },
  { "Han",                 PT_SC,   ucp_Han },
  { "Hangul",              PT_SC,   ucp_Hangul },
  { "Hanunoo",             PT_SC,   ucp_Hanunoo },
  { "Hebrew",              PT_SC,   ucp_Hebrew },
  { "Hiragana",            PT_SC,   ucp_Hiragana },
  { "Inherited",           PT_SC,   ucp_Inherited },
  { "Kannada",             PT_SC,   ucp_Kannada },
  { "Katakana",            PT_SC,   ucp_Katakana },
  { "Kharoshthi",          PT_SC,   ucp_Kharoshthi },
  { "Khmer",               PT_SC,   ucp_Khmer },
  { "L",                   PT_GC,   ucp_L },
  { "L&",                  PT_LAMP, 0 },
  { "Lao",                 PT_SC,   ucp_Lao },
  { "Latin",               PT_SC,   ucp_Latin },
  { "Limbu",               PT_SC,   ucp_Limbu },
  { "Linear_B",            PT_SC,   ucp_Linear_B },
  { "Ll",                  PT_PC,   ucp_Ll },
  { "Lm",                  PT_PC,   ucp_Lm },
  { "Lo",                  PT_PC,   ucp_Lo },
  { "Lt",                  PT_PC,   ucp_Lt },
  { "Lu",                  PT_PC,   ucp_Lu },
  { "M",                   PT_GC,   ucp_M },
  { "Malayalam",           PT_SC,   ucp_Malayalam },
  { "Mc",                  PT_PC,   ucp_Mc },
  { "Me",                  PT_PC,   ucp_Me },
  { "Mn",                  PT_PC,   ucp_Mn },
  { "Mongolian",           PT_SC,   ucp_Mongolian },
  { "Myanmar",             PT_SC,   ucp_Myanmar },
  { "N",                   PT_GC,   ucp_N },
  { "Nd",                  PT_PC,   ucp_Nd },
  { "New_Tai_Lue",         PT_SC,   ucp_New_Tai_Lue },
  { "Nko",                 PT_SC,   ucp_Nko },
  { "Nl",                  PT_PC,   ucp_Nl },
  { "No",                  PT_PC,   ucp_No },
  { "Ogham",               PT_SC,   ucp_Ogham },
  { "Old_Italic",          PT_SC,   ucp_Old_Italic },
  { "Old_Persian",         PT_SC,   ucp_Old_Persian },
  { "Oriya",               PT_SC,   ucp_Oriya },
  { "Osmanya",             PT_SC,   ucp_Osmanya },
  { "P",                   PT_GC,   ucp_P },
  { "Pc",                  PT_PC,   ucp_Pc },
  { "Pd",                  PT_PC,   ucp_Pd },
  { "Pe",                  PT_PC,   ucp_Pe },
  { "Pf",                  PT_PC,   ucp_Pf },
  { "Phags_Pa",            PT_SC,   ucp_Phags_Pa },
  { "Phoenician",          PT_SC,   ucp_Phoenician },
  { "Pi",                  PT_PC,   ucp_Pi },
  { "Po",                  PT_PC,   ucp_Po },
  { "Ps",                  PT_PC,   ucp_Ps },
  { "Runic",               PT_SC,   ucp_Runic },
  { "S",                   PT_GC,   ucp_S },
  { "Sc",                  PT_PC,   ucp_Sc },
  { "Shavian",             PT_SC,   ucp_Shavian },
  { "Sinhala",             PT_SC,   ucp_Sinhala },
  { "Sk",                  PT_PC,   ucp_Sk },
  { "Sm",                  PT_PC,   ucp_Sm },
  { "So",                  PT_PC,   ucp_So },
  { "Syloti_Nagri",        PT_SC,   ucp_Syloti_Nagri },
  { "Syriac",              PT_SC,   ucp_Syriac },
  { "Tagalog",             PT_SC,   ucp_Tagalog },
  { "Tagbanwa",            PT_SC,   ucp_Tagbanwa },
  { "Tai_Le",              PT_SC,   ucp_Tai_Le },
  { "Tamil",               PT_SC,   ucp_Tamil },
  { "Telugu",              PT_SC,   ucp_Telugu },
  { "Thaana",              PT_SC,   ucp_Thaana },
  { "Thai",                PT_SC,   ucp_Thai },
  { "Tibetan",             PT_SC,   ucp_Tibetan },
  { "Tifinagh",            PT_SC,   ucp_Tifinagh },
  { "Ugaritic",            PT_SC,   ucp_Ugaritic },
  { "Yi",                  PT_SC,   ucp_Yi },
  { "Z",                   PT_GC,   ucp_Z },
  { "Zl",                  PT_PC,   ucp_Zl },
  { "Zp",                  PT_PC,   ucp_Zp },
  { "Zs",                  PT_PC,   ucp_Zs }
};

const int _pcre_utt_size = sizeof(_pcre_utt)/sizeof(ucp_type_table);

/* End of pcre_tables.c */
