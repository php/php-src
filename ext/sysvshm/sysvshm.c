/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
#include "zend_types.h"

#if HAVE_SYSVSHM

#include <errno.h>

#include "php_sysvshm.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"
#include "zend_interfaces.h"
#include "php_ini.h"

static inline shm_object *shm_object_from_zend_object(zend_object *object) {
	return (shm_object*)((char*)(object) - XtOffsetOf(shm_object, std));
}
static zend_object *shm_object_new(zend_class_entry *class_type) /* {{{ */
{
    shm_object *intern = ecalloc(1, sizeof(shm_object) + zend_object_properties_size(class_type));

    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    intern->std.handlers = &shm_object_handlers;

    return &intern->std;
}
/* }}} */
static void shm_object_free_storage(zend_object *object) /* {{{ */
{
	shm_object *intern = shm_object_from_zend_object(object);

	if (intern->shm_object_ptr != NULL) {
		sysvshm_shm *shm_ptr = intern->shm_object_ptr;
		shmdt((void *) shm_ptr->ptr);
		efree(shm_ptr);
		intern->shm_object_ptr = NULL;
	}

	zend_object_std_dtor(&intern->std);
}
/* }}} */

/* {{{ sysvshm_functions[]
 */
static const zend_function_entry sysvshm_functions[] = {
	PHP_FE(shm_attach,		arginfo_shm_attach)
	PHP_FE(shm_detach,		arginfo_shm_detach)
	PHP_FE(shm_remove, 		arginfo_shm_remove)
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

THREAD_LS sysvshm_module php_sysvshm;

static int shm_set_shm_data(sysvshm_chunk_head *ptr, zend_long key, const char *data, zend_long len);
static zend_long shm_has_shm_data(sysvshm_chunk_head *ptr, zend_long key);
static int php_remove_shm_data(sysvshm_chunk_head *ptr, zend_long shm_varpos);

static void shm_object_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	char *shm_ptr;
	sysvshm_chunk_head *chunk_ptr;
	zend_long shm_key, shm_id, shm_size = php_sysvshm.init_mem, shm_flag = 0666;

	if (SUCCESS != zend_parse_parameters_throw(ZEND_NUM_ARGS(), "l|ll", &shm_key, &shm_size, &shm_flag)) {
		return;
	}

	if (shm_size < 1) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Segment size " ZEND_LONG_FMT " must be greater than zero", shm_size);
		RETURN_NULL();
  	}

	shm_object_ptr = (sysvshm_shm *) emalloc(sizeof(sysvshm_shm));

	/* get the id from a specified key or create new shared memory */
	if ((shm_id = shmget(shm_key, 0, 0)) < 0) {
		if (shm_size < (zend_long)sizeof(sysvshm_chunk_head)) {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Failed for key 0x" ZEND_XLONG_FMT ": memorysize too small", shm_key);
			efree(shm_object_ptr);
			RETURN_NULL();
		}
		if ((shm_id = shmget(shm_key, shm_size, shm_flag | IPC_CREAT | IPC_EXCL)) < 0) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Failed for key 0x" ZEND_XLONG_FMT ": %s", shm_key, strerror(errno));
			efree(shm_object_ptr);
			RETURN_NULL();
		}
	}

	if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void *) -1) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Failed for key 0x" ZEND_XLONG_FMT ": %s", shm_key, strerror(errno));
		efree(shm_object_ptr);
		RETURN_NULL();
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

	shm_object_ptr->key = shm_key;
	shm_object_ptr->id = shm_id;
	shm_object_ptr->ptr = chunk_ptr;

    intern = shm_object_from_zend_object(Z_OBJ_P(return_value));
	intern->shm_object_ptr = shm_object_ptr;
}
static int shm_set(sysvshm_shm *shm_object_ptr, zend_long shm_key, zval *arg_var)
{
	int ret;
	smart_str shm_var = {0};
	php_serialize_data_t var_hash;

	if (!shm_object_ptr) {
		smart_str_free(&shm_var);
		return 0;
	}

	/* setup string-variable and serialize */
	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&shm_var, arg_var, &var_hash);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	/* insert serialized variable into shared memory */
	ret = shm_set_shm_data(shm_object_ptr->ptr, shm_key, shm_var.s? ZSTR_VAL(shm_var.s) : NULL, shm_var.s? ZSTR_LEN(shm_var.s) : 0);

	/* free string */
	smart_str_free(&shm_var);

	return ret;
}
static void shm_get(sysvshm_shm *shm_object_ptr, zend_long shm_key, zval *return_value)
{
	zval *shm_id;
	char *shm_data;
	zend_long shm_varpos;
	sysvshm_chunk *shm_var;
	php_unserialize_data_t var_hash;

	/* setup string-variable and serialize */
	/* get serialized variable from shared memory */
	shm_varpos = shm_has_shm_data((shm_object_ptr->ptr), shm_key);

	if (shm_varpos < 0) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Variable key " ZEND_LONG_FMT " doesn't exist", shm_key);
		RETURN_NULL();
	}
	shm_var = (sysvshm_chunk*) ((char *)shm_object_ptr->ptr + shm_varpos);
	shm_data = &shm_var->mem;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (php_var_unserialize(return_value, (const unsigned char **) &shm_data, (unsigned char *) shm_data + shm_var->length, &var_hash) != 1) {
		zend_throw_exception(spl_ce_RuntimeException, "Variable data in shared memory is corrupted", 0);
		RETURN_NULL();
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}

