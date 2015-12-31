/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Christian Cartus <cartus@atrior.de>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* This has been built and tested on Linux 2.2.14
 *
 * This has been built and tested on Solaris 2.6.
 * It may not compile or execute correctly on other systems.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_SYSVSHM

#include <errno.h>

#include "php_sysvshm.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"
#include "php_ini.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_attach, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, memsize)
	ZEND_ARG_INFO(0, perm)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_detach, 0, 0, 1)
	ZEND_ARG_INFO(0, shm_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_has_var, 0, 0, 2)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_INFO(0, variable_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_remove, 0, 0, 1)
	ZEND_ARG_INFO(0, shm_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_put_var, 0, 0, 3)
	ZEND_ARG_INFO(0, shm_identifier)
	ZEND_ARG_INFO(0, variable_key)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_get_var, 0, 0, 2)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_INFO(0, variable_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_remove_var, 0, 0, 2)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_INFO(0, variable_key)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ sysvshm_functions[]
 */
const zend_function_entry sysvshm_functions[] = {
	PHP_FE(shm_attach,		arginfo_shm_attach)
	PHP_FE(shm_remove,		arginfo_shm_detach)
	PHP_FE(shm_detach, 		arginfo_shm_remove)
	PHP_FE(shm_put_var,		arginfo_shm_put_var)
	PHP_FE(shm_has_var,		arginfo_shm_has_var)
	PHP_FE(shm_get_var,		arginfo_shm_get_var)
	PHP_FE(shm_remove_var,	arginfo_shm_remove_var)
	PHP_FE_END
};
/* }}} */

/* {{{ sysvshm_module_entry
 */
zend_module_entry sysvshm_module_entry = {
	STANDARD_MODULE_HEADER,
	"sysvshm",
	sysvshm_functions,
	PHP_MINIT(sysvshm),
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_SYSVSHM_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SYSVSHM
ZEND_GET_MODULE(sysvshm)
#endif

#undef shm_ptr					/* undefine AIX-specific macro */

#define SHM_FETCH_RESOURCE(shm_ptr, z_ptr) do { \
	if ((shm_ptr = (sysvshm_shm *)zend_fetch_resource(Z_RES_P(z_ptr), PHP_SHM_RSRC_NAME, php_sysvshm.le_shm)) == NULL) { \
		RETURN_FALSE; \
	} \
} while (0)

THREAD_LS sysvshm_module php_sysvshm;

static int php_put_shm_data(sysvshm_chunk_head *ptr, zend_long key, const char *data, zend_long len);
static zend_long php_check_shm_data(sysvshm_chunk_head *ptr, zend_long key);
static int php_remove_shm_data(sysvshm_chunk_head *ptr, zend_long shm_varpos);

/* {{{ php_release_sysvshm
 */
static void php_release_sysvshm(zend_resource *rsrc)
{
	sysvshm_shm *shm_ptr = (sysvshm_shm *) rsrc->ptr;
	shmdt((void *) shm_ptr->ptr);
	efree(shm_ptr);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sysvshm)
{
	php_sysvshm.le_shm = zend_register_list_destructors_ex(php_release_sysvshm, NULL, PHP_SHM_RSRC_NAME, module_number);

	if (cfg_get_long("sysvshm.init_mem", &php_sysvshm.init_mem) == FAILURE) {
		php_sysvshm.init_mem=10000;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ proto int shm_attach(int key [, int memsize [, int perm]])
   Creates or open a shared memory segment */
PHP_FUNCTION(shm_attach)
{
	sysvshm_shm *shm_list_ptr;
	char *shm_ptr;
	sysvshm_chunk_head *chunk_ptr;
	zend_long shm_key, shm_id, shm_size = php_sysvshm.init_mem, shm_flag = 0666;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "l|ll", &shm_key, &shm_size, &shm_flag)) {
		return;
	}

	if (shm_size < 1) {
		php_error_docref(NULL, E_WARNING, "Segment size must be greater than zero");
		RETURN_FALSE;
  	}

	shm_list_ptr = (sysvshm_shm *) emalloc(sizeof(sysvshm_shm));

	/* get the id from a specified key or create new shared memory */
	if ((shm_id = shmget(shm_key, 0, 0)) < 0) {
		if (shm_size < sizeof(sysvshm_chunk_head)) {
			php_error_docref(NULL, E_WARNING, "failed for key 0x%px: memorysize too small", shm_key);
			efree(shm_list_ptr);
			RETURN_FALSE;
		}
		if ((shm_id = shmget(shm_key, shm_size, shm_flag | IPC_CREAT | IPC_EXCL)) < 0) {
			php_error_docref(NULL, E_WARNING, "failed for key 0x%px: %s", shm_key, strerror(errno));
			efree(shm_list_ptr);
			RETURN_FALSE;
		}
	}

	if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void *) -1) {
		php_error_docref(NULL, E_WARNING, "failed for key 0x%px: %s", shm_key, strerror(errno));
		efree(shm_list_ptr);
		RETURN_FALSE;
	}

	/* check if shm is already initialized */
	chunk_ptr = (sysvshm_chunk_head *) shm_ptr;
	if (strcmp((char*) &(chunk_ptr->magic), "PHP_SM") != 0) {
		strcpy((char*) &(chunk_ptr->magic), "PHP_SM");
		chunk_ptr->start = sizeof(sysvshm_chunk_head);
		chunk_ptr->end = chunk_ptr->start;
		chunk_ptr->total = shm_size;
		chunk_ptr->free = shm_size-chunk_ptr->end;
	}

	shm_list_ptr->key = shm_key;
	shm_list_ptr->id = shm_id;
	shm_list_ptr->ptr = chunk_ptr;

	RETURN_RES(zend_register_resource(shm_list_ptr, php_sysvshm.le_shm));
}
/* }}} */

