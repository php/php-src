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

#define USES_TYPES
#include "mysys_priv.h"
#include <my_dir.h>
#include "mysys_err.h"
#include <errno.h>
#if defined(MSDOS) || defined(__WIN__)
#include <share.h>
#endif

	/*
	** Create a new file
	** Arguments:
	** Path-name of file
	** Read | write on file (umask value)
	** Read & Write on open file
	** Special flags
	*/


File my_create(const char *FileName, int CreateFlags, int access_flags,
	       myf MyFlags)
{
  int fd;
  DBUG_ENTER("my_create");
  DBUG_PRINT("my",("Name: '%s' CreateFlags: %d  AccessFlags: %d  MyFlags: %d",
		   FileName, CreateFlags, access_flags, MyFlags));

#if !defined(NO_OPEN_3)
  fd = open((my_string) FileName, access_flags | O_CREAT,
	    CreateFlags ? CreateFlags : my_umask);
#elif defined(VMS)
  fd = open((my_string) FileName, access_flags | O_CREAT, 0,
	    "ctx=stm","ctx=bin");
#elif defined(MSDOS) || defined(__WIN__)
  if (access_flags & O_SHARE)
    fd = sopen((my_string) FileName, access_flags | O_CREAT | O_BINARY,
	       SH_DENYNO, MY_S_IREAD | MY_S_IWRITE);
  else
    fd =  open((my_string) FileName, access_flags | O_CREAT | O_BINARY,
	       MY_S_IREAD | MY_S_IWRITE);
#else
  fd = open(FileName, access_flags);
#endif

  if (fd >= 0)
  {
    if ((int) fd >= MY_NFILE)
    {
      DBUG_PRINT("exit",("fd: %d",fd));
      DBUG_RETURN(fd);				/* safeguard */
    }
    if ((my_file_info[fd].name = (char*) my_strdup(FileName,MyFlags)))
    {
      my_file_opened++;
      my_file_info[fd].type = FILE_BY_CREATE;
      DBUG_PRINT("exit",("fd: %d",fd));
      DBUG_RETURN(fd);
    }
    VOID(my_close(fd,MyFlags));
    my_errno=ENOMEM;
  }
  else
    my_errno=errno;
  if (MyFlags & (MY_FFNF | MY_FAE | MY_WME))
    my_error(EE_CANTCREATEFILE, MYF(ME_BELL+ME_WAITTANG), FileName,my_errno);
  DBUG_RETURN(-1);
} /* my_create */
