/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

	/* Malloc many pointers at the same time */
	/* format myFlags,ptr,length,ptr,length ... until null ptr */

#include "mysys_priv.h"
#include <stdarg.h>

gptr my_multi_malloc(myf myFlags, ...)
{
  va_list args;
  char **ptr,*start,*res;
  uint tot_length,length;
  DBUG_ENTER("my_multi_malloc");

  va_start(args,myFlags);
  tot_length=0;
  while ((ptr=va_arg(args, char **)))
  {
    length=va_arg(args,uint);
    tot_length+=ALIGN_SIZE(length);
  }
  va_end(args);

  if (!(start=(char *) my_malloc(tot_length,myFlags)))
    DBUG_RETURN(0); /* purecov: inspected */

  va_start(args,myFlags);
  res=start;
  while ((ptr=va_arg(args, char **)))
  {
    *ptr=res;
    length=va_arg(args,uint);
    res+=ALIGN_SIZE(length);
  }
  va_end(args);
  DBUG_RETURN((gptr) start);
}
