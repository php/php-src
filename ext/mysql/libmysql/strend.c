/*  File   : strend.c
    Author : Richard A. O'Keefe.
    Updated: 23 April 1984
    Defines: strend()

    strend(s) returns a character pointer to the NUL which ends s.  That
    is,  strend(s)-s  ==  strlen(s). This is useful for adding things at
    the end of strings.  It is redundant, because  strchr(s,'\0')  could
    be used instead, but this is clearer and faster.
    Beware: the asm version works only if strlen(s) < 65535.
*/

#include <global.h>
#include "m_string.h"

#if	VaxAsm

char *strend(s)
const char *s;
{
  asm("locc $0,$65535,*4(ap)");
  asm("movl r1,r0");
}

#else	/* ~VaxAsm */

char *strend(register const char *s)
{
  while (*s++);
  return (char*) (s-1);
}

#endif	/* VaxAsm */
