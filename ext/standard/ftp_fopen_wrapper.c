/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   |          Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
#include <winsock2.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
#endif

#include "php_standard.h"

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
#include <winsock2.h>
#elif defined(NETWARE) && defined(USE_WINSOCK)
#include <novsock2.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#if defined(PHP_WIN32) || defined(__riscos__) || defined(NETWARE)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "php_fopen_wrappers.h"

#define FTPS_ENCRYPT_DATA 1
#define GET_FTP_RESULT(stream)	get_ftp_result((stream), tmp_line, sizeof(tmp_line) TSRMLS_CC)

typedef struct _php_ftp_dirstream_data {
	php_stream *datastream;
	php_stream *controlstream;
	php_stream *dirstream;
} php_ftp_dirstream_data;

/* {{{ get_ftp_result
 */
static inline int get_ftp_result(php_stream *stream, char *buffer, size_t buffer_size TSRMLS_DC)
{
	buffer[0] = '\0'; /* in case read fails to read anything */
	while (php_stream_gets(stream, buffer, buffer_size-1) &&
		   !(isdigit((int) buffer[0]) && isdigit((int) buffer[1]) &&
			 isdigit((int) buffer[2]) && buffer[3] == ' '));
	return strtol(buffer, NULL, 10);
}
/* }}} */

/* {{{ php_stream_ftp_stream_stat
 */
static int php_stream_ftp_stream_stat(php_stream_wrapper *wrapper, php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	/* For now, we return with a failure code to prevent the underlying
	 * file's details from being used instead. */
	return -1;
}
/* }}} */

/* {{{ php_stream_ftp_stream_close
 */
static int php_stream_ftp_stream_close(php_stream_wrapper *wrapper, php_stream *stream TSRMLS_DC)
{
	php_stream *controlstream = stream->wrapperthis;
	int ret = 0;

	if (controlstream) {
		if (strpbrk(stream->mode, "wa+")) {
			char tmp_line[512];
			int result;

			/* For write modes close data stream first to signal EOF to server */
			result = GET_FTP_RESULT(controlstream);
			if (result != 226 && result != 250) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "FTP server error %d:%s", result, tmp_line);
				ret = EOF;
			}
		}

		php_stream_write_string(controlstream, "QUIT\r\n");
		php_stream_close(controlstream);
		stream->wrapperthis = NULL;
	}

	return ret;
}
/* }}} */

/* {{{ php_ftp_fopen_connect
 */
static php_stream *php_ftp_fopen_connect(php_stream_wrapper *wrapper, const char *path, const char *mode, int options,
										 char **opened_path, php_stream_context *context, php_stream **preuseid,
										 php_url **presource, int *puse_ssl, int *puse_ssl_on_data TSRMLS_DC)
{
	php_stream *stream = NULL, *reuseid = NULL;
	php_url *resource = NULL;
	int result, use_ssl, use_ssl_on_data = 0, tmp_len;
	char tmp_line[512];
	char *transport;
	int transport_len;

	resource = php_url_parse(path);
	if (resource == NULL || resource->path == NULL) {
		if (resource && presource) {
			*presource = resource;
		}
		return NULL;
	}

	use_ssl = resource->scheme && (strlen(resource->scheme) > 3) && resource->scheme[3] == 's';

	/* use port 21 if one wasn't specified */
	if (resource->port == 0)
		resource->port = 21;

	transport_len = spprintf(&transport, 0, "tcp://%s:%d", resource->host, resource->port);
	stream = php_stream_xport_create(transport, transport_len, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, NULL, context, NULL, NULL);
	efree(transport);
	if (stream == NULL) {
		result = 0; /* silence */
		goto connect_errexit;
	}

	php_stream_context_set(stream, context);
	php_stream_notify_info(context, PHP_STREAM_NOTIFY_CONNECT, NULL, 0);

	/* Start talking to ftp server */
	result = GET_FTP_RESULT(stream);
	if (result > 299 || result < 200) {
		php_stream_notify_error(context, PHP_STREAM_NOTIFY_FAILURE, tmp_line, result);
		goto connect_errexit;
	}

	if (use_ssl)	{

		/* send the AUTH TLS request name */
		php_stream_write_string(stream, "AUTH TLS\r\n");

		/* get the response */
		result = GET_FTP_RESULT(stream);
		if (result != 234) {
			/* AUTH TLS not supported try AUTH SSL */
			php_stream_write_string(stream, "AUTH SSL\r\n");

			/* get the response */
			result = GET_FTP_RESULT(stream);
			if (result != 334) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Server doesn't support FTPS.");
				goto connect_errexit;
			} else {
				/* we must reuse the old SSL session id */
				/* if we talk to an old ftpd-ssl */
				reuseid = stream;
			}
		} else {
			/* encrypt data etc */


		}

	}

	if (use_ssl) {
		if (php_stream_xport_crypto_setup(stream,
				STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0
				|| php_stream_xport_crypto_enable(stream, 1 TSRMLS_CC) < 0) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to activate SSL mode");
			php_stream_close(stream);
			stream = NULL;
			goto connect_errexit;
		}

		/* set PBSZ to 0 */
		php_stream_write_string(stream, "PBSZ 0\r\n");

		/* ignore the response */
		result = GET_FTP_RESULT(stream);

		/* set data connection protection level */
#if FTPS_ENCRYPT_DATA
		php_stream_write_string(stream, "PROT P\r\n");

		/* get the response */
		result = GET_FTP_RESULT(stream);
		use_ssl_on_data = (result >= 200 && result<=299) || reuseid;
#else
		php_stream_write_string(stream, "PROT C\r\n");

		/* get the response */
		result = GET_FTP_RESULT(stream);
#endif
	}

