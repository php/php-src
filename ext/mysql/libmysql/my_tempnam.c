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
#include "my_static.h"
#include "mysys_err.h"

#define TMP_EXT ".tmp"				/* Extension of tempfile  */
#if ! defined(P_tmpdir)
#define P_tmpdir ""
#endif

#ifdef HAVE_TEMPNAM
#ifndef MSDOS
extern char **environ;
#endif
#endif

/* Make a uniq temp file name by using dir and adding something after
   pfx to make name uniq. Name is made by adding a uniq 8 length-string and
   TMP_EXT after pfx.
   Returns pointer to malloced area for filename. Should be freed by
   free().
   The name should be uniq, but it isn't checked if it file allready exists.
   Uses tempnam() if function exist on system.
   This function fixes that if dir is given it's used. For example
   MSDOS tempnam() uses always TMP environment-variable if it exists.
*/
	/* ARGSUSED */

my_string my_tempnam(const char *dir, const char *pfx,
		     myf MyFlags  __attribute__((unused)))
{
#ifdef _MSC_VER
  char temp[FN_REFLEN],*end,*res,**old_env,*temp_env[1];
  old_env=environ;
  if (dir)
  {
    end=strend(dir)-1;
    if (!dir[0])
    {				/* Change empty string to current dir */
      temp[0]= FN_CURLIB;
      temp[1]= 0;
      dir=temp;
    }
    else if (*end == FN_DEVCHAR)
    {				/* Get current dir for drive */
      _fullpath(temp,dir,FN_REFLEN);
      dir=temp;
    }
    else if (*end == FN_LIBCHAR && dir < end && end[-1] != FN_DEVCHAR)
    {
      strmake(temp,dir,(uint) (end-dir));	/* Copy and remove last '\' */
      dir=temp;
    }
    environ=temp_env;		/* Force use of dir (dir not checked) */
    temp_env[0]=0;
  }
  res=tempnam((char*) dir,(my_string) pfx);
  environ=old_env;
  return res;
#else
#ifdef __ZTC__
  if (!dir)
  {				/* If empty test first if TMP can be used */
    dir=getenv("TMP");
  }
  return tempnam((char*) dir,(my_string) pfx); /* Use stand. dir with prefix */
#else
#ifdef HAVE_TEMPNAM
  char temp[2],*res,**old_env,*temp_env[1];

  if (dir && !dir[0])
  {				/* Change empty string to current dir */
    temp[0]= FN_CURLIB;
    temp[1]= 0;
    dir=temp;
  }
  old_env=environ;
  if (dir)
  {				/* Don't use TMPDIR if dir is given */
    environ=temp_env;
    temp_env[0]=0;
  }
  res=tempnam((char*) dir,(my_string) pfx); /* Use stand. dir with prefix */
  environ=old_env;
  if (!res)
    DBUG_PRINT("error",("Got error: %d from tempnam",errno));
  return res;
#else
  register long uniq;
  register int length;
  my_string pos,end_pos;
  DBUG_ENTER("my_tempnam");
					/* Make a uniq nummber */
  pthread_mutex_lock(&THR_LOCK_open);
  uniq= ((long) getpid() << 20) + (long) _my_tempnam_used++ ;
  pthread_mutex_unlock(&THR_LOCK_open);
  if (!dir && !(dir=getenv("TMPDIR")))	/* Use this if possibly */
    dir=P_tmpdir;			/* Use system default */
  length=strlen(dir)+strlen(pfx)+1;

  DBUG_PRINT("test",("mallocing %d byte",length+8+sizeof(TMP_EXT)+1));
  if (!(pos=(char*) malloc(length+8+sizeof(TMP_EXT)+1)))
  {
    if (MyFlags & MY_FAE+MY_WME)
      my_error(EE_OUTOFMEMORY, MYF(ME_BELL+ME_WAITTANG),
	       length+8+sizeof(TMP_EXT)+1);
    DBUG_RETURN(NullS);
  }
  end_pos=strmov(pos,dir);
  if (end_pos != pos && end_pos[-1] != FN_LIBCHAR)
    *end_pos++=FN_LIBCHAR;
  end_pos=strmov(end_pos,pfx);

  for (length=0 ; length < 8 && uniq ; length++)
  {
    *end_pos++= _dig_vec[(int) (uniq & 31)];
    uniq >>= 5;
  }
  VOID(strmov(end_pos,TMP_EXT));
  DBUG_PRINT("exit",("tempnam: '%s'",pos));
  DBUG_RETURN(pos);
#endif /* HAVE_TEMPNAM */
#endif /* __ZTC__ */
#endif /* _MSC_VER */
} /* my_tempnam */
