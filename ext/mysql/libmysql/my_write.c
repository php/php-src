/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include "mysys_err.h"
#include <errno.h>


	/* Write a chunk of bytes to a file */

uint my_write(int Filedes, const byte *Buffer, uint Count, myf MyFlags)
{
  uint writenbytes,errors;
  ulong written;
  DBUG_ENTER("my_write");
  DBUG_PRINT("my",("Fd: %d  Buffer: %lx  Count: %d  MyFlags: %d",
		   Filedes, Buffer, Count, MyFlags));
  errors=0; written=0L;

  for (;;)
  {
    if ((writenbytes = (uint) write(Filedes, Buffer, Count)) == Count)
      break;
    if ((int) writenbytes != -1)
    {						/* Safeguard */
      written+=writenbytes;
      Buffer+=writenbytes;
      Count-=writenbytes;
    }
    my_errno=errno;
    DBUG_PRINT("error",("Write only %d bytes, error: %d",
			writenbytes,my_errno));
#ifndef NO_BACKGROUND
#ifdef THREAD
    if (my_thread_var->abort)
      MyFlags&= ~ MY_WAIT_IF_FULL;		/* End if aborted by user */
#endif
    if (my_errno == ENOSPC && (MyFlags & MY_WAIT_IF_FULL) &&
	(uint) writenbytes != (uint) -1)
    {
      if (!(errors++ % MY_WAIT_GIVE_USER_A_MESSAGE))
	my_error(EE_DISK_FULL,MYF(ME_BELL | ME_NOREFRESH),
		 my_filename(Filedes));
      VOID(sleep(MY_WAIT_FOR_USER_TO_FIX_PANIC));
      continue;
    }
    if (!writenbytes)
    {
      /* We may come here on an interrupt or if the file quote is exeeded */
      if (my_errno == EINTR)
	continue;
      if (!errors++)				/* Retry once */
      {
	errno=EFBIG;				/* Assume this is the error */
	continue;
      }
    }
    else if ((uint) writenbytes != (uint) -1)
      continue;					/* Retry */
#endif
    if (MyFlags & (MY_NABP | MY_FNABP))
    {
      if (MyFlags & (MY_WME | MY_FAE | MY_FNABP))
      {
	my_error(EE_WRITE, MYF(ME_BELL+ME_WAITTANG),
		 my_filename(Filedes),my_errno);
      }
      DBUG_RETURN(MY_FILE_ERROR);		/* Error on read */
    }
    else
      break;					/* Return bytes written */
  }
  if (MyFlags & (MY_NABP | MY_FNABP))
    DBUG_RETURN(0);			/* Want only errors */
  DBUG_RETURN(writenbytes+written);
} /* my_write */
