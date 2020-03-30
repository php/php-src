/*
   +----------------------------------------------------------------------+
   | PHP version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Slava Poliakov <hackie@prohost.org>                         |
   |          Ilia Alshanetsky <ilia@prohost.org>                         |
   +----------------------------------------------------------------------+
 */

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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_shmop_open, 0, 0, 4)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shmop_read, 0, 0, 3)
	ZEND_ARG_INFO(0, shmid)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shmop_close, 0, 0, 1)
	ZEND_ARG_INFO(0, shmid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shmop_size, 0, 0, 1)
	ZEND_ARG_INFO(0, shmid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shmop_write, 0, 0, 3)
	ZEND_ARG_INFO(0, shmid)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shmop_delete, 0, 0, 1)
	ZEND_ARG_INFO(0, shmid)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ shmop_functions[]
 */
static const zend_function_entry shmop_functions[] = {
	PHP_FE(shmop_open, 		arginfo_shmop_open)
	PHP_FE(shmop_read, 		arginfo_shmop_read)
	PHP_FE(shmop_close, 	arginfo_shmop_close)
	PHP_FE(shmop_size, 		arginfo_shmop_size)
	PHP_FE(shmop_write, 	arginfo_shmop_write)
	PHP_FE(shmop_delete, 	arginfo_shmop_delete)
	PHP_FE_END
};
/* }}} */

/* {{{ shmop_module_entry
 */
zend_module_entry shmop_module_entry = {
	STANDARD_MODULE_HEADER,
	"shmop",
	shmop_functions,
	PHP_MINIT(shmop),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(shmop),
	PHP_SHMOP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SHMOP
ZEND_GET_MODULE(shmop)
#endif

/* {{{ rsclean
 */
static void rsclean(zend_resource *rsrc)
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

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(shmop)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "shmop support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource shmop_open(int key, string flags, int mode, int size)
   gets and attaches a shared memory segment */
PHP_FUNCTION(shmop_open)
{
	zend_long key, mode, size;
	struct php_shmop *shmop;
	struct shmid_ds shm;
	char *flags;
	size_t flags_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lsll", &key, &flags, &flags_len, &mode, &size) == FAILURE) {
		return;
	}

	if (flags_len != 1) {
		php_error_docref(NULL, E_WARNING, "%s is not a valid flag", flags);
		RETURN_FALSE;
	}

	shmop = emalloc(sizeof(struct php_shmop));
	memset(shmop, 0, sizeof(struct php_shmop));

	shmop->key = key;
	shmop->shmflg |= mode;

	switch (flags[0])
	{
		case 'a':
			shmop->shmatflg |= SHM_RDONLY;
			break;
		case 'c':
			shmop->shmflg |= IPC_CREAT;
			shmop->size = size;
			break;
		case 'n':
			shmop->shmflg |= (IPC_CREAT | IPC_EXCL);
			shmop->size = size;
			break;
		case 'w':
			/* noop
				shm segment is being opened for read & write
				will fail if segment does not exist
			*/
			break;
		default:
			php_error_docref(NULL, E_WARNING, "invalid access mode");
			goto err;
	}

	if (shmop->shmflg & IPC_CREAT && shmop->size < 1) {
		php_error_docref(NULL, E_WARNING, "Shared memory segment size must be greater than zero");
		goto err;
	}

	shmop->shmid = shmget(shmop->key, shmop->size, shmop->shmflg);
	if (shmop->shmid == -1) {
		php_error_docref(NULL, E_WARNING, "unable to attach or create shared memory segment '%s'", strerror(errno));
		goto err;
	}

	if (shmctl(shmop->shmid, IPC_STAT, &shm)) {
		/* please do not add coverage here: the segment would be leaked and impossible to delete via php */
		php_error_docref(NULL, E_WARNING, "unable to get shared memory segment information '%s'", strerror(errno));
		goto err;
	}

	if (shm.shm_segsz > ZEND_LONG_MAX) {
		php_error_docref(NULL, E_WARNING, "shared memory segment too large to attach");
		goto err;
	}

	shmop->addr = shmat(shmop->shmid, 0, shmop->shmatflg);
	if (shmop->addr == (char*) -1) {
		php_error_docref(NULL, E_WARNING, "unable to attach to shared memory segment '%s'", strerror(errno));
		goto err;
	}

	shmop->size = shm.shm_segsz;

	RETURN_RES(zend_register_resource(shmop, shm_type));
err:
	efree(shmop);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string shmop_read(resource shmid, int start, int count)
   reads from a shm segment */
PHP_FUNCTION(shmop_read)
{
	zval *shmid;
	zend_long start, count;
	struct php_shmop *shmop;
	char *startaddr;
	int bytes;
	zend_string *return_string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &shmid, &start, &count) == FAILURE) {
		return;
	}

	if ((shmop = (struct php_shmop *)zend_fetch_resource(Z_RES_P(shmid), "shmop", shm_type)) == NULL) {
		RETURN_FALSE;
	}

	if (start < 0 || start > shmop->size) {
		php_error_docref(NULL, E_WARNING, "start is out of range");
		RETURN_FALSE;
	}

	if (count < 0 || start > (INT_MAX - count) || start + count > shmop->size) {
		php_error_docref(NULL, E_WARNING, "count is out of range");
		RETURN_FALSE;
	}

	startaddr = shmop->addr + start;
	bytes = count ? count : shmop->size - start;

	return_string = zend_string_init(startaddr, bytes, 0);

	RETURN_NEW_STR(return_string);
}
/* }}} */