#define PHP_FTP_CNTRL_CHK(val, val_len, err_msg) {	\
	unsigned char *s = val, *e = s + val_len;	\
	while (s < e) {	\
		if (iscntrl(*s)) {	\
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, err_msg, val);	\
			goto connect_errexit;	\
		}	\
		s++;	\
	}	\
}

	/* send the user name */
	if (resource->user != NULL) {
		tmp_len = php_raw_url_decode(resource->user, strlen(resource->user));

		PHP_FTP_CNTRL_CHK(resource->user, tmp_len, "Invalid login %s")

		php_stream_printf(stream TSRMLS_CC, "USER %s\r\n", resource->user);
	} else {
		php_stream_write_string(stream, "USER anonymous\r\n");
	}

	/* get the response */
	result = GET_FTP_RESULT(stream);

	/* if a password is required, send it */
	if (result >= 300 && result <= 399) {
		php_stream_notify_info(context, PHP_STREAM_NOTIFY_AUTH_REQUIRED, tmp_line, 0);

		if (resource->pass != NULL) {
			tmp_len = php_raw_url_decode(resource->pass, strlen(resource->pass));

			PHP_FTP_CNTRL_CHK(resource->pass, tmp_len, "Invalid password %s")

			php_stream_printf(stream TSRMLS_CC, "PASS %s\r\n", resource->pass);
		} else {
			/* if the user has configured who they are,
			   send that as the password */
			if (FG(from_address)) {
				php_stream_printf(stream TSRMLS_CC, "PASS %s\r\n", FG(from_address));
			} else {
				php_stream_write_string(stream, "PASS anonymous\r\n");
			}
		}

		/* read the response */
		result = GET_FTP_RESULT(stream);

		if (result > 299 || result < 200) {
			php_stream_notify_error(context, PHP_STREAM_NOTIFY_AUTH_RESULT, tmp_line, result);
		} else {
			php_stream_notify_info(context, PHP_STREAM_NOTIFY_AUTH_RESULT, tmp_line, result);
		}
	}
	if (result > 299 || result < 200) {
		goto connect_errexit;
	}

	if (puse_ssl) {
		*puse_ssl = use_ssl;
	}
	if (puse_ssl_on_data) {
		*puse_ssl_on_data = use_ssl_on_data;
	}
	if (preuseid) {
		*preuseid = reuseid;
	}
	if (presource) {
		*presource = resource;
	}

	return stream;

connect_errexit:
	if (resource) {
		php_url_free(resource);
	}

	if (stream) {
		php_stream_close(stream);
	}

	return NULL;
}
/* }}} */

/* {{{ php_fopen_do_pasv
 */
