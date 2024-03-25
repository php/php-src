/*
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
   | Author: Christian Cartus <cartus@atrior.de>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef HAVE_SYSVSHM

#include <errno.h>

#include "php_sysvshm.h"
#include "sysvshm_arginfo.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"
#include "php_ini.h"

/* SysvSharedMemory class */

zend_class_entry *sysvshm_ce;
static zend_object_handlers sysvshm_object_handlers;

static inline sysvshm_shm *sysvshm_from_obj(zend_object *obj) {
	return (sysvshm_shm *)((char *)(obj) - XtOffsetOf(sysvshm_shm, std));
}

#define Z_SYSVSHM_P(zv) sysvshm_from_obj(Z_OBJ_P(zv))

static zend_object *sysvshm_create_object(zend_class_entry *class_type) {
	sysvshm_shm *intern = zend_object_alloc(sizeof(sysvshm_shm), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *sysvshm_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct SysvSharedMemory, use shm_attach() instead");
	return NULL;
}

static void sysvshm_free_obj(zend_object *object)
{
	sysvshm_shm *sysvshm = sysvshm_from_obj(object);

	if (sysvshm->ptr) {
		shmdt((void *) sysvshm->ptr);
	}

	zend_object_std_dtor(&sysvshm->std);
}

/* {{{ sysvshm_module_entry */
zend_module_entry sysvshm_module_entry = {
	STANDARD_MODULE_HEADER,
	"sysvshm",
	ext_functions,
	PHP_MINIT(sysvshm),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sysvshm),
	PHP_SYSVSHM_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SYSVSHM
ZEND_GET_MODULE(sysvshm)
#endif

#undef shm_ptr					/* undefine AIX-specific macro */

/* TODO: Make this thread-safe. */
sysvshm_module php_sysvshm;

static int php_put_shm_data(sysvshm_chunk_head *ptr, zend_long key, const char *data, zend_long len);
static zend_long php_check_shm_data(sysvshm_chunk_head *ptr, zend_long key);
static int php_remove_shm_data(sysvshm_chunk_head *ptr, zend_long shm_varpos);

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(sysvshm)
{
	sysvshm_ce = register_class_SysvSharedMemory();
	sysvshm_ce->create_object = sysvshm_create_object;
	sysvshm_ce->default_object_handlers = &sysvshm_object_handlers;

	memcpy(&sysvshm_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	sysvshm_object_handlers.offset = XtOffsetOf(sysvshm_shm, std);
	sysvshm_object_handlers.free_obj = sysvshm_free_obj;
	sysvshm_object_handlers.get_constructor = sysvshm_get_constructor;
	sysvshm_object_handlers.clone_obj = NULL;
	sysvshm_object_handlers.compare = zend_objects_not_comparable;

	if (cfg_get_long("sysvshm.init_mem", &php_sysvshm.init_mem) == FAILURE) {
		php_sysvshm.init_mem=10000;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(sysvshm)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "sysvshm support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ Creates or open a shared memory segment */
PHP_FUNCTION(shm_attach)
{
	sysvshm_shm *shm_list_ptr;
	char *shm_ptr;
	sysvshm_chunk_head *chunk_ptr;
	zend_long shm_key, shm_id, shm_size, shm_flag = 0666;
	bool shm_size_is_null = 1;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "l|l!l", &shm_key, &shm_size, &shm_size_is_null, &shm_flag)) {
		RETURN_THROWS();
	}

	if (shm_size_is_null) {
		shm_size = php_sysvshm.init_mem;
	}

	if (shm_size < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	/* get the id from a specified key or create new shared memory */
	if ((shm_id = shmget(shm_key, 0, 0)) < 0) {
		if (shm_size < (zend_long)sizeof(sysvshm_chunk_head)) {
			php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": memorysize too small", shm_key);
			RETURN_FALSE;
		}
		if ((shm_id = shmget(shm_key, shm_size, shm_flag | IPC_CREAT | IPC_EXCL)) < 0) {
			php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": %s", shm_key, strerror(errno));
			RETURN_FALSE;
		}
	}

	if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void *) -1) {
		php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": %s", shm_key, strerror(errno));
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

	object_init_ex(return_value, sysvshm_ce);

	shm_list_ptr = Z_SYSVSHM_P(return_value);

	shm_list_ptr->key = shm_key;
	shm_list_ptr->id = shm_id;
	shm_list_ptr->ptr = chunk_ptr;
}
/* }}} */

/* {{{ Disconnects from shared memory segment */
PHP_FUNCTION(shm_detach)
{
	zval *shm_id;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "O", &shm_id, sysvshm_ce)) {
		RETURN_THROWS();
	}

	shm_list_ptr = Z_SYSVSHM_P(shm_id);
	if (!shm_list_ptr->ptr) {
		zend_throw_error(NULL, "Shared memory block has already been destroyed");
		RETURN_THROWS();
	}

	shmdt((void *) shm_list_ptr->ptr);
	shm_list_ptr->ptr = NULL;

	RETURN_TRUE;
}
/* }}} */

