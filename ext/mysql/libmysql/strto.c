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
  strtol,strtoul,strtoll,strtoull
  convert string to long, unsigned long, long long or unsigned long long.
  strtoxx(char *src,char **ptr,int base)
  converts the string pointed to by src to an long of appropriate long and
  returnes it. It skips leading spaces and tabs (but not newlines, formfeeds,
  backspaces), then it accepts an optional sign and a sequence of digits
  in the specified radix.
  If the value of ptr is not (char **)NULL, a pointer to the character
  terminating the scan is returned in the location pointed to by ptr.
  Trailing spaces will NOT be skipped.

  If an error is detected, the result will be LONG_MIN, 0 or LONG_MAX,
  (or LONGLONG..)  and errno will be set to
	EDOM	if there are no digits
	ERANGE	if the result would overflow.
  the ptr will be set to src.
  This file is based on the strtol from the the GNU C Library.
  it can be compiled with the UNSIGNED and/or LONGLONG flag set
*/

#include <global.h>
#include "m_string.h"
#include "m_ctype.h"
#include "my_sys.h"			/* defines errno */
#include <errno.h>

#ifdef LONGLONG
#define UTYPE_MAX (~(ulonglong) 0)
#define TYPE_MIN LONGLONG_MIN
#define TYPE_MAX LONGLONG_MAX
#define longtype longlong
#define ulongtype ulonglong
#ifdef UNSIGNED
#define function ulongtype strtoull
#else
#define function longtype strtoll
#endif
#else
#define UTYPE_MAX (ulong) ~0L
#define TYPE_MIN LONG_MIN
#define TYPE_MAX LONG_MAX
#define longtype long
#define ulongtype unsigned long
#ifdef UNSIGNED
#define function ulongtype strtoul
#else
#define function longtype strtol
#endif
#endif


/* Convert NPTR to an `unsigned long int' or `long int' in base BASE.
   If BASE is 0 the base is determined by the presence of a leading
   zero, indicating octal or a leading "0x" or "0X", indicating hexadecimal.
   If BASE is < 2 or > 36, it is reset to 10.
   If ENDPTR is not NULL, a pointer to the character after the last
   one converted is stored in *ENDPTR.	*/


function (const char *nptr,char **endptr,int base)
{
  int negative;
  register ulongtype cutoff;
  register unsigned int cutlim;
  register ulongtype i;
  register const char *s;
  register unsigned char c;
  const char *save;
  int overflow;

  if (base < 0 || base == 1 || base > 36)
    base = 10;

  s = nptr;

  /* Skip white space.	*/
  while (isspace (*s))
    ++s;
  if (*s == '\0')
  {
    goto noconv;
  }

  /* Check for a sign.	*/
  if (*s == '-')
  {
    negative = 1;
    ++s;
  }
  else if (*s == '+')
  {
    negative = 0;
    ++s;
  }
  else
    negative = 0;

  if (base == 16 && s[0] == '0' && toupper (s[1]) == 'X')
    s += 2;

  /* If BASE is zero, figure it out ourselves.	*/
  if (base == 0)
  {
    if (*s == '0')
    {
      if (toupper (s[1]) == 'X')
      {
	s += 2;
	base = 16;
      }
      else
	base = 8;
    }
    else
      base = 10;
  }

  /* Save the pointer so we can check later if anything happened.  */
  save = s;

  cutoff = UTYPE_MAX / (unsigned long int) base;
  cutlim = (uint) (UTYPE_MAX % (unsigned long int) base);

  overflow = 0;
  i = 0;
  for (c = *s; c != '\0'; c = *++s)
  {
    if (isdigit (c))
      c -= '0';
    else if (isalpha (c))
      c = toupper (c) - 'A' + 10;
    else
      break;
    if (c >= base)
      break;
    /* Check for overflow.  */
    if (i > cutoff || (i == cutoff && c > cutlim))
      overflow = 1;
    else
    {
      i *= (ulongtype) base;
      i += c;
    }
  }

  /* Check if anything actually happened.  */
  if (s == save)
    goto noconv;

  /* Store in ENDPTR the address of one character
     past the last character we converted.  */
  if (endptr != NULL)
    *endptr = (char *) s;

#ifndef UNSIGNED
  /* Check for a value that is within the range of
     `unsigned long int', but outside the range of `long int'.	*/
  if (negative)
  {
    if (i  > (ulongtype) TYPE_MIN)
      overflow = 1;
  }
  else if (i > (ulongtype) TYPE_MAX)
    overflow = 1;
#endif

  if (overflow)
  {
    my_errno=ERANGE;
#ifdef UNSIGNED
    return UTYPE_MAX;
#else
    return negative ? TYPE_MIN : TYPE_MAX;
#endif
  }

  /* Return the result of the appropriate sign.  */
  return (negative ? -((longtype) i) : i);

noconv:
  /* There was no number to convert.  */
  my_errno=EDOM;
  if (endptr != NULL)
    *endptr = (char *) nptr;
  return 0L;
}
