/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
** Ask for a password from tty
** This is an own file to avoid conflicts with curses
*/
#include <global.h>
#include <my_sys.h>
#include "mysql.h"
#include <m_string.h>
#include <m_ctype.h>
#include <dbug.h>

#if defined(HAVE_BROKEN_GETPASS) && !defined(HAVE_GETPASSPHRASE)
#undef HAVE_GETPASS
#endif

#ifdef HAVE_GETPASS
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif /* HAVE_PWD_H */
#else /* ! HAVE_GETPASS */
#if !defined( __WIN__) && !defined(OS2)
#include <sys/ioctl.h>
#ifdef HAVE_TERMIOS_H				/* For tty-password */
#include	<termios.h>
#define TERMIO	struct termios
#else
#ifdef HAVE_TERMIO_H				/* For tty-password */
#include	<termio.h>
#define TERMIO	struct termio
#else
#include	<sgtty.h>
#define TERMIO	struct sgttyb
#endif
#endif
#ifdef alpha_linux_port
#include <asm/ioctls.h>				/* QQ; Fix this in configure */
#include <asm/termiobits.h>
#endif
#else
#include <conio.h>
#endif /* __WIN__ */
#endif /* HAVE_GETPASS */

#ifdef HAVE_GETPASSPHRASE			/* For Solaris */
#define getpass(A) getpassphrase(A)
#endif

#if defined( __WIN__) || defined(OS2)
/* were just going to fake it here and get input from the keyboard */

char *get_tty_password(char *opt_message)
{
  char to[80];
  char *pos=to,*end=to+sizeof(to)-1;
  int i=0;
  DBUG_ENTER("get_tty_password");
  fprintf(stdout,opt_message ? opt_message : "Enter password: ");
  for (;;)
  {
    char tmp;
    tmp=_getch();
    if (tmp == '\b' || (int) tmp == 127)
    {
      if (pos != to)
      {
	_cputs("\b \b");
	pos--;
	continue;
      }
    }
    if (tmp == '\n' || tmp == '\r' || tmp == 3)
      break;
    if (iscntrl(tmp) || pos == end)
      continue;
    _cputs("*");
    *(pos++) = tmp;
  }
  while (pos != to && isspace(pos[-1]) == ' ')
    pos--;					/* Allow dummy space at end */
  *pos=0;
  _cputs("\n");
  DBUG_RETURN(my_strdup(to,MYF(MY_FAE)));
}

#else


#ifndef HAVE_GETPASS
/*
** Can't use fgets, because readline will get confused
** length is max number of chars in to, not counting \0
*  to will not include the eol characters.
*/

static void get_password(char *to,uint length,int fd,bool echo)
{
  char *pos=to,*end=to+length;

  for (;;)
  {
    char tmp;
    if (my_read(fd,&tmp,1,MYF(0)) != 1)
      break;
    if (tmp == '\b' || (int) tmp == 127)
    {
      if (pos != to)
      {
	if (echo)
	{
	  fputs("\b \b",stdout);
	  fflush(stdout);
	}
	pos--;
	continue;
      }
    }
    if (tmp == '\n' || tmp == '\r' || tmp == 3)
      break;
    if (iscntrl(tmp) || pos == end)
      continue;
    if (echo)
    {
      fputc('*',stdout);
      fflush(stdout);
    }
    *(pos++) = tmp;
  }
  while (pos != to && isspace(pos[-1]) == ' ')
    pos--;					/* Allow dummy space at end */
  *pos=0;
  return;
}
#endif /* ! HAVE_GETPASS */


char *get_tty_password(char *opt_message)
{
#ifdef HAVE_GETPASS
  char *passbuff;
#else /* ! HAVE_GETPASS */
  TERMIO org,tmp;
#endif /* HAVE_GETPASS */
  char buff[80];

  DBUG_ENTER("get_tty_password");

#ifdef HAVE_GETPASS
  passbuff = getpass(opt_message ? opt_message : "Enter password: ");

  /* copy the password to buff and clear original (static) buffer */
  strnmov(buff, passbuff, sizeof(buff) - 1);
#ifdef _PASSWORD_LEN
  memset(passbuff, 0, _PASSWORD_LEN);
#endif
#else 
  if (isatty(fileno(stdout)))
  {
    fputs(opt_message ? opt_message : "Enter password: ",stdout);
    fflush(stdout);
  }
#if defined(HAVE_TERMIOS_H)
  tcgetattr(fileno(stdin), &org);
  tmp = org;
  tmp.c_lflag &= ~(ECHO | ISIG | ICANON);
  tmp.c_cc[VMIN] = 1;
  tmp.c_cc[VTIME] = 0;
  tcsetattr(fileno(stdin), TCSADRAIN, &tmp);
  get_password(buff, sizeof(buff)-1, fileno(stdin), isatty(fileno(stdout)));
  tcsetattr(fileno(stdin), TCSADRAIN, &org);
#elif defined(HAVE_TERMIO_H)
  ioctl(fileno(stdin), (int) TCGETA, &org);
  tmp=org;
  tmp.c_lflag &= ~(ECHO | ISIG | ICANON);
  tmp.c_cc[VMIN] = 1;
  tmp.c_cc[VTIME]= 0;
  ioctl(fileno(stdin),(int) TCSETA, &tmp);
  get_password(buff,sizeof(buff)-1,fileno(stdin),isatty(fileno(stdout)));
  ioctl(fileno(stdin),(int) TCSETA, &org);
#else
  gtty(fileno(stdin), &org);
  tmp=org;
  tmp.sg_flags &= ~ECHO;
  tmp.sg_flags |= RAW;
  stty(fileno(stdin), &tmp);
  get_password(buff,sizeof(buff)-1,fileno(stdin),isatty(fileno(stdout)));
  stty(fileno(stdin), &org);
#endif
  if (isatty(fileno(stdout)))
    fputc('\n',stdout);
#endif /* HAVE_GETPASS */

  DBUG_RETURN(my_strdup(buff,MYF(MY_FAE)));
}
#endif /*__WIN__*/
