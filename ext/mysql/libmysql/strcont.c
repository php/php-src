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

/*  File   : strcont.c
    Author : Monty
    Updated: 1988.07.27
    Defines: strcont()

    strcont(str, set) if str contanies any character in the string set.
    The result is the position of the first found character in str, or NullS
    if there isn't anything found.

*/

#include <global.h>
#include "m_string.h"

my_string strcont(reg1 const char *str,reg2 const char *set)
{
  reg3 my_string start = (my_string) set;

  while (*str)
  {
    while (*set)
    {
      if (*set++ == *str)
	return ((char*) str);
    }
    set=start; str++;
  }
  return (NullS);
} /* strcont */
