/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
  This function is only used by some old ISAM code.
  When we remove ISAM support from MySQL, we should also delete this file

  One should instead use the functions in mf_tempfile.c
*/

#include "mysys_priv.h"
#include <m_string.h>

/* HPUX 11.0 doesn't allow us to change the environ pointer */
#ifdef HPUX11
#undef HAVE_TEMPNAM
#endif

#include "my_static.h"
#include "mysys_err.h"

#define TMP_EXT ".tmp"				/* Extension of tempfile  */
#if ! defined(P_tmpdir)
#define P_tmpdir ""
#endif

#ifdef HAVE_TEMPNAM
#if !defined( MSDOS) && !defined(OS2) && !defined(__NETWARE__)
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
#ifdef OS2
  /* changing environ variable doesn't work with VACPP */
  char  buffer[256];
  sprintf( buffer, "TMP=%s", dir);
  /* remove ending backslash */
  if (buffer[strlen(buffer)-1] == '\\')
     buffer[strlen(buffer)-1] = '\0';
  putenv( buffer);
#elif !defined(__NETWARE__)
  old_env=(char**)environ;
  if (dir)
  {				/* Don't use TMPDIR if dir is given */
    /*
      The following strange cast is required because the IBM compiler on AIX
      doesn't allow us to cast the value of environ.
      The cast of environ is needed as some systems doesn't allow us to
      update environ with a char ** pointer. (const mismatch)
    */
    (*(char***) &environ)=(char**) temp_env;
    temp_env[0]=0;
  }
#endif
  res=tempnam((char*) dir,(my_string) pfx); /* Use stand. dir with prefix */
#if !defined(OS2) && !defined(__NETWARE__)
  (*(char***) &environ)=(char**) old_env;
#endif
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