/* {{{ proto bool shm_detach(resource shm_identifier)
   Disconnects from shared memory segment */
PHP_FUNCTION(shm_detach)
{
	zval *shm_id;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "r", &shm_id)) {
		return;
	}
	SHM_FETCH_RESOURCE(shm_list_ptr, shm_id);
	RETURN_BOOL(SUCCESS == zend_list_close(Z_RES_P(shm_id)));
}
/* }}} */

/* {{{ proto bool shm_remove(resource shm_identifier)
   Removes shared memory from Unix systems */
PHP_FUNCTION(shm_remove)
{
	zval *shm_id;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "r", &shm_id)) {
		return;
	}
	SHM_FETCH_RESOURCE(shm_list_ptr, shm_id);

	if (shmctl(shm_list_ptr->id, IPC_RMID, NULL) < 0) {
		php_error_docref(NULL, E_WARNING, "failed for key 0x%x, id %ld: %s", shm_list_ptr->key, Z_LVAL_P(shm_id), strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool shm_put_var(resource shm_identifier, int variable_key, mixed variable)
   Inserts or updates a variable in shared memory */
PHP_FUNCTION(shm_put_var)
{
	zval *shm_id, *arg_var;
	int ret;
	zend_long shm_key;
	sysvshm_shm *shm_list_ptr;
	smart_str shm_var = {0};
	php_serialize_data_t var_hash;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &shm_id, &shm_key, &arg_var)) {
		return;
	}

	/* setup string-variable and serialize */
	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&shm_var, arg_var, &var_hash);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	shm_list_ptr = zend_fetch_resource(Z_RES_P(shm_id), PHP_SHM_RSRC_NAME, php_sysvshm.le_shm);
	if (!shm_list_ptr) {
		smart_str_free(&shm_var);
		RETURN_FALSE;
	}

	/* insert serialized variable into shared memory */
	ret = php_put_shm_data(shm_list_ptr->ptr, shm_key, shm_var.s? ZSTR_VAL(shm_var.s) : NULL, shm_var.s? ZSTR_LEN(shm_var.s) : 0);

	/* free string */
	smart_str_free(&shm_var);

	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "not enough shared memory left");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed shm_get_var(resource id, int variable_key)
   Returns a variable from shared memory */
