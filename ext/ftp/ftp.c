/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@php.net> (resume functions)            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_FTP

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <string.h>
#include <time.h>
#ifdef PHP_WIN32
#include <winsock2.h>
#elif defined(NETWARE)
#ifdef USE_WINSOCK    /* Modified to use Winsock (NOVSOCK2.H), atleast for now */
#include <novsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#else
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <errno.h>

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#if HAVE_OPENSSL_EXT
#include <openssl/ssl.h>
#endif

#include "ftp.h"
#include "ext/standard/fsock.h"

/* Additional headers for NetWare */
#if defined(NETWARE) && !defined(USE_WINSOCK)
#include <sys/select.h>
#endif

/* sends an ftp command, returns true on success, false on error.
 * it sends the string "cmd args\r\n" if args is non-null, or
 * "cmd\r\n" if args is null
 */
static int		ftp_putcmd(	ftpbuf_t *ftp,
					const char *cmd,
					const char *args);

/* wrapper around send/recv to handle timeouts */
static int		my_send(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t len);
static int		my_recv(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t len);
static int		my_accept(ftpbuf_t *ftp, php_socket_t s, struct sockaddr *addr, socklen_t *addrlen);

/* reads a line the socket , returns true on success, false on error */
static int		ftp_readline(ftpbuf_t *ftp);

/* reads an ftp response, returns true on success, false on error */
static int		ftp_getresp(ftpbuf_t *ftp);

/* sets the ftp transfer type */
static int		ftp_type(ftpbuf_t *ftp, ftptype_t type);

/* opens up a data stream */
static databuf_t*	ftp_getdata(ftpbuf_t *ftp TSRMLS_DC);

/* accepts the data connection, returns updated data buffer */
static databuf_t*	data_accept(databuf_t *data, ftpbuf_t *ftp TSRMLS_DC);

/* closes the data connection, returns NULL */
static databuf_t*	data_close(ftpbuf_t *ftp, databuf_t *data);

/* generic file lister */
static char**		ftp_genlist(ftpbuf_t *ftp, const char *cmd, const char *path TSRMLS_DC);

/* IP and port conversion box */
union ipbox {
	struct in_addr	ia[2];
	unsigned short	s[4];
	unsigned char	c[8];
};

/* {{{ ftp_open
 */
ftpbuf_t*
ftp_open(const char *host, short port, long timeout_sec TSRMLS_DC)
{
	ftpbuf_t		*ftp;
	socklen_t		 size;
	struct timeval tv;


	/* alloc the ftp structure */
	ftp = ecalloc(1, sizeof(*ftp));

	tv.tv_sec = timeout_sec;
	tv.tv_usec = 0;

	ftp->fd = php_network_connect_socket_to_host(host,
			(unsigned short) (port ? port : 21), SOCK_STREAM,
			0, &tv, NULL, NULL, NULL, 0 TSRMLS_CC);
	if (ftp->fd == -1) {
		goto bail;
	}

	/* Default Settings */
	ftp->timeout_sec = timeout_sec;
	ftp->nb = 0;

	size = sizeof(ftp->localaddr);
	memset(&ftp->localaddr, 0, size);
	if (getsockname(ftp->fd, (struct sockaddr*) &ftp->localaddr, &size) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "getsockname failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (!ftp_getresp(ftp) || ftp->resp != 220) {
		goto bail;
	}

	return ftp;

bail:
	if (ftp->fd != -1) {
		closesocket(ftp->fd);
	}
	efree(ftp);
	return NULL;
}
/* }}} */

/* {{{ ftp_close
 */
ftpbuf_t*
ftp_close(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return NULL;
	}
	if (ftp->data) {
		data_close(ftp, ftp->data);
	}
	if (ftp->fd != -1) {
#if HAVE_OPENSSL_EXT
		if (ftp->ssl_active) {
			SSL_shutdown(ftp->ssl_handle);
		}
#endif		
		closesocket(ftp->fd);
	}	
	ftp_gc(ftp);
	efree(ftp);
	return NULL;
}
/* }}} */

/* {{{ ftp_gc
 */
void
ftp_gc(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return;
	}
	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}
	if (ftp->syst) {
		efree(ftp->syst);
		ftp->syst = NULL;
	}
}
/* }}} */

/* {{{ ftp_quit
 */
int
ftp_quit(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}

	if (!ftp_putcmd(ftp, "QUIT", NULL)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 221) {
		return 0;
	}

	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_login
 */
