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
#include "my_static.h"
#include <errno.h>
#include "mysys_err.h"

static void	make_ftype(my_string to,int flag);

	/* Open a file as stream */

FILE *my_fopen(const char *FileName, int Flags, myf MyFlags)
					/* Path-name of file */
					/* Read | write .. */
					/* Special flags */
{
  FILE *fd;
  char type[5];
  DBUG_ENTER("my_fopen");
  DBUG_PRINT("my",("Name: '%s'  Flags: %d  MyFlags: %d",
		   FileName, Flags, MyFlags));

  make_ftype(type,Flags);
  if ((fd = fopen(FileName, type)) != 0)
  {
    /*
      The test works if MY_NFILE < 128. The problem is that fileno() is char
      on some OS (SUNOS). Actually the filename save isn't that important
      so we can ignore if this doesn't work.
    */
    if ((uint) fileno(fd) >= MY_NFILE)
      DBUG_RETURN(fd);				/* safeguard */
    pthread_mutex_lock(&THR_LOCK_open);
    if ((my_file_info[fileno(fd)].name = (char*)
	 my_strdup(FileName,MyFlags)))
    {
      my_stream_opened++;
      my_file_info[fileno(fd)].type = STREAM_BY_FOPEN;
      pthread_mutex_unlock(&THR_LOCK_open);
      DBUG_PRINT("exit",("stream: %lx",fd));
      DBUG_RETURN(fd);
    }
    pthread_mutex_unlock(&THR_LOCK_open);
    (void) my_fclose(fd,MyFlags);
    my_errno=ENOMEM;
  }
  else
    my_errno=errno;
  DBUG_PRINT("error",("Got error %d on open",my_errno));
  if (MyFlags & (MY_FFNF | MY_FAE | MY_WME))
    my_error((Flags & O_RDONLY) || (Flags == O_RDONLY ) ? EE_FILENOTFOUND :
	     EE_CANTCREATEFILE,
	     MYF(ME_BELL+ME_WAITTANG), FileName,my_errno);
  DBUG_RETURN((FILE*) 0);
} /* my_fopen */


	/* Close a stream */

int my_fclose(FILE *fd, myf MyFlags)
{
  int err,file;
  DBUG_ENTER("my_fclose");
  DBUG_PRINT("my",("stream: %lx  MyFlags: %d",fd, MyFlags));

  pthread_mutex_lock(&THR_LOCK_open);
  file=fileno(fd);
  if ((err = fclose(fd)) < 0)
  {
    my_errno=errno;
    if (MyFlags & (MY_FAE | MY_WME))
      my_error(EE_BADCLOSE, MYF(ME_BELL+ME_WAITTANG),
	       my_filename(file),errno);
  }
  if ((uint) file < MY_NFILE && my_file_info[file].type != UNOPEN)
  {
    my_file_info[file].type = UNOPEN;
    my_free(my_file_info[file].name, MYF(0));
    my_stream_opened--;
  }
  pthread_mutex_unlock(&THR_LOCK_open);
  DBUG_RETURN(err);
} /* my_fclose */


	/* Make a stream out of a file handle */

FILE *my_fdopen(File Filedes, int Flags, myf MyFlags)

				/* Read | write .. */
				/* Special flags */
{
  FILE *fd;
  char type[5];
  DBUG_ENTER("my_fdopen");
  DBUG_PRINT("my",("Fd: %d  Flags: %d  MyFlags: %d",
		   Filedes, Flags, MyFlags));

  make_ftype(type,Flags);
  if ((fd = fdopen(Filedes, type)) == 0)
  {
    my_errno=errno;
    if (MyFlags & (MY_FAE | MY_WME))
      my_error(EE_CANT_OPEN_STREAM, MYF(ME_BELL+ME_WAITTANG),errno);
  }
  else
  {
    pthread_mutex_lock(&THR_LOCK_open);
    if (my_file_info[Filedes].type != UNOPEN)
    {
      my_file_info[Filedes].type = STREAM_BY_FDOPEN;
      my_file_opened--;			/* File is opened with my_open ! */
      my_stream_opened++;
    }
    pthread_mutex_unlock(&THR_LOCK_open);
  }

  DBUG_PRINT("exit",("stream: %lx",fd));
  DBUG_RETURN(fd);
} /* my_fdopen */


	/* Make a filehandler-open-typestring from ordinary inputflags */

static void make_ftype(register my_string to, register int flag)
{
#if FILE_BINARY					/* If we have binary-files */
  reg3 int org_flag=flag;
#endif
  flag&= ~FILE_BINARY;				/* remove binary bit */
  if (flag == O_RDONLY)
    *to++= 'r';
  else if (flag == O_WRONLY)
    *to++= 'w';
  else
  {						/* Add '+' after theese */
    if (flag == O_RDWR)
      *to++= 'r';
    else if (flag & O_APPEND)
      *to++= 'a';
    else
      *to++= 'w';				/* Create file */
    *to++= '+';
  }
#if FILE_BINARY					/* If we have binary-files */
  if (org_flag & FILE_BINARY)
    *to++='b';
#endif
  *to='\0';
} /* make_ftype */
