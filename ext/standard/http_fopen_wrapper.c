/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_network.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#elif defined(NETWARE)
/*#include <ws2nlm.h>*/
/*#include <sys/socket.h>*/
#ifdef NEW_LIBC
#include <sys/param.h>
#else
#include "netware/param.h"
#endif
#else
#include <sys/param.h>
#endif

#include "php_standard.h"

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
#include <winsock.h>
#elif defined(NETWARE) && defined(USE_WINSOCK)
/*#include <ws2nlm.h>*/
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

#define HTTP_HEADER_BLOCK_SIZE		256

/* {{{ php_fopen_url_wrap_http
 */
FILE *php_fopen_url_wrap_http(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path TSRMLS_DC)
{
	FILE *fp=NULL;
	php_url *resource=NULL;
	char tmp_line[256];
	char location[512];
	char hdr_line[8192];
	int body = 0;
	char *scratch;
	unsigned char *tmp;
	int len;
	int reqok = 0;
	zval *response_header;
	char *http_header_line;
	int http_header_line_length, http_header_line_size;

	resource = php_url_parse((char *) path);
	if (resource == NULL) {
		php_error(E_WARNING, "Invalid URL specified, %s", path);
		*issock = BAD_URL;
		return NULL;
	}
	/* use port 80 if one wasn't specified */
	if (resource->port == 0) {
		resource->port = 80;
	}
	
	*socketd = php_hostconnect(resource->host, resource->port, SOCK_STREAM, 0);
	if (*socketd == -1) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		php_url_free(resource);
		return NULL;
	}
#if 0
	if ((fp = fdopen(*socketd, "r+")) == NULL) {
		php_url_free(resource);
		return NULL;
	}
#ifdef HAVE_SETVBUF
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0) {
		php_url_free(resource);
		return NULL;
	}