int
ftp_login(ftpbuf_t *ftp, const char *user, const char *pass TSRMLS_DC)
{
#if HAVE_OPENSSL_EXT
	SSL_CTX	*ctx = NULL;
	long ssl_ctx_options = SSL_OP_ALL;
#endif
	if (ftp == NULL) {
		return 0;
	}

#if HAVE_OPENSSL_EXT
	if (ftp->use_ssl && !ftp->ssl_active) {
		if (!ftp_putcmd(ftp, "AUTH", "TLS")) {
			return 0;
		}
		if (!ftp_getresp(ftp)) {
			return 0;
		}
			
		if (ftp->resp != 234) {
			if (!ftp_putcmd(ftp, "AUTH", "SSL")) {
				return 0;
			}
			if (!ftp_getresp(ftp)) {
				return 0;
			}
				
			if (ftp->resp != 334) {
				return 0;
			} else {
				ftp->old_ssl = 1;
				ftp->use_ssl_for_data = 1;
			}
		}
		
		ctx = SSL_CTX_new(SSLv23_client_method());
		if (ctx == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create the SSL context");
			return 0;
		}

#if OPENSSL_VERSION_NUMBER >= 0x0090605fL
		ssl_ctx_options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
#endif
		SSL_CTX_set_options(ctx, ssl_ctx_options);

		ftp->ssl_handle = SSL_new(ctx);
		if (ftp->ssl_handle == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create the SSL handle");
			SSL_CTX_free(ctx);
			return 0;
		}

		SSL_set_fd(ftp->ssl_handle, ftp->fd);

		if (SSL_connect(ftp->ssl_handle) <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSL/TLS handshake failed");
			SSL_shutdown(ftp->ssl_handle);
			return 0;
		}

		ftp->ssl_active = 1;

		if (!ftp->old_ssl) {

			/* set protection buffersize to zero */
			if (!ftp_putcmd(ftp, "PBSZ", "0")) {
				return 0;
			}
			if (!ftp_getresp(ftp)) {
				return 0;
			}

			/* enable data conn encryption */
			if (!ftp_putcmd(ftp, "PROT", "P")) {
				return 0;
			}
			if (!ftp_getresp(ftp)) {
				return 0;
			}
			
			ftp->use_ssl_for_data = (ftp->resp >= 200 && ftp->resp <=299);		
		}
	}
#endif

	if (!ftp_putcmd(ftp, "USER", user)) {
		return 0;
	}
	if (!ftp_getresp(ftp)) {
		return 0;
	}
	if (ftp->resp == 230) {
		return 1;
	}
	if (ftp->resp != 331) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "PASS", pass)) {
		return 0;
	}
	if (!ftp_getresp(ftp)) {
		return 0;
	}
	return (ftp->resp == 230);
}
/* }}} */

/* {{{ ftp_reinit
 */
int
ftp_reinit(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}	

	ftp_gc(ftp);

	ftp->nb = 0;

	if (!ftp_putcmd(ftp, "REIN", NULL)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 220) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_syst
 */
const char*
ftp_syst(ftpbuf_t *ftp)
{
	char *syst, *end;

	if (ftp == NULL) {
		return NULL;
	}

	/* default to cached value */
	if (ftp->syst) {
		return ftp->syst;
	}
	if (!ftp_putcmd(ftp, "SYST", NULL)) {
		return NULL;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 215) { 
		return NULL;
	}
	syst = ftp->inbuf;
	while (*syst == ' ') {
		syst++;
	}
	if ((end = strchr(syst, ' '))) {
		*end = 0;
	}
	ftp->syst = estrdup(syst);
	if (end) {
		*end = ' ';
	}
	return ftp->syst;
}
/* }}} */

/* {{{ ftp_pwd
 */
const char*
ftp_pwd(ftpbuf_t *ftp)
{
	char *pwd, *end;

	if (ftp == NULL) {
		return NULL;
	}

	/* default to cached value */
	if (ftp->pwd) {
		return ftp->pwd;
	}
	if (!ftp_putcmd(ftp, "PWD", NULL)) {
		return NULL;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 257) { 
		return NULL;
	}
	/* copy out the pwd from response */
	if ((pwd = strchr(ftp->inbuf, '"')) == NULL) { 
		return NULL;
	}
	if ((end = strrchr(++pwd, '"')) == NULL) { 
		return NULL;
	}
	ftp->pwd = estrndup(pwd, end - pwd);

	return ftp->pwd;
}
/* }}} */

/* {{{ ftp_exec
 */
int
ftp_exec(ftpbuf_t *ftp, const char *cmd)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "SITE EXEC", cmd)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_raw
 */
void
ftp_raw(ftpbuf_t *ftp, const char *cmd, zval *return_value)
{
	if (ftp == NULL || cmd == NULL) {
		RETURN_NULL();
	}
	if (!ftp_putcmd(ftp, cmd, NULL)) {
		RETURN_NULL();
	}
	array_init(return_value);
	while (ftp_readline(ftp)) {
		add_next_index_string(return_value, ftp->inbuf, 1);
		if (isdigit(ftp->inbuf[0]) && isdigit(ftp->inbuf[1]) && isdigit(ftp->inbuf[2]) && ftp->inbuf[3] == ' ') {
			return;
		}
	}
}
/* }}} */

/* {{{ ftp_chdir
 */
