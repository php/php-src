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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@fast.no>                                   |
   |          Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   | PHP 4.0 patches by Thies C. Arntzen (thies@thieso.net)               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Synced with php 3.0 revision 1.218 1999-06-16 [ssb] */

/* {{{ includes */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/flock_compat.h"
#include "ext/standard/exec.h"
#include "ext/standard/php_filestat.h"
#include "php_open_temporary_file.h"

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
#include "win32/winutil.h"
#else
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif
#include "ext/standard/head.h"
#include "safe_mode.h"
#include "php_string.h"
#include "file.h"
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include "fsock.h"
#include "fopen_wrappers.h"
#include "php_globals.h"

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#if MISSING_FCLOSE_DECL
extern int fclose(FILE *);
#endif

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

#include "scanf.h"
#include "zend_API.h"

#ifdef ZTS
int file_globals_id;
#else
php_file_globals file_globals;
#endif

/* }}} */
/* {{{ ZTS-stuff / Globals / Prototypes */

/* sharing globals is *evil* */
static int le_fopen, le_popen, le_socket; 


/* }}} */
/* {{{ Module-Stuff */

static void _file_popen_dtor(zend_rsrc_list_entry *rsrc)
{
	FILE *pipe = (FILE *)rsrc->ptr;
	FLS_FETCH();

	FG(pclose_ret) = pclose(pipe);
}


static void _file_socket_dtor(zend_rsrc_list_entry *rsrc) 
{
	int *sock = (int *)rsrc->ptr;
	SOCK_FCLOSE(*sock);
#if HAVE_SHUTDOWN
	shutdown(*sock, 0);
#endif
	efree(sock);
}


static void _file_fopen_dtor(zend_rsrc_list_entry *rsrc) 
{
	FILE *fp = (FILE *)rsrc->ptr;
	fclose(fp);
}


PHPAPI int php_file_le_fopen(void) /* XXX doe we really want this???? */
{
	return le_fopen;
}

PHPAPI int php_file_le_popen(void) /* XXX you may not like this, but I need it. -- KK */
{
	return le_popen;
}


PHPAPI int php_file_le_socket(void) /* XXX doe we really want this???? */
{
	return le_socket;
}


static void file_globals_ctor(FLS_D)
{
	zend_hash_init(&FG(ht_fsock_keys), 0, NULL, NULL, 1);
	zend_hash_init(&FG(ht_fsock_socks), 0, NULL, (void (*)(void *))php_msock_destroy, 1);
	FG(def_chunk_size) = PHP_FSOCK_CHUNK_SIZE;
	FG(phpsockbuf) = NULL;
	FG(fgetss_state) = 0;
	FG(pclose_ret) = 0;
}


static void file_globals_dtor(FLS_D)
{
	zend_hash_destroy(&FG(ht_fsock_socks));
	zend_hash_destroy(&FG(ht_fsock_keys));
	php_cleanup_sockbuf(1 FLS_CC);
}


