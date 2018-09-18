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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg_webdata_transfer.h"
#include "ext/standard/php_var.h"

static int phpdbg_is_auto_global(char *name, int len) {
	int ret;
	zend_string *str = zend_string_init(name, len, 0);
	ret = zend_is_auto_global(str);
	zend_string_free(str);
	return ret;
}

PHPDBG_API void phpdbg_webdata_compress(char **msg, size_t *len) {
	zval array;
	HashTable *ht;
	zval zv[9] = {{{0}}};

	array_init(&array);
	ht = Z_ARRVAL(array);

	/* fetch superglobals */
	{
		phpdbg_is_auto_global(ZEND_STRL("GLOBALS"));
		/* might be JIT */
		phpdbg_is_auto_global(ZEND_STRL("_ENV"));
		phpdbg_is_auto_global(ZEND_STRL("_SERVER"));
		phpdbg_is_auto_global(ZEND_STRL("_REQUEST"));
		array_init(&zv[1]);
		zend_hash_copy(Z_ARRVAL(zv[1]), &EG(symbol_table), NULL);
		Z_ARRVAL(zv[1])->pDestructor = NULL; /* we're operating on a copy! Don't double free zvals */
		zend_hash_str_del(Z_ARRVAL(zv[1]), ZEND_STRL("GLOBALS")); /* do not use the reference to itself in json */
		zend_hash_str_add(ht, ZEND_STRL("GLOBALS"), &zv[1]);
	}

	/* save php://input */
	{
		php_stream *stream;
		zend_string *str;

		stream = php_stream_temp_create_ex(TEMP_STREAM_DEFAULT, SAPI_POST_BLOCK_SIZE, PG(upload_tmp_dir));
		if ((str = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0))) {
			ZVAL_STR(&zv[2], str);
		} else {
			ZVAL_EMPTY_STRING(&zv[2]);
		}
		Z_SET_REFCOUNT(zv[2], 1);
		zend_hash_str_add(ht, ZEND_STRL("input"), &zv[2]);
	}

	/* change sapi name */
	{
		if (sapi_module.name) {
			ZVAL_STRING(&zv[6], sapi_module.name);
		} else {
			Z_TYPE_INFO(zv[6]) = IS_NULL;
		}
		zend_hash_str_add(ht, ZEND_STRL("sapi_name"), &zv[6]);
		Z_SET_REFCOUNT(zv[6], 1);
	}

	/* handle modules / extensions */
	{
		zend_module_entry *module;
		zend_extension *extension;
		zend_llist_position pos;

		array_init(&zv[7]);
		ZEND_HASH_FOREACH_PTR(&module_registry, module) {
			zval *value = ecalloc(sizeof(zval), 1);
			ZVAL_STRING(value, module->name);
			zend_hash_next_index_insert(Z_ARRVAL(zv[7]), value);
		} ZEND_HASH_FOREACH_END();
		zend_hash_str_add(ht, ZEND_STRL("modules"), &zv[7]);

		array_init(&zv[8]);
		extension = (zend_extension *) zend_llist_get_first_ex(&zend_extensions, &pos);
		while (extension) {
			zval *value = ecalloc(sizeof(zval), 1);
			ZVAL_STRING(value, extension->name);
			zend_hash_next_index_insert(Z_ARRVAL(zv[8]), value);
			extension = (zend_extension *) zend_llist_get_next_ex(&zend_extensions, &pos);
		}
		zend_hash_str_add(ht, ZEND_STRL("extensions"), &zv[8]);
	}

	/* switch cwd */
	if (SG(options) & SAPI_OPTION_NO_CHDIR) {
		char *ret = NULL;
		char path[MAXPATHLEN];

#if HAVE_GETCWD
		ret = VCWD_GETCWD(path, MAXPATHLEN);
#elif HAVE_GETWD
		ret = VCWD_GETWD(path);
#endif
		if (ret) {
			ZVAL_STRING(&zv[5], path);
			Z_SET_REFCOUNT(zv[5], 1);
			zend_hash_str_add(ht, ZEND_STRL("cwd"), &zv[5]);
		}
	}

	/* get system ini entries */
	{
		zend_ini_entry *ini_entry;

		array_init(&zv[3]);
		ZEND_HASH_FOREACH_PTR(EG(ini_directives), ini_entry) {
			zval *value = ecalloc(sizeof(zval), 1);
			if (ini_entry->modified) {
				if (!ini_entry->orig_value) {
					efree(value);
					continue;
				}
				ZVAL_STR(value, ini_entry->orig_value);
			} else {
				if (!ini_entry->value) {
					efree(value);
					continue;
				}
				ZVAL_STR(value, ini_entry->value);
			}
			zend_hash_add(Z_ARRVAL(zv[3]), ini_entry->name, value);
		} ZEND_HASH_FOREACH_END();
		zend_hash_str_add(ht, ZEND_STRL("systemini"), &zv[3]);
	}

	/* get perdir ini entries */
	if (EG(modified_ini_directives)) {
		zend_ini_entry *ini_entry;

		array_init(&zv[4]);
		ZEND_HASH_FOREACH_PTR(EG(ini_directives), ini_entry) {
			zval *value = ecalloc(sizeof(zval), 1);
			if (!ini_entry->value) {
				efree(value);
				continue;
			}
			ZVAL_STR(value, ini_entry->value);
			zend_hash_add(Z_ARRVAL(zv[4]), ini_entry->name, value);
		} ZEND_HASH_FOREACH_END();
		zend_hash_str_add(ht, ZEND_STRL("userini"), &zv[4]);
	}

	/* encode data */
	{
		php_serialize_data_t var_hash;
		smart_str buf = {0};

		PHP_VAR_SERIALIZE_INIT(var_hash);
		php_var_serialize(&buf, &array, &var_hash);
		PHP_VAR_SERIALIZE_DESTROY(var_hash);
		*msg = ZSTR_VAL(buf.s);
		*len = ZSTR_LEN(buf.s);
	}

	zend_array_destroy(Z_ARR(array));
}
