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
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "SAPI.h"

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

#include "safe_mode.h"
#include "php_realpath.h"
#include "ext/standard/head.h"
#include "ext/standard/php_standard.h"
#include "zend_compile.h"

#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef S_ISREG
#define S_ISREG(mode)	(((mode)&S_IFMT) & S_IFREG)
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

/* The following macros are borrowed from virtual_cwd.c and should be put in
 * a joint header file when we move virtual_cwd to TSRM */
#ifdef ZEND_WIN32
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define DEFAULT_SLASH '\\'
#else
#define IS_SLASH(c)	((c) == '/')
#define DEFAULT_SLASH '/'
#endif


typedef FILE * (*php_fopen_url_wrapper_t) (const char *, char *, int, int *, int *, char **) ;

static FILE *php_fopen_url_wrapper(const char *, char *, int, int *, int *, char **);
static FILE *php_fopen_url_wrap_http(const char *, char *, int, int *, int *, char **);
static FILE *php_fopen_url_wrap_ftp(const char *, char *, int, int *, int *, char **);
static FILE *php_fopen_url_wrap_php(const char *, char *, int, int *, int *, char **);

int php_get_ftp_result(int socketd);



HashTable fopen_url_wrappers_hash;

PHPAPI int php_register_url_wrapper(char *protocol, FILE * (*wrapper)(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)) {
#if PHP_URL_FOPEN
	return zend_hash_add(&fopen_url_wrappers_hash, protocol, strlen(protocol)+1, &wrapper, sizeof(wrapper), NULL);
#else
	return FAILURE;
#endif
}

PHPAPI int php_unregister_url_wrapper(char *protocol) {
#if PHP_URL_FOPEN
	return zend_hash_del(&fopen_url_wrappers_hash, protocol, strlen(protocol)+1);
#else
	return SUCCESS;
#endif
}

int php_init_fopen_wrappers(void) 
{
	int status = SUCCESS;
#if PHP_URL_FOPEN
	if (zend_hash_init(&fopen_url_wrappers_hash, 0, NULL, NULL, 1)==FAILURE) {
		return FAILURE;
	}

 	if(FAILURE==php_register_url_wrapper("http",php_fopen_url_wrap_http)) {
		status = FAILURE;
	} else
	if(FAILURE==php_register_url_wrapper("ftp",php_fopen_url_wrap_ftp)) {
		status = FAILURE;
	} else
	if(FAILURE==php_register_url_wrapper("php",php_fopen_url_wrap_php)) {
		status = FAILURE;
	}

	if(FAILURE==status) {
		zend_hash_destroy(&fopen_url_wrappers_hash);
	}
#endif
	return status;
}

int php_shutdown_fopen_wrappers(void) 
{
#if PHP_URL_FOPEN
		zend_hash_destroy(&fopen_url_wrappers_hash);
#endif

	return SUCCESS;
}


