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
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef VMS
#include <rms.h>
#include <iodef.h>
#include <descrip.h>
#endif /* VMS */

static my_string NEAR_F expand_tilde(my_string *path);

	/* Pack a dirname ; Changes HOME to ~/ and current dev to ./ */
	/* from is a dirname (from dirname() ?) ending with FN_LIBCHAR */

void pack_dirname(my_string to, const char *from)

						/* to may be == from */
{
  int cwd_err;
  uint d_length,length,buff_length;
  my_string start;
  char buff[FN_REFLEN];
  DBUG_ENTER("pack_dirname");

  (void) intern_filename(to,from);		/* Change to intern name */

#ifdef FN_DEVCHAR
  if ((start=strrchr(to,FN_DEVCHAR)) != 0)	/* Skipp device part */
    start++;
  else
#endif
    start=to;

  LINT_INIT(buff_length);
  if (!(cwd_err= my_getwd(buff,FN_REFLEN,MYF(0))))
  {
    buff_length= (uint) strlen(buff);
    d_length=(uint) (start-to);
    if ((start == to ||
	 (buff_length == d_length && !bcmp(buff,start,d_length))) &&
	*start != FN_LIBCHAR && *start)
    {						/* Put current dir before */
      bchange(to,d_length,buff,buff_length,(uint) strlen(to)+1);
    }
  }

  if ((d_length= cleanup_dirname(to,to)) != 0)
  {
    length=0;
    if (home_dir)
    {
      length= (uint) strlen(home_dir);
      if (home_dir[length-1] == FN_LIBCHAR)
	length--;				/* Don't test last '/' */
    }
    if (length > 1 && length < d_length)
    {						/* test if /xx/yy -> ~/yy */
      if (bcmp(to,home_dir,length) == 0 && to[length] == FN_LIBCHAR)
      {
	to[0]=FN_HOMELIB;			/* Filename begins with ~ */
	(void) strmov_overlapp(to+1,to+length);
      }
    }
    if (! cwd_err)
    {						/* Test if cwd is ~/... */
      if (length > 1 && length < buff_length)
      {
	if (bcmp(buff,home_dir,length) == 0 && buff[length] == FN_LIBCHAR)
	{
	  buff[0]=FN_HOMELIB;
	  (void) strmov_overlapp(buff+1,buff+length);
	}
      }
      if (is_prefix(to,buff))
      {
	length= (uint) strlen(buff);
	if (to[length])
	  (void) strmov_overlapp(to,to+length);	/* Remove everything before */
	else
	{
	  to[0]= FN_CURLIB;			/* Put ./ instead of cwd */
	  to[1]= FN_LIBCHAR;
	  to[2]= '\0';
	}
      }
    }
  }
  DBUG_PRINT("exit",("to: '%s'",to));
  DBUG_VOID_RETURN;
} /* pack_dirname */


	/* remove unwanted chars from dirname */
	/* if "/../" removes prev dir; "/~/" removes all before ~ */
	/* "//" is same as "/", except on Win32 at start of a file  */
	/* "/./" is removed */
	/* Unpacks home_dir if "~/.." used */
	/* Unpacks current dir if if "./.." used */

uint cleanup_dirname(register my_string to, const char *from)
						/* to may be == from */

