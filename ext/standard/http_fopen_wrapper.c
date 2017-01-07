/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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
   |          Wez Furlong <wez@thebrainroom.com>                          |
   |          Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_streams.h"
#include "php_network.h"
#include "php_ini.h"
#include "ext/standard/basic_functions.h"
#include "zend_smart_str.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
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

#define HTTP_HEADER_BLOCK_SIZE		1024
#define PHP_URL_REDIRECT_MAX		20
#define HTTP_HEADER_USER_AGENT		1
#define HTTP_HEADER_HOST			2
#define HTTP_HEADER_AUTH			4
#define HTTP_HEADER_FROM			8
#define HTTP_HEADER_CONTENT_LENGTH	16
#define HTTP_HEADER_TYPE			32
#define HTTP_HEADER_CONNECTION		64

#define HTTP_WRAPPER_HEADER_INIT    1
#define HTTP_WRAPPER_REDIRECTED     2

static inline void strip_header(char *header_bag, char *lc_header_bag,
		const char *lc_header_name)
{
	char *lc_header_start = strstr(lc_header_bag, lc_header_name);
	char *header_start = header_bag + (lc_header_start - lc_header_bag);

	if (lc_header_start
	&& (lc_header_start == lc_header_bag || *(lc_header_start-1) == '\n')
	) {
		char *lc_eol = strchr(lc_header_start, '\n');
		char *eol = header_start + (lc_eol - lc_header_start);

		if (lc_eol) {
			size_t eollen = strlen(lc_eol);

			memmove(lc_header_start, lc_eol+1, eollen);
			memmove(header_start, eol+1, eollen);
		} else {
			*lc_header_start = '\0';
			*header_start = '\0';
		}
	}
}

