/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

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
