/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Hartmut Holzgraefe <hartmut@six.de>                         |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#define IS_EXT_MODULE
#define _GNU_SOURCE

#include "php.h"
#include "php_zlib.h"
#include "fopen_wrappers.h"

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

static int gz_seeker(void *cookie,off_t position, int whence) {
	return gzseek(((struct gz_cookie *)cookie)->gz_file,(z_off_t)position,whence); 
}

static int gz_closer(void *cookie) {
	int ret=gzclose(((struct gz_cookie *)cookie)->gz_file);
	free(cookie);
	cookie=NULL;  
	return ret;
}



static COOKIE_IO_FUNCTIONS_T gz_cookie_functions =   
{ gz_reader 
, gz_writer
, gz_seeker
, gz_closer
};

FILE *zlib_fopen_wrapper(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path TSRMLS_DC)
{
	struct gz_cookie *gc = NULL;
	FILE *fp;
    int fissock=0, fsocketd=0;

	gc = (struct gz_cookie *)malloc(sizeof(struct gz_cookie));

	if(gc) {
		*issock = 0;
		
		while(*path!=':') 
			path++;
		
		path++;

		fp = php_fopen_wrapper(path, mode, options|IGNORE_URL, &fissock, &fsocketd, NULL TSRMLS_CC);
		
		if (!fp) {
			free(gc);
			return NULL;
		}
		
		gc->gz_file = gzdopen(fileno(fp), mode);
                
		if(gc->gz_file) {
			return fopencookie(gc,mode,gz_cookie_functions);		
		} else {
		    free(gc);
			return NULL;
		}
	}
	errno = ENOENT;
	return NULL;
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
