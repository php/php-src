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

/*  File   : strcend.c
    Author : Michael Widenius:	ifdef MC68000
    Updated: 20 April 1984
    Defines: strcend()

    strcend(s, c) returns a pointer to the  first  place  in  s where  c
    occurs,  or a pointer to the end-null of s if c does not occur in s.
*/

#include <global.h>
#include "m_string.h"

#if defined(MC68000) && defined(DS90)

char *strcend(const char *s, pchar c)
{
asm("		movl	4(a7),a0	");
asm("		movl	8(a7),d1	");
asm(".L2:	movb	(a0)+,d0	");
asm("		cmpb	d0,d1		");
asm("		beq	.L1		");
asm("		tstb	d0		");
asm("		bne	.L2		");
asm(".L1:	movl	a0,d0		");
asm("		subql	#1,d0		");
}

#else

char *strcend(register const char *s, register pchar c)
{
  for (;;)
  {
     if (*s == (char) c) return (char*) s;
     if (!*s++) return (char*) s-1;
  }
}

#endif