static unsigned short php_fopen_do_pasv(php_stream *stream, char *ip, size_t ip_size, char **phoststart TSRMLS_DC)
{
	char tmp_line[512];
	int result, i;
	unsigned short portno;
	char *tpath, *ttpath, *hoststart=NULL;

#ifdef HAVE_IPV6
	/* We try EPSV first, needed for IPv6 and works on some IPv4 servers */
	php_stream_write_string(stream, "EPSV\r\n");
	result = GET_FTP_RESULT(stream);

	/* check if we got a 229 response */
	if (result != 229) {
#endif
		/* EPSV failed, let's try PASV */
		php_stream_write_string(stream, "PASV\r\n");
		result = GET_FTP_RESULT(stream);

		/* make sure we got a 227 response */
		if (result != 227) {
			return 0;
		}

		/* parse pasv command (129, 80, 95, 25, 13, 221) */
		tpath = tmp_line;
		/* skip over the "227 Some message " part */
		for (tpath += 4; *tpath && !isdigit((int) *tpath); tpath++);
		if (!*tpath) {
			return 0;
		}
		/* skip over the host ip, to get the port */
		hoststart = tpath;
		for (i = 0; i < 4; i++) {
			for (; isdigit((int) *tpath); tpath++);
			if (*tpath != ',') {
				return 0;
			}
			*tpath='.';
			tpath++;
		}
		tpath[-1] = '\0';
		memcpy(ip, hoststart, ip_size);
		ip[ip_size-1] = '\0';
		hoststart = ip;

		/* pull out the MSB of the port */
		portno = (unsigned short) strtoul(tpath, &ttpath, 10) * 256;
		if (ttpath == NULL) {
			/* didn't get correct response from PASV */
			return 0;
		}
		tpath = ttpath;
		if (*tpath != ',') {
			return 0;
		}
		tpath++;
		/* pull out the LSB of the port */
		portno += (unsigned short) strtoul(tpath, &ttpath, 10);
#ifdef HAVE_IPV6
	} else {
		/* parse epsv command (|||6446|) */
		for (i = 0, tpath = tmp_line + 4; *tpath; tpath++) {
			if (*tpath == '|') {
				i++;
				if (i == 3)
					break;
			}
		}
		if (i < 3) {
			return 0;
		}
		/* pull out the port */
		portno = (unsigned short) strtoul(tpath + 1, &ttpath, 10);
	}
#endif
	if (ttpath == NULL) {
		/* didn't get correct response from EPSV/PASV */
		return 0;
	}

	if (phoststart) {
		*phoststart = hoststart;
	}

	return portno;
}
/* }}} */

/* {{{ php_fopen_url_wrap_ftp
 */
