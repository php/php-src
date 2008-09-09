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


/* This module is a wrapper that provides a POSIX API to the underlying PCRE
functions. */


#include "config.h"


/* Ensure that the PCREPOSIX_EXP_xxx macros are set appropriately for
compiling these functions. This must come before including pcreposix.h, where
they are set for an application (using these functions) if they have not
previously been set. */

#if defined(_WIN32) && !defined(PCRE_STATIC)
#  define PCREPOSIX_EXP_DECL extern __declspec(dllexport)
#  define PCREPOSIX_EXP_DEFN __declspec(dllexport)
#endif

#include "pcre.h"
#include "pcre_internal.h"
#include "pcreposix.h"


/* Table to translate PCRE compile time error codes into POSIX error codes. */

static const int eint[] = {
  0,           /* no error */
  REG_EESCAPE, /* \ at end of pattern */
  REG_EESCAPE, /* \c at end of pattern */
  REG_EESCAPE, /* unrecognized character follows \ */
  REG_BADBR,   /* numbers out of order in {} quantifier */
  REG_BADBR,   /* number too big in {} quantifier */
  REG_EBRACK,  /* missing terminating ] for character class */
  REG_ECTYPE,  /* invalid escape sequence in character class */
  REG_ERANGE,  /* range out of order in character class */
  REG_BADRPT,  /* nothing to repeat */
  REG_BADRPT,  /* operand of unlimited repeat could match the empty string */
  REG_ASSERT,  /* internal error: unexpected repeat */
  REG_BADPAT,  /* unrecognized character after (? */
  REG_BADPAT,  /* POSIX named classes are supported only within a class */
  REG_EPAREN,  /* missing ) */
  REG_ESUBREG, /* reference to non-existent subpattern */
  REG_INVARG,  /* erroffset passed as NULL */
  REG_INVARG,  /* unknown option bit(s) set */
  REG_EPAREN,  /* missing ) after comment */
  REG_ESIZE,   /* parentheses nested too deeply */
  REG_ESIZE,   /* regular expression too large */
  REG_ESPACE,  /* failed to get memory */
  REG_EPAREN,  /* unmatched brackets */
  REG_ASSERT,  /* internal error: code overflow */
  REG_BADPAT,  /* unrecognized character after (?< */
  REG_BADPAT,  /* lookbehind assertion is not fixed length */
  REG_BADPAT,  /* malformed number or name after (?( */
  REG_BADPAT,  /* conditional group contains more than two branches */
  REG_BADPAT,  /* assertion expected after (?( */
  REG_BADPAT,  /* (?R or (?[+-]digits must be followed by ) */
  REG_ECTYPE,  /* unknown POSIX class name */
  REG_BADPAT,  /* POSIX collating elements are not supported */
  REG_INVARG,  /* this version of PCRE is not compiled with PCRE_UTF8 support */
  REG_BADPAT,  /* spare error */
  REG_BADPAT,  /* character value in \x{...} sequence is too large */
  REG_BADPAT,  /* invalid condition (?(0) */
  REG_BADPAT,  /* \C not allowed in lookbehind assertion */
  REG_EESCAPE, /* PCRE does not support \L, \l, \N, \U, or \u */
  REG_BADPAT,  /* number after (?C is > 255 */
  REG_BADPAT,  /* closing ) for (?C expected */
  REG_BADPAT,  /* recursive call could loop indefinitely */
  REG_BADPAT,  /* unrecognized character after (?P */
  REG_BADPAT,  /* syntax error in subpattern name (missing terminator) */
  REG_BADPAT,  /* two named subpatterns have the same name */
  REG_BADPAT,  /* invalid UTF-8 string */
  REG_BADPAT,  /* support for \P, \p, and \X has not been compiled */
  REG_BADPAT,  /* malformed \P or \p sequence */
  REG_BADPAT,  /* unknown property name after \P or \p */
  REG_BADPAT,  /* subpattern name is too long (maximum 32 characters) */
  REG_BADPAT,  /* too many named subpatterns (maximum 10,000) */
  REG_BADPAT,  /* repeated subpattern is too long */
  REG_BADPAT,  /* octal value is greater than \377 (not in UTF-8 mode) */
  REG_BADPAT,  /* internal error: overran compiling workspace */
  REG_BADPAT,  /* internal error: previously-checked referenced subpattern not found */
  REG_BADPAT,  /* DEFINE group contains more than one branch */
  REG_BADPAT,  /* repeating a DEFINE group is not allowed */
  REG_INVARG,  /* inconsistent NEWLINE options */
  REG_BADPAT,  /* \g is not followed followed by an (optionally braced) non-zero number */
  REG_BADPAT,  /* (?+ or (?- must be followed by a non-zero number */
  REG_BADPAT,  /* number is too big */
  REG_BADPAT,  /* subpattern name expected */
  REG_BADPAT,  /* digit expected after (?+ */
  REG_BADPAT   /* ] is an invalid data character in JavaScript compatibility mode */
};

