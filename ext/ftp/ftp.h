/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@php.net> (resume functions)            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef	FTP_H
#define	FTP_H

#include "php_network.h"

#include <stdio.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#define	FTP_DEFAULT_TIMEOUT	90
#define FTP_DEFAULT_AUTOSEEK 1
#define PHP_FTP_FAILED			0
#define PHP_FTP_FINISHED		1
#define PHP_FTP_MOREDATA		2

/* XXX this should be configurable at runtime XXX */
#define	FTP_BUFSIZE	4096

typedef enum ftptype {
	FTPTYPE_ASCII,
	FTPTYPE_IMAGE
} ftptype_t;

typedef struct databuf
{
	int		listener;		/* listener socket */
	int		fd;			/* data connection */
	ftptype_t	type;			/* transfer type */
	char		buf[FTP_BUFSIZE];	/* data buffer */
#if HAVE_OPENSSL_EXT
	SSL		*ssl_handle;	/* ssl handle */
	int		ssl_active;		/* flag if ssl is active or not */
#endif
} databuf_t;

typedef struct ftpbuf
{
	int		fd;			/* control connection */
	php_sockaddr_storage	localaddr;	/* local address */
	int		resp;			/* last response code */
	char		inbuf[FTP_BUFSIZE];	/* last response text */
	char		*extra;			/* extra characters */
	int		extralen;		/* number of extra chars */
	char		outbuf[FTP_BUFSIZE];	/* command output buffer */
	char		*pwd;			/* cached pwd */
	char		*syst;			/* cached system type */
	ftptype_t	type;			/* current transfer type */
	int		pasv;			/* 0=off; 1=pasv; 2=ready */
	php_sockaddr_storage	pasvaddr;	/* passive mode address */
	long	timeout_sec;	/* User configureable timeout (seconds) */
	int			autoseek;	/* User configureable autoseek flag */

	int				nb;		/* "nonblocking" transfer in progress */
	databuf_t		*data;	/* Data connection for "nonblocking" transfers */
	php_stream		*stream; /* output stream for "nonblocking" transfers */
	int				lastch;		/* last char of previous call */
	int				direction;	/* recv = 0 / send = 1 */
	int				closestream;/* close or not close stream */
#if HAVE_OPENSSL_EXT
	int				use_ssl; /* enable(1) or disable(0) ssl */
	int				use_ssl_for_data; /* en/disable ssl for the dataconnection */
	int				old_ssl;	/* old mode = forced data encryption */
	SSL				*ssl_handle;      /* handle for control connection */
	int				ssl_active;		  /* ssl active on control conn */
#endif

} ftpbuf_t;



/* open a FTP connection, returns ftpbuf (NULL on error)
 * port is the ftp port in network byte order, or 0 for the default
 */
ftpbuf_t*	ftp_open(const char *host, short port, long timeout_sec TSRMLS_DC);

/* quits from the ftp session (it still needs to be closed)
 * return true on success, false on error
 */
int		ftp_quit(ftpbuf_t *ftp);

/* frees up any cached data held in the ftp buffer */
void		ftp_gc(ftpbuf_t *ftp);

/* close the FTP connection and return NULL */
ftpbuf_t*	ftp_close(ftpbuf_t *ftp);

/* logs into the FTP server, returns true on success, false on error */
int		ftp_login(ftpbuf_t *ftp, const char *user, const char *pass TSRMLS_DC);

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

/* Set permissions on a file */
int		ftp_chmod(ftpbuf_t *ftp, const int mode, const char *filename);

/* returns a NULL-terminated array of filenames in the given path
 * or NULL on error.  the return array must be freed (but don't
 * free the array elements)
 */
char**		ftp_nlist(ftpbuf_t *ftp, const char *path TSRMLS_DC);

/* returns a NULL-terminated array of lines returned by the ftp
 * LIST command for the given path or NULL on error.  the return
 * array must be freed (but don't
 * free the array elements)
 */
char**		ftp_list(ftpbuf_t *ftp, const char *path, int recursive TSRMLS_DC);

/* switches passive mode on or off
 * returns true on success, false on error
 */
int		ftp_pasv(ftpbuf_t *ftp, int pasv);

/* retrieves a file and saves its contents to outfp
 * returns true on success, false on error
 */
int		ftp_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, ftptype_t type, int resumepos TSRMLS_DC);

/* stores the data from a file, socket, or process as a file on the remote server
 * returns true on success, false on error
 */
int		ftp_put(ftpbuf_t *ftp, const char *path, php_stream *instream, ftptype_t type, int startpos TSRMLS_DC);

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

/* retrieves part of a file and saves its contents to outfp
 * returns true on success, false on error
 */
int		ftp_nb_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, ftptype_t type, int resumepos TSRMLS_DC);

/* stores the data from a file, socket, or process as a file on the remote server
 * returns true on success, false on error
 */
int		ftp_nb_put(ftpbuf_t *ftp, const char *path, php_stream *instream, ftptype_t type, int startpos TSRMLS_DC);

/* continues a previous nb_(f)get command
 */
int		ftp_nb_continue_read(ftpbuf_t *ftp TSRMLS_DC);

/* continues a previous nb_(f)put command
 */
int		ftp_nb_continue_write(ftpbuf_t *ftp TSRMLS_DC);


#endif
