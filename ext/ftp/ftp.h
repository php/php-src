/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@php.net> (resume functions)            |
   +----------------------------------------------------------------------+
 */

#ifndef	FTP_H
#define	FTP_H

#include "php_network.h"

#include <stdio.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#define	FTP_DEFAULT_TIMEOUT	90
#define FTP_DEFAULT_AUTOSEEK 1
#define FTP_DEFAULT_USEPASVADDRESS	1
#define PHP_FTP_FAILED			0
#define PHP_FTP_FINISHED		1
#define PHP_FTP_MOREDATA		2

/* XXX this should be configurable at runtime XXX */
#define	FTP_BUFSIZE	4096

typedef enum ftptype {
	FTPTYPE_ASCII=1,
	FTPTYPE_IMAGE
} ftptype_t;

typedef struct databuf
{
	int		listener;		/* listener socket */
	php_socket_t		fd;			/* data connection */
	ftptype_t	type;			/* transfer type */
	char		buf[FTP_BUFSIZE];	/* data buffer */
#ifdef HAVE_FTP_SSL
	SSL		*ssl_handle;	/* ssl handle */
	int		ssl_active;		/* flag if ssl is active or not */
#endif
} databuf_t;

typedef struct ftpbuf
{
	php_socket_t		fd;			/* control connection */
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
	zend_long	timeout_sec;	/* User configurable timeout (seconds) */
	int			autoseek;	/* User configurable autoseek flag */
	int			usepasvaddress;	/* Use the address returned by the pasv command */

	int				nb;		/* "nonblocking" transfer in progress */
	databuf_t		*data;	/* Data connection for "nonblocking" transfers */
	php_stream		*stream; /* output stream for "nonblocking" transfers */
	int				lastch;		/* last char of previous call */
	int				direction;	/* recv = 0 / send = 1 */
	int				closestream;/* close or not close stream */
#ifdef HAVE_FTP_SSL
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
ftpbuf_t*	ftp_open(const char *host, short port, zend_long timeout_sec);

/* quits from the ftp session (it still needs to be closed)
 * return true on success, false on error
 */
int		ftp_quit(ftpbuf_t *ftp);

/* frees up any cached data held in the ftp buffer */
void		ftp_gc(ftpbuf_t *ftp);

/* close the FTP connection and return NULL */
ftpbuf_t*	ftp_close(ftpbuf_t *ftp);

/* logs into the FTP server, returns true on success, false on error */
int		ftp_login(ftpbuf_t *ftp, const char *user, const size_t user_len, const char *pass, const size_t pass_len);

/* reinitializes the connection, returns true on success, false on error */
int		ftp_reinit(ftpbuf_t *ftp);

/* returns the remote system type (NULL on error) */
const char*	ftp_syst(ftpbuf_t *ftp);

/* returns the present working directory (NULL on error) */
const char*	ftp_pwd(ftpbuf_t *ftp);

/* exec a command [special features], return true on success, false on error */
int 	ftp_exec(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len);

/* send a raw ftp command, return response as a hashtable, NULL on error */
void	ftp_raw(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len, zval *return_value);

/* changes directories, return true on success, false on error */
int		ftp_chdir(ftpbuf_t *ftp, const char *dir, const size_t dir_len);

/* changes to parent directory, return true on success, false on error */
int		ftp_cdup(ftpbuf_t *ftp);

/* creates a directory, return the directory name on success, NULL on error.
 * the return value must be freed
 */
zend_string* ftp_mkdir(ftpbuf_t *ftp, const char *dir, const size_t dir_len);

/* removes a directory, return true on success, false on error */
int		ftp_rmdir(ftpbuf_t *ftp, const char *dir, const size_t dir_len);

/* Set permissions on a file */
int		ftp_chmod(ftpbuf_t *ftp, const int mode, const char *filename, const int filename_len);

/* Allocate space on remote server with ALLO command
 * Many servers will respond with 202 Allocation not necessary,
 * however some servers will not accept STOR or APPE until ALLO is confirmed.
 * If response is passed, it is estrdup()ed from ftp->inbuf and must be freed
 * or assigned to a zval returned to the user */
int		ftp_alloc(ftpbuf_t *ftp, const zend_long size, zend_string **response);

/* returns a NULL-terminated array of filenames in the given path
 * or NULL on error.  the return array must be freed (but don't
 * free the array elements)
 */
char**		ftp_nlist(ftpbuf_t *ftp, const char *path, const size_t path_len);

/* returns a NULL-terminated array of lines returned by the ftp
 * LIST command for the given path or NULL on error.  the return
 * array must be freed (but don't
 * free the array elements)
 */
char**		ftp_list(ftpbuf_t *ftp, const char *path, const size_t path_len, int recursive);

/* populates a hashtable with the facts contained in one line of
 * an MLSD response.
 */
int			ftp_mlsd_parse_line(HashTable *ht, const char *input);

/* returns a NULL-terminated array of lines returned by the ftp
 * MLSD command for the given path or NULL on error.  the return
 * array must be freed (but don't
 * free the array elements)
 */
char**		ftp_mlsd(ftpbuf_t *ftp, const char *path, const size_t path_len);

/* switches passive mode on or off
 * returns true on success, false on error
 */
int		ftp_pasv(ftpbuf_t *ftp, int pasv);

/* retrieves a file and saves its contents to outfp
 * returns true on success, false on error
 */
int		ftp_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, const size_t path_len, ftptype_t type, zend_long resumepos);

/* stores the data from a file, socket, or process as a file on the remote server
 * returns true on success, false on error
 */
int		ftp_put(ftpbuf_t *ftp, const char *path, const size_t path_len, php_stream *instream, ftptype_t type, zend_long startpos);

/* append the data from a file, socket, or process as a file on the remote server
 * returns true on success, false on error
 */
int		ftp_append(ftpbuf_t *ftp, const char *path, const size_t path_len, php_stream *instream, ftptype_t type);

/* returns the size of the given file, or -1 on error */
zend_long		ftp_size(ftpbuf_t *ftp, const char *path, const size_t path_len);

/* returns the last modified time of the given file, or -1 on error */
time_t		ftp_mdtm(ftpbuf_t *ftp, const char *path, const size_t path_len);

/* renames a file on the server */
int		ftp_rename(ftpbuf_t *ftp, const char *src, const size_t src_len, const char *dest, const size_t dest_len);

/* deletes the file from the server */
int		ftp_delete(ftpbuf_t *ftp, const char *path, const size_t path_len);

/* sends a SITE command to the server */
int		ftp_site(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len);

/* retrieves part of a file and saves its contents to outfp
 * returns true on success, false on error
 */
int		ftp_nb_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, const size_t path_len, ftptype_t type, zend_long resumepos);

/* stores the data from a file, socket, or process as a file on the remote server
 * returns true on success, false on error
 */
int		ftp_nb_put(ftpbuf_t *ftp, const char *path, const size_t path_len, php_stream *instream, ftptype_t type, zend_long startpos);

/* continues a previous nb_(f)get command
 */
int		ftp_nb_continue_read(ftpbuf_t *ftp);

/* continues a previous nb_(f)put command
 */
int		ftp_nb_continue_write(ftpbuf_t *ftp);


#endif
