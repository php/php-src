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
  str2int(src, radix, lower, upper, &val)
  converts the string pointed to by src to an integer and stores it in
  val.	It skips leading spaces and tabs (but not newlines, formfeeds,
  backspaces), then it accepts an optional sign and a sequence of digits
  in the specified radix.  The result should satisfy lower <= *val <= upper.
  The result is a pointer to the first character after the number;
  trailing spaces will NOT be skipped.

  If an error is detected, the result will be NullS, the value put
  in val will be 0, and errno will be set to
	EDOM	if there are no digits
	ERANGE	if the result would overflow or otherwise fail to lie
		within the specified bounds.
  Check that the bounds are right for your machine.
  This looks amazingly complicated for what you probably thought was an
  easy task.  Coping with integer overflow and the asymmetric range of
  twos complement machines is anything but easy.

  So that users of atoi and atol can check whether an error occured,
  I have taken a wholly unprecedented step: errno is CLEARED if this
  call has no problems.
*/

#include <global.h>
#include "m_string.h"
#include "m_ctype.h"
#include "my_sys.h"			/* defines errno */
#include <errno.h>

#define char_val(X) (X >= '0' && X <= '9' ? X-'0' :\
		     X >= 'A' && X <= 'Z' ? X-'A'+10 :\
		     X >= 'a' && X <= 'z' ? X-'a'+10 :\
		     '\177')

char *str2int(register const char *src, register int radix, long int lower, long int upper, long int *val)
{
  int sign;			/* is number negative (+1) or positive (-1) */
  int n;			/* number of digits yet to be converted */
  long limit;			/* "largest" possible valid input */
  long scale;			/* the amount to multiply next digit by */
  long sofar;			/* the running value */
  register int d;		/* (negative of) next digit */
  char *start;
  int digits[32];		/* Room for numbers */

  /*  Make sure *val is sensible in case of error  */

  *val = 0;

  /*  Check that the radix is in the range 2..36  */

#ifndef DBUG_OFF
  if (radix < 2 || radix > 36) {
    errno=EDOM;
    return NullS;
  }
#endif

  /*  The basic problem is: how do we handle the conversion of
      a number without resorting to machine-specific code to
      check for overflow?  Obviously, we have to ensure that
      no calculation can overflow.  We are guaranteed that the
      "lower" and "upper" arguments are valid machine integers.
      On sign-and-magnitude, twos-complement, and ones-complement
      machines all, if +|n| is representable, so is -|n|, but on
      twos complement machines the converse is not true.  So the
      "maximum" representable number has a negative representative.
      Limit is set to min(-|lower|,-|upper|); this is the "largest"
      number we are concerned with.	*/

  /*  Calculate Limit using Scale as a scratch variable  */

  if ((limit = lower) > 0) limit = -limit;
  if ((scale = upper) > 0) scale = -scale;
  if (scale < limit) limit = scale;

  /*  Skip leading spaces and check for a sign.
      Note: because on a 2s complement machine MinLong is a valid
      integer but |MinLong| is not, we have to keep the current
      converted value (and the scale!) as *negative* numbers,
      so the sign is the opposite of what you might expect.
      */
  while (isspace(*src)) src++;
  sign = -1;
  if (*src == '+') src++; else
    if (*src == '-') src++, sign = 1;

  /*  Skip leading zeros so that we never compute a power of radix
      in scale that we won't have a need for.  Otherwise sticking
      enough 0s in front of a number could cause the multiplication
      to overflow when it neededn't.
      */
  start=(char*) src;
  while (*src == '0') src++;

  /*  Move over the remaining digits.  We have to convert from left
      to left in order to avoid overflow.  Answer is after last digit.
      */

  for (n = 0; (digits[n]=char_val(*src)) < radix && n < 20; n++,src++) ;

  /*  Check that there is at least one digit  */

  if (start == src) {
    errno=EDOM;
    return NullS;
  }

  /*  The invariant we want to maintain is that src is just
      to the right of n digits, we've converted k digits to
      sofar, scale = -radix**k, and scale < sofar < 0.	Now
      if the final number is to be within the original
      Limit, we must have (to the left)*scale+sofar >= Limit,
      or (to the left)*scale >= Limit-sofar, i.e. the digits
      to the left of src must form an integer <= (Limit-sofar)/(scale).
      In particular, this is true of the next digit.  In our
      incremental calculation of Limit,

      IT IS VITAL that (-|N|)/(-|D|) = |N|/|D|
      */

  for (sofar = 0, scale = -1; --n >= 1;)
  {
    if ((long) -(d=digits[n]) < limit) {
      errno=ERANGE;
      return NullS;
    }
    limit = (limit+d)/radix, sofar += d*scale; scale *= radix;
  }
  if (n == 0)
  {
    if ((long) -(d=digits[n]) < limit)		/* get last digit */
    {
      errno=ERANGE;
      return NullS;
    }
    sofar+=d*scale;
  }

  /*  Now it might still happen that sofar = -32768 or its equivalent,
      so we can't just multiply by the sign and check that the result
      is in the range lower..upper.  All of this caution is a right
      pain in the neck.  If only there were a standard routine which
      says generate thus and such a signal on integer overflow...
      But not enough machines can do it *SIGH*.
      */
  if (sign < 0)
  {
    if (sofar < -LONG_MAX || (sofar= -sofar) > upper)
    {
      errno=ERANGE;
      return NullS;
    }
  }
  else if (sofar < lower)
  {
    errno=ERANGE;
    return NullS;
  }
  *val = sofar;
  errno=0;			/* indicate that all went well */
  return (char*) src;
}

	/* Theese are so slow compared with ordinary, optimized atoi */

#ifdef WANT_OUR_ATOI

int atoi(const char *src)
{
  long val;
  str2int(src, 10, (long) INT_MIN, (long) INT_MAX, &val);
  return (int) val;
}


long atol(const char *src)
{
  long val;
  str2int(src, 10, LONG_MIN, LONG_MAX, &val);
  return val;
}

#endif /* WANT_OUR_ATOI */
