/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
  strmov(dst, src) moves all the  characters  of  src  (including  the
  closing NUL) to dst, and returns a pointer to the new closing NUL in
  dst.	 The similar UNIX routine strcpy returns the old value of dst,
  which I have never found useful.  strmov(strmov(dst,a),b) moves a//b
  into dst, which seems useful.
*/

#include <global.h>
#include "m_string.h"

#ifdef BAD_STRING_COMPILER
#undef strmov
#define strmov strmov_overlapp
#endif

#ifndef strmov

#if !defined(MC68000) && !defined(DS90)

char *strmov(register char *dst, register const char *src)
{
  while ((*dst++ = *src++)) ;
  return dst-1;
}

#else

char *strmov(dst, src)
     char *dst, *src;
{
  asm("		movl	4(a7),a1	");
  asm("		movl	8(a7),a0	");
  asm(".L4:	movb	(a0)+,(a1)+	");
  asm("		jne	.L4		");
  asm("		movl	a1,d0		");
  asm("		subql	#1,d0		");
}

#endif

#endif /* strmov */
