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
