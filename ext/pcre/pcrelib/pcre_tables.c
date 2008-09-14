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

/* Table to translate from particular type value to the general value. */

const int _pcre_ucp_gentype[] = {
  ucp_C, ucp_C, ucp_C, ucp_C, ucp_C,  /* Cc, Cf, Cn, Co, Cs */
  ucp_L, ucp_L, ucp_L, ucp_L, ucp_L,  /* Ll, Lu, Lm, Lo, Lt */
  ucp_M, ucp_M, ucp_M,                /* Mc, Me, Mn */
  ucp_N, ucp_N, ucp_N,                /* Nd, Nl, No */
  ucp_P, ucp_P, ucp_P, ucp_P, ucp_P,  /* Pc, Pd, Pe, Pf, Pi */
  ucp_P, ucp_P,                       /* Ps, Po */
  ucp_S, ucp_S, ucp_S, ucp_S,         /* Sc, Sk, Sm, So */
  ucp_Z, ucp_Z, ucp_Z                 /* Zl, Zp, Zs */
};

/* The pcre_utt[] table below translates Unicode property names into type and
code values. It is searched by binary chop, so must be in collating sequence of
name. Originally, the table contained pointers to the name strings in the first
field of each entry. However, that leads to a large number of relocations when
a shared library is dynamically loaded. A significant reduction is made by
putting all the names into a single, large string and then using offsets in the
table itself. Maintenance is more error-prone, but frequent changes to this
data are unlikely.

July 2008: There is now a script called maint/GenerateUtt.py which can be used
to generate this data instead of maintaining it entirely by hand. */

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
  "Carian\0"
  "Cc\0"
  "Cf\0"
  "Cham\0"
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
  "Kayah_Li\0"
  "Kharoshthi\0"
  "Khmer\0"
  "L\0"
  "L&\0"
  "Lao\0"
  "Latin\0"
  "Lepcha\0"
  "Limbu\0"
  "Linear_B\0"
  "Ll\0"
  "Lm\0"
  "Lo\0"
  "Lt\0"
  "Lu\0"
  "Lycian\0"
  "Lydian\0"
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
  "Ol_Chiki\0"
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
  "Rejang\0"
  "Runic\0"
  "S\0"
  "Saurashtra\0"
  "Sc\0"
  "Shavian\0"
  "Sinhala\0"
  "Sk\0"
  "Sm\0"
  "So\0"
  "Sundanese\0"
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
  "Vai\0"
  "Yi\0"
  "Z\0"
  "Zl\0"
  "Zp\0"
  "Zs\0";