php_stream * php_stream_url_wrap_ftp(php_stream_wrapper *wrapper, const char *path, const char *mode,
									 int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL, *datastream = NULL;
	php_url *resource = NULL;
	char tmp_line[512];
	char ip[sizeof("123.123.123.123")];
	unsigned short portno;
	char *hoststart = NULL;
	int result = 0, use_ssl, use_ssl_on_data=0;
	php_stream *reuseid=NULL;
	size_t file_size = 0;
	zval **tmpzval;
	int allow_overwrite = 0;
	int read_write = 0;
	char *transport;
	int transport_len;

	tmp_line[0] = '\0';

	if (strpbrk(mode, "r+")) {
		read_write = 1; /* Open for reading */
	}
	if (strpbrk(mode, "wa+")) {
		if (read_write) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP does not support simultaneous read/write connections");
			return NULL;
		}
		if (strchr(mode, 'a')) {
			read_write = 3; /* Open for Appending */
		} else {
			read_write = 2; /* Open for writing */
		}
	}
	if (!read_write) {
		/* No mode specified? */
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unknown file open mode");
		return NULL;
	}

	if (context &&
		php_stream_context_get_option(context, "ftp", "proxy", &tmpzval) == SUCCESS) {
		if (read_write == 1) {
			/* Use http wrapper to proxy ftp request */
			return php_stream_url_wrap_http(wrapper, path, mode, options, opened_path, context STREAMS_CC TSRMLS_CC);
		} else {
			/* ftp proxy is read-only */
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP proxy may only be used in read mode");
			return NULL;
		}
	}

	stream = php_ftp_fopen_connect(wrapper, path, mode, options, opened_path, context, &reuseid, &resource, &use_ssl, &use_ssl_on_data TSRMLS_CC);
	if (!stream) {
		goto errexit;
	}

	/* set the connection to be binary */
	php_stream_write_string(stream, "TYPE I\r\n");
	result = GET_FTP_RESULT(stream);
	if (result > 299 || result < 200)
		goto errexit;

	/* find out the size of the file (verifying it exists) */
	php_stream_printf(stream TSRMLS_CC, "SIZE %s\r\n", resource->path);

	/* read the response */
	result = GET_FTP_RESULT(stream);
	if (read_write == 1) {
		/* Read Mode */
		char *sizestr;

		/* when reading file, it must exist */
		if (result > 299 || result < 200) {
			errno = ENOENT;
			goto errexit;
		}

		sizestr = strchr(tmp_line, ' ');
		if (sizestr) {
			sizestr++;
			file_size = atoi(sizestr);
			php_stream_notify_file_size(context, file_size, tmp_line, result);
		}
	} else if (read_write == 2) {
		/* when writing file (but not appending), it must NOT exist, unless a context option exists which allows it */
		if (context && php_stream_context_get_option(context, "ftp", "overwrite", &tmpzval) == SUCCESS) {
			allow_overwrite = Z_LVAL_PP(tmpzval);
		}
		if (result <= 299 && result >= 200) {
			if (allow_overwrite) {
				/* Context permits overwriting file,
				   so we just delete whatever's there in preparation */
				php_stream_printf(stream TSRMLS_CC, "DELE %s\r\n", resource->path);
				result = GET_FTP_RESULT(stream);
				if (result >= 300 || result <= 199) {
					goto errexit;
				}
			} else {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Remote file already exists and overwrite context option not specified");
				errno = EEXIST;
				goto errexit;
			}
		}
	}

	/* set up the passive connection */
	portno = php_fopen_do_pasv(stream, ip, sizeof(ip), &hoststart TSRMLS_CC);

	if (!portno) {
		goto errexit;
	}

	/* Send RETR/STOR command */
	if (read_write == 1) {
		/* set resume position if applicable */
		if (context &&
			php_stream_context_get_option(context, "ftp", "resume_pos", &tmpzval) == SUCCESS &&
			Z_TYPE_PP(tmpzval) == IS_LONG &&
			Z_LVAL_PP(tmpzval) > 0) {
			php_stream_printf(stream TSRMLS_CC, "REST %ld\r\n", Z_LVAL_PP(tmpzval));
			result = GET_FTP_RESULT(stream);
			if (result < 300 || result > 399) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to resume from offset %ld", Z_LVAL_PP(tmpzval));
				goto errexit;
			}
		}

		/* retrieve file */
		memcpy(tmp_line, "RETR", sizeof("RETR"));
	} else if (read_write == 2) {
		/* Write new file */
		memcpy(tmp_line, "STOR", sizeof("STOR"));
	} else {
		/* Append */
		memcpy(tmp_line, "APPE", sizeof("APPE"));
	}
	php_stream_printf(stream TSRMLS_CC, "%s %s\r\n", tmp_line, (resource->path != NULL ? resource->path : "/"));

	/* open the data channel */
	if (hoststart == NULL) {
		hoststart = resource->host;
	}
	transport_len = spprintf(&transport, 0, "tcp://%s:%d", hoststart, portno);
	datastream = php_stream_xport_create(transport, transport_len, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, NULL, context, NULL, NULL);
	efree(transport);
	if (datastream == NULL) {
		goto errexit;
	}

	result = GET_FTP_RESULT(stream);
	if (result != 150 && result != 125) {
		/* Could not retrieve or send the file
		 * this data will only be sent to us after connection on the data port was initiated.
		 */
		php_stream_close(datastream);
		datastream = NULL;
		goto errexit;
	}

	php_stream_context_set(datastream, context);
	php_stream_notify_progress_init(context, 0, file_size);

	if (use_ssl_on_data && (php_stream_xport_crypto_setup(datastream,
			STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0 ||
			php_stream_xport_crypto_enable(datastream, 1 TSRMLS_CC) < 0)) {

		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to activate SSL mode");
		php_stream_close(datastream);
		datastream = NULL;
		goto errexit;
	}

	/* remember control stream */
	datastream->wrapperthis = stream;

	php_url_free(resource);
	return datastream;

errexit:
	if (resource) {
		php_url_free(resource);
	}
	if (stream) {
		php_stream_notify_error(context, PHP_STREAM_NOTIFY_FAILURE, tmp_line, result);
		php_stream_close(stream);
	}
	if (tmp_line[0] != '\0')
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP server reports %s", tmp_line);
	return NULL;
}
/* }}} */

