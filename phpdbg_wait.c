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

#include "phpdbg_wait.h"
#include "phpdbg_prompt.h"
#include "ext/json/JSON_parser.h"
#include "ext/standard/basic_functions.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);
ZEND_EXTERN_MODULE_GLOBALS(json);

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


static int phpdbg_dearm_autoglobals(zend_auto_global *auto_global TSRMLS_DC) {
	if (auto_global->name_len != sizeof("GLOBALS") - 1 || memcmp(auto_global->name, "GLOBALS", sizeof("GLOBALS") - 1)) {
		auto_global->armed = 0;
	}

	return ZEND_HASH_APPLY_KEEP;
}

typedef struct {
	HashTable *ht[2];
	HashPosition pos[2];
} phpdbg_intersect_ptr;

static int phpdbg_array_data_compare(const void *a, const void *b TSRMLS_DC) {
	Bucket *f, *s;
	zval result;
	zval *first, *second;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	first = *((zval **) f->pData);
	second = *((zval **) s->pData);

	if (string_compare_function(&result, first, second TSRMLS_CC) == FAILURE) {
		return 0;
	}

	if (Z_LVAL(result) < 0) {
		return -1;
	} else if (Z_LVAL(result) > 0) {
		return 1;
	}

	return 0;
}

static void phpdbg_array_intersect_init(phpdbg_intersect_ptr *info, HashTable *ht1, HashTable *ht2 TSRMLS_DC) {
	info->ht[0] = ht1;
	info->ht[1] = ht2;

	zend_hash_sort(info->ht[0], zend_qsort, (compare_func_t) phpdbg_array_data_compare, 0 TSRMLS_CC);
	zend_hash_sort(info->ht[1], zend_qsort, (compare_func_t) phpdbg_array_data_compare, 0 TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(info->ht[0], &info->pos[0]);
	zend_hash_internal_pointer_reset_ex(info->ht[1], &info->pos[1]);
}

/* -1 => first array, 0 => both arrays equal, 1 => second array */
static int phpdbg_array_intersect(phpdbg_intersect_ptr *info, zval ***ptr) {
	int ret;
	zval **zvpp[2];
	int invalid = !info->ht[0] + !info->ht[1];

	if (invalid > 0) {
		invalid = !info->ht[0];

		if (zend_hash_get_current_data_ex(info->ht[invalid], (void **) ptr, &info->pos[invalid]) == FAILURE) {
			*ptr = NULL;
			return 0;
		}

		zend_hash_move_forward_ex(info->ht[invalid], &info->pos[invalid]);

		return invalid ? 1 : -1;
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

void phpdbg_webdata_decompress(char *msg, int len TSRMLS_DC) {
#ifdef HAVE_JSON
	zval *free_zv = NULL;
	zval zv, **zvpp;
	HashTable *ht;
	php_json_decode(&zv, msg, len, 1, 1000 /* enough */ TSRMLS_CC);

	if (JSON_G(error_code) != PHP_JSON_ERROR_NONE) {
		phpdbg_error("wait", "type=\"invaliddata\" import=\"fail\"", "Malformed JSON was sent to this socket, arborting");
		return;
	}

	ht = Z_ARRVAL(zv);

	/* Reapply symbol table */
	if (zend_hash_find(ht, "GLOBALS", sizeof("GLOBALS"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		{
			zval **srv;
			if (zend_hash_find(Z_ARRVAL_PP(zvpp), "_SERVER", sizeof("_SERVER"), (void **) &srv) == SUCCESS && Z_TYPE_PP(srv) == IS_ARRAY) {
				zval **script;
				if (zend_hash_find(Z_ARRVAL_PP(srv), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void **) &script) == SUCCESS && Z_TYPE_PP(script) == IS_STRING) {
					phpdbg_param_t param;
					param.str = Z_STRVAL_PP(script);
					PHPDBG_COMMAND_HANDLER(exec)(&param TSRMLS_CC);
				}
			}
		}

		PG(auto_globals_jit) = 0;
		zend_hash_apply(CG(auto_globals), (apply_func_t) phpdbg_dearm_autoglobals TSRMLS_CC);

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
		if (PHPDBG_G(sapi_name_ptr)) {
			free(PHPDBG_G(sapi_name_ptr));
		}
		if (Z_TYPE_PP(zvpp) == IS_STRING) {
			PHPDBG_G(sapi_name_ptr) = sapi_module.name = strdup(Z_STRVAL_PP(zvpp));
		} else {
			PHPDBG_G(sapi_name_ptr) = sapi_module.name = NULL;
		}
	}

	if (zend_hash_find(ht, "modules", sizeof("modules"), (void **) &zvpp) == SUCCESS && Z_TYPE_PP(zvpp) == IS_ARRAY) {
		HashPosition position;
		phpdbg_intersect_ptr pos;
		zval **module;
		zend_module_entry *mod;
		HashTable zv_registry;

		/* intersect modules, unregister modules loaded "too much", announce not yet registered modules (phpdbg_notice) */

		zend_hash_init(&zv_registry, zend_hash_num_elements(&module_registry), 0, ZVAL_PTR_DTOR, 0);
		for (zend_hash_internal_pointer_reset_ex(&module_registry, &position);
		     zend_hash_get_current_data_ex(&module_registry, (void **) &mod, &position) == SUCCESS;
		     zend_hash_move_forward_ex(&module_registry, &position)) {
			if (mod->name) {
				zval **value = emalloc(sizeof(zval *));
				MAKE_STD_ZVAL(*value);
				ZVAL_STRING(*value, mod->name, 1);
				zend_hash_next_index_insert(&zv_registry, value, sizeof(zval *), NULL);
			}
		}

		phpdbg_array_intersect_init(&pos, &zv_registry, Z_ARRVAL_PP(zvpp) TSRMLS_CC);
		do {
			int mode = phpdbg_array_intersect(&pos, &module);
			if (mode < 0) {
				// loaded module, but not needed
				if (strcmp(PHPDBG_NAME, Z_STRVAL_PP(module))) {
					zend_hash_del(&module_registry, Z_STRVAL_PP(module), Z_STRLEN_PP(module) + 1);
				}
			} else if (mode > 0) {
				// not loaded module
				if (!sapi_module.name || strcmp(sapi_module.name, Z_STRVAL_PP(module))) {
					phpdbg_notice("wait", "missingmodule=\"%.*s\"", "The module %.*s isn't present in " PHPDBG_NAME ", you still can load via dl /path/to/module/%.*s.so", Z_STRLEN_PP(module), Z_STRVAL_PP(module), Z_STRLEN_PP(module), Z_STRVAL_PP(module));
				}
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
				phpdbg_notice("wait", "missingextension=\"%.*s\"", "The Zend extension %.*s isn't present in " PHPDBG_NAME ", you still can load via dl /path/to/extension.so", Z_STRLEN_PP(name), Z_STRVAL_PP(name));
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
							if (original_ini->modified && original_ini->orig_value != original_ini->value) {
								efree(original_ini->value);
							}
							original_ini->value = Z_STRVAL_PP(ini_entry);
							original_ini->value_length = Z_STRLEN_PP(ini_entry);
							Z_TYPE_PP(ini_entry) = IS_NULL; /* don't free the value */
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

	/* Reapply raw input */
	/* ??? */
#endif
}

PHPDBG_COMMAND(wait) /* {{{ */
{
#ifdef HAVE_JSON
	struct sockaddr_un local, remote;
	int rlen, sr, sl;
	unlink(PHPDBG_G(socket_path));
	if (PHPDBG_G(socket_server_fd) == -1) {
		int len;
		PHPDBG_G(socket_server_fd) = sl = socket(AF_UNIX, SOCK_STREAM, 0);

		local.sun_family = AF_UNIX;
		strcpy(local.sun_path, PHPDBG_G(socket_path));
		len = strlen(local.sun_path) + sizeof(local.sun_family);
		if (bind(sl, (struct sockaddr *)&local, len) == -1) {
			phpdbg_error("wait", "type=\"nosocket\" import=\"fail\"", "Unable to connect to UNIX domain socket at %s defined by phpdbg.path ini setting", PHPDBG_G(socket_path));
			return FAILURE;
		}

		chmod(PHPDBG_G(socket_path), 0666);

		listen(sl, 2);
	} else {
		sl = PHPDBG_G(socket_server_fd);
	}

	rlen = sizeof(remote);
	sr = accept(sl, (struct sockaddr *) &remote, (socklen_t *) &rlen);

	char msglen[5];
	int recvd = 4;

	do {
		recvd -= recv(sr, &(msglen[4 - recvd]), recvd, 0);
	} while (recvd > 0);

	recvd = *(size_t *) msglen;
	char *data = emalloc(recvd);

	do {
		recvd -= recv(sr, &(data[(*(int *) msglen) - recvd]), recvd, 0);
	} while (recvd > 0);

	phpdbg_webdata_decompress(data, *(int *) msglen TSRMLS_CC);

	if (PHPDBG_G(socket_fd) != -1) {
		close(PHPDBG_G(socket_fd));
	}
	PHPDBG_G(socket_fd) = sr;

	efree(data);

	phpdbg_notice("wait", "import=\"success\"", "Successfully imported request data, stopped before executing");

	return SUCCESS;
#endif
} /* }}} */
