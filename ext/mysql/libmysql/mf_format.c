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
#ifdef HAVE_REALPATH
#include <sys/param.h>
#include <sys/stat.h>
#endif

	/* format a filename with replace of library and extension */
	/* params to and name may be identicall */
	/* function doesn't change name if name != to */
	/* Flag may be: 1   replace filenames library with 'dsk' */
	/*		2   replace extension with 'form' */
	/*		4   Unpack filename (replace ~ with home) */
	/*		8   Pack filename as short as possibly */
	/*		16  Resolve symbolic links for filename */
	/*		32  Resolve filename to full path */

#ifdef MAXPATHLEN
#define BUFF_LEN MAXPATHLEN
#else
#define BUFF_LEN FN_LEN
#endif

my_string fn_format(my_string to, const char *name, const char *dsk,
		    const char *form, int flag)
{
  reg1 uint length;
  char dev[FN_REFLEN], buff[BUFF_LEN], *pos, *startpos;
  const char *ext;
  DBUG_ENTER("fn_format");
  DBUG_PRINT("enter",("name: %s  dsk: %s  form: %s  flag: %d",
		       name,dsk,form,flag));

	/* Kopiera & skippa enheten */
  name+=(length=dirname_part(dev,(startpos=(my_string) name)));
  if (length == 0 || flag & 1)
  {
    (void) strmov(dev,dsk);			/* Use given directory */
    convert_dirname(dev);			/* Fix to this OS */
  }
  if (flag & 8)
    pack_dirname(dev,dev);			/* Put in ./.. and ~/.. */
  if (flag & 4)
    (void) unpack_dirname(dev,dev);		/* Replace ~/.. with dir */
  if ((pos=strchr(name,FN_EXTCHAR)) != NullS)
  {
    if ((flag & 2) == 0)			/* Skall vi byta extension ? */
    {
      length=strlength(name);			/* Old extension */
      ext = "";
    }
    else
    {
      length=(uint) (pos-(char*) name);		/* Change extension */
      ext= form;
    }
  }
  else
  {
    length=strlength(name);			/* Har ingen ext- tag nya */
    ext=form;
  }

  if (strlen(dev)+length+strlen(ext) >= FN_REFLEN || length >= FN_LEN )
  {				/* To long path, return original */
    uint tmp_length=strlength(startpos);
    DBUG_PRINT("error",("dev: '%s'  ext: '%s'  length: %d",dev,ext,length));
    (void) strmake(to,startpos,min(tmp_length,FN_REFLEN-1));
  }
  else
  {
    if (to == startpos)
    {
      bmove(buff,(char*) name,length);		/* Save name for last copy */
      name=buff;
    }
    (void) strmov(strnmov(strmov(to,dev),name,length),ext);
#ifdef FN_UPPER_CASE
    caseup_str(to);
#endif
#ifdef FN_LOWER_CASE
    casedn_str(to);
#endif
  }
  /* Purify gives a lot of UMR errors when using realpath */
#if defined(HAVE_REALPATH) && !defined(HAVE_purify)
  if (flag & 16)
  {
    struct stat stat_buff;
    if (flag & 32 || (!lstat(to,&stat_buff) && S_ISLNK(stat_buff.st_mode)))
    {
      if (realpath(to,buff))
	strmov(to,buff);
    }
  }
#endif
  DBUG_RETURN (to);
} /* fn_format */


	/*
	  strlength(const string str)
	  Return length of string with end-space:s not counted.
	  */

size_s strlength(const char *str)
{
  reg1 my_string pos;
  reg2 my_string found;
  DBUG_ENTER("strlength");

  pos=found=(char*) str;

  while (*pos)
  {
    if (*pos != ' ')
    {
      while (*++pos && *pos != ' ') {};
      if (!*pos)
      {
	found=pos;			/* String ends here */
	break;
      }
    }
    found=pos;
    while (*++pos == ' ') {};
  }
  DBUG_RETURN((size_s) (found-(char*) str));
} /* strlength */