{
  reg5 uint length;
  reg2 my_string pos;
  reg3 my_string from_ptr;
  reg4 my_string start;
  char parent[5],				/* for "FN_PARENTDIR" */
       buff[FN_REFLEN+1],*end_parentdir;
  DBUG_ENTER("cleanup_dirname");
  DBUG_PRINT("enter",("from: '%s'",from));

  start=buff;
  from_ptr=(my_string) from;
#ifdef FN_DEVCHAR
  if ((pos=strrchr(from_ptr,FN_DEVCHAR)) != 0)
  {						/* Skipp device part */
    length=(uint) (pos-from_ptr)+1;
    start=strnmov(buff,from_ptr,length); from_ptr+=length;
  }
#endif

  parent[0]=FN_LIBCHAR;
  length=(uint) (strmov(parent+1,FN_PARENTDIR)-parent);
  for (pos=start ; (*pos= *from_ptr++) != 0 ; pos++)
  {
    if (*pos == '/')
      *pos = FN_LIBCHAR;
    if (*pos == FN_LIBCHAR)
    {
      if ((uint) (pos-start) > length && bcmp(pos-length,parent,length) == 0)
      {						/* If .../../; skipp prev */
	pos-=length;
	if (pos != start)
	{					 /* not /../ */
	  pos--;
	  if (*pos == FN_HOMELIB && (pos == start || pos[-1] == FN_LIBCHAR))
	  {
	    if (!home_dir)
	    {
	      pos+=length+1;			/* Don't unpack ~/.. */
	      continue;
	    }
	    pos=strmov(buff,home_dir)-1;	/* Unpacks ~/.. */
	    if (*pos == FN_LIBCHAR)
	      pos--;				/* home ended with '/' */
	  }
	  if (*pos == FN_CURLIB && (pos == start || pos[-1] == FN_LIBCHAR))
	  {
	    if (my_getwd(curr_dir,FN_REFLEN,MYF(0)))
	    {
	      pos+=length+1;			/* Don't unpack ./.. */
	      continue;
	    }
	    pos=strmov(buff,curr_dir)-1;	/* Unpacks ./.. */
	    if (*pos == FN_LIBCHAR)
	      pos--;				/* home ended with '/' */
	  }
	  end_parentdir=pos;
	  while (pos >= start && *pos != FN_LIBCHAR)	/* remove prev dir */
	    pos--;
	  if (pos[1] == FN_HOMELIB || bcmp(pos,parent,length) == 0)
	  {					/* Don't remove ~user/ */
	    pos=strmov(end_parentdir+1,parent);
	    *pos=FN_LIBCHAR;
	    continue;
	  }
	}
      }
      else if ((uint) (pos-start) == length-1 &&
	       !bcmp(start,parent+1,length-1))
	start=pos;				/* Starts with "../" */
      else if (pos-start > 0 && pos[-1] == FN_LIBCHAR)
      {
#ifdef FN_NETWORK_DRIVES
	if (pos-start != 1)
#endif
	  pos--;			/* Remove dupplicate '/' */
      }
      else if (pos-start > 1 && pos[-1] == FN_CURLIB && pos[-2] == FN_LIBCHAR)
	pos-=2;					/* Skipp /./ */
      else if (pos > buff+1 && pos[-1] == FN_HOMELIB && pos[-2] == FN_LIBCHAR)
      {					/* Found ..../~/  */
	buff[0]=FN_HOMELIB;
	buff[1]=FN_LIBCHAR;
	start=buff; pos=buff+1;
      }
    }
  }
  (void) strmov(to,buff);
  DBUG_PRINT("exit",("to: '%s'",to));
  DBUG_RETURN((uint) (pos-buff));
} /* cleanup_dirname */


	/*
	  On system where you don't have symbolic links, the following
	  code will allow you to create a file: 
	  directory-name.lnk that should contain the real path
	  to the directory.  This will be used if the directory name
	  doesn't exists
	*/
	  

my_bool my_use_symdir=0;	/* Set this if you want to use symdirs */

#ifdef USE_SYMDIR
void symdirget(char *dir)
{
  char buff[FN_REFLEN];
  char *pos=strend(dir);
  if (dir[0] && pos[-1] != FN_DEVCHAR && access(dir, F_OK))
  {
    FILE *fp;
    char temp= *(--pos);            /* May be "/" or "\" */
    strmov(pos,".sym");
    fp = my_fopen(dir, O_RDONLY,MYF(0));
    *pos++=temp; *pos=0;	  /* Restore old filename */
    if (fp)
    {
      if (fgets(buff, sizeof(buff), fp))
      {
	for (pos=strend(buff);
	     pos > buff && (iscntrl(pos[-1]) || isspace(pos[-1])) ;
	     pos --);
	strmake(dir,buff, (uint) (pos-buff));
      }
      my_fclose(fp,MYF(0));
    }
  }
}
#endif /* USE_SYMDIR */

	/* Unpacks dirname to name that can be used by open... */
	/* Make that last char of to is '/' if from not empty and
	   from doesn't end in FN_DEVCHAR */
	/* Uses cleanup_dirname and changes ~/.. to home_dir/.. */
	/* Returns length of new directory */