PHP_MINIT_FUNCTION(file)
{
	le_fopen = zend_register_list_destructors_ex(_file_fopen_dtor, NULL, "file", module_number);
	le_popen = zend_register_list_destructors_ex(_file_popen_dtor, NULL, "pipe", module_number);
	le_socket = zend_register_list_destructors_ex(_file_socket_dtor, NULL, "socket", module_number);

#ifdef ZTS
	file_globals_id = ts_allocate_id(sizeof(php_file_globals), (ts_allocate_ctor) file_globals_ctor, (ts_allocate_dtor) file_globals_dtor);
#else
	file_globals_ctor(FLS_C);
#endif

	REGISTER_LONG_CONSTANT("SEEK_SET", SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SEEK_CUR", SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SEEK_END", SEEK_END, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_SH", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_EX", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_UN", 3, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_NB", 4, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

/* }}} */

PHP_MSHUTDOWN_FUNCTION(file)
{
#ifndef ZTS
	FLS_FETCH();

	file_globals_dtor(FLS_C);
#endif
	return SUCCESS;
}



/* {{{ proto bool flock(int fp, int operation [, int wouldblock])
   Portable file locking */

static int flock_values[] = { LOCK_SH, LOCK_EX, LOCK_UN };

PHP_FUNCTION(flock)
{
    pval **arg1, **arg2, **arg3;
    int type, fd, act, ret, arg_count = ARG_COUNT(ht);
	void *what;
	
    if (arg_count > 3 || zend_get_parameters_ex(arg_count, &arg1, &arg2, &arg3) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	
	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);
	
	if (type == le_socket) {
		fd = *(int *) what;
	} else {
		fd = fileno((FILE*) what);
	}
	
    convert_to_long_ex(arg2);

    act = (*arg2)->value.lval & 3;
    if (act < 1 || act > 3) {
		php_error(E_WARNING, "Illegal operation argument");
		RETURN_FALSE;
    }

    /* flock_values contains all possible actions
       if (arg2 & 4) we won't block on the lock */
    act = flock_values[act - 1] | ((*arg2)->value.lval & 4 ? LOCK_NB : 0);
    if ((ret=flock(fd, act)) == -1) {
        RETURN_FALSE;
    }
	if(ret==-1 && errno==EWOULDBLOCK && arg_count==3) {
		(*arg3)->type = IS_LONG;
		(*arg3)->value.lval=1;
	}
    RETURN_TRUE;
}

/* }}} */

#define PHP_META_UNSAFE ".\\+*?[^]$() "

/* {{{ proto array get_meta_tags(string filename [, int use_include_path])
   Extracts all meta tag content attributes from a file and returns an array */

PHP_FUNCTION(get_meta_tags)
{
	pval **filename, **arg2;
	FILE *fp;
	int use_include_path = 0;
	int issock=0, socketd=0;
	int in_tag=0, in_meta_tag=0, looking_for_val=0, done=0, ulc=0;
	int num_parts=0, lc=0;
	int token_len=0;
	char *token_data=NULL, *name=NULL, *value=NULL, *temp=NULL;
	php_meta_tags_token tok, tok_last;
	PLS_FETCH();
	
	/* check args */
	switch (ARG_COUNT(ht)) {
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
		use_include_path = (*arg2)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	
	fp = php_fopen_wrapper((*filename)->value.str.val,"rb", use_include_path|ENFORCE_SAFE_MODE, &issock, &socketd, NULL);
	if (!fp && !socketd) {
		if (issock != BAD_URL) {
			char *tmp = estrndup(Z_STRVAL_PP(filename), Z_STRLEN_PP(filename));
			php_strip_url_passwd(tmp);
			php_error(E_WARNING,"get_meta_tags(\"%s\") - %s", tmp, strerror(errno));
			efree(tmp);
		}
		RETURN_FALSE;
	}
	
	if (array_init(return_value)==FAILURE) {
		if (issock) {
			SOCK_FCLOSE(socketd);
		} else {
			fclose(fp);
		}
		RETURN_FALSE;
	}

	tok_last = TOK_EOF;

	while (!done && (tok = php_next_meta_token(fp,socketd,issock,&ulc,&lc,&token_data,&token_len)) != TOK_EOF) {
		if (tok == TOK_ID) {
			if (tok_last == TOK_OPENTAG) {
				in_meta_tag = !strcasecmp("meta",token_data);
			} else if (tok_last == TOK_SLASH && in_tag) {
				if (strcasecmp("head",token_data) == 0) {
					/* We are done here! */
					done = 1;
				}
			} else if (tok_last == TOK_EQUAL && looking_for_val) {

				if (!num_parts) {
					/* This is a single word attribute */
					temp = name = estrndup(token_data,token_len);

					while (temp && *temp) {
						if (strchr(PHP_META_UNSAFE, *temp)) {
							*temp = '_';
						}
						temp++;
					}
					num_parts++;
				} else {
					if (PG(magic_quotes_runtime)) {
						value = php_addslashes(token_data,0,&token_len,0);
					} else {
						value = estrndup(token_data,token_len);
					}

					/* Insert the value into the array */
					add_assoc_string(return_value, name, value, 0);
					num_parts = 0;
				}
				looking_for_val = 0;
			} else {
				if (in_meta_tag) {
					if (strcasecmp("name",token_data) == 0 || strcasecmp("content",token_data) == 0) {
						looking_for_val = 1;
					} else {
						looking_for_val = 0;
					}
				}
			}
		} else if (tok == TOK_STRING && tok_last == TOK_EQUAL && looking_for_val) {
			if (!num_parts) {
				/* First, get the name value and store it */
				temp = name = estrndup(token_data,token_len);
				while (temp && *temp) {
					if (strchr(PHP_META_UNSAFE, *temp)) {
						*temp = '_';
					}
					temp++;
				}
				num_parts++;
			} else {
				/* Then get the value value and store it, quoting if neccessary */
				if (PG(magic_quotes_runtime)) {
					value = php_addslashes(token_data,0,&token_len,0);
				} else {
					value = estrndup(token_data,token_len);
				}

				/* Insert the value into the array */
				add_assoc_string(return_value, name, value, 0);
				num_parts = 0;
			}
			looking_for_val = 0;
		} else if (tok == TOK_OPENTAG) {
			if (looking_for_val) {
				looking_for_val = 0;
			}
			in_tag = 1;
		} else if (tok == TOK_CLOSETAG) {
			/* We never made it to the value, free the name */
			if (num_parts) {
				efree(name);
			}
			/* Reset all of our flags */
			in_tag = in_meta_tag = looking_for_val = num_parts = 0;
		}

		tok_last = tok;

		if (token_data)
			efree(token_data);

		token_data = NULL;
	}

    if (issock) {
        SOCK_FCLOSE(socketd);
    } else {
        fclose(fp);
    }
}

/* }}} */
/* {{{ proto array file(string filename [, int use_include_path])
   Read entire file into an array */

#define PHP_FILE_BUF_SIZE	80

PHP_FUNCTION(file)
{
	pval **filename, **arg2;
	FILE *fp;
	char *slashed, *target_buf;
	register int i=0;
	int use_include_path = 0;
	int issock=0, socketd=0;
	int target_len, len;
	zend_bool reached_eof=0;
	PLS_FETCH();
	
	/* check args */
	switch (ARG_COUNT(ht)) {
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
		use_include_path = (*arg2)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	
	fp = php_fopen_wrapper((*filename)->value.str.val,"rb", use_include_path|ENFORCE_SAFE_MODE, &issock, &socketd, NULL);
	if (!fp && !socketd) {
		if (issock != BAD_URL) {
			char *tmp = estrndup(Z_STRVAL_PP(filename), Z_STRLEN_PP(filename));
			php_strip_url_passwd(tmp);
			php_error(E_WARNING,"file(\"%s\") - %s", tmp, strerror(errno));
			efree(tmp);
		}
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}	
	
	/* Now loop through the file and do the magic quotes thing if needed */
	target_len = 0;
	target_buf = NULL;
	while (1) {
		if (!target_buf) {
			target_buf = (char *) emalloc(PHP_FILE_BUF_SIZE+1);
			target_buf[PHP_FILE_BUF_SIZE] = 0; /* avoid overflows */
		} else {
			target_buf = (char *) erealloc(target_buf, target_len+PHP_FILE_BUF_SIZE+1);
			target_buf[target_len+PHP_FILE_BUF_SIZE] = 0; /* avoid overflows */
		}
		if (FP_FGETS(target_buf+target_len, PHP_FILE_BUF_SIZE, socketd, fp, issock)==NULL) {
			if (target_len==0) {
				efree(target_buf);
				break;
			} else {
				reached_eof = 1;
			}
		}
		if (!reached_eof) {
			target_len += strlen(target_buf+target_len);
			if (target_buf[target_len-1]!='\n') {
				continue;
			}
		}
		if (PG(magic_quotes_runtime)) {
			slashed = php_addslashes(target_buf, target_len, &len, 1); /* 1 = free source string */
            add_index_stringl(return_value, i++, slashed, len, 0);
		} else {
			target_buf = erealloc(target_buf, target_len+1); /* do we really want to do that? */
			add_index_stringl(return_value, i++, target_buf, target_len, 0);
		}
		if (reached_eof) {
			break;
		}
		target_buf = NULL;
		target_len = 0;
	}
	if (issock) {
		SOCK_FCLOSE(socketd);
	} else {
		fclose(fp);
	}
}


/* }}} */
/* {{{ proto string tempnam(string dir, string prefix)
   Create a unique filename in a directory */

PHP_FUNCTION(tempnam)
{
	pval **arg1, **arg2;
	char *d;
	char *opened_path;
	char p[64];
	FILE *fp;
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	d = estrndup(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1));
	strlcpy(p, Z_STRVAL_PP(arg2), sizeof(p));

	if ((fp = php_open_temporary_file(d, p, &opened_path))) {
		fclose(fp);
		RETVAL_STRING(opened_path, 0);
	} else {
		RETVAL_FALSE;
	}
	efree(d);
}

/* }}} */
/* {{{ proto int tmpfile(void)
   Create a temporary file that will be deleted automatically after use */
PHP_NAMED_FUNCTION(php_if_tmpfile)
{
	FILE *fp;
	if (ARG_COUNT(ht) != 0) {
		WRONG_PARAM_COUNT;
	}
	fp = tmpfile();
	if (fp == NULL) {
		php_error(E_WARNING, "tmpfile: %s", strerror(errno));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, fp, le_fopen);
}
/* }}} */

/* {{{ proto int fopen(string filename, string mode [, int use_include_path])
   Open a file or a URL and return a file pointer */

PHP_NAMED_FUNCTION(php_if_fopen)
{
	pval **arg1, **arg2, **arg3;
	FILE *fp;
	char *p;
	int *sock;
	int use_include_path = 0;
	int issock=0, socketd=0;
	FLS_FETCH();
	
	switch(ARG_COUNT(ht)) {
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
		use_include_path = (*arg3)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	p = estrndup((*arg2)->value.str.val,(*arg2)->value.str.len);

	/*
	 * We need a better way of returning error messages from
	 * php_fopen_wrapper().
	 */
	fp = php_fopen_wrapper((*arg1)->value.str.val, p, use_include_path|ENFORCE_SAFE_MODE, &issock, &socketd, NULL);
	if (!fp && !socketd) {
		if (issock != BAD_URL) {
			char *tmp = estrndup(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1));
			php_strip_url_passwd(tmp);
			php_error(E_WARNING,"fopen(\"%s\",\"%s\") - %s", tmp, p, strerror(errno));
			efree(tmp);
		}
		efree(p);
		RETURN_FALSE;
	}

	efree(p);
	FG(fgetss_state)=0;

	if (issock) {
		sock=emalloc(sizeof(int));
		*sock=socketd;
		ZEND_REGISTER_RESOURCE(return_value,sock,le_socket);
	} else {
		ZEND_REGISTER_RESOURCE(return_value,fp,le_fopen);
	}
}

/* }}} */	
/* {{{ proto int fclose(int fp)
   Close an open file pointer */

PHP_FUNCTION(fclose)
{
	pval **arg1;
	int type;
	void *what;
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,2,le_fopen,le_socket);
	ZEND_VERIFY_RESOURCE(what);
	
	zend_list_delete((*arg1)->value.lval);
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int popen(string command, string mode)
   Execute a command and open either a read or a write pipe to it */

PHP_FUNCTION(popen)
{
	pval **arg1, **arg2;
	FILE *fp;
	char *p,*tmp = NULL;
	char *b, buf[1024];
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	p = estrndup((*arg2)->value.str.val,(*arg2)->value.str.len);
	if (PG(safe_mode)){
		b = strchr((*arg1)->value.str.val,' ');
		if (!b) {
			b = strrchr((*arg1)->value.str.val,'/');
		} else {
			char *c;
			c = (*arg1)->value.str.val;
			while((*b!='/')&&(b!=c)) {
				b--;
			}
			if (b==c) {
				b=NULL;
			}
		}
		if (b) {
			snprintf(buf,sizeof(buf),"%s%s",PG(safe_mode_exec_dir),b);
		} else {
			snprintf(buf,sizeof(buf),"%s/%s",PG(safe_mode_exec_dir),(*arg1)->value.str.val);
		}

		tmp = php_escape_shell_cmd(buf);
		fp = V_POPEN(tmp,p);
		efree(tmp);

		if (!fp) {
			php_error(E_WARNING,"popen(\"%s\",\"%s\") - %s",buf,p,strerror(errno));
			RETURN_FALSE;
		}
	} else {
		fp = V_POPEN((*arg1)->value.str.val, p);
		if (!fp) {
			php_error(E_WARNING,"popen(\"%s\",\"%s\") - %s",(*arg1)->value.str.val,p,strerror(errno));
			efree(p);
			RETURN_FALSE;
		}
	}
	efree(p);

	ZEND_REGISTER_RESOURCE(return_value,fp,le_popen);
}

/* }}} */
/* {{{ proto int pclose(int fp)
   Close a file pointer opened by popen() */

PHP_FUNCTION(pclose)
{
	pval **arg1;
	void *what;
	FLS_FETCH();
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",NULL,1,le_popen);
	ZEND_VERIFY_RESOURCE(what);
	
	zend_list_delete((*arg1)->value.lval);
	RETURN_LONG(FG(pclose_ret));
}

/* }}} */
/* {{{ proto int feof(int fp)
   Test for end-of-file on a file pointer */

PHP_FUNCTION(feof)
{
	pval **arg1;
	int type;
	int issock=0;
	int socketd=0;
	void *what;
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int *) what;
	} 

	if (FP_FEOF(socketd, (FILE*)what, issock)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int set_socket_blocking(int socket_descriptor, int mode)
   Set blocking/non-blocking mode on a socket */
PHPAPI int php_set_sock_blocking(int socketd, int block)
{
      int ret = SUCCESS;
      int flags;
      int myflag = 0;
      
#ifdef PHP_WIN32
      /* with ioctlsocket, a non-zero sets nonblocking, a zero sets blocking */
	  flags = !block;
	  if (ioctlsocket(socketd,FIONBIO,&flags)==SOCKET_ERROR){
		  php_error(E_WARNING,"%s",WSAGetLastError());
		  ret = FALSE;
      }
#else
      flags = fcntl(socketd, F_GETFL);
#ifdef O_NONBLOCK
      myflag = O_NONBLOCK; /* POSIX version */
#elif defined(O_NDELAY)
      myflag = O_NDELAY;   /* old non-POSIX version */
#endif
      if (!block) {
              flags |= myflag;
      } else {
              flags &= ~myflag;
      }
      fcntl(socketd, F_SETFL, flags);
#endif
      return ret;
}

PHP_FUNCTION(socket_set_blocking)
{
	pval **arg1, **arg2;
	int block;
	int socketd = 0;
	void *what;
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",NULL,1,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	convert_to_long_ex(arg2);
	block = (*arg2)->value.lval;
	
	socketd = *(int*)what;

	if (php_set_sock_blocking(socketd, block) == FAILURE)
		RETURN_FALSE;

	php_sockset_blocking(socketd, block == 0 ? 0 : 1);
	
	RETURN_TRUE;
}

/* }}} */

PHP_FUNCTION(set_socket_blocking)
{
	php_error(E_NOTICE, "set_socket_blocking() is deprecated, use socket_set_blocking() instead");
	PHP_FN(socket_set_blocking)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* {{{ proto bool socket_set_timeout(int socket_descriptor, int seconds, int microseconds)
   Set timeout on socket read to seconds + microseonds */
PHP_FUNCTION(socket_set_timeout)
{
#if HAVE_SYS_TIME_H
	zval **socket, **seconds, **microseconds;
	int type;
	void *what;
	int socketd = 0;
	struct timeval t;

	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &socket, &seconds, &microseconds)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(socket, -1, "File-Handle", &type, 1, le_socket);
	ZEND_VERIFY_RESOURCE(what);
	socketd = *(int *)what;
	
	convert_to_long_ex(seconds);
	t.tv_sec = (*seconds)->value.lval;

	if (ZEND_NUM_ARGS() == 3) {
		convert_to_long_ex(microseconds);
		t.tv_usec = (*microseconds)->value.lval % 1000000;
		t.tv_sec += (*microseconds)->value.lval / 1000000;
	}
	else
		t.tv_usec = 0;

	php_sockset_timeout(socketd, &t);
	RETURN_TRUE;
#endif /* HAVE_SYS_TIME_H */
}

/* }}} */


/* {{{ proto array socket_get_status(resource socket_descriptor)
   Return an array describing socket status */
PHP_FUNCTION(socket_get_status)
{
	zval **socket;
	int type;
	void *what;
	int socketd = 0;
	struct php_sockbuf *sock;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &socket) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(socket, -1, "File-Handle", &type, 1, le_socket);
	ZEND_VERIFY_RESOURCE(what);
	socketd = *(int *)what;
	sock = php_get_socket(socketd);

	array_init(return_value);

	add_assoc_bool(return_value, "timed_out", sock->timeout_event);
	add_assoc_bool(return_value, "blocked", sock->is_blocked);
	add_assoc_bool(return_value, "eof", sock->eof);
	add_assoc_long(return_value, "unread_bytes", sock->writepos - sock->readpos);
}
/* }}} */


/* {{{ proto string fgets(int fp, int length)
   Get a line from file pointer */

PHP_FUNCTION(fgets)
{
	pval **arg1, **arg2;
	int len, type;
	char *buf;
	int issock=0;
	int socketd=0;
	void *what;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	convert_to_long_ex(arg2);
	len = (*arg2)->value.lval;

	if (len < 0) {
		php_error(E_WARNING, "length parameter to fgets() may not be negative");
		RETURN_FALSE;
	}

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	buf = emalloc(sizeof(char) * (len + 1));
	/* needed because recv doesnt put a null at the end*/
	memset(buf,0,len+1);
#ifdef HAVE_FLUSHIO
	if (type == le_fopen) {
		fseek((FILE*)what, 0, SEEK_CUR);
	}
#endif
	if (FP_FGETS(buf, len, socketd, (FILE*)what, issock) == NULL) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		if (PG(magic_quotes_runtime)) {
			return_value->value.str.val = php_addslashes(buf,0,&return_value->value.str.len,1);
		} else {
			return_value->value.str.val = buf;
			return_value->value.str.len = strlen(return_value->value.str.val);
			/* resize buffer if it's much larger than the result */
			if(return_value->value.str.len < len/2) {
				return_value->value.str.val = erealloc(buf,return_value->value.str.len+1);
			}
		}
		return_value->type = IS_STRING;
	}
}

/* }}} */
/* {{{ proto string fgetc(int fp)
   Get a character from file pointer */

PHP_FUNCTION(fgetc) {
	pval **arg1;
	int type;
	char *buf;
	int issock=0;
	int socketd=0;
	void *what;
	int result;
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

#ifdef HAVE_FLUSHIO
	if (type == le_fopen) {
		fseek((FILE*)what, 0, SEEK_CUR);
	}
#endif
	buf = emalloc(sizeof(int));
	if ((result = FP_FGETC(socketd, (FILE*)what, issock)) == EOF) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		buf[0]=result;
		buf[1]='\0';
		return_value->value.str.val = buf; 
		return_value->value.str.len = 1; 
		return_value->type = IS_STRING;
	}
}

/* }}} */

/* {{{ proto string fgetss(int fp, int length [, string allowable_tags])
   Get a line from file pointer and strip HTML tags */

PHP_FUNCTION(fgetss)
{
	pval **fd, **bytes, **allow=NULL;
	int len, type;
	char *buf;
	int issock=0;
	int socketd=0;
	void *what;
	char *allowed_tags=NULL;
	int allowed_tags_len=0;
	FLS_FETCH();

	switch(ARG_COUNT(ht)) {
	case 2:
		if (zend_get_parameters_ex(2, &fd, &bytes) == FAILURE) {
			RETURN_FALSE;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &fd, &bytes, &allow) == FAILURE) {
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

	what = zend_fetch_resource(fd,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	convert_to_long_ex(bytes);
	len = (*bytes)->value.lval;
    if (len < 0) {
		php_error(E_WARNING, "length parameter to fgetss() may not be negative");
		RETURN_FALSE;
    }

	buf = emalloc(sizeof(char) * (len + 1));
	/*needed because recv doesnt set null char at end*/
	memset(buf, 0, len + 1);
	if (FP_FGETS(buf, len, socketd, (FILE*)what, issock) == NULL) {
		efree(buf);
		RETURN_FALSE;
	}

	/* strlen() can be used here since we are doing it on the return of an fgets() anyway */
	php_strip_tags(buf, strlen(buf), FG(fgetss_state), allowed_tags, allowed_tags_len);

	RETURN_STRING(buf, 0);
}

/* }}} */
/* {{{ proto mixed fscanf(string str, string format [, string ...])
   Implements a mostly ANSI compatible fscanf() */
PHP_FUNCTION(fscanf)
{
    int  result;
    pval **file_handle, **format_string;
    int len, type;
    char *buf;
    int issock=0;
    int socketd=0;
    void *what;
    
    zval ***args;
    int argCount;   
    
    argCount = ZEND_NUM_ARGS();
    if (argCount < 2) {
        WRONG_PARAM_COUNT;
    }
    args = (zval ***)emalloc(argCount * sizeof(zval **));
    if (!args || (zend_get_parameters_array_ex(argCount,args) == FAILURE)) {
        efree( args );
        WRONG_PARAM_COUNT;
    }
    
    file_handle    = args[0];
    format_string  = args[1];

    what = zend_fetch_resource(file_handle,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);

    /*
     * we can't do a ZEND_VERIFY_RESOURCE(what), otherwise we end up
     * with a leak if we have an invalid filehandle. This needs changing
     * if the code behind ZEND_VERIFY_RESOURCE changed. - cc
     */
    if (!what) {
        efree(args);
        RETURN_FALSE;
    }

    len = SCAN_MAX_FSCANF_BUFSIZE;

    if (type == le_socket) {
        issock=1;
        socketd=*(int*)what;
    }
    buf = emalloc(sizeof(char) * (len + 1));
    /* needed because recv doesnt put a null at the end*/
    memset(buf,0,len+1);
    if (FP_FGETS(buf, len, socketd, (FILE*)what, issock) == NULL) {
        efree(buf);
        RETVAL_FALSE;
    } else {
        convert_to_string_ex( format_string );  
        result = php_sscanf_internal( buf,(*format_string)->value.str.val,
                        argCount,args, 2,&return_value);
        efree(args);
        efree(buf);
        if (SCAN_ERROR_WRONG_PARAM_COUNT == result) {
            WRONG_PARAM_COUNT
        }
    }


}
/* }}} */

/* {{{ proto int fwrite(int fp, string str [, int length])
   Binary-safe file write */

PHP_FUNCTION(fwrite)
{
	pval **arg1, **arg2, **arg3=NULL;
	int ret,type;
	int num_bytes;
	int issock=0;
	int socketd=0;
	void *what;
	PLS_FETCH();

	switch (ARG_COUNT(ht)) {
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

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	if (!arg3 && PG(magic_quotes_runtime)) {
		zval_copy_ctor(*arg2);
		php_stripslashes((*arg2)->value.str.val,&num_bytes);
	}

	if (issock){
		ret = SOCK_WRITEL((*arg2)->value.str.val,num_bytes,socketd);
	} else {
#ifdef HAVE_FLUSHIO
		if (type == le_fopen) {
			fseek((FILE*)what, 0, SEEK_CUR);
		}
#endif
		ret = fwrite((*arg2)->value.str.val,1,num_bytes,(FILE*)what);
	}
	RETURN_LONG(ret);
}

/* }}} */	
/* {{{ proto int fflush(int fp)
   Flushes output */

PHP_FUNCTION(fflush)
{
	pval **arg1;
	int ret,type;
	int issock=0;
	int socketd=0;
	void *what;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	if (issock){
		ret = fsync(socketd);
	} else {
		ret = fflush((FILE*)what);
	}

	if (ret) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */	
/* {{{ proto int set_file_buffer(int fp, int buffer)
   Set file write buffer */

PHP_FUNCTION(set_file_buffer)
{
	pval **arg1, **arg2;
	int ret,type,buff;
	void *what;
      
	switch (ARG_COUNT(ht)) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2)==FAILURE) {
			RETURN_FALSE;
		} 
		break;
	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}                               

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,2,le_fopen,le_popen);
	ZEND_VERIFY_RESOURCE(what);

	convert_to_long_ex(arg2);
	buff = (*arg2)->value.lval;    

	/* if buff is 0 then set to non-buffered */
	if (buff == 0){
		ret = setvbuf((FILE*)what, NULL, _IONBF, 0);
	} else {
		ret = setvbuf((FILE*)what, NULL, _IOFBF, buff);
	}

	RETURN_LONG(ret);
}

/* }}} */     
/* {{{ proto int rewind(int fp)
   Rewind the position of a file pointer */

PHP_FUNCTION(rewind)
{
	pval **arg1;
	void *what;
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	what = zend_fetch_resource(arg1,-1,"File-Handle",NULL,2,le_fopen,le_popen);
	ZEND_VERIFY_RESOURCE(what);
	
	rewind((FILE*) what);
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int ftell(int fp)
   Get file pointer's read/write position */

PHP_FUNCTION(ftell)
{
	pval **arg1;
	void *what;
	long ret;
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	what = zend_fetch_resource(arg1,-1,"File-Handle",NULL,2,le_fopen,le_popen);
	ZEND_VERIFY_RESOURCE(what);

	ret = ftell((FILE*) what);
	if(ret == -1) {
		RETURN_FALSE;
	} 
	
	RETURN_LONG(ret);
}

/* }}} */
/* {{{ proto int fseek(int fp, int offset [, int whence])
   Seek on a file pointer */

PHP_FUNCTION(fseek)
{
	zval **arg1, **arg2, **arg3;
	int argcount = ARG_COUNT(ht), whence = SEEK_SET;
	void *what;
	
	if (argcount < 2 || argcount > 3 ||
	    zend_get_parameters_ex(argcount, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	what = zend_fetch_resource(arg1,-1,"File-Handle",NULL,2,le_fopen,le_popen);
	ZEND_VERIFY_RESOURCE(what);

	convert_to_long_ex(arg2);
	if (argcount > 2) {
		convert_to_long_ex(arg3);
		whence = (*arg3)->value.lval;
	}
	
	RETURN_LONG(fseek((FILE*)what, (*arg2)->value.lval, whence));
}

/* }}} */
/* {{{ proto int mkdir(string pathname, int mode)
   Create a directory */

PHP_FUNCTION(mkdir)
{
	pval **arg1, **arg2;
	int ret;
	mode_t mode;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_long_ex(arg2);
	mode = (mode_t) (*arg2)->value.lval;
	if (PG(safe_mode) &&(!php_checkuid((*arg1)->value.str.val, NULL, CHECKUID_ALLOW_ONLY_DIR))) {
		RETURN_FALSE;
	}
	ret = V_MKDIR((*arg1)->value.str.val, mode);
	if (ret < 0) {
		php_error(E_WARNING,"MkDir failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */	
/* {{{ proto int rmdir(string dirname)
   Remove a directory */

PHP_FUNCTION(rmdir)
{
	pval **arg1;
	int ret;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	if (PG(safe_mode) &&(!php_checkuid((*arg1)->value.str.val, NULL, CHECKUID_ALLOW_FILE_NOT_EXISTS))) {
		RETURN_FALSE;
	}
	ret = V_RMDIR((*arg1)->value.str.val);
	if (ret < 0) {
		php_error(E_WARNING,"RmDir failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */	
/* {{{ php_passthru_fd */

static size_t php_passthru_fd(int socketd, FILE *fp, int issock)
{
	size_t bcount = 0;
	int ready = 0;
	char buf[8192];
	
#ifdef HAVE_MMAP 
	if(!issock) {
		int fd;
		struct stat sbuf;
		off_t off;
		void *p;
		size_t len;

		fd = fileno(fp);
		fstat(fd, &sbuf);
	
		if (sbuf.st_size > sizeof(buf)) {
			off = ftell(fp);
			len = sbuf.st_size - off;
			p = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, off);
			if (p != (void *) MAP_FAILED) {
				PHPWRITE(p, len);
				munmap(p, len);
				bcount += len;
				ready = 1;
			}
		}
	}
#endif

	if(!ready) {
		int b;

		while ((b = FP_FREAD(buf, sizeof(buf), socketd, fp, issock)) > 0) {
			PHPWRITE(buf, b);
			bcount += b;
		}
	}
		
	return bcount;
}

/* }}} */
/* {{{ proto int readfile(string filename [, int use_include_path])
   Output a file or a URL */

PHP_FUNCTION(readfile)
{
	pval **arg1, **arg2;
	FILE *fp;
	int size=0;
	int use_include_path=0;
	int issock=0, socketd=0;
	int rsrc_id;
	
	/* check args */
	switch (ARG_COUNT(ht)) {
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
		use_include_path = (*arg2)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	/*
	 * We need a better way of returning error messages from
	 * php_fopen_wrapper().
	 */
	fp = php_fopen_wrapper((*arg1)->value.str.val,"rb", use_include_path|ENFORCE_SAFE_MODE, &issock, &socketd, NULL);
	if (!fp && !socketd){
		if (issock != BAD_URL) {
			char *tmp = estrndup(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1));
			php_strip_url_passwd(tmp);
			php_error(E_WARNING,"readfile(\"%s\") - %s", tmp, strerror(errno));
			efree(tmp);
		}
		RETURN_FALSE;
	}

	if (issock) {
		int *sock=emalloc(sizeof(int));
		*sock = socketd;
		rsrc_id = ZEND_REGISTER_RESOURCE(NULL,sock,php_file_le_socket());
	} else {
		rsrc_id = ZEND_REGISTER_RESOURCE(NULL,fp,php_file_le_fopen());
	}

	size = php_passthru_fd(socketd, fp, issock);

	zend_list_delete(rsrc_id);

	RETURN_LONG(size);
}

/* }}} */
/* {{{ proto int umask([int mask])
   Return or change the umask */

PHP_FUNCTION(umask)
{
	pval **arg1;
	int oldumask;
	int arg_count = ARG_COUNT(ht);
	
	oldumask = umask(077);

	if (arg_count == 0) {
		umask(oldumask);
	} else {
		if (arg_count > 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg1);
		umask((*arg1)->value.lval);
	}

	/* XXX we should maybe reset the umask after each request! */

	RETURN_LONG(oldumask);
}

/* }}} */
/* {{{ proto int fpassthru(int fp)
   Output all remaining data from a file pointer */

PHP_FUNCTION(fpassthru)
{
	pval **arg1;
	int size, type;
	int issock=0;
	int socketd=0;
	void *what;
	
	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	size = php_passthru_fd(socketd, (FILE*) what, issock);

	zend_list_delete((*arg1)->value.lval);

	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto int rename(string old_name, string new_name)
   Rename a file */
PHP_FUNCTION(rename)
{
	pval **old_arg, **new_arg;
	char *old_name, *new_name;
	int ret;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &old_arg, &new_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(old_arg);
	convert_to_string_ex(new_arg);

	old_name = (*old_arg)->value.str.val;
	new_name = (*new_arg)->value.str.val;

	if (PG(safe_mode) &&(!php_checkuid(old_name, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}
	ret = V_RENAME(old_name, new_name);

	if (ret == -1) {
		php_error(E_WARNING,"Rename failed (%s)", strerror(errno));
		RETURN_FALSE;
	}

	RETVAL_TRUE;
}
/* }}} */


/* {{{ proto int unlink(string filename)
   Delete a file */
PHP_FUNCTION(unlink)
{
	pval **filename;
	int ret;
	PLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if (PG(safe_mode) && !php_checkuid((*filename)->value.str.val, NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	ret = V_UNLINK((*filename)->value.str.val);
	if (ret == -1) {
		php_error(E_WARNING, "Unlink failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	/* Clear stat cache */
	PHP_FN(clearstatcache)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int ftruncate (int fp, int size)
   Truncate file to 'size' length */
PHP_NAMED_FUNCTION(php_if_ftruncate)
{
	zval **fp , **size;
	short int ret;
	int type;
	void *what;

	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &fp, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	what = zend_fetch_resource(fp,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket)
	{
		php_error(E_WARNING, "can't truncate sockets!");
		RETURN_FALSE;
	}

	convert_to_long_ex(size);

	ret = ftruncate(fileno((FILE *)what), (*size)->value.lval);
	RETURN_LONG(ret + 1);
}
/* }}} */

/* {{{ proto int fstat(int fp)
   Stat() on a filehandle */
PHP_NAMED_FUNCTION(php_if_fstat)
{
	zval **fp;
	int type;
	void *what;
	struct stat stat_sb;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &fp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(fp,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (fstat(fileno((FILE *) what ), &stat_sb)) {
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_long ( return_value , "dev" , stat_sb.st_dev );
	add_assoc_long ( return_value , "ino" , stat_sb.st_ino );
	add_assoc_long ( return_value , "mode" , stat_sb.st_mode );
	add_assoc_long ( return_value , "nlink" , stat_sb.st_nlink );
	add_assoc_long ( return_value , "uid" , stat_sb.st_uid );
	add_assoc_long ( return_value , "gid" , stat_sb.st_gid );

#ifdef HAVE_ST_RDEV
	add_assoc_long ( return_value, "rdev" , stat_sb.st_rdev );
#endif
#ifdef HAVE_ST_BLKSIZE
	add_assoc_long ( return_value , "blksize" , stat_sb.st_blksize );
#endif
	
	add_assoc_long ( return_value , "size" , stat_sb.st_size );
	add_assoc_long ( return_value , "atime" , stat_sb.st_atime );
	add_assoc_long ( return_value , "mtime" , stat_sb.st_mtime );
	add_assoc_long ( return_value , "ctime" , stat_sb.st_ctime );

#ifdef HAVE_ST_BLOCKS
	add_assoc_long ( return_value , "blocks" , stat_sb.st_blocks );
#endif
}
/* }}} */


/* {{{ proto int copy(string source_file, string destination_file)
   Copy a file */

PHP_FUNCTION(copy)
{
	pval **source, **target;
	PLS_FETCH();

	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &source, &target) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(source);
	convert_to_string_ex(target);

	if (PG(safe_mode) &&(!php_checkuid((*source)->value.str.val, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (PG(safe_mode) &&(!php_checkuid((*target)->value.str.val, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_copy_file(Z_STRVAL_PP(source), Z_STRVAL_PP(target))==SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */


PHPAPI int php_copy_file(char *src, char *dest)
{
	char buffer[8192];
	int fd_s,fd_t,read_bytes;
	int ret = FAILURE;

#ifdef PHP_WIN32
	if ((fd_s=V_OPEN((src,O_RDONLY|_O_BINARY)))==-1) {
#else
	if ((fd_s=V_OPEN((src,O_RDONLY)))==-1) {
#endif
		php_error(E_WARNING,"Unable to open '%s' for reading:  %s", src, strerror(errno));
		return FAILURE;
	}
#ifdef PHP_WIN32
	if ((fd_t=V_OPEN((dest,_O_WRONLY|_O_CREAT|_O_TRUNC|_O_BINARY,_S_IREAD|_S_IWRITE)))==-1) {
#else
	if ((fd_t=V_CREAT(dest,0777))==-1) {
#endif
		php_error(E_WARNING,"Unable to create '%s':  %s", dest, strerror(errno));
		close(fd_s);
		return FAILURE;
	}

#ifdef HAVE_MMAP
	{
		void *srcfile;
		struct stat sbuf;

		if (fstat(fd_s, &sbuf)) {
			goto cleanup;
		}
		srcfile = mmap(NULL, sbuf.st_size, PROT_READ, MAP_SHARED, fd_s, 0);
		if (srcfile != (void *) MAP_FAILED) {
			if (write(fd_t, srcfile, sbuf.st_size) == sbuf.st_size)
				ret = SUCCESS;
			munmap(srcfile, sbuf.st_size);
			goto cleanup;
		}
	}
#endif

	while ((read_bytes=read(fd_s,buffer,8192))!=-1 && read_bytes!=0) {
		if (write(fd_t,buffer,read_bytes)==-1) {
			php_error(E_WARNING,"Unable to write to '%s':  %s", dest, strerror(errno));
			goto cleanup;
		}
	}
	ret = SUCCESS;
	
cleanup:
	close(fd_s);
	close(fd_t);
	return ret;
}	




/* {{{ proto string fread(int fp, int length)
   Binary-safe file read */

PHP_FUNCTION(fread)
{
	pval **arg1, **arg2;
	int len, type;
	int issock=0;
	int socketd=0;
	void *what;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(arg1,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	convert_to_long_ex(arg2);
	len = (*arg2)->value.lval;
    if (len < 0) {
		php_error(E_WARNING, "length parameter to fread() may not be negative");
		RETURN_FALSE;
    }

	return_value->value.str.val = emalloc(sizeof(char) * (len + 1));
	/* needed because recv doesnt put a null at the end*/
	
	if (!issock) {
#ifdef HAVE_FLUSHIO
		if (type == le_fopen)
			fseek((FILE*)what, 0, SEEK_CUR);
		}
#endif
		return_value->value.str.len = fread(return_value->value.str.val, 1, len, (FILE*)what);
		return_value->value.str.val[return_value->value.str.len] = 0;
	} else {
		return_value->value.str.len = SOCK_FREAD(return_value->value.str.val, len, socketd);
	}
	if (PG(magic_quotes_runtime)) {
		return_value->value.str.val = php_addslashes(return_value->value.str.val,return_value->value.str.len,&return_value->value.str.len,1);
	}
	return_value->type = IS_STRING;
}

/* }}} */
/* {{{ proto array fgetcsv(int fp, int length [, string delimiter])
   Get line from file pointer and parse for CSV fields */
 
PHP_FUNCTION(fgetcsv) {
	char *temp, *tptr, *bptr, *lineEnd;
	char delimiter = ',';	/* allow this to be set as parameter */

	/* first section exactly as php_fgetss */

	pval **fd, **bytes, **p_delim;
	int len, type;
	char *buf;
	int issock=0;
	int socketd=0;
	void *what;
	
	switch(ARG_COUNT(ht)) {
	case 2:
		if (zend_get_parameters_ex(2, &fd, &bytes) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
		
	case 3:
		if (zend_get_parameters_ex(3, &fd, &bytes, &p_delim) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(p_delim);
		/* Make sure that there is at least one character in string */
		if ((*p_delim)->value.str.len < 1) {
			WRONG_PARAM_COUNT;
		}
			/* use first character from string */
		delimiter = (*p_delim)->value.str.val[0];
		break;
		
	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}

	what = zend_fetch_resource(fd,-1,"File-Handle",&type,3,le_fopen,le_popen,le_socket);
	ZEND_VERIFY_RESOURCE(what);

	if (type == le_socket) {
		issock=1;
		socketd=*(int*)what;
	}

	convert_to_long_ex(bytes);
	len = (*bytes)->value.lval;
    if (len < 0) {
		php_error(E_WARNING, "length parameter to fgetcsv() may not be negative");
		RETURN_FALSE;
    }

	buf = emalloc(sizeof(char) * (len + 1));
	/*needed because recv doesnt set null char at end*/
	memset(buf, 0, len + 1);
	if (FP_FGETS(buf, len, socketd, (FILE*)what, issock) == NULL) {
		efree(buf);
		RETURN_FALSE;
	}

	/* Now into new section that parses buf for comma/quote delimited fields */

	/* Strip trailing space from buf, saving end of line in case required for quoted field */

	lineEnd = emalloc(sizeof(char) * (len + 1));
        bptr = buf;
        tptr = buf + strlen(buf) -1;
        while ( isspace((int)*tptr) && (*tptr!=delimiter) && (tptr > bptr) ) tptr--;
        tptr++;
        strcpy(lineEnd, tptr);

	/* add single space - makes it easier to parse trailing null field */
	*tptr++ = ' ';
	*tptr = 0;

	/* reserve workspace for building each individual field */

	temp = emalloc(sizeof(char) * len);	/*	unlikely but possible! */
	tptr = temp;

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		efree(lineEnd);
		efree(temp);
		efree(buf);
		RETURN_FALSE;
	}

	/* Main loop to read CSV fields */
	/* NB this routine will return a single null entry for a blank line */

	do	{
		/* 1. Strip any leading space */		
		while(isspace((int)*bptr) && (*bptr!=delimiter)) bptr++;	
		/* 2. Read field, leaving bptr pointing at start of next field */
		if (*bptr == '"') {
			/* 2A. handle quote delimited field */
			bptr++;		/* move on to first character in field */
			while (*bptr) {
				if (*bptr == '"') {
					/* handle the double-quote */
					if ( *(bptr+1) == '"') {
					/* embedded double quotes */
						*tptr++ = *bptr; bptr +=2;
					} else {
					/* must be end of string - skip to start of next field or end */
						while ( (*bptr != delimiter) && *bptr ) bptr++;
						if (*bptr == delimiter) bptr++;
						*tptr=0;	/* terminate temporary string */
						break;	/* .. from handling this field - resumes at 3. */
					}
				} else {
				/* normal character */
					*tptr++ = *bptr++;

                                        if (*bptr == 0) {       /* embedded line end? */
                                                *(tptr-1)=0;            /* remove space character added on reading line */
                                                strcat(temp,lineEnd);   /* add the embedded line end to the field */

                                                        /* read a new line from input, as at start of routine */
                                                memset(buf,0,len+1);
						if (FP_FGETS(buf, len, socketd, (FILE*)what, issock) == NULL) {
                                                        efree(lineEnd); efree(temp); efree(buf);
														zval_dtor(return_value);
                                                        RETURN_FALSE;
                                                        }
                                                bptr = buf;
                                                tptr = buf + strlen(buf) -1;
                                                while ( isspace((int)*tptr) && (*tptr!=delimiter) && (tptr > bptr) ) tptr--;
                                                tptr++; strcpy(lineEnd, tptr);
                                                *tptr++ = ' ';  *tptr = 0;

                                                tptr=temp;      /* reset temp pointer to end of field as read so far */
                                                while (*tptr) tptr++;
                                        }
				}
			}
		} else {
			/* 2B. Handle non-quoted field */
			while ( (*bptr != delimiter) && *bptr ) *tptr++ = *bptr++;
			*tptr=0;	/* terminate temporary string */
			if (strlen(temp)) {
				tptr--;
				while (isspace((int)*tptr) && (*tptr!=delimiter)) *tptr-- = 0;	/* strip any trailing spaces */
			}
			if (*bptr == delimiter) bptr++;
		}
		/* 3. Now pass our field back to php */
		add_next_index_string(return_value, temp, 1);
		tptr=temp;
	} while (*bptr);
	
	efree(lineEnd);
	efree(temp);
	efree(buf);
}

/* }}} */


#if (!defined(PHP_WIN32) && !defined(__BEOS__)) || defined(ZTS)
/* {{{ proto string realpath(string path)
   Return the resolved path */
PHP_FUNCTION(realpath)
{
	zval **path;
	char resolved_path_buff[MAXPATHLEN];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &path) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path);
	
	if (V_REALPATH((*path)->value.str.val, resolved_path_buff)) {
		RETURN_STRING(resolved_path_buff, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif


#if 0

static fd_set readfd;
static int max_fd;

PHP_FUNCTION(fd_set)
{
        pval **arg;
        void *what;
        int type, fd;

        if(ARG_COUNT(ht) <= 0) {
                php_error(E_WARNING, "fd_set: Must be passed at least one value" );
                var_uninit(return_value);
                return;
        }
        else if(ARG_COUNT(ht) == 1) {
                if(zend_get_parameters_ex(1, &arg) == FAILURE) {
                        WRONG_PARAM_COUNT;
                }
        what = zend_fetch_resource(arg,-1,"select",&type,3,le_fopen,le_socket,le_popen);
        ZEND_VERIFY_RESOURCE(what);
        if(type == le_socket) {
                fd = *(int *)what;
        } else {
                fd = fileno((FILE *)what);
        }
        max_fd = fd;
        FD_ZERO(&readfd);
        FD_SET(max_fd, &readfd);
        }
        else {
                pval ***args = (pval ***) emalloc(sizeof(pval **) * ARG_COUNT(ht));
                int i;
                if(zend_get_parameters_array_ex(ARG_COUNT(ht), args) == FAILURE) {
                        efree(args);
                        WRONG_PARAM_COUNT;
                }
                FD_ZERO(&readfd);
                for(i = 0; i < ARG_COUNT(ht); i++) {
                        what = zend_fetch_resource(*args,-1,"select",&type,3,le_fopen,le_socket,le_popen);
                        ZEND_VERIFY_RESOURCE(what);
                        if(type == le_socket) {
                                fd = *(int *)what;
                        } else {
                                fd = fileno((FILE *)what);
                        }
                FD_SET(fd, &readfd);
                if(fd > max_fd) max_fd = fd;
		}
	efree(args);
	}
	RETURN_LONG(1);
}

PHP_FUNCTION(select)
{
	pval **timeout;
	struct timeval tv;

	if(ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &timeout) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(timeout);

	tv.tv_sec = (*timeout)->value.lval / 1000000;
	tv.tv_usec = (*timeout)->value.lval % 1000000;

	RETURN_LONG(select(max_fd + 1,&readfd,NULL,NULL,((*timeout)->value.lval <= 0) ? NULL : &tv));
}

PHP_FUNCTION(fd_isset)
{
	pval **fdarg;
	void *what;
	int type, fd;

	if(ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &fdarg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(fdarg,-1,"select",&type,3,le_fopen,le_socket,le_popen);
	ZEND_VERIFY_RESOURCE(what);

	if(type == le_socket) {
		fd = *(int *)what;
	} else {
		fd = fileno((FILE *)what);
	}

	if(FD_ISSET(fd,&readfd)) {
		RETURN_TRUE;
	}	
	RETURN_FALSE;
}	

#endif

/* Function reads all data from file or socket and puts it into the buffer */
size_t php_fread_all(char **buf, int socket, FILE *fp, int issock) {
	size_t ret;
	char *ptr;
	size_t len = 0, max_len;
	int step = PHP_FSOCK_CHUNK_SIZE;
	int min_room = PHP_FSOCK_CHUNK_SIZE/4;
	
	ptr = *buf = emalloc(step);
	max_len = step;

	while((ret = FP_FREAD(ptr, max_len - len, socket, fp, issock))) {
		len += ret;
		if(len + min_room >= max_len) {
			*buf = erealloc(*buf, max_len + step);
			max_len += step;
			ptr = *buf + len;
		}
	}

	if(len) {
		*buf = erealloc(*buf, len);
	} else {
		efree(*buf);
		*buf = NULL;
	}

	return len;
}

/* See http://www.w3.org/TR/html4/intro/sgmltut.html#h-3.2.2 */
#define PHP_META_HTML401_CHARS "-_.:"

/* Tokenizes an HTML file for get_meta_tags */
php_meta_tags_token php_next_meta_token(FILE *fp, int socketd, int issock, int *use_last_char, int *last_char, char **data, int *datalen) {
	int ch, compliment;
	char buff[META_DEF_BUFSIZE + 1];

	memset((void *)buff,0,META_DEF_BUFSIZE + 1);

	while (*use_last_char || (!FP_FEOF(socketd,fp,issock) && (ch = FP_FGETC(socketd,fp,issock)))) {

		if(FP_FEOF(socketd,fp,issock))
			break;

		if (*use_last_char) {
			ch = *last_char;
			*use_last_char = 0;
		}

        switch (ch) {
        case '<':
            return TOK_OPENTAG;
            break;
        case '>':
            return TOK_CLOSETAG;
            break;
        case '=':
            return TOK_EQUAL;
            break;
        case '/':
            return TOK_SLASH;
            break;
        case '\'':
        case '"':
            compliment = ch;
            *datalen = 0;
            while (!FP_FEOF(socketd,fp,issock) && (ch = FP_FGETC(socketd,fp,issock)) && ch != compliment) {
				buff[(*datalen)++] = ch;

				if (*datalen == META_DEF_BUFSIZE)
					break;
			}
			
            *data = (char *) emalloc( *datalen + 1 );
			memcpy(*data,buff,*datalen+1);

			return TOK_STRING;
			break;
		case '\n':
		case '\r':
		case '\t':
			break;
		case ' ':
            return TOK_SPACE;
            break;
        default:
            if (isalnum(ch)) {
                *datalen = 0;
                buff[(*datalen)++] = ch;
				while (!FP_FEOF(socketd,fp,issock) &&
					   (ch = FP_FGETC(socketd,fp,issock)) &&
					   (isalnum(ch) || strchr(PHP_META_HTML401_CHARS,ch))) {

					buff[(*datalen)++] = ch;

					if (*datalen == META_DEF_BUFSIZE)
						break;
				}

				/* This is ugly, but we have to replace ungetc */
                if (!isalpha(ch) && ch != '-') {
					*use_last_char = 1;
					*last_char = ch;
				}

                *data = (char *) emalloc( *datalen + 1 );
                memcpy(*data,buff,*datalen+1);

				return TOK_ID;
			} else {
				return TOK_OTHER;
			}
			break;
		}
	}

	return TOK_EOF;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */


