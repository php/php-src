/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include "mysys_err.h"
#include <errno.h>


	/* Read a chunk of bytes from a file  */

uint my_read(File Filedes, byte *Buffer, uint Count, myf MyFlags)
				/* File descriptor */
				/* Buffer must be at least count bytes */
				/* Max number of bytes returnd */
				/* Flags on what to do on error */
{
  uint readbytes;
  DBUG_ENTER("my_read");
  DBUG_PRINT("my",("Fd: %d  Buffer: %lx  Count: %u  MyFlags: %d",
		   Filedes, Buffer, Count, MyFlags));

  for (;;)
  {
    errno=0;					/* Linux doesn't reset this */
    if ((readbytes = (uint) read(Filedes, Buffer, Count)) != Count)
    {
      my_errno=errno ? errno : -1;
      DBUG_PRINT("warning",("Read only %ld bytes off %ld from %d, errno: %d",
			    readbytes,Count,Filedes,my_errno));
#ifdef THREAD
      if (readbytes == 0 && errno == EINTR)
	continue;				/* Interrupted */
#endif
      if (MyFlags & (MY_WME | MY_FAE | MY_FNABP))
      {
	if ((int) readbytes == -1)
	  my_error(EE_READ, MYF(ME_BELL+ME_WAITTANG),
		   my_filename(Filedes),my_errno);
	else if (MyFlags & (MY_NABP | MY_FNABP))
	  my_error(EE_EOFERR, MYF(ME_BELL+ME_WAITTANG),
		   my_filename(Filedes),my_errno);
      }
      if ((int) readbytes == -1 || (MyFlags & (MY_FNABP | MY_NABP)))
	DBUG_RETURN(MY_FILE_ERROR);	/* Return with error */
    }

    if (MyFlags & (MY_NABP | MY_FNABP))
      readbytes=0;			/* Ok on read */
    break;
  }
  DBUG_RETURN(readbytes);
} /* my_read */