int
ftp_chdir(ftpbuf_t *ftp, const char *dir)
{
	if (ftp == NULL) {
		return 0;
	}

	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}

	if (!ftp_putcmd(ftp, "CWD", dir)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_cdup
 */
int
ftp_cdup(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}

	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}

	if (!ftp_putcmd(ftp, "CDUP", NULL)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_mkdir
 */
char*
ftp_mkdir(ftpbuf_t *ftp, const char *dir)
{
	char *mkd, *end;

	if (ftp == NULL) {
		return NULL;
	}
	if (!ftp_putcmd(ftp, "MKD", dir)) {
		return NULL;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 257) {
		return NULL;
	}
	/* copy out the dir from response */
	if ((mkd = strchr(ftp->inbuf, '"')) == NULL) {
		mkd = estrdup(dir);
		return mkd;
	}
	if ((end = strrchr(++mkd, '"')) == NULL) {
		return NULL;
	}
	*end = 0;
	mkd = estrdup(mkd);
	*end = '"';

	return mkd;
}
/* }}} */

/* {{{ ftp_rmdir
 */
int
ftp_rmdir(ftpbuf_t *ftp, const char *dir)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RMD", dir)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_chmod
 */
int
ftp_chmod(ftpbuf_t *ftp, const int mode, const char *filename, const int filename_len)
{
	char *buffer;

	if (ftp == NULL || filename_len <= 0) {
		return 0;
	}

	spprintf(&buffer, 0, "CHMOD %o %s", mode, filename);

	if (!ftp_putcmd(ftp, "SITE", buffer)) {
		efree(buffer);
		return 0;
	}

	efree(buffer);

	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		return 0;
	}
	
	return 1;
}
/* }}} */

/* {{{ ftp_alloc
 */
int
ftp_alloc(ftpbuf_t *ftp, const int size, char **response)
{
	char buffer[64];

	if (ftp == NULL || size <= 0) {
		return 0;
	}

	snprintf(buffer, sizeof(buffer) - 1, "%d", size);

	if (!ftp_putcmd(ftp, "ALLO", buffer)) {
		return 0;
	}

	if (!ftp_getresp(ftp)) {
		return 0;
	}

	if (response && ftp->inbuf) {
		*response = estrdup(ftp->inbuf);
	}

	if (ftp->resp < 200 || ftp->resp >= 300) {
		return 0;
	}

	return 1;	
}
/* }}} */

/* {{{ ftp_nlist
 */
char**
ftp_nlist(ftpbuf_t *ftp, const char *path TSRMLS_DC)
{
	return ftp_genlist(ftp, "NLST", path TSRMLS_CC);
}
/* }}} */

/* {{{ ftp_list
 */
char**
ftp_list(ftpbuf_t *ftp, const char *path, int recursive TSRMLS_DC)
{
	return ftp_genlist(ftp, ((recursive) ? "LIST -R" : "LIST"), path TSRMLS_CC);
}
/* }}} */

/* {{{ ftp_type
 */
int
ftp_type(ftpbuf_t *ftp, ftptype_t type)
{
	char typechar[2] = "?";

	if (ftp == NULL) {
		return 0;
	}
	if (type == ftp->type) { 
		return 1;
	}
	if (type == FTPTYPE_ASCII) {
		typechar[0] = 'A';
	} else if (type == FTPTYPE_IMAGE) {
		typechar[0] = 'I';
	} else {
		return 0;
	}
	if (!ftp_putcmd(ftp, "TYPE", typechar)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		return 0;
	}
	ftp->type = type;

	return 1;
}
/* }}} */

/* {{{ ftp_pasv
 */
int
ftp_pasv(ftpbuf_t *ftp, int pasv)
{
	char			*ptr;
	union ipbox		ipbox;
	unsigned long		b[6];
	socklen_t			n;
	struct sockaddr *sa;
	struct sockaddr_in *sin;

	if (ftp == NULL) {
		return 0;
	}
	if (pasv && ftp->pasv == 2) {
		return 1;
	}
	ftp->pasv = 0;
	if (!pasv) {
		return 1;
	}
	n = sizeof(ftp->pasvaddr);
	memset(&ftp->pasvaddr, 0, n);
	sa = (struct sockaddr *) &ftp->pasvaddr;

#if HAVE_IPV6
	if (getpeername(ftp->fd, sa, &n) < 0) {
		return 0;
	}
	if (sa->sa_family == AF_INET6) {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		char *endptr, delimiter;

		/* try EPSV first */
		if (!ftp_putcmd(ftp, "EPSV", NULL)) {
			return 0;
		}
		if (!ftp_getresp(ftp)) {
			return 0;
		}
		if (ftp->resp == 229) {
			/* parse out the port */
			for (ptr = ftp->inbuf; *ptr && *ptr != '('; ptr++);
			if (!*ptr) {
				return 0;
			}
			delimiter = *++ptr;
			for (n = 0; *ptr && n < 3; ptr++) {
				if (*ptr == delimiter) {
					n++;
				}
			}

			sin6->sin6_port = htons((unsigned short) strtoul(ptr, &endptr, 10));
			if (ptr == endptr || *endptr != delimiter) {
				return 0;
			}
			ftp->pasv = 2;
			return 1;
		}
	}

	/* fall back to PASV */
#endif

	if (!ftp_putcmd(ftp, "PASV", NULL)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 227) { 
		return 0;
	}
	/* parse out the IP and port */
	for (ptr = ftp->inbuf; *ptr && !isdigit(*ptr); ptr++);
	n = sscanf(ptr, "%lu,%lu,%lu,%lu,%lu,%lu", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]);
	if (n != 6) {
		return 0;
	}
	for (n = 0; n < 6; n++) {
		ipbox.c[n] = (unsigned char) b[n];
	}
	sin = (struct sockaddr_in *) sa;
	sin->sin_family = AF_INET;
	sin->sin_addr = ipbox.ia[0];
	sin->sin_port = ipbox.s[2];

	ftp->pasv = 2;

	return 1;
}
/* }}} */

