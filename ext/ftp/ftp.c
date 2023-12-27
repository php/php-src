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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

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

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_FTP_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#include "ftp.h"
#include "ext/standard/fsock.h"

#ifdef PHP_WIN32
# undef ETIMEDOUT
# define ETIMEDOUT WSAETIMEDOUT
#endif

/* sends an ftp command, returns true on success, false on error.
 * it sends the string "cmd args\r\n" if args is non-null, or
 * "cmd\r\n" if args is null
 */
static int		ftp_putcmd(	ftpbuf_t *ftp,
					const char *cmd,
					const size_t cmd_len,
					const char *args,
					const size_t args_len);

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
static databuf_t*	ftp_getdata(ftpbuf_t *ftp);

/* accepts the data connection, returns updated data buffer */
static databuf_t*	data_accept(databuf_t *data, ftpbuf_t *ftp);

/* closes the data connection, no-op if already closed */
static void		data_close(ftpbuf_t *ftp);

/* generic file lister */
static char**		ftp_genlist(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len, const char *path, const size_t path_len);

#ifdef HAVE_FTP_SSL
/* shuts down a TLS/SSL connection */
static void		ftp_ssl_shutdown(ftpbuf_t *ftp, php_socket_t fd, SSL *ssl_handle);
#endif

/* IP and port conversion box */
union ipbox {
	struct in_addr	ia[2];
	unsigned short	s[4];
	unsigned char	c[8];
};

/* {{{ ftp_open */
ftpbuf_t*
ftp_open(const char *host, short port, zend_long timeout_sec)
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
			0, &tv, NULL, NULL, NULL, 0, STREAM_SOCKOP_NONE);
	if (ftp->fd == -1) {
		goto bail;
	}

	/* Default Settings */
	ftp->timeout_sec = timeout_sec;
	ftp->nb = 0;

	size = sizeof(ftp->localaddr);
	memset(&ftp->localaddr, 0, size);
	if (getsockname(ftp->fd, (struct sockaddr*) &ftp->localaddr, &size) != 0) {
		php_error_docref(NULL, E_WARNING, "getsockname failed: %s (%d)", strerror(errno), errno);
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

/* {{{ ftp_close */
ftpbuf_t*
ftp_close(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return NULL;
	}
#ifdef HAVE_FTP_SSL
	if (ftp->last_ssl_session) {
		SSL_SESSION_free(ftp->last_ssl_session);
	}
#endif
	data_close(ftp);
	if (ftp->stream && ftp->closestream) {
			php_stream_close(ftp->stream);
	}
	if (ftp->fd != -1) {
#ifdef HAVE_FTP_SSL
		if (ftp->ssl_active) {
			ftp_ssl_shutdown(ftp, ftp->fd, ftp->ssl_handle);
		}
#endif
		closesocket(ftp->fd);
	}
	ftp_gc(ftp);
	efree(ftp);
	return NULL;
}
/* }}} */

/* {{{ ftp_gc */
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

