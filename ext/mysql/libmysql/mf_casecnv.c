/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA */

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
  register char *end=str+(uint) strlen(str);
  if (use_mb(default_charset_info))
    while (*str)
    {
      if ((l=my_ismbchar(default_charset_info, str,end))) str+=l;
      else *str=toupper(*str),++str;
    }
  else
#endif
    while ((*str = toupper(*str)) != 0)
      str++;
} /* caseup_str */

	/* string to lowercase */

void casedn_str(my_string str)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+(uint) strlen(str);
  if (use_mb(default_charset_info))
    while (*str)
    {
      if ((l=my_ismbchar(default_charset_info, str,end))) str+=l;
      else *str=tolower(*str),++str;
    }
  else
#endif
    while ((*str= tolower(*str)) != 0)
      str++;
} /* casedn_str */


	/* to uppercase */

void caseup(my_string str, uint length)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+length;
  if (use_mb(default_charset_info))
    while (str<end)
    {
      if ((l=my_ismbchar(default_charset_info, str,end))) str+=l;
      else *str=toupper(*str),++str;
    }
  else
#endif
    for ( ; length>0 ; length--, str++)
      *str= toupper(*str);
} /* caseup */

	/* to lowercase */

void casedn(my_string str, uint length)
{
#ifdef USE_MB
  register uint32 l;
  register char *end=str+length;
  if (use_mb(default_charset_info))
    while (str<end)
    {
      if ((l=my_ismbchar(default_charset_info, str,end))) str+=l;
      else *str=tolower(*str),++str;
    }
  else
#endif
    for ( ; length>0 ; length--, str++)
      *str= tolower(*str);
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

my_string my_strcasestr(const char *str, const char *search)
{
 uchar *i,*j,*pos;

 pos=(uchar*) str;
skipp:
 while (*pos != '\0')
 {
   if (toupper((uchar) *pos++) == toupper((uchar) *search))
   {
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
  register const char *end=s+(uint) strlen(s);
  if (use_mb(default_charset_info))
  {
    while (s<end)
    {
      if ((l=my_ismbchar(default_charset_info, s,end)))
      {
        while (l--)
          if (*s++ != *t++) return 1;
      }
      else if (my_ismbhead(default_charset_info, *t)) return 1;
      else if (toupper((uchar) *s++) != toupper((uchar) *t++)) return 1;
    }
    return *t;
  }
  else
#endif
  {
    while (toupper((uchar) *s) == toupper((uchar) *t++))
      if (!*s++) return 0;
    return ((int) toupper((uchar) s[0]) - (int) toupper((uchar) t[-1]));
  }
}


int my_casecmp(const char *s, const char *t, uint len)
{
#ifdef USE_MB
  register uint32 l;
  register const char *end=s+len;
  if (use_mb(default_charset_info))
  {
    while (s<end)
    {
      if ((l=my_ismbchar(default_charset_info, s,end)))
      {
        while (l--)
          if (*s++ != *t++) return 1;
      }
      else if (my_ismbhead(default_charset_info, *t)) return 1;
      else if (toupper((uchar) *s++) != toupper((uchar) *t++)) return 1;
    }
    return 0;
  }
  else
#endif
  {
    while (len-- != 0 && toupper(*s++) == toupper(*t++)) ;
    return (int) len+1;
  }
}


int my_strsortcmp(const char *s, const char *t)
{
#ifdef USE_STRCOLL
  if (use_strcoll(default_charset_info))
    return my_strcoll(default_charset_info, (uchar *)s, (uchar *)t);
  else
#endif
  {
    while (my_sort_order[(uchar) *s] == my_sort_order[(uchar) *t++])
      if (!*s++) return 0;
    return ((int) my_sort_order[(uchar) s[0]] -
            (int) my_sort_order[(uchar) t[-1]]);
  }
}

int my_sortcmp(const char *s, const char *t, uint len)
{
#ifdef USE_STRCOLL
  if (use_strcoll(default_charset_info))
    return my_strnncoll(default_charset_info,
                        (uchar *)s, len, (uchar *)t, len);
  else
#endif
  {
    while (len--)
    {
      if (my_sort_order[(uchar) *s++] != my_sort_order[(uchar) *t++])
        return ((int) my_sort_order[(uchar) s[-1]] -
                (int) my_sort_order[(uchar) t[-1]]);
    }
    return 0;
  }
}

int my_sortncmp(const char *s, uint s_len, const char *t, uint t_len)
{
#ifdef USE_STRCOLL
  if (use_strcoll(default_charset_info))
    return my_strnncoll(default_charset_info,
                        (uchar *)s, s_len, (uchar *)t, t_len);
  else
#endif
  {
    uint len= min(s_len,t_len);
    while (len--)
    {
      if (my_sort_order[(uchar) *s++] != my_sort_order[(uchar) *t++])
        return ((int) my_sort_order[(uchar) s[-1]] -
                (int) my_sort_order[(uchar) t[-1]]);
    }
    return (int) (s_len - t_len);
  }
}
