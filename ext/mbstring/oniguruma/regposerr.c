/**********************************************************************

  regposerr.c - Oniguruma (regular expression library)

  Copyright (C) 2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "config.h"
#include "onigposix.h"

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
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
  "invalid argument",                        /* REG_EONIG_BADARG   */
  "multi-thread error"                       /* REG_EONIG_THREAD   */
};

#include <stdio.h>


extern size_t
regerror(int posix_ecode, const regex_t* reg, char* buf, size_t size)
{
  char* s;
  char tbuf[35];
  size_t len;

  if (posix_ecode > 0 && posix_ecode < sizeof(ESTRING) / sizeof(ESTRING[0])) {
    s = ESTRING[posix_ecode];
  }
  else if (posix_ecode == 0) {
    s = "";
  }
  else {
    sprintf(tbuf, "undefined error code (%d)", posix_ecode);
    s = tbuf;
  }

  len = strlen(s) + 1;

  if (buf != NULL && size > 0) {
    strncpy(buf, s, size - 1);
    buf[size - 1] = '\0';
  }
  return len;
}
