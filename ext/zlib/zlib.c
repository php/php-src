/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stefan Röhrich <sr@linux.de>                                |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#define IS_EXT_MODULE

#include "php.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if (WIN32|WINNT)
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
/* #include <sys/uio.h> */
#endif
#include "ext/standard/head.h"
#include "safe_mode.h"
#include "ext/standard/php3_standard.h"
#include "php3_zlib.h"
#include "fopen-wrappers.h"
#if HAVE_PWD_H
#if WIN32|WINNT
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#include "snprintf.h"
#if HAVE_ZLIB
#if defined(HAVE_UNISTD_H) && (WIN32|WINNT)
#undef HAVE_UNISTD_H
#endif

#include <zlib.h>

#if COMPILE_DL
#include "dl/phpdl.h"
#ifndef PUTS
#define PUTS(a) php_printf("%s",a)
#endif
#ifndef PUTC
#define PUTC(a) PUTS(a)
#endif
#ifndef PHPWRITE
#define PHPWRITE(a,n) php3_write((a),(n))
#endif
#endif

#ifdef ZTS
int zlib_globals_id;
#else
static php_zlib_globals zlib_globals;
#endif


/* True globals, no need for thread safety */
static int le_zp;

function_entry php3_zlib_functions[] = {
	PHP_FE(readgzfile,					NULL)
	PHP_FE(gzrewind,					NULL)
	PHP_FE(gzclose,						NULL)
	PHP_FE(gzeof,						NULL)
	PHP_FE(gzgetc,						NULL)
	PHP_FE(gzgets,						NULL)
	PHP_FE(gzgetss,						NULL)
	PHP_FE(gzread,						NULL)
	PHP_FE(gzopen,						NULL)
	PHP_FE(gzpassthru,					NULL)
	PHP_FE(gzseek,						NULL)
	PHP_FE(gztell,						NULL)
	PHP_FE(gzwrite,						NULL)
	PHP_FALIAS(gzputs,		gzwrite,	NULL)
	PHP_FE(gzfile,						NULL)
	{NULL, NULL, NULL}
};

php3_module_entry php3_zlib_module_entry = {
	"zlib",
	php3_zlib_functions,
	PHP_MINIT(zlib),
	PHP_MSHUTDOWN(zlib),
	NULL,
	NULL,
	PHP_MINFO(zlib),
	STANDARD_MODULE_PROPERTIES
};

#if defined(COMPILE_DL)
DLEXPORT php3_module_entry *get_module(void) { return &php_zlib_module_entry; }
#endif

static void phpi_destructor_gzclose(gzFile *zp) {
	(void)gzclose(zp);
}

#ifdef ZTS
static void php_zlib_init_globals(ZLIBLS_D)
{
        ZLIBG(gzgetss_state) = 0;
}
#endif

