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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg_wait.h"
#include "phpdbg_prompt.h"
#include "ext/standard/php_var.h"
#include "ext/standard/basic_functions.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

static void phpdbg_rebuild_http_globals_array(int type, const char *name) {
	zval *zvp;
	if (Z_TYPE(PG(http_globals)[type]) != IS_UNDEF) {
		zval_dtor(&PG(http_globals)[type]);
	}
	if ((zvp = zend_hash_str_find(&EG(symbol_table), name, strlen(name)))) {
		Z_ADDREF_P(zvp);
		PG(http_globals)[type] = *zvp;
	}
}


static int phpdbg_dearm_autoglobals(zend_auto_global *auto_global) {
	if (ZSTR_LEN(auto_global->name) != sizeof("GLOBALS") - 1 || memcmp(ZSTR_VAL(auto_global->name), "GLOBALS", sizeof("GLOBALS") - 1)) {
		auto_global->armed = 0;
	}

	return ZEND_HASH_APPLY_KEEP;
}

typedef struct {
	HashTable *ht[2];
	HashPosition pos[2];
} phpdbg_intersect_ptr;

static int phpdbg_array_data_compare(const void *a, const void *b) {
	Bucket *f, *s;
	int result;
	zval *first, *second;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	first = &f->val;
	second = &s->val;

	result = string_compare_function(first, second);

	if (result < 0) {
		return -1;
	} else if (result > 0) {
		return 1;
	}

	return 0;
}

static void phpdbg_array_intersect_init(phpdbg_intersect_ptr *info, HashTable *ht1, HashTable *ht2) {
	info->ht[0] = ht1;
	info->ht[1] = ht2;

	zend_hash_sort(info->ht[0], (compare_func_t) phpdbg_array_data_compare, 0);
	zend_hash_sort(info->ht[1], (compare_func_t) phpdbg_array_data_compare, 0);

	zend_hash_internal_pointer_reset_ex(info->ht[0], &info->pos[0]);
	zend_hash_internal_pointer_reset_ex(info->ht[1], &info->pos[1]);
}

/* -1 => first array, 0 => both arrays equal, 1 => second array */
static int phpdbg_array_intersect(phpdbg_intersect_ptr *info, zval **ptr) {
	int ret;
	zval *zvp[2];
	int invalid = !info->ht[0] + !info->ht[1];

	if (invalid > 0) {
		invalid = !info->ht[0];

		if (!(*ptr = zend_hash_get_current_data_ex(info->ht[invalid], &info->pos[invalid]))) {
			return 0;
		}

		zend_hash_move_forward_ex(info->ht[invalid], &info->pos[invalid]);

		return invalid ? 1 : -1;
	}

	if (!(zvp[0] = zend_hash_get_current_data_ex(info->ht[0], &info->pos[0]))) {
		info->ht[0] = NULL;
		return phpdbg_array_intersect(info, ptr);
	}
	if (!(zvp[1] = zend_hash_get_current_data_ex(info->ht[1], &info->pos[1]))) {
		info->ht[1] = NULL;
		return phpdbg_array_intersect(info, ptr);
	}

	ret = zend_binary_zval_strcmp(zvp[0], zvp[1]);

	if (ret <= 0) {
		*ptr = zvp[0];
		zend_hash_move_forward_ex(info->ht[0], &info->pos[0]);
	}
	if (ret >= 0) {
		*ptr = zvp[1];
		zend_hash_move_forward_ex(info->ht[1], &info->pos[1]);
	}

	return ret;
}

