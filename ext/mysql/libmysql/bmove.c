/*  File   : bmove.c
    Author : Richard A. O'Keefe.
	     Michael Widenius;	ifdef MC68000
    Updated: 23 April 1984
    Defines: bmove()

    bmove(dst, src, len) moves exactly "len" bytes from the source "src"
    to the destination "dst".  It does not check for NUL characters as
    strncpy() and strnmov() do.  Thus if your C compiler doesn't support
    structure assignment, you can simulate it with
    bmove(&to, &from, sizeof from);
    The standard 4.2bsd routine for this purpose is bcopy.  But as bcopy
    has its first two arguments the other way around you may find this a
    bit easier to get right.
    No value is returned.

    Note: the "b" routines are there to exploit certain VAX order codes,
    but the MOVC3 instruction will only move 65535 characters.	 The asm
    code is presented for your interest and amusement.
*/

#include <global.h>
#include "m_string.h"

#if !defined(HAVE_BMOVE) && !defined(bmove)

#if VaxAsm

void bmove(dst, src, len)
    char *dst, *src;
    uint len;
    {
 asm("movc3 12(ap),*8(ap),*4(ap)");
    }

#else
#if defined(MC68000) && defined(DS90)

void bmove(dst, src, len)
char *dst,*src;
uint len;				/* 0 <= len <= 65535 */
{
asm("		movl	12(a7),d0	");
asm("		subql	#1,d0		");
asm("		blt	.L5		");
asm("		movl	4(a7),a1	");
asm("		movl	8(a7),a0	");
asm(".L4:	movb	(a0)+,(a1)+	");
asm("		dbf	d0,.L4		");
asm(".L5:				");
}
#else

void bmove(dst, src, len)
register byte *dst;
register const char *src;
register uint len;
{
  while (len-- != 0) *dst++ = *src++;
}
#endif
#endif
#endif
