/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/* Synced with php3 revision 1.25 1999-06-16 [ssb] */

#ifndef _FOPEN_WRAPPERS_H
#define _FOPEN_WRAPPERS_H

#include "php_globals.h"

#define IGNORE_PATH	0
#define USE_PATH	1
#define IGNORE_URL	2
/* There's no USE_URL. */
#if WIN32|WINNT
# define IGNORE_URL_WIN 2
#else
# define IGNORE_URL_WIN 0
#endif
#define ENFORCE_SAFE_MODE 4

#if WIN32|WINNT
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
# define SOCK_RECV_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define SOCK_RECV_ERR -1
#endif
#define SOCK_WRITE(d,s) send(s,d,strlen(d),0)
#define SOCK_WRITEL(d,l,s) send(s,d,l,0)
#define SOCK_FGETC(s) _php3_sock_fgetc((s))
#define SOCK_FGETS(b,l,s) _php3_sock_fgets((b),(l),(s))
#define SOCK_FEOF(sock) _php3_sock_feof((sock))
#define SOCK_FREAD(ptr,size,sock) _php3_sock_fread((ptr),(size),(sock))
#define SOCK_FCLOSE(s) _php3_sock_close(s)

#define FP_FGETS(buf,len,sock,fp,issock) \
	((issock)?SOCK_FGETS(buf,len,sock):fgets(buf,len,fp))
#define FP_FREAD(buf,len,sock,fp,issock) \
	((issock)?SOCK_FREAD(buf,len,sock):fread(buf,1,len,fp))
#define FP_FEOF(sock,fp,issock) \
	((issock)?SOCK_FEOF(sock):feof(fp))
#define FP_FGETC(sock,fp,issock) \
	((issock)?SOCK_FGETC(sock):fgetc(fp))

/* values for issock */
#define IS_NOT_SOCKET	0
#define IS_SOCKET		1
#define BAD_URL			2

extern PHPAPI FILE *php3_fopen_wrapper(char *filename, char *mode, int options, int *issock, int *socketd, char **opened_path);

PHPAPI FILE *php3_fopen_for_parser(void);

extern PHPAPI int _php3_check_open_basedir(char *path);
extern PHPAPI int _php3_check_specific_open_basedir(char *basedir, char *path PLS_DC);

extern PHPAPI FILE *php3_fopen_with_path(char *filename, char *mode, char *path, char **opened_path);

extern PHPAPI int php3_isurl(char *path);
extern PHPAPI char *php3_strip_url_passwd(char *path);

extern PHPAPI char *expand_filepath(char *filepath);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
