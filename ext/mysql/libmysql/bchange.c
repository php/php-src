/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*  File   : bchange.c
    Author : Michael widenius
    Updated: 1987-03-20
    Defines: bchange()

    bchange(dst, old_length, src, new_length, tot_length)
    replaces old_length characters at dst to new_length characters from
    src in a buffer with tot_length bytes.
*/

#include <global.h>
#include "m_string.h"

void bchange(register char *dst, uint old_length, register const char *src, uint new_length, uint tot_length)
{
  uint rest=tot_length-old_length;
  if (old_length < new_length)
    bmove_upp(dst+rest+new_length,dst+tot_length,rest);
  else
    bmove(dst+new_length,dst+old_length,rest);
  memcpy(dst,src,new_length);
}