/*
	When open_basedir is not NULL, check if the given filename is located in
	open_basedir. Returns -1 if error or not in the open_basedir, else 0
	
	When open_basedir is NULL, always return 0
*/
PHPAPI int php_check_specific_open_basedir(char *basedir, char *path PLS_DC)
{
	char resolved_name[MAXPATHLEN];
	char resolved_basedir[MAXPATHLEN];
	char local_open_basedir[MAXPATHLEN];
	int local_open_basedir_pos;
	SLS_FETCH();
	
	/* Special case basedir==".": Use script-directory */
	if ((strcmp(PG(open_basedir), ".") == 0) && 
		SG(request_info).path_translated &&
		*SG(request_info).path_translated
		) {
		strlcpy(local_open_basedir, SG(request_info).path_translated, sizeof(local_open_basedir));
		local_open_basedir_pos = strlen(local_open_basedir) - 1;

		/* Strip filename */
		while (!IS_SLASH(local_open_basedir[local_open_basedir_pos])
				&& (local_open_basedir_pos >= 0)) {
			local_open_basedir[local_open_basedir_pos--] = 0;
		}
	} else {
		/* Else use the unmodified path */
		strlcpy(local_open_basedir, basedir, sizeof(local_open_basedir));
	}

	/* Resolve the real path into resolved_name */
	if ((php_realpath(path, resolved_name) != NULL) && (php_realpath(local_open_basedir, resolved_basedir) != NULL)) {
		/* Check the path */
#ifdef PHP_WIN32
		if (strncasecmp(resolved_basedir, resolved_name, strlen(resolved_basedir)) == 0) {
#else
		if (strncmp(resolved_basedir, resolved_name, strlen(resolved_basedir)) == 0) {
#endif
			/* File is in the right directory */
			return 0;
		} else {
			return -1;
		}
	} else {
		/* Unable to resolve the real path, return -1 */
		return -1;
	}
}

PHPAPI int php_check_open_basedir(char *path)
{
	PLS_FETCH();

	/* Only check when open_basedir is available */
	if (PG(open_basedir) && *PG(open_basedir)) {
		char *pathbuf;
		char *ptr;
		char *end;

		pathbuf = estrdup(PG(open_basedir));

		ptr = pathbuf;

		while (ptr && *ptr) {
#ifdef PHP_WIN32
			end = strchr(ptr, ';');
#else
			end = strchr(ptr, ':');
#endif
			if (end != NULL) {
				*end = '\0';
				end++;
			}

			if (php_check_specific_open_basedir(ptr, path PLS_CC) == 0) {
				efree(pathbuf);
				return 0;
			}

			ptr = end;
		}
		php_error(E_WARNING, "open_basedir restriction in effect. File is in wrong directory.");
		efree(pathbuf);
		return -1;
	}

	/* Nothing to check... */
	return 0;
}

PHPAPI FILE *php_fopen_wrapper(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	int cm=2;  /* checkuid mode: 2 = if file does not exist, check directory */
	PLS_FETCH();

	if (opened_path) {
		*opened_path = NULL;
	}

	/* FIXME  Lets not get in the habit of doing stuff like this.  This should
	   be runtime enabled, NOT compile time. */
#if PHP_URL_FOPEN
	if (!(options & IGNORE_URL)) {
		return php_fopen_url_wrapper(path, mode, options, issock, socketd, opened_path);
	}
#endif

	if (options & USE_PATH && PG(include_path) != NULL) {
		return php_fopen_with_path(path, mode, PG(include_path), opened_path);
	} else {
		FILE *fp;

		if (!strcmp(mode,"r") || !strcmp(mode,"r+")) {
			cm=0;
		}
		if (options & ENFORCE_SAFE_MODE && PG(safe_mode) && (!php_checkuid(path, cm))) {
			return NULL;
		}
		if (php_check_open_basedir(path)) {
			return NULL;
		}
		fp = V_FOPEN(path, mode);
		if (fp && opened_path) {
			*opened_path = expand_filepath(path);
		}
		return fp;
	}
}


PHPAPI FILE *php_fopen_primary_script(void)
{
	FILE *fp;
	struct stat st;
	char *temp, *path_info, *filename;
	int length;
	PLS_FETCH();
	SLS_FETCH();

	filename = SG(request_info).path_translated;
	path_info = SG(request_info).request_uri;
#if HAVE_PWD_H
	if (PG(user_dir) && *PG(user_dir)
		&& path_info && '/' == path_info[0] && '~' == path_info[1]) {

		char user[32];
		struct passwd *pw;
		char *s = strchr(path_info + 2, '/');

		filename = NULL;	/* discard the original filename, it must not be used */
		if (s) {			/* if there is no path name after the file, do not bother */
							/* to try open the directory */
			length = s - (path_info + 2);
			if (length > sizeof(user) - 1)
				length = sizeof(user) - 1;
			memcpy(user, path_info + 2, length);
			user[length] = '\0';

			pw = getpwnam(user);
			if (pw && pw->pw_dir) {
				filename = emalloc(strlen(PG(user_dir)) + strlen(path_info) + strlen(pw->pw_dir) + 4);
				if (filename) {
					sprintf(filename, "%s%c%s%c%s", pw->pw_dir, DEFAULT_SLASH,
								PG(user_dir), DEFAULT_SLASH, s+1); /* Safe */
					STR_FREE(SG(request_info).path_translated);
					SG(request_info).path_translated = filename;
				}
			}
		}
	} else
#endif
#ifdef PHP_WIN32
	if (PG(doc_root) && path_info && (IS_SLASH(*PG(doc_root))
		|| strstr(PG(doc_root),":\\") || strstr(PG(doc_root),":/"))) {
#else
	if (PG(doc_root) && path_info && IS_SLASH(*PG(doc_root))) {
#endif
		length = strlen(PG(doc_root));
		filename = emalloc(length + strlen(path_info) + 2);
		if (filename) {
			memcpy(filename, PG(doc_root), length);
			if (!IS_SLASH(filename[length - 1]))	/* length is never 0 */
				filename[length++] = DEFAULT_SLASH;
			if (IS_SLASH(path_info[0]))
				length--;
			strcpy(filename + length, path_info);
			STR_FREE(SG(request_info).path_translated);
			SG(request_info).path_translated = filename;
		}
	}							/* if doc_root && path_info */
	if (!filename) {
		/* we have to free SG(request_info).path_translated here because
		   php_destroy_request_info assumes that it will get
		   freed when the include_names hash is emptied, but
		   we're not adding it in this case */
		STR_FREE(SG(request_info).path_translated);
		SG(request_info).path_translated = NULL;
		return NULL;
	}
	fp = V_FOPEN(filename, "r");

	/* refuse to open anything that is not a regular file */
	if (fp && (0 > fstat(fileno(fp), &st) || !S_ISREG(st.st_mode))) {
		fclose(fp);
		fp = NULL;
	}
	if (!fp) {
		php_error(E_ERROR, "Unable to open %s", filename);
		STR_FREE(SG(request_info).path_translated);	/* for same reason as above */
		return NULL;
	}
	
	temp = estrdup(filename);
	php_dirname(temp, strlen(temp));
	if (*temp) {
		V_CHDIR(temp);
	}
	efree(temp);
	SG(request_info).path_translated = filename;

	return fp;
}


/*
 * Tries to open a file with a PATH-style list of directories.
 * If the filename starts with "." or "/", the path is ignored.
 */
PHPAPI FILE *php_fopen_with_path(char *filename, char *mode, char *path, char **opened_path)
{
	char *pathbuf, *ptr, *end;
	char trypath[MAXPATHLEN + 1];
	struct stat sb;
	FILE *fp;
	int cm=2;
	PLS_FETCH();

	if (opened_path) {
		*opened_path = NULL;
	}

	if(!strcmp(mode,"r") || !strcmp(mode,"r+")) cm=0;
	/* Relative path open */
	if (*filename == '.') {
		if (PG(safe_mode) && (!php_checkuid(filename, cm))) {
			return NULL;
		}
		if (php_check_open_basedir(filename)) return NULL;
		fp = V_FOPEN(filename, mode);
		if (fp && opened_path) {
			*opened_path = expand_filepath(filename);
		}
		return fp;
	}
	/* Absolute path open - prepend document_root in safe mode */
#ifdef PHP_WIN32
	if (IS_SLASH(*filename) || (filename[1] == ':')) {
#else
	if (IS_SLASH(*filename) {
#endif
		if (PG(safe_mode)) {
			if(PG(doc_root)) {
				snprintf(trypath, MAXPATHLEN, "%s%s", PG(doc_root), filename);
			} else {
				strlcpy(trypath,filename,sizeof(trypath));
			}
			if (!php_checkuid(trypath, cm)) {
				return NULL;
			}
			if (php_check_open_basedir(trypath)) return NULL;
			fp = V_FOPEN(trypath, mode);
			if (fp && opened_path) {
				*opened_path = expand_filepath(trypath);
			}
			return fp;
		} else {
			if (php_check_open_basedir(filename)) {
				return NULL;
			}
			fp = V_FOPEN(filename, mode);
			if (fp && opened_path) {
				*opened_path = expand_filepath(filename);
			}
			return fp;
		}
	}
	if (!path || (path && !*path)) {
		if (PG(safe_mode) && (!php_checkuid(filename, cm))) {
			return NULL;
		}
		if (php_check_open_basedir(filename)) {
			return NULL;
		}
		fp = V_FOPEN(filename, mode);
		if (fp && opened_path) {
			*opened_path = strdup(filename);
		}
		return fp;
	}
	pathbuf = estrdup(path);

	ptr = pathbuf;

	while (ptr && *ptr) {
#ifdef PHP_WIN32
		end = strchr(ptr, ';');
#else
		end = strchr(ptr, ':');
#endif
		if (end != NULL) {
			*end = '\0';
			end++;
		}
		snprintf(trypath, MAXPATHLEN, "%s/%s", ptr, filename);
		if (PG(safe_mode)) {
			if (V_STAT(trypath, &sb) == 0 && (!php_checkuid(trypath, cm))) {
				efree(pathbuf);
				return NULL;
			}
		}
		if ((fp = V_FOPEN(trypath, mode)) != NULL) {
			if (php_check_open_basedir(trypath)) {
				fclose(fp);
				efree(pathbuf);
				return NULL;
			}
			if (opened_path) {
				*opened_path = expand_filepath(trypath);
			}
			efree(pathbuf);
			return fp;
		}
		ptr = end;
	}
	efree(pathbuf);
	return NULL;
}

/*
 * If the specified path starts with "http://" (insensitive to case),
 * a socket is opened to the specified web server and a file pointer is
 * position at the start of the body of the response (past any headers).
 * This makes a HTTP/1.0 request, and knows how to pass on the username
 * and password for basic authentication.
 *
 * If the specified path starts with "ftp://" (insensitive to case),
 * a pair of sockets are used to request the specified file and a file
 * pointer to the requested file is returned. Passive mode ftp is used,
 * so if the server doesn't support this, it will fail!
 *
 * Otherwise, fopen is called as usual and the file pointer is returned.
 */


static FILE *php_fopen_url_wrap_http(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	FILE *fp=NULL;
	url *resource=NULL;
	struct sockaddr_in server;
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
	
	*socketd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socketd == SOCK_ERR) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
	server.sin_family = AF_INET;
	
	if (lookup_hostname(resource->host, &server.sin_addr)) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
	server.sin_port = htons(resource->port);
	
	if (connect(*socketd, (struct sockaddr *) &server, sizeof(server)) == SOCK_CONN_ERR) {
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
		if (location[0] != '\0') {
			return php_fopen_url_wrapper(location, mode, options, issock, socketd, opened_path);
		} else {
			return NULL;
		}
	}
	free_url(resource);
	*issock = 1;
	return (fp);
}
 
 static FILE *php_fopen_url_wrap_ftp(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	FILE *fp=NULL;
	url *resource=NULL;
	struct sockaddr_in server;
	char tmp_line[512];
	unsigned short portno;
	char *scratch;
	int result;
	int i;
	char *tpath, *ttpath;
	
	resource = url_parse((char *) path);
	if (resource == NULL) {
		php_error(E_WARNING, "Invalid URL specified, %s", path);
		*issock = BAD_URL;
		return NULL;
	} else if (resource->path == NULL) {
		php_error(E_WARNING, "No file-path specified");
		free_url(resource);
		*issock = BAD_URL;
		return NULL;
	}
	/* use port 21 if one wasn't specified */
	if (resource->port == 0)
		resource->port = 21;
	
	*socketd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socketd == SOCK_ERR) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
	server.sin_family = AF_INET;
	
	if (lookup_hostname(resource->host, &server.sin_addr)) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
	server.sin_port = htons(resource->port);
	
	if (connect(*socketd, (struct sockaddr *) &server, sizeof(server)) == SOCK_CONN_ERR) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
#if 0
	if ((fpc = fdopen(*socketd, "r+")) == NULL) {
		free_url(resource);
		return NULL;
	}
#ifdef HAVE_SETVBUF
	if ((setvbuf(fpc, NULL, _IONBF, 0)) != 0) {
		free_url(resource);
		fclose(fpc);
		return NULL;
	}
#endif
#endif
	
	/* Start talking to ftp server */
	result = php_get_ftp_result(*socketd);
	if (result > 299 || result < 200) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
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
		if (result > 299 || result < 200) {
			free_url(resource);
			SOCK_FCLOSE(*socketd);
			*socketd = 0;
			return NULL;
		}
	} else if (result > 299 || result < 200) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	
	/* set the connection to be binary */
	SOCK_WRITE("TYPE I\r\n", *socketd);
	result = php_get_ftp_result(*socketd);
	if (result > 299 || result < 200) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	
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
			free_url(resource);
			SOCK_FCLOSE(*socketd);
			*socketd = 0;
			errno = ENOENT;
			return NULL;
		}
	} else {
		/* when writing file, it must NOT exist */
		if (result <= 299 && result >= 200) {
			php_error(E_WARNING, "File already exists");
			free_url(resource);
			SOCK_FCLOSE(*socketd);
			*socketd = 0;
			errno = EEXIST;
			return NULL;
		}
	}
	
	/* set up the passive connection */
	SOCK_WRITE("PASV\r\n", *socketd);
	while (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, *socketd) &&
		   !(isdigit((int) tmp_line[0]) && isdigit((int) tmp_line[1]) &&
			 isdigit((int) tmp_line[2]) && tmp_line[3] == ' '));
	
	/* make sure we got a 227 response */
	if (strncmp(tmp_line, "227", 3)) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	/* parse pasv command (129,80,95,25,13,221) */
	tpath = tmp_line;
	
	/* skip over the "227 Some message " part */
	for (tpath += 4; *tpath && !isdigit((int) *tpath); tpath++);
	if (!*tpath) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	/* skip over the host ip, we just assume it's the same */
	for (i = 0; i < 4; i++) {
		for (; isdigit((int) *tpath); tpath++);
		if (*tpath == ',') {
			tpath++;
		} else {
			SOCK_FCLOSE(*socketd);
			*socketd = 0;
			return NULL;
		}
	}
	
	/* pull out the MSB of the port */
	portno = (unsigned short) strtol(tpath, &ttpath, 10) * 256;
	if (ttpath == NULL) {
		/* didn't get correct response from PASV */
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	tpath = ttpath;
	if (*tpath == ',') {
		tpath++;
	} else {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	
	/* pull out the LSB of the port */
	portno += (unsigned short) strtol(tpath, &ttpath, 10);
	
	if (ttpath == NULL) {
		/* didn't get correct response from PASV */
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
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
	*socketd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socketd == SOCK_ERR) {
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		free_url(resource);
		return NULL;
	}
	server.sin_family = AF_INET;
	
	if (lookup_hostname(resource->host, &server.sin_addr)) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
	server.sin_port = htons(portno);
	
	if (connect(*socketd, (struct sockaddr *) &server, sizeof(server)) == SOCK_CONN_ERR) {
		free_url(resource);
		SOCK_FCLOSE(*socketd);
		*socketd = 0;
		return NULL;
	}
#if 0
	if (mode[0] == 'r') {
		if ((fp = fdopen(*socketd, "r+")) == NULL) {
			free_url(resource);
			return NULL;
		}
	} else {
		if ((fp = fdopen(*socketd, "w+")) == NULL) {
			free_url(resource);
			return NULL;
		}
	}
#ifdef HAVE_SETVBUF
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0) {
		free_url(resource);
		fclose(fp);
		return NULL;
	}
#endif
#endif
	free_url(resource);
	*issock = 1;
	return (fp);
}

