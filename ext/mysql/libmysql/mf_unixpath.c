/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include <m_string.h>

	/* convert filename to unix style filename */
	/* If MSDOS converts '\' to '/' */

void to_unix_path(my_string to __attribute__((unused)))
{
#if FN_LIBCHAR != '/'
  {
    to--;
    while ((to=strchr(to+1,FN_LIBCHAR)) != 0)
      *to='/';
  }
#endif
}