/* Table of texts corresponding to POSIX error codes */

static const char *const pstring[] = {
  "",                                /* Dummy for value 0 */
  "internal error",                  /* REG_ASSERT */
  "invalid repeat counts in {}",     /* BADBR      */
  "pattern error",                   /* BADPAT     */
  "? * + invalid",                   /* BADRPT     */
  "unbalanced {}",                   /* EBRACE     */
  "unbalanced []",                   /* EBRACK     */
  "collation error - not relevant",  /* ECOLLATE   */
  "bad class",                       /* ECTYPE     */
  "bad escape sequence",             /* EESCAPE    */
  "empty expression",                /* EMPTY      */
  "unbalanced ()",                   /* EPAREN     */
  "bad range inside []",             /* ERANGE     */
  "expression too big",              /* ESIZE      */
  "failed to get memory",            /* ESPACE     */
  "bad back reference",              /* ESUBREG    */
  "bad argument",                    /* INVARG     */
  "match failed"                     /* NOMATCH    */
};




/*************************************************
*          Translate error code to string        *
*************************************************/

PCREPOSIX_EXP_DEFN size_t PCRE_CALL_CONVENTION
regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size)
{
const char *message, *addmessage;
size_t length, addlength;

message = (errcode >= (int)(sizeof(pstring)/sizeof(char *)))?
  "unknown error code" : pstring[errcode];
length = strlen(message) + 1;

addmessage = " at offset ";
addlength = (preg != NULL && (int)preg->re_erroffset != -1)?
  strlen(addmessage) + 6 : 0;

if (errbuf_size > 0)
  {
  if (addlength > 0 && errbuf_size >= length + addlength)
    sprintf(errbuf, "%s%s%-6d", message, addmessage, (int)preg->re_erroffset);
  else
    {
    strncpy(errbuf, message, errbuf_size - 1);
    errbuf[errbuf_size-1] = 0;
    }
  }

return length + addlength;
}




/*************************************************
*           Free store held by a regex           *
*************************************************/

PCREPOSIX_EXP_DEFN void PCRE_CALL_CONVENTION
regfree(regex_t *preg)
{
(pcre_free)(preg->re_pcre);
}




/*************************************************
*            Compile a regular expression        *
*************************************************/

/*
Arguments:
  preg        points to a structure for recording the compiled expression
  pattern     the pattern to compile
  cflags      compilation flags

Returns:      0 on success
              various non-zero codes on failure
*/

PCREPOSIX_EXP_DEFN int PCRE_CALL_CONVENTION
regcomp(regex_t *preg, const char *pattern, int cflags)
{
const char *errorptr;
int erroffset;
int errorcode;
int options = 0;

if ((cflags & REG_ICASE) != 0)   options |= PCRE_CASELESS;
if ((cflags & REG_NEWLINE) != 0) options |= PCRE_MULTILINE;
if ((cflags & REG_DOTALL) != 0)  options |= PCRE_DOTALL;
if ((cflags & REG_NOSUB) != 0)   options |= PCRE_NO_AUTO_CAPTURE;
if ((cflags & REG_UTF8) != 0)    options |= PCRE_UTF8;

preg->re_pcre = pcre_compile2(pattern, options, &errorcode, &errorptr,
  &erroffset, NULL);
preg->re_erroffset = erroffset;

if (preg->re_pcre == NULL) return eint[errorcode];

preg->re_nsub = pcre_info((const pcre *)preg->re_pcre, NULL, NULL);
return 0;
}




