/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*  File   : bmove.c
    Author : Michael widenius
    Updated: 1987-03-20
    Defines: bmove_upp()

    bmove_upp(dst, src, len) moves exactly "len" bytes from the source
    "src-len" to the destination "dst-len" counting downwards.
*/

#include <global.h>
#include "m_string.h"

#if defined(MC68000) && defined(DS90)

/* 0 <= len <= 65535 */
void bmove_upp(byte *dst, const byte *src,uint len)
{
asm("		movl	12(a7),d0	");
asm("		subql	#1,d0		");
asm("		blt	.L5		");
asm("		movl	4(a7),a1	");
asm("		movl	8(a7),a0	");
asm(".L4:	movb	-(a0),-(a1)	");
asm("		dbf	d0,.L4		");
asm(".L5:				");
}
#else

void bmove_upp(register char *dst, register const char *src, register uint len)
{
  while (len-- != 0) *--dst = *--src;
}

#endif