/* {{{ proto void shmop_close(resource shmid)
   closes a shared memory segment */
PHP_FUNCTION(shmop_close)
{
	zval *shmid;
	struct php_shmop *shmop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &shmid) == FAILURE) {
		return;
	}


	if ((shmop = (struct php_shmop *)zend_fetch_resource(Z_RES_P(shmid), "shmop", shm_type)) == NULL) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(shmid));
}
/* }}} */

/* {{{ proto int shmop_size(resource shmid)
   returns the shm size */
PHP_FUNCTION(shmop_size)
{
	zval *shmid;
	struct php_shmop *shmop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &shmid) == FAILURE) {
		return;
	}

	if ((shmop = (struct php_shmop *)zend_fetch_resource(Z_RES_P(shmid), "shmop", shm_type)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(shmop->size);
}
/* }}} */

/* {{{ proto int shmop_write(resource shmid, string data, int offset)
   writes to a shared memory segment */
PHP_FUNCTION(shmop_write)
{
	struct php_shmop *shmop;
	zend_long writesize;
	zend_long offset;
	zend_string *data;
	zval *shmid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSl", &shmid, &data, &offset) == FAILURE) {
		return;
	}

	if ((shmop = (struct php_shmop *)zend_fetch_resource(Z_RES_P(shmid), "shmop", shm_type)) == NULL) {
		RETURN_FALSE;
	}

	if ((shmop->shmatflg & SHM_RDONLY) == SHM_RDONLY) {
		php_error_docref(NULL, E_WARNING, "trying to write to a read only segment");
		RETURN_FALSE;
	}

	if (offset < 0 || offset > shmop->size) {
		php_error_docref(NULL, E_WARNING, "offset out of range");
		RETURN_FALSE;
	}

	writesize = ((zend_long)ZSTR_LEN(data) < shmop->size - offset) ? (zend_long)ZSTR_LEN(data) : shmop->size - offset;
	memcpy(shmop->addr + offset, ZSTR_VAL(data), writesize);

	RETURN_LONG(writesize);
}
/* }}} */

/* {{{ proto bool shmop_delete(resource shmid)
   mark segment for deletion */
PHP_FUNCTION(shmop_delete)
{
	zval *shmid;
	struct php_shmop *shmop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &shmid) == FAILURE) {
		return;
	}

	if ((shmop = (struct php_shmop *)zend_fetch_resource(Z_RES_P(shmid), "shmop", shm_type)) == NULL) {
		RETURN_FALSE;
	}

	if (shmctl(shmop->shmid, IPC_RMID, NULL)) {
		php_error_docref(NULL, E_WARNING, "can't mark segment for deletion (are you the owner?)");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#endif	/* HAVE_SHMOP */
