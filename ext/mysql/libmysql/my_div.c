/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"

my_string my_filename(File fd)
{
  DBUG_ENTER("my_filename");
  if (fd >= MY_NFILE)
    DBUG_RETURN((char*) "UNKNOWN");
  if (fd >= 0 && my_file_info[fd].type != UNOPEN)
  {
    DBUG_RETURN(my_file_info[fd].name);
  }
  else
    DBUG_RETURN((char*) "UNOPENED");	/* Debug message */
}
