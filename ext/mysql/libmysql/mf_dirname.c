/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include <m_string.h>

	/* Functions definied in this file */

uint dirname_length(const char *name)
{
  register my_string pos,gpos;
#ifdef FN_DEVCHAR
  if ((pos=(char*)strrchr(name,FN_DEVCHAR)) == 0)
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

char *convert_dirname(my_string to)
{
  reg1 char *pos;
#ifdef FN_UPPER_CASE
  caseup_str(to);
#endif
#ifdef FN_LOWER_CASE
  casedn_str(to);
#endif
#if FN_LIBCHAR != '/'
  {
    pos=to-1;					/* Change from '/' */
    while ((pos=strchr(pos+1,'/')) != 0)
      *pos=FN_LIBCHAR;
  }
#endif
#ifdef FN_C_BEFORE_DIR_2
  {
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
    pos=strend(to);
    if (pos != to && (pos[-1] != FN_LIBCHAR && pos[-1] != FN_DEVCHAR))
    {
      *pos++=FN_LIBCHAR;
      *pos=0;
    }
  }
#endif
  return pos;					/* Pointer to end of dir */
} /* convert_dirname */
