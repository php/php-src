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

/* my_setwd() and my_getwd() works with intern_filenames !! */

#include "mysys_priv.h"
#include <m_string.h>
#include "mysys_err.h"
#ifdef HAVE_GETWD
#include <sys/param.h>
#endif
#if defined(MSDOS) || defined(__WIN__)
#include <m_ctype.h>
#include <dos.h>
#include <direct.h>
#endif


	/* Gets current working directory in buff. Directory is allways ended
	   with FN_LIBCHAR */
	/* One must pass a buffer to my_getwd. One can allways use
	   curr_dir[] */

int my_getwd(my_string buf, uint size, myf MyFlags)
{
  my_string pos;
  DBUG_ENTER("my_getwd");
  DBUG_PRINT("my",("buf: %lx  size: %d  MyFlags %d", buf,size,MyFlags));

#if ! defined(MSDOS)
  if (curr_dir[0])				/* Current pos is saved here */
    VOID(strmake(buf,&curr_dir[0],size-1));
  else
#endif
  {
#if defined(HAVE_GETCWD)
    if (!getcwd(buf,size-2) && MyFlags & MY_WME)
    {
      my_errno=errno;
      my_error(EE_GETWD,MYF(ME_BELL+ME_WAITTANG),errno);
      return(-1);
    }
#elif defined(HAVE_GETWD)
    {
      char pathname[MAXPATHLEN];
      getwd(pathname);
      strmake(buf,pathname,size-1);
    }
#elif defined(VMS)
    if (!getcwd(buf,size-2,1) && MyFlags & MY_WME)
    {
      my_errno=errno;
      my_error(EE_GETWD,MYF(ME_BELL+ME_WAITTANG),errno);
      return(-1);
    }
    intern_filename(buf,buf);
#else
#error "No way to get current directory"
#endif
    if (*((pos=strend(buf))-1) != FN_LIBCHAR)  /* End with FN_LIBCHAR */
    {
      pos[0]= FN_LIBCHAR;
      pos[1]=0;
    }
    (void) strmake(&curr_dir[0],buf,(size_s) (FN_REFLEN-1));
  }
  DBUG_RETURN(0);
} /* my_getwd */


	/* Set new working directory */

int my_setwd(const char *dir, myf MyFlags)
{
  int res;
  size_s length;
  my_string start,pos;
#if defined(VMS) || defined(MSDOS)
  char buff[FN_REFLEN];
#endif
  DBUG_ENTER("my_setwd");
  DBUG_PRINT("my",("dir: '%s'  MyFlags %d", dir, MyFlags));

  start=(my_string) dir;
#if defined(MSDOS)		/* MSDOS chdir can't change drive */
#if !defined(_DDL) && !defined(WIN32)
  if ((pos=strchr(dir,FN_DEVCHAR)) != 0)
  {
    uint drive,drives;

    pos++;				/* Skipp FN_DEVCHAR */
    drive=(uint) (toupper(dir[0])-'A'+1); drives= (uint) -1;
    if ((pos-(byte*) dir) == 2 && drive > 0 && drive < 32)
    {
      _dos_setdrive(drive,&drives);
      _dos_getdrive(&drives);
    }
    if (drive != drives)
    {
      *pos='\0';			/* Dir is now only drive */
      my_errno=errno;
      my_error(EE_SETWD,MYF(ME_BELL+ME_WAITTANG),dir,ENOENT);
      DBUG_RETURN(-1);
    }
    dir=pos;				/* drive changed, change now path */
  }
#endif
  if (*((pos=strend(dir)-1)) == FN_LIBCHAR && pos != dir)
  {
    strmov(buff,dir)[-1]=0;			/* Remove last '/' */
    dir=buff;
  }
#endif /* MSDOS*/
  if (! dir[0] || (dir[0] == FN_LIBCHAR && dir[1] == 0))
    dir=FN_ROOTDIR;
#ifdef VMS
  {
    pos=strmov(buff,dir);
    if (pos[-1] != FN_LIBCHAR)
    {
      pos[0]=FN_LIBCHAR;		/* Mark as directory */
      pos[1]=0;
    }
    system_filename(buff,buff);		/* Change to VMS format */
    dir=buff;
  }
#endif /* VMS */
  if ((res=chdir((char*) dir)) != 0)
  {
    my_errno=errno;
    if (MyFlags & MY_WME)
      my_error(EE_SETWD,MYF(ME_BELL+ME_WAITTANG),start,errno);
  }
  else
  {
    if (test_if_hard_path(start))
    {						/* Hard pathname */
      pos=strmake(&curr_dir[0],start,(size_s) FN_REFLEN-1);
      if (pos[-1] != FN_LIBCHAR)
      {
	length=(uint) (pos-(char*) curr_dir);
	curr_dir[length]=FN_LIBCHAR;		/* must end with '/' */
	curr_dir[length+1]='\0';
      }
    }
    else
      curr_dir[0]='\0';				/* Don't save name */
  }
  DBUG_RETURN(res);
} /* my_setwd */



	/* Test if hard pathname */
	/* Returns 1 if dirname is a hard path */

int test_if_hard_path(register const char *dir_name)
{
  if (dir_name[0] == FN_HOMELIB && dir_name[1] == FN_LIBCHAR)
    return (home_dir != NullS && test_if_hard_path(home_dir));
  if (dir_name[0] == FN_LIBCHAR)
    return (TRUE);
#ifdef FN_DEVCHAR
  return (strchr(dir_name,FN_DEVCHAR) != 0);
#else
  return FALSE;
#endif
} /* test_if_hard_path */
