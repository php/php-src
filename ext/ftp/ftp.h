/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |          Andrew Skalski      <askalski@chek.com>                     |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef	FTP_H
#define	FTP_H

#include <stdio.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

/* XXX these should be configurable at runtime XXX */
#define	FTP_BUFSIZE	4096
#define	FTP_TIMEOUT	90

typedef enum ftptype {
	FTPTYPE_ASCII,
	FTPTYPE_IMAGE
} ftptype_t;

typedef struct ftpbuf
{
	int		fd;			/* control connection */
	struct in_addr	localaddr;		/* local inet address */
	int		resp;			/* last response code */
	char		inbuf[FTP_BUFSIZE];	/* last response text */
	char		*extra;			/* extra characters */
	int		extralen;		/* number of extra chars */
	char		outbuf[FTP_BUFSIZE];	/* command output buffer */
	char		*pwd;			/* cached pwd */
	char		*syst;			/* cached system type */
	ftptype_t	type;			/* current transfer type */
	int		pasv;			/* 0=off; 1=pasv; 2=ready */
	struct sockaddr_in	pasvaddr;	/* passive mode address */
} ftpbuf_t;

typedef struct databuf
{
	int		listener;		/* listener socket */
	int		fd;			/* data connection */
	ftptype_t	type;			/* transfer type */
	char		buf[FTP_BUFSIZE];	/* data buffer */
} databuf_t;


/* open a FTP connection, returns ftpbuf (NULL on error)
 * port is the ftp port in network byte order, or 0 for the default
 */
ftpbuf_t*	ftp_open(const char *host, short port);

/* quits from the ftp session (it still needs to be closed)
 * return true on success, false on error
 */
int		ftp_quit(ftpbuf_t *ftp);

/* frees up any cached data held in the ftp buffer */
void		ftp_gc(ftpbuf_t *ftp);

/* close the FTP connection and return NULL */
ftpbuf_t*	ftp_close(ftpbuf_t *ftp);

/* logs into the FTP server, returns true on success, false on error */
int		ftp_login(ftpbuf_t *ftp, const char *user, const char *pass);

/* reinitializes the connection, returns true on success, false on error */
int		ftp_reinit(ftpbuf_t *ftp);

/* returns the remote system type (NULL on error) */
const char*	ftp_syst(ftpbuf_t *ftp);

/* returns the present working directory (NULL on error) */
const char*	ftp_pwd(ftpbuf_t *ftp);

/* exec a command [special features], return true on success, false on error */
int 	ftp_exec(ftpbuf_t *ftp, const char *cmd);

/* changes directories, return true on success, false on error */
int		ftp_chdir(ftpbuf_t *ftp, const char *dir);

/* changes to parent directory, return true on success, false on error */
int		ftp_cdup(ftpbuf_t *ftp);

/* creates a directory, return the directory name on success, NULL on error.
 * the return value must be freed
 */
char*		ftp_mkdir(ftpbuf_t *ftp, const char *dir);

/* removes a directory, return true on success, false on error */
int		ftp_rmdir(ftpbuf_t *ftp, const char *dir);

/* returns a NULL-terminated array of filenames in the given path
 * or NULL on error.  the return array must be freed (but don't
 * free the array elements)
 */
char**		ftp_nlist(ftpbuf_t *ftp, const char *path);

/* returns a NULL-terminated array of lines returned by the ftp
 * LIST command for the given path or NULL on error.  the return
 * array must be freed (but don't
 * free the array elements)
 */
char**		ftp_list(ftpbuf_t *ftp, const char *path);

/* switches passive mode on or off
 * returns true on success, false on error
 */
int		ftp_pasv(ftpbuf_t *ftp, int pasv);

/* retrieves a file and saves its contents to outfp
 * returns true on success, false on error
 */
int		ftp_get(ftpbuf_t *ftp, FILE *outfp, const char *path,
			ftptype_t type);

/* stores the data from infp as a file on the remote server
 * returns true on success, false on error
 */
int		ftp_put(ftpbuf_t *ftp, const char *path, FILE *infp,
			ftptype_t type);

/* returns the size of the given file, or -1 on error */
int		ftp_size(ftpbuf_t *ftp, const char *path);

/* returns the last modified time of the given file, or -1 on error */
time_t		ftp_mdtm(ftpbuf_t *ftp, const char *path);

/* renames a file on the server */
int		ftp_rename(ftpbuf_t *ftp, const char *src, const char *dest);

/* deletes the file from the server */
int		ftp_delete(ftpbuf_t *ftp, const char *path);

/* sends a SITE command to the server */
int		ftp_site(ftpbuf_t *ftp, const char *cmd);

#endif
