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
