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


/* This module contains the external function pcre_dfa_exec(), which is an
alternative matching function that uses a DFA algorithm. This is NOT Perl-
compatible, but it has advantages in certain applications. */


#include "pcre_internal.h"


/* For use to indent debugging output */

#define SP "                   "



/*************************************************
*      Code parameters and static tables         *
*************************************************/

/* These are offsets that are used to turn the OP_TYPESTAR and friends opcodes
into others, under special conditions. A gap of 10 between the blocks should be
enough. */

#define OP_PROP_EXTRA    (EXTRACT_BASIC_MAX+1)
#define OP_EXTUNI_EXTRA  (EXTRACT_BASIC_MAX+11)


/* This table identifies those opcodes that are followed immediately by a
character that is to be tested in some way. This makes is possible to
centralize the loading of these characters. In the case of Type * etc, the
"character" is the opcode for \D, \d, \S, \s, \W, or \w, which will always be a
small value. */

static uschar coptable[] = {
  0,                             /* End                                    */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* \A, \G, \B, \b, \D, \d, \S, \s, \W, \w */
  0, 0,                          /* Any, Anybyte                           */
  0, 0, 0,                       /* NOTPROP, PROP, EXTUNI                  */
  0, 0, 0, 0, 0,                 /* \Z, \z, Opt, ^, $                      */
  1,                             /* Char                                   */
  1,                             /* Charnc                                 */
  1,                             /* not                                    */
  /* Positive single-char repeats                                          */
  1, 1, 1, 1, 1, 1,              /* *, *?, +, +?, ?, ??                    */
  3, 3, 3,                       /* upto, minupto, exact                   */
  /* Negative single-char repeats - only for chars < 256                   */
  1, 1, 1, 1, 1, 1,              /* NOT *, *?, +, +?, ?, ??                */
  3, 3, 3,                       /* NOT upto, minupto, exact               */
  /* Positive type repeats                                                 */
  1, 1, 1, 1, 1, 1,              /* Type *, *?, +, +?, ?, ??               */
  3, 3, 3,                       /* Type upto, minupto, exact              */
  /* Character class & ref repeats                                         */
  0, 0, 0, 0, 0, 0,              /* *, *?, +, +?, ?, ??                    */
  0, 0,                          /* CRRANGE, CRMINRANGE                    */
  0,                             /* CLASS                                  */
  0,                             /* NCLASS                                 */
  0,                             /* XCLASS - variable length               */
  0,                             /* REF                                    */
  0,                             /* RECURSE                                */
  0,                             /* CALLOUT                                */
  0,                             /* Alt                                    */
  0,                             /* Ket                                    */
  0,                             /* KetRmax                                */
  0,                             /* KetRmin                                */
  0,                             /* Assert                                 */
  0,                             /* Assert not                             */
  0,                             /* Assert behind                          */
  0,                             /* Assert behind not                      */
  0,                             /* Reverse                                */
  0,                             /* Once                                   */
  0,                             /* COND                                   */
  0,                             /* CREF                                   */
  0, 0,                          /* BRAZERO, BRAMINZERO                    */
  0,                             /* BRANUMBER                              */
  0                              /* BRA                                    */
};

/* These 2 tables allow for compact code for testing for \D, \d, \S, \s, \W,
and \w */

static uschar toptable1[] = {
  0, 0, 0, 0, 0,
  ctype_digit, ctype_digit,
  ctype_space, ctype_space,
  ctype_word,  ctype_word,
  0                               /* OP_ANY */
};

static uschar toptable2[] = {
  0, 0, 0, 0, 0,
  ctype_digit, 0,
  ctype_space, 0,
  ctype_word,  0,
  1                               /* OP_ANY */
};


/* Structure for holding data about a particular state, which is in effect the
current data for an active path through the match tree. It must consist
entirely of ints because the working vector we are passed, and which we put
these structures in, is a vector of ints. */

typedef struct stateblock {
  int offset;                     /* Offset to opcode */
  int count;                      /* Count for repeats */
  int ims;                        /* ims flag bits */
  int data;                       /* Some use extra data */
} stateblock;

#define INTS_PER_STATEBLOCK  (sizeof(stateblock)/sizeof(int))


#ifdef DEBUG
/*************************************************
*             Print character string             *
*************************************************/

/* Character string printing function for debugging.

Arguments:
  p            points to string
  length       number of bytes
  f            where to print

Returns:       nothing
*/

static void
pchars(unsigned char *p, int length, FILE *f)
{
int c;
while (length-- > 0)
  {
  if (isprint(c = *(p++)))
    fprintf(f, "%c", c);
  else
    fprintf(f, "\\x%02x", c);
  }
}
#endif



/*************************************************
*    Execute a Regular Expression - DFA engine   *
*************************************************/

/* This internal function applies a compiled pattern to a subject string,
starting at a given point, using a DFA engine. This function is called from the
external one, possibly multiple times if the pattern is not anchored. The
function calls itself recursively for some kinds of subpattern.

Arguments:
  md                the match_data block with fixed information
  this_start_code   the opening bracket of this subexpression's code
  current_subject   where we currently are in the subject string
  start_offset      start offset in the subject string
  offsets           vector to contain the matching string offsets
  offsetcount       size of same
  workspace         vector of workspace
  wscount           size of same
  ims               the current ims flags
  rlevel            function call recursion level
  recursing         regex recursive call level

Returns:            > 0 =>
                    = 0 =>
                     -1 => failed to match
                   < -1 => some kind of unexpected problem

The following macros are used for adding states to the two state vectors (one
for the current character, one for the following character). */

