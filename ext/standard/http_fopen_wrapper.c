/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
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
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#ifdef PHP_WIN32
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "php_fopen_wrappers.h"

FILE *php_fopen_url_wrap_http(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	FILE *fp=NULL;
	php_url *resource=NULL;
	char tmp_line[512];
	char location[512];
	char hdr_line[8192];
	int body = 0;
	char *scratch;
	unsigned char *tmp;
	int len;
	int reqok = 0;

	resource = url_parse((char *) path);
	if (resource == NULL) {
		php_error(E_WARNING, "Invalid URL specified, %s", path);
		*issock = BAD_URL;
		return NULL;
	}
	/* use port 80 if one wasn't specified */
	if (resource->port == 0)
		resource->port = 80;
	
	*socketd = php_hostconnect(resource->host, resource->port, SOCK_STREAM, 0);
	if (*socketd == -1) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
#if 0
	if ((fp = fdopen(*socketd, "r+")) == NULL) {
		free_url(resource);
		return NULL;
	}
#ifdef HAVE_SETVBUF
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0) {
		free_url(resource);
		return NULL;
	}
#endif
#endif							/*win32 */
	
	strcpy(hdr_line, "GET ");
	
	/* tell remote http which file to get */
	if (resource->path != NULL) {
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
		scratch = (char *) emalloc(strlen(resource->user) + strlen(resource->pass) + 2);
		if (!scratch) {
			free_url(resource);
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
	if (!SOCK_FEOF(*socketd)) {
		/* get response header */
		if (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, *socketd) != NULL) {
			if (strncmp(tmp_line + 8, " 200 ", 5) == 0) {
				reqok = 1;
			}
		}
	}
	/* Read past HTTP headers */
	while (!body && !SOCK_FEOF(*socketd)) {
		if (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, *socketd) != NULL) {
			char *p = tmp_line;
			
			tmp_line[sizeof(tmp_line)-1] = '\0';
			
			while (*p) {
				if (*p == '\n' || *p == '\r') {
					*p = '\0';
				}
				p++;
			}
			
			if (!strncasecmp(tmp_line, "Location: ", 10)) {
				strlcpy(location, tmp_line + 10, sizeof(location));
			}
			
			if (tmp_line[0] == '\0') {
					body = 1;
			}
		}
	}
	if (!reqok) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
#if 0
		if (location[0] != '\0') {
			return php_fopen_url_wrapper(location, mode, options, issock, socketd, opened_path);
		} else {
			return NULL;
		}
#else
		return NULL;
#endif
	}
	free_url(resource);
	*issock = 1;
	return (fp);
}