/* {{{ ftp_get
 */
int
ftp_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, ftptype_t type, int resumepos TSRMLS_DC)
{
	databuf_t		*data = NULL;
	int			lastch;
	size_t			rcvd;
	char			arg[11];

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	
	ftp->data = data;

	if (resumepos > 0) {
		if (resumepos > 2147483647) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "PHP cannot handle files greater than 2147483647 bytes.");
			goto bail;
		}
		snprintf(arg, sizeof(arg), "%u", resumepos);
		if (!ftp_putcmd(ftp, "REST", arg)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "RETR", path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}

	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}

	lastch = 0;
	while ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == -1) {
			goto bail;
		}

		if (type == FTPTYPE_ASCII) {
#ifndef PHP_WIN32
			char *s;
#endif
			char *ptr = data->buf;
			char *e = ptr + rcvd;
			/* logic depends on the OS EOL
			 * Win32 -> \r\n
			 * Everything Else \n
			 */
#ifdef PHP_WIN32
			php_stream_write(outstream, ptr, (e - ptr));
			ptr = e;
#else
			while (e > ptr && (s = memchr(ptr, '\r', (e - ptr)))) {
				php_stream_write(outstream, ptr, (s - ptr));
				if (*(s + 1) == '\n') {
					s++;
					php_stream_putc(outstream, '\n');
				}
				ptr = s + 1;
			}
#endif
			if (ptr < e) {
				php_stream_write(outstream, ptr, (e - ptr));
			}
		} else if (rcvd != php_stream_write(outstream, data->buf, rcvd)) {
			goto bail;
		}
	}

	ftp->data = data = data_close(ftp, data);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}

	return 1;
bail:
	ftp->data = data_close(ftp, data);
	return 0;
}
/* }}} */

/* {{{ ftp_put
 */
int
ftp_put(ftpbuf_t *ftp, const char *path, php_stream *instream, ftptype_t type, int startpos TSRMLS_DC)
{
	databuf_t		*data = NULL;
	int			size;
	char			*ptr;
	int			ch;
	char			arg[11];

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}
	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	ftp->data = data;	

	if (startpos > 0) {
		if (startpos > 2147483647) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "PHP cannot handle files with a size greater than 2147483647 bytes.");
			goto bail;
		}
		snprintf(arg, sizeof(arg), "%u", startpos);
		if (!ftp_putcmd(ftp, "REST", arg)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "STOR", path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}
	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}

	size = 0;
	ptr = data->buf;
	while (!php_stream_eof(instream) && (ch = php_stream_getc(instream))!=EOF) {
		/* flush if necessary */
		if (FTP_BUFSIZE - size < 2) {
			if (my_send(ftp, data->fd, data->buf, size) != size) {
				goto bail;
			}
			ptr = data->buf;
			size = 0;
		}

		if (ch == '\n' && type == FTPTYPE_ASCII) {
			*ptr++ = '\r';
			size++;
		}

		*ptr++ = ch;
		size++;
	}

	if (size && my_send(ftp, data->fd, data->buf, size) != size) {
		goto bail;
	}
	ftp->data = data = data_close(ftp, data);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250 && ftp->resp != 200)) {
		goto bail;
	}
	return 1;
bail:
	ftp->data = data_close(ftp, data);
	return 0;
}
/* }}} */

/* {{{ ftp_size
 */
int
ftp_size(ftpbuf_t *ftp, const char *path)
{
	if (ftp == NULL) {
		return -1;
	}
	if (!ftp_type(ftp, FTPTYPE_IMAGE)) {
		return -1;
	}
	if (!ftp_putcmd(ftp, "SIZE", path)) {
		return -1;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 213) {
		return -1;
	}
	return atoi(ftp->inbuf);
}
/* }}} */

/* {{{ ftp_mdtm
 */
time_t
ftp_mdtm(ftpbuf_t *ftp, const char *path)
{
	time_t		stamp;
	struct tm	*gmt, tmbuf;
	struct tm	tm;
	char		*ptr;
	int		n;

	if (ftp == NULL) {
		return -1;
	}
	if (!ftp_putcmd(ftp, "MDTM", path)) {
		return -1;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 213) {
		return -1;
	}
	/* parse out the timestamp */
	for (ptr = ftp->inbuf; *ptr && !isdigit(*ptr); ptr++);
	n = sscanf(ptr, "%4u%2u%2u%2u%2u%2u", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	if (n != 6) {
		return -1;
	}
	tm.tm_year -= 1900;
	tm.tm_mon--;
	tm.tm_isdst = -1;

	/* figure out the GMT offset */
	stamp = time(NULL);
	gmt = php_gmtime_r(&stamp, &tmbuf);
	if (!gmt) {
		return -1;
	}
	gmt->tm_isdst = -1;

	/* apply the GMT offset */
	tm.tm_sec += stamp - mktime(gmt);
	tm.tm_isdst = gmt->tm_isdst;

	stamp = mktime(&tm);

	return stamp;
}
/* }}} */

/* {{{ ftp_delete
 */
int
ftp_delete(ftpbuf_t *ftp, const char *path)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "DELE", path)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_rename
 */