PHP_MINIT_FUNCTION(zlib)
{
#ifdef ZTS
        zlib_globals_id = ts_allocate_id(sizeof(php_zlib_globals), php_zlib_init_globals, NULL);
#else
        ZLIBG(gzgetss_state)=0;
#endif
	le_zp = register_list_destructors(phpi_destructor_gzclose,NULL);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(zlib)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(zlib)
{
		PUTS("Zlib support active (compiled with ");
		PUTS(ZLIB_VERSION);
		PUTS(", linked with ");
		PUTS((char *)zlibVersion());
		PUTS(").");
}

static gzFile *php3_gzopen_with_path(char *filename, char *mode, char *path, char **opened_path);

static gzFile php3_gzopen_wrapper(char *path, char *mode, int options)
{
	PLS_FETCH();
	
	if (options & USE_PATH && PG(include_path) != NULL) {
		return php3_gzopen_with_path(path, mode, PG(include_path), NULL);
	}
	else {
		if (options & ENFORCE_SAFE_MODE && PG(safe_mode) && (!_php3_checkuid(path,1))) {
			return NULL;
		}
		if (_php3_check_open_basedir(path)) return NULL;
		return gzopen(path, mode);
	}
}

/*
 * Tries to open a .gz-file with a PATH-style list of directories.
 * If the filename starts with "." or "/", the path is ignored.
 */
static gzFile *php3_gzopen_with_path(char *filename, char *mode, char *path, char **opened_path)
{
	char *pathbuf, *ptr, *end;
	char trypath[MAXPATHLEN + 1];
	struct stat sb;
	gzFile *zp;
	PLS_FETCH();
	
	if (opened_path) {
		*opened_path = NULL;
	}
	
	/* Relative path open */
	if (*filename == '.') {
		if (PG(safe_mode) &&(!_php3_checkuid(filename,2))) {
			return(NULL);
		}
		if (_php3_check_open_basedir(filename)) return NULL;
		zp = gzopen(filename, mode);
		if (zp && opened_path) {
			*opened_path = expand_filepath(filename);
		}
		return zp;
	}

	/* Absolute path open - prepend document_root in safe mode */
#if WIN32|WINNT
	if ((*filename == '\\')||(*filename == '/')||(filename[1] == ':')) {
#else
	if (*filename == '/') {
#endif
		if (PG(safe_mode)) {
			if(PG(doc_root)) {
				snprintf(trypath, MAXPATHLEN, "%s%s", PG(doc_root), filename);
			} else {
				strlcpy(trypath,filename,sizeof(trypath));
			}
			if (!_php3_checkuid(trypath,2)) {
				return(NULL);
			}
			if (_php3_check_open_basedir(trypath)) return NULL;
			zp = gzopen(trypath, mode);
			if (zp && opened_path) {
				*opened_path = expand_filepath(trypath);
			}
			return zp;
		} else {
			if (_php3_check_open_basedir(filename)) return NULL;
			return gzopen(filename, mode);
		}
	}

	if (!path || (path && !*path)) {
		if (PG(safe_mode) &&(!_php3_checkuid(filename,2))) {
			return(NULL);
		}
		if (_php3_check_open_basedir(filename)) return NULL;
		zp = gzopen(filename, mode);
		if (zp && opened_path) {
			*opened_path = strdup(filename);
		}
		return zp;
	}

	pathbuf = estrdup(path);

	ptr = pathbuf;

	while (ptr && *ptr) {
#if WIN32|WINNT
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
			if (stat(trypath,&sb) == 0 &&(!_php3_checkuid(trypath,2))) {
				efree(pathbuf);
				return(NULL);
			}
		}
		if ((zp = gzopen(trypath, mode)) != NULL) {
			if (_php3_check_open_basedir(trypath)) {
				gzclose(zp);
				efree(pathbuf);
				return NULL;
			}
			if (opened_path) {
				*opened_path = expand_filepath(trypath);
			}
			efree(pathbuf);
			return zp;
		}
		ptr = end;
	}
	efree(pathbuf);
	return NULL;
}

/* {{{ proto array gzfile(string filename [, int use_include_path])
Read und uncompress entire .gz-file into an array */
PHP_FUNCTION(gzfile) {
	pval *filename, *arg2;
	gzFile zp;
	char *slashed, buf[8192];
	register int i=0;
	int use_include_path = 0;
	PLS_FETCH();

	/* check args */
	switch (ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht,1,&filename) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht,2,&filename,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg2);
		use_include_path = arg2->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	zp = php3_gzopen_wrapper(filename->value.str.val,"r", use_include_path|ENFORCE_SAFE_MODE);
	if (!zp) {
		php_error(E_WARNING,"gzFile(\"%s\") - %s",filename->value.str.val,strerror(errno));
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* Now loop through the file and do the magic quotes thing if needed */
	memset(buf,0,8191);
	while(gzgets(zp, buf, 8191) != NULL) {
		if (PG(magic_quotes_runtime)) {
			int len;
			
			slashed = php_addslashes(buf,0,&len,0); /* 0 = don't free source string */
            add_index_stringl(return_value, i++, slashed, len, 0);
		} else {
			add_index_string(return_value, i++, buf, 1);
		}
	}
	gzclose(zp);
}
/* }}} */

/* {{{ proto int gzopen(string filename, string mode [, int use_include_path])
Open a .gz-file and return a .gz-file pointer */
PHP_FUNCTION(gzopen) {
	pval *arg1, *arg2, *arg3;
	gzFile *zp;
	char *p;
	int use_include_path = 0;
	ZLIBLS_FETCH();
	
	switch(ARG_COUNT(ht)) {
	case 2:
		if (getParameters(ht,2,&arg1,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (getParameters(ht,3,&arg1,&arg2,&arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg3);
		use_include_path = arg3->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	convert_to_string(arg2);
	p = estrndup(arg2->value.str.val,arg2->value.str.len);

	/*
	 * We need a better way of returning error messages from
	 * php3_gzopen_wrapper().
	 */
	zp = php3_gzopen_wrapper(arg1->value.str.val, p, use_include_path|ENFORCE_SAFE_MODE);
	if (!zp) {
		php_error(E_WARNING,"gzopen(\"%s\",\"%s\") - %s",
					arg1->value.str.val, p, strerror(errno));
		efree(p);
		RETURN_FALSE;
	}
	ZLIBG(gzgetss_state)=0;
	efree(p);
	ZEND_REGISTER_RESOURCE(return_value, zp, le_zp);
}	
/* }}} */

/* {{{ proto int gzclose(int zp)
Close an open .gz-file pointer */
PHP_FUNCTION(gzclose) {
	pval *arg1;
	gzFile *zp;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);
	zend_list_delete(arg1->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int gzeof(int zp)
Test for end-of-file on a .gz-file pointer */
PHP_FUNCTION(gzeof) {
	pval *arg1;
	gzFile *zp;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	if ((gzeof(zp))) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string gzgets(int zp, int length)
Get a line from .gz-file pointer */
PHP_FUNCTION(gzgets) {
	pval *arg1, *arg2;
	gzFile *zp;
	int len;
	char *buf;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg2);
	len = arg2->value.lval;

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	buf = emalloc(sizeof(char) * (len + 1));
	/* needed because recv doesnt put a null at the end*/
	memset(buf,0,len+1);
	if (!(gzgets(zp, buf, len) != NULL)) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		if (PG(magic_quotes_runtime)) {
			return_value->value.str.val = php_addslashes(buf,0,&return_value->value.str.len,1);
		} else {
			return_value->value.str.val = buf;
			return_value->value.str.len = strlen(return_value->value.str.val);
		}
		return_value->type = IS_STRING;
	}
	return;
}
/* }}} */

/* {{{ proto string gzgetc(int zp)
Get a character from .gz-file pointer */
PHP_FUNCTION(gzgetc) {
	pval *arg1;
	gzFile *zp;
	int c;
	char *buf;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	buf = emalloc(sizeof(char) * 2);
	if ((c=gzgetc(zp)) == (-1)) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		buf[0]=(char)c;
		buf[1]='\0';
		return_value->value.str.val = buf; 
		return_value->value.str.len = 1; 
		return_value->type = IS_STRING;
	}
	return;
}
/* }}} */

/* Strip any HTML tags while reading */
/* {{{ proto string gzgetss(int zp, int length)
Get a line from file pointer and strip HTML tags */
PHP_FUNCTION(gzgetss)
{
	pval *fd, *bytes;
	gzFile *zp;
	int len;
	char *buf;
	ZLIBLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &fd, &bytes) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(bytes);

	len = bytes->value.lval;

	ZEND_FETCH_RESOURCE(zp, gzFile *, fd, -1, "Zlib file", le_zp);

	buf = emalloc(sizeof(char) * (len + 1));
	/*needed because recv doesnt set null char at end*/
	memset(buf,0,len+1);
	if (!(gzgets(zp, buf, len) != NULL)) {
		efree(buf);
		RETURN_FALSE;
	}

	_php3_strip_tags(buf, ZLIBG(gzgetss_state));
	RETURN_STRING(buf, 0);
	
}
/* }}} */

/* {{{ proto int gzwrite(int zp, string str [, int length])
Binary-safe .gz-file write */
PHP_FUNCTION(gzwrite) {
	pval *arg1, *arg2, *arg3=NULL;
	gzFile *zp;
	int ret;
	int num_bytes;
	PLS_FETCH();

	switch (ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &arg1, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(arg2);
			num_bytes = arg2->value.str.len;
			break;
		case 3:
			if (getParameters(ht, 3, &arg1, &arg2, &arg3)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(arg2);
			convert_to_long(arg3);
			num_bytes = MIN(arg3->value.lval, arg2->value.str.len);
			break;
		default:
			WRONG_PARAM_COUNT;
			/* NOTREACHED */
			break;
	}				
	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	/* strip slashes only if the length wasn't specified explicitly */
	if (!arg3 && PG(magic_quotes_runtime)) {
		php_stripslashes(arg2->value.str.val,&num_bytes);
	}

	ret = gzwrite(zp, arg2->value.str.val,num_bytes);
	RETURN_LONG(ret);
}	
/* }}} */

/* {{{ proto int gzputs(int zp, string str [, int length])
   An alias for gzwrite */
/* }}} */

/* {{{ proto int gzrewind(int zp)
Rewind the position of a .gz-file pointer */
PHP_FUNCTION(gzrewind) {
	pval *arg1;
	gzFile *zp;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	gzrewind(zp);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int gztell(int zp)
Get .gz-file pointer's read/write position */
PHP_FUNCTION(gztell) {
	pval *arg1;
	long pos;
	gzFile *zp;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	pos = gztell(zp);
	RETURN_LONG(pos);
}
/* }}} */

/* {{{ proto int gzseek(int zp, int offset)
Seek on a file pointer */
PHP_FUNCTION(gzseek) {
	pval *arg1, *arg2;
	int ret;
	long pos;
	gzFile *zp;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg2);
	pos = arg2->value.lval;

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

 	ret = gzseek(zp,pos,SEEK_SET);
	RETURN_LONG(ret);
}
/* }}} */

/*
 * Read a file and write the ouput to stdout
 */
/* {{{ proto int readgzfile(string filename [, int use_include_path])
Output a .gz-file */
PHP_FUNCTION(readgzfile) {
	pval *arg1, *arg2;
	char buf[8192];
	gzFile *zp;
	int b, size;
	int use_include_path = 0;

	
	/* check args */
	switch (ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht,1,&arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht,2,&arg1,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg2);
		use_include_path = arg2->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);	

	/*
	 * We need a better way of returning error messages from
	 * php3_gzopen_wrapper().
	 */
	zp = php3_gzopen_wrapper(arg1->value.str.val,"r", use_include_path|ENFORCE_SAFE_MODE);
	if (!zp){
		php_error(E_WARNING,"ReadGzFile(\"%s\") - %s",arg1->value.str.val,strerror(errno));
		RETURN_FALSE;
	}
	size= 0;
	while((b = gzread(zp, buf, sizeof(buf))) > 0) {
		PHPWRITE(buf,b);
		size += b ;
	}
   	gzclose(zp);
	RETURN_LONG(size);
}
/* }}} */

/*
 * Read to EOF on a file descriptor and write the output to stdout.
 */
/* {{{ proto int gzpassthru(int zp)
Output all remaining data from a .gz-file pointer */
PHP_FUNCTION(gzpassthru) {
	pval *arg1;
	gzFile *zp;
	char buf[8192];
	int size, b;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	size = 0;
	while((b = gzread(zp, buf, sizeof(buf))) > 0) {
		PHPWRITE(buf,b);
		size += b ;
	}
/*  gzclose(zp); */
	zend_list_delete(arg1->value.lval);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto string gzread(int zp, int length)
Binary-safe file read */
PHP_FUNCTION(gzread)
{
	pval *arg1, *arg2;
	gzFile *zp;
	int len;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg2);
	len = arg2->value.lval;

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	return_value->value.str.val = emalloc(sizeof(char) * (len + 1));
	/* needed because recv doesnt put a null at the end*/
	
	return_value->value.str.len = gzread(zp, return_value->value.str.val, len);
	return_value->value.str.val[return_value->value.str.len] = 0;

	if (PG(magic_quotes_runtime)) {
		return_value->value.str.val = php_addslashes(return_value->value.str.val,return_value->value.str.len,&return_value->value.str.len,1);
	}
	return_value->type = IS_STRING;
}
/* }}} */

#endif /* HAVE_ZLIB */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