php_stream *php_stream_url_wrap_http_ex(php_stream_wrapper *wrapper,
		const char *path, const char *mode, int options, zend_string **opened_path,
		php_stream_context *context, int redirect_max, int flags STREAMS_DC) /* {{{ */
{
	php_stream *stream = NULL;
	php_url *resource = NULL;
	int use_ssl;
	int use_proxy = 0;
	char *scratch = NULL;
	zend_string *tmp = NULL;
	char *ua_str = NULL;
	zval *ua_zval = NULL, *tmpzval = NULL, ssl_proxy_peer_name;
	size_t scratch_len = 0;
	char location[HTTP_HEADER_BLOCK_SIZE];
	zval response_header;
	int reqok = 0;
	char *http_header_line = NULL;
	char tmp_line[128];
	size_t chunk_size = 0, file_size = 0;
	int eol_detect = 0;
	char *transport_string;
	zend_string *errstr = NULL;
	size_t transport_len;
	int have_header = 0;
	zend_bool request_fulluri = 0, ignore_errors = 0;
	char *protocol_version = NULL;
	int protocol_version_len = 3; /* Default: "1.0" */
	struct timeval timeout;
	char *user_headers = NULL;
	int header_init = ((flags & HTTP_WRAPPER_HEADER_INIT) != 0);
	int redirected = ((flags & HTTP_WRAPPER_REDIRECTED) != 0);
	zend_bool follow_location = 1;
	php_stream_filter *transfer_encoding = NULL;
	int response_code;
	zend_array *symbol_table;

	ZVAL_UNDEF(&response_header);
	tmp_line[0] = '\0';

	if (redirect_max < 1) {
		php_stream_wrapper_log_error(wrapper, options, "Redirection limit reached, aborting");
		return NULL;
	}

	resource = php_url_parse(path);
	if (resource == NULL) {
		return NULL;
	}

	if (strncasecmp(resource->scheme, "http", sizeof("http")) && strncasecmp(resource->scheme, "https", sizeof("https"))) {
		if (!context ||
			(tmpzval = php_stream_context_get_option(context, wrapper->wops->label, "proxy")) == NULL ||
			Z_TYPE_P(tmpzval) != IS_STRING ||
			Z_STRLEN_P(tmpzval) <= 0) {
			php_url_free(resource);
			return php_stream_open_wrapper_ex(path, mode, REPORT_ERRORS, NULL, context);
		}
		/* Called from a non-http wrapper with http proxying requested (i.e. ftp) */
		request_fulluri = 1;
		use_ssl = 0;
		use_proxy = 1;

		transport_len = Z_STRLEN_P(tmpzval);
		transport_string = estrndup(Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval));
	} else {
		/* Normal http request (possibly with proxy) */

		if (strpbrk(mode, "awx+")) {
			php_stream_wrapper_log_error(wrapper, options, "HTTP wrapper does not support writeable connections");
			php_url_free(resource);
			return NULL;
		}

		use_ssl = resource->scheme && (strlen(resource->scheme) > 4) && resource->scheme[4] == 's';
		/* choose default ports */
		if (use_ssl && resource->port == 0)
			resource->port = 443;
		else if (resource->port == 0)
			resource->port = 80;

		if (context &&
			(tmpzval = php_stream_context_get_option(context, wrapper->wops->label, "proxy")) != NULL &&
			Z_TYPE_P(tmpzval) == IS_STRING &&
			Z_STRLEN_P(tmpzval) > 0) {
			use_proxy = 1;
			transport_len = Z_STRLEN_P(tmpzval);
			transport_string = estrndup(Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval));
		} else {
			transport_len = spprintf(&transport_string, 0, "%s://%s:%d", use_ssl ? "ssl" : "tcp", resource->host, resource->port);
		}
	}

	if (context && (tmpzval = php_stream_context_get_option(context, wrapper->wops->label, "timeout")) != NULL) {
		double d = zval_get_double(tmpzval);
#ifndef PHP_WIN32
		timeout.tv_sec = (time_t) d;
		timeout.tv_usec = (size_t) ((d - timeout.tv_sec) * 1000000);
#else
		timeout.tv_sec = (long) d;
		timeout.tv_usec = (long) ((d - timeout.tv_sec) * 1000000);
#endif
	} else {
#ifndef PHP_WIN32
		timeout.tv_sec = FG(default_socket_timeout);
#else
		timeout.tv_sec = (long)FG(default_socket_timeout);
#endif
		timeout.tv_usec = 0;
	}

	stream = php_stream_xport_create(transport_string, transport_len, options,
			STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
			NULL, &timeout, context, &errstr, NULL);

	if (stream) {
		php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &timeout);
	}

	if (errstr) {
		php_stream_wrapper_log_error(wrapper, options, "%s", ZSTR_VAL(errstr));
		zend_string_release(errstr);
		errstr = NULL;
	}

	efree(transport_string);

	if (stream && use_proxy && use_ssl) {
		smart_str header = {0};

		/* Set peer_name or name verification will try to use the proxy server name */
		if (!context || (tmpzval = php_stream_context_get_option(context, "ssl", "peer_name")) == NULL) {
			ZVAL_STRING(&ssl_proxy_peer_name, resource->host);
			php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "peer_name", &ssl_proxy_peer_name);
			zval_ptr_dtor(&ssl_proxy_peer_name);
		}

		smart_str_appendl(&header, "CONNECT ", sizeof("CONNECT ")-1);
		smart_str_appends(&header, resource->host);
		smart_str_appendc(&header, ':');
		smart_str_append_unsigned(&header, resource->port);
		smart_str_appendl(&header, " HTTP/1.0\r\n", sizeof(" HTTP/1.0\r\n")-1);

	    /* check if we have Proxy-Authorization header */
		if (context && (tmpzval = php_stream_context_get_option(context, "http", "header")) != NULL) {
			char *s, *p;

			if (Z_TYPE_P(tmpzval) == IS_ARRAY) {
				zval *tmpheader = NULL;

				ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tmpzval), tmpheader) {
					if (Z_TYPE_P(tmpheader) == IS_STRING) {
						s = Z_STRVAL_P(tmpheader);
						do {
							while (*s == ' ' || *s == '\t') s++;
							p = s;
							while (*p != 0 && *p != ':' && *p != '\r' && *p !='\n') p++;
							if (*p == ':') {
								p++;
								if (p - s == sizeof("Proxy-Authorization:") - 1 &&
								    zend_binary_strcasecmp(s, sizeof("Proxy-Authorization:") - 1,
								        "Proxy-Authorization:", sizeof("Proxy-Authorization:") - 1) == 0) {
									while (*p != 0 && *p != '\r' && *p !='\n') p++;
									smart_str_appendl(&header, s, p - s);
									smart_str_appendl(&header, "\r\n", sizeof("\r\n")-1);
									goto finish;
								} else {
									while (*p != 0 && *p != '\r' && *p !='\n') p++;
								}
							}
							s = p;
							while (*s == '\r' || *s == '\n') s++;
						} while (*s != 0);
					}
				} ZEND_HASH_FOREACH_END();
			} else if (Z_TYPE_P(tmpzval) == IS_STRING && Z_STRLEN_P(tmpzval)) {
				s = Z_STRVAL_P(tmpzval);
				do {
					while (*s == ' ' || *s == '\t') s++;
					p = s;
					while (*p != 0 && *p != ':' && *p != '\r' && *p !='\n') p++;
					if (*p == ':') {
						p++;
						if (p - s == sizeof("Proxy-Authorization:") - 1 &&
						    zend_binary_strcasecmp(s, sizeof("Proxy-Authorization:") - 1,
						        "Proxy-Authorization:", sizeof("Proxy-Authorization:") - 1) == 0) {
							while (*p != 0 && *p != '\r' && *p !='\n') p++;
							smart_str_appendl(&header, s, p - s);
							smart_str_appendl(&header, "\r\n", sizeof("\r\n")-1);
							goto finish;
						} else {
							while (*p != 0 && *p != '\r' && *p !='\n') p++;
						}
					}
					s = p;
					while (*s == '\r' || *s == '\n') s++;
				} while (*s != 0);
			}
		}
