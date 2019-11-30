/**********************************************************************
  regposerr.c - Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2019  K.Kosako
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* Can't include regint.h etc.. for conflict of regex_t.
   Define ONIGURUMA_EXPORT here for onigposix.h.
 */
#ifndef ONIGURUMA_EXPORT
#define ONIGURUMA_EXPORT
#endif

#include "config.h"
#include "onigposix.h"

#include <string.h>

#if defined(__GNUC__)
#  define ARG_UNUSED  __attribute__ ((unused))
#else
#  define ARG_UNUSED
#endif

#if defined(_WIN32) && !defined(__GNUC__)
#define xsnprintf   sprintf_s
#define xstrncpy(dest,src,size)   strncpy_s(dest,size,src,_TRUNCATE)
#else
#define xsnprintf   snprintf
#define xstrncpy    strncpy
#endif

static char* ESTRING[] = {
  NULL,
  "failed to match",                         /* REG_NOMATCH    */
  "Invalid regular expression",              /* REG_BADPAT     */
  "invalid collating element referenced",    /* REG_ECOLLATE   */
  "invalid character class type referenced", /* REG_ECTYPE     */
  "bad backslash-escape sequence",           /* REG_EESCAPE    */
  "invalid back reference number",           /* REG_ESUBREG    */
  "imbalanced [ and ]",                      /* REG_EBRACK     */
  "imbalanced ( and )",                      /* REG_EPAREN     */
  "imbalanced { and }",                      /* REG_EBRACE     */
  "invalid repeat range {n,m}",              /* REG_BADBR      */
  "invalid range",                           /* REG_ERANGE     */
  "Out of memory",                           /* REG_ESPACE     */
  "? * + not preceded by valid regular expression", /* REG_BADRPT   */

  /* Extended errors */
  "internal error",                          /* REG_EONIG_INTERNAL */
  "invalid wide char value",                 /* REG_EONIG_BADWC    */
  "invalid argument"                         /* REG_EONIG_BADARG   */
};

#include <stdio.h>


extern size_t
regerror(int posix_ecode, const regex_t* reg ARG_UNUSED, char* buf,
         size_t size)
{
  char* s;
  char tbuf[35];
  size_t len;

  if (posix_ecode > 0
      && posix_ecode < (int )(sizeof(ESTRING) / sizeof(ESTRING[0]))) {
    s = ESTRING[posix_ecode];
  }
  else if (posix_ecode == 0) {
    s = "";
  }
  else {
    xsnprintf(tbuf, sizeof(tbuf), "undefined error code (%d)", posix_ecode);
    s = tbuf;
  }

  len = strlen(s) + 1; /* use strlen() because s is ascii encoding. */

  if (buf != NULL && size > 0) {
    xstrncpy(buf, s, size - 1);
    buf[size - 1] = '\0';
  }
  return len;
}