#endif
#endif							/*win32 */
	
	strcpy(hdr_line, "GET ");
	
	/* tell remote http which file to get */
	if (resource->path != NULL && *resource->path) {
		strlcat(hdr_line, resource->path, sizeof(hdr_line));
	} else {
		strlcat(hdr_line, "/", sizeof(hdr_line));
	}
	/* append the query string, if any */
	if (resource->query != NULL) {
		strlcat(hdr_line, "?", sizeof(hdr_line));
		strlcat(hdr_line, resource->query, sizeof(hdr_line));
	}
	strlcat(hdr_line, " HTTP/1.0\r\n", sizeof(hdr_line));
	SOCK_WRITE(hdr_line, *socketd);
	
	/* send authorization header if we have user/pass */
	if (resource->user != NULL && resource->pass != NULL) {
		php_url_decode(resource->user, strlen(resource->user));
		php_url_decode(resource->pass, strlen(resource->pass));

		scratch = (char *) emalloc(strlen(resource->user) + strlen(resource->pass) + 2);
		if (!scratch) {
			php_url_free(resource);
			return NULL;
		}
		strcpy(scratch, resource->user);
		strcat(scratch, ":");
		strcat(scratch, resource->pass);
		tmp = php_base64_encode((unsigned char *)scratch, strlen(scratch), NULL);
		
		if (snprintf(hdr_line, sizeof(hdr_line),
					 "Authorization: Basic %s\r\n", tmp) > 0) {
			SOCK_WRITE(hdr_line, *socketd);
		}
		
		efree(scratch);
		efree(tmp);
	}
	/* if the user has configured who they are, send a From: line */
	if (cfg_get_string("from", &scratch) == SUCCESS) {
		if (snprintf(hdr_line, sizeof(hdr_line),
					 "From: %s\r\n", scratch) > 0) {
			SOCK_WRITE(hdr_line, *socketd);
		}
		
	}
	/* send a Host: header so name-based virtual hosts work */
	if (resource->port != 80) {
		len = snprintf(hdr_line, sizeof(hdr_line),
					   "Host: %s:%i\r\n", resource->host, resource->port);
	} else {
		len = snprintf(hdr_line, sizeof(hdr_line),
					   "Host: %s\r\n", resource->host);
	}
	if(len > sizeof(hdr_line) - 1) {
		len = sizeof(hdr_line) - 1;
	}
	if (len > 0) {
		SOCK_WRITE(hdr_line, *socketd);
	}
	
	/* identify ourselves and end the headers */
	SOCK_WRITE("User-Agent: PHP/" PHP_VERSION "\r\n\r\n", *socketd);
	
	body = 0;
	location[0] = '\0';

	MAKE_STD_ZVAL(response_header);
	array_init(response_header);

	if (!SOCK_FEOF(*socketd)) {
		/* get response header */
		if (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, *socketd) != NULL) {
			zval *http_response;

			MAKE_STD_ZVAL(http_response);
			if (strncmp(tmp_line + 8, " 200 ", 5) == 0) {
				reqok = 1;
			}
			Z_STRLEN_P(http_response) = strlen(tmp_line);
			Z_STRVAL_P(http_response) = estrndup(tmp_line, Z_STRLEN_P(http_response));
			if (Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=='\n') {
				Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=0;
				Z_STRLEN_P(http_response)--;
				if (Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=='\r') {
					Z_STRVAL_P(http_response)[Z_STRLEN_P(http_response)-1]=0;
					Z_STRLEN_P(http_response)--;
				}
			}
			Z_TYPE_P(http_response) = IS_STRING;
			zend_hash_next_index_insert(Z_ARRVAL_P(response_header), &http_response, sizeof(zval *), NULL);
		}
	}


	/* Read past HTTP headers */
	while (!body && !SOCK_FEOF(*socketd)) {
		http_header_line = emalloc(HTTP_HEADER_BLOCK_SIZE);
		http_header_line_size = HTTP_HEADER_BLOCK_SIZE;
		http_header_line_length = 0;
		if (SOCK_FGETS(http_header_line, HTTP_HEADER_BLOCK_SIZE-1, *socketd) != NULL) {
			char *p;
			zend_bool found_eol=0;
			zval *http_header;
			
			http_header_line[HTTP_HEADER_BLOCK_SIZE-1] = '\0';
			
			do {
				p = http_header_line+http_header_line_length;
				while (*p) {
					while (*p == '\n' || *p == '\r') {
						*p = '\0';
						p--;
						found_eol=1;
					}
					if (found_eol) {
						break;
					}
					p++;
				}
				if (!found_eol) {
					http_header_line_size += HTTP_HEADER_BLOCK_SIZE;
					http_header_line_length += HTTP_HEADER_BLOCK_SIZE-1;
					http_header_line = erealloc(http_header_line, http_header_line_size);
					if (SOCK_FGETS(http_header_line+http_header_line_length, HTTP_HEADER_BLOCK_SIZE-1, *socketd)==NULL) {
						http_header_line[http_header_line_length] = 0;
						break;
					}
				} else {
					http_header_line_length = p-http_header_line+1;
				}
			} while (!found_eol);
			
			if (!strncasecmp(http_header_line, "Location: ", 10)) {
				strlcpy(location, http_header_line + 10, sizeof(location));
			}
			
			if (http_header_line[0] == '\0') {
				body = 1;
			}

			if (http_header_line_length>0) {
				MAKE_STD_ZVAL(http_header);
				Z_STRVAL_P(http_header) = http_header_line;
				Z_STRLEN_P(http_header) = http_header_line_length;
				Z_TYPE_P(http_header) = IS_STRING;
				zend_hash_next_index_insert(Z_ARRVAL_P(response_header), &http_header, sizeof(zval *), NULL);
			} else {
				efree(http_header_line);
			}
		}
	}

	if (!reqok) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		if (location[0] != '\0') {
			zval **response_header_new, *entry, **entryp;
			char new_path[512];

			*new_path='\0';
			if (strlen(location)<8 || strncasecmp(location, "http://", 7)) {
				strcpy(new_path, "http://");
				strlcat(new_path, resource->host, sizeof(new_path));
				if (resource->port != 80) {
					snprintf(new_path+strlen(new_path), sizeof(new_path)-strlen(new_path)-1, ":%d", resource->port);
				}
				if (*location != '/') {
					php_dirname(resource->path, strlen(resource->path));
					snprintf (new_path+strlen(new_path), sizeof(new_path)-strlen(new_path)-1, "%s/", resource->path);
				}
				strlcat(new_path, location, sizeof(new_path));
			}
			else {
				strlcpy(new_path, location, sizeof(new_path));
			}
			php_url_free(resource);
			fp = php_fopen_url_wrap_http(new_path, mode, options, issock, socketd, opened_path TSRMLS_CC);
			if (zend_hash_find(EG(active_symbol_table), "http_response_header", sizeof("http_response_header"), (void **) &response_header_new) == SUCCESS) {
				entryp = &entry;
				MAKE_STD_ZVAL(entry);
				ZVAL_EMPTY_STRING(entry);
				zend_hash_next_index_insert(Z_ARRVAL_P(response_header), entryp, sizeof(zval *), NULL);
				zend_hash_internal_pointer_reset(Z_ARRVAL_PP(response_header_new));
				while (zend_hash_get_current_data(Z_ARRVAL_PP(response_header_new), (void **)&entryp) == SUCCESS) {
					zval_add_ref(entryp);
					zend_hash_next_index_insert(Z_ARRVAL_P(response_header), entryp, sizeof(zval *), NULL);
					zend_hash_move_forward(Z_ARRVAL_PP(response_header_new));
				}
			}
			goto out;
		} else {
			php_url_free(resource);
			fp = NULL;
			goto out;
		}
	}
	php_url_free(resource);
	*issock = 1;
 out:
	{
		ZEND_SET_SYMBOL(EG(active_symbol_table), "http_response_header", response_header);
	}	
	return (fp);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
