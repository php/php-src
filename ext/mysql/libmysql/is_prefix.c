/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*  File   : is_prefix.c
    Author : Michael Widenius
    Defines: is_prefix()

    is_prefix(s, t) returns 1 if s starts with t.
    A empty t is allways a prefix.
*/

#include <global.h>
#include "m_string.h"

int is_prefix(register const char *s, register const char *t)
{
  while (*t)
    if (*s++ != *t++) return 0;
  return 1;					/* WRONG */
}
