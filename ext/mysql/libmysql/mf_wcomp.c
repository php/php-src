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

/* Funktions for comparing with wild-cards */

#include "mysys_priv.h"

	/* Test if a string is "comparable" to a wild-card string */
	/* returns 0 if the strings are "comparable" */

char wild_many='*';
char wild_one='?';
char wild_prefix=0;

int wild_compare(register const char *str, register const char *wildstr)
{
  reg3 int flag;
  DBUG_ENTER("wild_compare");

  while (*wildstr)
  {
    while (*wildstr && *wildstr != wild_many && *wildstr != wild_one)
    {
      if (*wildstr == wild_prefix && wildstr[1])
	wildstr++;
      if (*wildstr++ != *str++) DBUG_RETURN(1);
    }
    if (! *wildstr ) DBUG_RETURN (*str != 0);
    if (*wildstr++ == wild_one)
    {
      if (! *str++) DBUG_RETURN (1);	/* One char; skipp */
    }
    else
    {						/* Found '*' */
      if (!*wildstr) DBUG_RETURN(0);		/* '*' as last char: OK */
      flag=(*wildstr != wild_many && *wildstr != wild_one);
      do
      {
	if (flag)
	{
	  char cmp;
	  if ((cmp= *wildstr) == wild_prefix && wildstr[1])
	    cmp=wildstr[1];
	  while (*str && *str != cmp)
	    str++;
	  if (!*str) DBUG_RETURN (1);
	}
	if (wild_compare(str,wildstr) == 0) DBUG_RETURN (0);
      } while (*str++ && wildstr[0] != wild_many);
      DBUG_RETURN(1);
    }
  }
  DBUG_RETURN (*str != '\0');
} /* wild_compare */
