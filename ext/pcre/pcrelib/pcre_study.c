/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2007 University of Cambridge

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


/* This module contains the external function pcre_study(), along with local
supporting functions. */


#include "pcre_internal.h"


/* Returns from set_start_bits() */

enum { SSB_FAIL, SSB_DONE, SSB_CONTINUE };


/*************************************************
*      Set a bit and maybe its alternate case    *
*************************************************/

/* Given a character, set its bit in the table, and also the bit for the other
version of a letter if we are caseless.

Arguments:
  start_bits    points to the bit map
  c             is the character
  caseless      the caseless flag
  cd            the block with char table pointers

Returns:        nothing
*/

static void
set_bit(uschar *start_bits, unsigned int c, BOOL caseless, compile_data *cd)
{
start_bits[c/8] |= (1 << (c&7));
if (caseless && (cd->ctypes[c] & ctype_letter) != 0)
  start_bits[cd->fcc[c]/8] |= (1 << (cd->fcc[c]&7));
}



/*************************************************
*          Create bitmap of starting bytes       *
*************************************************/

/* This function scans a compiled unanchored expression recursively and
attempts to build a bitmap of the set of possible starting bytes. As time goes
by, we may be able to get more clever at doing this. The SSB_CONTINUE return is
useful for parenthesized groups in patterns such as (a*)b where the group
provides some optional starting bytes but scanning must continue at the outer
level to find at least one mandatory byte. At the outermost level, this
function fails unless the result is SSB_DONE.

Arguments:
  code         points to an expression
  start_bits   points to a 32-byte table, initialized to 0
  caseless     the current state of the caseless flag
  utf8         TRUE if in UTF-8 mode
  cd           the block with char table pointers

Returns:       SSB_FAIL     => Failed to find any starting bytes
               SSB_DONE     => Found mandatory starting bytes
               SSB_CONTINUE => Found optional starting bytes
*/