uint unpack_dirname(my_string to, const char *from)

						  /* to may be == from */
{
  uint length,h_length;
  char buff[FN_REFLEN+1+4],*suffix,*tilde_expansion;
  DBUG_ENTER("unpack_dirname");

  (void) intern_filename(buff,from);		/* Change to intern name */
  length= (uint) strlen(buff);			/* Fix that '/' is last */
  if (length &&
#ifdef FN_DEVCHAR
      buff[length-1] != FN_DEVCHAR &&
#endif
      buff[length-1] != FN_LIBCHAR && buff[length-1] != '/')
  {
    buff[length]=FN_LIBCHAR;
    buff[length+1]= '\0';
  }

  length=cleanup_dirname(buff,buff);
  if (buff[0] == FN_HOMELIB)
  {
    suffix=buff+1; tilde_expansion=expand_tilde(&suffix);
    if (tilde_expansion)
    {
      length-=(uint) (suffix-buff)-1;
      if (length+(h_length= (uint) strlen(tilde_expansion)) <= FN_REFLEN)
      {
	if (tilde_expansion[h_length-1] == FN_LIBCHAR)
	  h_length--;
	if (buff+h_length < suffix)
	  bmove(buff+h_length,suffix,length);
	else
	  bmove_upp(buff+h_length+length,suffix+length,length);
	bmove(buff,tilde_expansion,h_length);
      }
    }
  }
#ifdef USE_SYMDIR
  if (my_use_symdir)
    symdirget(buff);
#endif
  DBUG_RETURN(system_filename(to,buff));	/* Fix for open */
} /* unpack_dirname */


	/* Expand tilde to home or user-directory */
	/* Path is reset to point at FN_LIBCHAR after ~xxx */

static my_string NEAR_F expand_tilde(my_string *path)
{
  if (path[0][0] == FN_LIBCHAR)
    return home_dir;			/* ~/ expanded to home */
#ifdef HAVE_GETPWNAM
  {
    char *str,save;
    struct passwd *user_entry;

    if (!(str=strchr(*path,FN_LIBCHAR)))
      str=strend(*path);
    save= *str; *str= '\0';
    user_entry=getpwnam(*path);
    *str=save;
    endpwent();
    if (user_entry)
    {
      *path=str;
      return user_entry->pw_dir;
    }
  }
#endif
  return (my_string) 0;
}

	/* fix filename so it can be used by open, create .. */
	/* to may be == from */
	/* Returns to */

my_string unpack_filename(my_string to, const char *from)
{
  uint length,n_length;
  char buff[FN_REFLEN];
  DBUG_ENTER("unpack_filename");

  length=dirname_part(buff,from);		/* copy & convert dirname */
  n_length=unpack_dirname(buff,buff);
  if (n_length+strlen(from+length) < FN_REFLEN)
  {
    (void) strmov(buff+n_length,from+length);
    (void) system_filename(to,buff);		/* Fix to usably filename */
  }
  else
    (void) system_filename(to,from);		/* Fix to usably filename */
  DBUG_RETURN(to);
} /* unpack_filename */


	/* Convert filename (unix standard) to system standard */
	/* Used before system command's like open(), create() .. */
	/* Returns to */

uint system_filename(my_string to, const char *from)
{
#ifndef FN_C_BEFORE_DIR
  return (uint) (strmake(to,from,FN_REFLEN-1)-to);
#else	/* VMS */

	/* change 'dev:lib/xxx' to 'dev:[lib]xxx' */
	/* change 'dev:xxx' to 'dev:xxx' */
	/* change './xxx' to 'xxx' */
	/* change './lib/' or lib/ to '[.lib]' */
	/* change '/x/y/z to '[x.y]x' */
	/* change 'dev:/x' to 'dev:[000000]x' */

  int libchar_found,length;
  my_string to_pos,from_pos,pos;
  char buff[FN_REFLEN];
  DBUG_ENTER("system_filename");

  libchar_found=0;
  (void) strmov(buff,from);			 /* If to == from */
  from_pos= buff;
  if ((pos=strrchr(from_pos,FN_DEVCHAR)))	/* Skipp device part */
  {
    pos++;
    to_pos=strnmov(to,from_pos,(size_s) (pos-from_pos));
    from_pos=pos;
  }
  else
    to_pos=to;

  if (from_pos[0] == FN_CURLIB && from_pos[1] == FN_LIBCHAR)
    from_pos+=2;				/* Skipp './' */
  if (strchr(from_pos,FN_LIBCHAR))
  {
    *(to_pos++) = FN_C_BEFORE_DIR;
    if (strinstr(from_pos,FN_ROOTDIR) == 1)
    {
      from_pos+=strlen(FN_ROOTDIR);		/* Actually +1 but... */
      if (! strchr(from_pos,FN_LIBCHAR))
      {						/* No dir, use [000000] */
	to_pos=strmov(to_pos,FN_C_ROOT_DIR);
	libchar_found++;
      }
    }
    else
      *(to_pos++)=FN_C_DIR_SEP;			/* '.' gives current dir */

    while ((pos=strchr(from_pos,FN_LIBCHAR)))
    {
      if (libchar_found++)
	*(to_pos++)=FN_C_DIR_SEP;		/* Add '.' between dirs */
      if (strinstr(from_pos,FN_PARENTDIR) == 1 &&
	  from_pos+strlen(FN_PARENTDIR) == pos)
	to_pos=strmov(to_pos,FN_C_PARENT_DIR);	/* Found '../' */
      else
	to_pos=strnmov(to_pos,from_pos,(size_s) (pos-from_pos));
      from_pos=pos+1;
    }
    *(to_pos++)=FN_C_AFTER_DIR;
  }
  length=(int) (strmov(to_pos,from_pos)-to);
  DBUG_PRINT("exit",("name: '%s'",to));
  DBUG_RETURN((uint) length);
#endif
} /* system_filename */


	/* Fix a filename to intern (UNIX format) */