PHP_FUNCTION(shm_get_var)
{
	zval *shm_id;
	zend_long shm_key;
	sysvshm_shm *shm_list_ptr;
	char *shm_data;
	zend_long shm_varpos;
	sysvshm_chunk *shm_var;
	php_unserialize_data_t var_hash;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &shm_id, &shm_key)) {
		return;
	}
	SHM_FETCH_RESOURCE(shm_list_ptr, shm_id);

	/* setup string-variable and serialize */
	/* get serialized variable from shared memory */
	shm_varpos = php_check_shm_data((shm_list_ptr->ptr), shm_key);

	if (shm_varpos < 0) {
		php_error_docref(NULL, E_WARNING, "variable key %pd doesn't exist", shm_key);
		RETURN_FALSE;
	}
	shm_var = (sysvshm_chunk*) ((char *)shm_list_ptr->ptr + shm_varpos);
	shm_data = &shm_var->mem;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (php_var_unserialize(return_value, (const unsigned char **) &shm_data, (unsigned char *) shm_data + shm_var->length, &var_hash) != 1) {
		php_error_docref(NULL, E_WARNING, "variable data in shared memory is corrupted");
		RETVAL_FALSE;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}
/* }}} */

/* {{{ proto bool shm_has_var(resource id, int variable_key)
	Checks whether a specific entry exists */
PHP_FUNCTION(shm_has_var)
{
	zval *shm_id;
	zend_long shm_key;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &shm_id, &shm_key)) {
		return;
	}
	SHM_FETCH_RESOURCE(shm_list_ptr, shm_id);
	RETURN_BOOL(php_check_shm_data(shm_list_ptr->ptr, shm_key) >= 0);
}
/* }}} */

/* {{{ proto bool shm_remove_var(resource id, int variable_key)
   Removes variable from shared memory */
PHP_FUNCTION(shm_remove_var)
{
	zval *shm_id;
	zend_long shm_key, shm_varpos;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &shm_id, &shm_key)) {
		return;
	}
	SHM_FETCH_RESOURCE(shm_list_ptr, shm_id);

	shm_varpos = php_check_shm_data((shm_list_ptr->ptr), shm_key);

	if (shm_varpos < 0) {
		php_error_docref(NULL, E_WARNING, "variable key %pd doesn't exist", shm_key);
		RETURN_FALSE;
	}
	php_remove_shm_data((shm_list_ptr->ptr), shm_varpos);
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_put_shm_data
 * inserts an ascii-string into shared memory */
static int php_put_shm_data(sysvshm_chunk_head *ptr, zend_long key, const char *data, zend_long len)
{
	sysvshm_chunk *shm_var;
	zend_long total_size;
	zend_long shm_varpos;

	total_size = ((zend_long) (len + sizeof(sysvshm_chunk) - 1) / sizeof(zend_long)) * sizeof(zend_long) + sizeof(zend_long); /* zend_long alligment */

	if ((shm_varpos = php_check_shm_data(ptr, key)) > 0) {
		php_remove_shm_data(ptr, shm_varpos);
	}

	if (ptr->free < total_size) {
		return -1; /* not enough memory */
	}

	shm_var = (sysvshm_chunk *) ((char *) ptr + ptr->end);
	shm_var->key = key;
	shm_var->length = len;
	shm_var->next = total_size;
	memcpy(&(shm_var->mem), data, len);
	ptr->end += total_size;
	ptr->free -= total_size;
	return 0;
}
/* }}} */

/* {{{ php_check_shm_data
 */
static zend_long php_check_shm_data(sysvshm_chunk_head *ptr, zend_long key)
{
	zend_long pos;
	sysvshm_chunk *shm_var;

	pos = ptr->start;

	for (;;) {
		if (pos >= ptr->end) {
			return -1;
		}
		shm_var = (sysvshm_chunk*) ((char *) ptr + pos);
		if (shm_var->key == key) {
			return pos;
		}
		pos += shm_var->next;

		if (shm_var->next <= 0 || pos < ptr->start) {
			return -1;
		}
	}
	return -1;
}
/* }}} */

/* {{{ php_remove_shm_data
 */
static int php_remove_shm_data(sysvshm_chunk_head *ptr, zend_long shm_varpos)
{
	sysvshm_chunk *chunk_ptr, *next_chunk_ptr;
	zend_long memcpy_len;

	chunk_ptr = (sysvshm_chunk *) ((char *) ptr + shm_varpos);
	next_chunk_ptr = (sysvshm_chunk *) ((char *) ptr + shm_varpos + chunk_ptr->next);

	memcpy_len = ptr->end-shm_varpos - chunk_ptr->next;
	ptr->free += chunk_ptr->next;
	ptr->end -= chunk_ptr->next;
	if (memcpy_len > 0) {
		memmove(chunk_ptr, next_chunk_ptr, memcpy_len);
	}
	return 0;
}
/* }}} */

#endif /* HAVE_SYSVSHM */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
