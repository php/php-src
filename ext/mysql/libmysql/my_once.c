/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* Not MT-SAFE */

#ifdef SAFEMALLOC			/* We don't need SAFEMALLOC here */
#undef SAFEMALLOC
#endif

#include "mysys_priv.h"
#include "my_static.h"
#include "mysys_err.h"

	/* alloc for things we don't nead to free */
	/* No DBUG_ENTER... here to get smaller dbug-startup */

gptr my_once_alloc(unsigned int Size, myf MyFlags)
{
  uint get_size,max_left;
  gptr point;
  reg1 USED_MEM *next;
  reg2 USED_MEM **prev;

  Size= ALIGN_SIZE(Size);
  prev= &my_once_root_block;
  max_left=0;
  for (next=my_once_root_block ; next && next->left < Size ; next= next->next)
  {
    if (next->left > max_left)
      max_left=next->left;
    prev= &next->next;
  }
  if (! next)
  {						/* Time to alloc new block */
    get_size= Size+ALIGN_SIZE(sizeof(USED_MEM));
    if (max_left*4 < my_once_extra && get_size < my_once_extra)
      get_size=my_once_extra;			/* Normal alloc */

    if ((next = (USED_MEM*) malloc(get_size)) == 0)
    {
      my_errno=errno;
      if (MyFlags & (MY_FAE+MY_WME))
	my_error(EE_OUTOFMEMORY, MYF(ME_BELL+ME_WAITTANG),get_size);
      return((gptr) 0);
    }
    DBUG_PRINT("test",("my_once_malloc %u byte malloced",get_size));
    next->next= 0;
    next->size= get_size;
    next->left= get_size-ALIGN_SIZE(sizeof(USED_MEM));
    *prev=next;
  }
  point= (gptr) ((char*) next+ (next->size-next->left));
  next->left-= Size;

  return(point);
} /* my_once_alloc */


	/* deallocate everything used by my_once_alloc */

void my_once_free(void)
{
  reg1 USED_MEM *next,*old;
  DBUG_ENTER("my_once_free");

  for (next=my_once_root_block ; next ; )
  {
    old=next; next= next->next ;
    free((gptr) old);
  }
  my_once_root_block=0;

  DBUG_VOID_RETURN;
} /* my_once_free */
