/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

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
