/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"

#include "mysys_err.h"

int my_delete(const char *name, myf MyFlags)
{
  int err;
  DBUG_ENTER("my_delete");
  DBUG_PRINT("my",("name %s MyFlags %d", name, MyFlags));

  if ((err = unlink(name)) == -1)
  {
    my_errno=errno;
    if (MyFlags & (MY_FAE+MY_WME))
      my_error(EE_DELETE,MYF(ME_BELL+ME_WAITTANG+(MyFlags & ME_NOINPUT)),
	       name,errno);
  }
  DBUG_RETURN(err);
} /* my_delete */
