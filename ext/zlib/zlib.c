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
   |          Stefan Röhrich <sr@linux.de>                                |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#define IS_EXT_MODULE

#include "php_config.h"

#if HAVE_FOPENCOOKIE 
#define _GNU_SOURCE
#define __USE_GNU
#include "libio.h"
#endif 

#include "php.h"


 
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
/* #include <sys/uio.h> */
#endif
#include "ext/standard/head.h"
#include "safe_mode.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_zlib.h"
#include "fopen-wrappers.h"
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if HAVE_ZLIB
#if defined(HAVE_UNISTD_H) && defined(PHP_WIN32)
#undef HAVE_UNISTD_H
#endif

#include <zlib.h>

#ifdef COMPILE_DL_ZLIB
#ifndef PUTS
#define PUTS(a) php_printf("%s",a)
#endif
#ifndef PUTC
#define PUTC(a) PUTS(a)
#endif
#ifndef PHPWRITE
#define PHPWRITE(a,n) php_write((a),(n))
#endif
#endif

#ifdef ZTS
int zlib_globals_id;
#else
static php_zlib_globals zlib_globals;
#endif

#if HAVE_FOPENCOOKIE 
static FILE *zlib_fopen_wrapper(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path);
#endif 

/* True globals, no need for thread safety */
static int le_zp;

function_entry php_zlib_functions[] = {
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
	PHP_FE(gzcompress,                  NULL)
	PHP_FE(gzuncompress,                NULL)
	{NULL, NULL, NULL}
};

zend_module_entry php_zlib_module_entry = {
	"zlib",
	php_zlib_functions,
	PHP_MINIT(zlib),
	PHP_MSHUTDOWN(zlib),
	NULL,
	NULL,
	PHP_MINFO(zlib),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ZLIB
ZEND_GET_MODULE(php_zlib)
#endif

static void phpi_destructor_gzclose(zend_rsrc_list_entry *rsrc)
{
	gzFile *zp = (gzFile *)rsrc->ptr;
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
	PLS_FETCH();

#ifdef ZTS
        zlib_globals_id = ts_allocate_id(sizeof(php_zlib_globals), (ts_allocate_ctor) php_zlib_init_globals, NULL);
#else
        ZLIBG(gzgetss_state)=0;
#endif
	le_zp = register_list_destructors(phpi_destructor_gzclose,NULL,"zlib");

#if HAVE_FOPENCOOKIE

	if(PG(allow_url_fopen)) {
		php_register_url_wrapper("zlib",zlib_fopen_wrapper);
	}
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(zlib)
{
#if HAVE_FOPENCOOKIE
	PLS_FETCH();

	if(PG(allow_url_fopen)) {
	    php_unregister_url_wrapper("zlib"); 
    }
#endif
	
	return SUCCESS;
}

PHP_MINFO_FUNCTION(zlib)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "ZLib Support", "enabled");
#if HAVE_FOPENCOOKIE
	php_info_print_table_row(2, "'zlib:' fopen wrapper", "enabled");
#endif
	php_info_print_table_row(2, "Compiled Version", ZLIB_VERSION );
	php_info_print_table_row(2, "Linked Version", (char *)zlibVersion() );
	php_info_print_table_end();
}

static gzFile php_gzopen_wrapper(char *path, char *mode, int options)
{
	FILE *f;
	int issock=0, socketd=0;

	f = php_fopen_wrapper(path, mode, options, &issock, &socketd, NULL);

	if (!f) {
		return NULL;
	}
	return gzdopen(fileno(f), mode);
}

/* {{{ proto array gzfile(string filename [, int use_include_path])
   Read und uncompress entire .gz-file into an array */
