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

/*  File   : is_prefix.c
    Author : Michael Widenius
    Defines: is_prefix()

    is_prefix(s, t) returns 1 if s starts with t.
    A empty t is allways a prefix.
*/

#include <global.h>
#include "m_string.h"

int is_prefix(register const char *s, register const char *t)
{
  while (*t)
    if (*s++ != *t++) return 0;
  return 1;					/* WRONG */
}