static FILE *php_fopen_url_wrap_php(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	const char *res = path + 6;

	*issock = 0;
	
	if (!strcasecmp(res, "stdin")) {
		return fdopen(STDIN_FILENO, mode);
	} else if (!strcasecmp(res, "stdout")) {
		return fdopen(STDOUT_FILENO, mode);
	} else if (!strcasecmp(res, "stderr")) {
		return fdopen(STDERR_FILENO, mode);
	}
	
	return NULL;
}

static FILE *php_fopen_url_wrapper(const char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	FILE *fp = NULL;
	const char *p;
	const char *protocol=NULL;
	int n=0;

	for(p=path;isalnum((int)*p);p++) 
		n++;
	if((*p==':')&&(n>1)) {
		protocol=path;
	} 
		
	if(protocol) {
		php_fopen_url_wrapper_t *wrapper=NULL;
		char *protocopy = emalloc(n+1);

		if(protocopy) {
			strncpy(protocopy,protocol,n);
			protocopy[n]='\0';
			if(FAILURE==zend_hash_find(&fopen_url_wrappers_hash, protocopy, n+1,(void **)&wrapper)) {
				wrapper=NULL;
			}		
			efree(protocopy);
		}
		if(wrapper)
			return (*wrapper)(path, mode, options, issock, socketd, opened_path);	
	} 

	if( !protocol || !strncasecmp(protocol, "file",n)){
		PLS_FETCH();
		
		*issock = 0;
		
		if(protocol) {
			if(path[n+1]=='/') {
				if(path[n+2]=='/') { 
					php_error(E_WARNING, "remote host file access not supported, %s", path);
					return NULL;
				}
			}
			path+= n+1; 			
		}		

		if (options & USE_PATH) {
			fp = php_fopen_with_path((char *) path, mode, PG(include_path), opened_path);
		} else {
			int cm=2;
			if(!strcmp(mode,"r") || !strcmp(mode,"r+")) cm=0;
			if (options & ENFORCE_SAFE_MODE && PG(safe_mode) && (!php_checkuid(path, cm))) {
				fp = NULL;
			} else {
				if (php_check_open_basedir((char *) path)) {
					fp = NULL;
				} else {
					fp = V_FOPEN(path, mode);
				}
			}
		}


		return (fp);
	}
			
	php_error(E_WARNING, "Invalid URL specified, %s", path);
	return NULL;
}