/* {{{ php_ftp_dirsteam_read
 */
static size_t php_ftp_dirstream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_dirent *ent = (php_stream_dirent *)buf;
	php_stream *innerstream;
	size_t tmp_len;
	char *basename;
	size_t basename_len;

	innerstream =  ((php_ftp_dirstream_data *)stream->abstract)->datastream;

	if (count != sizeof(php_stream_dirent)) {
		return 0;
	}

	if (php_stream_eof(innerstream)) {
		return 0;
	}

	if (!php_stream_get_line(innerstream, ent->d_name, sizeof(ent->d_name), &tmp_len)) {
		return 0;
	}

	php_basename(ent->d_name, tmp_len, NULL, 0, &basename, &basename_len TSRMLS_CC);
	if (!basename) {
		return 0;
	}

	if (!basename_len) {
		efree(basename);
		return 0;
	}

	tmp_len = MIN(sizeof(ent->d_name), basename_len - 1);
	memcpy(ent->d_name, basename, tmp_len);
	ent->d_name[tmp_len - 1] = '\0';
	efree(basename);

	/* Trim off trailing whitespace characters */
	while (tmp_len > 0 &&
			(ent->d_name[tmp_len - 1] == '\n' || ent->d_name[tmp_len - 1] == '\r' ||
			 ent->d_name[tmp_len - 1] == '\t' || ent->d_name[tmp_len - 1] == ' ')) {
		ent->d_name[--tmp_len] = '\0';
	}

	return sizeof(php_stream_dirent);
}
/* }}} */

/* {{{ php_ftp_dirstream_close
 */
static int php_ftp_dirstream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_ftp_dirstream_data *data = stream->abstract;

	/* close control connection */
	if (data->controlstream) {
		php_stream_close(data->controlstream);
		data->controlstream = NULL;
	}
	/* close data connection */
	php_stream_close(data->datastream);
	data->datastream = NULL;

	efree(data);
	stream->abstract = NULL;

	return 0;
}
/* }}} */

/* ftp dirstreams only need to support read and close operations,
   They can't be rewound because the underlying ftp stream can't be rewound. */
static php_stream_ops php_ftp_dirstream_ops = {
	NULL, /* write */
	php_ftp_dirstream_read, /* read */
	php_ftp_dirstream_close, /* close */
	NULL, /* flush */
	"ftpdir",
	NULL, /* rewind */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set option */
};

/* {{{ php_stream_ftp_opendir
 */
