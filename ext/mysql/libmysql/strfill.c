/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*  File   : strfill.c
    Author : Monty
    Updated: 1987.04.16
    Defines: strfill()

    strfill(dest, len, fill) makes a string of fill-characters. The result
    string is of length == len. The des+len character is allways set to NULL.
    strfill() returns pointer to dest+len;
*/

#include <global.h>
#include "m_string.h"

my_string strfill(my_string s,uint len,pchar fill)
{
  while (len--) *s++ = fill;
  *(s) = '\0';
  return(s);
} /* strfill */
