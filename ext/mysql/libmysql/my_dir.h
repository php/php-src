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

#ifndef _my_dir_h
#define _my_dir_h
#ifdef	__cplusplus
extern "C" {
#endif

#ifndef MY_DIR_H
#define MY_DIR_H

#include <sys/stat.h>

	/* Defines for my_dir and my_stat */

#define MY_S_IFMT	S_IFMT	/* type of file */
#define MY_S_IFDIR	S_IFDIR /* directory */
#define MY_S_IFCHR	S_IFCHR /* character special */
#define MY_S_IFBLK	S_IFBLK /* block special */
#define MY_S_IFREG	S_IFREG /* regular */
#define MY_S_IFIFO	S_IFIFO /* fifo */
#define MY_S_ISUID	S_ISUID /* set user id on execution */
#define MY_S_ISGID	S_ISGID /* set group id on execution */
#define MY_S_ISVTX	S_ISVTX /* save swapped text even after use */
#define MY_S_IREAD	S_IREAD /* read permission, owner */
#define MY_S_IWRITE	S_IWRITE	/* write permission, owner */
#define MY_S_IEXEC	S_IEXEC /* execute/search permission, owner */

#define MY_S_ISDIR(m)	(((m) & MY_S_IFMT) == MY_S_IFDIR)
#define MY_S_ISCHR(m)	(((m) & MY_S_IFMT) == MY_S_IFCHR)
#define MY_S_ISBLK(m)	(((m) & MY_S_IFMT) == MY_S_IFBLK)
#define MY_S_ISREG(m)	(((m) & MY_S_IFMT) == MY_S_IFREG)
#define MY_S_ISFIFO(m)	(((m) & MY_S_IFMT) == MY_S_IFIFO)

#define MY_DONT_SORT	512	/* my_lib; Don't sort files */
#define MY_WANT_STAT	1024	/* my_lib; stat files */

	/* typedefs for my_dir & my_stat */

#ifdef USE_MY_STAT_STRUCT

typedef struct my_stat
{
  dev_t		st_dev;		/* major & minor device numbers */
  ino_t		st_ino;		/* inode number */
  ushort	st_mode;	/* file permissons (& suid sgid .. bits) */
  short		st_nlink;	/* number of links to file */
  ushort	st_uid;		/* user id */
  ushort	st_gid;		/* group id */
  dev_t		st_rdev;	/* more major & minor device numbers (???) */
  off_t		st_size;	/* size of file */
  time_t	st_atime;	/* time for last read */
  time_t	st_mtime;	/* time for last contens modify */
  time_t	st_ctime;	/* time for last inode or contents modify */
} MY_STAT;

#else

#define MY_STAT struct stat	/* Orginal struct have what we need */

#endif /* USE_MY_STAT_STRUCT */

typedef struct fileinfo		/* Struct returned from my_dir & my_stat */
{
  char			*name;
  MY_STAT		mystat;
} FILEINFO;

typedef struct st_my_dir	/* Struct returned from my_dir */
{
  struct fileinfo	*dir_entry;
  uint			number_off_files;
} MY_DIR;

extern MY_DIR *my_dir(const char *path,myf MyFlags);
extern void my_dirend(MY_DIR *buffer);
extern MY_STAT *my_stat(const char *path, MY_STAT *stat_area, myf my_flags);
extern int my_fstat(int filenr, MY_STAT *stat_area, myf MyFlags);

#endif /* MY_DIR_H */

#ifdef	__cplusplus
}
#endif
#endif