/* {{{ proto void SharedMemoryBlock::__construct(int key [, int memsize [, int perm]])
   Argument constructor */
ZEND_METHOD(SharedMemoryBlock, __construct)
{
	return_value = getThis();
	shm_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void SharedMemoryBlock::has(int variable_key)
   Check whether a specific entry exists */
ZEND_METHOD(SharedMemoryBlock, has)
{
	zend_long shm_key;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(shm_key)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(getThis()));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_ptr = intern->shm_object_ptr;

	RETURN_BOOL(shm_has_shm_data(shm_object_ptr->ptr, shm_key) >= 0);
}
/* }}} */

/* {{{ proto void SharedMemoryBlock::get(int variable_key)
   Returns a variable from shared memory */
ZEND_METHOD(SharedMemoryBlock, get)
{
	zend_long shm_key;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(shm_key)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(getThis()));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_get(intern->shm_object_ptr, shm_key, return_value);
}
/* }}} */

/* {{{ proto void SharedMemoryBlock::set(int variable_key, mixed arg_var)
   Inserts or updates a variable in shared memory
 */
ZEND_METHOD(SharedMemoryBlock, set)
{
	zval *arg_var;
	zend_long shm_key;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;
	int ret;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(shm_key)
		Z_PARAM_ZVAL(arg_var)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(getThis()));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	ret = shm_set(intern->shm_object_ptr, shm_key, arg_var);

	if (ret == -1) {
		zend_throw_exception(spl_ce_RuntimeException, "Not enough shared memory left", 0);
	}
}
/* }}} */

/* {{{ proto void SharedMemoryBlock::remove()
   Removes a variable from shared memory */
ZEND_METHOD(SharedMemoryBlock, remove)
{
	zend_long shm_key, shm_varpos;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(shm_key)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(getThis()));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_ptr = intern->shm_object_ptr;
	shm_varpos = shm_has_shm_data((shm_object_ptr->ptr), shm_key);

	if (shm_varpos < 0) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Variable key " ZEND_LONG_FMT " doesn't exist", shm_key);
		RETURN_NULL();
	}
	php_remove_shm_data((shm_object_ptr->ptr), shm_varpos);
	RETURN_NULL();
}
/* }}} */

/* {{{ proto void SharedMemoryBlock::free()
   Removes shared memory from Unix systems */
