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