finish:
		smart_str_appendl(&header, "\r\n", sizeof("\r\n")-1);

		if (php_stream_write(stream, ZSTR_VAL(header.s), ZSTR_LEN(header.s)) != ZSTR_LEN(header.s)) {
			php_stream_wrapper_log_error(wrapper, options, "Cannot connect to HTTPS server through proxy");
			php_stream_close(stream);
			stream = NULL;
		}
 	 	smart_str_free(&header);

 	 	if (stream) {
 	 		char header_line[HTTP_HEADER_BLOCK_SIZE];

			/* get response header */
			while (php_stream_gets(stream, header_line, HTTP_HEADER_BLOCK_SIZE-1) != NULL) {
				if (header_line[0] == '\n' ||
				    header_line[0] == '\r' ||
				    header_line[0] == '\0') {
				  break;
				}
			}
		}

		/* enable SSL transport layer */
		if (stream) {
			if (php_stream_xport_crypto_setup(stream, STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL) < 0 ||
			    php_stream_xport_crypto_enable(stream, 1) < 0) {
				php_stream_wrapper_log_error(wrapper, options, "Cannot connect to HTTPS server through proxy");
				php_stream_close(stream);
				stream = NULL;
			}
		}
	}

	if (stream == NULL)
		goto out;

	/* avoid buffering issues while reading header */
	if (options & STREAM_WILL_CAST)
		chunk_size = php_stream_set_chunk_size(stream, 1);

	/* avoid problems with auto-detecting when reading the headers -> the headers
	 * are always in canonical \r\n format */
	eol_detect = stream->flags & (PHP_STREAM_FLAG_DETECT_EOL | PHP_STREAM_FLAG_EOL_MAC);
	stream->flags &= ~(PHP_STREAM_FLAG_DETECT_EOL | PHP_STREAM_FLAG_EOL_MAC);

	php_stream_context_set(stream, context);

	php_stream_notify_info(context, PHP_STREAM_NOTIFY_CONNECT, NULL, 0);

	if (header_init && context && (tmpzval = php_stream_context_get_option(context, "http", "max_redirects")) != NULL) {
		redirect_max = (int)zval_get_long(tmpzval);
	}

	if (context && (tmpzval = php_stream_context_get_option(context, "http", "method")) != NULL) {
		if (Z_TYPE_P(tmpzval) == IS_STRING && Z_STRLEN_P(tmpzval) > 0) {
			/* As per the RFC, automatically redirected requests MUST NOT use other methods than
			 * GET and HEAD unless it can be confirmed by the user */
			if (!redirected
				|| (Z_STRLEN_P(tmpzval) == 3 && memcmp("GET", Z_STRVAL_P(tmpzval), 3) == 0)
				|| (Z_STRLEN_P(tmpzval) == 4 && memcmp("HEAD",Z_STRVAL_P(tmpzval), 4) == 0)
			) {
				scratch_len = strlen(path) + 29 + Z_STRLEN_P(tmpzval);
				scratch = emalloc(scratch_len);
				strlcpy(scratch, Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval) + 1);
				strncat(scratch, " ", 1);
			}
		}
	}

	if (context && (tmpzval = php_stream_context_get_option(context, "http", "protocol_version")) != NULL) {
		protocol_version_len = (int)spprintf(&protocol_version, 0, "%.1F", zval_get_double(tmpzval));
	}

	if (!scratch) {
		scratch_len = strlen(path) + 29 + protocol_version_len;
		scratch = emalloc(scratch_len);
		strncpy(scratch, "GET ", scratch_len);
	}

	/* Should we send the entire path in the request line, default to no. */
	if (!request_fulluri && context &&
		(tmpzval = php_stream_context_get_option(context, "http", "request_fulluri")) != NULL) {
		request_fulluri = zend_is_true(tmpzval);
	}

	if (request_fulluri) {
		/* Ask for everything */
		strcat(scratch, path);
	} else {
		/* Send the traditional /path/to/file?query_string */

		/* file */
		if (resource->path && *resource->path) {
			strlcat(scratch, resource->path, scratch_len);
		} else {
			strlcat(scratch, "/", scratch_len);
		}

		/* query string */
		if (resource->query) {
			strlcat(scratch, "?", scratch_len);
			strlcat(scratch, resource->query, scratch_len);
		}
	}

	/* protocol version we are speaking */
	if (protocol_version) {
		strlcat(scratch, " HTTP/", scratch_len);
		strlcat(scratch, protocol_version, scratch_len);
		strlcat(scratch, "\r\n", scratch_len);
	} else {
		strlcat(scratch, " HTTP/1.0\r\n", scratch_len);
	}

	/* send it */
	php_stream_write(stream, scratch, strlen(scratch));

	if (context && (tmpzval = php_stream_context_get_option(context, "http", "header")) != NULL) {
		tmp = NULL;

		if (Z_TYPE_P(tmpzval) == IS_ARRAY) {
			zval *tmpheader = NULL;
			smart_str tmpstr = {0};

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tmpzval), tmpheader) {
				if (Z_TYPE_P(tmpheader) == IS_STRING) {
					smart_str_append(&tmpstr, Z_STR_P(tmpheader));
					smart_str_appendl(&tmpstr, "\r\n", sizeof("\r\n") - 1);
				}
			} ZEND_HASH_FOREACH_END();
			smart_str_0(&tmpstr);
			/* Remove newlines and spaces from start and end. there's at least one extra \r\n at the end that needs to go. */
			if (tmpstr.s) {
				tmp = php_trim(tmpstr.s, NULL, 0, 3);
				smart_str_free(&tmpstr);
			}
		} else if (Z_TYPE_P(tmpzval) == IS_STRING && Z_STRLEN_P(tmpzval)) {
			/* Remove newlines and spaces from start and end php_trim will estrndup() */
			tmp = php_trim(Z_STR_P(tmpzval), NULL, 0, 3);
		}
		if (tmp && ZSTR_LEN(tmp)) {
			char *s;
			char *t;

			user_headers = estrndup(ZSTR_VAL(tmp), ZSTR_LEN(tmp));

			if (ZSTR_IS_INTERNED(tmp)) {
				tmp = zend_string_init(ZSTR_VAL(tmp), ZSTR_LEN(tmp), 0);
			} else if (GC_REFCOUNT(tmp) > 1) {
				GC_REFCOUNT(tmp)--;
				tmp = zend_string_init(ZSTR_VAL(tmp), ZSTR_LEN(tmp), 0);
			}

			/* Make lowercase for easy comparison against 'standard' headers */
			php_strtolower(ZSTR_VAL(tmp), ZSTR_LEN(tmp));
			t = ZSTR_VAL(tmp);

			if (!header_init) {
				/* strip POST headers on redirect */
				strip_header(user_headers, t, "content-length:");
				strip_header(user_headers, t, "content-type:");
			}

			if ((s = strstr(t, "user-agent:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_USER_AGENT;
			}
			if ((s = strstr(t, "host:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_HOST;
			}
			if ((s = strstr(t, "from:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_FROM;
				}
			if ((s = strstr(t, "authorization:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_AUTH;
			}
			if ((s = strstr(t, "content-length:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_CONTENT_LENGTH;
			}
			if ((s = strstr(t, "content-type:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_TYPE;
			}
			if ((s = strstr(t, "connection:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				 have_header |= HTTP_HEADER_CONNECTION;
			}
			/* remove Proxy-Authorization header */
			if (use_proxy && use_ssl && (s = strstr(t, "proxy-authorization:")) &&
			    (s == t || *(s-1) == '\r' || *(s-1) == '\n' ||
			                 *(s-1) == '\t' || *(s-1) == ' ')) {
				char *p = s + sizeof("proxy-authorization:") - 1;

				while (s > t && (*(s-1) == ' ' || *(s-1) == '\t')) s--;
				while (*p != 0 && *p != '\r' && *p != '\n') p++;
				while (*p == '\r' || *p == '\n') p++;
				if (*p == 0) {
					if (s == t) {
						efree(user_headers);
						user_headers = NULL;
					} else {
						while (s > t && (*(s-1) == '\r' || *(s-1) == '\n')) s--;
						user_headers[s - t] = 0;
					}
				} else {
					memmove(user_headers + (s - t), user_headers + (p - t), strlen(p) + 1);
				}
			}

		}
		if (tmp) {
			zend_string_release(tmp);
		}
	}

	/* auth header if it was specified */
	if (((have_header & HTTP_HEADER_AUTH) == 0) && resource->user) {
		zend_string *stmp;
		/* decode the strings first */
		php_url_decode(resource->user, strlen(resource->user));

		/* scratch is large enough, since it was made large enough for the whole URL */
		strcpy(scratch, resource->user);
		strcat(scratch, ":");

		/* Note: password is optional! */
		if (resource->pass) {
			php_url_decode(resource->pass, strlen(resource->pass));
			strcat(scratch, resource->pass);
		}

		stmp = php_base64_encode((unsigned char*)scratch, strlen(scratch));

		if (snprintf(scratch, scratch_len, "Authorization: Basic %s\r\n", ZSTR_VAL(stmp)) > 0) {
			php_stream_write(stream, scratch, strlen(scratch));
			php_stream_notify_info(context, PHP_STREAM_NOTIFY_AUTH_REQUIRED, NULL, 0);
		}

		zend_string_free(stmp);
	}

	/* if the user has configured who they are, send a From: line */
	if (((have_header & HTTP_HEADER_FROM) == 0) && FG(from_address)) {
		if (snprintf(scratch, scratch_len, "From: %s\r\n", FG(from_address)) > 0)
			php_stream_write(stream, scratch, strlen(scratch));
	}

	/* Send Host: header so name-based virtual hosts work */
	if ((have_header & HTTP_HEADER_HOST) == 0) {
		if ((use_ssl && resource->port != 443 && resource->port != 0) ||
			(!use_ssl && resource->port != 80 && resource->port != 0)) {
			if (snprintf(scratch, scratch_len, "Host: %s:%i\r\n", resource->host, resource->port) > 0)
				php_stream_write(stream, scratch, strlen(scratch));
		} else {
			if (snprintf(scratch, scratch_len, "Host: %s\r\n", resource->host) > 0) {
				php_stream_write(stream, scratch, strlen(scratch));
			}
		}
	}

	/* Send a Connection: close header to avoid hanging when the server
	 * interprets the RFC literally and establishes a keep-alive connection,
	 * unless the user specifically requests something else by specifying a
	 * Connection header in the context options. Send that header even for
	 * HTTP/1.0 to avoid issues when the server respond with a HTTP/1.1
	 * keep-alive response, which is the preferred response type. */
	if ((have_header & HTTP_HEADER_CONNECTION) == 0) {
		php_stream_write_string(stream, "Connection: close\r\n");
	}

	if (context &&
	    (ua_zval = php_stream_context_get_option(context, "http", "user_agent")) != NULL &&
		Z_TYPE_P(ua_zval) == IS_STRING) {
		ua_str = Z_STRVAL_P(ua_zval);
	} else if (FG(user_agent)) {
		ua_str = FG(user_agent);
	}

	if (((have_header & HTTP_HEADER_USER_AGENT) == 0) && ua_str) {
#define _UA_HEADER "User-Agent: %s\r\n"
		char *ua;
		size_t ua_len;

		ua_len = sizeof(_UA_HEADER) + strlen(ua_str);

		/* ensure the header is only sent if user_agent is not blank */
		if (ua_len > sizeof(_UA_HEADER)) {
			ua = emalloc(ua_len + 1);
			if ((ua_len = slprintf(ua, ua_len, _UA_HEADER, ua_str)) > 0) {
				ua[ua_len] = 0;
				php_stream_write(stream, ua, ua_len);
			} else {
				php_error_docref(NULL, E_WARNING, "Cannot construct User-agent header");
			}
			efree(ua);
		}
	}

	if (user_headers) {
		/* A bit weird, but some servers require that Content-Length be sent prior to Content-Type for POST
		 * see bug #44603 for details. Since Content-Type maybe part of user's headers we need to do this check first.
		 */
		if (
				header_init &&
				context &&
				!(have_header & HTTP_HEADER_CONTENT_LENGTH) &&
				(tmpzval = php_stream_context_get_option(context, "http", "content")) != NULL &&
				Z_TYPE_P(tmpzval) == IS_STRING && Z_STRLEN_P(tmpzval) > 0
		) {
			scratch_len = slprintf(scratch, scratch_len, "Content-Length: %d\r\n", Z_STRLEN_P(tmpzval));
			php_stream_write(stream, scratch, scratch_len);
			have_header |= HTTP_HEADER_CONTENT_LENGTH;
		}

		php_stream_write(stream, user_headers, strlen(user_headers));
		php_stream_write(stream, "\r\n", sizeof("\r\n")-1);
		efree(user_headers);
	}

	/* Request content, such as for POST requests */
	if (header_init && context &&
		(tmpzval = php_stream_context_get_option(context, "http", "content")) != NULL &&
		Z_TYPE_P(tmpzval) == IS_STRING && Z_STRLEN_P(tmpzval) > 0) {
		if (!(have_header & HTTP_HEADER_CONTENT_LENGTH)) {
			scratch_len = slprintf(scratch, scratch_len, "Content-Length: %d\r\n", Z_STRLEN_P(tmpzval));
			php_stream_write(stream, scratch, scratch_len);
		}
		if (!(have_header & HTTP_HEADER_TYPE)) {
			php_stream_write(stream, "Content-Type: application/x-www-form-urlencoded\r\n",
				sizeof("Content-Type: application/x-www-form-urlencoded\r\n") - 1);
			php_error_docref(NULL, E_NOTICE, "Content-type not specified assuming application/x-www-form-urlencoded");
		}
		php_stream_write(stream, "\r\n", sizeof("\r\n")-1);
		php_stream_write(stream, Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval));
	} else {
		php_stream_write(stream, "\r\n", sizeof("\r\n")-1);
	}

	location[0] = '\0';

	symbol_table = zend_rebuild_symbol_table();

	if (header_init) {
		zval ztmp;
		array_init(&ztmp);
		zend_set_local_var_str("http_response_header", sizeof("http_response_header")-1, &ztmp, 0);
	}

	{
		zval *response_header_ptr = zend_hash_str_find_ind(symbol_table, "http_response_header", sizeof("http_response_header")-1);
		if (!response_header_ptr || Z_TYPE_P(response_header_ptr) != IS_ARRAY) {
			ZVAL_UNDEF(&response_header);
			goto out;
		} else {
			ZVAL_COPY(&response_header, response_header_ptr);
		}
	}

	if (!php_stream_eof(stream)) {
		size_t tmp_line_len;
		/* get response header */

		if (php_stream_get_line(stream, tmp_line, sizeof(tmp_line) - 1, &tmp_line_len) != NULL) {
			zval http_response;

			if (tmp_line_len > 9) {
				response_code = atoi(tmp_line + 9);
			} else {
				response_code = 0;
			}
			if (context && NULL != (tmpzval = php_stream_context_get_option(context, "http", "ignore_errors"))) {
				ignore_errors = zend_is_true(tmpzval);
			}
			/* when we request only the header, don't fail even on error codes */
			if ((options & STREAM_ONLY_GET_HEADERS) || ignore_errors) {
				reqok = 1;
			}

			/* status codes of 1xx are "informational", and will be followed by a real response
			 * e.g "100 Continue". RFC 7231 states that unexpected 1xx status MUST be parsed,
			 * and MAY be ignored. As such, we need to skip ahead to the "real" status*/
			if (response_code >= 100 && response_code < 200) {
				/* consume lines until we find a line starting 'HTTP/1' */
				while (
					!php_stream_eof(stream)
					&& php_stream_get_line(stream, tmp_line, sizeof(tmp_line) - 1, &tmp_line_len) != NULL
					&& ( tmp_line_len < sizeof("HTTP/1") - 1 || strncasecmp(tmp_line, "HTTP/1", sizeof("HTTP/1") - 1) )
				);

				if (tmp_line_len > 9) {
					response_code = atoi(tmp_line + 9);
				} else {
					response_code = 0;
				}
			}
			/* all status codes in the 2xx range are defined by the specification as successful;
			 * all status codes in the 3xx range are for redirection, and so also should never
			 * fail */
			if (response_code >= 200 && response_code < 400) {
				reqok = 1;
			} else {
				switch(response_code) {
					case 403:
						php_stream_notify_error(context, PHP_STREAM_NOTIFY_AUTH_RESULT,
								tmp_line, response_code);
						break;
					default:
						/* safety net in the event tmp_line == NULL */
						if (!tmp_line_len) {
							tmp_line[0] = '\0';
						}
						php_stream_notify_error(context, PHP_STREAM_NOTIFY_FAILURE,
								tmp_line, response_code);
				}
			}
			if (tmp_line[tmp_line_len - 1] == '\n') {
				--tmp_line_len;
				if (tmp_line[tmp_line_len - 1] == '\r') {
					--tmp_line_len;
				}
			}
			ZVAL_STRINGL(&http_response, tmp_line, tmp_line_len);
			zend_hash_next_index_insert(Z_ARRVAL(response_header), &http_response);
		}
	} else {
		php_stream_wrapper_log_error(wrapper, options, "HTTP request failed, unexpected end of socket!");
		goto out;
	}

	/* read past HTTP headers */

	http_header_line = emalloc(HTTP_HEADER_BLOCK_SIZE);

	while (!php_stream_eof(stream)) {
		size_t http_header_line_length;
		
		if (php_stream_get_line(stream, http_header_line, HTTP_HEADER_BLOCK_SIZE, &http_header_line_length) && *http_header_line != '\n' && *http_header_line != '\r') {
			char *e = http_header_line + http_header_line_length - 1;
			char *http_header_value;
			if (*e != '\n') {
				do { /* partial header */
					if (php_stream_get_line(stream, http_header_line, HTTP_HEADER_BLOCK_SIZE, &http_header_line_length) == NULL) {
						php_stream_wrapper_log_error(wrapper, options, "Failed to read HTTP headers");
						goto out;
					}
					e = http_header_line + http_header_line_length - 1;
				} while (*e != '\n');
				continue;
			}
			while (e >= http_header_line && (*e == '\n' || *e == '\r')) {
				e--;
			}

			/* The primary definition of an HTTP header in RFC 7230 states:
			 * > Each header field consists of a case-insensitive field name followed
			 * > by a colon (":"), optional leading whitespace, the field value, and
			 * > optional trailing whitespace. */

			/* Strip trailing whitespace */
			while (e >= http_header_line && (*e == ' ' || *e == '\t')) {
				e--;
			}

			/* Terminate header line */
			e++;
			*e = '\0';
			http_header_line_length = e - http_header_line;

			http_header_value = memchr(http_header_line, ':', http_header_line_length);
			if (http_header_value) {
				http_header_value++; /* Skip ':' */

				/* Strip leading whitespace */
				while (http_header_value < e
						&& (*http_header_value == ' ' || *http_header_value == '\t')) {
					http_header_value++;
				}
			}

			if (!strncasecmp(http_header_line, "Location:", sizeof("Location:")-1)) {
				if (context && (tmpzval = php_stream_context_get_option(context, "http", "follow_location")) != NULL) {
					follow_location = zval_is_true(tmpzval);
				} else if (!((response_code >= 300 && response_code < 304)
						|| 307 == response_code || 308 == response_code)) {
					/* we shouldn't redirect automatically
					if follow_location isn't set and response_code not in (300, 301, 302, 303 and 307)
					see http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3.1
					RFC 7238 defines 308: http://tools.ietf.org/html/rfc7238 */
					follow_location = 0;
				}
				strlcpy(location, http_header_value, sizeof(location));
			} else if (!strncasecmp(http_header_line, "Content-Type:", sizeof("Content-Type:")-1)) {
				php_stream_notify_info(context, PHP_STREAM_NOTIFY_MIME_TYPE_IS, http_header_value, 0);
			} else if (!strncasecmp(http_header_line, "Content-Length:", sizeof("Content-Length")-1)) {
				file_size = atoi(http_header_value);
				php_stream_notify_file_size(context, file_size, http_header_line, 0);
			} else if (
				!strncasecmp(http_header_line, "Transfer-Encoding:", sizeof("Transfer-Encoding")-1)
				&& !strncasecmp(http_header_value, "Chunked", sizeof("Chunked")-1)
			) {

				/* create filter to decode response body */
				if (!(options & STREAM_ONLY_GET_HEADERS)) {
					zend_long decode = 1;

					if (context && (tmpzval = php_stream_context_get_option(context, "http", "auto_decode")) != NULL) {
						decode = zend_is_true(tmpzval);
					}
					if (decode) {
						transfer_encoding = php_stream_filter_create("dechunk", NULL, php_stream_is_persistent(stream));
						if (transfer_encoding) {
							/* don't store transfer-encodeing header */
							continue;
						}
					}
				}
			}

			{
				zval http_header;
				ZVAL_STRINGL(&http_header, http_header_line, http_header_line_length);
				zend_hash_next_index_insert(Z_ARRVAL(response_header), &http_header);
			}
		} else {
			break;
		}
	}

	if (!reqok || (location[0] != '\0' && follow_location)) {
		if (!follow_location || (((options & STREAM_ONLY_GET_HEADERS) || ignore_errors) && redirect_max <= 1)) {
			goto out;
		}

		if (location[0] != '\0')
			php_stream_notify_info(context, PHP_STREAM_NOTIFY_REDIRECTED, location, 0);

		php_stream_close(stream);
		stream = NULL;

		if (location[0] != '\0') {

			char new_path[HTTP_HEADER_BLOCK_SIZE];
			char loc_path[HTTP_HEADER_BLOCK_SIZE];

			*new_path='\0';
			if (strlen(location)<8 || (strncasecmp(location, "http://", sizeof("http://")-1) &&
							strncasecmp(location, "https://", sizeof("https://")-1) &&
							strncasecmp(location, "ftp://", sizeof("ftp://")-1) &&
							strncasecmp(location, "ftps://", sizeof("ftps://")-1)))
			{
				if (*location != '/') {
					if (*(location+1) != '\0' && resource->path) {
						char *s = strrchr(resource->path, '/');
						if (!s) {
							s = resource->path;
							if (!s[0]) {
								efree(s);
								s = resource->path = estrdup("/");
							} else {
								*s = '/';
							}
						}
						s[1] = '\0';
						if (resource->path && *(resource->path) == '/' && *(resource->path + 1) == '\0') {
							snprintf(loc_path, sizeof(loc_path) - 1, "%s%s", resource->path, location);
						} else {
							snprintf(loc_path, sizeof(loc_path) - 1, "%s/%s", resource->path, location);
						}
					} else {
						snprintf(loc_path, sizeof(loc_path) - 1, "/%s", location);
					}
				} else {
					strlcpy(loc_path, location, sizeof(loc_path));
				}
				if ((use_ssl && resource->port != 443) || (!use_ssl && resource->port != 80)) {
					snprintf(new_path, sizeof(new_path) - 1, "%s://%s:%d%s", resource->scheme, resource->host, resource->port, loc_path);
				} else {
					snprintf(new_path, sizeof(new_path) - 1, "%s://%s%s", resource->scheme, resource->host, loc_path);
				}
			} else {
				strlcpy(new_path, location, sizeof(new_path));
			}

			php_url_free(resource);
			/* check for invalid redirection URLs */
			if ((resource = php_url_parse(new_path)) == NULL) {
				php_stream_wrapper_log_error(wrapper, options, "Invalid redirect URL! %s", new_path);
				goto out;
			}

#define CHECK_FOR_CNTRL_CHARS(val) { \
	if (val) { \
		unsigned char *s, *e; \
		size_t l; \
		l = php_url_decode(val, strlen(val)); \
		s = (unsigned char*)val; e = s + l; \
		while (s < e) { \
			if (iscntrl(*s)) { \
				php_stream_wrapper_log_error(wrapper, options, "Invalid redirect URL! %s", new_path); \
				goto out; \
			} \
			s++; \
		} \
	} \
}
			/* check for control characters in login, password & path */
			if (strncasecmp(new_path, "http://", sizeof("http://") - 1) || strncasecmp(new_path, "https://", sizeof("https://") - 1)) {
				CHECK_FOR_CNTRL_CHARS(resource->user)
				CHECK_FOR_CNTRL_CHARS(resource->pass)
				CHECK_FOR_CNTRL_CHARS(resource->path)
			}
			stream = php_stream_url_wrap_http_ex(wrapper, new_path, mode, options, opened_path, context, --redirect_max, HTTP_WRAPPER_REDIRECTED STREAMS_CC);
		} else {
			php_stream_wrapper_log_error(wrapper, options, "HTTP request failed! %s", tmp_line);
		}
	}
out:
	if (protocol_version) {
		efree(protocol_version);
	}

	if (http_header_line) {
		efree(http_header_line);
	}

	if (scratch) {
		efree(scratch);
	}

	if (resource) {
		php_url_free(resource);
	}

	if (stream) {
		if (header_init) {
			ZVAL_COPY(&stream->wrapperdata, &response_header);
		}
		php_stream_notify_progress_init(context, 0, file_size);

		/* Restore original chunk size now that we're done with headers */
		if (options & STREAM_WILL_CAST)
			php_stream_set_chunk_size(stream, (int)chunk_size);

		/* restore the users auto-detect-line-endings setting */
		stream->flags |= eol_detect;

		/* as far as streams are concerned, we are now at the start of
		 * the stream */
		stream->position = 0;

		/* restore mode */
		strlcpy(stream->mode, mode, sizeof(stream->mode));

		if (transfer_encoding) {
			php_stream_filter_append(&stream->readfilters, transfer_encoding);
		}
	} else {
		if (transfer_encoding) {
			php_stream_filter_free(transfer_encoding);
		}
	}

	zval_ptr_dtor(&response_header);

	return stream;
}
/* }}} */

php_stream *php_stream_url_wrap_http(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC) /* {{{ */
{
	return php_stream_url_wrap_http_ex(wrapper, path, mode, options, opened_path, context, PHP_URL_REDIRECT_MAX, HTTP_WRAPPER_HEADER_INIT STREAMS_CC);
}
/* }}} */

static int php_stream_http_stream_stat(php_stream_wrapper *wrapper, php_stream *stream, php_stream_statbuf *ssb) /* {{{ */
{
	/* one day, we could fill in the details based on Date: and Content-Length:
	 * headers.  For now, we return with a failure code to prevent the underlying
	 * file's details from being used instead. */
	return -1;
}
/* }}} */

static php_stream_wrapper_ops http_stream_wops = {
	php_stream_url_wrap_http,
	NULL, /* stream_close */
	php_stream_http_stream_stat,
	NULL, /* stat_url */
	NULL, /* opendir */
	"http",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};

PHPAPI php_stream_wrapper php_stream_http_wrapper = {
	&http_stream_wops,
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
