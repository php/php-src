/*
   +----------------------------------------------------------------------+
   | PHP version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
#include "shmop_arginfo.h"

# ifndef PHP_WIN32
# include <sys/ipc.h>
# include <sys/shm.h>
#else
#include "tsrm_win32.h"
#endif


#ifdef HAVE_SHMOP

#include "ext/standard/info.h"

/* {{{ shmop_module_entry */
zend_module_entry shmop_module_entry = {
	STANDARD_MODULE_HEADER,
	"shmop",
	ext_functions,
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

typedef struct php_shmop
{
	int shmid;
	key_t key;
	int shmflg;
	int shmatflg;
	char *addr;
	zend_long size;
	zend_object std;
} php_shmop;

zend_class_entry *shmop_ce;
static zend_object_handlers shmop_object_handlers;

static inline php_shmop *shmop_from_obj(zend_object *obj)
{
	return (php_shmop *)((char *)(obj) - XtOffsetOf(php_shmop, std));
}

#define Z_SHMOP_P(zv) shmop_from_obj(Z_OBJ_P(zv))

static zend_object *shmop_create_object(zend_class_entry *class_type)
{
	php_shmop *intern = zend_object_alloc(sizeof(php_shmop), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &shmop_object_handlers;

	return &intern->std;
}

static zend_function *shmop_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "Cannot directly construct Shmop, use shmop_open() instead");
	return NULL;
}

static void shmop_free_obj(zend_object *object)
{
	php_shmop *shmop = shmop_from_obj(object);

	shmdt(shmop->addr);

	zend_object_std_dtor(&shmop->std);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(shmop)
{
	shmop_ce = register_class_Shmop();
	shmop_ce->create_object = shmop_create_object;

	memcpy(&shmop_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	shmop_object_handlers.offset = XtOffsetOf(php_shmop, std);
	shmop_object_handlers.free_obj = shmop_free_obj;
	shmop_object_handlers.get_constructor = shmop_get_constructor;
	shmop_object_handlers.clone_obj = NULL;
	shmop_object_handlers.compare = zend_objects_not_comparable;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(shmop)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "shmop support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ gets and attaches a shared memory segment */
PHP_FUNCTION(shmop_open)
{
	zend_long key, mode, size;
	php_shmop *shmop;
	struct shmid_ds shm;
	char *flags;
	size_t flags_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lsll", &key, &flags, &flags_len, &mode, &size) == FAILURE) {
		RETURN_THROWS();
	}

	if (flags_len != 1) {
		zend_argument_value_error(2, "must be a valid access mode");
		RETURN_THROWS();
	}

	object_init_ex(return_value, shmop_ce);
	shmop = Z_SHMOP_P(return_value);
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
			zend_argument_value_error(2, "must be a valid access mode");
			goto err;
	}

	if (shmop->shmflg & IPC_CREAT && shmop->size < 1) {
		zend_argument_value_error(4, "must be greater than 0 for the \"c\" and \"n\" access modes");
		goto err;
	}

	shmop->shmid = shmget(shmop->key, shmop->size, shmop->shmflg);
	if (shmop->shmid == -1) {
		php_error_docref(NULL, E_WARNING, "Unable to attach or create shared memory segment \"%s\"", strerror(errno));
		goto err;
	}

	if (shmctl(shmop->shmid, IPC_STAT, &shm)) {
		/* please do not add coverage here: the segment would be leaked and impossible to delete via php */
		php_error_docref(NULL, E_WARNING, "Unable to get shared memory segment information \"%s\"", strerror(errno));
		goto err;
	}

	if (shm.shm_segsz > ZEND_LONG_MAX) {
		zend_argument_value_error(4, "is too large");
		goto err;
	}

	shmop->addr = shmat(shmop->shmid, 0, shmop->shmatflg);
	if (shmop->addr == (char*) -1) {
		php_error_docref(NULL, E_WARNING, "Unable to attach to shared memory segment \"%s\"", strerror(errno));
		goto err;
	}

	shmop->size = shm.shm_segsz;
	return;

err:
	zend_object_release(Z_OBJ_P(return_value));
	RETURN_FALSE;
}
/* }}} */

/* {{{ reads from a shm segment */
PHP_FUNCTION(shmop_read)
{
	zval *shmid;
	zend_long start, count;
	php_shmop *shmop;
	char *startaddr;
	int bytes;
	zend_string *return_string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oll", &shmid, shmop_ce, &start, &count) == FAILURE) {
		RETURN_THROWS();
	}

	shmop = Z_SHMOP_P(shmid);

	if (start < 0 || start > shmop->size) {
		zend_argument_value_error(2, "must be between 0 and the segment size");
		RETURN_THROWS();
	}

	if (count < 0 || start > (ZEND_LONG_MAX - count) || start + count > shmop->size) {
		zend_argument_value_error(3, "is out of range");
		RETURN_THROWS();
	}

	startaddr = shmop->addr + start;
	bytes = count ? count : shmop->size - start;

	return_string = zend_string_init(startaddr, bytes, 0);

	RETURN_NEW_STR(return_string);
}
/* }}} */

/* {{{ used to close a shared memory segment; now a NOP */
PHP_FUNCTION(shmop_close)
{
	zval *shmid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &shmid, shmop_ce) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ returns the shm size */
PHP_FUNCTION(shmop_size)
{
	zval *shmid;
	php_shmop *shmop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &shmid, shmop_ce) == FAILURE) {
		RETURN_THROWS();
	}

	shmop = Z_SHMOP_P(shmid);

	RETURN_LONG(shmop->size);
}
/* }}} */

/* {{{ writes to a shared memory segment */
PHP_FUNCTION(shmop_write)
{
	php_shmop *shmop;
	zend_long writesize;
	zend_long offset;
	zend_string *data;
	zval *shmid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSl", &shmid, shmop_ce, &data, &offset) == FAILURE) {
		RETURN_THROWS();
	}

	shmop = Z_SHMOP_P(shmid);

	if ((shmop->shmatflg & SHM_RDONLY) == SHM_RDONLY) {
		zend_throw_error(NULL, "Read-only segment cannot be written");
		RETURN_THROWS();
	}

	if (offset < 0 || offset > shmop->size) {
		zend_argument_value_error(3, "is out of range");
		RETURN_THROWS();
	}

	writesize = ((zend_long)ZSTR_LEN(data) < shmop->size - offset) ? (zend_long)ZSTR_LEN(data) : shmop->size - offset;
	memcpy(shmop->addr + offset, ZSTR_VAL(data), writesize);

	RETURN_LONG(writesize);
}
/* }}} */

/* {{{ mark segment for deletion */
PHP_FUNCTION(shmop_delete)
{
	zval *shmid;
	php_shmop *shmop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &shmid, shmop_ce) == FAILURE) {
		RETURN_THROWS();
	}

	shmop = Z_SHMOP_P(shmid);

	if (shmctl(shmop->shmid, IPC_RMID, NULL)) {
		php_error_docref(NULL, E_WARNING, "Can't mark segment for deletion (are you the owner?)");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#endif	/* HAVE_SHMOP */
