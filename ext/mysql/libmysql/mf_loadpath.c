/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include <m_string.h>

	/* Returns full load-path for a file. to may be = path */
	/* if path is a hard-path return path */
	/* if path starts with home-dir return path */
	/* if path starts with current dir or parent-dir unpack path */
	/* if there is no path, prepend with own_path_prefix if given */
	/* else unpack path according to current dir */

my_string my_load_path(my_string to, const char *path,
		       const char *own_path_prefix)
{
  char buff[FN_REFLEN];
  DBUG_ENTER("my_load_path");
  DBUG_PRINT("enter",("path: %s  prefix: %s",path,
		      own_path_prefix ? own_path_prefix : ""));

  if ((path[0] == FN_HOMELIB && path[1] == FN_LIBCHAR) ||
      test_if_hard_path(path))
    VOID(strmov(buff,path));
  else if ((path[0] == FN_CURLIB && path[1] == FN_LIBCHAR) ||
	   (is_prefix((gptr) path,FN_PARENTDIR) &&
	    path[strlen(FN_PARENTDIR)] == FN_LIBCHAR) ||
	   ! own_path_prefix)
  {
    if (! my_getwd(buff,(uint) (FN_REFLEN-strlen(path)),MYF(0)))
      VOID(strcat(buff,path));
    else
      VOID(strmov(buff,path));
  }
  else
    VOID(strxmov(buff,own_path_prefix,path,NullS));
  strmov(to,buff);
  DBUG_PRINT("exit",("to: %s",to));
  DBUG_RETURN(to);
} /* my_load_path */
