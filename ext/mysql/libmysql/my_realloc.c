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
  if ((point = realloc(oldpoint,Size)) == NULL)
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
