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
#include "mysys_err.h"
#include <m_string.h>
#include <stdarg.h>
#include <m_ctype.h>

/* Define some external variables for error handling */

const char ** NEAR errmsg[MAXMAPS]={0,0,0,0};
char NEAR errbuff[NRERRBUFFS][ERRMSGSIZE];

/* Error message to user */
/*VARARGS2*/

int my_error(int nr,myf MyFlags, ...)
{
  va_list	ap;
  uint		olen, plen;
  reg1 const char *tpos;
  reg2 char	*endpos;
  char		* par;
  char		ebuff[ERRMSGSIZE+20];
  DBUG_ENTER("my_error");

  va_start(ap,MyFlags);
  DBUG_PRINT("my", ("nr: %d  MyFlags: %d  errno: %d", nr, MyFlags, errno));

  if (nr / ERRMOD == GLOB && errmsg[GLOB] == 0)
    init_glob_errs();

  olen=(uint) strlen(tpos=errmsg[nr / ERRMOD][nr % ERRMOD]);
  endpos=ebuff;

  while (*tpos)
  {
    if (tpos[0] != '%')
    {
      *endpos++= *tpos++;	/* Copy ordinary char */
      olen++;
      continue;
    }
    if (*++tpos == '%')		/* test if %% */
    {
      olen--;
    }
    else
    {
      /* Skipp if max size is used (to be compatible with printf) */
      while (isdigit(*tpos) || *tpos == '.' || *tpos == '-')
	tpos++;
      if (*tpos == 's')				/* String parameter */
      {
	par = va_arg(ap, char *);
	plen = (uint) strlen(par);
	if (olen + plen < ERRMSGSIZE+2)		/* Replace if possible */
	{
	  endpos=strmov(endpos,par);
	  tpos++;
	  olen+=plen-2;
	  continue;
	}
      }
      else if (*tpos == 'd' || *tpos == 'u')	/* Integer parameter */
      {
	register int iarg;
	iarg = va_arg(ap, int);
	if (*tpos == 'd')
	  plen= (uint) (int2str((long) iarg,endpos, -10) - endpos);
	else
	  plen= (uint) (int2str((long) (uint) iarg,endpos,10)- endpos);
	if (olen + plen < ERRMSGSIZE+2) /* Replace parameter if possible */
	{
	  endpos+=plen;
	  tpos++;
	  olen+=plen-2;
	  continue;
	}
      }
    }
    *endpos++='%';		/* % used as % or unknown code */
  }
  *endpos='\0';			/* End of errmessage */
  va_end(ap);
  DBUG_RETURN((*error_handler_hook)(nr, ebuff, MyFlags));
}

	/* Error as printf */

int my_printf_error (uint error, const char *format, myf MyFlags, ...)
{
  va_list args;
  char ebuff[ERRMSGSIZE+20];

  va_start(args,MyFlags);
  (void) vsprintf (ebuff,format,args);
  va_end(args);
  return (*error_handler_hook)(error, ebuff, MyFlags);
}

	/* Give message using error_handler_hook */

int my_message(uint error, const char *str, register myf MyFlags)
{
  return (*error_handler_hook)(error, str, MyFlags);
}
