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

/*  File   : strfill.c
    Author : Monty
    Updated: 1987.04.16
    Defines: strfill()

    strfill(dest, len, fill) makes a string of fill-characters. The result
    string is of length == len. The des+len character is allways set to NULL.
    strfill() returns pointer to dest+len;
*/

#include <global.h>
#include "m_string.h"

my_string strfill(my_string s,uint len,pchar fill)
{
  while (len--) *s++ = fill;
  *(s) = '\0';
  return(s);
} /* strfill */
