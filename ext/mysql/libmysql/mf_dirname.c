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

#include "mysys_priv.h"
#include <m_string.h>

	/* Functions definied in this file */

uint dirname_length(const char *name)
{
  register my_string pos,gpos;
#ifdef FN_DEVCHAR
  if ((pos=strrchr(name,FN_DEVCHAR)) == 0)
#endif
    pos=(char*) name-1;

  gpos= pos++;
  for ( ; *pos ; pos++)				/* Find last FN_LIBCHAR */
    if (*pos == FN_LIBCHAR || *pos == '/'
#ifdef FN_C_AFTER_DIR
	|| *pos == FN_C_AFTER_DIR || *pos == FN_C_AFTER_DIR_2
#endif
	)
      gpos=pos;
  return ((uint) (uint) (gpos+1-(char*) name));
}


	/* Gives directory part of filename. Directory ends with '/' */
	/* Returns length of directory part */

uint dirname_part(my_string to, const char *name)
{
  uint length;
  DBUG_ENTER("dirname_part");
  DBUG_PRINT("enter",("'%s'",name));

  length=dirname_length(name);
  (void) strmake(to,(char*) name,min(length,FN_REFLEN-2));
  convert_dirname(to);				/* Convert chars */
  DBUG_RETURN(length);
} /* dirname */


	/* convert dirname to use under this system */
	/* If MSDOS converts '/' to '\' */
	/* If VMS converts '<' to '[' and '>' to ']' */
	/* Adds a '/' to end if there isn't one and the last isn't a dev_char */
	/* ARGSUSED */

#ifndef FN_DEVCHAR
#define FN_DEVCHAR '\0'				/* For easier code */
#endif

void convert_dirname(my_string to)
{
#ifdef FN_UPPER_CASE
  caseup_str(to);
#endif
#ifdef FN_LOWER_CASE
  casedn_str(to);
#endif
#if FN_LIBCHAR != '/'
  {
    reg1 my_string pos;
    pos=to-1;					/* Change from '/' */
    while ((pos=strchr(pos+1,'/')) != 0)
      *pos=FN_LIBCHAR;
  }
#endif
#ifdef FN_C_BEFORE_DIR_2
  {
    reg1 my_string pos;
    for (pos=to ; *pos ; pos++)
    {
      if (*pos == FN_C_BEFORE_DIR_2)
	*pos=FN_C_BEFORE_DIR;
      if (*pos == FN_C_AFTER_DIR_2)
	*pos=FN_C_AFTER_DIR;
    }
  }
#else
  {					/* Append FN_LIBCHAR if not there */
    char *end=strend(to);
    if (end != to && (end[-1] != FN_LIBCHAR && end[-1] != FN_DEVCHAR))
    {
      end[0]=FN_LIBCHAR;
      end[1]=0;
    }
  }
#endif
} /* convert_dirname */
