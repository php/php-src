/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*  File   : strcont.c
    Author : Monty
    Updated: 1988.07.27
    Defines: strcont()

    strcont(str, set) if str contanies any character in the string set.
    The result is the position of the first found character in str, or NullS
    if there isn't anything found.

*/

#include <global.h>
#include "m_string.h"

my_string strcont(reg1 const char *str,reg2 const char *set)
{
  reg3 my_string start = (my_string) set;

  while (*str)
  {
    while (*set)
    {
      if (*set++ == *str)
	return ((char*) str);
    }
    set=start; str++;
  }
  return (NullS);
} /* strcont */