/* {{{ Removes shared memory from Unix systems */
PHP_FUNCTION(shm_remove)
{
	zval *shm_id;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "O", &shm_id, sysvshm_ce)) {
		RETURN_THROWS();
	}

	shm_list_ptr = Z_SYSVSHM_P(shm_id);
	if (!shm_list_ptr->ptr) {
		zend_throw_error(NULL, "Shared memory block has already been destroyed");
		RETURN_THROWS();
	}

	if (shmctl(shm_list_ptr->id, IPC_RMID, NULL) < 0) {
		php_error_docref(NULL, E_WARNING, "Failed for key 0x%x, id " ZEND_LONG_FMT ": %s", shm_list_ptr->key, Z_LVAL_P(shm_id), strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Inserts or updates a variable in shared memory */
PHP_FUNCTION(shm_put_var)
{
	zval *shm_id, *arg_var;
	int ret;
	zend_long shm_key;
	sysvshm_shm *shm_list_ptr;
	smart_str shm_var = {0};
	php_serialize_data_t var_hash;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "Olz", &shm_id, sysvshm_ce, &shm_key, &arg_var)) {
		RETURN_THROWS();
	}

	shm_list_ptr = Z_SYSVSHM_P(shm_id);
	if (!shm_list_ptr->ptr) {
		zend_throw_error(NULL, "Shared memory block has already been destroyed");
		RETURN_THROWS();
	}

	/* setup string-variable and serialize */
	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&shm_var, arg_var, &var_hash);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	/* insert serialized variable into shared memory */
	ret = php_put_shm_data(shm_list_ptr->ptr, shm_key, shm_var.s? ZSTR_VAL(shm_var.s) : NULL, shm_var.s? ZSTR_LEN(shm_var.s) : 0);

	/* free string */
	smart_str_free(&shm_var);

	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "Not enough shared memory left");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns a variable from shared memory */
PHP_FUNCTION(shm_get_var)
{
	zval *shm_id;
	zend_long shm_key;
	sysvshm_shm *shm_list_ptr;
	char *shm_data;
	zend_long shm_varpos;
	sysvshm_chunk *shm_var;
	php_unserialize_data_t var_hash;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &shm_id, sysvshm_ce, &shm_key)) {
		RETURN_THROWS();
	}

	shm_list_ptr = Z_SYSVSHM_P(shm_id);
	if (!shm_list_ptr->ptr) {
		zend_throw_error(NULL, "Shared memory block has already been destroyed");
		RETURN_THROWS();
	}

	/* setup string-variable and serialize */
	/* get serialized variable from shared memory */
	shm_varpos = php_check_shm_data(shm_list_ptr->ptr, shm_key);

	if (shm_varpos < 0) {
		php_error_docref(NULL, E_WARNING, "Variable key " ZEND_LONG_FMT " doesn't exist", shm_key);
		RETURN_FALSE;
	}
	shm_var = (sysvshm_chunk*) ((char *)shm_list_ptr->ptr + shm_varpos);
	shm_data = &shm_var->mem;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (php_var_unserialize(return_value, (const unsigned char **) &shm_data, (unsigned char *) shm_data + shm_var->length, &var_hash) != 1) {
		php_error_docref(NULL, E_WARNING, "Variable data in shared memory is corrupted");
		RETVAL_FALSE;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}
/* }}} */

/* {{{ Checks whether a specific entry exists */
PHP_FUNCTION(shm_has_var)
{
	zval *shm_id;
	zend_long shm_key;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &shm_id, sysvshm_ce, &shm_key)) {
		RETURN_THROWS();
	}

	shm_list_ptr = Z_SYSVSHM_P(shm_id);
	if (!shm_list_ptr->ptr) {
		zend_throw_error(NULL, "Shared memory block has already been destroyed");
		RETURN_THROWS();
	}

	RETURN_BOOL(php_check_shm_data(shm_list_ptr->ptr, shm_key) >= 0);
}
/* }}} */

/* {{{ Removes variable from shared memory */
PHP_FUNCTION(shm_remove_var)
{
	zval *shm_id;
	zend_long shm_key, shm_varpos;
	sysvshm_shm *shm_list_ptr;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &shm_id, sysvshm_ce, &shm_key)) {
		RETURN_THROWS();
	}

	shm_list_ptr = Z_SYSVSHM_P(shm_id);
	if (!shm_list_ptr->ptr) {
		zend_throw_error(NULL, "Shared memory block has already been destroyed");
		RETURN_THROWS();
	}

	shm_varpos = php_check_shm_data(shm_list_ptr->ptr, shm_key);

	if (shm_varpos < 0) {
		php_error_docref(NULL, E_WARNING, "Variable key " ZEND_LONG_FMT " doesn't exist", shm_key);
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

/* {{{ php_check_shm_data */
static zend_long php_check_shm_data(sysvshm_chunk_head *ptr, zend_long key)
{
	zend_long pos;
	sysvshm_chunk *shm_var;

	ZEND_ASSERT(ptr);

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

/* {{{ php_remove_shm_data */
static int php_remove_shm_data(sysvshm_chunk_head *ptr, zend_long shm_varpos)
{
	sysvshm_chunk *chunk_ptr, *next_chunk_ptr;
	zend_long memcpy_len;

	ZEND_ASSERT(ptr);

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