#define ADD_ACTIVE(x,y) \
  if (active_count++ < wscount) \
    { \
    next_active_state->offset = (x); \
    next_active_state->count  = (y); \
    next_active_state->ims    = ims; \
    next_active_state++; \
    DPRINTF(("%.*sADD_ACTIVE(%d,%d)\n", rlevel*2-2, SP, (x), (y))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

#define ADD_ACTIVE_DATA(x,y,z) \
  if (active_count++ < wscount) \
    { \
    next_active_state->offset = (x); \
    next_active_state->count  = (y); \
    next_active_state->ims    = ims; \
    next_active_state->data   = (z); \
    next_active_state++; \
    DPRINTF(("%.*sADD_ACTIVE_DATA(%d,%d,%d)\n", rlevel*2-2, SP, (x), (y), (z))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

#define ADD_NEW(x,y) \
  if (new_count++ < wscount) \
    { \
    next_new_state->offset = (x); \
    next_new_state->count  = (y); \
    next_new_state->ims    = ims; \
    next_new_state++; \
    DPRINTF(("%.*sADD_NEW(%d,%d)\n", rlevel*2-2, SP, (x), (y))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

#define ADD_NEW_DATA(x,y,z) \
  if (new_count++ < wscount) \
    { \
    next_new_state->offset = (x); \
    next_new_state->count  = (y); \
    next_new_state->ims    = ims; \
    next_new_state->data   = (z); \
    next_new_state++; \
    DPRINTF(("%.*sADD_NEW_DATA(%d,%d,%d)\n", rlevel*2-2, SP, (x), (y), (z))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

/* And now, here is the code */

static int
internal_dfa_exec(
  dfa_match_data *md,
  const uschar *this_start_code,
  const uschar *current_subject,
  int start_offset,
  int *offsets,
  int offsetcount,
  int *workspace,
  int wscount,
  int ims,
  int  rlevel,
  int  recursing)
{
stateblock *active_states, *new_states, *temp_states;
stateblock *next_active_state, *next_new_state;

const uschar *ctypes, *lcc, *fcc;
const uschar *ptr;
const uschar *end_code;

int active_count, new_count, match_count;

/* Some fields in the md block are frequently referenced, so we load them into
independent variables in the hope that this will perform better. */

const uschar *start_subject = md->start_subject;
const uschar *end_subject = md->end_subject;
const uschar *start_code = md->start_code;

BOOL utf8 = (md->poptions & PCRE_UTF8) != 0;

rlevel++;
offsetcount &= (-2);

wscount -= 2;
wscount = (wscount - (wscount % (INTS_PER_STATEBLOCK * 2))) /
          (2 * INTS_PER_STATEBLOCK);

DPRINTF(("\n%.*s---------------------\n"
  "%.*sCall to internal_dfa_exec f=%d r=%d\n",
  rlevel*2-2, SP, rlevel*2-2, SP, rlevel, recursing));

ctypes = md->tables + ctypes_offset;
lcc = md->tables + lcc_offset;
fcc = md->tables + fcc_offset;

match_count = PCRE_ERROR_NOMATCH;   /* A negative number */

active_states = (stateblock *)(workspace + 2);
next_new_state = new_states = active_states + wscount;
new_count = 0;

/* The first thing in any (sub) pattern is a bracket of some sort. Push all
the alternative states onto the list, and find out where the end is. This
makes is possible to use this function recursively, when we want to stop at a
matching internal ket rather than at the end.

If the first opcode in the first alternative is OP_REVERSE, we are dealing with
a backward assertion. In that case, we have to find out the maximum amount to
move back, and set up each alternative appropriately. */

if (this_start_code[1+LINK_SIZE] == OP_REVERSE)
  {
  int max_back = 0;
  int gone_back;

  end_code = this_start_code;
  do
    {
    int back = GET(end_code, 2+LINK_SIZE);
    if (back > max_back) max_back = back;
    end_code += GET(end_code, 1);
    }
  while (*end_code == OP_ALT);

  /* If we can't go back the amount required for the longest lookbehind
  pattern, go back as far as we can; some alternatives may still be viable. */

#ifdef SUPPORT_UTF8
  /* In character mode we have to step back character by character */

  if (utf8)
    {
    for (gone_back = 0; gone_back < max_back; gone_back++)
      {
      if (current_subject <= start_subject) break;
      current_subject--;
      while (current_subject > start_subject &&
             (*current_subject & 0xc0) == 0x80)
        current_subject--;
      }
    }
  else
#endif

  /* In byte-mode we can do this quickly. */

    {
    gone_back = (current_subject - max_back < start_subject)?
      current_subject - start_subject : max_back;
    current_subject -= gone_back;
    }

  /* Now we can process the individual branches. */

  end_code = this_start_code;
  do
    {
    int back = GET(end_code, 2+LINK_SIZE);
    if (back <= gone_back)
      {
      int bstate = end_code - start_code + 2 + 2*LINK_SIZE;
      ADD_NEW_DATA(-bstate, 0, gone_back - back);
      }
    end_code += GET(end_code, 1);
    }
  while (*end_code == OP_ALT);
 }

/* This is the code for a "normal" subpattern (not a backward assertion). The
start of a whole pattern is always one of these. If we are at the top level,
we may be asked to restart matching from the same point that we reached for a
previous partial match. We still have to scan through the top-level branches to
find the end state. */

else
  {
  end_code = this_start_code;

  /* Restarting */

  if (rlevel == 1 && (md->moptions & PCRE_DFA_RESTART) != 0)
    {
    do { end_code += GET(end_code, 1); } while (*end_code == OP_ALT);
    new_count = workspace[1];
    if (!workspace[0])
      memcpy(new_states, active_states, new_count * sizeof(stateblock));
    }

  /* Not restarting */

  else
    {
    do
      {
      ADD_NEW(end_code - start_code + 1 + LINK_SIZE, 0);
      end_code += GET(end_code, 1);
      }
    while (*end_code == OP_ALT);
    }
  }

workspace[0] = 0;    /* Bit indicating which vector is current */

DPRINTF(("%.*sEnd state = %d\n", rlevel*2-2, SP, end_code - start_code));

/* Loop for scanning the subject */

ptr = current_subject;
for (;;)
  {
  int i, j;
  int c, d, clen, dlen;

  /* Make the new state list into the active state list and empty the
  new state list. */

  temp_states = active_states;
  active_states = new_states;
  new_states = temp_states;
  active_count = new_count;
  new_count = 0;

  workspace[0] ^= 1;              /* Remember for the restarting feature */
  workspace[1] = active_count;

#ifdef DEBUG
  printf("%.*sNext character: rest of subject = \"", rlevel*2-2, SP);
  pchars((uschar *)ptr, strlen((char *)ptr), stdout);
  printf("\"\n");

  printf("%.*sActive states: ", rlevel*2-2, SP);
  for (i = 0; i < active_count; i++)
    printf("%d/%d ", active_states[i].offset, active_states[i].count);
  printf("\n");
#endif

  /* Set the pointers for adding new states */

  next_active_state = active_states + active_count;
  next_new_state = new_states;

  /* Load the current character from the subject outside the loop, as many
  different states may want to look at it, and we assume that at least one
  will. */

  if (ptr < end_subject)
    {
    clen = 1;
#ifdef SUPPORT_UTF8
    if (utf8) { GETCHARLEN(c, ptr, clen); } else
#endif  /* SUPPORT_UTF8 */
    c = *ptr;
    }
  else
    {
    clen = 0;    /* At end subject */
    c = -1;
    }

  /* Scan up the active states and act on each one. The result of an action
  may be to add more states to the currently active list (e.g. on hitting a
  parenthesis) or it may be to put states on the new list, for considering
  when we move the character pointer on. */

  for (i = 0; i < active_count; i++)
    {
    stateblock *current_state = active_states + i;
    const uschar *code;
    int state_offset = current_state->offset;
    int count, codevalue;
    int chartype, othercase;

#ifdef DEBUG
    printf ("%.*sProcessing state %d c=", rlevel*2-2, SP, state_offset);
    if (c < 0) printf("-1\n");
      else if (c > 32 && c < 127) printf("'%c'\n", c);
        else printf("0x%02x\n", c);
#endif

    /* This variable is referred to implicity in the ADD_xxx macros. */

    ims = current_state->ims;

    /* A negative offset is a special case meaning "hold off going to this
    (negated) state until the number of characters in the data field have
    been skipped". */

    if (state_offset < 0)
      {
      if (current_state->data > 0)
        {
        DPRINTF(("%.*sSkipping this character\n", rlevel*2-2, SP));
        ADD_NEW_DATA(state_offset, current_state->count,
          current_state->data - 1);
        continue;
        }
      else
        {
        current_state->offset = state_offset = -state_offset;
        }
      }

    /* Check for a duplicate state with the same count, and skip if found. */

    for (j = 0; j < i; j++)
      {
      if (active_states[j].offset == state_offset &&
          active_states[j].count == current_state->count)
        {
        DPRINTF(("%.*sDuplicate state: skipped\n", rlevel*2-2, SP));
        goto NEXT_ACTIVE_STATE;
        }
      }

    /* The state offset is the offset to the opcode */

    code = start_code + state_offset;
    codevalue = *code;
    if (codevalue >= OP_BRA) codevalue = OP_BRA; /* All brackets are equal */

    /* If this opcode is followed by an inline character, load it. It is
    tempting to test for the presence of a subject character here, but that
    is wrong, because sometimes zero repetitions of the subject are
    permitted.

    We also use this mechanism for opcodes such as OP_TYPEPLUS that take an
    argument that is not a data character - but is always one byte long.
    Unfortunately, we have to take special action to deal with  \P, \p, and
    \X in this case. To keep the other cases fast, convert these ones to new
    opcodes. */

    if (coptable[codevalue] > 0)
      {
      dlen = 1;
#ifdef SUPPORT_UTF8
      if (utf8) { GETCHARLEN(d, (code + coptable[codevalue]), dlen); } else
#endif  /* SUPPORT_UTF8 */
      d = code[coptable[codevalue]];
      if (codevalue >= OP_TYPESTAR)
        {
        if (d == OP_ANYBYTE) return PCRE_ERROR_DFA_UITEM;
        if (d >= OP_NOTPROP)
          codevalue += (d == OP_EXTUNI)? OP_EXTUNI_EXTRA : OP_PROP_EXTRA;
        }
      }
    else
      {
      dlen = 0;         /* Not strictly necessary, but compilers moan */
      d = -1;           /* if these variables are not set. */
      }


    /* Now process the individual opcodes */

    switch (codevalue)
      {

/* ========================================================================== */
      /* Reached a closing bracket. If not at the end of the pattern, carry
      on with the next opcode. Otherwise, unless we have an empty string and
      PCRE_NOTEMPTY is set, save the match data, shifting up all previous
      matches so we always have the longest first. */

      case OP_KET:
      case OP_KETRMIN:
      case OP_KETRMAX:
      if (code != end_code)
        {
        ADD_ACTIVE(state_offset + 1 + LINK_SIZE, 0);
        if (codevalue != OP_KET)
          {
          ADD_ACTIVE(state_offset - GET(code, 1), 0);
          }
        }
      else if (ptr > current_subject || (md->moptions & PCRE_NOTEMPTY) == 0)
        {
        if (match_count < 0) match_count = (offsetcount >= 2)? 1 : 0;
          else if (match_count > 0 && ++match_count * 2 >= offsetcount)
            match_count = 0;
        count = ((match_count == 0)? offsetcount : match_count * 2) - 2;
        if (count > 0) memmove(offsets + 2, offsets, count * sizeof(int));
        if (offsetcount >= 2)
          {
          offsets[0] = current_subject - start_subject;
          offsets[1] = ptr - start_subject;
          DPRINTF(("%.*sSet matched string = \"%.*s\"\n", rlevel*2-2, SP,
            offsets[1] - offsets[0], current_subject));
          }
        if ((md->moptions & PCRE_DFA_SHORTEST) != 0)
          {
          DPRINTF(("%.*sEnd of internal_dfa_exec %d: returning %d\n"
            "%.*s---------------------\n\n", rlevel*2-2, SP, rlevel,
            match_count, rlevel*2-2, SP));
          return match_count;
          }
        }
      break;

/* ========================================================================== */
      /* These opcodes add to the current list of states without looking
      at the current character. */

      /*-----------------------------------------------------------------*/
      case OP_ALT:
      do { code += GET(code, 1); } while (*code == OP_ALT);
      ADD_ACTIVE(code - start_code, 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_BRA:
      do
        {
        ADD_ACTIVE(code - start_code + 1 + LINK_SIZE, 0);
        code += GET(code, 1);
        }
      while (*code == OP_ALT);
      break;

      /*-----------------------------------------------------------------*/
      case OP_BRAZERO:
      case OP_BRAMINZERO:
      ADD_ACTIVE(state_offset + 1, 0);
      code += 1 + GET(code, 2);
      while (*code == OP_ALT) code += GET(code, 1);
      ADD_ACTIVE(code - start_code + 1 + LINK_SIZE, 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_BRANUMBER:
      ADD_ACTIVE(state_offset + 1 + LINK_SIZE, 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_CIRC:
      if ((ptr == start_subject && (md->moptions & PCRE_NOTBOL) == 0) ||
          ((ims & PCRE_MULTILINE) != 0 && ptr[-1] == NEWLINE))
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EOD:
      if (ptr >= end_subject) { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_OPT:
      ims = code[1];
      ADD_ACTIVE(state_offset + 2, 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_SOD:
      if (ptr == start_subject) { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_SOM:
      if (ptr == start_subject + start_offset) { ADD_ACTIVE(state_offset + 1, 0); }
      break;


/* ========================================================================== */
      /* These opcodes inspect the next subject character, and sometimes
      the previous one as well, but do not have an argument. The variable
      clen contains the length of the current character and is zero if we are
      at the end of the subject. */

      /*-----------------------------------------------------------------*/
      case OP_ANY:
      if (clen > 0 && (c != NEWLINE || (ims & PCRE_DOTALL) != 0))
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EODN:
      if (clen == 0 || (c == NEWLINE && ptr + 1 == end_subject))
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_DOLL:
      if ((md->moptions & PCRE_NOTEOL) == 0)
        {
        if (clen == 0 || (c == NEWLINE && (ptr + 1 == end_subject ||
                                (ims & PCRE_MULTILINE) != 0)))
          { ADD_ACTIVE(state_offset + 1, 0); }
        }
      else if (c == NEWLINE && (ims & PCRE_MULTILINE) != 0)
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/

      case OP_DIGIT:
      case OP_WHITESPACE:
      case OP_WORDCHAR:
      if (clen > 0 && c < 256 &&
            ((ctypes[c] & toptable1[codevalue]) ^ toptable2[codevalue]) != 0)
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_NOT_DIGIT:
      case OP_NOT_WHITESPACE:
      case OP_NOT_WORDCHAR:
      if (clen > 0 && (c >= 256 ||
            ((ctypes[c] & toptable1[codevalue]) ^ toptable2[codevalue]) != 0))
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_WORD_BOUNDARY:
      case OP_NOT_WORD_BOUNDARY:
        {
        int left_word, right_word;

        if (ptr > start_subject)
          {
          const uschar *temp = ptr - 1;
#ifdef SUPPORT_UTF8
          if (utf8) BACKCHAR(temp);
#endif
          GETCHARTEST(d, temp);
          left_word = d < 256 && (ctypes[d] & ctype_word) != 0;
          }
        else left_word = 0;

        if (clen > 0) right_word = c < 256 && (ctypes[c] & ctype_word) != 0;
          else right_word = 0;

        if ((left_word == right_word) == (codevalue == OP_NOT_WORD_BOUNDARY))
          { ADD_ACTIVE(state_offset + 1, 0); }
        }
      break;


#ifdef SUPPORT_UCP

      /*-----------------------------------------------------------------*/
      /* Check the next character by Unicode property. We will get here only
      if the support is in the binary; otherwise a compile-time error occurs.
      */

      case OP_PROP:
      case OP_NOTPROP:
      if (clen > 0)
        {
        int rqdtype, category;
        category = ucp_findchar(c, &chartype, &othercase);
        rqdtype = code[1];
        if (rqdtype >= 128)
          {
          if ((rqdtype - 128 == category) == (codevalue == OP_PROP))
            { ADD_NEW(state_offset + 2, 0); }
          }
        else
          {
          if ((rqdtype == chartype) == (codevalue == OP_PROP))
            { ADD_NEW(state_offset + 2, 0); }
          }
        }
      break;
#endif



/* ========================================================================== */
      /* These opcodes likewise inspect the subject character, but have an
      argument that is not a data character. It is one of these opcodes:
      OP_ANY, OP_DIGIT, OP_NOT_DIGIT, OP_WHITESPACE, OP_NOT_SPACE, OP_WORDCHAR,
      OP_NOT_WORDCHAR. The value is loaded into d. */

      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || c != '\n' || (ims & PCRE_DOTALL) != 0) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          count++;
          ADD_NEW(state_offset, count);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || c != '\n' || (ims & PCRE_DOTALL) != 0) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          ADD_NEW(state_offset + 2, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || c != '\n' || (ims & PCRE_DOTALL) != 0) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          ADD_NEW(state_offset, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPEEXACT:
      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      if (codevalue != OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 4, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || c != '\n' || (ims & PCRE_DOTALL) != 0) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + 4, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;

/* ========================================================================== */
      /* These are virtual opcodes that are used when something like
      OP_TYPEPLUS has OP_PROP, OP_NOTPROP, or OP_EXTUNI as its argument. It
      keeps the code above fast for the other cases. The argument is in the
      d variable. */

      case OP_PROP_EXTRA + OP_TYPEPLUS:
      case OP_PROP_EXTRA + OP_TYPEMINPLUS:
      count = current_state->count;           /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 3, 0); }
      if (clen > 0)
        {
        int category = ucp_findchar(c, &chartype, &othercase);
        int rqdtype = code[2];
        if ((d == OP_PROP) ==
            (rqdtype == ((rqdtype >= 128)? (category + 128) : chartype)))
          { count++; ADD_NEW(state_offset, count); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXTUNI_EXTRA + OP_TYPEPLUS:
      case OP_EXTUNI_EXTRA + OP_TYPEMINPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0 && ucp_findchar(c, &chartype, &othercase) != ucp_M)
        {
        const uschar *nptr = ptr + clen;
        int ncount = 0;
        while (nptr < end_subject)
          {
          int nd;
          int ndlen = 1;
          GETCHARLEN(nd, nptr, ndlen);
          if (ucp_findchar(nd, &chartype, &othercase) != ucp_M) break;
          ncount++;
          nptr += ndlen;
          }
        count++;
        ADD_NEW_DATA(-state_offset, count, ncount);
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_PROP_EXTRA + OP_TYPEQUERY:
      case OP_PROP_EXTRA + OP_TYPEMINQUERY:
      count = 3;
      goto QS1;

      case OP_PROP_EXTRA + OP_TYPESTAR:
      case OP_PROP_EXTRA + OP_TYPEMINSTAR:
      count = 0;

      QS1:

      ADD_ACTIVE(state_offset + 3, 0);
      if (clen > 0)
        {
        int category = ucp_findchar(c, &chartype, &othercase);
        int rqdtype = code[2];
        if ((d == OP_PROP) ==
            (rqdtype == ((rqdtype >= 128)? (category + 128) : chartype)))
          { ADD_NEW(state_offset + count, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXTUNI_EXTRA + OP_TYPEQUERY:
      case OP_EXTUNI_EXTRA + OP_TYPEMINQUERY:
      count = 2;
      goto QS2;

      case OP_EXTUNI_EXTRA + OP_TYPESTAR:
      case OP_EXTUNI_EXTRA + OP_TYPEMINSTAR:
      count = 0;

      QS2:

      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0 && ucp_findchar(c, &chartype, &othercase) != ucp_M)
        {
        const uschar *nptr = ptr + clen;
        int ncount = 0;
        while (nptr < end_subject)
          {
          int nd;
          int ndlen = 1;
          GETCHARLEN(nd, nptr, ndlen);
          if (ucp_findchar(nd, &chartype, &othercase) != ucp_M) break;
          ncount++;
          nptr += ndlen;
          }
        ADD_NEW_DATA(-(state_offset + count), 0, ncount);
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_PROP_EXTRA + OP_TYPEEXACT:
      case OP_PROP_EXTRA + OP_TYPEUPTO:
      case OP_PROP_EXTRA + OP_TYPEMINUPTO:
      if (codevalue != OP_PROP_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 5, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        int category = ucp_findchar(c, &chartype, &othercase);
        int rqdtype = code[4];
        if ((d == OP_PROP) ==
            (rqdtype == ((rqdtype >= 128)? (category + 128) : chartype)))
          {
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + 5, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXTUNI_EXTRA + OP_TYPEEXACT:
      case OP_EXTUNI_EXTRA + OP_TYPEUPTO:
      case OP_EXTUNI_EXTRA + OP_TYPEMINUPTO:
      if (codevalue != OP_EXTUNI_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 4, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0 && ucp_findchar(c, &chartype, &othercase) != ucp_M)
        {
        const uschar *nptr = ptr + clen;
        int ncount = 0;
        while (nptr < end_subject)
          {
          int nd;
          int ndlen = 1;
          GETCHARLEN(nd, nptr, ndlen);
          if (ucp_findchar(nd, &chartype, &othercase) != ucp_M) break;
          ncount++;
          nptr += ndlen;
          }
        if (++count >= GET2(code, 1))
          { ADD_NEW_DATA(-(state_offset + 4), 0, ncount); }
        else
          { ADD_NEW_DATA(-state_offset, count, ncount); }
        }
      break;

/* ========================================================================== */
      /* These opcodes are followed by a character that is usually compared
      to the current subject character; it is loaded into d. We still get
      here even if there is no subject character, because in some cases zero
      repetitions are permitted. */

      /*-----------------------------------------------------------------*/
      case OP_CHAR:
      if (clen > 0 && c == d) { ADD_NEW(state_offset + dlen + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_CHARNC:
      if (clen == 0) break;

#ifdef SUPPORT_UTF8
      if (utf8)
        {
        if (c == d) { ADD_NEW(state_offset + dlen + 1, 0); } else
          {
          if (c < 128) othercase = fcc[c]; else

          /* If we have Unicode property support, we can use it to test the
          other case of the character, if there is one. The result of
          ucp_findchar() is < 0 if the char isn't found, and othercase is
          returned as zero if there isn't another case. */

#ifdef SUPPORT_UCP
          if (ucp_findchar(c, &chartype, &othercase) < 0)
#endif
            othercase = -1;

          if (d == othercase) { ADD_NEW(state_offset + dlen + 1, 0); }
          }
        }
      else
#endif  /* SUPPORT_UTF8 */

      /* Non-UTF-8 mode */
        {
        if (lcc[c] == lcc[d]) { ADD_NEW(state_offset + 2, 0); }
        }
      break;


#ifdef SUPPORT_UCP
      /*-----------------------------------------------------------------*/
      /* This is a tricky one because it can match more than one character.
      Find out how many characters to skip, and then set up a negative state
      to wait for them to pass before continuing. */

      case OP_EXTUNI:
      if (clen > 0 && ucp_findchar(c, &chartype, &othercase) != ucp_M)
        {
        const uschar *nptr = ptr + clen;
        int ncount = 0;
        while (nptr < end_subject)
          {
          int nclen = 1;
          GETCHARLEN(c, nptr, nclen);
          if (ucp_findchar(c, &chartype, &othercase) != ucp_M) break;
          ncount++;
          nptr += nclen;
          }
        ADD_NEW_DATA(-(state_offset + 1), 0, ncount);
        }
      break;
#endif

      /*-----------------------------------------------------------------*/
      /* Match a negated single character. This is only used for one-byte
      characters, that is, we know that d < 256. The character we are
      checking (c) can be multibyte. */

      case OP_NOT:
      if (clen > 0)
        {
        int otherd = ((ims & PCRE_CASELESS) != 0)? fcc[d] : d;
        if (c != d && c != otherd) { ADD_NEW(state_offset + dlen + 1, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_PLUS:
      case OP_MINPLUS:
      case OP_NOTPLUS:
      case OP_NOTMINPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + dlen + 1, 0); }
      if (clen > 0)
        {
        int otherd = -1;
        if ((ims & PCRE_CASELESS) != 0)
          {
#ifdef SUPPORT_UTF8
          if (utf8 && c >= 128)
            {
#ifdef SUPPORT_UCP
            if (ucp_findchar(d, &chartype, &otherd) < 0) otherd = -1;
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF8 */
          otherd = fcc[d];
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          { count++; ADD_NEW(state_offset, count); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_QUERY:
      case OP_MINQUERY:
      case OP_NOTQUERY:
      case OP_NOTMINQUERY:
      ADD_ACTIVE(state_offset + dlen + 1, 0);
      if (clen > 0)
        {
        int otherd = -1;
        if ((ims && PCRE_CASELESS) != 0)
          {
#ifdef SUPPORT_UTF8
          if (utf8 && c >= 128)
            {
#ifdef SUPPORT_UCP
            if (ucp_findchar(c, &chartype, &otherd) < 0) otherd = -1;
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF8 */
          otherd = fcc[d];
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          { ADD_NEW(state_offset + dlen + 1, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_STAR:
      case OP_MINSTAR:
      case OP_NOTSTAR:
      case OP_NOTMINSTAR:
      ADD_ACTIVE(state_offset + dlen + 1, 0);
      if (clen > 0)
        {
        int otherd = -1;
        if ((ims && PCRE_CASELESS) != 0)
          {
#ifdef SUPPORT_UTF8
          if (utf8 && c >= 128)
            {
#ifdef SUPPORT_UCP
            if (ucp_findchar(c, &chartype, &otherd) < 0) otherd = -1;
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF8 */
          otherd = fcc[d];
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          { ADD_NEW(state_offset, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXACT:
      case OP_UPTO:
      case OP_MINUPTO:
      case OP_NOTEXACT:
      case OP_NOTUPTO:
      case OP_NOTMINUPTO:
      if (codevalue != OP_EXACT && codevalue != OP_NOTEXACT)
        { ADD_ACTIVE(state_offset + dlen + 3, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        int otherd = -1;
        if ((ims & PCRE_CASELESS) != 0)
          {
#ifdef SUPPORT_UTF8
          if (utf8 && c >= 128)
            {
#ifdef SUPPORT_UCP
            if (ucp_findchar(d, &chartype, &otherd) < 0) otherd = -1;
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF8 */
          otherd = fcc[d];
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          {
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + dlen + 3, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;


/* ========================================================================== */
      /* These are the class-handling opcodes */

      case OP_CLASS:
      case OP_NCLASS:
      case OP_XCLASS:
        {
        BOOL isinclass = FALSE;
        int next_state_offset;
        const uschar *ecode;

        /* For a simple class, there is always just a 32-byte table, and we
        can set isinclass from it. */

        if (codevalue != OP_XCLASS)
          {
          ecode = code + 33;
          if (clen > 0)
            {
            isinclass = (c > 255)? (codevalue == OP_NCLASS) :
              ((code[1 + c/8] & (1 << (c&7))) != 0);
            }
          }

        /* An extended class may have a table or a list of single characters,
        ranges, or both, and it may be positive or negative. There's a
        function that sorts all this out. */

        else
         {
         ecode = code + GET(code, 1);
         if (clen > 0) isinclass = _pcre_xclass(c, code + 1 + LINK_SIZE);
         }

        /* At this point, isinclass is set for all kinds of class, and ecode
        points to the byte after the end of the class. If there is a
        quantifier, this is where it will be. */

        next_state_offset = ecode - start_code;

        switch (*ecode)
          {
          case OP_CRSTAR:
          case OP_CRMINSTAR:
          ADD_ACTIVE(next_state_offset + 1, 0);
          if (isinclass) { ADD_NEW(state_offset, 0); }
          break;

          case OP_CRPLUS:
          case OP_CRMINPLUS:
          count = current_state->count;  /* Already matched */
          if (count > 0) { ADD_ACTIVE(next_state_offset + 1, 0); }
          if (isinclass) { count++; ADD_NEW(state_offset, count); }
          break;

          case OP_CRQUERY:
          case OP_CRMINQUERY:
          ADD_ACTIVE(next_state_offset + 1, 0);
          if (isinclass) { ADD_NEW(next_state_offset + 1, 0); }
          break;

          case OP_CRRANGE:
          case OP_CRMINRANGE:
          count = current_state->count;  /* Already matched */
          if (count >= GET2(ecode, 1))
            { ADD_ACTIVE(next_state_offset + 5, 0); }
          if (isinclass)
            {
            if (++count >= GET2(ecode, 3))
              { ADD_NEW(next_state_offset + 5, 0); }
            else
              { ADD_NEW(state_offset, count); }
            }
          break;

          default:
          if (isinclass) { ADD_NEW(next_state_offset, 0); }
          break;
          }
        }
      break;

/* ========================================================================== */
      /* These are the opcodes for fancy brackets of various kinds. We have
      to use recursion in order to handle them. */

      case OP_ASSERT:
      case OP_ASSERT_NOT:
      case OP_ASSERTBACK:
      case OP_ASSERTBACK_NOT:
        {
        int rc;
        int local_offsets[2];
        int local_workspace[1000];
        const uschar *endasscode = code + GET(code, 1);

        while (*endasscode == OP_ALT) endasscode += GET(endasscode, 1);

        rc = internal_dfa_exec(
          md,                                   /* static match data */
          code,                                 /* this subexpression's code */
          ptr,                                  /* where we currently are */
          ptr - start_subject,                  /* start offset */
          local_offsets,                        /* offset vector */
          sizeof(local_offsets)/sizeof(int),    /* size of same */
          local_workspace,                      /* workspace vector */
          sizeof(local_workspace)/sizeof(int),  /* size of same */
          ims,                                  /* the current ims flags */
          rlevel,                               /* function recursion level */
          recursing);                           /* pass on regex recursion */

        if ((rc >= 0) == (codevalue == OP_ASSERT || codevalue == OP_ASSERTBACK))
            { ADD_ACTIVE(endasscode + LINK_SIZE + 1 - start_code, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_COND:
        {
        int local_offsets[1000];
        int local_workspace[1000];
        int condcode = code[LINK_SIZE+1];

        /* The only supported version of OP_CREF is for the value 0xffff, which
        means "test if in a recursion". */

        if (condcode == OP_CREF)
          {
          int value = GET2(code, LINK_SIZE+2);
          if (value != 0xffff) return PCRE_ERROR_DFA_UCOND;
          if (recursing > 0) { ADD_ACTIVE(state_offset + LINK_SIZE + 4, 0); }
            else { ADD_ACTIVE(state_offset + GET(code, 1) + LINK_SIZE + 1, 0); }
          }

        /* Otherwise, the condition is an assertion */

        else
          {
          int rc;
          const uschar *asscode = code + LINK_SIZE + 1;
          const uschar *endasscode = asscode + GET(asscode, 1);

          while (*endasscode == OP_ALT) endasscode += GET(endasscode, 1);

          rc = internal_dfa_exec(
            md,                                   /* fixed match data */
            asscode,                              /* this subexpression's code */
            ptr,                                  /* where we currently are */
            ptr - start_subject,                  /* start offset */
            local_offsets,                        /* offset vector */
            sizeof(local_offsets)/sizeof(int),    /* size of same */
            local_workspace,                      /* workspace vector */
            sizeof(local_workspace)/sizeof(int),  /* size of same */
            ims,                                  /* the current ims flags */
            rlevel,                               /* function recursion level */
            recursing);                           /* pass on regex recursion */

          if ((rc >= 0) ==
                (condcode == OP_ASSERT || condcode == OP_ASSERTBACK))
            { ADD_ACTIVE(endasscode + LINK_SIZE + 1 - start_code, 0); }
          else
            { ADD_ACTIVE(state_offset + GET(code, 1) + LINK_SIZE + 1, 0); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_RECURSE:
        {
        int local_offsets[1000];
        int local_workspace[1000];
        int rc;

        DPRINTF(("%.*sStarting regex recursion %d\n", rlevel*2-2, SP,
          recursing + 1));

        rc = internal_dfa_exec(
          md,                                   /* fixed match data */
          start_code + GET(code, 1),            /* this subexpression's code */
          ptr,                                  /* where we currently are */
          ptr - start_subject,                  /* start offset */
          local_offsets,                        /* offset vector */
          sizeof(local_offsets)/sizeof(int),    /* size of same */
          local_workspace,                      /* workspace vector */
          sizeof(local_workspace)/sizeof(int),  /* size of same */
          ims,                                  /* the current ims flags */
          rlevel,                               /* function recursion level */
          recursing + 1);                       /* regex recurse level */

        DPRINTF(("%.*sReturn from regex recursion %d: rc=%d\n", rlevel*2-2, SP,
          recursing + 1, rc));

        /* Ran out of internal offsets */

        if (rc == 0) return PCRE_ERROR_DFA_RECURSE;

        /* For each successful matched substring, set up the next state with a
        count of characters to skip before trying it. Note that the count is in
        characters, not bytes. */

        if (rc > 0)
          {
          for (rc = rc*2 - 2; rc >= 0; rc -= 2)
            {
            const uschar *p = start_subject + local_offsets[rc];
            const uschar *pp = start_subject + local_offsets[rc+1];
            int charcount = local_offsets[rc+1] - local_offsets[rc];
            while (p < pp) if ((*p++ & 0xc0) == 0x80) charcount--;
            if (charcount > 0)
              {
              ADD_NEW_DATA(-(state_offset + LINK_SIZE + 1), 0, (charcount - 1));
              }
            else
              {
              ADD_ACTIVE(state_offset + LINK_SIZE + 1, 0);
              }
            }
          }
        else if (rc != PCRE_ERROR_NOMATCH) return rc;
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_ONCE:
        {
        const uschar *endcode;
        int local_offsets[2];
        int local_workspace[1000];

        int rc = internal_dfa_exec(
          md,                                   /* fixed match data */
          code,                                 /* this subexpression's code */
          ptr,                                  /* where we currently are */
          ptr - start_subject,                  /* start offset */
          local_offsets,                        /* offset vector */
          sizeof(local_offsets)/sizeof(int),    /* size of same */
          local_workspace,                      /* workspace vector */
          sizeof(local_workspace)/sizeof(int),  /* size of same */
          ims,                                  /* the current ims flags */
          rlevel,                               /* function recursion level */
          recursing);                           /* pass on regex recursion */

        if (rc >= 0)
          {
          const uschar *end_subpattern = code;
          int charcount = local_offsets[1] - local_offsets[0];
          int next_state_offset, repeat_state_offset;
          BOOL is_repeated;

          do { end_subpattern += GET(end_subpattern, 1); }
            while (*end_subpattern == OP_ALT);
          next_state_offset = end_subpattern - start_code + LINK_SIZE + 1;

          /* If the end of this subpattern is KETRMAX or KETRMIN, we must
          arrange for the repeat state also to be added to the relevant list.
          Calculate the offset, or set -1 for no repeat. */

          repeat_state_offset = (*end_subpattern == OP_KETRMAX ||
                                 *end_subpattern == OP_KETRMIN)?
            end_subpattern - start_code - GET(end_subpattern, 1) : -1;

          /* If we have matched an empty string, add the next state at the
          current character pointer. This is important so that the duplicate
          checking kicks in, which is what breaks infinite loops that match an
          empty string. */

          if (charcount == 0)
            {
            ADD_ACTIVE(next_state_offset, 0);
            }

          /* Optimization: if there are no more active states, and there
          are no new states yet set up, then skip over the subject string
          right here, to save looping. Otherwise, set up the new state to swing
          into action when the end of the substring is reached. */

          else if (i + 1 >= active_count && new_count == 0)
            {
            ptr += charcount;
            clen = 0;
            ADD_NEW(next_state_offset, 0);

            /* If we are adding a repeat state at the new character position,
            we must fudge things so that it is the only current state.
            Otherwise, it might be a duplicate of one we processed before, and
            that would cause it to be skipped. */

            if (repeat_state_offset >= 0)
              {
              next_active_state = active_states;
              active_count = 0;
              i = -1;
              ADD_ACTIVE(repeat_state_offset, 0);
              }
            }
          else
            {
            const uschar *p = start_subject + local_offsets[0];
            const uschar *pp = start_subject + local_offsets[1];
            while (p < pp) if ((*p++ & 0xc0) == 0x80) charcount--;
            ADD_NEW_DATA(-next_state_offset, 0, (charcount - 1));
            if (repeat_state_offset >= 0)
              { ADD_NEW_DATA(-repeat_state_offset, 0, (charcount - 1)); }
            }

          }
        else if (rc != PCRE_ERROR_NOMATCH) return rc;
        }
      break;


/* ========================================================================== */
      /* Handle callouts */

      case OP_CALLOUT:
      if (pcre_callout != NULL)
        {
        int rrc;
        pcre_callout_block cb;
        cb.version          = 1;   /* Version 1 of the callout block */
        cb.callout_number   = code[1];
        cb.offset_vector    = offsets;
        cb.subject          = (char *)start_subject;
        cb.subject_length   = end_subject - start_subject;
        cb.start_match      = current_subject - start_subject;
        cb.current_position = ptr - start_subject;
        cb.pattern_position = GET(code, 2);
        cb.next_item_length = GET(code, 2 + LINK_SIZE);
        cb.capture_top      = 1;
        cb.capture_last     = -1;
        cb.callout_data     = md->callout_data;
        if ((rrc = (*pcre_callout)(&cb)) < 0) return rrc;   /* Abandon */
        if (rrc == 0) { ADD_ACTIVE(state_offset + 2 + 2*LINK_SIZE, 0); }
        }
      break;


/* ========================================================================== */
      default:        /* Unsupported opcode */
      return PCRE_ERROR_DFA_UITEM;
      }

    NEXT_ACTIVE_STATE: continue;

    }      /* End of loop scanning active states */

  /* We have finished the processing at the current subject character. If no
  new states have been set for the next character, we have found all the
  matches that we are going to find. If we are at the top level and partial
  matching has been requested, check for appropriate conditions. */

  if (new_count <= 0)
    {
    if (match_count < 0 &&                     /* No matches found */
        rlevel == 1 &&                         /* Top level match function */
        (md->moptions & PCRE_PARTIAL) != 0 &&  /* Want partial matching */
        ptr >= end_subject &&                  /* Reached end of subject */
        ptr > current_subject)                 /* Matched non-empty string */
      {
      if (offsetcount >= 2)
        {
        offsets[0] = current_subject - start_subject;
        offsets[1] = end_subject - start_subject;
        }
      match_count = PCRE_ERROR_PARTIAL;
      }

    DPRINTF(("%.*sEnd of internal_dfa_exec %d: returning %d\n"
      "%.*s---------------------\n\n", rlevel*2-2, SP, rlevel, match_count,
      rlevel*2-2, SP));
    return match_count;
    }

  /* One or more states are active for the next character. */

  ptr += clen;    /* Advance to next subject character */
  }               /* Loop to move along the subject string */

/* Control never gets here, but we must keep the compiler happy. */

DPRINTF(("%.*s+++ Unexpected end of internal_dfa_exec %d +++\n"
  "%.*s---------------------\n\n", rlevel*2-2, SP, rlevel, rlevel*2-2, SP));
return PCRE_ERROR_NOMATCH;
}




/*************************************************
*    Execute a Regular Expression - DFA engine   *
*************************************************/

/* This external function applies a compiled re to a subject string using a DFA
engine. This function calls the internal function multiple times if the pattern
is not anchored.

Arguments:
  argument_re     points to the compiled expression
  extra_data      points to extra data or is NULL (not currently used)
  subject         points to the subject string
  length          length of subject string (may contain binary zeros)
  start_offset    where to start in the subject string
  options         option bits
  offsets         vector of match offsets
  offsetcount     size of same
  workspace       workspace vector
  wscount         size of same

Returns:          > 0 => number of match offset pairs placed in offsets
                  = 0 => offsets overflowed; longest matches are present
                   -1 => failed to match
                 < -1 => some kind of unexpected problem
*/

EXPORT int
pcre_dfa_exec(const pcre *argument_re, const pcre_extra *extra_data,
  const char *subject, int length, int start_offset, int options, int *offsets,
  int offsetcount, int *workspace, int wscount)
{
real_pcre *re = (real_pcre *)argument_re;
dfa_match_data match_block;
BOOL utf8, anchored, startline, firstline;
const uschar *current_subject, *end_subject, *lcc;

pcre_study_data internal_study;
const pcre_study_data *study = NULL;
real_pcre internal_re;

const uschar *req_byte_ptr;
const uschar *start_bits = NULL;
BOOL first_byte_caseless = FALSE;
BOOL req_byte_caseless = FALSE;
int first_byte = -1;
int req_byte = -1;
int req_byte2 = -1;

/* Plausibility checks */

if ((options & ~PUBLIC_DFA_EXEC_OPTIONS) != 0) return PCRE_ERROR_BADOPTION;
if (re == NULL || subject == NULL || workspace == NULL ||
   (offsets == NULL && offsetcount > 0)) return PCRE_ERROR_NULL;
if (offsetcount < 0) return PCRE_ERROR_BADCOUNT;
if (wscount < 20) return PCRE_ERROR_DFA_WSSIZE;

/* We need to find the pointer to any study data before we test for byte
flipping, so we scan the extra_data block first. This may set two fields in the
match block, so we must initialize them beforehand. However, the other fields
in the match block must not be set until after the byte flipping. */

match_block.tables = re->tables;
match_block.callout_data = NULL;

if (extra_data != NULL)
  {
  unsigned int flags = extra_data->flags;
  if ((flags & PCRE_EXTRA_STUDY_DATA) != 0)
    study = (const pcre_study_data *)extra_data->study_data;
  if ((flags & PCRE_EXTRA_MATCH_LIMIT) != 0) return PCRE_ERROR_DFA_UMLIMIT;
  if ((flags & PCRE_EXTRA_CALLOUT_DATA) != 0)
    match_block.callout_data = extra_data->callout_data;
  if ((flags & PCRE_EXTRA_TABLES) != 0)
    match_block.tables = extra_data->tables;
  }

/* Check that the first field in the block is the magic number. If it is not,
test for a regex that was compiled on a host of opposite endianness. If this is
the case, flipped values are put in internal_re and internal_study if there was
study data too. */

if (re->magic_number != MAGIC_NUMBER)
  {
  re = _pcre_try_flipped(re, &internal_re, study, &internal_study);
  if (re == NULL) return PCRE_ERROR_BADMAGIC;
  if (study != NULL) study = &internal_study;
  }

/* Set some local values */

current_subject = (const unsigned char *)subject + start_offset;
end_subject = (const unsigned char *)subject + length;
req_byte_ptr = current_subject - 1;

utf8 = (re->options & PCRE_UTF8) != 0;
anchored = (options & PCRE_ANCHORED) != 0 || (re->options & PCRE_ANCHORED) != 0;

/* The remaining fixed data for passing around. */

match_block.start_code = (const uschar *)argument_re +
    re->name_table_offset + re->name_count * re->name_entry_size;
match_block.start_subject = (const unsigned char *)subject;
match_block.end_subject = end_subject;
match_block.moptions = options;
match_block.poptions = re->options;

/* Check a UTF-8 string if required. Unfortunately there's no way of passing
back the character offset. */

#ifdef SUPPORT_UTF8
if (utf8 && (options & PCRE_NO_UTF8_CHECK) == 0)
  {
  if (_pcre_valid_utf8((uschar *)subject, length) >= 0)
    return PCRE_ERROR_BADUTF8;
  if (start_offset > 0 && start_offset < length)
    {
    int tb = ((uschar *)subject)[start_offset];
    if (tb > 127)
      {
      tb &= 0xc0;
      if (tb != 0 && tb != 0xc0) return PCRE_ERROR_BADUTF8_OFFSET;
      }
    }
  }
#endif

/* If the exec call supplied NULL for tables, use the inbuilt ones. This
is a feature that makes it possible to save compiled regex and re-use them
in other programs later. */

if (match_block.tables == NULL) match_block.tables = _pcre_default_tables;

/* The lower casing table and the "must be at the start of a line" flag are
used in a loop when finding where to start. */

lcc = match_block.tables + lcc_offset;
startline = (re->options & PCRE_STARTLINE) != 0;
firstline = (re->options & PCRE_FIRSTLINE) != 0;

/* Set up the first character to match, if available. The first_byte value is
never set for an anchored regular expression, but the anchoring may be forced
at run time, so we have to test for anchoring. The first char may be unset for
an unanchored pattern, of course. If there's no first char and the pattern was
studied, there may be a bitmap of possible first characters. */

if (!anchored)
  {
  if ((re->options & PCRE_FIRSTSET) != 0)
    {
    first_byte = re->first_byte & 255;
    if ((first_byte_caseless = ((re->first_byte & REQ_CASELESS) != 0)) == TRUE)
      first_byte = lcc[first_byte];
    }
  else
    {
    if (startline && study != NULL &&
         (study->options & PCRE_STUDY_MAPPED) != 0)
      start_bits = study->start_bits;
    }
  }

/* For anchored or unanchored matches, there may be a "last known required
character" set. */

if ((re->options & PCRE_REQCHSET) != 0)
  {
  req_byte = re->req_byte & 255;
  req_byte_caseless = (re->req_byte & REQ_CASELESS) != 0;
  req_byte2 = (match_block.tables + fcc_offset)[req_byte];  /* case flipped */
  }

/* Call the main matching function, looping for a non-anchored regex after a
failed match. Unless restarting, optimize by moving to the first match
character if possible, when not anchored. Then unless wanting a partial match,
check for a required later character. */

for (;;)
  {
  int rc;

  if ((options & PCRE_DFA_RESTART) == 0)
    {
    const uschar *save_end_subject = end_subject;

    /* Advance to a unique first char if possible. If firstline is TRUE, the
    start of the match is constrained to the first line of a multiline string.
    Implement this by temporarily adjusting end_subject so that we stop scanning
    at a newline. If the match fails at the newline, later code breaks this loop.
    */

    if (firstline)
      {
      const uschar *t = current_subject;
      while (t < save_end_subject && *t != '\n') t++;
      end_subject = t;
      }

    if (first_byte >= 0)
      {
      if (first_byte_caseless)
        while (current_subject < end_subject &&
               lcc[*current_subject] != first_byte)
          current_subject++;
      else
        while (current_subject < end_subject && *current_subject != first_byte)
          current_subject++;
      }

    /* Or to just after \n for a multiline match if possible */

    else if (startline)
      {
      if (current_subject > match_block.start_subject + start_offset)
        {
        while (current_subject < end_subject && current_subject[-1] != NEWLINE)
          current_subject++;
        }
      }

    /* Or to a non-unique first char after study */

    else if (start_bits != NULL)
      {
      while (current_subject < end_subject)
        {
        register unsigned int c = *current_subject;
        if ((start_bits[c/8] & (1 << (c&7))) == 0) current_subject++;
          else break;
        }
      }

    /* Restore fudged end_subject */

    end_subject = save_end_subject;
    }

  /* If req_byte is set, we know that that character must appear in the subject
  for the match to succeed. If the first character is set, req_byte must be
  later in the subject; otherwise the test starts at the match point. This
  optimization can save a huge amount of work in patterns with nested unlimited
  repeats that aren't going to match. Writing separate code for cased/caseless
  versions makes it go faster, as does using an autoincrement and backing off
  on a match.

  HOWEVER: when the subject string is very, very long, searching to its end can
  take a long time, and give bad performance on quite ordinary patterns. This
  showed up when somebody was matching /^C/ on a 32-megabyte string... so we
  don't do this when the string is sufficiently long.

  ALSO: this processing is disabled when partial matching is requested.
  */

  if (req_byte >= 0 &&
      end_subject - current_subject < REQ_BYTE_MAX &&
      (options & PCRE_PARTIAL) == 0)
    {
    register const uschar *p = current_subject + ((first_byte >= 0)? 1 : 0);

    /* We don't need to repeat the search if we haven't yet reached the
    place we found it at last time. */

    if (p > req_byte_ptr)
      {
      if (req_byte_caseless)
        {
        while (p < end_subject)
          {
          register int pp = *p++;
          if (pp == req_byte || pp == req_byte2) { p--; break; }
          }
        }
      else
        {
        while (p < end_subject)
          {
          if (*p++ == req_byte) { p--; break; }
          }
        }

      /* If we can't find the required character, break the matching loop,
      which will cause a return or PCRE_ERROR_NOMATCH. */

      if (p >= end_subject) break;

      /* If we have found the required character, save the point where we
      found it, so that we don't search again next time round the loop if
      the start hasn't passed this character yet. */

      req_byte_ptr = p;
      }
    }

  /* OK, now we can do the business */

  rc = internal_dfa_exec(
    &match_block,                              /* fixed match data */
    match_block.start_code,                    /* this subexpression's code */
    current_subject,                           /* where we currently are */
    start_offset,                              /* start offset in subject */
    offsets,                                   /* offset vector */
    offsetcount,                               /* size of same */
    workspace,                                 /* workspace vector */
    wscount,                                   /* size of same */
    re->options & (PCRE_CASELESS|PCRE_MULTILINE|PCRE_DOTALL), /* ims flags */
    0,                                         /* function recurse level */
    0);                                        /* regex recurse level */

  /* Anything other than "no match" means we are done, always; otherwise, carry
  on only if not anchored. */

  if (rc != PCRE_ERROR_NOMATCH || anchored) return rc;

  /* Advance to the next subject character unless we are at the end of a line
  and firstline is set. */

  if (firstline && *current_subject == NEWLINE) break;
  current_subject++;

#ifdef SUPPORT_UTF8
  if (utf8)
    {
    while (current_subject < end_subject && (*current_subject & 0xc0) == 0x80)
      current_subject++;
    }
#endif

  if (current_subject > end_subject) break;
  }

return PCRE_ERROR_NOMATCH;
}

/* End of pcre_dfa_exec.c */
