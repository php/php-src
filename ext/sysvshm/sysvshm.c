/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
#include "ext/standard/php_smart_str.h"

/* {{{ sysvshm_functions[]
 */
function_entry sysvshm_functions[] = {
	PHP_FE(shm_attach, NULL)
	PHP_FE(shm_remove, NULL)
	PHP_FE(shm_detach, NULL)
	PHP_FE(shm_put_var, NULL)
	PHP_FE(shm_get_var, NULL)
	PHP_FE(shm_remove_var, NULL)
	{NULL, NULL, NULL}	
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
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SYSVSHM
ZEND_GET_MODULE(sysvshm)
#endif

#undef shm_ptr					/* undefine AIX-specific macro */

THREAD_LS sysvshm_module php_sysvshm;

/* {{{ php_release_sysvshm
 */
static void php_release_sysvshm(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	sysvshm_shm *shm_ptr = (sysvshm_shm *)rsrc->ptr;
	shmdt((void*) shm_ptr->ptr);
	efree(shm_ptr);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sysvshm)
{    
	php_sysvshm.le_shm = zend_register_list_destructors_ex(php_release_sysvshm, NULL, "sysvshm", module_number);
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
	pval **arg_key,**arg_size,**arg_flag;
	long shm_size,shm_flag;
	sysvshm_shm *shm_list_ptr;
	char *shm_ptr;
	sysvshm_chunk_head *chunk_ptr;
	key_t shm_key = (key_t) 0;
	long shm_id,list_id;
	int ac = ZEND_NUM_ARGS();

	shm_flag = 0666;
	shm_size = php_sysvshm.init_mem;
	
	if (ac < 1 || ac > 3 || zend_get_parameters_ex(ac, &arg_key, &arg_size, &arg_flag) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (ac) {
		case 3:
			convert_to_long_ex(arg_flag);
			shm_flag = Z_LVAL_PP(arg_flag);
		case 2:
			convert_to_long_ex(arg_size);
			shm_size= Z_LVAL_PP(arg_size);
		case 1:
			convert_to_long_ex(arg_key);
			shm_key = Z_LVAL_PP(arg_key);
	}

	if ((shm_list_ptr = (sysvshm_shm *) emalloc(sizeof(sysvshm_shm))) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed for key 0x%x: cannot allocate internal listelement", shm_key);
		RETURN_FALSE;
	}

	/* get the id from a specified key or create new shared memory */
	if ((shm_id = shmget(shm_key, 0, 0)) < 0) {
		if (shm_size < sizeof(sysvshm_chunk_head)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed for key 0x%x: memorysize too small", shm_key);
			RETURN_FALSE;
		}
		if ((shm_id = shmget(shm_key, shm_size, shm_flag | IPC_CREAT|IPC_EXCL)) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed for key 0x%x: %s", shm_key, strerror(errno));
			RETURN_FALSE;
		}
	}

	if ((shm_ptr = shmat(shm_id,NULL,0)) == (void *)-1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed for key 0x%x: %s", shm_key, strerror(errno));
		RETURN_FALSE;
	}

	/* check if shm is already initialized */
	chunk_ptr = (sysvshm_chunk_head*) shm_ptr;
	if (strcmp((char*) &(chunk_ptr->magic), "PHP_SM") !=0 ) {
		strcpy((char*) &(chunk_ptr->magic), "PHP_SM");	
		chunk_ptr->start = sizeof(sysvshm_chunk_head);
		chunk_ptr->end = chunk_ptr->start;
		chunk_ptr->total = shm_size;
		chunk_ptr->free = shm_size-chunk_ptr->end;
	}


	shm_list_ptr->key = shm_key;
	shm_list_ptr->id = shm_id;
	shm_list_ptr->ptr = chunk_ptr;
	list_id = zend_list_insert(shm_list_ptr, php_sysvshm.le_shm);
	RETURN_LONG(list_id);
}
/* }}} */

/* {{{ proto int shm_detach(int shm_identifier)
   Disconnects from shared memory segment */
PHP_FUNCTION(shm_detach)
{
	pval **arg_id;
	long id;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_id);
	
	id = Z_LVAL_PP(arg_id);

	zend_list_delete(id);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int shm_remove(int shm_identifier)
   Removes shared memory from Unix systems */

PHP_FUNCTION(shm_remove)
{
	pval **arg_id;
	long id;
	int type;
	sysvshm_shm *shm_list_ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_id);
	
	id = Z_LVAL_PP(arg_id);

	shm_list_ptr = (sysvshm_shm *) zend_list_find(id, &type);

	if (!shm_list_ptr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The parameter is not a valid shm_indentifier");
		RETURN_FALSE;
	}

	if (shmctl(shm_list_ptr->id, IPC_RMID,NULL) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed for key 0x%x, id %i: %s", shm_list_ptr->key, id,strerror(errno));
		RETURN_FALSE;
	} 

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int shm_put_var(int shm_identifier, int variable_key, mixed variable)
   Inserts or updates a variable in shared memory */
PHP_FUNCTION(shm_put_var)
{
	pval **arg_id, **arg_key, **arg_var;
	long key, id;
	sysvshm_shm *shm_list_ptr;
	int type;
	smart_str shm_var = {0};
	int ret;	
	php_serialize_data_t var_hash;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg_id, &arg_key, &arg_var) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
			
	convert_to_long_ex(arg_id);
	id = Z_LVAL_PP(arg_id);
	convert_to_long_ex(arg_key);
	key = Z_LVAL_PP(arg_key);

	shm_list_ptr = (sysvshm_shm *) zend_list_find(id, &type);
	if (type != php_sysvshm.le_shm) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	/* setup string-variable and serialize */

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&shm_var, arg_var, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	/* insert serialized variable into shared memory */
	ret = php_put_shm_data(shm_list_ptr->ptr, key, shm_var.c, shm_var.len);

	/* free string */
	smart_str_free(&shm_var);
	
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "not enough shared memory left");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed shm_get_var(int id, int variable_key)
   Returns a variable from shared memory */
PHP_FUNCTION(shm_get_var)
{
	pval **arg_id, **arg_key;
	long key, id;
	sysvshm_shm *shm_list_ptr;
	int type;
	char *shm_data;	
	long shm_varpos;
	sysvshm_chunk *shm_var;
	php_unserialize_data_t var_hash;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg_id, &arg_key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_id);
	id = Z_LVAL_PP(arg_id);
	convert_to_long_ex(arg_key);
	key = Z_LVAL_PP(arg_key);

	shm_list_ptr = (sysvshm_shm *) zend_list_find(id, &type);
	if (type != php_sysvshm.le_shm) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	/* setup string-variable and serialize */
	/* get serialized variable from shared memory */
	shm_varpos = php_check_shm_data((shm_list_ptr->ptr), key);

	if (shm_varpos < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "variable key %d doesn't exist", key);
		RETURN_FALSE;
	}
	shm_var = (sysvshm_chunk*) ((char*)shm_list_ptr->ptr + shm_varpos);
	shm_data = &shm_var->mem;
	
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (php_var_unserialize(&return_value, (const char **) &shm_data, shm_data+shm_var->length,&var_hash TSRMLS_CC) != 1) {
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "variable data in shared memory is corruped");
		RETURN_FALSE;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}
