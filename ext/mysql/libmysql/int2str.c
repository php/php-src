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
  Defines: int2str(), itoa(), ltoa()

  int2str(dst, radix, val)
  converts the (long) integer "val" to character form and moves it to
  the destination string "dst" followed by a terminating NUL.  The
  result is normally a pointer to this NUL character, but if the radix
  is dud the result will be NullS and nothing will be changed.

  If radix is -2..-36, val is taken to be SIGNED.
  If radix is  2.. 36, val is taken to be UNSIGNED.
  That is, val is signed if and only if radix is.  You will normally
  use radix -10 only through itoa and ltoa, for radix 2, 8, or 16
  unsigned is what you generally want.

  _dig_vec is public just in case someone has a use for it.
  The definitions of itoa and ltoa are actually macros in m_string.h,
  but this is where the code is.

  Note: The standard itoa() returns a pointer to the argument, when int2str
	returns the pointer to the end-null.
	itoa assumes that 10 -base numbers are allways signed and other arn't.
*/

#include <global.h>
#include "m_string.h"

char NEAR _dig_vec[] =
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";


char *int2str(register long int val, register char *dst, register int radix)
{
  char buffer[65];
  register char *p;
  long int new_val;

  if (radix < 0) {
    if (radix < -36 || radix > -2) return NullS;
    if (val < 0) {
      *dst++ = '-';
      val = -val;
    }
    radix = -radix;
  } else {
    if (radix > 36 || radix < 2) return NullS;
  }
  /*  The slightly contorted code which follows is due to the
      fact that few machines directly support unsigned long / and %.
      Certainly the VAX C compiler generates a subroutine call.  In
      the interests of efficiency (hollow laugh) I let this happen
      for the first digit only; after that "val" will be in range so
      that signed integer division will do.  Sorry 'bout that.
      CHECK THE CODE PRODUCED BY YOUR C COMPILER.  The first % and /
      should be unsigned, the second % and / signed, but C compilers
      tend to be extraordinarily sensitive to minor details of style.
      This works on a VAX, that's all I claim for it.
      */
  p = &buffer[sizeof(buffer)-1];
  *p = '\0';
  new_val=(ulong) val / (ulong) radix;
  *--p = _dig_vec[(uchar) ((ulong) val- (ulong) new_val*(ulong) radix)];
  val = new_val;
#ifdef HAVE_LDIV
  while (val != 0)
  {
    ldiv_t res;
    res=ldiv(val,radix);
    *--p = _dig_vec[res.rem];
    val= res.quot;
  }
#else
  while (val != 0)
  {
    new_val=val/radix;
    *--p = _dig_vec[(uchar) (val-new_val*radix)];
    val= new_val;
  }
#endif
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}


/*
  This is a faster version of the above optimized for the normal case of
   radix 10 / -10
*/

char *int10_to_str(long int val,char *dst,int radix)
{
  char buffer[65];
  register char *p;
  long int new_val;

  if (radix < 0)				/* -10 */
  {
    if (val < 0)
    {
      *dst++ = '-';
      val = -val;
    }
  }

  p = &buffer[sizeof(buffer)-1];
  *p = '\0';
  new_val= (long) ((unsigned long int) val / 10);
  *--p = '0'+ (char) ((unsigned long int) val - (unsigned long) new_val * 10);
  val = new_val;

  while (val != 0)
  {
    new_val=val/10;
    *--p = '0' + (char) (val-new_val*10);
    val= new_val;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}


#ifdef USE_MY_ITOA

	/* Change to less general itoa interface */

char *my_itoa(int val, char *dst, int radix)
{
  VOID(int2str((long) val,dst,(radix == 10 ? -10 : radix)));
  return dst;
}

char *my_ltoa(long int val, char *dst, int radix)
{
  VOID(int2str((long) val,dst,(radix == 10 ? -10 : radix)));
  return dst;
}

#endif
