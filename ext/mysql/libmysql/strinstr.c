/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*  File   : strinstr.c
    Author : Monty & David
    Updated: 1986.12.08
    Defines: strinstr()

    strinstr(src, pat) looks for an instance of pat in src.  pat is not a
    regex(3) pattern, it is a literal string which must be matched exactly.
    The result 0 if the pattern was not found else it is the start char of
    the pattern counted from the beginning of the string, where the first
    char is 1.
*/

#include <global.h>
#include "m_string.h"

uint strinstr(reg1 const char *str,reg4 const char *search)
{
  reg2 my_string i,j;
  my_string start = (my_string) str;

 skipp:
  while (*str != '\0')
  {
    if (*str++ == *search)
    {
      i=(my_string) str; j= (my_string) search+1;
      while (*j)
	if (*i++ != *j++) goto skipp;
      return ((uint) (str - start));
    }
  }
  return (0);
}
