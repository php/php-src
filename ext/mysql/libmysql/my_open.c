/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#define USES_TYPES
#include "mysys_priv.h"
#include "mysys_err.h"
#include <errno.h>
#if defined(MSDOS) || defined(__WIN__)
#include <share.h>
#endif

	/* Open a file */

File my_open(const char *FileName, int Flags, myf MyFlags)
				/* Path-name of file */
				/* Read | write .. */
				/* Special flags */
{
  File fd;
  DBUG_ENTER("my_open");
  DBUG_PRINT("my",("Name: '%s'  Flags: %d  MyFlags: %d",
		   FileName, Flags, MyFlags));
#if defined(MSDOS) || defined(__WIN__)
  if (Flags & O_SHARE)
    fd = sopen((my_string) FileName, (Flags & ~O_SHARE) | O_BINARY, SH_DENYNO);
  else
    fd = open((my_string) FileName, Flags | O_BINARY);
#elif !defined(NO_OPEN_3)
  fd = open(FileName, Flags, 0);	/* Normal unix */
#else
  fd = open((my_string) FileName, Flags);
#endif

  if ((int) fd >= 0)
  {
    if ((int) fd >= MY_NFILE)
      DBUG_RETURN(fd);				/* safeguard */
    pthread_mutex_lock(&THR_LOCK_open);
    if ((my_file_info[fd].name = (char*) my_strdup(FileName,MyFlags)))
    {
      my_file_opened++;
      my_file_info[fd].type = FILE_BY_OPEN;
      pthread_mutex_unlock(&THR_LOCK_open);
      DBUG_PRINT("exit",("fd: %d",fd));
      DBUG_RETURN(fd);
    }
    pthread_mutex_unlock(&THR_LOCK_open);
    (void) my_close(fd,MyFlags);
    my_errno=ENOMEM;
  }
  else
    my_errno=errno;
  DBUG_PRINT("error",("Got error %d on open",my_errno));
  if (MyFlags & (MY_FFNF | MY_FAE | MY_WME))
    my_error(EE_FILENOTFOUND, MYF(ME_BELL+ME_WAITTANG), FileName,my_errno);
  DBUG_RETURN(fd);
} /* my_open */


	/* Close a file */

int my_close(File fd, myf MyFlags)
{
  int err;
  DBUG_ENTER("my_close");
  DBUG_PRINT("my",("fd: %d  MyFlags: %d",fd, MyFlags));

  pthread_mutex_lock(&THR_LOCK_open);
  if ((err = close(fd)) != 0)
  {
    my_errno=errno;
    if (MyFlags & (MY_FAE | MY_WME))
      my_error(EE_BADCLOSE, MYF(ME_BELL+ME_WAITTANG),my_filename(fd),errno);
  }
  if ((uint) fd < MY_NFILE && my_file_info[fd].type != UNOPEN)
  {
    my_file_opened--;
    my_free(my_file_info[fd].name, MYF(0));
    my_file_info[fd].type = UNOPEN;
  }
  pthread_mutex_unlock(&THR_LOCK_open);
  DBUG_RETURN(err);
} /* my_close */