/* }}} */

/* {{{ proto int shm_remove_var(int id, int variable_key)
   Removes variable from shared memory */
PHP_FUNCTION(shm_remove_var)
{
	pval **arg_id, **arg_key;
	long key, id;
	sysvshm_shm *shm_list_ptr;
	int type;
	long shm_varpos;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg_id, &arg_key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_id);
	id = Z_LVAL_PP(arg_id);
	convert_to_long_ex(arg_key);
	key = Z_LVAL_PP(arg_key);

	shm_list_ptr = (sysvshm_shm *) zend_list_find(id, &type);
	if (type != php_sysvshm.le_shm) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	shm_varpos = php_check_shm_data((shm_list_ptr->ptr), key);

	if (shm_varpos < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "variable key %d doesn't exist", key);
		RETURN_FALSE;
	}
	php_remove_shm_data((shm_list_ptr->ptr), shm_varpos);	
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_put_shm_data
 * inserts an ascii-string into shared memory */
int php_put_shm_data(sysvshm_chunk_head *ptr,long key,char *data, long len) {
	sysvshm_chunk* shm_var;
	long total_size;
	long shm_varpos;	

	total_size = ((long) (len+sizeof(sysvshm_chunk)-1)/4 )*4+4;    /* 4-byte alligment */

	if ((shm_varpos = php_check_shm_data(ptr,key)) > 0) {
		php_remove_shm_data(ptr, shm_varpos);	
	}
	
	if (ptr->free < total_size) {
		return -1;   /* not enough memeory */
	}

	shm_var = (sysvshm_chunk*) ((char *)ptr + ptr->end);	
	shm_var->key = key;
	shm_var->length = len;
	shm_var->next = total_size;   
	memcpy(&(shm_var->mem),data,len);	
	ptr->end += total_size;
	ptr->free -= total_size;
	return 0;
}
/* }}} */

/* {{{ php_check_shm_data
 */
long php_check_shm_data(sysvshm_chunk_head *ptr, long key) {
	long pos;
	sysvshm_chunk *shm_var;

	pos = ptr->start;
			
	for (;;) {
		if (pos >= ptr->end) {
			return -1;
		}
		shm_var = (sysvshm_chunk*) ((char*)ptr + pos);
		if (shm_var->key == key) {
			return pos;
		}	
		pos += shm_var->next;
	}
	return -1;
}
/* }}} */

/* {{{ php_remove_shm_data
 */
int php_remove_shm_data(sysvshm_chunk_head *ptr, long shm_varpos) {
	sysvshm_chunk *chunk_ptr, *next_chunk_ptr;
	long memcpy_len;
	
	chunk_ptr = (sysvshm_chunk*) ((char*)ptr + shm_varpos);
	next_chunk_ptr = (sysvshm_chunk*) ((char*)ptr + shm_varpos + chunk_ptr->next);
	
	memcpy_len = ptr->end-shm_varpos - chunk_ptr->next;
	ptr->free += chunk_ptr->next;
	ptr->end -= chunk_ptr->next;
	if (memcpy_len > 0)
		memcpy(chunk_ptr,next_chunk_ptr,memcpy_len);
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