php_stream * php_stream_ftp_opendir(php_stream_wrapper *wrapper, const char *path, const char *mode, int options,
									char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream, *reuseid, *datastream = NULL;
	php_ftp_dirstream_data *dirsdata;
	php_url *resource = NULL;
	int result = 0, use_ssl, use_ssl_on_data = 0;
	char *hoststart = NULL, tmp_line[512];
	char ip[sizeof("123.123.123.123")];
	unsigned short portno;

	tmp_line[0] = '\0';

	stream = php_ftp_fopen_connect(wrapper, path, mode, options, opened_path, context, &reuseid, &resource, &use_ssl, &use_ssl_on_data TSRMLS_CC);
	if (!stream) {
		goto opendir_errexit;
	}

	/* set the connection to be ascii */
	php_stream_write_string(stream, "TYPE A\r\n");
	result = GET_FTP_RESULT(stream);
	if (result > 299 || result < 200)
		goto opendir_errexit;

	// tmp_line isn't relevant after the php_fopen_do_pasv().
	tmp_line[0] = '\0';

	/* set up the passive connection */
	portno = php_fopen_do_pasv(stream, ip, sizeof(ip), &hoststart TSRMLS_CC);

	if (!portno) {
		goto opendir_errexit;
	}

	/* open the data channel */
	if (hoststart == NULL) {
		hoststart = resource->host;
	}

	datastream = php_stream_sock_open_host(hoststart, portno, SOCK_STREAM, 0, 0);
	if (datastream == NULL) {
		goto opendir_errexit;
	}


	php_stream_printf(stream TSRMLS_CC, "NLST %s\r\n", (resource->path != NULL ? resource->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result != 150 && result != 125) {
		/* Could not retrieve or send the file
		 * this data will only be sent to us after connection on the data port was initiated.
		 */
		php_stream_close(datastream);
		datastream = NULL;
		goto opendir_errexit;
	}

	php_stream_context_set(datastream, context);
	if (use_ssl_on_data && (php_stream_xport_crypto_setup(datastream,
			STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0 ||
			php_stream_xport_crypto_enable(datastream, 1 TSRMLS_CC) < 0)) {

		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to activate SSL mode");
		php_stream_close(datastream);
		datastream = NULL;
		goto opendir_errexit;
	}

	php_url_free(resource);

	dirsdata = emalloc(sizeof *dirsdata);
	dirsdata->datastream = datastream;
	dirsdata->controlstream = stream;
	dirsdata->dirstream = php_stream_alloc(&php_ftp_dirstream_ops, dirsdata, 0, mode);

	return dirsdata->dirstream;

opendir_errexit:
	if (resource) {
		php_url_free(resource);
	}
	if (stream) {
		php_stream_notify_error(context, PHP_STREAM_NOTIFY_FAILURE, tmp_line, result);
		php_stream_close(stream);
	}
	if (tmp_line[0] != '\0') {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP server reports %s", tmp_line);
	}
	return NULL;
}
/* }}} */

/* {{{ php_stream_ftp_url_stat
 */
static int php_stream_ftp_url_stat(php_stream_wrapper *wrapper, const char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int result;
	char tmp_line[512];

	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) return -1;

	stream = php_ftp_fopen_connect(wrapper, url, "r", 0, NULL, context, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		goto stat_errexit;
	}

	ssb->sb.st_mode = 0644;									/* FTP won't give us a valid mode, so aproximate one based on being readable */
	php_stream_printf(stream TSRMLS_CC, "CWD %s\r\n", (resource->path != NULL ? resource->path : "/")); /* If we can CWD to it, it's a directory (maybe a link, but we can't tell) */
	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		ssb->sb.st_mode |= S_IFREG;
	} else {
		ssb->sb.st_mode |= S_IFDIR;
	}

	php_stream_write_string(stream, "TYPE I\r\n"); /* we need this since some servers refuse to accept SIZE command in ASCII mode */

	result = GET_FTP_RESULT(stream);

	if(result < 200 || result > 299) {
		goto stat_errexit;
	}

	php_stream_printf(stream TSRMLS_CC, "SIZE %s\r\n", (resource->path != NULL ? resource->path : "/"));
	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		/* Failure either means it doesn't exist
		   or it's a directory and this server
		   fails on listing directory sizes */
		if (ssb->sb.st_mode & S_IFDIR) {
			ssb->sb.st_size = 0;
		} else {
			goto stat_errexit;
		}
	} else {
		ssb->sb.st_size = atoi(tmp_line + 4);
	}

	php_stream_printf(stream TSRMLS_CC, "MDTM %s\r\n", (resource->path != NULL ? resource->path : "/"));
	result = GET_FTP_RESULT(stream);
	if (result == 213) {
		char *p = tmp_line + 4;
		int n;
		struct tm tm, tmbuf, *gmt;
		time_t stamp;

		while (p - tmp_line < sizeof(tmp_line) && !isdigit(*p)) {
			p++;
		}

		if (p - tmp_line > sizeof(tmp_line)) {
			goto mdtm_error;
		}

		n = sscanf(p, "%4u%2u%2u%2u%2u%2u", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
		if (n != 6) {
			goto mdtm_error;
		}

		tm.tm_year -= 1900;
		tm.tm_mon--;
		tm.tm_isdst = -1;

		/* figure out the GMT offset */
		stamp = time(NULL);
		gmt = php_gmtime_r(&stamp, &tmbuf);
		if (!gmt) {
			goto mdtm_error;
		}
		gmt->tm_isdst = -1;

		/* apply the GMT offset */
		tm.tm_sec += stamp - mktime(gmt);
		tm.tm_isdst = gmt->tm_isdst;

		ssb->sb.st_mtime = mktime(&tm);
	} else {
		/* error or unsupported command */
mdtm_error:
		ssb->sb.st_mtime = -1;
	}

	ssb->sb.st_ino = 0;						/* Unknown values */
	ssb->sb.st_dev = 0;
	ssb->sb.st_uid = 0;
	ssb->sb.st_gid = 0;
	ssb->sb.st_atime = -1;
	ssb->sb.st_ctime = -1;

	ssb->sb.st_nlink = 1;
	ssb->sb.st_rdev = -1;
#ifdef HAVE_ST_BLKSIZE
	ssb->sb.st_blksize = 4096;				/* Guess since FTP won't expose this information */
#ifdef HAVE_ST_BLOCKS
	ssb->sb.st_blocks = (int)((4095 + ssb->sb.st_size) / ssb->sb.st_blksize); /* emulate ceil */
#endif
#endif
	php_stream_close(stream);
	php_url_free(resource);
	return 0;

stat_errexit:
	if (resource) {
		php_url_free(resource);
	}
	if (stream) {
		php_stream_close(stream);
	}
	return -1;
}
/* }}} */