const ucp_type_table _pcre_utt[] = {
  {   0, PT_ANY, 0 },
  {   4, PT_SC, ucp_Arabic },
  {  11, PT_SC, ucp_Armenian },
  {  20, PT_SC, ucp_Balinese },
  {  29, PT_SC, ucp_Bengali },
  {  37, PT_SC, ucp_Bopomofo },
  {  46, PT_SC, ucp_Braille },
  {  54, PT_SC, ucp_Buginese },
  {  63, PT_SC, ucp_Buhid },
  {  69, PT_GC, ucp_C },
  {  71, PT_SC, ucp_Canadian_Aboriginal },
  {  91, PT_SC, ucp_Carian },
  {  98, PT_PC, ucp_Cc },
  { 101, PT_PC, ucp_Cf },
  { 104, PT_SC, ucp_Cham },
  { 109, PT_SC, ucp_Cherokee },
  { 118, PT_PC, ucp_Cn },
  { 121, PT_PC, ucp_Co },
  { 124, PT_SC, ucp_Common },
  { 131, PT_SC, ucp_Coptic },
  { 138, PT_PC, ucp_Cs },
  { 141, PT_SC, ucp_Cuneiform },
  { 151, PT_SC, ucp_Cypriot },
  { 159, PT_SC, ucp_Cyrillic },
  { 168, PT_SC, ucp_Deseret },
  { 176, PT_SC, ucp_Devanagari },
  { 187, PT_SC, ucp_Ethiopic },
  { 196, PT_SC, ucp_Georgian },
  { 205, PT_SC, ucp_Glagolitic },
  { 216, PT_SC, ucp_Gothic },
  { 223, PT_SC, ucp_Greek },
  { 229, PT_SC, ucp_Gujarati },
  { 238, PT_SC, ucp_Gurmukhi },
  { 247, PT_SC, ucp_Han },
  { 251, PT_SC, ucp_Hangul },
  { 258, PT_SC, ucp_Hanunoo },
  { 266, PT_SC, ucp_Hebrew },
  { 273, PT_SC, ucp_Hiragana },
  { 282, PT_SC, ucp_Inherited },
  { 292, PT_SC, ucp_Kannada },
  { 300, PT_SC, ucp_Katakana },
  { 309, PT_SC, ucp_Kayah_Li },
  { 318, PT_SC, ucp_Kharoshthi },
  { 329, PT_SC, ucp_Khmer },
  { 335, PT_GC, ucp_L },
  { 337, PT_LAMP, 0 },
  { 340, PT_SC, ucp_Lao },
  { 344, PT_SC, ucp_Latin },
  { 350, PT_SC, ucp_Lepcha },
  { 357, PT_SC, ucp_Limbu },
  { 363, PT_SC, ucp_Linear_B },
  { 372, PT_PC, ucp_Ll },
  { 375, PT_PC, ucp_Lm },
  { 378, PT_PC, ucp_Lo },
  { 381, PT_PC, ucp_Lt },
  { 384, PT_PC, ucp_Lu },
  { 387, PT_SC, ucp_Lycian },
  { 394, PT_SC, ucp_Lydian },
  { 401, PT_GC, ucp_M },
  { 403, PT_SC, ucp_Malayalam },
  { 413, PT_PC, ucp_Mc },
  { 416, PT_PC, ucp_Me },
  { 419, PT_PC, ucp_Mn },
  { 422, PT_SC, ucp_Mongolian },
  { 432, PT_SC, ucp_Myanmar },
  { 440, PT_GC, ucp_N },
  { 442, PT_PC, ucp_Nd },
  { 445, PT_SC, ucp_New_Tai_Lue },
  { 457, PT_SC, ucp_Nko },
  { 461, PT_PC, ucp_Nl },
  { 464, PT_PC, ucp_No },
  { 467, PT_SC, ucp_Ogham },
  { 473, PT_SC, ucp_Ol_Chiki },
  { 482, PT_SC, ucp_Old_Italic },
  { 493, PT_SC, ucp_Old_Persian },
  { 505, PT_SC, ucp_Oriya },
  { 511, PT_SC, ucp_Osmanya },
  { 519, PT_GC, ucp_P },
  { 521, PT_PC, ucp_Pc },
  { 524, PT_PC, ucp_Pd },
  { 527, PT_PC, ucp_Pe },
  { 530, PT_PC, ucp_Pf },
  { 533, PT_SC, ucp_Phags_Pa },
  { 542, PT_SC, ucp_Phoenician },
  { 553, PT_PC, ucp_Pi },
  { 556, PT_PC, ucp_Po },
  { 559, PT_PC, ucp_Ps },
  { 562, PT_SC, ucp_Rejang },
  { 569, PT_SC, ucp_Runic },
  { 575, PT_GC, ucp_S },
  { 577, PT_SC, ucp_Saurashtra },
  { 588, PT_PC, ucp_Sc },
  { 591, PT_SC, ucp_Shavian },
  { 599, PT_SC, ucp_Sinhala },
  { 607, PT_PC, ucp_Sk },
  { 610, PT_PC, ucp_Sm },
  { 613, PT_PC, ucp_So },
  { 616, PT_SC, ucp_Sundanese },
  { 626, PT_SC, ucp_Syloti_Nagri },
  { 639, PT_SC, ucp_Syriac },
  { 646, PT_SC, ucp_Tagalog },
  { 654, PT_SC, ucp_Tagbanwa },
  { 663, PT_SC, ucp_Tai_Le },
  { 670, PT_SC, ucp_Tamil },
  { 676, PT_SC, ucp_Telugu },
  { 683, PT_SC, ucp_Thaana },
  { 690, PT_SC, ucp_Thai },
  { 695, PT_SC, ucp_Tibetan },
  { 703, PT_SC, ucp_Tifinagh },
  { 712, PT_SC, ucp_Ugaritic },
  { 721, PT_SC, ucp_Vai },
  { 725, PT_SC, ucp_Yi },
  { 728, PT_GC, ucp_Z },
  { 730, PT_PC, ucp_Zl },
  { 733, PT_PC, ucp_Zp },
  { 736, PT_PC, ucp_Zs }
};

const int _pcre_utt_size = sizeof(_pcre_utt)/sizeof(ucp_type_table);

#endif  /* SUPPORT_UTF8 */

/* End of pcre_tables.c */