int php_get_ftp_result(int socketd)
{
	char tmp_line[513];

	while (SOCK_FGETS(tmp_line, sizeof(tmp_line)-1, socketd) &&
		   !(isdigit((int) tmp_line[0]) && isdigit((int) tmp_line[1]) &&
			 isdigit((int) tmp_line[2]) && tmp_line[3] == ' '));

	return strtol(tmp_line, NULL, 10);
}

PHPAPI int php_is_url(char *path)
{
	return (!strncasecmp(path, "http://", 7) || !strncasecmp(path, "ftp://", 6));
}


PHPAPI char *php_strip_url_passwd(char *url)
{
	register char *p = url, *url_start;
	
	while (*p) {
		if (*p==':' && *(p+1)=='/' && *(p+2)=='/') {
			/* found protocol */
			url_start = p = p+3;
			
			while (*p) {
				if (*p=='@') {
					int i;
					
					for (i=0; i<3 && url_start<p; i++, url_start++) {
						*url_start = '.';
					}
					for (; *p; p++) {
						*url_start++ = *p;
					}
					*url_start=0;
					break;
				}
				p++;
			}
			return url;
		}
		p++;
	}
	return url;
}


#if 1

PHPAPI char *expand_filepath(char *filepath)
{
	cwd_state new_state;
	char cwd[MAXPATHLEN+1];
	char *result;

	result = V_GETCWD(cwd, MAXPATHLEN);	
	if (!result) {
		cwd[0] = '\0';
	}

	new_state.cwd = strdup(cwd);
	new_state.cwd_length = strlen(cwd);

	virtual_file_ex(&new_state, filepath, NULL);
	return new_state.cwd;
}

