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


/* This module contains some fixed tables that are used by more than one of the
PCRE code modules. The tables are also #included by the pcretest program, which
uses macros to change their names from _pcre_xxx to xxxx, thereby avoiding name
clashes with the library. */


#include "config.h"

#include "pcre_internal.h"


/* Table of sizes for the fixed-length opcodes. It's defined in a macro so that
the definition is next to the definition of the opcodes in pcre_internal.h. */

const uschar _pcre_OP_lengths[] = { OP_LENGTHS };



/*************************************************
*           Tables for UTF-8 support             *
*************************************************/

/* These are the breakpoints for different numbers of bytes in a UTF-8
character. */

#ifdef SUPPORT_UTF8

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

/* The pcre_utt[] table below translates Unicode property names into type and
code values. It is searched by binary chop, so must be in collating sequence of
name. Originally, the table contained pointers to the name strings in the first
field of each entry. However, that leads to a large number of relocations when
a shared library is dynamically loaded. A significant reduction is made by
putting all the names into a single, large string and then using offsets in the
table itself. Maintenance is more error-prone, but frequent changes to this
data is unlikely. */

const char _pcre_utt_names[] =
  "Any\0"
  "Arabic\0"
  "Armenian\0"
  "Balinese\0"
  "Bengali\0"
  "Bopomofo\0"
  "Braille\0"
  "Buginese\0"
  "Buhid\0"
  "C\0"
  "Canadian_Aboriginal\0"
  "Cc\0"
  "Cf\0"
  "Cherokee\0"
  "Cn\0"
  "Co\0"
  "Common\0"
  "Coptic\0"
  "Cs\0"
  "Cuneiform\0"
  "Cypriot\0"
  "Cyrillic\0"
  "Deseret\0"
  "Devanagari\0"
  "Ethiopic\0"
  "Georgian\0"
  "Glagolitic\0"
  "Gothic\0"
  "Greek\0"
  "Gujarati\0"
  "Gurmukhi\0"
  "Han\0"
  "Hangul\0"
  "Hanunoo\0"
  "Hebrew\0"
  "Hiragana\0"
  "Inherited\0"
  "Kannada\0"
  "Katakana\0"
  "Kharoshthi\0"
  "Khmer\0"
  "L\0"
  "L&\0"
  "Lao\0"
  "Latin\0"
  "Limbu\0"
  "Linear_B\0"
  "Ll\0"
  "Lm\0"
  "Lo\0"
  "Lt\0"
  "Lu\0"
  "M\0"
  "Malayalam\0"
  "Mc\0"
  "Me\0"
  "Mn\0"
  "Mongolian\0"
  "Myanmar\0"
  "N\0"
  "Nd\0"
  "New_Tai_Lue\0"
  "Nko\0"
  "Nl\0"
  "No\0"
  "Ogham\0"
  "Old_Italic\0"
  "Old_Persian\0"
  "Oriya\0"
  "Osmanya\0"
  "P\0"
  "Pc\0"
  "Pd\0"
  "Pe\0"
  "Pf\0"
  "Phags_Pa\0"
  "Phoenician\0"
  "Pi\0"
  "Po\0"
  "Ps\0"
  "Runic\0"
  "S\0"
  "Sc\0"
  "Shavian\0"
  "Sinhala\0"
  "Sk\0"
  "Sm\0"
  "So\0"
  "Syloti_Nagri\0"
  "Syriac\0"
  "Tagalog\0"
  "Tagbanwa\0"
  "Tai_Le\0"
  "Tamil\0"
  "Telugu\0"
  "Thaana\0"
  "Thai\0"
  "Tibetan\0"
  "Tifinagh\0"
  "Ugaritic\0"
  "Yi\0"
  "Z\0"
  "Zl\0"
  "Zp\0"
  "Zs\0";