PHP_FUNCTION(gzfile) {
	pval **filename, **arg2;
	gzFile zp;
	char *slashed, buf[8192];
	register int i=0;
	int use_include_path = 0;
	PLS_FETCH();

	/* check args */
	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1,&filename) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2,&filename,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg2);
		use_include_path = (*arg2)->value.lval?USE_PATH:0;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	zp = php_gzopen_wrapper((*filename)->value.str.val,"r", use_include_path|ENFORCE_SAFE_MODE);
	if (!zp) {
		php_error(E_WARNING,"gzFile(\"%s\") - %s",(*filename)->value.str.val,strerror(errno));
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
	pval **arg1, **arg2, **arg3;
	gzFile *zp;
	char *p;
	int use_include_path = 0;
	ZLIBLS_FETCH();
	
	switch(ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2,&arg1,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3,&arg1,&arg2,&arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg3);
		use_include_path = (*arg3)->value.lval?USE_PATH:0;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	p = estrndup((*arg2)->value.str.val,(*arg2)->value.str.len);

	/*
	 * We need a better way of returning error messages from
	 * php_gzopen_wrapper().
	 */
	zp = php_gzopen_wrapper((*arg1)->value.str.val, p, use_include_path|ENFORCE_SAFE_MODE);
	if (!zp) {
		php_error(E_WARNING,"gzopen(\"%s\",\"%s\") - %s",
					(*arg1)->value.str.val, p, strerror(errno));
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
	pval **arg1;
	gzFile *zp;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);
	zend_list_delete((*arg1)->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int gzeof(int zp)
   Test for end-of-file on a .gz-file pointer */
PHP_FUNCTION(gzeof) {
	pval **arg1;
	gzFile *zp;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
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
	pval **arg1, **arg2;
	gzFile *zp;
	int len;
	char *buf;
	PLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg2);
	len = (*arg2)->value.lval;

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
	pval **arg1;
	gzFile *zp;
	int c;
	char *buf;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
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
/* {{{ proto string gzgetss(int zp, int length [, string allowable_tags])
   Get a line from file pointer and strip HTML tags */
PHP_FUNCTION(gzgetss)
{
	pval **fd, **bytes, **allow=NULL;
	gzFile *zp;
	int len;
	char *buf;
	char *allowed_tags=NULL;
	int allowed_tags_len=0;
	ZLIBLS_FETCH();
	
	switch(ZEND_NUM_ARGS()) {
		case 2:
			if(zend_get_parameters_ex(2, &fd, &bytes) == FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &fd, &bytes, &allow) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(allow);
			allowed_tags = (*allow)->value.str.val;
			allowed_tags_len = (*allow)->value.str.len;
			break;
		default:
			WRONG_PARAM_COUNT;
			/* NOTREACHED */
			break;
	}

	convert_to_long_ex(bytes);

	len = (*bytes)->value.lval;

	ZEND_FETCH_RESOURCE(zp, gzFile *, fd, -1, "Zlib file", le_zp);

	buf = emalloc(sizeof(char) * (len + 1));
	/*needed because recv doesnt set null char at end*/
	memset(buf,0,len+1);
	if (!(gzgets(zp, buf, len) != NULL)) {
		efree(buf);
		RETURN_FALSE;
	}

	/* strlen() can be used here since we are doing it on the return of an fgets() anyway */
	php_strip_tags(buf, strlen(buf), ZLIBG(gzgetss_state), allowed_tags, allowed_tags_len);
	RETURN_STRING(buf, 0);
	
}
/* }}} */

/* {{{ proto int gzwrite(int zp, string str [, int length])
   Binary-safe .gz-file write */
PHP_FUNCTION(gzwrite) {
	pval **arg1, **arg2, **arg3=NULL;
	gzFile *zp;
	int ret;
	int num_bytes;
	PLS_FETCH();

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &arg1, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(arg2);
			num_bytes = (*arg2)->value.str.len;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(arg2);
			convert_to_long_ex(arg3);
			num_bytes = MIN((*arg3)->value.lval, (*arg2)->value.str.len);
			break;
		default:
			WRONG_PARAM_COUNT;
			/* NOTREACHED */
			break;
	}				
	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	/* strip slashes only if the length wasn't specified explicitly */
	if (!arg3 && PG(magic_quotes_runtime)) {
		php_stripslashes((*arg2)->value.str.val,&num_bytes);
	}

	ret = gzwrite(zp, (*arg2)->value.str.val,num_bytes);
	RETURN_LONG(ret);
}	
/* }}} */

/* {{{ proto int gzputs(int zp, string str [, int length])
   An alias for gzwrite */
/* }}} */

/* {{{ proto int gzrewind(int zp)
   Rewind the position of a .gz-file pointer */
PHP_FUNCTION(gzrewind) {
	pval **arg1;
	gzFile *zp;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
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
	pval **arg1;
	long pos;
	gzFile *zp;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
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
	pval **arg1, **arg2;
	int ret;
	gzFile *zp;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg2);

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

 	ret = gzseek(zp, (*arg2)->value.lval, SEEK_SET);
	RETURN_LONG(ret);
}
/* }}} */

/*
 * Read a file and write the ouput to stdout
 */
/* {{{ proto int readgzfile(string filename [, int use_include_path])
   Output a .gz-file */