#else

PHPAPI char *expand_filepath(char *filepath)
{
	char *retval = NULL;

	if (filepath[0] == '.') {
		char *cwd = malloc(MAXPATHLEN + 1);

		if (V_GETCWD(cwd, MAXPATHLEN)) {
			char *cwd_end = cwd + strlen(cwd);

			if (filepath[1] == '.') {	/* parent directory - .. */
				/* erase the last directory name from the path */
#ifdef PHP_WIN32
				while (*cwd_end != '/' || *cwd_end != '\\') {
#else
				while (*cwd_end != '/') {
#endif
					*cwd_end-- = 0;
				}
				filepath++;		/* make filepath appear as a current directory path */
			}
#ifdef PHP_WIN32
			if (cwd_end > cwd && (*cwd_end == '/' || *cwd_end == '\\')) { /* remove trailing slashes */
#else
			if (cwd_end > cwd && *cwd_end == '/') {		/* remove trailing slashes */
#endif
				*cwd_end-- = 0;
			}
			retval = (char *) malloc(strlen(cwd) + strlen(filepath) - 1 + 1);
			strcpy(retval, cwd);
			strcat(retval, filepath + 1);
			free(cwd);
		}
	}
	if (!retval) {
		retval = strdup(filepath);
	}
	return retval;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