/* {{{ php_stream_ftp_unlink
 */
static int php_stream_ftp_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int result;
	char tmp_line[512];

	stream = php_ftp_fopen_connect(wrapper, url, "r", 0, NULL, NULL, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", url);
		}
		goto unlink_errexit;
	}

	if (resource->path == NULL) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path provided in %s", url);
		}
		goto unlink_errexit;
	}

	/* Attempt to delete the file */
	php_stream_printf(stream TSRMLS_CC, "DELE %s\r\n", (resource->path != NULL ? resource->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error Deleting file: %s", tmp_line);
		}
		goto unlink_errexit;
	}

	php_url_free(resource);
	php_stream_close(stream);
	return 1;

unlink_errexit:
	if (resource) {
		php_url_free(resource);
	}
	if (stream) {
		php_stream_close(stream);
	}
	return 0;
}
/* }}} */

/* {{{ php_stream_ftp_rename
 */
static int php_stream_ftp_rename(php_stream_wrapper *wrapper, const char *url_from, const char *url_to, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource_from = NULL, *resource_to = NULL;
	int result;
	char tmp_line[512];

	resource_from = php_url_parse(url_from);
	resource_to = php_url_parse(url_to);
	/* Must be same scheme (ftp/ftp or ftps/ftps), same host, and same port
		(or a 21/0 0/21 combination which is also "same")
	   Also require paths to/from */
	if (!resource_from ||
		!resource_to ||
		!resource_from->scheme ||
		!resource_to->scheme ||
		strcmp(resource_from->scheme, resource_to->scheme) ||
		!resource_from->host ||
		!resource_to->host ||
		strcmp(resource_from->host, resource_to->host) ||
		(resource_from->port != resource_to->port &&
		 resource_from->port * resource_to->port != 0 &&
		 resource_from->port + resource_to->port != 21) ||
		!resource_from->path ||
		!resource_to->path) {
		goto rename_errexit;
	}

	stream = php_ftp_fopen_connect(wrapper, url_from, "r", 0, NULL, NULL, NULL, NULL, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", resource_from->host);
		}
		goto rename_errexit;
	}

	/* Rename FROM */
	php_stream_printf(stream TSRMLS_CC, "RNFR %s\r\n", (resource_from->path != NULL ? resource_from->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result < 300 || result > 399) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error Renaming file: %s", tmp_line);
		}
		goto rename_errexit;
	}

	/* Rename TO */
	php_stream_printf(stream TSRMLS_CC, "RNTO %s\r\n", (resource_to->path != NULL ? resource_to->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error Renaming file: %s", tmp_line);
		}
		goto rename_errexit;
	}

	php_url_free(resource_from);
	php_url_free(resource_to);
	php_stream_close(stream);
	return 1;