int
ftp_rename(ftpbuf_t *ftp, const char *src, const char *dest)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RNFR", src)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 350) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RNTO", dest)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_site
 */
int
ftp_site(ftpbuf_t *ftp, const char *cmd)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "SITE", cmd)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp < 200 || ftp->resp >= 300) {
		return 0;
	}

	return 1;
}
/* }}} */

/* static functions */

/* {{{ ftp_putcmd
 */
int
ftp_putcmd(ftpbuf_t *ftp, const char *cmd, const char *args)
{
	int		size;
	char		*data;

	if (strpbrk(cmd, "\r\n")) {
		return 0;
	} 
	/* build the output buffer */
	if (args && args[0]) {
		/* "cmd args\r\n\0" */
		if (strlen(cmd) + strlen(args) + 4 > FTP_BUFSIZE) {
			return 0;
		}
		if (strpbrk(args, "\r\n")) {
			return 0;
		}
		size = slprintf(ftp->outbuf, sizeof(ftp->outbuf), "%s %s\r\n", cmd, args);
	} else {
		/* "cmd\r\n\0" */
		if (strlen(cmd) + 3 > FTP_BUFSIZE) {
			return 0;
		}
		size = slprintf(ftp->outbuf, sizeof(ftp->outbuf), "%s\r\n", cmd);
	}

	data = ftp->outbuf;

	/* Clear the extra-lines buffer */
	ftp->extra = NULL;

	if (my_send(ftp, ftp->fd, data, size) != size) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_readline
 */
int
ftp_readline(ftpbuf_t *ftp)
{
	int		size, rcvd;
	char		*data, *eol;

	/* shift the extra to the front */
	size = FTP_BUFSIZE;
	rcvd = 0;
	if (ftp->extra) {
		memmove(ftp->inbuf, ftp->extra, ftp->extralen);
		rcvd = ftp->extralen;
	}

	data = ftp->inbuf;

	do {
		size -= rcvd;
		for (eol = data; rcvd; rcvd--, eol++) {
			if (*eol == '\r') {
				*eol = 0;
				ftp->extra = eol + 1;
				if (rcvd > 1 && *(eol + 1) == '\n') {
					ftp->extra++;
					rcvd--;
				}
				if ((ftp->extralen = --rcvd) == 0) {
					ftp->extra = NULL;
				}
				return 1;
			} else if (*eol == '\n') {
				*eol = 0;
				ftp->extra = eol + 1;
				if ((ftp->extralen = --rcvd) == 0) {
					ftp->extra = NULL;
				}
				return 1;
			}
		}

		data = eol;
		if ((rcvd = my_recv(ftp, ftp->fd, data, size)) < 1) {
			return 0;
		}
	} while (size);

	return 0;
}
/* }}} */

/* {{{ ftp_getresp
 */
int
ftp_getresp(ftpbuf_t *ftp)
{
	char *buf;

	if (ftp == NULL) {
		return 0;
	}
	buf = ftp->inbuf;
	ftp->resp = 0;

	while (1) {

		if (!ftp_readline(ftp)) {
			return 0;
		}

		/* Break out when the end-tag is found */
		if (isdigit(ftp->inbuf[0]) && isdigit(ftp->inbuf[1]) && isdigit(ftp->inbuf[2]) && ftp->inbuf[3] == ' ') {
			break;
		}
	}

	/* translate the tag */
	if (!isdigit(ftp->inbuf[0]) || !isdigit(ftp->inbuf[1]) || !isdigit(ftp->inbuf[2])) {
		return 0;
	}

	ftp->resp = 100 * (ftp->inbuf[0] - '0') + 10 * (ftp->inbuf[1] - '0') + (ftp->inbuf[2] - '0');

	memmove(ftp->inbuf, ftp->inbuf + 4, FTP_BUFSIZE - 4);

	if (ftp->extra) {
		ftp->extra -= 4;
	}
	return 1;
}
/* }}} */

/* {{{ my_send
 */
int
my_send(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t len)
{
	int		n, size, sent;

	size = len;
	while (size) {
		n = php_pollfd_for_ms(s, POLLOUT, ftp->timeout_sec * 1000);

		if (n < 1) {

#if !defined(PHP_WIN32) && !(defined(NETWARE) && defined(USE_WINSOCK))
			if (n == 0) {
				errno = ETIMEDOUT;
			}
#endif
			return -1;
		}

#if HAVE_OPENSSL_EXT
		if (ftp->use_ssl && ftp->fd == s && ftp->ssl_active) {
			sent = SSL_write(ftp->ssl_handle, buf, size);
		} else if (ftp->use_ssl && ftp->fd != s && ftp->use_ssl_for_data && ftp->data->ssl_active) {	
			sent = SSL_write(ftp->data->ssl_handle, buf, size);
		} else {
#endif
			sent = send(s, buf, size, 0);
#if HAVE_OPENSSL_EXT
		}
#endif
		if (sent == -1) {
			return -1;
		}

		buf = (char*) buf + sent;
		size -= sent;
	}

	return len;
}
/* }}} */

/* {{{ my_recv
 */
int
my_recv(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t len)
{
	int		n, nr_bytes;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, ftp->timeout_sec * 1000);
	if (n < 1) {
#if !defined(PHP_WIN32) && !(defined(NETWARE) && defined(USE_WINSOCK))
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return -1;
	}

#if HAVE_OPENSSL_EXT
	if (ftp->use_ssl && ftp->fd == s && ftp->ssl_active) {
		nr_bytes = SSL_read(ftp->ssl_handle, buf, len);
	} else if (ftp->use_ssl && ftp->fd != s && ftp->use_ssl_for_data && ftp->data->ssl_active) {	
		nr_bytes = SSL_read(ftp->data->ssl_handle, buf, len);
	} else {
#endif
		nr_bytes = recv(s, buf, len, 0);
#if HAVE_OPENSSL_EXT
	}
#endif	
	return (nr_bytes);
}
/* }}} */