/* {{{ ftp_quit */
int
ftp_quit(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}

	if (!ftp_putcmd(ftp, "QUIT", sizeof("QUIT")-1, NULL, (size_t) 0)) {
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

#ifdef HAVE_FTP_SSL
static int ftp_ssl_new_session_cb(SSL *ssl, SSL_SESSION *sess)
{
	ftpbuf_t *ftp = SSL_get_app_data(ssl);

	/* Technically there can be multiple sessions per connection, but we only care about the most recent one. */
	if (ftp->last_ssl_session) {
		SSL_SESSION_free(ftp->last_ssl_session);
	}
	ftp->last_ssl_session = SSL_get1_session(ssl);

	/* Return 0 as we are not using OpenSSL's session cache. */
	return 0;
}
#endif

/* {{{ ftp_login */
int
ftp_login(ftpbuf_t *ftp, const char *user, const size_t user_len, const char *pass, const size_t pass_len)
{
#ifdef HAVE_FTP_SSL
	SSL_CTX	*ctx = NULL;
	long ssl_ctx_options = SSL_OP_ALL;
	int err, res;
	bool retry;
#endif
	if (ftp == NULL) {
		return 0;
	}

#ifdef HAVE_FTP_SSL
	if (ftp->use_ssl && !ftp->ssl_active) {
		if (!ftp_putcmd(ftp, "AUTH", sizeof("AUTH")-1, "TLS", sizeof("TLS")-1)) {
			return 0;
		}
		if (!ftp_getresp(ftp)) {
			return 0;
		}

		if (ftp->resp != 234) {
			if (!ftp_putcmd(ftp, "AUTH", sizeof("AUTH")-1, "SSL", sizeof("SSL")-1)) {
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
			php_error_docref(NULL, E_WARNING, "Failed to create the SSL context");
			return 0;
		}

#if OPENSSL_VERSION_NUMBER >= 0x0090605fL
		ssl_ctx_options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
#endif
		SSL_CTX_set_options(ctx, ssl_ctx_options);

		/* Allow SSL to re-use sessions.
		 * We're relying on our own session storage as only at most one session will ever be active per FTP connection. */
		SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_BOTH | SSL_SESS_CACHE_NO_INTERNAL);
		SSL_CTX_sess_set_new_cb(ctx, ftp_ssl_new_session_cb);

		ftp->ssl_handle = SSL_new(ctx);
		SSL_set_app_data(ftp->ssl_handle, ftp); /* Needed for ftp_ssl_new_session_cb */
		SSL_CTX_free(ctx);

		if (ftp->ssl_handle == NULL) {
			php_error_docref(NULL, E_WARNING, "Failed to create the SSL handle");
			return 0;
		}

		SSL_set_fd(ftp->ssl_handle, ftp->fd);

		do {
			res = SSL_connect(ftp->ssl_handle);
			err = SSL_get_error(ftp->ssl_handle, res);

			/* TODO check if handling other error codes would make sense */
			switch (err) {
				case SSL_ERROR_NONE:
					retry = 0;
					break;

				case SSL_ERROR_ZERO_RETURN:
					retry = 0;
					SSL_shutdown(ftp->ssl_handle);
					break;

				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE: {
						php_pollfd p;
						int i;

						p.fd = ftp->fd;
						p.events = (err == SSL_ERROR_WANT_READ) ? (POLLIN|POLLPRI) : POLLOUT;
						p.revents = 0;

						i = php_poll2(&p, 1, 300);

						retry = i > 0;
					}
					break;

				default:
					php_error_docref(NULL, E_WARNING, "SSL/TLS handshake failed");
					SSL_shutdown(ftp->ssl_handle);
					SSL_free(ftp->ssl_handle);
					return 0;
			}
		} while (retry);

		ftp->ssl_active = 1;

		if (!ftp->old_ssl) {

			/* set protection buffersize to zero */
			if (!ftp_putcmd(ftp, "PBSZ", sizeof("PBSZ")-1, "0", sizeof("0")-1)) {
				return 0;
			}
			if (!ftp_getresp(ftp)) {
				return 0;
			}

			/* enable data conn encryption */
			if (!ftp_putcmd(ftp, "PROT", sizeof("PROT")-1, "P", sizeof("P")-1)) {
				return 0;
			}
			if (!ftp_getresp(ftp)) {
				return 0;
			}

			ftp->use_ssl_for_data = (ftp->resp >= 200 && ftp->resp <=299);
		}
	}
#endif

	if (!ftp_putcmd(ftp, "USER", sizeof("USER")-1, user, user_len)) {
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
	if (!ftp_putcmd(ftp, "PASS", sizeof("PASS")-1, pass, pass_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp)) {
		return 0;
	}
	return (ftp->resp == 230);
}
/* }}} */

/* {{{ ftp_reinit */
int
ftp_reinit(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}

	ftp_gc(ftp);

	ftp->nb = 0;

	if (!ftp_putcmd(ftp, "REIN", sizeof("REIN")-1, NULL, (size_t) 0)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 220) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_syst */
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
	if (!ftp_putcmd(ftp, "SYST", sizeof("SYST")-1, NULL, (size_t) 0)) {
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

/* {{{ ftp_pwd */
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
	if (!ftp_putcmd(ftp, "PWD", sizeof("PWD")-1, NULL, (size_t) 0)) {
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

/* {{{ ftp_exec */
int
ftp_exec(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "SITE EXEC", sizeof("SITE EXEC")-1, cmd, cmd_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_raw */
void
ftp_raw(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len, zval *return_value)
{
	if (ftp == NULL || cmd == NULL) {
		RETURN_NULL();
	}
	if (!ftp_putcmd(ftp, cmd, cmd_len, NULL, (size_t) 0)) {
		RETURN_NULL();
	}
	array_init(return_value);
	while (ftp_readline(ftp)) {
		add_next_index_string(return_value, ftp->inbuf);
		if (isdigit(ftp->inbuf[0]) && isdigit(ftp->inbuf[1]) && isdigit(ftp->inbuf[2]) && ftp->inbuf[3] == ' ') {
			return;
		}
	}
}
/* }}} */

/* {{{ ftp_chdir */
int
ftp_chdir(ftpbuf_t *ftp, const char *dir, const size_t dir_len)
{
	if (ftp == NULL) {
		return 0;
	}

	if (ftp->pwd) {
		efree(ftp->pwd);
		ftp->pwd = NULL;
	}

	if (!ftp_putcmd(ftp, "CWD", sizeof("CWD")-1, dir, dir_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_cdup */
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

	if (!ftp_putcmd(ftp, "CDUP", sizeof("CDUP")-1, NULL, (size_t) 0)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_mkdir */
zend_string*
ftp_mkdir(ftpbuf_t *ftp, const char *dir, const size_t dir_len)
{
	char *mkd, *end;
	zend_string *ret;

	if (ftp == NULL) {
		return NULL;
	}
	if (!ftp_putcmd(ftp, "MKD", sizeof("MKD")-1, dir, dir_len)) {
		return NULL;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 257) {
		return NULL;
	}
	/* copy out the dir from response */
	if ((mkd = strchr(ftp->inbuf, '"')) == NULL) {
		return zend_string_init(dir, dir_len, 0);
	}
	if ((end = strrchr(++mkd, '"')) == NULL) {
		return NULL;
	}
	*end = 0;
	ret = zend_string_init(mkd, end - mkd, 0);
	*end = '"';

	return ret;
}
/* }}} */

/* {{{ ftp_rmdir */
int
ftp_rmdir(ftpbuf_t *ftp, const char *dir, const size_t dir_len)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RMD", sizeof("RMD")-1, dir, dir_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_chmod */
int
ftp_chmod(ftpbuf_t *ftp, const int mode, const char *filename, const int filename_len)
{
	char *buffer;
	size_t buffer_len;

	if (ftp == NULL || filename_len <= 0) {
		return 0;
	}

	buffer_len = spprintf(&buffer, 0, "CHMOD %o %s", mode, filename);

	if (!buffer) {
		return 0;
	}

	if (!ftp_putcmd(ftp, "SITE", sizeof("SITE")-1, buffer, buffer_len)) {
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

/* {{{ ftp_alloc */
int
ftp_alloc(ftpbuf_t *ftp, const zend_long size, zend_string **response)
{
	char buffer[64];
	int buffer_len;

	if (ftp == NULL || size <= 0) {
		return 0;
	}

	buffer_len = snprintf(buffer, sizeof(buffer) - 1, ZEND_LONG_FMT, size);

	if (buffer_len < 0) {
		return 0;
	}

	if (!ftp_putcmd(ftp, "ALLO", sizeof("ALLO")-1, buffer, buffer_len)) {
		return 0;
	}

	if (!ftp_getresp(ftp)) {
		return 0;
	}

	if (response) {
		*response = zend_string_init(ftp->inbuf, strlen(ftp->inbuf), 0);
	}

	if (ftp->resp < 200 || ftp->resp >= 300) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_nlist */
char**
ftp_nlist(ftpbuf_t *ftp, const char *path, const size_t path_len)
{
	return ftp_genlist(ftp, "NLST", sizeof("NLST")-1, path, path_len);
}
/* }}} */

/* {{{ ftp_list */
char**
ftp_list(ftpbuf_t *ftp, const char *path, const size_t path_len, int recursive)
{
	return ftp_genlist(ftp, ((recursive) ? "LIST -R" : "LIST"), ((recursive) ? sizeof("LIST -R")-1 : sizeof("LIST")-1), path, path_len);
}
/* }}} */

/* {{{ ftp_mlsd */
char**
ftp_mlsd(ftpbuf_t *ftp, const char *path, const size_t path_len)
{
	return ftp_genlist(ftp, "MLSD", sizeof("MLSD")-1, path, path_len);
}
/* }}} */

/* {{{ ftp_mlsd_parse_line */
int
ftp_mlsd_parse_line(HashTable *ht, const char *input) {

	zval zstr;
	const char *end = input + strlen(input);

	const char *sp = memchr(input, ' ', end - input);
	if (!sp) {
		php_error_docref(NULL, E_WARNING, "Missing pathname in MLSD response");
		return FAILURE;
	}

	/* Extract pathname */
	ZVAL_STRINGL(&zstr, sp + 1, end - sp - 1);
	zend_hash_update(ht, ZSTR_KNOWN(ZEND_STR_NAME), &zstr);
	end = sp;

	while (input < end) {
		const char *semi, *eq;

		/* Find end of fact */
		semi = memchr(input, ';', end - input);
		if (!semi) {
			php_error_docref(NULL, E_WARNING, "Malformed fact in MLSD response");
			return FAILURE;
		}

		/* Separate fact key and value */
		eq = memchr(input, '=', semi - input);
		if (!eq) {
			php_error_docref(NULL, E_WARNING, "Malformed fact in MLSD response");
			return FAILURE;
		}

		ZVAL_STRINGL(&zstr, eq + 1, semi - eq - 1);
		zend_hash_str_update(ht, input, eq - input, &zstr);
		input = semi + 1;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ ftp_type */
int
ftp_type(ftpbuf_t *ftp, ftptype_t type)
{
	const char *typechar;

	if (ftp == NULL) {
		return 0;
	}
	if (type == ftp->type) {
		return 1;
	}
	if (type == FTPTYPE_ASCII) {
		typechar = "A";
	} else if (type == FTPTYPE_IMAGE) {
		typechar = "I";
	} else {
		return 0;
	}
	if (!ftp_putcmd(ftp, "TYPE", sizeof("TYPE")-1, typechar, 1)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 200) {
		return 0;
	}
	ftp->type = type;

	return 1;
}
/* }}} */

/* {{{ ftp_pasv */
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

	if (getpeername(ftp->fd, sa, &n) < 0) {
		return 0;
	}

#ifdef HAVE_IPV6
	if (sa->sa_family == AF_INET6) {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		char *endptr, delimiter;

		/* try EPSV first */
		if (!ftp_putcmd(ftp, "EPSV", sizeof("EPSV")-1, NULL, (size_t) 0)) {
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

	if (!ftp_putcmd(ftp, "PASV",  sizeof("PASV")-1, NULL, (size_t) 0)) {
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
	if (ftp->usepasvaddress) {
		sin->sin_addr = ipbox.ia[0];
	}
	sin->sin_port = ipbox.s[2];

	ftp->pasv = 2;

	return 1;
}
/* }}} */

/* {{{ ftp_get */
int
ftp_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, const size_t path_len, ftptype_t type, zend_long resumepos)
{
	databuf_t		*data = NULL;
	size_t			rcvd;
	char			arg[MAX_LENGTH_OF_LONG];

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp)) == NULL) {
		goto bail;
	}

	if (resumepos > 0) {
		int arg_len = snprintf(arg, sizeof(arg), ZEND_LONG_FMT, resumepos);

		if (arg_len < 0) {
			goto bail;
		}
		if (!ftp_putcmd(ftp, "REST", sizeof("REST")-1, arg, arg_len)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "RETR", sizeof("RETR")-1, path, path_len)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}

	if ((data = data_accept(data, ftp)) == NULL) {
		goto bail;
	}

	while ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == (size_t)-1) {
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

	data_close(ftp);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}

	return 1;
bail:
	data_close(ftp);
	return 0;
}
/* }}} */

static zend_result ftp_send_stream_to_data_socket(ftpbuf_t *ftp, databuf_t *data, php_stream *instream, ftptype_t type, bool send_once_and_return)
{
	if (type == FTPTYPE_ASCII) {
		/* Change (and later restore) flags to make sure php_stream_get_line() searches '\n'. */
		const uint32_t flags_mask = PHP_STREAM_FLAG_EOL_UNIX | PHP_STREAM_FLAG_DETECT_EOL | PHP_STREAM_FLAG_EOL_MAC;
		uint32_t old_flags = instream->flags & flags_mask;
		instream->flags = (instream->flags & ~flags_mask) | PHP_STREAM_FLAG_EOL_UNIX;

		char *ptr = data->buf;
		const char *end = data->buf + FTP_BUFSIZE;
		while (!php_stream_eof(instream)) {
			size_t line_length;
			if (!php_stream_get_line(instream, ptr, end - ptr, &line_length)) {
				break;
			}

			ZEND_ASSERT(line_length != 0);

			ptr += line_length - 1;
			/* Replace \n with \r\n */
			if (*ptr == '\n') {
				*ptr = '\r';
				/* The streams layer always puts a \0 byte at the end of a line,
				 * so there is always place to add an extra byte. */
				*++ptr = '\n';
			}

			ptr++;

			/* If less than 2 bytes remain, either the buffer is completely full or there is a single byte left to put a '\0'
			 * which isn't really useful, in this case send and reset the buffer. */
			if (end - ptr < 2) {
				size_t send_size = FTP_BUFSIZE - (end - ptr);
				if (UNEXPECTED(my_send(ftp, data->fd, data->buf, send_size) != send_size)) {
					instream->flags = (instream->flags & ~flags_mask) | old_flags;
					return FAILURE;
				}
				ptr = data->buf;
				if (send_once_and_return) {
					break;
				}
			}
		}

		instream->flags = (instream->flags & ~flags_mask) | old_flags;

		if (end - ptr < FTP_BUFSIZE) {
			size_t send_size = FTP_BUFSIZE - (end - ptr);
			if (UNEXPECTED(my_send(ftp, data->fd, data->buf, send_size) != send_size)) {
				return FAILURE;
			}
		}
	} else {
		while (!php_stream_eof(instream)) {
			ssize_t size = php_stream_read(instream, data->buf, FTP_BUFSIZE);
			if (size == 0) {
				break;
			}
			if (UNEXPECTED(size < 0)) {
				return FAILURE;
			}
			if (UNEXPECTED(my_send(ftp, data->fd, data->buf, size) != size)) {
				return FAILURE;
			}
			if (send_once_and_return) {
				break;
			}
		}
	}

	return SUCCESS;
}

/* {{{ ftp_put */
int
ftp_put(ftpbuf_t *ftp, const char *path, const size_t path_len, php_stream *instream, ftptype_t type, zend_long startpos)
{
	databuf_t		*data = NULL;
	char			arg[MAX_LENGTH_OF_LONG];

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}
	if ((data = ftp_getdata(ftp)) == NULL) {
		goto bail;
	}

	if (startpos > 0) {
		int arg_len = snprintf(arg, sizeof(arg), ZEND_LONG_FMT, startpos);

		if (arg_len < 0) {
			goto bail;
		}
		if (!ftp_putcmd(ftp, "REST", sizeof("REST")-1, arg, arg_len)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "STOR", sizeof("STOR")-1, path, path_len)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}
	if ((data = data_accept(data, ftp)) == NULL) {
		goto bail;
	}

	if (ftp_send_stream_to_data_socket(ftp, data, instream, type, false) != SUCCESS) {
		goto bail;
	}

	data_close(ftp);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250 && ftp->resp != 200)) {
		goto bail;
	}
	return 1;
bail:
	data_close(ftp);
	return 0;
}
/* }}} */


/* {{{ ftp_append */
int
ftp_append(ftpbuf_t *ftp, const char *path, const size_t path_len, php_stream *instream, ftptype_t type)
{
	databuf_t		*data = NULL;

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}
	if ((data = ftp_getdata(ftp)) == NULL) {
		goto bail;
	}
	ftp->data = data;

	if (!ftp_putcmd(ftp, "APPE", sizeof("APPE")-1, path, path_len)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}
	if ((data = data_accept(data, ftp)) == NULL) {
		goto bail;
	}

	if (ftp_send_stream_to_data_socket(ftp, data, instream, type, false) != SUCCESS) {
		goto bail;
	}

	data_close(ftp);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250 && ftp->resp != 200)) {
		goto bail;
	}
	return 1;
bail:
	data_close(ftp);
	return 0;
}
/* }}} */

/* {{{ ftp_size */
zend_long
ftp_size(ftpbuf_t *ftp, const char *path, const size_t path_len)
{
	if (ftp == NULL) {
		return -1;
	}
	if (!ftp_type(ftp, FTPTYPE_IMAGE)) {
		return -1;
	}
	if (!ftp_putcmd(ftp, "SIZE", sizeof("SIZE")-1, path, path_len)) {
		return -1;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 213) {
		return -1;
	}
	return ZEND_ATOL(ftp->inbuf);
}
/* }}} */

/* {{{ ftp_mdtm */
time_t
ftp_mdtm(ftpbuf_t *ftp, const char *path, const size_t path_len)
{
	time_t		stamp;
	struct tm	*gmt, tmbuf;
	struct tm	tm;
	char		*ptr;
	int		n;

	if (ftp == NULL) {
		return -1;
	}
	if (!ftp_putcmd(ftp, "MDTM", sizeof("MDTM")-1, path, path_len)) {
		return -1;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 213) {
		return -1;
	}
	/* parse out the timestamp */
	for (ptr = ftp->inbuf; *ptr && !isdigit(*ptr); ptr++);
	n = sscanf(ptr, "%4d%2d%2d%2d%2d%2d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
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

/* {{{ ftp_delete */
int
ftp_delete(ftpbuf_t *ftp, const char *path, const size_t path_len)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "DELE", sizeof("DELE")-1, path, path_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ ftp_rename */
int
ftp_rename(ftpbuf_t *ftp, const char *src, const size_t src_len, const char *dest, const size_t dest_len)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RNFR", sizeof("RNFR")-1, src, src_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 350) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "RNTO", sizeof("RNTO")-1, dest, dest_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp != 250) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_site */
int
ftp_site(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len)
{
	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_putcmd(ftp, "SITE", sizeof("SITE")-1, cmd, cmd_len)) {
		return 0;
	}
	if (!ftp_getresp(ftp) || ftp->resp < 200 || ftp->resp >= 300) {
		return 0;
	}

	return 1;
}
/* }}} */

/* static functions */

/* {{{ ftp_putcmd */
int
ftp_putcmd(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len, const char *args, const size_t args_len)
{
	int		size;
	char		*data;

	if (strpbrk(cmd, "\r\n")) {
		return 0;
	}
	/* build the output buffer */
	if (args && args[0]) {
		/* "cmd args\r\n\0" */
		if (cmd_len + args_len + 4 > FTP_BUFSIZE) {
			return 0;
		}
		if (strpbrk(args, "\r\n")) {
			return 0;
		}
		size = slprintf(ftp->outbuf, sizeof(ftp->outbuf), "%s %s\r\n", cmd, args);
	} else {
		/* "cmd\r\n\0" */
		if (cmd_len + 3 > FTP_BUFSIZE) {
			return 0;
		}
		size = slprintf(ftp->outbuf, sizeof(ftp->outbuf), "%s\r\n", cmd);
	}

	data = ftp->outbuf;

	/* Clear the inbuf and extra-lines buffer */
	ftp->inbuf[0] = '\0';
	ftp->extra = NULL;

	if (my_send(ftp, ftp->fd, data, size) != size) {
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ftp_readline */
int
ftp_readline(ftpbuf_t *ftp)
{
	long		size, rcvd;
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
			*data = 0;
			return 0;
		}
	} while (size);

	*data = 0;
	return 0;
}
/* }}} */

/* {{{ ftp_getresp */
int
ftp_getresp(ftpbuf_t *ftp)
{
	if (ftp == NULL) {
		return 0;
	}
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

int single_send(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t size) {
#ifdef HAVE_FTP_SSL
	int err;
	bool retry = 0;
	SSL *handle = NULL;
	php_socket_t fd;
	size_t sent;

	if (ftp->use_ssl && ftp->fd == s && ftp->ssl_active) {
		handle = ftp->ssl_handle;
		fd = ftp->fd;
	} else if (ftp->use_ssl && ftp->fd != s && ftp->use_ssl_for_data && ftp->data->ssl_active) {
		handle = ftp->data->ssl_handle;
		fd = ftp->data->fd;
	} else {
		return send(s, buf, size, 0);
	}

	do {
		sent = SSL_write(handle, buf, size);
		err = SSL_get_error(handle, sent);

		switch (err) {
			case SSL_ERROR_NONE:
				retry = 0;
				break;

			case SSL_ERROR_ZERO_RETURN:
				retry = 0;
				SSL_shutdown(handle);
				break;

			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_CONNECT: {
					php_pollfd p;
					int i;

					p.fd = fd;
					p.events = POLLOUT;
					p.revents = 0;

					i = php_poll2(&p, 1, 300);

					retry = i > 0;
				}
				break;

			default:
				php_error_docref(NULL, E_WARNING, "SSL write failed");
				return -1;
		}
	} while (retry);
	return sent;
#else
	return send(s, buf, size, 0);
#endif
}

/* {{{ my_send */
int
my_send(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t len)
{
	zend_long size, sent;
	int       n;

	size = len;
	while (size) {
		n = php_pollfd_for_ms(s, POLLOUT, ftp->timeout_sec * 1000);

		if (n < 1) {
			char buf[256];
			if (n == 0) {
#ifdef PHP_WIN32
				_set_errno(ETIMEDOUT);
#else
				errno = ETIMEDOUT;
#endif
			}
			php_error_docref(NULL, E_WARNING, "%s", php_socket_strerror(errno, buf, sizeof buf));
			return -1;
		}

		sent = single_send(ftp, s, buf, size);
		if (sent == -1) {
			return -1;
		}

		buf = (char*) buf + sent;
		size -= sent;
	}

	return len;
}
/* }}} */

/* {{{ my_recv */
int
my_recv(ftpbuf_t *ftp, php_socket_t s, void *buf, size_t len)
{
	int		n, nr_bytes;
#ifdef HAVE_FTP_SSL
	int err;
	bool retry = 0;
	SSL *handle = NULL;
	php_socket_t fd;
#endif
	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, ftp->timeout_sec * 1000);
	if (n < 1) {
		char buf[256];
		if (n == 0) {
#ifdef PHP_WIN32
			_set_errno(ETIMEDOUT);
#else
			errno = ETIMEDOUT;
#endif
		}
		php_error_docref(NULL, E_WARNING, "%s", php_socket_strerror(errno, buf, sizeof buf));
		return -1;
	}

#ifdef HAVE_FTP_SSL
	if (ftp->use_ssl && ftp->fd == s && ftp->ssl_active) {
		handle = ftp->ssl_handle;
		fd = ftp->fd;
	} else if (ftp->use_ssl && ftp->fd != s && ftp->use_ssl_for_data && ftp->data->ssl_active) {
		handle = ftp->data->ssl_handle;
		fd = ftp->data->fd;
	}

	if (handle) {
		do {
			nr_bytes = SSL_read(handle, buf, len);
			err = SSL_get_error(handle, nr_bytes);

			switch (err) {
				case SSL_ERROR_NONE:
					retry = 0;
					break;

				case SSL_ERROR_ZERO_RETURN:
					retry = 0;
					SSL_shutdown(handle);
					break;

				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_CONNECT: {
						php_pollfd p;
						int i;

						p.fd = fd;
						p.events = POLLIN|POLLPRI;
						p.revents = 0;

						i = php_poll2(&p, 1, 300);

						retry = i > 0;
					}
					break;

				default:
					php_error_docref(NULL, E_WARNING, "SSL read failed");
					return -1;
			}
		} while (retry);
	} else {
#endif
		nr_bytes = recv(s, buf, len, 0);
#ifdef HAVE_FTP_SSL
	}
#endif
	return (nr_bytes);
}
/* }}} */

/* {{{ data_available */
int
data_available(ftpbuf_t *ftp, php_socket_t s)
{
	int		n;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, 1000);
	if (n < 1) {
		char buf[256];
		if (n == 0) {
#ifdef PHP_WIN32
			_set_errno(ETIMEDOUT);
#else
			errno = ETIMEDOUT;
#endif
		}
		php_error_docref(NULL, E_WARNING, "%s", php_socket_strerror(errno, buf, sizeof buf));
		return 0;
	}

	return 1;
}
/* }}} */
/* {{{ data_writeable */
int
data_writeable(ftpbuf_t *ftp, php_socket_t s)
{
	int		n;

	n = php_pollfd_for_ms(s, POLLOUT, 1000);
	if (n < 1) {
		char buf[256];
		if (n == 0) {
#ifdef PHP_WIN32
			_set_errno(ETIMEDOUT);
#else
			errno = ETIMEDOUT;
#endif
		}
		php_error_docref(NULL, E_WARNING, "%s", php_socket_strerror(errno, buf, sizeof buf));
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ my_accept */
int
my_accept(ftpbuf_t *ftp, php_socket_t s, struct sockaddr *addr, socklen_t *addrlen)
{
	int		n;

	n = php_pollfd_for_ms(s, PHP_POLLREADABLE, ftp->timeout_sec * 1000);
	if (n < 1) {
		char buf[256];
		if (n == 0) {
#ifdef PHP_WIN32
			_set_errno(ETIMEDOUT);
#else
			errno = ETIMEDOUT;
#endif
		}
		php_error_docref(NULL, E_WARNING, "%s", php_socket_strerror(errno, buf, sizeof buf));
		return -1;
	}

	return accept(s, addr, addrlen);
}
/* }}} */

/* {{{ ftp_getdata */
databuf_t*
ftp_getdata(ftpbuf_t *ftp)
{
	int				fd = -1;
	databuf_t		*data;
	php_sockaddr_storage addr;
	struct sockaddr *sa;
	socklen_t		size;
	union ipbox		ipbox;
	char			arg[sizeof("255, 255, 255, 255, 255, 255")];
	struct timeval	tv;
	int				arg_len;


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
		php_error_docref(NULL, E_WARNING, "socket() failed: %s (%d)", strerror(errno), errno);
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
			php_error_docref(NULL, E_WARNING, "php_connect_nonb() failed: %s (%d)", strerror(errno), errno);
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
		php_error_docref(NULL, E_WARNING, "bind() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (getsockname(fd, (struct sockaddr*) &addr, &size) != 0) {
		php_error_docref(NULL, E_WARNING, "getsockname() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	if (listen(fd, 5) != 0) {
		php_error_docref(NULL, E_WARNING, "listen() failed: %s (%d)", strerror(errno), errno);
		goto bail;
	}

	data->listener = fd;

#if defined(HAVE_IPV6)
	if (sa->sa_family == AF_INET6) {
		/* need to use EPRT */
		char eprtarg[INET6_ADDRSTRLEN + sizeof("|x||xxxxx|")];
		char out[INET6_ADDRSTRLEN];
		int eprtarg_len;
		const char *r;
		r = inet_ntop(AF_INET6, &((struct sockaddr_in6*) sa)->sin6_addr, out, sizeof(out));
		ZEND_ASSERT(r != NULL);

		eprtarg_len = snprintf(eprtarg, sizeof(eprtarg), "|2|%s|%hu|", out, ntohs(((struct sockaddr_in6 *) &addr)->sin6_port));

		if (eprtarg_len < 0) {
			goto bail;
		}

		if (!ftp_putcmd(ftp, "EPRT", sizeof("EPRT")-1, eprtarg, eprtarg_len)) {
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
	arg_len = snprintf(arg, sizeof(arg), "%u,%u,%u,%u,%u,%u", ipbox.c[0], ipbox.c[1], ipbox.c[2], ipbox.c[3], ipbox.c[4], ipbox.c[5]);

	if (arg_len < 0) {
		goto bail;
	}
	if (!ftp_putcmd(ftp, "PORT", sizeof("PORT")-1, arg, arg_len)) {
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

/* {{{ data_accept */
databuf_t*
data_accept(databuf_t *data, ftpbuf_t *ftp)
{
	php_sockaddr_storage addr;
	socklen_t			size;

#ifdef HAVE_FTP_SSL
	SSL_CTX		*ctx;
	SSL_SESSION *session;
	int err, res;
	bool retry;
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
#ifdef HAVE_FTP_SSL

	/* now enable ssl if we need to */
	if (ftp->use_ssl && ftp->use_ssl_for_data) {
		ctx = SSL_get_SSL_CTX(ftp->ssl_handle);
		if (ctx == NULL) {
			php_error_docref(NULL, E_WARNING, "data_accept: failed to retrieve the existing SSL context");
			return 0;
		}

		data->ssl_handle = SSL_new(ctx);
		if (data->ssl_handle == NULL) {
			php_error_docref(NULL, E_WARNING, "data_accept: failed to create the SSL handle");
			return 0;
		}

		SSL_set_fd(data->ssl_handle, data->fd);

		if (ftp->old_ssl) {
			SSL_copy_session_id(data->ssl_handle, ftp->ssl_handle);
		}

		/* get the session from the control connection so we can re-use it */
		session = ftp->last_ssl_session;
		if (session == NULL) {
			php_error_docref(NULL, E_WARNING, "data_accept: failed to retrieve the existing SSL session");
			SSL_free(data->ssl_handle);
			return 0;
		}

		/* and set it on the data connection */
		SSL_set_app_data(data->ssl_handle, ftp); /* Needed for ftp_ssl_new_session_cb */
		res = SSL_set_session(data->ssl_handle, session);
		if (res == 0) {
			php_error_docref(NULL, E_WARNING, "data_accept: failed to set the existing SSL session");
			SSL_free(data->ssl_handle);
			return 0;
		}

		do {
			res = SSL_connect(data->ssl_handle);
			err = SSL_get_error(data->ssl_handle, res);

			switch (err) {
				case SSL_ERROR_NONE:
					retry = 0;
					break;

				case SSL_ERROR_ZERO_RETURN:
					retry = 0;
					SSL_shutdown(data->ssl_handle);
					break;

				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE: {
						php_pollfd p;
						int i;

						p.fd = data->fd;
						p.events = (err == SSL_ERROR_WANT_READ) ? (POLLIN|POLLPRI) : POLLOUT;
						p.revents = 0;

						i = php_poll2(&p, 1, 300);

						retry = i > 0;
					}
					break;

				default:
					php_error_docref(NULL, E_WARNING, "data_accept: SSL/TLS handshake failed");
					SSL_shutdown(data->ssl_handle);
					SSL_free(data->ssl_handle);
					return 0;
			}
		} while (retry);

		data->ssl_active = 1;
	}

#endif

	return data;
}
/* }}} */

/* {{{ ftp_ssl_shutdown */
#ifdef HAVE_FTP_SSL
static void ftp_ssl_shutdown(ftpbuf_t *ftp, php_socket_t fd, SSL *ssl_handle) {
	/* In TLS 1.3 it's common to receive session tickets after the handshake has completed. We need to train
	   the socket (read the tickets until EOF/close_notify alert) before closing the socket. Otherwise the
	   server might get an ECONNRESET which might lead to data truncation on server side.
	*/
	char buf[256]; /* We will use this for the OpenSSL error buffer, so it has
			  to be at least 256 bytes long.*/
	int done = 1, err, nread;
	unsigned long sslerror;

	err = SSL_shutdown(ssl_handle);
	if (err < 0) {
		php_error_docref(NULL, E_WARNING, "SSL_shutdown failed");
	}
	else if (err == 0) {
		/* The shutdown is not yet finished. Call SSL_read() to do a bidirectional shutdown. */
		done = 0;
	}

	while (!done && data_available(ftp, fd)) {
		ERR_clear_error();
		nread = SSL_read(ssl_handle, buf, sizeof(buf));
		if (nread <= 0) {
			err = SSL_get_error(ssl_handle, nread);
			switch (err) {
				case SSL_ERROR_NONE: /* this is not an error */
				case SSL_ERROR_ZERO_RETURN: /* no more data */
					/* This is the expected response. There was no data but only
					   the close notify alert */
					done = 1;
					break;
				case SSL_ERROR_WANT_READ:
					/* there's data pending, re-invoke SSL_read() */
					break;
				case SSL_ERROR_WANT_WRITE:
					/* SSL wants a write. Really odd. Let's bail out. */
					done = 1;
					break;
				case SSL_ERROR_SYSCALL:
					/* most likely the peer closed the connection without
					   sending a close_notify shutdown alert;
					   bail out to avoid raising a spurious warning */
					done = 1;
					break;
				default:
					if ((sslerror = ERR_get_error())) {
						ERR_error_string_n(sslerror, buf, sizeof(buf));
						php_error_docref(NULL, E_WARNING, "SSL_read on shutdown: %s", buf);
					} else if (errno) {
						php_error_docref(NULL, E_WARNING, "SSL_read on shutdown: %s (%d)", strerror(errno), errno);
					}
					done = 1;
					break;
			}
		}
	}
	(void)SSL_free(ssl_handle);
}
#endif
/* }}} */

/* {{{ data_close */
void data_close(ftpbuf_t *ftp)
{
	ZEND_ASSERT(ftp != NULL);
	databuf_t *data = ftp->data;
	if (data == NULL) {
		return;
	}
	if (data->listener != -1) {
#ifdef HAVE_FTP_SSL
		if (data->ssl_active) {
			/* don't free the data context, it's the same as the control */
			ftp_ssl_shutdown(ftp, data->listener, data->ssl_handle);
			data->ssl_active = 0;
		}
#endif
		closesocket(data->listener);
	}
	if (data->fd != -1) {
#ifdef HAVE_FTP_SSL
		if (data->ssl_active) {
			/* don't free the data context, it's the same as the control */
			ftp_ssl_shutdown(ftp, data->fd, data->ssl_handle);
			data->ssl_active = 0;
		}
#endif
		closesocket(data->fd);
	}
	ftp->data = NULL;
	efree(data);
}
/* }}} */

/* {{{ ftp_genlist */
char**
ftp_genlist(ftpbuf_t *ftp, const char *cmd, const size_t cmd_len, const char *path, const size_t path_len)
{
	php_stream	*tmpstream = NULL;
	databuf_t	*data = NULL;
	char		*ptr;
	int		ch, lastch;
	size_t		size, rcvd;
	size_t		lines;
	char		**ret = NULL;
	char		**entry;
	char		*text;


	if ((tmpstream = php_stream_fopen_tmpfile()) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create temporary file.  Check permissions in temporary files directory.");
		return NULL;
	}

	if (!ftp_type(ftp, FTPTYPE_ASCII)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp)) == NULL) {
		goto bail;
	}
	ftp->data = data;

	if (!ftp_putcmd(ftp, cmd, cmd_len, path, path_len)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125 && ftp->resp != 226)) {
		goto bail;
	}

	/* some servers don't open a ftp-data connection if the directory is empty */
	if (ftp->resp == 226) {
		data_close(ftp);
		php_stream_close(tmpstream);
		return ecalloc(1, sizeof(char*));
	}

	/* pull data buffer into tmpfile */
	if ((data = data_accept(data, ftp)) == NULL) {
		goto bail;
	}
	size = 0;
	lines = 0;
	lastch = 0;
	while ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == (size_t)-1 || rcvd > ((size_t)(-1))-size) {
			goto bail;
		}

		php_stream_write(tmpstream, data->buf, rcvd);

		size += rcvd;
		for (ptr = data->buf; rcvd; rcvd--, ptr++) {
			if (*ptr == '\n' && lastch == '\r') {
				lines++;
			}
			lastch = *ptr;
		}
	}

	data_close(ftp);

	php_stream_rewind(tmpstream);

	ret = safe_emalloc((lines + 1), sizeof(char*), size);

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
	data_close(ftp);
	php_stream_close(tmpstream);
	if (ret)
		efree(ret);
	return NULL;
}
/* }}} */

/* {{{ ftp_nb_get */
int
ftp_nb_get(ftpbuf_t *ftp, php_stream *outstream, const char *path, const size_t path_len, ftptype_t type, zend_long resumepos)
{
	databuf_t		*data = NULL;
	char			arg[MAX_LENGTH_OF_LONG];

	if (ftp == NULL) {
		return PHP_FTP_FAILED;
	}

	if (ftp->data != NULL) {
		/* If there is a transfer in action, abort it.
		 * If we don't, we get an invalid state and memory leaks when the new connection gets opened. */
		data_close(ftp);
		if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
			goto bail;
		}
	}

	if (!ftp_type(ftp, type)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp)) == NULL) {
		goto bail;
	}

	if (resumepos>0) {
		int arg_len = snprintf(arg, sizeof(arg), ZEND_LONG_FMT, resumepos);

		if (arg_len < 0) {
			goto bail;
		}
		if (!ftp_putcmd(ftp, "REST", sizeof("REST")-1, arg, arg_len)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "RETR", sizeof("RETR")-1, path, path_len)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}

	if ((data = data_accept(data, ftp)) == NULL) {
		goto bail;
	}

	ftp->data = data;
	ftp->stream = outstream;
	ftp->lastch = 0;
	ftp->nb = 1;

	return (ftp_nb_continue_read(ftp));

bail:
	data_close(ftp);
	return PHP_FTP_FAILED;
}
/* }}} */

/* {{{ ftp_nb_continue_read */
int
ftp_nb_continue_read(ftpbuf_t *ftp)
{
	databuf_t	*data = NULL;
	char		*ptr;
	char		lastch;
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
		if (rcvd == (size_t)-1) {
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

	data_close(ftp);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}

	ftp->nb = 0;
	return PHP_FTP_FINISHED;
bail:
	ftp->nb = 0;
	data_close(ftp);
	return PHP_FTP_FAILED;
}
/* }}} */

/* {{{ ftp_nb_put */
int
ftp_nb_put(ftpbuf_t *ftp, const char *path, const size_t path_len, php_stream *instream, ftptype_t type, zend_long startpos)
{
	databuf_t		*data = NULL;
	char			arg[MAX_LENGTH_OF_LONG];

	if (ftp == NULL) {
		return 0;
	}
	if (!ftp_type(ftp, type)) {
		goto bail;
	}
	if ((data = ftp_getdata(ftp)) == NULL) {
		goto bail;
	}
	if (startpos > 0) {
		int arg_len = snprintf(arg, sizeof(arg), ZEND_LONG_FMT, startpos);

		if (arg_len < 0) {
			goto bail;
		}
		if (!ftp_putcmd(ftp, "REST", sizeof("REST")-1, arg, arg_len)) {
			goto bail;
		}
		if (!ftp_getresp(ftp) || (ftp->resp != 350)) {
			goto bail;
		}
	}

	if (!ftp_putcmd(ftp, "STOR", sizeof("STOR")-1, path, path_len)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125)) {
		goto bail;
	}
	if ((data = data_accept(data, ftp)) == NULL) {
		goto bail;
	}
	ftp->data = data;
	ftp->stream = instream;
	ftp->lastch = 0;
	ftp->nb = 1;

	return (ftp_nb_continue_write(ftp));

bail:
	data_close(ftp);
	return PHP_FTP_FAILED;
}
/* }}} */


/* {{{ ftp_nb_continue_write */
int
ftp_nb_continue_write(ftpbuf_t *ftp)
{
	/* check if we can write more data */
	if (!data_writeable(ftp, ftp->data->fd)) {
		return PHP_FTP_MOREDATA;
	}

	if (ftp_send_stream_to_data_socket(ftp, ftp->data, ftp->stream, ftp->type, true) != SUCCESS) {
		goto bail;
	}

	if (!php_stream_eof(ftp->stream)) {
		return PHP_FTP_MOREDATA;
	}

	data_close(ftp);

	if (!ftp_getresp(ftp) || (ftp->resp != 226 && ftp->resp != 250)) {
		goto bail;
	}
	ftp->nb = 0;
	return PHP_FTP_FINISHED;
bail:
	data_close(ftp);
	ftp->nb = 0;
	return PHP_FTP_FAILED;
}
/* }}} */
