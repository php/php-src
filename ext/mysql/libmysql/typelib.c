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

/* Functions to handle typelib */

#include "mysys_priv.h"
#include <m_string.h>
#include <m_ctype.h>

/***************************************************************************
** Search after a fieldtype. Endspace in x is not compared.
** If part, uniq field is found and full_name == 0 then x is expanded
** to full field.
** full_name has the following bit values:
**   If & 1 accept only hole names
**   If & 2 don't expand if half field
**   If & 4 allow #number# as type
****************************************************************************/

int find_type(my_string x, TYPELIB *typelib, uint full_name)
{
  int find,pos,findpos;
  reg1 my_string i;
  reg2 const char *j;
  DBUG_ENTER("find_type");
  DBUG_PRINT("enter",("x: '%s'  lib: %lx",x,typelib));

  if (!typelib->count)
  {
    DBUG_PRINT("exit",("no count"));
    DBUG_RETURN(0);
  }
  LINT_INIT(findpos);
  find=0;
  for (pos=0 ; (j=typelib->type_names[pos]) ; pos++)
  {
    for (i=x ; *i && toupper(*i) == toupper(*j) ; i++, j++) ;
    if (! *j)
    {
      while (*i == ' ')
	i++;					/* skipp_end_space */
      if (! *i)
	DBUG_RETURN(pos+1);
    }
    if (! *i)
    {
      find++;
      findpos=pos;
    }
  }
  if (find == 0 && (full_name & 4) && x[0] == '#' && strend(x)[-1] == '#' &&
      (findpos=atoi(x+1)-1) >= 0 && (uint) findpos < typelib->count)
	find=1;
  else if (find == 0 || ! x[0])
  {
    DBUG_PRINT("exit",("Couldn't find type"));
    DBUG_RETURN(0);
  }
  else if (find != 1 || (full_name & 1))
  {
    DBUG_PRINT("exit",("Too many possybilities"));
    DBUG_RETURN(-1);
  }
  if (!(full_name & 2))
    (void) strmov(x,typelib->type_names[findpos]);
  DBUG_RETURN(findpos+1);
} /* find_type */


	/* Get name of type nr 'nr' */
	/* Warning first type is 1, 0 = empty field */

void make_type(register my_string to, register uint nr, register TYPELIB *typelib)
{
  DBUG_ENTER("make_type");
  if (!nr)
    to[0]=0;
  else
    (void) strmov(to,get_type(typelib,nr-1));
  DBUG_VOID_RETURN;
} /* make_type */


	/* Get type */
	/* Warning first type is 0 */

const char *get_type(TYPELIB *typelib, uint nr)
{
  if (nr < (uint) typelib->count && typelib->type_names)
    return(typelib->type_names[nr]);
  return "?";
}
