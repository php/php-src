/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

This module is a wrapper that provides a POSIX API to the underlying PCRE
functions.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-1999 University of Cambridge

-----------------------------------------------------------------------------
Permission is granted to anyone to use this software for any purpose on any
computer system, and to redistribute it freely, subject to the following
restrictions:

1. This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

4. If PCRE is embedded in any software that is released under the GNU
   General Purpose Licence (GPL), then the terms of that licence shall
   supersede any condition above with which it is incompatible.
-----------------------------------------------------------------------------
*/

#include "internal.h"
#include "pcreposix.h"
#include "stdlib.h"



/* Corresponding tables of PCRE error messages and POSIX error codes. */

static const char *estring[] = {
  ERR1,  ERR2,  ERR3,  ERR4,  ERR5,  ERR6,  ERR7,  ERR8,  ERR9,  ERR10,
  ERR11, ERR12, ERR13, ERR14, ERR15, ERR16, ERR17, ERR18, ERR19, ERR20,
  ERR21, ERR22, ERR23, ERR24, ERR25 };

static int eint[] = {
  REG_EESCAPE, /* "\\ at end of pattern" */
  REG_EESCAPE, /* "\\c at end of pattern" */
  REG_EESCAPE, /* "unrecognized character follows \\" */
  REG_BADBR,   /* "numbers out of order in {} quantifier" */
  REG_BADBR,   /* "number too big in {} quantifier" */
  REG_EBRACK,  /* "missing terminating ] for character class" */
  REG_ECTYPE,  /* "invalid escape sequence in character class" */
  REG_ERANGE,  /* "range out of order in character class" */
  REG_BADRPT,  /* "nothing to repeat" */
  REG_BADRPT,  /* "operand of unlimited repeat could match the empty string" */
  REG_ASSERT,  /* "internal error: unexpected repeat" */
  REG_BADPAT,  /* "unrecognized character after (?" */
  REG_ESIZE,   /* "too many capturing parenthesized sub-patterns" */
  REG_EPAREN,  /* "missing )" */
  REG_ESUBREG, /* "back reference to non-existent subpattern" */
  REG_INVARG,  /* "erroffset passed as NULL" */
  REG_INVARG,  /* "unknown option bit(s) set" */
  REG_EPAREN,  /* "missing ) after comment" */
  REG_ESIZE,   /* "too many sets of parentheses" */
  REG_ESIZE,   /* "regular expression too large" */
  REG_ESPACE,  /* "failed to get memory" */
  REG_EPAREN,  /* "unmatched brackets" */
  REG_ASSERT,  /* "internal error: code overflow" */
  REG_BADPAT,  /* "unrecognized character after (?<" */
  REG_BADPAT,  /* "lookbehind assertion is not fixed length" */
  REG_BADPAT,  /* "malformed number after (?(" */
  REG_BADPAT,  /* "conditional group containe more than two branches" */
  REG_BADPAT   /* "assertion expected after (?(" */
};

/* Table of texts corresponding to POSIX error codes */

static const char *pstring[] = {
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
*          Translate PCRE text code to int       *
*************************************************/

/* PCRE compile-time errors are given as strings defined as macros. We can just
look them up in a table to turn them into POSIX-style error codes. */

static int
pcre_posix_error_code(const char *s)
{
size_t i;
for (i = 0; i < sizeof(estring)/sizeof(char *); i++)
  if (strcmp(s, estring[i]) == 0) return eint[i];
return REG_ASSERT;
}



/*************************************************
*          Translate error code to string        *
*************************************************/

size_t
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

void
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

int
regcomp(regex_t *preg, const char *pattern, int cflags)
{
const char *errorptr;
int erroffset;
int options = 0;

if ((cflags & REG_ICASE) != 0) options |= PCRE_CASELESS;
if ((cflags & REG_NEWLINE) != 0) options |= PCRE_MULTILINE;

preg->re_pcre = pcre_compile(pattern, options, &errorptr, &erroffset, NULL);
preg->re_erroffset = erroffset;

if (preg->re_pcre == NULL) return pcre_posix_error_code(errorptr);

preg->re_nsub = pcre_info(preg->re_pcre, NULL, NULL);
return 0;
}




/*************************************************
*              Match a regular expression        *
*************************************************/

int
regexec(regex_t *preg, const char *string, size_t nmatch,
  regmatch_t pmatch[], int eflags)
{
int rc;
int options = 0;

if ((eflags & REG_NOTBOL) != 0) options |= PCRE_NOTBOL;
if ((eflags & REG_NOTEOL) != 0) options |= PCRE_NOTEOL;

preg->re_erroffset = (size_t)(-1);   /* Only has meaning after compile */

rc = pcre_exec(preg->re_pcre, NULL, string, (int)strlen(string), 0, options,
  (int *)pmatch, nmatch * 2);

if (rc == 0) return 0;    /* All pmatch were filled in */

if (rc > 0)
  {
  size_t i;
  for (i = rc; i < nmatch; i++) pmatch[i].rm_so = pmatch[i].rm_eo = -1;
  return 0;
  }

else switch(rc)
  {
  case PCRE_ERROR_NOMATCH: return REG_NOMATCH;
  case PCRE_ERROR_NULL: return REG_INVARG;
  case PCRE_ERROR_BADOPTION: return REG_INVARG;
  case PCRE_ERROR_BADMAGIC: return REG_INVARG;
  case PCRE_ERROR_UNKNOWN_NODE: return REG_ASSERT;
  case PCRE_ERROR_NOMEMORY: return REG_ESPACE;
  default: return REG_ASSERT;
  }
}

/* End of pcreposix.c */