rename_errexit:
	if (resource_from) {
		php_url_free(resource_from);
	}
	if (resource_to) {
		php_url_free(resource_to);
	}
	if (stream) {
		php_stream_close(stream);
	}
	return 0;
}
/* }}} */

/* {{{ php_stream_ftp_mkdir
 */
static int php_stream_ftp_mkdir(php_stream_wrapper *wrapper, const char *url, int mode, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int result, recursive = options & PHP_STREAM_MKDIR_RECURSIVE;
	char tmp_line[512];

	stream = php_ftp_fopen_connect(wrapper, url, "r", 0, NULL, NULL, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", url);
		}
		goto mkdir_errexit;
	}

	if (resource->path == NULL) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path provided in %s", url);
		}
		goto mkdir_errexit;
	}

	if (!recursive) {
		php_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", resource->path);
		result = GET_FTP_RESULT(stream);
    } else {
        /* we look for directory separator from the end of string, thus hopefuly reducing our work load */
        char *p, *e, *buf;

        buf = estrdup(resource->path);
        e = buf + strlen(buf);

        /* find a top level directory we need to create */
        while ((p = strrchr(buf, '/'))) {
            *p = '\0';
			php_stream_printf(stream TSRMLS_CC, "CWD %s\r\n", buf);
			result = GET_FTP_RESULT(stream);
			if (result >= 200 && result <= 299) {
				*p = '/';
				break;
			}
        }
        if (p == buf) {
			php_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", resource->path);
			result = GET_FTP_RESULT(stream);
        } else {
			php_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", buf);
			result = GET_FTP_RESULT(stream);
			if (result >= 200 && result <= 299) {
				if (!p) {
					p = buf;
				}
				/* create any needed directories if the creation of the 1st directory worked */
				while (++p != e) {
					if (*p == '\0' && *(p + 1) != '\0') {
						*p = '/';
						php_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", buf);
						result = GET_FTP_RESULT(stream);
						if (result < 200 || result > 299) {
							if (options & REPORT_ERRORS) {
								php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", tmp_line);
							}
							break;
						}
					}
				}
			}
		}
        efree(buf);
    }

	php_url_free(resource);
	php_stream_close(stream);

	if (result < 200 || result > 299) {
		/* Failure */
		return 0;
	}

	return 1;

mkdir_errexit:
	if (resource) {
		php_url_free(resource);
	}
	if (stream) {
		php_stream_close(stream);
	}
	return 0;
}
/* }}} */

/* {{{ php_stream_ftp_rmdir
 */
static int php_stream_ftp_rmdir(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int result;
	char tmp_line[512];

	stream = php_ftp_fopen_connect(wrapper, url, "r", 0, NULL, NULL, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", url);
		}
		goto rmdir_errexit;
	}

	if (resource->path == NULL) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path provided in %s", url);
		}
		goto rmdir_errexit;
	}

	php_stream_printf(stream TSRMLS_CC, "RMD %s\r\n", resource->path);
	result = GET_FTP_RESULT(stream);

	if (result < 200 || result > 299) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", tmp_line);
		}
		goto rmdir_errexit;
	}

	php_url_free(resource);
	php_stream_close(stream);

	return 1;

rmdir_errexit:
	if (resource) {
		php_url_free(resource);
	}
	if (stream) {
		php_stream_close(stream);
	}
	return 0;
}
/* }}} */

static php_stream_wrapper_ops ftp_stream_wops = {
	php_stream_url_wrap_ftp,
	php_stream_ftp_stream_close, /* stream_close */
	php_stream_ftp_stream_stat,
	php_stream_ftp_url_stat, /* stat_url */
	php_stream_ftp_opendir, /* opendir */
	"ftp",
	php_stream_ftp_unlink, /* unlink */
	php_stream_ftp_rename, /* rename */
	php_stream_ftp_mkdir,  /* mkdir */
	php_stream_ftp_rmdir   /* rmdir */
};

PHPAPI php_stream_wrapper php_stream_ftp_wrapper =	{
	&ftp_stream_wops,
	NULL,
	1 /* is_url */
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
