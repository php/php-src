/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
#include "ext/json/php_json.h"

PHPDBG_API void phpdbg_webdata_compress(char **msg, int *len TSRMLS_DC) {
#ifdef HAVE_JSON
	smart_str buf = {0};
	zval array;
	HashTable *ht;
	/* I really need to change that to an array of zvals... */
	zval zv1 = {{0}}, *zvp1 = &zv1;
	zval zv2 = {{0}}, *zvp2 = &zv2;
	zval zv3 = {{0}}, *zvp3 = &zv3;
	zval zv4 = {{0}}, *zvp4 = &zv4;
	zval zv5 = {{0}}, *zvp5 = &zv5;
	zval zv6 = {{0}}, *zvp6 = &zv6;
	zval zv7 = {{0}}, *zvp7 = &zv7;
	zval zv8 = {{0}}, *zvp8 = &zv8;

	array_init(&array);
	ht = Z_ARRVAL(array);

	/* fetch superglobals */
	{
		zend_is_auto_global(ZEND_STRL("GLOBALS") TSRMLS_CC);
		/* might be JIT */
		zend_is_auto_global(ZEND_STRL("_ENV") TSRMLS_CC);
		zend_is_auto_global(ZEND_STRL("_SERVER") TSRMLS_CC);
		zend_is_auto_global(ZEND_STRL("_REQUEST") TSRMLS_CC);
		array_init(&zv1);
		zend_hash_copy(Z_ARRVAL(zv1), &EG(symbol_table), NULL, (void *) NULL, sizeof(zval *));
		Z_ARRVAL(zv1)->pDestructor = NULL; /* we're operating on a copy! Don't double free zvals */
		zend_hash_del(Z_ARRVAL(zv1), "GLOBALS", sizeof("GLOBALS")); /* do not use the reference to itself in json */
		zend_hash_add(ht, "GLOBALS", sizeof("GLOBALS"), &zvp1, sizeof(zval *), NULL);
	}

	/* save php://input */
	{
		php_stream *stream;
		int len;
		char *contents;

		stream = php_stream_temp_create_ex(TEMP_STREAM_DEFAULT, SAPI_POST_BLOCK_SIZE, PG(upload_tmp_dir));
		if ((len = php_stream_copy_to_mem(stream, &contents, PHP_STREAM_COPY_ALL, 0)) > 0) {
			ZVAL_STRINGL(&zv2, contents, len, 0);
		} else {
			ZVAL_EMPTY_STRING(&zv2);
		}
		Z_SET_REFCOUNT(zv2, 2);
		zend_hash_add(ht, "input", sizeof("input"), &zvp2, sizeof(zval *), NULL);
	}

	/* change sapi name */
	{
		if (sapi_module.name) {
			ZVAL_STRING(&zv6, sapi_module.name, 0);
		} else {
			Z_TYPE(zv6) = IS_NULL;
		}
		Z_SET_REFCOUNT(zv6, 2);
		zend_hash_add(ht, "sapi_name", sizeof("sapi_name"), &zvp6, sizeof(zval *), NULL);
	}

	/* handle modules / extensions */
	{
		HashPosition position;
		zend_module_entry *module;
		zend_extension *extension;
		zend_llist_position pos;

		array_init(&zv7);
		for (zend_hash_internal_pointer_reset_ex(&module_registry, &position);
		     zend_hash_get_current_data_ex(&module_registry, (void**) &module, &position) == SUCCESS;
		     zend_hash_move_forward_ex(&module_registry, &position)) {
			zval **value = emalloc(sizeof(zval *));
			ALLOC_ZVAL(*value);
			ZVAL_STRING(*value, module->name, 1);
			zend_hash_next_index_insert(Z_ARRVAL(zv7), value, sizeof(zval *), NULL);
		}
		zend_hash_add(ht, "modules", sizeof("modules"), &zvp7, sizeof(zval *), NULL);

		array_init(&zv8);
		extension = (zend_extension *) zend_llist_get_first_ex(&zend_extensions, &pos);
		while (extension) {
			zval **value = emalloc(sizeof(zval *));
			ALLOC_ZVAL(*value);
			ZVAL_STRING(*value, extension->name, 1);
			zend_hash_next_index_insert(Z_ARRVAL(zv8), value, sizeof(zval *), NULL);
			extension = (zend_extension *) zend_llist_get_next_ex(&zend_extensions, &pos);
		}
		zend_hash_add(ht, "extensions", sizeof("extensions"), &zvp8, sizeof(zval *), NULL);
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
			ZVAL_STRING(&zv5, path, 1);
			Z_SET_REFCOUNT(zv5, 1);
			zend_hash_add(ht, "cwd", sizeof("cwd"), &zvp5, sizeof(zval *), NULL);
		}
	}

	/* get system ini entries */
	{
		HashPosition position;
		zend_ini_entry *ini_entry;

		array_init(&zv3);
		for (zend_hash_internal_pointer_reset_ex(EG(ini_directives), &position);
		     zend_hash_get_current_data_ex(EG(ini_directives), (void**) &ini_entry, &position) == SUCCESS;
		     zend_hash_move_forward_ex(EG(ini_directives), &position)) {
			zval **value = emalloc(sizeof(zval *));
			if (ini_entry->modified) {
				if (!ini_entry->orig_value) {
					efree(value);
					continue;
				}
				ALLOC_ZVAL(*value);
				ZVAL_STRINGL(*value, ini_entry->orig_value, ini_entry->orig_value_length, 1);
			} else {
				if (!ini_entry->value) {
					efree(value);
					continue;
				}
				ALLOC_ZVAL(*value);
				ZVAL_STRINGL(*value, ini_entry->value, ini_entry->value_length, 1);
			}
			zend_hash_add(Z_ARRVAL(zv3), ini_entry->name, ini_entry->name_length, value, sizeof(zval *), NULL);
		}
		zend_hash_add(ht, "systemini", sizeof("systemini"), &zvp3, sizeof(zval *), NULL);
	}

	/* get perdir ini entries */
	if (EG(modified_ini_directives)) {
		HashPosition position;
		zend_ini_entry *ini_entry;

		array_init(&zv4);
		for (zend_hash_internal_pointer_reset_ex(EG(modified_ini_directives), &position);
		     zend_hash_get_current_data_ex(EG(modified_ini_directives), (void**) &ini_entry, &position) == SUCCESS;
		     zend_hash_move_forward_ex(EG(modified_ini_directives), &position)) {
			zval **value = emalloc(sizeof(zval *));
			if (!ini_entry->value) {
				efree(value);
				continue;
			}
			ALLOC_ZVAL(*value);
			ZVAL_STRINGL(*value, ini_entry->value, ini_entry->value_length, 1);
			zend_hash_add(Z_ARRVAL(zv4), ini_entry->name, ini_entry->name_length, value, sizeof(zval *), NULL);
		}
		zend_hash_add(ht, "userini", sizeof("userini"), &zvp4, sizeof(zval *), NULL);
	}

	/* encode data */
	php_json_encode(&buf, &array, 0 TSRMLS_CC);
	*msg = buf.c;
	*len = buf.len;
	zval_dtor(&array);
#endif
}
