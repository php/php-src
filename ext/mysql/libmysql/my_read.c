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
