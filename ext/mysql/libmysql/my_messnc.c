/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"

int my_message_no_curses(uint error __attribute__((unused)),
			 const char *str, myf MyFlags)
{
  DBUG_ENTER("my_message_no_curses");
  DBUG_PRINT("enter",("message: %s",str));
  (void) fflush(stdout);
  if (MyFlags & ME_BELL)
    (void) fputc('\007',stderr);				/* Bell */
  if (my_progname)
  {
    (void)fputs(my_progname,stderr); (void)fputs(": ",stderr);
  }
  (void)fputs(str,stderr);
  (void)fputc('\n',stderr);
  (void)fflush(stderr);
  DBUG_RETURN(0);
}