/* {{{ data_available
 */
int
data_available(ftpbuf_t *ftp, php_socket_t s)
{
	int		n;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, 1000);
	if (n < 1) {
#if !defined(PHP_WIN32) && !(defined(NETWARE) && defined(USE_WINSOCK))
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return 0;
	}

	return 1;
}
/* }}} */
/* {{{ data_writeable
 */
int
data_writeable(ftpbuf_t *ftp, php_socket_t s)
{
	int		n;

	n = php_pollfd_for_ms(s, POLLOUT, 1000);
	if (n < 1) {
#ifndef PHP_WIN32
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ my_accept
 */
int
my_accept(ftpbuf_t *ftp, php_socket_t s, struct sockaddr *addr, socklen_t *addrlen)
{
	int		n;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, ftp->timeout_sec * 1000);
	if (n < 1) {
#if !defined(PHP_WIN32) && !(defined(NETWARE) && defined(USE_WINSOCK))
		if (n == 0) {
			errno = ETIMEDOUT;
		}
#endif
		return -1;
	}

	return accept(s, addr, addrlen);
}
/* }}} */

/* {{{ ftp_getdata
 */
databuf_t*
ftp_getdata(ftpbuf_t *ftp TSRMLS_DC)
{
	int			fd = -1;
	databuf_t		*data;
	php_sockaddr_storage addr;
	struct sockaddr *sa;
	socklen_t		size;
	union ipbox		ipbox;
	char			arg[sizeof("255, 255, 255, 255, 255, 255")];
	struct timeval	tv;


	/* ask for a passive connection if we need one */
	if (ftp->pasv && !ftp_pasv(ftp, 1)) {
		return NULL;
	}
	/* alloc the data structure */
	data = ecalloc(1, sizeof(*data));
	data->listener = -1;
	data->fd = -1;
	data->type = ftp->type;

	sa = (struct sockaddr *) &ftp->localaddr;
	/* bind/listen */
	if ((fd = socket(sa->sa_family, SOCK_STREAM, 0)) == SOCK_ERR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "socket() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	/* passive connection handler */
	if (ftp->pasv) {
		/* clear the ready status */
		ftp->pasv = 1;

		/* connect */
		/* Win 95/98 seems not to like size > sizeof(sockaddr_in) */
		size = php_sockaddr_size(&ftp->pasvaddr);
		tv.tv_sec = ftp->timeout_sec;
		tv.tv_usec = 0;
		if (php_connect_nonb(fd, (struct sockaddr*) &ftp->pasvaddr, size, &tv) == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_connect_nonb() failed: %s (%d)", strerror(errno), errno);
			goto bail;
		}

		data->fd = fd;

		ftp->data = data;
		return data;
	}


	/* active (normal) connection */

	/* bind to a local address */
	php_any_addr(sa->sa_family, &addr, 0);
	size = php_sockaddr_size(&addr);

	if (bind(fd, (struct sockaddr*) &addr, size) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "bind() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (getsockname(fd, (struct sockaddr*) &addr, &size) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "getsockname() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (listen(fd, 5) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "listen() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	data->listener = fd;

#if HAVE_IPV6 && HAVE_INET_NTOP
	if (sa->sa_family == AF_INET6) {
		/* need to use EPRT */
		char eprtarg[INET6_ADDRSTRLEN + sizeof("|x||xxxxx|")];
		char out[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &((struct sockaddr_in6*) sa)->sin6_addr, out, sizeof(out));
		snprintf(eprtarg, sizeof(eprtarg), "|2|%s|%hu|", out, ntohs(((struct sockaddr_in6 *) &addr)->sin6_port));

		if (!ftp_putcmd(ftp, "EPRT", eprtarg)) {
			goto bail;
		}

		if (!ftp_getresp(ftp) || ftp->resp != 200) {
			goto bail;
		}

		ftp->data = data;
		return data;
	}
#endif

	/* send the PORT */
	ipbox.ia[0] = ((struct sockaddr_in*) sa)->sin_addr;
	ipbox.s[2] = ((struct sockaddr_in*) &addr)->sin_port;
	snprintf(arg, sizeof(arg), "%u,%u,%u,%u,%u,%u", ipbox.c[0], ipbox.c[1], ipbox.c[2], ipbox.c[3], ipbox.c[4], ipbox.c[5]);

	if (!ftp_putcmd(ftp, "PORT", arg)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		goto bail;
	}

	ftp->data = data;
	return data;

bail:
	if (fd != -1) {
		closesocket(fd);
	}
	efree(data);
	return NULL;
}
/* }}} */

/* {{{ data_accept
 */
databuf_t*
data_accept(databuf_t *data, ftpbuf_t *ftp TSRMLS_DC)
{
	php_sockaddr_storage addr;
	socklen_t			size;

#if HAVE_OPENSSL_EXT
	SSL_CTX		*ctx;
	long ssl_ctx_options = SSL_OP_ALL;
#endif

	if (data->fd != -1) {
		goto data_accepted;
	}
	size = sizeof(addr);
	data->fd = my_accept(ftp, data->listener, (struct sockaddr*) &addr, &size);
	closesocket(data->listener);
	data->listener = -1;

	if (data->fd == -1) {
		efree(data);
		return NULL;
	}

data_accepted:
#if HAVE_OPENSSL_EXT
	
	/* now enable ssl if we need to */
	if (ftp->use_ssl && ftp->use_ssl_for_data) {
		ctx = SSL_CTX_new(SSLv23_client_method());
		if (ctx == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_accept: failed to create the SSL context");
			return 0;
		}

#if OPENSSL_VERSION_NUMBER >= 0x0090605fL
		ssl_ctx_options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
#endif
		SSL_CTX_set_options(ctx, ssl_ctx_options);

		data->ssl_handle = SSL_new(ctx);
		if (data->ssl_handle == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_accept: failed to create the SSL handle");
			SSL_CTX_free(ctx);
			return 0;
		}
			
		
		SSL_set_fd(data->ssl_handle, data->fd);

		if (ftp->old_ssl) {
			SSL_copy_session_id(data->ssl_handle, ftp->ssl_handle);
		}
			
		if (SSL_connect(data->ssl_handle) <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_accept: SSL/TLS handshake failed");
			SSL_shutdown(data->ssl_handle);
			return 0;
		}
			
		data->ssl_active = 1;
	}	

#endif

	return data;
}
/* }}} */

/* {{{ data_close
 */
databuf_t*
data_close(ftpbuf_t *ftp, databuf_t *data)
{
	if (data == NULL) {
		return NULL;
	}
	if (data->listener != -1) {
#if HAVE_OPENSSL_EXT
		if (data->ssl_active) {
			SSL_shutdown(data->ssl_handle);
			data->ssl_active = 0;
		}
#endif				
		closesocket(data->listener);
	}	
	if (data->fd != -1) {
#if HAVE_OPENSSL_EXT
		if (data->ssl_active) {
			SSL_shutdown(data->ssl_handle);
			data->ssl_active = 0;
		}
#endif				
		closesocket(data->fd);
	}	
	if (ftp) {
		ftp->data = NULL;
	}
	efree(data);
	return NULL;
}
/* }}} */

/* {{{ ftp_genlist
 */
char**
ftp_genlist(ftpbuf_t *ftp, const char *cmd, const char *path TSRMLS_DC)
{
	php_stream	*tmpstream = NULL;
	databuf_t	*data = NULL;
	char		*ptr;
	int		ch, lastch;
	int		size, rcvd;
	int		lines;
	char		**ret = NULL;
	char		**entry;
	char		*text;


	if ((tmpstream = php_stream_fopen_tmpfile()) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create temporary file.  Check permissions in temporary files directory.");
		return NULL;
	}

	if (!ftp_type(ftp, FTPTYPE_ASCII)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	ftp->data = data;	

	if (!ftp_putcmd(ftp, cmd, path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125 && ftp->resp != 226)) {
		goto bail;
	}

	/* some servers don't open a ftp-data connection if the directory is empty */
	if (ftp->resp == 226) {
		ftp->data = data_close(ftp, data);
		php_stream_close(tmpstream);
		return ecalloc(1, sizeof(char**));
	}

	/* pull data buffer into tmpfile */
	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	size = 0;
	lines = 0;
	lastch = 0;
	while ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == -1) {
			goto bail;
		}

		php_stream_write(tmpstream, data->buf, rcvd);

		size += rcvd;
		for (ptr = data->buf; rcvd; rcvd--, ptr++) {
			if (*ptr == '\n' && lastch == '\r') {
				lines++;
			} else {
				size++;
			}
			lastch = *ptr;
		}
	}

	ftp->data = data = data_close(ftp, data);

	php_stream_rewind(tmpstream);

	ret = safe_emalloc((lines + 1), sizeof(char**), size * sizeof(char*));

	entry = ret;
	text = (char*) (ret + lines + 1);
	*entry = text;
	lastch = 0;
	while ((ch = php_stream_getc(tmpstream)) != EOF) {
		if (ch == '\n' && lastch == '\r') {
			*(text - 1) = 0;
			*++entry = text;
		} else {
			*text++ = ch;
		}
		lastch = ch;
	}
	*entry = NULL;

	php_stream_close(tmpstream);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		efree(ret);
		return NULL;
	}

	return ret;
bail:
	ftp->data = data_close(ftp, data);
	php_stream_close(tmpstream);
	if (ret)
		efree(ret);
	return NULL;
}
/* }}} */

/* {{{ ftp_nb_get
 */
int
ftp_nb_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, ftptype_t type, int resumepos TSRMLS_DC)
{
	databuf_t		*data = NULL;
	char			arg[11];

	if (ftp == NULL) {
		return PHP_FTP_FAILED;
	}

	if (!ftp_type(ftp, type)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}

	if (resumepos>0) {
		/* We are working on an architecture that supports 64-bit integers
		 * since php is 32 bit by design, we bail out with warning
		 */
		if (resumepos > 2147483647) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "PHP cannot handle files greater than 2147483648 bytes.");
			goto bail;
		}
		snprintf(arg, sizeof(arg), "%u", resumepos);
		if (!ftp_putcmd(ftp, "REST", arg)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "RETR", path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}

	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}

	ftp->data = data;
	ftp->stream = outstream;
	ftp->lastch = 0;
	ftp->nb = 1;

	return (ftp_nb_continue_read(ftp TSRMLS_CC));