const ucp_type_table _pcre_utt[] = {
  { 0,   PT_ANY, 0 },
  { 4,   PT_SC, ucp_Arabic },
  { 11,  PT_SC, ucp_Armenian },
  { 20,  PT_SC, ucp_Balinese },
  { 29,  PT_SC, ucp_Bengali },
  { 37,  PT_SC, ucp_Bopomofo },
  { 46,  PT_SC, ucp_Braille },
  { 54,  PT_SC, ucp_Buginese },
  { 63,  PT_SC, ucp_Buhid },
  { 69,  PT_GC, ucp_C },
  { 71,  PT_SC, ucp_Canadian_Aboriginal },
  { 91,  PT_PC, ucp_Cc },
  { 94,  PT_PC, ucp_Cf },
  { 97,  PT_SC, ucp_Cherokee },
  { 106, PT_PC, ucp_Cn },
  { 109, PT_PC, ucp_Co },
  { 112, PT_SC, ucp_Common },
  { 119, PT_SC, ucp_Coptic },
  { 126, PT_PC, ucp_Cs },
  { 129, PT_SC, ucp_Cuneiform },
  { 139, PT_SC, ucp_Cypriot },
  { 147, PT_SC, ucp_Cyrillic },
  { 156, PT_SC, ucp_Deseret },
  { 164, PT_SC, ucp_Devanagari },
  { 175, PT_SC, ucp_Ethiopic },
  { 184, PT_SC, ucp_Georgian },
  { 193, PT_SC, ucp_Glagolitic },
  { 204, PT_SC, ucp_Gothic },
  { 211, PT_SC, ucp_Greek },
  { 217, PT_SC, ucp_Gujarati },
  { 226, PT_SC, ucp_Gurmukhi },
  { 235, PT_SC, ucp_Han },
  { 239, PT_SC, ucp_Hangul },
  { 246, PT_SC, ucp_Hanunoo },
  { 254, PT_SC, ucp_Hebrew },
  { 261, PT_SC, ucp_Hiragana },
  { 270, PT_SC, ucp_Inherited },
  { 280, PT_SC, ucp_Kannada },
  { 288, PT_SC, ucp_Katakana },
  { 297, PT_SC, ucp_Kharoshthi },
  { 308, PT_SC, ucp_Khmer },
  { 314, PT_GC, ucp_L },
  { 316, PT_LAMP, 0 },
  { 319, PT_SC, ucp_Lao },
  { 323, PT_SC, ucp_Latin },
  { 329, PT_SC, ucp_Limbu },
  { 335, PT_SC, ucp_Linear_B },
  { 344, PT_PC, ucp_Ll },
  { 347, PT_PC, ucp_Lm },
  { 350, PT_PC, ucp_Lo },
  { 353, PT_PC, ucp_Lt },
  { 356, PT_PC, ucp_Lu },
  { 359, PT_GC, ucp_M },
  { 361, PT_SC, ucp_Malayalam },
  { 371, PT_PC, ucp_Mc },
  { 374, PT_PC, ucp_Me },
  { 377, PT_PC, ucp_Mn },
  { 380, PT_SC, ucp_Mongolian },
  { 390, PT_SC, ucp_Myanmar },
  { 398, PT_GC, ucp_N },
  { 400, PT_PC, ucp_Nd },
  { 403, PT_SC, ucp_New_Tai_Lue },
  { 415, PT_SC, ucp_Nko },
  { 419, PT_PC, ucp_Nl },
  { 422, PT_PC, ucp_No },
  { 425, PT_SC, ucp_Ogham },
  { 431, PT_SC, ucp_Old_Italic },
  { 442, PT_SC, ucp_Old_Persian },
  { 454, PT_SC, ucp_Oriya },
  { 460, PT_SC, ucp_Osmanya },
  { 468, PT_GC, ucp_P },
  { 470, PT_PC, ucp_Pc },
  { 473, PT_PC, ucp_Pd },
  { 476, PT_PC, ucp_Pe },
  { 479, PT_PC, ucp_Pf },
  { 482, PT_SC, ucp_Phags_Pa },
  { 491, PT_SC, ucp_Phoenician },
  { 502, PT_PC, ucp_Pi },
  { 505, PT_PC, ucp_Po },
  { 508, PT_PC, ucp_Ps },
  { 511, PT_SC, ucp_Runic },
  { 517, PT_GC, ucp_S },
  { 519, PT_PC, ucp_Sc },
  { 522, PT_SC, ucp_Shavian },
  { 530, PT_SC, ucp_Sinhala },
  { 538, PT_PC, ucp_Sk },
  { 541, PT_PC, ucp_Sm },
  { 544, PT_PC, ucp_So },
  { 547, PT_SC, ucp_Syloti_Nagri },
  { 560, PT_SC, ucp_Syriac },
  { 567, PT_SC, ucp_Tagalog },
  { 575, PT_SC, ucp_Tagbanwa },
  { 584, PT_SC, ucp_Tai_Le },
  { 591, PT_SC, ucp_Tamil },
  { 597, PT_SC, ucp_Telugu },
  { 604, PT_SC, ucp_Thaana },
  { 611, PT_SC, ucp_Thai },
  { 616, PT_SC, ucp_Tibetan },
  { 624, PT_SC, ucp_Tifinagh },
  { 633, PT_SC, ucp_Ugaritic },
  { 642, PT_SC, ucp_Yi },
  { 645, PT_GC, ucp_Z },
  { 647, PT_PC, ucp_Zl },
  { 650, PT_PC, ucp_Zp },
  { 653, PT_PC, ucp_Zs }
};

const int _pcre_utt_size = sizeof(_pcre_utt)/sizeof(ucp_type_table);

#endif  /* SUPPORT_UTF8 */

/* End of pcre_tables.c */
