/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB
   This file is public domain and comes with NO WARRANTY of any kind */

/*
  Functions to convert to lover_case and to upper_case in scandinavia.

  case_sort converts a character string to a representaion that can
  be compared by strcmp to find with is alfabetical bigger.
  (lower- and uppercase letters is compared as the same)
*/

#include "mysys_priv.h"
#include <m_ctype.h>

	/* string to uppercase */

void caseup_str(my_string str)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+strlen(str);
  while (*str)
  {
    if ((l=ismbchar(str,end))) str+=l;
    else *str=toupper(*str),++str;
  }
#else
  while ((*str = toupper(*str)) != 0)
    str++;
#endif
} /* caseup_str */

	/* string to lowercase */

void casedn_str(my_string str)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+strlen(str);
  while (*str)
  {
    if ((l=ismbchar(str,end))) str+=l;
    else *str=tolower(*str),++str;
  }
#else
  while ((*str= tolower(*str)) != 0)
    str++;
#endif
} /* casedn_str */


	/* to uppercase */

void caseup(my_string str, uint length)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+length;
  while (str<end)
  {
    if ((l=ismbchar(str,end))) str+=l;
    else *str=toupper(*str),++str;
  }
#else
  for ( ; length>0 ; length--, str++)
    *str= toupper(*str);
#endif
} /* caseup */

	/* to lowercase */

void casedn(my_string str, uint length)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+length;
  while (str<end)
  {
    if ((l=ismbchar(str,end))) str+=l;
    else *str=tolower(*str),++str;
  }
#else
  for ( ; length>0 ; length--, str++)
    *str= tolower(*str);
#endif
} /* casedn */

	/* to sort-string that can be compared to get text in order */

void case_sort(my_string str, uint length)
{
  for ( ; length>0 ; length--, str++)
    *str= (char) my_sort_order[(uchar) *str];
} /* case_sort */

	/* find string in another with no case_sensivity */

/* ToDo: This function should be modified to support multibyte charset.
         However it is not used untill 3.23.5.
	 Wei He (hewei@mail.ied.ac.cn)
*/

my_string strcasestr(const char *str, const char *search)
{
 uchar *i,*j,*pos;

 pos=(uchar*) str;
skipp:
 while (*pos != '\0')
 {
   if (toupper((uchar) *pos++) == toupper((uchar) *search)) {
     i=(uchar*) pos; j=(uchar*) search+1;
     while (*j)
       if (toupper(*i++) != toupper(*j++)) goto skipp;
     return ((char*) pos-1);
   }
 }
 return ((my_string) 0);
} /* strcstr */


	/* compare strings without regarding to case */

int my_strcasecmp(const char *s, const char *t)
{
#ifdef USE_MB
  register uint32 l;
  register const char *end=s+strlen(s);
  while (s<end)
  {
    if ((l=ismbchar(s,end)))
    {
      while (l--)
	if (*s++ != *t++) return 1;
    }
    else if (ismbhead(*t)) return 1;
    else if (toupper((uchar) *s++) != toupper((uchar) *t++)) return 1;
  }
  return *t;
#else
  while (toupper((uchar) *s) == toupper((uchar) *t++))
    if (!*s++) return 0;
  return ((int) toupper((uchar) s[0]) - (int) toupper((uchar) t[-1]));
#endif
}


int my_casecmp(const char *s, const char *t, uint len)
{
#ifdef USE_MB
  register uint32 l;
  register const char *end=s+len;
  while (s<end)
  {
    if ((l=ismbchar(s,end)))
    {
      while (l--)
	if (*s++ != *t++) return 1;
    }
    else if (ismbhead(*t)) return 1;
    else if (toupper((uchar) *s++) != toupper((uchar) *t++)) return 1;
  }
  return 0;
#else
  while (len-- != 0 && toupper(*s++) == toupper(*t++)) ;
  return (int) len+1;
#endif
}


int my_strsortcmp(const char *s, const char *t)
{
#ifdef USE_STRCOLL
  return my_strcoll((uchar *)s, (uchar *)t);
#else
  while (my_sort_order[(uchar) *s] == my_sort_order[(uchar) *t++])
    if (!*s++) return 0;
  return ((int) my_sort_order[(uchar) s[0]] - (int) my_sort_order[(uchar) t[-1]]);
#endif
}

int my_sortcmp(const char *s, const char *t, uint len)
{
#ifndef USE_STRCOLL
  while (len--)
  {
    if (my_sort_order[(uchar) *s++] != my_sort_order[(uchar) *t++])
      return ((int) my_sort_order[(uchar) s[-1]] -
	      (int) my_sort_order[(uchar) t[-1]]);
  }
  return 0;
#else
  return my_strnncoll((uchar *)s, len, (uchar *)t, len);
#endif
}
