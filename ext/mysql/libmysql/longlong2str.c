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
  Defines: longlong2str();

  longlong2str(dst, radix, val)
  converts the (longlong) integer "val" to character form and moves it to
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

#if defined(HAVE_LONG_LONG) && !defined(longlong2str) && !defined(HAVE_LONGLONG2STR)

extern char NEAR _dig_vec[];

/*
  This assumes that longlong multiplication is faster than longlong division.
*/

char *longlong2str(longlong val,char *dst,int radix)
{
  char buffer[65];
  register char *p;
  long long_val;

  if (radix < 0)
  {
    if (radix < -36 || radix > -2) return (char*) 0;
    if (val < 0) {
      *dst++ = '-';
      val = -val;
    }
    radix = -radix;
  }
  else
  {
    if (radix > 36 || radix < 2) return (char*) 0;
  }
  if (val == 0)
  {
    *dst++='0';
    *dst='\0';
    return dst;
  }
  p = &buffer[sizeof(buffer)-1];
  *p = '\0';

  while ((ulonglong) val > (ulonglong) LONG_MAX)
  {
    ulonglong quo=(ulonglong) val/(uint) radix;
    uint rem= (uint) (val- quo* (uint) radix);
    *--p = _dig_vec[rem];
    val= quo;
  }
  long_val= (long) val;
  while (long_val != 0)
  {
    long quo= long_val/radix;
    *--p = _dig_vec[(uchar) (long_val - quo*radix)];
    long_val= quo;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}

#endif

#ifndef longlong10_to_str
char *longlong10_to_str(longlong val,char *dst,int radix)
{
  char buffer[65];
  register char *p;
  long long_val;

  if (radix < 0)
  {
    if (val < 0)
    {
      *dst++ = '-';
      val = -val;
    }
  }

  if (val == 0)
  {
    *dst++='0';
    *dst='\0';
    return dst;
  }
  p = &buffer[sizeof(buffer)-1];
  *p = '\0';

  while ((ulonglong) val > (ulonglong) LONG_MAX)
  {
    ulonglong quo=(ulonglong) val/(uint) 10;
    uint rem= (uint) (val- quo* (uint) 10);
    *--p = _dig_vec[rem];
    val= quo;
  }
  long_val= (long) val;
  while (long_val != 0)
  {
    long quo= long_val/10;
    *--p = _dig_vec[(uchar) (long_val - quo*10)];
    long_val= quo;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}
#endif
