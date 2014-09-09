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

#ifndef PHPDBG_WEBDATA_TRANSFER_H
#define PHPDBG_WEBDATA_TRANSFER_H

/* {{{ remote console headers */
#ifndef _WIN32
#	include <sys/socket.h>
#	include <sys/un.h>
#	include <sys/select.h>
#	include <sys/types.h>
#endif /* }}} */

#include "zend.h"
#include "phpdbg.h"
#include "ext/json/php_json.h"
#include "ext/standard/basic_functions.h"

static inline void phpdbg_webdata_compress(char **msg, int *len TSRMLS_DC) {
	smart_str buf = {0};
	zval array;
	HashTable *ht;
	/* I really need to change that to an array of zvals... */
	zval zv1, *zvp1 = &zv1;
	zval zv2, *zvp2 = &zv2;
	zval zv3, *zvp3 = &zv3;
	zval zv4, *zvp4 = &zv4;
	zval zv5, *zvp5 = &zv5;
	zval zv6, *zvp6 = &zv6;
	zval zv7, *zvp7 = &zv7;
	zval zv8, *zvp8 = &zv8;

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
		zend_hash_add(ht, "input", sizeof("input"), &zvp2, sizeof(zval *), NULL);
	}

	/* change sapi name */
	{
		if (sapi_module.name) {
			ZVAL_STRING(&zv6, sapi_module.name, 1);
		} else {
			Z_TYPE(zv6) = IS_NULL;
		}
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
	{
		char *ret = NULL;
		char path[MAXPATHLEN];

#if HAVE_GETCWD
		ret = VCWD_GETCWD(path, MAXPATHLEN);
#elif HAVE_GETWD
		ret = VCWD_GETWD(path);
#endif
		if (ret) {
			ZVAL_STRING(&zv5, path, 1);
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
}

static void phpdbg_rebuild_http_globals_array(int type, const char *name TSRMLS_DC) {
	zval **zvpp;
	if (PG(http_globals)[type]) {
		zval_dtor(PG(http_globals)[type]);
	}
	if (zend_hash_find(&EG(symbol_table), name, strlen(name) + 1, (void **) &zvpp) == SUCCESS) {
		Z_SET_REFCOUNT_PP(zvpp, 2);
		PG(http_globals)[type] = *zvpp;
	}
}

/*
static int phpdbg_remove_rearm_autoglobals(zend_auto_global *auto_global TSRMLS_DC) {
//	zend_hash_del(&EG(symbol_table), auto_global->name, auto_global->name_len + 1);

	return 1;
}*/

typedef struct {
	HashTable *ht[2];
	HashPosition pos[2];
} phpdbg_intersect_ptr;

static void phpdbg_array_intersect_init(phpdbg_intersect_ptr *info, HashTable *ht1, HashTable *ht2 TSRMLS_DC) {
	info->ht[0] = ht1;
	info->ht[1] = ht2;

	zend_hash_sort(info->ht[0], zend_qsort, (compare_func_t) string_compare_function, 0 TSRMLS_CC);
	zend_hash_sort(info->ht[1], zend_qsort, (compare_func_t) string_compare_function, 0 TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(info->ht[0], &info->pos[0]);
	zend_hash_internal_pointer_reset_ex(info->ht[1], &info->pos[1]);
}

/* -1 => first array, 0 => both arrays equal, 1 => second array */
static int phpdbg_array_intersect(phpdbg_intersect_ptr *info, zval ***ptr) {
	int ret;
	zval **zvpp[2];
	int invalid = !info->ht[0] + !info->ht[1];

	if (invalid > 0) {
		invalid = !!info->ht[0];

		if (zend_hash_get_current_data_ex(info->ht[invalid], (void **) ptr, &info->pos[invalid]) == FAILURE) {
			*ptr = NULL;
			return 0;
		}

		zend_hash_move_forward_ex(info->ht[invalid], &info->pos[invalid]);

		return invalid ? -1 : 1;
	}

	if (zend_hash_get_current_data_ex(info->ht[0], (void **) &zvpp[0], &info->pos[0]) == FAILURE) {
		info->ht[0] = NULL;
		return phpdbg_array_intersect(info, ptr);
	}
	if (zend_hash_get_current_data_ex(info->ht[1], (void **) &zvpp[1], &info->pos[1]) == FAILURE) {
		info->ht[1] = NULL;
		return phpdbg_array_intersect(info, ptr);
	}

	ret = zend_binary_zval_strcmp(*zvpp[0], *zvpp[1]);

	if (ret <= 0) {
		*ptr = zvpp[0];
		zend_hash_move_forward_ex(info->ht[0], &info->pos[0]);
	}
	if (ret >= 0) {
		*ptr = zvpp[1];
		zend_hash_move_forward_ex(info->ht[1], &info->pos[1]);
	}

	return ret;
}

static inline void phpdbg_webdata_decompress(char *msg, int len TSRMLS_DC) {
	zval *free_zv = NULL;
	zval zv, **zvpp;
	HashTable *ht;
	php_json_decode(&zv, msg, len, 1, 1000 /* enough */ TSRMLS_CC);
	ht = Z_ARRVAL(zv);

	/* Reapply symbol table */
	if (zend_hash_find(ht, "GLOBALS", sizeof("GLOBALS"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		zend_hash_clean(&EG(symbol_table));
		EG(symbol_table) = *Z_ARRVAL_PP(zvpp);

		/* Rebuild cookies, env vars etc. from GLOBALS (PG(http_globals)) */
		phpdbg_rebuild_http_globals_array(TRACK_VARS_POST, "_POST" TSRMLS_CC);
		phpdbg_rebuild_http_globals_array(TRACK_VARS_GET, "_GET" TSRMLS_CC);
		phpdbg_rebuild_http_globals_array(TRACK_VARS_COOKIE, "_COOKIE" TSRMLS_CC);
		phpdbg_rebuild_http_globals_array(TRACK_VARS_SERVER, "_SERVER" TSRMLS_CC);
		phpdbg_rebuild_http_globals_array(TRACK_VARS_ENV, "_ENV" TSRMLS_CC);
		phpdbg_rebuild_http_globals_array(TRACK_VARS_FILES, "_FILES" TSRMLS_CC);

		Z_ADDREF_PP(zvpp);
		free_zv = *zvpp;
	}

	if (zend_hash_find(ht, "input", sizeof("input"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_STRING) {
		if (SG(request_info).request_body) {
			php_stream_close(SG(request_info).request_body);
		}
		SG(request_info).request_body = php_stream_temp_create_ex(TEMP_STREAM_DEFAULT, SAPI_POST_BLOCK_SIZE, PG(upload_tmp_dir));
		php_stream_truncate_set_size(SG(request_info).request_body, 0);
		php_stream_write(SG(request_info).request_body, Z_STRVAL_PP(zvpp), Z_STRLEN_PP(zvpp));
	}

	if (zend_hash_find(ht, "cwd", sizeof("cwd"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_STRING) {
		if (VCWD_CHDIR(Z_STRVAL_PP(zvpp)) == SUCCESS) {
			if (BG(CurrentStatFile) && !IS_ABSOLUTE_PATH(BG(CurrentStatFile), strlen(BG(CurrentStatFile)))) {
				efree(BG(CurrentStatFile));
				BG(CurrentStatFile) = NULL;
			}
			if (BG(CurrentLStatFile) && !IS_ABSOLUTE_PATH(BG(CurrentLStatFile), strlen(BG(CurrentLStatFile)))) {
				efree(BG(CurrentLStatFile));
				BG(CurrentLStatFile) = NULL;
			}
		}
	}

	if (zend_hash_find(ht, "sapi_name", sizeof("sapi_name"), (void **) &zvpp) == SUCCESS && (Z_TYPE_PP(zvpp) == IS_STRING || Z_TYPE_PP(zvpp) == IS_NULL)) {
		if (sapi_module.name) {
			efree(sapi_module.name);
		}
		if (Z_TYPE_PP(zvpp) == IS_STRING) {
			sapi_module.name = estrndup(Z_STRVAL_PP(zvpp), Z_STRLEN_PP(zvpp));
		} else {
			sapi_module.name = NULL;
		}
	}

	if (zend_hash_find(ht, "modules", sizeof("modules"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		HashPosition position;
		phpdbg_intersect_ptr pos;
		zval **module;
		zend_module_entry *mod;
		HashTable zv_registry;

		/* intersect modules, unregister mpdules loaded "too much", announce not yet registered modules (phpdbg_notice) */

		zend_hash_init(&zv_registry, zend_hash_num_elements(&zv_registry), 0, ZVAL_PTR_DTOR, 0);
		for (zend_hash_internal_pointer_reset_ex(&module_registry, &position);
		     zend_hash_get_current_data_ex(&module_registry, (void **) &mod, &position) == SUCCESS;
		     zend_hash_move_forward_ex(&module_registry, &position)) {
			zval **value = emalloc(sizeof(zval *));
			ALLOC_ZVAL(*value);
			ZVAL_STRING(*value, mod->name, 1);
			zend_hash_next_index_insert(&zv_registry, value, sizeof(zval *), NULL);
		}

		phpdbg_array_intersect_init(&pos, &zv_registry, Z_ARRVAL_PP(zvpp) TSRMLS_CC);
		do {
			int mode = phpdbg_array_intersect(&pos, &module);
			if (mode < 0) {
				// loaded module, but not needed
				zend_hash_del(&module_registry, Z_STRVAL_PP(module), Z_STRLEN_PP(module) + 1);
			} else if (mode > 0) {
				// not loaded module
				phpdbg_notice("The module %.*s isn't present in " PHPDBG_NAME ", you still can load via dl /path/to/module.so", Z_STRLEN_PP(module), Z_STRVAL_PP(module));
			}
		} while (module);

		zend_hash_clean(&zv_registry);
	}

	if (zend_hash_find(ht, "extensions", sizeof("extensions"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		zend_extension *extension;
		zend_llist_position pos;
		HashPosition hpos;
		zval **name, key;

		extension = (zend_extension *) zend_llist_get_first_ex(&zend_extensions, &pos);
		while (extension) {
			extension = (zend_extension *) zend_llist_get_next_ex(&zend_extensions, &pos);

			/* php_serach_array() body should be in some ZEND_API function */
			for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(zvpp), &hpos);
			     zend_hash_get_current_data_ex(Z_ARRVAL_PP(zvpp), (void **) &name, &hpos) == SUCCESS;
			     zend_hash_move_forward_ex(Z_ARRVAL_PP(zvpp), &hpos)) {
				if (Z_TYPE_PP(name) == IS_STRING && !zend_binary_strcmp(extension->name, strlen(extension->name), Z_STRVAL_PP(name), Z_STRLEN_PP(name))) {
					break;
				}
			}

			if (zend_hash_get_current_data_ex(Z_ARRVAL_PP(zvpp), (void **) &zvpp, &hpos) == FAILURE) {
				/* sigh, breaking the encapsulation, there aren't any functions manipulating the llist at the place of the zend_llist_position */
				zend_llist_element *elm = pos;
				if (elm->prev) {
					elm->prev->next = elm->next;
				} else {
					zend_extensions.head = elm->next;
				}
				if (elm->next) {
					elm->next->prev = elm->prev;
				} else {
					zend_extensions.tail = elm->prev;
				}
#if ZEND_EXTENSIONS_SUPPORT
				if (extension->shutdown) {
					extension->shutdown(extension);
				}
#endif
				if (zend_extensions.dtor) {
					zend_extensions.dtor(elm->data);
				}
				pefree(elm, zend_extensions.persistent);
				zend_extensions.count--;
			} else {
				zend_hash_get_current_key_zval_ex(Z_ARRVAL_PP(zvpp), &key, &hpos);
				if (Z_TYPE(key) == IS_LONG) {
					zend_hash_index_del(Z_ARRVAL_PP(zvpp), Z_LVAL(key));
				}
			}

			for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(zvpp), &hpos);
			     zend_hash_get_current_data_ex(Z_ARRVAL_PP(zvpp), (void **) &name, &hpos) == SUCCESS;
			     zend_hash_move_forward_ex(Z_ARRVAL_PP(zvpp), &hpos)) {
				phpdbg_notice("The (zend) extension %.*s isn't present in " PHPDBG_NAME ", you still can load via dl /path/to/extension.so", Z_STRLEN_PP(name), Z_STRVAL_PP(name));
			}
		}
	}

	zend_ini_deactivate(TSRMLS_C);

	if (zend_hash_find(ht, "systemini", sizeof("systemini"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		HashPosition position;
		zval **ini_entry;
		zend_ini_entry *original_ini;
		zval key;

		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(zvpp), &position);
		     zend_hash_get_current_data_ex(Z_ARRVAL_PP(zvpp), (void**) &ini_entry, &position) == SUCCESS;
		     zend_hash_move_forward_ex(Z_ARRVAL_PP(zvpp), &position)) {
			zend_hash_get_current_key_zval_ex(Z_ARRVAL_PP(zvpp), &key, &position);
			if (Z_TYPE(key) == IS_STRING) {
				if (Z_TYPE_PP(ini_entry) == IS_STRING) {
					if (zend_hash_find(EG(ini_directives), Z_STRVAL(key), Z_STRLEN(key) + 1, (void **) &original_ini) == SUCCESS) {
						if (!original_ini->on_modify || original_ini->on_modify(original_ini, Z_STRVAL_PP(ini_entry), Z_STRLEN_PP(ini_entry), original_ini->mh_arg1, original_ini->mh_arg2, original_ini->mh_arg3, ZEND_INI_STAGE_ACTIVATE TSRMLS_CC) == SUCCESS) {
							original_ini->value = Z_STRVAL_PP(ini_entry);
							original_ini->value_length = Z_STRLEN_PP(ini_entry);
							continue; /* don't free the string */
						}
					}
				}
				efree(Z_STRVAL(key));
			}
		}
	}

	if (zend_hash_find(ht, "userini", sizeof("userini"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		HashPosition position;
		zval **ini_entry;
		zval key;

		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(zvpp), &position);
		     zend_hash_get_current_data_ex(Z_ARRVAL_PP(zvpp), (void**) &ini_entry, &position) == SUCCESS;
		     zend_hash_move_forward_ex(Z_ARRVAL_PP(zvpp), &position)) {
			zend_hash_get_current_key_zval_ex(Z_ARRVAL_PP(zvpp), &key, &position);
			if (Z_TYPE(key) == IS_STRING) {
				if (Z_TYPE_PP(ini_entry) == IS_STRING) {
					zend_alter_ini_entry_ex(Z_STRVAL(key), Z_STRLEN(key) + 1, Z_STRVAL_PP(ini_entry), Z_STRLEN_PP(ini_entry), ZEND_INI_PERDIR, ZEND_INI_STAGE_HTACCESS, 1 TSRMLS_CC);
				}
				efree(Z_STRVAL(key));
			}
		}
	}

	zval_dtor(&zv);
	if (free_zv) {
		/* separate freeing to not dtor the symtable too, just the container zval... */
		efree(free_zv);
	}

	/* Remove and readd autoglobals */
/*	php_hash_environment(TSRMLS_C);
	zend_hash_apply(CG(auto_globals), (apply_func_t) phpdbg_remove_rearm_autoglobals TSRMLS_CC);
	php_startup_auto_globals(TSRMLS_C);
	zend_activate_auto_globals(TSRMLS_C);
*/
	/* Reapply raw input */
	/* ??? */
}

#endif /* PHPDBG_WEBDATA_TRANSFER_H */
