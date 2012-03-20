/* -*- c-file-style: "gnu" -*- */
/*
 * This is a quick-and-dirty emulator of the nl_langinfo(CODESET)
 * function defined in the Single Unix Specification for those systems
 * (FreeBSD, etc.) that don't have one yet. It behaves as if it had
 * been called after setlocale(LC_CTYPE, ""), that is it looks at
 * the locale environment variables.
 *
 * http://www.opengroup.org/onlinepubs/7908799/xsh/langinfo.h.html
 *
 * Please extend it as needed and suggest improvements to the author.
 * This emulator will hopefully become redundant soon as
 * nl_langinfo(CODESET) becomes more widely implemented.
 *
 * Since the proposed Li18nux encoding name registry is still not mature,
 * the output follows the MIME registry where possible:
 *
 *   http://www.iana.org/assignments/character-sets
 *
 * A possible autoconf test for the availability of nl_langinfo(CODESET)
 * can be found in
 *
 *   http://www.cl.cam.ac.uk/~mgk25/unicode.html#activate
 *
 * Markus.Kuhn@cl.cam.ac.uk -- 2002-03-11
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted. The author
 * disclaims all warranties with regard to this software.
 *
 * Latest version:
 *
 *   http://www.cl.cam.ac.uk/~mgk25/ucs/langinfo.c
 */

#include "ruby/missing.h"
#include <stdlib.h>
#include <string.h>
#if defined _WIN32 || defined __CYGWIN__
#include <windows.h>
#if defined _WIN32 && !defined strncasecmp
#define strncasecmp strnicmp
#endif
#endif
#ifdef HAVE_LANGINFO_H
#include "langinfo.h"
#endif

#define C_CODESET "US-ASCII"     /* Return this as the encoding of the
				  * C/POSIX locale. Could as well one day
				  * become "UTF-8". */

#if defined _WIN32 || defined __CYGWIN__
#define JA_CODESET "Windows-31J"
#else
#define JA_CODESET "EUC-JP"
#endif

#define digit(x) ((x) >= '0' && (x) <= '9')
#define strstart(s, n) (strncasecmp((s), (n), strlen(n)) == 0)

static char buf[16];

const char *
nl_langinfo_codeset(void)
{
  const char *l, *p;
  int n;

  if (((l = getenv("LC_ALL"))   && *l) ||
      ((l = getenv("LC_CTYPE")) && *l) ||
      ((l = getenv("LANG"))     && *l)) {
    /* check standardized locales */
    if (!strcmp(l, "C") || !strcmp(l, "POSIX"))
      return C_CODESET;
    /* check for encoding name fragment */
    p = strchr(l, '.');
    if (!p++) p = l;
    if (strstart(p, "UTF"))
	return "UTF-8";
    if ((n = 5, strstart(p, "8859-")) || (n = 9, strstart(p, "ISO-8859-"))) {
      if (digit(p[n])) {
	p += n;
	memcpy(buf, "ISO-8859-\0\0", 12);
	buf[9] = *p++;
	if (digit(*p)) buf[10] = *p++;
	return buf;
      }
    }
    if (strstart(p, "KOI8-R")) return "KOI8-R";
    if (strstart(p, "KOI8-U")) return "KOI8-U";
    if (strstart(p, "620")) return "TIS-620";
    if (strstart(p, "2312")) return "GB2312";
    if (strstart(p, "HKSCS")) return "Big5HKSCS";   /* no MIME charset */
    if (strstart(p, "BIG5")) return "Big5";
    if (strstart(p, "GBK")) return "GBK";           /* no MIME charset */
    if (strstart(p, "18030")) return "GB18030";     /* no MIME charset */
    if (strstart(p, "Shift_JIS") || strstart(p, "SJIS")) return "Windows-31J";
    /* check for conclusive modifier */
    if (strstart(p, "euro")) return "ISO-8859-15";
    /* check for language (and perhaps country) codes */
    if (strstart(l, "zh_TW")) return "Big5";
    if (strstart(l, "zh_HK")) return "Big5HKSCS";   /* no MIME charset */
    if (strstart(l, "zh")) return "GB2312";
    if (strstart(l, "ja")) return JA_CODESET;
    if (strstart(l, "ko")) return "EUC-KR";
    if (strstart(l, "ru")) return "KOI8-R";
    if (strstart(l, "uk")) return "KOI8-U";
    if (strstart(l, "pl") || strstart(l, "hr") ||
	strstart(l, "hu") || strstart(l, "cs") ||
	strstart(l, "sk") || strstart(l, "sl")) return "ISO-8859-2";
    if (strstart(l, "eo") || strstart(l, "mt")) return "ISO-8859-3";
    if (strstart(l, "el")) return "ISO-8859-7";
    if (strstart(l, "he")) return "ISO-8859-8";
    if (strstart(l, "tr")) return "ISO-8859-9";
    if (strstart(l, "th")) return "TIS-620";      /* or ISO-8859-11 */
    if (strstart(l, "lt")) return "ISO-8859-13";
    if (strstart(l, "cy")) return "ISO-8859-14";
    if (strstart(l, "ro")) return "ISO-8859-2";   /* or ISO-8859-16 */
    if (strstart(l, "am") || strstart(l, "vi")) return "UTF-8";
    /* Send me further rules if you like, but don't forget that we are
     * *only* interested in locale naming conventions on platforms
     * that do not already provide an nl_langinfo(CODESET) implementation. */
  }
  return NULL;
}

#ifdef HAVE_LANGINFO_H
char *nl_langinfo(nl_item item)
{
  const char *codeset;
  if (item != CODESET)
    return NULL;
  codeset = nl_langinfo_codeset();
  if (!codeset) codeset = C_CODESET;
  return (char *)codeset;
}
#endif

/* For a demo, compile with "gcc -W -Wall -o langinfo -D TEST langinfo.c" */

#ifdef TEST
#include <stdio.h>
int main()
{
  printf("%s\n", nl_langinfo(CODESET));
  return 0;
}
#endif
