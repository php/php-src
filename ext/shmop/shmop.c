/*
   +----------------------------------------------------------------------+
   | PHP version 4                                                        |
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
   | Authors: Slava Poliakov (hackie@prohost.org)                    	  |
   |          Ilia Alshanetsky (ilia@prohost.org)                         |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_shmop.h"
# ifndef PHP_WIN32
# include <sys/ipc.h>
# include <sys/shm.h>
#else
#include "tsrm_win32.h"
#endif


#if HAVE_SHMOP

#include "ext/standard/info.h"

#ifdef ZTS
int shmop_globals_id;
#else
php_shmop_globals shmop_globals;
#endif

int shm_type;

/* {{{ shmop_functions[] 
 */
function_entry shmop_functions[] = {
	PHP_FE(shmop_open, NULL)
	PHP_FE(shmop_read, NULL)
	PHP_FE(shmop_close, NULL)
	PHP_FE(shmop_size, NULL)
	PHP_FE(shmop_write, NULL)
	PHP_FE(shmop_delete, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in shmop_functions[] */
};
/* }}} */

/* {{{ shmop_module_entry
 */
zend_module_entry shmop_module_entry = {
	STANDARD_MODULE_HEADER,
	"shmop",
	shmop_functions,
	PHP_MINIT(shmop),
	PHP_MSHUTDOWN(shmop),
	NULL,
	NULL,
	PHP_MINFO(shmop),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SHMOP
ZEND_GET_MODULE(shmop)
#endif

/* {{{ rsclean
 */
static void rsclean(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct php_shmop *shmop = (struct php_shmop *)rsrc->ptr;

	shmdt(shmop->addr);
	efree(shmop);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(shmop)
{
	shm_type = zend_register_list_destructors_ex(rsclean, NULL, "shmop", module_number);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(shmop)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(shmop)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "shmop support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto int shmop_open (int key, int flags, int mode, int size)
   gets and attaches a shared memory segment */
PHP_FUNCTION(shmop_open)
{
	zval **key, **flags, **mode, **size;
	struct php_shmop *shmop;	
	struct shmid_ds shm;
	int rsid;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &key, &flags, &mode, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(key);
	convert_to_string_ex(flags);
	convert_to_long_ex(mode);
	convert_to_long_ex(size);

	shmop = emalloc(sizeof(struct php_shmop));
	memset(shmop, 0, sizeof(struct php_shmop));

	shmop->key = (*key)->value.lval;
	shmop->shmflg |= (*mode)->value.lval;

	if( (*flags)->value.str.len != 1 ) {
		php_error(E_WARNING, "shmop_open: invalid flag");
		RETURN_FALSE;
	}

	switch( (*flags)->value.str.val[0] ) 
	{
		case 'a':
			shmop->shmatflg |= SHM_RDONLY;
			break;
		case 'c':
			shmop->shmflg |= IPC_CREAT;
			shmop->size = (*size)->value.lval;
			break;
		case 'n':
			shmop->shmflg |= (IPC_CREAT|IPC_EXCL);
			shmop->size = (*size)->value.lval;
			break;	
		case 'w':
			/* noop 
				shm segment is being opened for read & write
				will fail if segment does not exist
			*/
			break;
		default:
			php_error(E_WARNING, "shmop_open: invalid access mode");
			efree(shmop);
			RETURN_FALSE;
	}

	shmop->shmid = shmget(shmop->key, shmop->size, shmop->shmflg);
	if (shmop->shmid == -1) {
		php_error(E_WARNING, "shmop_open: unable to attach or create shm segment");
		efree(shmop);
		RETURN_FALSE;
	}

	if (shmctl(shmop->shmid, IPC_STAT, &shm)) {
		efree(shmop);
		php_error(E_WARNING, "shmop_open: unable to get shm segment information");
		RETURN_FALSE;
	}	

	shmop->addr = shmat(shmop->shmid, 0, shmop->shmatflg);
	if (shmop->addr == (char*) -1) {
		efree(shmop);
		php_error(E_WARNING, "shmop_open: unable to attach to shm segment");
		RETURN_FALSE;
	}

	shmop->size = shm.shm_segsz;

	rsid = zend_list_insert(shmop, shm_type);
	RETURN_LONG(rsid);
}
/* }}} */

/* {{{ proto string shmop_read (int shmid, int start, int count)
   reads from a shm segment */
PHP_FUNCTION(shmop_read)
{
	zval **shmid, **start, **count;
	struct php_shmop *shmop;
	int type;
	char *startaddr;
	int bytes;
	char *return_string;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &shmid, &start, &count) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(shmid);
	convert_to_long_ex(start);
	convert_to_long_ex(count);

	shmop = zend_list_find((*shmid)->value.lval, &type);	

	if (!shmop) {
		php_error(E_WARNING, "shmop_read: can't find this segment");
		RETURN_FALSE;
	}

	if ((*start)->value.lval < 0 || (*start)->value.lval > shmop->size) {
		php_error(E_WARNING, "shmop_read: start is out of range");
		RETURN_FALSE;
	}

	if (((*start)->value.lval+(*count)->value.lval) > shmop->size) {
		php_error(E_WARNING, "shmop_read: count is out of range");
		RETURN_FALSE;
	}

	if ((*count)->value.lval < 0 ){
		php_error(E_WARNING, "shmop_read: count is out of range");
		RETURN_FALSE;
	}

	startaddr = shmop->addr + (*start)->value.lval;
	bytes = (*count)->value.lval ? (*count)->value.lval : shmop->size-(*start)->value.lval;

	return_string = emalloc(bytes);
	memcpy(return_string, startaddr, bytes);

	RETURN_STRINGL(return_string, bytes, 0);
}
/* }}} */

/* {{{ proto void shmop_close (int shmid)
   closes a shared memory segment */
PHP_FUNCTION(shmop_close)
{
	zval **shmid;
	struct php_shmop *shmop;
	int type;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &shmid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	shmop = zend_list_find((*shmid)->value.lval, &type);

	if (!shmop) {
		php_error(E_WARNING, "shmop_close: no such shmid");
		RETURN_FALSE;
	}
	zend_list_delete((*shmid)->value.lval);
}
/* }}} */

/* {{{ proto int shmop_size (int shmid)
   returns the shm size */
PHP_FUNCTION(shmop_size)
{
	zval **shmid;
	struct php_shmop *shmop;
	int type;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &shmid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(shmid);

	shmop = zend_list_find((*shmid)->value.lval, &type);

	if (!shmop) {
		php_error(E_WARNING, "shmop_size: no such segment");
		RETURN_FALSE;
	}

	RETURN_LONG(shmop->size);
}
/* }}} */

/* {{{ proto int shmop_write (int shmid, string data, int offset)
   writes to a shared memory segment */
PHP_FUNCTION(shmop_write)
{
	zval **shmid, **data, **offset;
	struct php_shmop *shmop;
	int type;
	int writesize;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &shmid, &data, &offset) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(shmid);
	convert_to_string_ex(data);
	convert_to_long_ex(offset);

	shmop = zend_list_find((*shmid)->value.lval, &type);

	if (!shmop) {
		php_error(E_WARNING, "shmop_write: error no such segment");
		RETURN_FALSE;
	}

	if( (shmop->shmatflg&SHM_RDONLY) == SHM_RDONLY ) {
		php_error(E_WARNING, "shmop_write: trying to write to a read only segment");
		RETURN_FALSE;
	}

	if ( (*offset)->value.lval > shmop->size ) {
		php_error(E_WARNING, "shmop_write: offset out of range");
		RETURN_FALSE;
	}

	writesize = ((*data)->value.str.len<shmop->size-(*offset)->value.lval) ? (*data)->value.str.len : shmop->size-(*offset)->value.lval;	
	memcpy(shmop->addr+(*offset)->value.lval, (*data)->value.str.val, writesize);

	RETURN_LONG(writesize);
}
/* }}} */

/* {{{ proto bool shmop_delete (int shmid)
   mark segment for deletion */
PHP_FUNCTION(shmop_delete)
{
	zval **shmid;
	struct php_shmop *shmop;
	int type;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &shmid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(shmid);

	shmop = zend_list_find((*shmid)->value.lval, &type);

	if (!shmop) {
		php_error(E_WARNING, "shmop_delete: error no such segment");
		RETURN_FALSE;
	}

	if (shmctl(shmop->shmid, IPC_RMID, NULL)) {
		php_error(E_WARNING, "shmop_delete: can't mark segment for deletion (are you the owner?)");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#endif	/* HAVE_SHMOP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
