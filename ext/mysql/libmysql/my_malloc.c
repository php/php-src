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

#ifdef SAFEMALLOC			/* We don't need SAFEMALLOC here */
#undef SAFEMALLOC
#endif

#include "mysys_priv.h"
#include "mysys_err.h"
#include <m_string.h>

	/* My memory allocator */

gptr my_malloc(unsigned int Size, myf MyFlags)
{
  gptr point;
  DBUG_ENTER("my_malloc");
  DBUG_PRINT("my",("Size: %u  MyFlags: %d",Size, MyFlags));

  if (!Size)
    Size=1;					/* Safety */
  if ((point = malloc(Size)) == NULL)
  {
    my_errno=errno;
    if (MyFlags & MY_FAE)
      error_handler_hook=fatal_error_handler_hook;
    if (MyFlags & (MY_FAE+MY_WME))
      my_error(EE_OUTOFMEMORY, MYF(ME_BELL+ME_WAITTANG),Size);
    if (MyFlags & MY_FAE)
      exit(1);
  }
  else if (MyFlags & MY_ZEROFILL)
    bzero(point,Size);
  DBUG_PRINT("exit",("ptr: %lx",point));
  DBUG_RETURN(point);
} /* my_malloc */


	/* Free memory allocated with my_malloc */
	/*ARGSUSED*/

void my_no_flags_free(gptr ptr)
{
  DBUG_ENTER("my_free");
  DBUG_PRINT("my",("ptr: %lx",ptr));
  if (ptr)
    free(ptr);
  DBUG_VOID_RETURN;
} /* my_free */


	/* malloc and copy */

gptr my_memdup(const byte *from, uint length, myf MyFlags)
{
  gptr ptr;
  if ((ptr=my_malloc(length,MyFlags)) != 0)
    memcpy((byte*) ptr, (byte*) from,(size_t) length);
  return(ptr);
}


my_string my_strdup(const char *from, myf MyFlags)
{
  gptr ptr;
  uint length=(uint) strlen(from)+1;
  if ((ptr=my_malloc(length,MyFlags)) != 0)
    memcpy((byte*) ptr, (byte*) from,(size_t) length);
  return((my_string) ptr);
}