PHP_FUNCTION(readgzfile) {
	pval **arg1, **arg2;
	char buf[8192];
	gzFile *zp;
	int b, size;
	int use_include_path = 0;

	
	/* check args */
	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1,&arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2,&arg1,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg2);
		use_include_path = (*arg2)->value.lval?USE_PATH:0;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	/*
	 * We need a better way of returning error messages from
	 * php_gzopen_wrapper().
	 */
	zp = php_gzopen_wrapper((*arg1)->value.str.val,"r", use_include_path|ENFORCE_SAFE_MODE);
	if (!zp){
		php_error(E_WARNING,"ReadGzFile(\"%s\") - %s",(*arg1)->value.str.val,strerror(errno));
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
	pval **arg1;
	gzFile *zp;
	char buf[8192];
	int size, b;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(zp, gzFile *, arg1, -1, "Zlib file", le_zp);

	size = 0;
	while((b = gzread(zp, buf, sizeof(buf))) > 0) {
		PHPWRITE(buf,b);
		size += b ;
	}
/*  gzclose(zp); */
	zend_list_delete((*arg1)->value.lval);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto string gzread(int zp, int length)
   Binary-safe file read */
PHP_FUNCTION(gzread)
{
	pval **arg1, **arg2;
	gzFile *zp;
	int len;
	PLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg2);
	len = (*arg2)->value.lval;

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
	

/* {{{ proto string gzcompress(string data [, int level]) 
   Gzip-compress a string */
PHP_FUNCTION(gzcompress)
{
	zval **data, **zlimit = NULL;
	int limit,status;
	unsigned long l2;
	char *s2;

	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &data) == FAILURE)
			WRONG_PARAM_COUNT;
		limit=-1;
		break;
	case 2:
		if (zend_get_parameters_ex(2, &data, &zlimit) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(zlimit);
		limit = (*zlimit)->value.lval;
		if((limit<0)||(limit>9)) {
			php_error(E_WARNING,"gzcompress: compression level must be whithin 0..9");
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;                                         
	}
	convert_to_string_ex(data);
	
	l2 = (*data)->value.str.len + ((*data)->value.str.len/1000) + 15;
	s2 = (char *) emalloc(l2);
	if(! s2) RETURN_FALSE;
	
	if(limit>=0) {
		status = compress2(s2,&l2,(*data)->value.str.val, (*data)->value.str.len,limit);
	} else {
		status = compress(s2,&l2,(*data)->value.str.val, (*data)->value.str.len);
	}
	
	if(status==Z_OK) {
		RETURN_STRINGL(s2, l2, 0);
	} else {
		efree(s2);
		php_error(E_WARNING,"gzcompress: %s",zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string gzuncompress(string data, int length) 
   Unzip a gzip-compressed string */
PHP_FUNCTION(gzuncompress)
{
	zval **data, **zlimit = NULL;
	int status,factor=1,maxfactor=8;
	unsigned long plength=0,length;
	char *s1=NULL,*s2=NULL;

	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &data) == FAILURE)
			WRONG_PARAM_COUNT;
		length=0;
		break;
	case 2:
		if (zend_get_parameters_ex(2, &data, &zlimit) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(zlimit);
		if((*zlimit)->value.lval<=0) {
			php_error(E_WARNING,"gzuncompress: length must be greater zero");
			RETURN_FALSE;
		}
		plength = (*zlimit)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;                                         
	}
	convert_to_string_ex(data);

	/*
	 zlib::uncompress() wants to know the output data length
	 if none was given as a parameter
	 we try from input length * 2 up to input length * 2^8
	 doubling it whenever it wasn't big enough
	 that should be eneugh for all real life cases	
	*/
	do {
		length=plength?plength:(*data)->value.str.len*(1<<factor++);
		s2 = (char *) erealloc(s1,length);
		if(! s2) { if(s1) efree(s1); RETURN_FALSE; }
		status = uncompress(s2, &length ,(*data)->value.str.val, (*data)->value.str.len);
		s1=s2;
	} while((status==Z_BUF_ERROR)&&(!plength)&&(factor<maxfactor));

	if(status==Z_OK) {
		s2 = erealloc(s2, length);
		RETURN_STRINGL(s2, length, 0);
	} else {
		efree(s2);
		php_error(E_WARNING,"gzuncompress: %s",zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

#if HAVE_FOPENCOOKIE
struct gz_cookie {
	gzFile gz_file;
};

static ssize_t gz_reader(void *cookie, char *buffer, size_t size)
{
	return gzread(((struct gz_cookie *)cookie)->gz_file,buffer,size); 
}

static ssize_t gz_writer(void *cookie, const char *buffer, size_t size) {
	return gzwrite(((struct gz_cookie *)cookie)->gz_file,(char *)buffer,size); 
}

static int gz_seeker(void *cookie,fpos_t position, int whence) {
	return gzseek(((struct gz_cookie *)cookie)->gz_file,position,whence); 
}

static int gz_closer(void *cookie) {
	gzclose(((struct gz_cookie *)cookie)->gz_file);
	efree(cookie);
	cookie=NULL;  
}


static cookie_io_functions_t gz_cookie_functions =   
{ gz_reader 
, gz_writer
, gz_seeker
, gz_closer
};

static FILE *zlib_fopen_wrapper(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	struct gz_cookie *gc = NULL;
	FILE *fp;
    int fissock=0, fsocketd=0;

	gc = (struct gz_cookie *)emalloc(sizeof(struct gz_cookie));

	if(gc) {
		*issock = 0;
		
		while(*path!=':') 
			path++;
		
		path++;

		fp = php_fopen_wrapper(path, mode, options|IGNORE_URL, &fissock, &fsocketd, NULL);
		
		if (!fp) {
			efree(gc);
			return NULL;
		}
		
		gc->gz_file = gzdopen(fileno(fp), mode);
                
		if(gc->gz_file) {
			return fopencookie(gc,mode,gz_cookie_functions);		
		} else {
			efree(gc);
			return NULL;
		}
	}
	errno = ENOENT;
	return NULL;
}


#endif



#endif /* HAVE_ZLIB */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
