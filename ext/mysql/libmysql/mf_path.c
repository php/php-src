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
#include <m_string.h>

static char *find_file_in_path(char *to,const char *name);

	/* Finds where program can find it's files.
	   pre_pathname is found by first locking at progname (argv[0]).
	   if progname contains path the path is returned.
	   else if progname is found in path, return it
	   else if progname is given and POSIX environment variable "_" is set
	   then path is taken from "_".
	   If filename doesn't contain a path append MY_BASEDIR_VERSION or
	   MY_BASEDIR if defined, else append "/my/running".
	   own_path_name_part is concatinated to result.
	   my_path puts result in to and returns to */

my_string my_path(my_string to, const char *progname,
		  const char *own_pathname_part)
{
  my_string start,end,prog;
  DBUG_ENTER("my_path");

  start=to;					/* Return this */
  if (progname && (dirname_part(to, progname) ||
		   find_file_in_path(to,progname) ||
		   ((prog=getenv("_")) != 0 && dirname_part(to,prog))))
  {
    VOID(intern_filename(to,to));
    if (!test_if_hard_path(to))
    {
      if (!my_getwd(curr_dir,FN_REFLEN,MYF(0)))
	bchange(to,0,curr_dir, (uint) strlen(curr_dir), (uint) strlen(to)+1);
    }
  }
  else
  {
    if ((end = getenv("MY_BASEDIR_VERSION")) == 0 &&
	(end = getenv("MY_BASEDIR")) == 0)
    {
#ifdef DEFAULT_BASEDIR
      end= (char*) DEFAULT_BASEDIR;
#else
      end= (char*) "/my/";
#endif
    }
    VOID(intern_filename(to,end));
    to=strend(to);
    if (to != start && to[-1] != FN_LIBCHAR)
      *to++ = FN_LIBCHAR;
    VOID(strmov(to,own_pathname_part));
  }
  DBUG_PRINT("exit",("to: '%s'",start));
  DBUG_RETURN(start);
} /* my_path */


	/* test if file without filename is found in path */
	/* Returns to if found and to has dirpart if found, else NullS */

#if defined(MSDOS) || defined(__WIN__)
#define F_OK 0
#define PATH_SEP ';'
#define PROGRAM_EXTENSION ".exe"
#else
#define PATH_SEP ':'
#endif

static char *find_file_in_path(char *to, const char *name)
{
  char *path,*pos,dir[2];
  const char *ext="";

  if (!(path=getenv("PATH")))
    return NullS;
  dir[0]=FN_LIBCHAR; dir[1]=0;
#ifdef PROGRAM_EXTENSION
  if (!fn_ext(name)[0])
    ext=PROGRAM_EXTENSION;
#endif

  for (pos=path ; (pos=strchr(pos,PATH_SEP)) ; path= ++pos)
  {
    if (path != pos)
    {
      strxmov(strnmov(to,path,(uint) (pos-path)),dir,name,ext,NullS);
      if (!access(to,F_OK))
      {
	to[(uint) (pos-path)+1]=0;	/* Return path only */
	return to;
      }
    }
  }
#ifdef __WIN__
  to[0]=FN_CURLIB;
  strxmov(to+1,dir,name,ext,NullS);
  if (!access(to,F_OK))			/* Test in current dir */
  {
    to[2]=0;				/* Leave ".\" */
    return to;
  }
#endif
  return NullS;				/* File not found */
}
