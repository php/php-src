/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* Routines to handle mallocing of results which will be freed the same time */

#include <global.h>
#include <my_sys.h>
#include <m_string.h>

void init_alloc_root(MEM_ROOT *mem_root,uint block_size)
{
  mem_root->free=mem_root->used=0;
  mem_root->min_malloc=16;
  mem_root->block_size=block_size-MALLOC_OVERHEAD-sizeof(USED_MEM)-8;
  mem_root->error_handler=0;
}

gptr alloc_root(MEM_ROOT *mem_root,unsigned int Size)
{
#if defined(HAVE_purify) && defined(EXTRA_DEBUG)
  reg1 USED_MEM *next;
  Size+=ALIGN_SIZE(sizeof(USED_MEM));

  if (!(next = (USED_MEM*) my_malloc(Size,MYF(MY_WME))))
  {
    if (mem_root->error_handler)
      (*mem_root->error_handler)();
    return((gptr) 0);				/* purecov: inspected */
  }
  next->next=mem_root->used;
  mem_root->used=next;
  return (gptr) (((char*) next)+ALIGN_SIZE(sizeof(USED_MEM)));
#else
  uint get_size,max_left;
  gptr point;
  reg1 USED_MEM *next;
  reg2 USED_MEM **prev;

  Size= ALIGN_SIZE(Size);
  prev= &mem_root->free;
  max_left=0;
  for (next= *prev ; next && next->left < Size ; next= next->next)
  {
    if (next->left > max_left)
      max_left=next->left;
    prev= &next->next;
  }
  if (! next)
  {						/* Time to alloc new block */
    get_size= Size+ALIGN_SIZE(sizeof(USED_MEM));
    if (max_left*4 < mem_root->block_size && get_size < mem_root->block_size)
      get_size=mem_root->block_size;		/* Normal alloc */

    if (!(next = (USED_MEM*) my_malloc(get_size,MYF(MY_WME))))
    {
      if (mem_root->error_handler)
	(*mem_root->error_handler)();
      return((gptr) 0);				/* purecov: inspected */
    }
    next->next= *prev;
    next->size= get_size;
    next->left= get_size-ALIGN_SIZE(sizeof(USED_MEM));
    *prev=next;
  }
  point= (gptr) ((char*) next+ (next->size-next->left));
  if ((next->left-= Size) < mem_root->min_malloc)
  {						/* Full block */
    *prev=next->next;				/* Remove block from list */
    next->next=mem_root->used;
    mem_root->used=next;
  }
  return(point);
#endif
}

	/* deallocate everything used by alloc_root */

void free_root(MEM_ROOT *root)
{
  reg1 USED_MEM *next,*old;
  DBUG_ENTER("free_root");

  if (!root)
    DBUG_VOID_RETURN; /* purecov: inspected */
  for (next= root->used ; next ; )
  {
    old=next; next= next->next ;
    my_free((gptr) old,MYF(0));
  }
  for (next= root->free ; next ; )
  {
    old=next; next= next->next ;
    my_free((gptr) old,MYF(0));
  }
  root->used=root->free=0;
  DBUG_VOID_RETURN;
}

char *strdup_root(MEM_ROOT *root,const char *str)
{
  uint len= (uint) strlen(str)+1;
  char *pos;
  if ((pos=alloc_root(root,len)))
    memcpy(pos,str,len);
  return pos;
}


char *memdup_root(MEM_ROOT *root,const char *str,uint len)
{
  char *pos;
  if ((pos=alloc_root(root,len)))
    memcpy(pos,str,len);
  return pos;
}