void phpdbg_webdata_decompress(char *msg, int len) {
	zval *free_zv = NULL;
	zval zv, *zvp;
	HashTable *ht;
	php_unserialize_data_t var_hash;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (!php_var_unserialize(&zv, (const unsigned char **) &msg, (unsigned char *) msg + len, &var_hash)) {
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		phpdbg_error("wait", "type=\"invaliddata\" import=\"fail\"", "Malformed serialized was sent to this socket, arborting");
		return;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	ht = Z_ARRVAL(zv);

	/* Reapply symbol table */
	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("GLOBALS"))) && Z_TYPE_P(zvp) == IS_ARRAY) {
		{
			zval *srv;
			if ((srv = zend_hash_str_find(Z_ARRVAL_P(zvp), ZEND_STRL("_SERVER"))) && Z_TYPE_P(srv) == IS_ARRAY) {
				zval *script;
				if ((script = zend_hash_str_find(Z_ARRVAL_P(srv), ZEND_STRL("SCRIPT_FILENAME"))) && Z_TYPE_P(script) == IS_STRING) {
					phpdbg_param_t param;
					param.str = Z_STRVAL_P(script);
					PHPDBG_COMMAND_HANDLER(exec)(&param);
				}
			}
		}

		PG(auto_globals_jit) = 0;
		zend_hash_apply(CG(auto_globals), (apply_func_t) phpdbg_dearm_autoglobals);

		zend_hash_clean(&EG(symbol_table));
		EG(symbol_table) = *Z_ARR_P(zvp);

		/* Rebuild cookies, env vars etc. from GLOBALS (PG(http_globals)) */
		phpdbg_rebuild_http_globals_array(TRACK_VARS_POST, "_POST");
		phpdbg_rebuild_http_globals_array(TRACK_VARS_GET, "_GET");
		phpdbg_rebuild_http_globals_array(TRACK_VARS_COOKIE, "_COOKIE");
		phpdbg_rebuild_http_globals_array(TRACK_VARS_SERVER, "_SERVER");
		phpdbg_rebuild_http_globals_array(TRACK_VARS_ENV, "_ENV");
		phpdbg_rebuild_http_globals_array(TRACK_VARS_FILES, "_FILES");

		Z_ADDREF_P(zvp);
		free_zv = zvp;
	}

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("input"))) && Z_TYPE_P(zvp) == IS_STRING) {
		if (SG(request_info).request_body) {
			php_stream_close(SG(request_info).request_body);
		}
		SG(request_info).request_body = php_stream_temp_create_ex(TEMP_STREAM_DEFAULT, SAPI_POST_BLOCK_SIZE, PG(upload_tmp_dir));
		php_stream_truncate_set_size(SG(request_info).request_body, 0);
		php_stream_write(SG(request_info).request_body, Z_STRVAL_P(zvp), Z_STRLEN_P(zvp));
	}

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("cwd"))) && Z_TYPE_P(zvp) == IS_STRING) {
		if (VCWD_CHDIR(Z_STRVAL_P(zvp)) == SUCCESS) {
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

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("sapi_name"))) && (Z_TYPE_P(zvp) == IS_STRING || Z_TYPE_P(zvp) == IS_NULL)) {
		if (PHPDBG_G(sapi_name_ptr)) {
			free(PHPDBG_G(sapi_name_ptr));
		}
		if (Z_TYPE_P(zvp) == IS_STRING) {
			PHPDBG_G(sapi_name_ptr) = sapi_module.name = strdup(Z_STRVAL_P(zvp));
		} else {
			PHPDBG_G(sapi_name_ptr) = sapi_module.name = NULL;
		}
	}

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("modules"))) && Z_TYPE_P(zvp) == IS_ARRAY) {
		phpdbg_intersect_ptr pos;
		zval *module;
		zend_module_entry *mod;
		HashTable zv_registry;

		/* intersect modules, unregister modules loaded "too much", announce not yet registered modules (phpdbg_notice) */

		zend_hash_init(&zv_registry, zend_hash_num_elements(&module_registry), 0, ZVAL_PTR_DTOR, 0);
		ZEND_HASH_FOREACH_PTR(&module_registry, mod) {
			if (mod->name) {
				zval value;
				ZVAL_NEW_STR(&value, zend_string_init(mod->name, strlen(mod->name), 0));
				zend_hash_next_index_insert(&zv_registry, &value);
			}
		} ZEND_HASH_FOREACH_END();

		phpdbg_array_intersect_init(&pos, &zv_registry, Z_ARRVAL_P(zvp));
		do {
			int mode = phpdbg_array_intersect(&pos, &module);
			if (mode < 0) {
				// loaded module, but not needed
				if (strcmp(PHPDBG_NAME, Z_STRVAL_P(module))) {
					zend_hash_del(&module_registry, Z_STR_P(module));
				}
			} else if (mode > 0) {
				// not loaded module
				if (!sapi_module.name || strcmp(sapi_module.name, Z_STRVAL_P(module))) {
					phpdbg_notice("wait", "missingmodule=\"%.*s\"", "The module %.*s isn't present in " PHPDBG_NAME ", you still can load via dl /path/to/module/%.*s.so", (int) Z_STRLEN_P(module), Z_STRVAL_P(module), (int) Z_STRLEN_P(module), Z_STRVAL_P(module));
				}
			}
		} while (module);

		zend_hash_clean(&zv_registry);
	}

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("extensions"))) && Z_TYPE_P(zvp) == IS_ARRAY) {
		zend_extension *extension;
		zend_llist_position pos;
		zval *name = NULL;
		zend_string *strkey;

		extension = (zend_extension *) zend_llist_get_first_ex(&zend_extensions, &pos);
		while (extension) {
			extension = (zend_extension *) zend_llist_get_next_ex(&zend_extensions, &pos);

			/* php_serach_array() body should be in some ZEND_API function... */
			ZEND_HASH_FOREACH_STR_KEY_PTR(Z_ARRVAL_P(zvp), strkey, name) {
				if (Z_TYPE_P(name) == IS_STRING && !zend_binary_strcmp(extension->name, strlen(extension->name), Z_STRVAL_P(name), Z_STRLEN_P(name))) {
					break;
				}
				name = NULL;
			} ZEND_HASH_FOREACH_END();

			if (name) {
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
				zend_hash_del(Z_ARRVAL_P(zvp), strkey);
			}
		}

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zvp), name) {
			if (Z_TYPE_P(name) == IS_STRING) {
				phpdbg_notice("wait", "missingextension=\"%.*s\"", "The Zend extension %.*s isn't present in " PHPDBG_NAME ", you still can load via dl /path/to/extension.so", (int) Z_STRLEN_P(name), Z_STRVAL_P(name));
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_ini_deactivate();

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("systemini"))) && Z_TYPE_P(zvp) == IS_ARRAY) {
		zval *ini_entry;
		zend_ini_entry *original_ini;
		zend_string *key;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zvp), key, ini_entry) {
			if (key && Z_TYPE_P(ini_entry) == IS_STRING) {
				if ((original_ini = zend_hash_find_ptr(EG(ini_directives), key))) {
					if (!original_ini->on_modify || original_ini->on_modify(original_ini, Z_STR_P(ini_entry), original_ini->mh_arg1, original_ini->mh_arg2, original_ini->mh_arg3, ZEND_INI_STAGE_ACTIVATE) == SUCCESS) {
						if (original_ini->modified && original_ini->orig_value != original_ini->value) {
							efree(original_ini->value);
						}
						original_ini->value = Z_STR_P(ini_entry);
						Z_ADDREF_P(ini_entry); /* don't free the string */
					}
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if ((zvp = zend_hash_str_find(ht, ZEND_STRL("userini"))) && Z_TYPE_P(zvp) == IS_ARRAY) {
		zval *ini_entry;
		zend_string *key;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zvp), key, ini_entry) {
			if (key && Z_TYPE_P(ini_entry) == IS_STRING) {
				zend_alter_ini_entry_ex(key, Z_STR_P(ini_entry), ZEND_INI_PERDIR, ZEND_INI_STAGE_HTACCESS, 1);
			}
		} ZEND_HASH_FOREACH_END();
	}

	zval_dtor(&zv);
	if (free_zv) {
		/* separate freeing to not dtor the symtable too, just the container zval... */
		efree(free_zv);
	}

	/* Reapply raw input */
	/* ??? */
}

PHPDBG_COMMAND(wait) /* {{{ */
{
#ifndef PHP_WIN32
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

	phpdbg_webdata_decompress(data, *(int *) msglen);

	if (PHPDBG_G(socket_fd) != -1) {
		close(PHPDBG_G(socket_fd));
	}
	PHPDBG_G(socket_fd) = sr;

	efree(data);

	phpdbg_notice("wait", "import=\"success\"", "Successfully imported request data, stopped before executing");
#endif

	return SUCCESS;
} /* }}} */