static int
set_start_bits(const uschar *code, uschar *start_bits, BOOL caseless,
  BOOL utf8, compile_data *cd)
{
register int c;
int yield = SSB_DONE;

#if 0
/* ========================================================================= */
/* The following comment and code was inserted in January 1999. In May 2006,
when it was observed to cause compiler warnings about unused values, I took it
out again. If anybody is still using OS/2, they will have to put it back
manually. */

/* This next statement and the later reference to dummy are here in order to
trick the optimizer of the IBM C compiler for OS/2 into generating correct
code. Apparently IBM isn't going to fix the problem, and we would rather not
disable optimization (in this module it actually makes a big difference, and
the pcre module can use all the optimization it can get). */

volatile int dummy;
/* ========================================================================= */
#endif

do
  {
  const uschar *tcode = code + (((int)*code == OP_CBRA)? 3:1) + LINK_SIZE;
  BOOL try_next = TRUE;

  while (try_next)    /* Loop for items in this branch */
    {
    int rc;
    switch(*tcode)
      {
      /* Fail if we reach something we don't understand */

      default:
      return SSB_FAIL;

      /* If we hit a bracket or a positive lookahead assertion, recurse to set
      bits from within the subpattern. If it can't find anything, we have to
      give up. If it finds some mandatory character(s), we are done for this
      branch. Otherwise, carry on scanning after the subpattern. */

      case OP_BRA:
      case OP_SBRA:
      case OP_CBRA:
      case OP_SCBRA:
      case OP_ONCE:
      case OP_ASSERT:
      rc = set_start_bits(tcode, start_bits, caseless, utf8, cd);
      if (rc == SSB_FAIL) return SSB_FAIL;
      if (rc == SSB_DONE) try_next = FALSE; else
        {
        do tcode += GET(tcode, 1); while (*tcode == OP_ALT);
        tcode += 1 + LINK_SIZE;
        }
      break;

      /* If we hit ALT or KET, it means we haven't found anything mandatory in
      this branch, though we might have found something optional. For ALT, we
      continue with the next alternative, but we have to arrange that the final
      result from subpattern is SSB_CONTINUE rather than SSB_DONE. For KET,
      return SSB_CONTINUE: if this is the top level, that indicates failure,
      but after a nested subpattern, it causes scanning to continue. */

      case OP_ALT:
      yield = SSB_CONTINUE;
      try_next = FALSE;
      break;

      case OP_KET:
      case OP_KETRMAX:
      case OP_KETRMIN:
      return SSB_CONTINUE;

      /* Skip over callout */

      case OP_CALLOUT:
      tcode += 2 + 2*LINK_SIZE;
      break;

      /* Skip over lookbehind and negative lookahead assertions */

      case OP_ASSERT_NOT:
      case OP_ASSERTBACK:
      case OP_ASSERTBACK_NOT:
      do tcode += GET(tcode, 1); while (*tcode == OP_ALT);
      tcode += 1 + LINK_SIZE;
      break;

      /* Skip over an option setting, changing the caseless flag */

      case OP_OPT:
      caseless = (tcode[1] & PCRE_CASELESS) != 0;
      tcode += 2;
      break;

      /* BRAZERO does the bracket, but carries on. */

      case OP_BRAZERO:
      case OP_BRAMINZERO:
      if (set_start_bits(++tcode, start_bits, caseless, utf8, cd) == SSB_FAIL)
        return SSB_FAIL;
/* =========================================================================
      See the comment at the head of this function concerning the next line,
      which was an old fudge for the benefit of OS/2.
      dummy = 1;
  ========================================================================= */
      do tcode += GET(tcode,1); while (*tcode == OP_ALT);
      tcode += 1 + LINK_SIZE;
      break;

      /* Single-char * or ? sets the bit and tries the next item */

      case OP_STAR:
      case OP_MINSTAR:
      case OP_POSSTAR:
      case OP_QUERY:
      case OP_MINQUERY:
      case OP_POSQUERY:
      set_bit(start_bits, tcode[1], caseless, cd);
      tcode += 2;
#ifdef SUPPORT_UTF8
      if (utf8 && tcode[-1] >= 0xc0)
        tcode += _pcre_utf8_table4[tcode[-1] & 0x3f];
#endif
      break;

      /* Single-char upto sets the bit and tries the next */

      case OP_UPTO:
      case OP_MINUPTO:
      case OP_POSUPTO:
      set_bit(start_bits, tcode[3], caseless, cd);
      tcode += 4;
#ifdef SUPPORT_UTF8
      if (utf8 && tcode[-1] >= 0xc0)
        tcode += _pcre_utf8_table4[tcode[-1] & 0x3f];
#endif
      break;

      /* At least one single char sets the bit and stops */

      case OP_EXACT:       /* Fall through */
      tcode += 2;

      case OP_CHAR:
      case OP_CHARNC:
      case OP_PLUS:
      case OP_MINPLUS:
      case OP_POSPLUS:
      set_bit(start_bits, tcode[1], caseless, cd);
      try_next = FALSE;
      break;

      /* Single character type sets the bits and stops */

      case OP_NOT_DIGIT:
      for (c = 0; c < 32; c++)
        start_bits[c] |= ~cd->cbits[c+cbit_digit];
      try_next = FALSE;
      break;

      case OP_DIGIT:
      for (c = 0; c < 32; c++)
        start_bits[c] |= cd->cbits[c+cbit_digit];
      try_next = FALSE;
      break;

      /* The cbit_space table has vertical tab as whitespace; we have to
      discard it. */

      case OP_NOT_WHITESPACE:
      for (c = 0; c < 32; c++)
        {
        int d = cd->cbits[c+cbit_space];
        if (c == 1) d &= ~0x08;
        start_bits[c] |= ~d;
        }
      try_next = FALSE;
      break;

      /* The cbit_space table has vertical tab as whitespace; we have to
      discard it. */

      case OP_WHITESPACE:
      for (c = 0; c < 32; c++)
        {
        int d = cd->cbits[c+cbit_space];
        if (c == 1) d &= ~0x08;
        start_bits[c] |= d;
        }
      try_next = FALSE;
      break;

      case OP_NOT_WORDCHAR:
      for (c = 0; c < 32; c++)
        start_bits[c] |= ~cd->cbits[c+cbit_word];
      try_next = FALSE;
      break;

      case OP_WORDCHAR:
      for (c = 0; c < 32; c++)
        start_bits[c] |= cd->cbits[c+cbit_word];
      try_next = FALSE;
      break;

      /* One or more character type fudges the pointer and restarts, knowing
      it will hit a single character type and stop there. */

      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      tcode++;
      break;

      case OP_TYPEEXACT:
      tcode += 3;
      break;

      /* Zero or more repeats of character types set the bits and then
      try again. */

      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEPOSUPTO:
      tcode += 2;               /* Fall through */

      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPOSSTAR:
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSQUERY:
      switch(tcode[1])
        {
        case OP_ANY:
        return SSB_FAIL;

        case OP_NOT_DIGIT:
        for (c = 0; c < 32; c++)
          start_bits[c] |= ~cd->cbits[c+cbit_digit];
        break;

        case OP_DIGIT:
        for (c = 0; c < 32; c++)
          start_bits[c] |= cd->cbits[c+cbit_digit];
        break;

        /* The cbit_space table has vertical tab as whitespace; we have to
        discard it. */

        case OP_NOT_WHITESPACE:
        for (c = 0; c < 32; c++)
          {
          int d = cd->cbits[c+cbit_space];
          if (c == 1) d &= ~0x08;
          start_bits[c] |= ~d;
          }
        break;

        /* The cbit_space table has vertical tab as whitespace; we have to
        discard it. */

        case OP_WHITESPACE:
        for (c = 0; c < 32; c++)
          {
          int d = cd->cbits[c+cbit_space];
          if (c == 1) d &= ~0x08;
          start_bits[c] |= d;
          }
        break;

        case OP_NOT_WORDCHAR:
        for (c = 0; c < 32; c++)
          start_bits[c] |= ~cd->cbits[c+cbit_word];
        break;

        case OP_WORDCHAR:
        for (c = 0; c < 32; c++)
          start_bits[c] |= cd->cbits[c+cbit_word];
        break;
        }

      tcode += 2;
      break;

      /* Character class where all the information is in a bit map: set the
      bits and either carry on or not, according to the repeat count. If it was
      a negative class, and we are operating with UTF-8 characters, any byte
      with a value >= 0xc4 is a potentially valid starter because it starts a
      character with a value > 255. */

      case OP_NCLASS:
#ifdef SUPPORT_UTF8
      if (utf8)
        {
        start_bits[24] |= 0xf0;              /* Bits for 0xc4 - 0xc8 */
        memset(start_bits+25, 0xff, 7);      /* Bits for 0xc9 - 0xff */
        }
#endif
      /* Fall through */

      case OP_CLASS:
        {
        tcode++;

        /* In UTF-8 mode, the bits in a bit map correspond to character
        values, not to byte values. However, the bit map we are constructing is
        for byte values. So we have to do a conversion for characters whose
        value is > 127. In fact, there are only two possible starting bytes for
        characters in the range 128 - 255. */

#ifdef SUPPORT_UTF8
        if (utf8)
          {
          for (c = 0; c < 16; c++) start_bits[c] |= tcode[c];
          for (c = 128; c < 256; c++)
            {
            if ((tcode[c/8] && (1 << (c&7))) != 0)
              {
              int d = (c >> 6) | 0xc0;            /* Set bit for this starter */
              start_bits[d/8] |= (1 << (d&7));    /* and then skip on to the */
              c = (c & 0xc0) + 0x40 - 1;          /* next relevant character. */
              }
            }
          }

        /* In non-UTF-8 mode, the two bit maps are completely compatible. */

        else
#endif
          {
          for (c = 0; c < 32; c++) start_bits[c] |= tcode[c];
          }

        /* Advance past the bit map, and act on what follows */

        tcode += 32;
        switch (*tcode)
          {
          case OP_CRSTAR:
          case OP_CRMINSTAR:
          case OP_CRQUERY:
          case OP_CRMINQUERY:
          tcode++;
          break;

          case OP_CRRANGE:
          case OP_CRMINRANGE:
          if (((tcode[1] << 8) + tcode[2]) == 0) tcode += 5;
            else try_next = FALSE;
          break;

          default:
          try_next = FALSE;
          break;
          }
        }
      break; /* End of bitmap class handling */

      }      /* End of switch */
    }        /* End of try_next loop */

  code += GET(code, 1);   /* Advance to next branch */
  }
while (*code == OP_ALT);
return yield;
}