bail:
	ftp->data = data_close(ftp, data);
	return PHP_FTP_FAILED;
}
/* }}} */

/* {{{ ftp_nb_continue_read
 */
int
ftp_nb_continue_read(ftpbuf_t *ftp TSRMLS_DC)
{
	databuf_t	*data = NULL;
	char		*ptr;
	int		lastch;
	size_t		rcvd;
	ftptype_t	type;

	data = ftp->data;

	/* check if there is already more data */
	if (!data_available(ftp, data->fd)) {
		return PHP_FTP_MOREDATA;
	}

	type = ftp->type;

	lastch = ftp->lastch;
	if ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == -1) {
			goto bail;
		}

		if (type == FTPTYPE_ASCII) {
			for (ptr = data->buf; rcvd; rcvd--, ptr++) {
				if (lastch == '\r' && *ptr != '\n') {
					php_stream_putc(ftp->stream, '\r');
				}
				if (*ptr != '\r') {
					php_stream_putc(ftp->stream, *ptr);
				}
				lastch = *ptr;
			}
		} else if (rcvd != php_stream_write(ftp->stream, data->buf, rcvd)) {
			goto bail;
		}

		ftp->lastch = lastch;
		return PHP_FTP_MOREDATA;
	}

	if (type == FTPTYPE_ASCII && lastch == '\r') {
		php_stream_putc(ftp->stream, '\r');
	}

	ftp->data = data = data_close(ftp, data);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}

	ftp->nb = 0;
	return PHP_FTP_FINISHED;