my_string intern_filename(my_string to, const char *from)
{
#ifndef VMS
  {
    uint length;
    char buff[FN_REFLEN];
    if (from == to)
    {						/* Dirname may destroy from */
      strmov(buff,from);
      from=buff;
    }
    length=dirname_part(to,from);			/* Copy dirname & fix chars */
    (void) strcat(to,from+length);
    return (to);
  }
#else	/* VMS */

	/* change 'dev:[lib]xxx' to 'dev:lib/xxx' */
	/* change 'dev:xxx' to 'dev:xxx' */
	/* change 'dev:x/y/[.lib]' to 'dev:x/y/lib/ */
	/* change '[.lib]' to './lib/' */
	/* change '[x.y]' or '[x.][y]' or '[x][.y]' to '/x/y/' */
	/* change '[000000.x] or [x.000000]' to '/x/' */

  int par_length,root_length;
  my_string pos,from_pos,to_pos,end_pos;
  char buff[FN_REFLEN];

  (void) strmov(buff,from);
  convert_dirname(buff);			/* change '<>' to '[]' */
  from_pos=buff;
  if ((pos=strrchr(from_pos,FN_DEVCHAR)))	/* Skipp device part */
  {
    pos++;
    to_pos=strnmov(to,from_pos,(size_s) (pos-from_pos));
    from_pos=pos;
  }
  else
    to_pos=to;

  root_length=strlen(FN_C_ROOT_DIR);
  if ((pos = strchr(from_pos,FN_C_BEFORE_DIR)) &&
      (end_pos = strrchr(pos+1,FN_C_AFTER_DIR)))
  {
    to_pos=strnmov(to_pos,from_pos,(size_s) (pos-from_pos));
				/* Copy all between ':' and '[' */
    from_pos=pos+1;
    if (strinstr(from_pos,FN_C_ROOT_DIR) == 1 &&
	(from_pos[root_length] == FN_C_DIR_SEP ||
	 from_pos[root_length] == FN_C_AFTER_DIR))
    {
      from_pos+=root_length+1;
    }
    else if (*from_pos == FN_C_DIR_SEP)
      *(to_pos++) = FN_CURLIB;			/* Set ./ first */
    *(to_pos++) = FN_LIBCHAR;

    par_length=strlen(FN_C_PARENT_DIR);
    pos=to_pos;
    for (; from_pos <= end_pos ; from_pos++)
    {
      switch (*from_pos) {
      case FN_C_DIR_SEP:
      case FN_C_AFTER_DIR:
	if (pos != to_pos)
	{
	  if ((int) (to_pos-pos) == root_length &&
	      is_suffix(pos,FN_C_ROOT_DIR))
	    to_pos=pos;				/* remove root-pos */
	  else
	  {
	    *(to_pos++)=FN_LIBCHAR;		/* Find lib */
	    pos=to_pos;
	  }
	}
	break;
      case FN_C_BEFORE_DIR:
	break;
      case '-':					/* *(FN_C_PARENT_DIR): */
	if (to_pos[-1] == FN_LIBCHAR &&
	    strncmp(from_pos,FN_C_PARENT_DIR,par_length) == 0)
	{					/* Change '-' to '..' */
	  to_pos=strmov(to_pos,FN_PARENTDIR);
	  *(to_pos++)=FN_LIBCHAR;
	  pos=to_pos;
	  from_pos+=par_length-1;
	  break;
	}
	/* Fall through */
      default:
	*(to_pos++)= *from_pos;
	break;
      }
    }
  }
  (void) strmov(to_pos,from_pos);
  return (to);
#endif /* VMS */
} /* intern_filename */
