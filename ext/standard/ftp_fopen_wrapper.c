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

static int php_get_ftp_result(int socketd)
{
	char tmp_line[513];

	while (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, socketd) &&
		   !(isdigit((int) tmp_line[0]) && isdigit((int) tmp_line[1]) &&
			 isdigit((int) tmp_line[2]) && tmp_line[3] == ' '));

	return strtol(tmp_line, NULL, 10);
}

/* {{{ php_fopen_url_wrap_ftp
 */
FILE *php_fopen_url_wrap_ftp(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path TSRMLS_DC)
{
	FILE *fp=NULL;
	php_url *resource=NULL;
	char tmp_line[512];
	unsigned short portno;
	char *scratch;
	int result;
	int i;
	char *tpath, *ttpath;
	
	resource = php_url_parse((char *) path);
	if (resource == NULL) {
		php_error(E_WARNING, "Invalid URL specified, %s", path);
		*issock = BAD_URL;
		return NULL;
	} else if (resource->path == NULL) {
		php_error(E_WARNING, "No file-path specified");
		php_url_free(resource);
		*issock = BAD_URL;
		return NULL;
	}
	/* use port 21 if one wasn't specified */
	if (resource->port == 0)
		resource->port = 21;

	*socketd = php_hostconnect(resource->host, resource->port, SOCK_STREAM, 0);
	if (*socketd == -1)
		goto errexit;
#if 0
	if ((fpc = fdopen(*socketd, "r+")) == NULL) {
		php_url_free(resource);
		return NULL;
	}
#ifdef HAVE_SETVBUF
	if ((setvbuf(fpc, NULL, _IONBF, 0)) != 0) {
		php_url_free(resource);
		fclose(fpc);
		return NULL;
	}
#endif
#endif
	
	/* Start talking to ftp server */
	result = php_get_ftp_result(*socketd);
	if (result > 299 || result < 200)
		goto errexit;

	/* send the user name */
	SOCK_WRITE("USER ", *socketd);
	if (resource->user != NULL) {
		php_raw_url_decode(resource->user, strlen(resource->user));
		SOCK_WRITE(resource->user, *socketd);
	} else {
		SOCK_WRITE("anonymous", *socketd);
	}
	SOCK_WRITE("\r\n", *socketd);
	
	/* get the response */
	result = php_get_ftp_result(*socketd);
	
	/* if a password is required, send it */
	if (result >= 300 && result <= 399) {
		SOCK_WRITE("PASS ", *socketd);
		if (resource->pass != NULL) {
			php_raw_url_decode(resource->pass, strlen(resource->pass));
			SOCK_WRITE(resource->pass, *socketd);
		} else {
			/* if the user has configured who they are,
			   send that as the password */
			if (cfg_get_string("from", &scratch) == SUCCESS) {
				SOCK_WRITE(scratch, *socketd);
			} else {
				SOCK_WRITE("anonymous", *socketd);
			}
		}
		SOCK_WRITE("\r\n", *socketd);
		
		/* read the response */
		result = php_get_ftp_result(*socketd);
	}
	if (result > 299 || result < 200)
		goto errexit;
	
	/* set the connection to be binary */
	SOCK_WRITE("TYPE I\r\n", *socketd);
	result = php_get_ftp_result(*socketd);
	if (result > 299 || result < 200)
		goto errexit;
	
	/* find out the size of the file (verifying it exists) */
	SOCK_WRITE("SIZE ", *socketd);
	SOCK_WRITE(resource->path, *socketd);
	SOCK_WRITE("\r\n", *socketd);
	
	/* read the response */
	result = php_get_ftp_result(*socketd);
	if (mode[0] == 'r') {
		/* when reading file, it must exist */
		if (result > 299 || result < 200) {
			php_error(E_WARNING, "File not found");
			php_url_free(resource);
			SOCK_FCLOSE(*socketd);
			*socketd = 0;
			errno = ENOENT;
			return NULL;
		}
	} else {
		/* when writing file, it must NOT exist */
		if (result <= 299 && result >= 200) {
			php_error(E_WARNING, "File already exists");
			php_url_free(resource);
			SOCK_FCLOSE(*socketd);
			*socketd = 0;
			errno = EEXIST;
			return NULL;
		}
	}
	
	/* set up the passive connection */

    /* We try EPSV first, needed for IPv6 and works on some IPv4 servers */
	SOCK_WRITE("EPSV\r\n", *socketd);
	while (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, *socketd) &&
		   !(isdigit((int) tmp_line[0]) && isdigit((int) tmp_line[1]) &&
			 isdigit((int) tmp_line[2]) && tmp_line[3] == ' '));

	/* check if we got a 229 response */
	if (strncmp(tmp_line, "229", 3)) {
		/* EPSV failed, let's try PASV */
		SOCK_WRITE("PASV\r\n", *socketd);
		while (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, *socketd) &&
			   !(isdigit((int) tmp_line[0]) && isdigit((int) tmp_line[1]) &&
				 isdigit((int) tmp_line[2]) && tmp_line[3] == ' '));
		/* make sure we got a 227 response */
		if (strncmp(tmp_line, "227", 3))
			goto errexit;
		/* parse pasv command (129, 80, 95, 25, 13, 221) */
		tpath = tmp_line;
		/* skip over the "227 Some message " part */
		for (tpath += 4; *tpath && !isdigit((int) *tpath); tpath++);
		if (!*tpath)
			goto errexit;
		/* skip over the host ip, we just assume it's the same */
		for (i = 0; i < 4; i++) {
			for (; isdigit((int) *tpath); tpath++);
			if (*tpath != ',')
				goto errexit;
			tpath++;
		}
		/* pull out the MSB of the port */
		portno = (unsigned short) strtol(tpath, &ttpath, 10) * 256;
		if (ttpath == NULL) {
			/* didn't get correct response from PASV */
			goto errexit;
		}
		tpath = ttpath;
		if (*tpath != ',')
			goto errexit;
		tpath++;
		/* pull out the LSB of the port */
		portno += (unsigned short) strtol(tpath, &ttpath, 10);
	} else {
		/* parse epsv command (|||6446|) */
		for (i = 0, tpath = tmp_line + 4; *tpath; tpath++) {
			if (*tpath == '|') {
				i++;
				if (i == 3)
					break;
			}
		}
		if (i < 3)
			goto errexit;
		/* pull out the port */
		portno = (unsigned short) strtol(tpath + 1, &ttpath, 10);
	}
	
	if (ttpath == NULL) {
		/* didn't get correct response from EPSV/PASV */
		goto errexit;
	}
	
	if (mode[0] == 'r') {
		/* retrieve file */
		SOCK_WRITE("RETR ", *socketd);
	} else {
		/* store file */
		SOCK_WRITE("STOR ", *socketd);
	} 
	if (resource->path != NULL) {
		SOCK_WRITE(resource->path, *socketd);
	} else {
		SOCK_WRITE("/", *socketd);
	}
	
	/* close control connection */
	SOCK_WRITE("\r\nQUIT\r\n", *socketd);
	SOCK_FCLOSE(*socketd);

	/* open the data channel */
	*socketd = php_hostconnect(resource->host, portno, SOCK_STREAM, 0);
	if (*socketd == -1)
		goto errexit;
#if 0
	if (mode[0] == 'r') {
		if ((fp = fdopen(*socketd, "r+")) == NULL) {
			php_url_free(resource);
			return NULL;
		}
	} else {
		if ((fp = fdopen(*socketd, "w+")) == NULL) {
			php_url_free(resource);
			return NULL;
		}
	}
#ifdef HAVE_SETVBUF
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0) {
		php_url_free(resource);
		fclose(fp);
		return NULL;
	}
#endif
#endif
	php_url_free(resource);
	*issock = 1;
	return (fp);

 errexit:
	php_url_free(resource);
	SOCK_FCLOSE(*socketd);
	*socketd = 0;
	return NULL;
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