bail:
	ftp->nb = 0;
	ftp->data = data_close(ftp, data);
	return PHP_FTP_FAILED;
}
/* }}} */

/* {{{ ftp_nb_put
 */
int
ftp_nb_put(ftpbuf_t *ftp, const char *path, php_stream *instream, ftptype_t type, int startpos TSRMLS_DC)
{
	databuf_t		*data = NULL;
	char			arg[11];

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}
	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	if (startpos > 0) {
		if (startpos > 2147483647) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "PHP cannot handle files with a size greater than 2147483647 bytes.");
			goto bail;
		}
		snprintf(arg, sizeof(arg), "%u", startpos);
		if (!ftp_putcmd(ftp, "REST", arg)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "STOR", path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}
	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) { 
		goto bail;
	}
	ftp->data = data;
	ftp->stream = instream;
	ftp->lastch = 0;
	ftp->nb = 1;

	return (ftp_nb_continue_write(ftp TSRMLS_CC));

bail:
	ftp->data = data_close(ftp, data);
	return PHP_FTP_FAILED;
}
/* }}} */


/* {{{ ftp_nb_continue_write
 */
int
ftp_nb_continue_write(ftpbuf_t *ftp TSRMLS_DC)
{
	int			size;
	char			*ptr;
	int 			ch;

	/* check if we can write more data */
	if (!data_writeable(ftp, ftp->data->fd)) {
		return PHP_FTP_MOREDATA;
	}

	size = 0;
	ptr = ftp->data->buf;
	while (!php_stream_eof(ftp->stream) && (ch = php_stream_getc(ftp->stream)) != EOF) {

		if (ch == '\n' && ftp->type == FTPTYPE_ASCII) {
			*ptr++ = '\r';
			size++;
		}

		*ptr++ = ch;
		size++;

		/* flush if necessary */
		if (FTP_BUFSIZE - size < 2) {
			if (my_send(ftp, ftp->data->fd, ftp->data->buf, size) != size) {
				goto bail;
			}
			return PHP_FTP_MOREDATA;
		}
	}

	if (size && my_send(ftp, ftp->data->fd, ftp->data->buf, size) != size) {
		goto bail;
	}
	ftp->data = data_close(ftp, ftp->data);
 
	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}
	ftp->nb = 0;
	return PHP_FTP_FINISHED;
bail:
	ftp->data = data_close(ftp, ftp->data);
	ftp->nb = 0;
	return PHP_FTP_FAILED;
}
/* }}} */

#endif /* HAVE_FTP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
