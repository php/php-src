/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
    strnmov(dst,src,length) moves length characters, or until end, of src to
    dst and appends a closing NUL to dst if src is shorter than length.
    The result is a pointer to the first NUL in dst, or is dst+n if dst was
    truncated.
*/

#include <global.h>
#include "m_string.h"

char *strnmov(register char *dst, register const char *src, uint n)
{
  while (n-- != 0) {
    if (!(*dst++ = *src++)) {
      return (char*) dst-1;
    }
  }
  return dst;
}