ZEND_METHOD(SharedMemoryBlock, free)
{
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    intern = shm_object_from_zend_object(Z_OBJ_P(getThis()));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_ptr = intern->shm_object_ptr;
	if (shmctl(shm_object_ptr->id, IPC_RMID, NULL) < 0) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Failed for key 0x%x, id " ZEND_LONG_FMT ": %s", shm_object_ptr->key, shm_object_ptr->id, strerror(errno));
	}

	shm_object_free_storage(Z_OBJ_P(getThis()));
	
	RETURN_NULL();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sysvshm)
{
    zend_class_entry tmp_ce;
    zend_function_entry shm_object_methods[] = {
        PHP_ME(SharedMemoryBlock, __construct,	arginfo_shm_object___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
        PHP_ME(SharedMemoryBlock, has,			arginfo_shm_object_has,         ZEND_ACC_PUBLIC)
        PHP_ME(SharedMemoryBlock, get,			arginfo_shm_object_get,         ZEND_ACC_PUBLIC)
        PHP_ME(SharedMemoryBlock, set,			arginfo_shm_object_set,         ZEND_ACC_PUBLIC)
        PHP_ME(SharedMemoryBlock, remove,		arginfo_shm_object_remove, 		ZEND_ACC_PUBLIC)
        PHP_ME(SharedMemoryBlock, free,		arginfo_shm_object_free, 	ZEND_ACC_PUBLIC)
        PHP_FE_END
    };
    INIT_CLASS_ENTRY(tmp_ce, "SharedMemoryBlock", shm_object_methods);
    tmp_ce.create_object = shm_object_new;
    shm_ce_ptr = zend_register_internal_class_ex(&tmp_ce, NULL);
    memcpy(&shm_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers)); 
    shm_object_handlers.offset = XtOffsetOf(shm_object, std);
	shm_object_handlers.free_obj = shm_object_free_storage;

	if (cfg_get_long("sysvshm.init_mem", &php_sysvshm.init_mem) == FAILURE) {
		php_sysvshm.init_mem=10000;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ proto SharedMemoryBlock shm_attach(int key [, int memsize [, int perm]])
   Creates or open a shared memory segment */
PHP_FUNCTION(shm_attach)
{
    object_init_ex(return_value, shm_ce_ptr);
    shm_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void shm_detach(SharedMemoryBlock object)
   Disconnects from shared memory segment */
PHP_FUNCTION(shm_detach)
{
	zval *object;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS_EX(object, shm_ce_ptr, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());

    intern = shm_object_from_zend_object(Z_OBJ_P(object));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_free_storage(Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto bool shm_remove(SharedMemoryBlock object)
   Removes shared memory from Unix systems */
PHP_FUNCTION(shm_remove)
{
	zval *object;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS_EX(object, shm_ce_ptr, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    intern = shm_object_from_zend_object(Z_OBJ_P(object));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_ptr = intern->shm_object_ptr;

	if (shmctl(shm_object_ptr->id, IPC_RMID, NULL) < 0) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Failed for key 0x%x, id " ZEND_LONG_FMT ": %s", shm_object_ptr->key, shm_object_ptr->id, strerror(errno));
	}
	shm_object_free_storage(Z_OBJ_P(object));
}
/* }}} */

/* {{{ proto bool shm_put_var(resource object, int variable_key, mixed variable)
   Inserts or updates a variable in shared memory */
PHP_FUNCTION(shm_put_var)
{
	zval *object, *arg_var;
	zend_long shm_key;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;
	int ret;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS_EX(object, shm_ce_ptr, 1, 0)
		Z_PARAM_LONG(shm_key)
		Z_PARAM_ZVAL(arg_var)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = (shm_object *)shm_object_from_zend_object(Z_OBJ_P(object));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}

	ret = shm_set(intern->shm_object_ptr, shm_key, arg_var);

	if (ret == -1) {
		zend_throw_exception(spl_ce_RuntimeException, "Not enough shared memory left", 0);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed shm_get_var(SharedMemoryBlock object, int variable_key)
   Returns a variable from shared memory */
PHP_FUNCTION(shm_get_var)
{
	zval *object;
	zend_long shm_key;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object, shm_ce_ptr, 1, 0)
		Z_PARAM_LONG(shm_key)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(object));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_get(intern->shm_object_ptr, shm_key, return_value);
}
/* }}} */

/* {{{ proto bool shm_has_var(SharedMemoryBlock object, int variable_key)
	Checks whether a specific entry exists */
PHP_FUNCTION(shm_has_var)
{
	zval *object;
	zend_long shm_key;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object, shm_ce_ptr, 1, 0)
		Z_PARAM_LONG(shm_key)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(object));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_ptr = intern->shm_object_ptr;

	RETURN_BOOL(shm_has_shm_data(shm_object_ptr->ptr, shm_key) >= 0);
}
/* }}} */

/* {{{ proto bool shm_remove_var(SharedMemoryBlock object, int variable_key)
   Removes variable from shared memory */
PHP_FUNCTION(shm_remove_var)
{
	zval *object;
	zend_long shm_key, shm_varpos;
	shm_object *intern;
	sysvshm_shm *shm_object_ptr;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object, shm_ce_ptr, 1, 0)
		Z_PARAM_LONG(shm_key)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	intern = shm_object_from_zend_object(Z_OBJ_P(object));
	if (intern->shm_object_ptr == NULL) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid SHaredMemoryBlock object internal state");
		RETURN_NULL();
	}
	shm_object_ptr = intern->shm_object_ptr;
	shm_varpos = shm_has_shm_data((shm_object_ptr->ptr), shm_key);

	if (shm_varpos < 0) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Variable key " ZEND_LONG_FMT " doesn't exist", shm_key);
		RETURN_FALSE;
	}
	php_remove_shm_data((shm_object_ptr->ptr), shm_varpos);
	RETURN_TRUE;
}
/* }}} */

/* {{{ shm_set_shm_data
 * inserts an ascii-string into shared memory */
static int shm_set_shm_data(sysvshm_chunk_head *ptr, zend_long key, const char *data, zend_long len)
{
	sysvshm_chunk *shm_var;
	zend_long total_size;
	zend_long shm_varpos;

	total_size = ((zend_long) (len + sizeof(sysvshm_chunk) - 1) / sizeof(zend_long)) * sizeof(zend_long) + sizeof(zend_long); /* zend_long alligment */

	if ((shm_varpos = shm_has_shm_data(ptr, key)) > 0) {
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

/* {{{ shm_has_shm_data
 */
static zend_long shm_has_shm_data(sysvshm_chunk_head *ptr, zend_long key)
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
