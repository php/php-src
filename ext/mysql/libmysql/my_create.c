/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#define USES_TYPES
#include "mysys_priv.h"
#include <my_dir.h>
#include "mysys_err.h"
#include <errno.h>
#if defined(MSDOS) || defined(__WIN__) || defined(__EMX__) || defined(OS2)
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

#if !defined(NO_OPEN_3) && !defined(__EMX__)
  fd = open((my_string) FileName, access_flags | O_CREAT,
	    CreateFlags ? CreateFlags : my_umask);
#elif defined(VMS)
  fd = open((my_string) FileName, access_flags | O_CREAT, 0,
	    "ctx=stm","ctx=bin");
#elif defined(MSDOS) || defined(__WIN__) || defined(__EMX__) || defined(OS2)
  if (access_flags & O_SHARE)
    fd = sopen((my_string) FileName, access_flags | O_CREAT | O_BINARY,
	       SH_DENYNO, MY_S_IREAD | MY_S_IWRITE);
  else
    fd =  open((my_string) FileName, access_flags | O_CREAT | O_BINARY,
	       MY_S_IREAD | MY_S_IWRITE);
#else
  fd = open(FileName, access_flags);
#endif

  DBUG_RETURN(my_register_filename(fd, FileName, FILE_BY_CREATE,
				   EE_CANTCREATEFILE, MyFlags));
} /* my_create */