/*************************************************
*          Study a compiled expression           *
*************************************************/

/* This function is handed a compiled expression that it must study to produce
information that will speed up the matching. It returns a pcre_extra block
which then gets handed back to pcre_exec().

Arguments:
  re        points to the compiled expression
  options   contains option bits
  errorptr  points to where to place error messages;
            set NULL unless error

Returns:    pointer to a pcre_extra block, with study_data filled in and the
              appropriate flag set;
            NULL on error or if no optimization possible
*/

PCRE_EXP_DEFN pcre_extra *
pcre_study(const pcre *external_re, int options, const char **errorptr)
{
uschar start_bits[32];
pcre_extra *extra;
pcre_study_data *study;
const uschar *tables;
uschar *code;
compile_data compile_block;
const real_pcre *re = (const real_pcre *)external_re;

*errorptr = NULL;

if (re == NULL || re->magic_number != MAGIC_NUMBER)
  {
  *errorptr = "argument is not a compiled regular expression";
  return NULL;
  }

if ((options & ~PUBLIC_STUDY_OPTIONS) != 0)
  {
  *errorptr = "unknown or incorrect option bit(s) set";
  return NULL;
  }

code = (uschar *)re + re->name_table_offset +
  (re->name_count * re->name_entry_size);

/* For an anchored pattern, or an unanchored pattern that has a first char, or
a multiline pattern that matches only at "line starts", no further processing
at present. */

if ((re->options & (PCRE_ANCHORED|PCRE_FIRSTSET|PCRE_STARTLINE)) != 0)
  return NULL;

/* Set the character tables in the block that is passed around */

tables = re->tables;
if (tables == NULL)
  (void)pcre_fullinfo(external_re, NULL, PCRE_INFO_DEFAULT_TABLES,
  (void *)(&tables));

compile_block.lcc = tables + lcc_offset;
compile_block.fcc = tables + fcc_offset;
compile_block.cbits = tables + cbits_offset;
compile_block.ctypes = tables + ctypes_offset;

/* See if we can find a fixed set of initial characters for the pattern. */

memset(start_bits, 0, 32 * sizeof(uschar));
if (set_start_bits(code, start_bits, (re->options & PCRE_CASELESS) != 0,
  (re->options & PCRE_UTF8) != 0, &compile_block) != SSB_DONE) return NULL;

/* Get a pcre_extra block and a pcre_study_data block. The study data is put in
the latter, which is pointed to by the former, which may also get additional
data set later by the calling program. At the moment, the size of
pcre_study_data is fixed. We nevertheless save it in a field for returning via
the pcre_fullinfo() function so that if it becomes variable in the future, we
don't have to change that code. */

extra = (pcre_extra *)(pcre_malloc)
  (sizeof(pcre_extra) + sizeof(pcre_study_data));

if (extra == NULL)
  {
  *errorptr = "failed to get memory";
  return NULL;
  }

study = (pcre_study_data *)((char *)extra + sizeof(pcre_extra));
extra->flags = PCRE_EXTRA_STUDY_DATA;
extra->study_data = study;

study->size = sizeof(pcre_study_data);
study->options = PCRE_STUDY_MAPPED;
memcpy(study->start_bits, start_bits, sizeof(start_bits));

return extra;
}

/* End of pcre_study.c */
