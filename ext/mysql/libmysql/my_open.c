/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#define USES_TYPES
#include "mysys_priv.h"
#include "mysys_err.h"
#include <my_dir.h>
#include <errno.h>
#if defined(MSDOS) || defined(__WIN__) || defined(__EMX__) || defined(OS2)
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
#if defined(MSDOS) || defined(__WIN__) || defined(__EMX__) || defined(OS2)
  if (Flags & O_SHARE)
    fd = sopen((my_string) FileName, (Flags & ~O_SHARE) | O_BINARY, SH_DENYNO,
	       MY_S_IREAD | MY_S_IWRITE);
  else
    fd = open((my_string) FileName, Flags | O_BINARY,
	      MY_S_IREAD | MY_S_IWRITE);
#elif !defined(NO_OPEN_3)
  fd = open(FileName, Flags, my_umask);	/* Normal unix */
#else
  fd = open((my_string) FileName, Flags);
#endif
  DBUG_RETURN(my_register_filename(fd, FileName, FILE_BY_OPEN,
				   EE_FILENOTFOUND, MyFlags));
} /* my_open */


	/* Close a file */

int my_close(File fd, myf MyFlags)
{
  int err;
  DBUG_ENTER("my_close");
  DBUG_PRINT("my",("fd: %d  MyFlags: %d",fd, MyFlags));

  pthread_mutex_lock(&THR_LOCK_open);
  if ((err = close(fd)))
  {
    DBUG_PRINT("error",("Got error %d on close",err));
    my_errno=errno;
    if (MyFlags & (MY_FAE | MY_WME))
      my_error(EE_BADCLOSE, MYF(ME_BELL+ME_WAITTANG),my_filename(fd),errno);
  }
  if ((uint) fd < MY_NFILE && my_file_info[fd].type != UNOPEN)
  {
    my_free(my_file_info[fd].name, MYF(0));
#if defined(THREAD) && !defined(HAVE_PREAD)
    pthread_mutex_destroy(&my_file_info[fd].mutex);
#endif
    my_file_info[fd].type = UNOPEN;
  }
  my_file_opened--;
  pthread_mutex_unlock(&THR_LOCK_open);
  DBUG_RETURN(err);
} /* my_close */


File my_register_filename(File fd, const char *FileName, enum file_type
			  type_of_file, uint error_message_number, myf MyFlags)
{
  if ((int) fd >= 0)
  {
    if ((int) fd >= MY_NFILE)
    {
#if defined(THREAD) && !defined(HAVE_PREAD)
      (void) my_close(fd,MyFlags);
      my_errno=EMFILE;
      if (MyFlags & (MY_FFNF | MY_FAE | MY_WME))
	my_error(EE_OUT_OF_FILERESOURCES, MYF(ME_BELL+ME_WAITTANG),
		 FileName, my_errno);
      return(-1);
#endif
      thread_safe_increment(my_file_opened,&THR_LOCK_open);
      return(fd);				/* safeguard */
    }
    pthread_mutex_lock(&THR_LOCK_open);
    if ((my_file_info[fd].name = (char*) my_strdup(FileName,MyFlags)))
    {
      my_file_opened++;
      my_file_info[fd].type = type_of_file;
#if defined(THREAD) && !defined(HAVE_PREAD)
      pthread_mutex_init(&my_file_info[fd].mutex,MY_MUTEX_INIT_FAST);
#endif
      pthread_mutex_unlock(&THR_LOCK_open);
      DBUG_PRINT("exit",("fd: %d",fd));
      return(fd);
    }
    pthread_mutex_unlock(&THR_LOCK_open);
    (void) my_close(fd, MyFlags);
    my_errno=ENOMEM;
  }
  else
    my_errno=errno;
  DBUG_PRINT("error",("Got error %d on open",my_errno));
  if (MyFlags & (MY_FFNF | MY_FAE | MY_WME))
    my_error(error_message_number, MYF(ME_BELL+ME_WAITTANG),
	     FileName, my_errno);
  return(fd);
}
