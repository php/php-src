/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#ifdef SAFEMALLOC			/* We don't need SAFEMALLOC here */
#undef SAFEMALLOC
#endif

#include "mysys_priv.h"
#include "mysys_err.h"

	/* My memory re allocator */

gptr my_realloc(gptr oldpoint, uint Size, myf MyFlags)
{
  gptr point;
  DBUG_ENTER("my_realloc");
  DBUG_PRINT("my",("ptr: %lx  Size: %u  MyFlags: %d",oldpoint, Size, MyFlags));

  if (!oldpoint && (MyFlags & MY_ALLOW_ZERO_PTR))
    DBUG_RETURN(my_malloc(Size,MyFlags));
#ifdef USE_HALLOC
  if (!(point = malloc(Size)))
  {
    if (MyFlags & MY_FREE_ON_ERROR)
      my_free(oldpoint,MyFlags);
    if (MyFlags & MY_HOLD_ON_ERROR)
      DBUG_RETURN(oldpoint);
    my_errno=errno;
    if (MyFlags & MY_FAE+MY_WME)
      my_error(EE_OUTOFMEMORY, MYF(ME_BELL+ME_WAITTANG),Size);
  }
  else
  {
    memcpy(point,oldpoint,Size);
    free(oldpoint);
  }
#else
  if ((point = (char*)realloc(oldpoint,Size)) == NULL)
  {
    if (MyFlags & MY_FREE_ON_ERROR)
      my_free(oldpoint,MyFLAGS);
    if (MyFlags & MY_HOLD_ON_ERROR)
      DBUG_RETURN(oldpoint);
    my_errno=errno;
    if (MyFlags & (MY_FAE+MY_WME))
      my_error(EE_OUTOFMEMORY, MYF(ME_BELL+ME_WAITTANG), Size);
  }
#endif
  DBUG_PRINT("exit",("ptr: %lx",point));
  DBUG_RETURN(point);
} /* my_realloc */