/*************************************************
*              Match a regular expression        *
*************************************************/

/* Unfortunately, PCRE requires 3 ints of working space for each captured
substring, so we have to get and release working store instead of just using
the POSIX structures as was done in earlier releases when PCRE needed only 2
ints. However, if the number of possible capturing brackets is small, use a
block of store on the stack, to reduce the use of malloc/free. The threshold is
in a macro that can be changed at configure time.

If REG_NOSUB was specified at compile time, the PCRE_NO_AUTO_CAPTURE flag will
be set. When this is the case, the nmatch and pmatch arguments are ignored, and
the only result is yes/no/error. */

PCREPOSIX_EXP_DEFN int PCRE_CALL_CONVENTION
regexec(const regex_t *preg, const char *string, size_t nmatch,
  regmatch_t pmatch[], int eflags)
{
int rc, so, eo;
int options = 0;
int *ovector = NULL;
int small_ovector[POSIX_MALLOC_THRESHOLD * 3];
BOOL allocated_ovector = FALSE;
BOOL nosub =
  (((const pcre *)preg->re_pcre)->options & PCRE_NO_AUTO_CAPTURE) != 0;

if ((eflags & REG_NOTBOL) != 0) options |= PCRE_NOTBOL;
if ((eflags & REG_NOTEOL) != 0) options |= PCRE_NOTEOL;

((regex_t *)preg)->re_erroffset = (size_t)(-1);  /* Only has meaning after compile */

/* When no string data is being returned, ensure that nmatch is zero.
Otherwise, ensure the vector for holding the return data is large enough. */

if (nosub) nmatch = 0;

else if (nmatch > 0)
  {
  if (nmatch <= POSIX_MALLOC_THRESHOLD)
    {
    ovector = &(small_ovector[0]);
    }
  else
    {
    if (nmatch > INT_MAX/(sizeof(int) * 3)) return REG_ESPACE;
    ovector = (int *)malloc(sizeof(int) * nmatch * 3);
    if (ovector == NULL) return REG_ESPACE;
    allocated_ovector = TRUE;
    }
  }

/* REG_STARTEND is a BSD extension, to allow for non-NUL-terminated strings.
The man page from OS X says "REG_STARTEND affects only the location of the
string, not how it is matched". That is why the "so" value is used to bump the
start location rather than being passed as a PCRE "starting offset". */

if ((eflags & REG_STARTEND) != 0)
  {
  so = pmatch[0].rm_so;
  eo = pmatch[0].rm_eo;
  }
else
  {
  so = 0;
  eo = strlen(string);
  }

rc = pcre_exec((const pcre *)preg->re_pcre, NULL, string + so, (eo - so),
  0, options, ovector, nmatch * 3);

if (rc == 0) rc = nmatch;    /* All captured slots were filled in */

if (rc >= 0)
  {
  size_t i;
  if (!nosub)
    {
    for (i = 0; i < (size_t)rc; i++)
      {
      pmatch[i].rm_so = ovector[i*2];
      pmatch[i].rm_eo = ovector[i*2+1];
      }
    if (allocated_ovector) free(ovector);
    for (; i < nmatch; i++) pmatch[i].rm_so = pmatch[i].rm_eo = -1;
    }
  return 0;
  }

else
  {
  if (allocated_ovector) free(ovector);
  switch(rc)
    {
    case PCRE_ERROR_NOMATCH: return REG_NOMATCH;
    case PCRE_ERROR_NULL: return REG_INVARG;
    case PCRE_ERROR_BADOPTION: return REG_INVARG;
    case PCRE_ERROR_BADMAGIC: return REG_INVARG;
    case PCRE_ERROR_UNKNOWN_NODE: return REG_ASSERT;
    case PCRE_ERROR_NOMEMORY: return REG_ESPACE;
    case PCRE_ERROR_MATCHLIMIT: return REG_ESPACE;
    case PCRE_ERROR_BADUTF8: return REG_INVARG;
    case PCRE_ERROR_BADUTF8_OFFSET: return REG_INVARG;
    default: return REG_ASSERT;
    }
  }
}

/* End of pcreposix.c */
