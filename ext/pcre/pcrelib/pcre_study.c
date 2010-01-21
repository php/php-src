/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2010 University of Cambridge

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


#include "config.h"

#include "pcre_internal.h"


/* Returns from set_start_bits() */

enum { SSB_FAIL, SSB_DONE, SSB_CONTINUE };



/*************************************************
*   Find the minimum subject length for a group  *
*************************************************/

/* Scan a parenthesized group and compute the minimum length of subject that
is needed to match it. This is a lower bound; it does not mean there is a
string of that length that matches. In UTF8 mode, the result is in characters
rather than bytes.

Arguments:
  code       pointer to start of group (the bracket)
  startcode  pointer to start of the whole pattern
  options    the compiling options

Returns:   the minimum length
           -1 if \C was encountered
           -2 internal error (missing capturing bracket)
*/

static int
find_minlength(const uschar *code, const uschar *startcode, int options)
{
int length = -1;
BOOL utf8 = (options & PCRE_UTF8) != 0;
BOOL had_recurse = FALSE;
register int branchlength = 0;
register uschar *cc = (uschar *)code + 1 + LINK_SIZE;

if (*code == OP_CBRA || *code == OP_SCBRA) cc += 2;

/* Scan along the opcodes for this branch. If we get to the end of the
branch, check the length against that of the other branches. */

for (;;)
  {
  int d, min;
  uschar *cs, *ce;
  register int op = *cc;

  switch (op)
    {
    case OP_COND:
    case OP_SCOND:

    /* If there is only one branch in a condition, the implied branch has zero
    length, so we don't add anything. This covers the DEFINE "condition"
    automatically. */

    cs = cc + GET(cc, 1);
    if (*cs != OP_ALT)
      {
      cc = cs + 1 + LINK_SIZE;
      break;
      }

    /* Otherwise we can fall through and treat it the same as any other
    subpattern. */

    case OP_CBRA:
    case OP_SCBRA:
    case OP_BRA:
    case OP_SBRA:
    case OP_ONCE:
    d = find_minlength(cc, startcode, options);
    if (d < 0) return d;
    branchlength += d;
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    cc += 1 + LINK_SIZE;
    break;

    /* Reached end of a branch; if it's a ket it is the end of a nested
    call. If it's ALT it is an alternation in a nested call. If it is
    END it's the end of the outer call. All can be handled by the same code. */

    case OP_ALT:
    case OP_KET:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_END:
    if (length < 0 || (!had_recurse && branchlength < length))
      length = branchlength;
    if (*cc != OP_ALT) return length;
    cc += 1 + LINK_SIZE;
    branchlength = 0;
    had_recurse = FALSE;
    break;

    /* Skip over assertive subpatterns */

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    /* Fall through */

    /* Skip over things that don't match chars */

    case OP_REVERSE:
    case OP_CREF:
    case OP_NCREF:
    case OP_RREF:
    case OP_NRREF:
    case OP_DEF:
    case OP_OPT:
    case OP_CALLOUT:
    case OP_SOD:
    case OP_SOM:
    case OP_EOD:
    case OP_EODN:
    case OP_CIRC:
    case OP_DOLL:
    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
    cc += _pcre_OP_lengths[*cc];
    break;

    /* Skip over a subpattern that has a {0} or {0,x} quantifier */

    case OP_BRAZERO:
    case OP_BRAMINZERO:
    case OP_SKIPZERO:
    cc += _pcre_OP_lengths[*cc];
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    cc += 1 + LINK_SIZE;
    break;

    /* Handle literal characters and + repetitions */

    case OP_CHAR:
    case OP_CHARNC:
    case OP_NOT:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_POSPLUS:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTPOSPLUS:
    branchlength++;
    cc += 2;
#ifdef SUPPORT_UTF8
    if (utf8 && cc[-1] >= 0xc0) cc += _pcre_utf8_table4[cc[-1] & 0x3f];
#endif
    break;

    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEPOSPLUS:
    branchlength++;
    cc += (cc[1] == OP_PROP || cc[1] == OP_NOTPROP)? 4 : 2;
    break;

    /* Handle exact repetitions. The count is already in characters, but we
    need to skip over a multibyte character in UTF8 mode.  */

    case OP_EXACT:
    case OP_NOTEXACT:
    branchlength += GET2(cc,1);
    cc += 4;
#ifdef SUPPORT_UTF8
    if (utf8 && cc[-1] >= 0xc0) cc += _pcre_utf8_table4[cc[-1] & 0x3f];
#endif
    break;

    case OP_TYPEEXACT:
    branchlength += GET2(cc,1);
    cc += (cc[3] == OP_PROP || cc[3] == OP_NOTPROP)? 6 : 4;
    break;

    /* Handle single-char non-literal matchers */

    case OP_PROP:
    case OP_NOTPROP:
    cc += 2;
    /* Fall through */

    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    case OP_EXTUNI:
    case OP_HSPACE:
    case OP_NOT_HSPACE:
    case OP_VSPACE:
    case OP_NOT_VSPACE:
    branchlength++;
    cc++;
    break;

    /* "Any newline" might match two characters */

    case OP_ANYNL:
    branchlength += 2;
    cc++;
    break;

    /* The single-byte matcher means we can't proceed in UTF-8 mode */

    case OP_ANYBYTE:
#ifdef SUPPORT_UTF8
    if (utf8) return -1;
#endif
    branchlength++;
    cc++;
    break;

    /* For repeated character types, we have to test for \p and \P, which have
    an extra two bytes of parameters. */

    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    case OP_TYPEPOSSTAR:
    case OP_TYPEPOSQUERY:
    if (cc[1] == OP_PROP || cc[1] == OP_NOTPROP) cc += 2;
    cc += _pcre_OP_lengths[op];
    break;

    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    case OP_TYPEPOSUPTO:
    if (cc[3] == OP_PROP || cc[3] == OP_NOTPROP) cc += 2;
    cc += _pcre_OP_lengths[op];
    break;

    /* Check a class for variable quantification */

#ifdef SUPPORT_UTF8
    case OP_XCLASS:
    cc += GET(cc, 1) - 33;
    /* Fall through */
#endif

    case OP_CLASS:
    case OP_NCLASS:
    cc += 33;

    switch (*cc)
      {
      case OP_CRPLUS:
      case OP_CRMINPLUS:
      branchlength++;
      /* Fall through */

      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      cc++;
      break;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      branchlength += GET2(cc,1);
      cc += 5;
      break;

      default:
      branchlength++;
      break;
      }
    break;

    /* Backreferences and subroutine calls are treated in the same way: we find
    the minimum length for the subpattern. A recursion, however, causes an
    a flag to be set that causes the length of this branch to be ignored. The
    logic is that a recursion can only make sense if there is another
    alternation that stops the recursing. That will provide the minimum length
    (when no recursion happens). A backreference within the group that it is
    referencing behaves in the same way.

    If PCRE_JAVASCRIPT_COMPAT is set, a backreference to an unset bracket
    matches an empty string (by default it causes a matching failure), so in
    that case we must set the minimum length to zero. */

    case OP_REF:
    if ((options & PCRE_JAVASCRIPT_COMPAT) == 0)
      {
      ce = cs = (uschar *)_pcre_find_bracket(startcode, utf8, GET2(cc, 1));
      if (cs == NULL) return -2;
      do ce += GET(ce, 1); while (*ce == OP_ALT);
      if (cc > cs && cc < ce)
        {
        d = 0;
        had_recurse = TRUE;
        }
      else d = find_minlength(cs, startcode, options);
      }
    else d = 0;
    cc += 3;

    /* Handle repeated back references */

    switch (*cc)
      {
      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      min = 0;
      cc++;
      break;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      min = GET2(cc, 1);
      cc += 5;
      break;

      default:
      min = 1;
      break;
      }

    branchlength += min * d;
    break;

    case OP_RECURSE:
    cs = ce = (uschar *)startcode + GET(cc, 1);
    if (cs == NULL) return -2;
    do ce += GET(ce, 1); while (*ce == OP_ALT);
    if (cc > cs && cc < ce)
      had_recurse = TRUE;
    else
      branchlength += find_minlength(cs, startcode, options);
    cc += 1 + LINK_SIZE;
    break;

    /* Anything else does not or need not match a character. We can get the
    item's length from the table, but for those that can match zero occurrences
    of a character, we must take special action for UTF-8 characters. */

    case OP_UPTO:
    case OP_NOTUPTO:
    case OP_MINUPTO:
    case OP_NOTMINUPTO:
    case OP_POSUPTO:
    case OP_STAR:
    case OP_MINSTAR:
    case OP_NOTMINSTAR:
    case OP_POSSTAR:
    case OP_NOTPOSSTAR:
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_NOTMINQUERY:
    case OP_POSQUERY:
    case OP_NOTPOSQUERY:
    cc += _pcre_OP_lengths[op];
#ifdef SUPPORT_UTF8
    if (utf8 && cc[-1] >= 0xc0) cc += _pcre_utf8_table4[cc[-1] & 0x3f];
#endif
    break;

    /* For the record, these are the opcodes that are matched by "default":
    OP_ACCEPT, OP_CLOSE, OP_COMMIT, OP_FAIL, OP_PRUNE, OP_SET_SOM, OP_SKIP,
    OP_THEN. */

    default:
    cc += _pcre_OP_lengths[op];
    break;
    }
  }
/* Control never gets here */
}



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
set_table_bit(uschar *start_bits, unsigned int c, BOOL caseless,
  compile_data *cd)
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

      /* SKIPZERO skips the bracket. */

      case OP_SKIPZERO:
      tcode++;
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
      set_table_bit(start_bits, tcode[1], caseless, cd);
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
      set_table_bit(start_bits, tcode[3], caseless, cd);
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
      set_table_bit(start_bits, tcode[1], caseless, cd);
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
        case OP_ALLANY:
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
              appropriate flags set;
            NULL on error or if no optimization possible
*/

PCRE_EXP_DEFN pcre_extra * PCRE_CALL_CONVENTION
pcre_study(const pcre *external_re, int options, const char **errorptr)
{
int min;
BOOL bits_set = FALSE;
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
a multiline pattern that matches only at "line starts", there is no point in
seeking a list of starting bytes. */

if ((re->options & PCRE_ANCHORED) == 0 &&
    (re->flags & (PCRE_FIRSTSET|PCRE_STARTLINE)) == 0)
  {
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
  bits_set = set_start_bits(code, start_bits,
    (re->options & PCRE_CASELESS) != 0, (re->options & PCRE_UTF8) != 0,
    &compile_block) == SSB_DONE;
  }

/* Find the minimum length of subject string. */

min = find_minlength(code, code, re->options);

/* Return NULL if no optimization is possible. */

if (!bits_set && min < 0) return NULL;

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
study->flags = 0;

if (bits_set)
  {
  study->flags |= PCRE_STUDY_MAPPED;
  memcpy(study->start_bits, start_bits, sizeof(start_bits));
  }

if (min >= 0)
  {
  study->flags |= PCRE_STUDY_MINLEN;
  study->minlength = min;
  }

return extra;
}

/* End of pcre_study.c */
