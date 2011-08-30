/*
   +----------------------------------------------------------------------+
   | Yet Another Framework                                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Xinchen Hui  <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/
   
/* $Id$ */

#include "main/php_output.h"

#define VIEW_BUFFER_BLOCK_SIZE	4096
#define VIEW_BUFFER_SIZE_MASK 	4095

zend_class_entry *yaf_view_simple_ce;

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
struct _yaf_view_simple_buffer {
	char *buffer;
	unsigned long size;
	unsigned long len;
	struct _yaf_view_simple_buffer *prev;
};

typedef struct _yaf_view_simple_buffer yaf_view_simple_buffer;

typedef int(*yaf_body_write_func)(const char *str, uint str_length TSRMLS_DC);

/** {{{ MACROS
 */
#define YAF_REDIRECT_OUTPUT_BUFFER(seg) \
	do { \
		if (!YAF_G(owrite_handler)) { \
			YAF_G(owrite_handler) = OG(php_body_write); \
		} \
		OG(php_body_write) = yaf_view_simple_render_write; \
		old_scope = EG(scope); \
		EG(scope) = yaf_view_simple_ce; \
		seg = (yaf_view_simple_buffer *)emalloc(sizeof(yaf_view_simple_buffer)); \
		memset(seg, 0, sizeof(yaf_view_simple_buffer)); \
		seg->prev  	 = YAF_G(buffer);\
		YAF_G(buffer) = seg; \
		YAF_G(buf_nesting)++;\
	} while (0)

#define YAF_RESTORE_OUTPUT_BUFFER(seg) \
	do { \
		OG(php_body_write) 	= (yaf_body_write_func)YAF_G(owrite_handler); \
		EG(scope) 			= old_scope; \
		YAF_G(buffer)  		= seg->prev; \
		if (!(--YAF_G(buf_nesting))) { \
			if (YAF_G(buffer)) { \
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Yaf output buffer collapsed"); \
			} else { \
				YAF_G(owrite_handler) = NULL; \
			} \
		} \
		if (seg->size) { \
			efree(seg->buffer); \
		} \
		efree(seg); \
	} while (0)
/* }}} */
#endif

/** {{{ ARG_INFO */
ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tempalte_dir)
	ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_assign_by_ref_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(1, value)
ZEND_END_ARG_INFO();
/* }}} */

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
/** {{{ static int yaf_view_simple_render_write(const char *str, uint str_length TSRMLS_DC)
*/
static int yaf_view_simple_render_write(const char *str, uint str_length TSRMLS_DC) {
	char *target;
	yaf_view_simple_buffer *buffer = YAF_G(buffer);

	if (!buffer->size) {
		buffer->size   = (str_length | VIEW_BUFFER_SIZE_MASK) + 1;
		buffer->len	   = str_length;
		buffer->buffer = emalloc(buffer->size);
		target = buffer->buffer;
	} else {
		size_t len = buffer->len + str_length;

		if (buffer->size < len + 1) {
			buffer->size   = (len | VIEW_BUFFER_SIZE_MASK) + 1;
			buffer->buffer = erealloc(buffer->buffer, buffer->size);
			if (!buffer->buffer) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Yaf output buffer collapsed");
			}
		}
		
		target = buffer->buffer + buffer->len;
		buffer->len = len;
	}

	memcpy(target, str, str_length);
	target[str_length] = '\0';

	return str_length;
}
/* }}} */
#endif

static int yaf_view_simple_valid_var_name(char *var_name, int len) /* {{{ */
{
	int i, ch;

	if (!var_name)
		return 0;

	/* These are allowed as first char: [a-zA-Z_\x7f-\xff] */
	ch = (int)((unsigned char *)var_name)[0];
	if (var_name[0] != '_' &&
			(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
			(ch < 97  /* a    */ || /* z    */ ch > 122) &&
			(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
	   ) {
		return 0;
	}

	/* And these as the rest: [a-zA-Z0-9_\x7f-\xff] */
	if (len > 1) {
		for (i = 1; i < len; i++) {
			ch = (int)((unsigned char *)var_name)[i];
			if (var_name[i] != '_' &&
					(ch < 48  /* 0    */ || /* 9    */ ch > 57)  &&
					(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
					(ch < 97  /* a    */ || /* z    */ ch > 122) &&
					(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
			   ) {	
				return 0;
			}
		}
	}
	return 1;
}
/* }}} */

/** {{{ static int yaf_view_simple_extract(zval *tpl_vars, zval *vars TSRMLS_DC) 
*/
static int yaf_view_simple_extract(zval *tpl_vars, zval *vars TSRMLS_DC) {
	zval **entry;
	char *var_name;
	long num_key;
	uint var_name_len;
	HashPosition pos;

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
	if (!EG(active_symbol_table)) {
		/*zend_rebuild_symbol_table(TSRMLS_C);*/
		return 1;
	}
#endif

	if (tpl_vars && Z_TYPE_P(tpl_vars) == IS_ARRAY) {
		for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(tpl_vars), &pos);
				zend_hash_get_current_data_ex(Z_ARRVAL_P(tpl_vars), (void **)&entry, &pos) == SUCCESS; 
				zend_hash_move_forward_ex(Z_ARRVAL_P(tpl_vars), &pos)) {
			if (zend_hash_get_current_key_ex(Z_ARRVAL_P(tpl_vars), &var_name, &var_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
				continue;
			}

			/* GLOBALS protection */
			if (var_name_len == sizeof("GLOBALS") && !strcmp(var_name, "GLOBALS")) {
				continue;
			}

			if (var_name_len == sizeof("this")  && !strcmp(var_name, "this") && EG(scope) && EG(scope)->name_length != 0) {
				continue;
			}


			if (yaf_view_simple_valid_var_name(var_name, var_name_len - 1)) {
				ZEND_SET_SYMBOL_WITH_LENGTH(EG(active_symbol_table), var_name, var_name_len, 
						*entry, Z_REFCOUNT_P(*entry) + 1, 0 /**PZVAL_IS_REF(*entry)*/);
			}
		}
	}

	if (vars && Z_TYPE_P(vars) == IS_ARRAY) {
		for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(vars), &pos);
				zend_hash_get_current_data_ex(Z_ARRVAL_P(vars), (void **)&entry, &pos) == SUCCESS; 
				zend_hash_move_forward_ex(Z_ARRVAL_P(vars), &pos)) {
			if (zend_hash_get_current_key_ex(Z_ARRVAL_P(vars), &var_name, &var_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
				continue;
			}

			/* GLOBALS protection */
			if (var_name_len == sizeof("GLOBALS") && !strcmp(var_name, "GLOBALS")) {
				continue;
			}

			if (var_name_len == sizeof("this")  && !strcmp(var_name, "this") && EG(scope) && EG(scope)->name_length != 0) {
				continue;
			}

			if (yaf_view_simple_valid_var_name(var_name, var_name_len - 1)) {
				ZEND_SET_SYMBOL_WITH_LENGTH(EG(active_symbol_table), var_name, var_name_len, 
						*entry, Z_REFCOUNT_P(*entry) + 1, 0 /**PZVAL_IS_REF(*entry)*/);
			}
		}
	}

	return 1;
}
/* }}} */

/** {{{ yaf_view_t * yaf_view_simple_instance(yaf_view_t *view, zval *tpl_dir, zval *options TSRMLS_DC)
*/
yaf_view_t * yaf_view_simple_instance(yaf_view_t *view, zval *tpl_dir, zval *options TSRMLS_DC) {
	zval *instance, *tpl_vars;

	instance = view;
	if (!instance) {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, yaf_view_simple_ce);
	}

	MAKE_STD_ZVAL(tpl_vars);
	array_init(tpl_vars);
	zend_update_property(yaf_view_simple_ce, instance, ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), tpl_vars TSRMLS_CC);
	zval_ptr_dtor(&tpl_vars);

	if (tpl_dir && Z_TYPE_P(tpl_dir) == IS_STRING && IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl_dir), Z_STRLEN_P(tpl_dir))) {
		zend_update_property(yaf_view_simple_ce, instance, ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR), tpl_dir TSRMLS_CC);
	}

	return instance;
}
/* }}} */

/** {{{ int yaf_view_simple_render(yaf_view_t *view, zval *tpl, zval * vars, zval *ret TSRMLS_DC)
*/
int yaf_view_simple_render(yaf_view_t *view, zval *tpl, zval * vars, zval *ret TSRMLS_DC) {
	zval *tpl_vars;
	char *script; 
	uint len; 

	HashTable *calling_symbol_table;
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
	zend_class_entry *old_scope;
	yaf_view_simple_buffer *buffer;
#endif

	ZVAL_NULL(ret);

	tpl_vars = zend_read_property(yaf_view_simple_ce, view, ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);
	if (EG(active_symbol_table)) {
		calling_symbol_table = EG(active_symbol_table);
	} else {
		calling_symbol_table = NULL;
	}

	ALLOC_HASHTABLE(EG(active_symbol_table));
	zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

	(void)yaf_view_simple_extract(tpl_vars, vars TSRMLS_CC);

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
	YAF_REDIRECT_OUTPUT_BUFFER(buffer);
#else 
	if (php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == FAILURE) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "failed to create buffer");
		return 0;
	}
#endif

	if (IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl), Z_STRLEN_P(tpl))) {
		script 	= Z_STRVAL_P(tpl);
		len 	= Z_STRLEN_P(tpl);
		if (yaf_loader_compose(script, len + 1, 0 TSRMLS_CC) == 0) {
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
			YAF_RESTORE_OUTPUT_BUFFER(buffer);
#else
			php_output_end(TSRMLS_C);
#endif
			if (calling_symbol_table) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = calling_symbol_table;
			}

			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW TSRMLS_CC, "Unable to find template %s", script);
			return 0;
		}
	} else {
		zval *tpl_dir = zend_read_property(yaf_view_simple_ce, view, ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR), 1 TSRMLS_CC);

		if (ZVAL_IS_NULL(tpl_dir)) {
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
			YAF_RESTORE_OUTPUT_BUFFER(buffer);
#else
			php_output_end(TSRMLS_C);
#endif

			if (calling_symbol_table) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = calling_symbol_table;
			}

			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW TSRMLS_CC,
				   	"Could not determine the view script path, you should call %s::setScriptPath to specific it", 
					yaf_view_simple_ce->name);
			return 0;
		}

		len = spprintf(&script, 0, "%s%c%s", Z_STRVAL_P(tpl_dir), DEFAULT_SLASH, Z_STRVAL_P(tpl));

		if (yaf_loader_compose(script, len + 1, 0 TSRMLS_CC) == 0) {
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
			YAF_RESTORE_OUTPUT_BUFFER(buffer);
#else
			php_output_end(TSRMLS_C);
#endif
			if (calling_symbol_table) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = calling_symbol_table;
			}

			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW TSRMLS_CC, "Unable to find template %s" , script);
			efree(script);
			return 0;
		}
		efree(script);
	}

	if (calling_symbol_table) {
		zend_hash_destroy(EG(active_symbol_table));
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = calling_symbol_table;
	}

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
	if (buffer->len) {
		ZVAL_STRINGL(ret, buffer->buffer, buffer->len, 1);
	}
#else
	if (php_output_get_contents(ret TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fetch ob content");
	}
#endif

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
			YAF_RESTORE_OUTPUT_BUFFER(buffer);
#else
			php_output_end(TSRMLS_C);
#endif
	return 1;
}
/* }}} */

/** {{{ int yaf_view_simple_display(yaf_view_t *view, zval *tpl, zval * vars, zval *ret TSRMLS_DC)
*/
int yaf_view_simple_display(yaf_view_t *view, zval *tpl, zval *vars, zval *ret TSRMLS_DC) {
	zval *tpl_vars;
	char *script;
	uint len;

	zend_class_entry *old_scope;
	HashTable *calling_symbol_table;

	ZVAL_NULL(ret);

	tpl_vars = zend_read_property(yaf_view_simple_ce, view, ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);
	if (EG(active_symbol_table)) {
		calling_symbol_table = EG(active_symbol_table);
	} else {
		calling_symbol_table = NULL;
	}

	ALLOC_HASHTABLE(EG(active_symbol_table));
	zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

	(void)yaf_view_simple_extract(tpl_vars, vars TSRMLS_CC);

	old_scope = EG(scope);
	EG(scope) = yaf_view_simple_ce;

	if (IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl), Z_STRLEN_P(tpl))) {
		script 	= Z_STRVAL_P(tpl);
		len 	= Z_STRLEN_P(tpl);
		if (yaf_loader_compose(script, len + 1, 0 TSRMLS_CC) == 0) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW TSRMLS_CC, "Unable to find template %s" , script);
			EG(scope) = old_scope;
			if (calling_symbol_table) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = calling_symbol_table;
			}
			return 0;
		}
	} else {
		zval *tpl_dir = zend_read_property(yaf_view_simple_ce, view, ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR), 1 TSRMLS_CC);

		if (ZVAL_IS_NULL(tpl_dir)) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW TSRMLS_CC, 
					"Could not determine the view script path, you should call %s::setScriptPath to specific it", yaf_view_simple_ce->name);
			EG(scope) = old_scope;
			if (calling_symbol_table) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = calling_symbol_table;
			}
			return 0;
		}

		len = spprintf(&script, 0, "%s%c%s", Z_STRVAL_P(tpl_dir), DEFAULT_SLASH, Z_STRVAL_P(tpl));
		if (yaf_loader_compose(script, len + 1, 0 TSRMLS_CC) == 0) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW TSRMLS_CC, "Unable to find template %s", script);
			efree(script);
			EG(scope) = old_scope;
			if (calling_symbol_table) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = calling_symbol_table;
			}
			return 0;
		}
		efree(script);
	}

	EG(scope) = old_scope;
	if (calling_symbol_table) {
		zend_hash_destroy(EG(active_symbol_table));
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = calling_symbol_table;
	}

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::__construct(string $tpl_dir, array $options = NULL) 
*/
PHP_METHOD(yaf_view_simple, __construct) {
	zval *tpl_dir, *options = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &tpl_dir, &options) == FAILURE) {
		return;
	}

	yaf_view_simple_instance(getThis(), tpl_dir, options TSRMLS_CC);
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::__isset($name)
*/
PHP_METHOD(yaf_view_simple, __isset) {
	char *name;
	uint len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		zval *tpl_vars = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(tpl_vars), name, len + 1)); 
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::setScriptPath(string $tpl_dir)
*/
PHP_METHOD(yaf_view_simple, setScriptPath) {
	zval *tpl_dir;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &tpl_dir) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(tpl_dir) == IS_STRING && IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl_dir), Z_STRLEN_P(tpl_dir))) {
		zend_update_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR), tpl_dir TSRMLS_CC);
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::getScriptPath(void)
*/
PHP_METHOD(yaf_view_simple, getScriptPath) {
	zval *tpl_dir = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR), 1 TSRMLS_CC);
	RETURN_ZVAL(tpl_dir, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::compose(string $script, zval *args)
*/
PHP_METHOD(yaf_view_simple, compose) {
	char *script;
	uint len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &script, &len) == FAILURE) {
		return;
	}

	if (!len) {
		RETURN_FALSE;
	}

}
/* }}} */

/** {{{ proto public Yaf_View_Simple::assign(mixed $value, mixed $value = null)
*/
PHP_METHOD(yaf_view_simple, assign) {
	uint argc = ZEND_NUM_ARGS();
	zval *tpl_vars = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);
	if (argc == 1) {
		zval *value;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
			return;
		}    

		if (Z_TYPE_P(value) == IS_ARRAY) {
			zend_hash_copy(Z_ARRVAL_P(tpl_vars), Z_ARRVAL_P(value), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
			RETURN_TRUE;
		} 
		RETURN_FALSE;
	} else if (argc == 2) {
		zval *value;
		char *name;
		uint len;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &value) == FAILURE) {
			return;
		}     

		Z_ADDREF_P(value);
		if (zend_hash_update(Z_ARRVAL_P(tpl_vars), name, len + 1, &value, sizeof(zval *), NULL) == SUCCESS) {
			RETURN_TRUE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::assignRef(mixed $value, mixed $value)
*/
PHP_METHOD(yaf_view_simple, assignRef) {
	char * name; int len;
	zval * value, * tpl_vars;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	} 

	tpl_vars = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);

	Z_ADDREF_P(value);
	if (zend_hash_update(Z_ARRVAL_P(tpl_vars), name, len + 1, &value, sizeof(zval *), NULL) == SUCCESS) {
		RETURN_TRUE;
	}		
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::get($name)
*/
PHP_METHOD(yaf_view_simple, get) {
	char *name;
	uint len;
	zval *tpl_vars, **ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	}    

	tpl_vars = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);

	if (tpl_vars && Z_TYPE_P(tpl_vars) == IS_ARRAY) {
		if (zend_hash_find(Z_ARRVAL_P(tpl_vars), name, len + 1, (void **) &ret) == SUCCESS) {
			RETURN_ZVAL(*ret, 1, 0);
		}
	} 

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::render(string $tpl, array $vars = NULL)
*/
PHP_METHOD(yaf_view_simple, render) {
	zval *tpl, *vars = NULL, *tpl_vars;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &tpl, &vars) == FAILURE) {
		return;
	}

	tpl_vars = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);
	if (!yaf_view_simple_render(getThis(), tpl, vars, return_value TSRMLS_CC)) {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::display(string $tpl, array $vars = NULL)
*/
PHP_METHOD(yaf_view_simple, display) {
	zval *tpl, *tpl_vars, *vars = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &tpl, &vars) == FAILURE) {
		return;
	}

	tpl_vars = zend_read_property(yaf_view_simple_ce, getThis(), ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), 1 TSRMLS_CC);
	if (!yaf_view_simple_display(getThis(), tpl, vars, return_value TSRMLS_CC)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/** {{{ yaf_view_simple_methods 
*/
zend_function_entry yaf_view_simple_methods[] = {
	PHP_ME(yaf_view_simple, __construct, yaf_view_simple_construct_arginfo, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, __isset, yaf_view_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, get, yaf_view_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, assign, yaf_view_assign_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, render, yaf_view_render_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, display, yaf_view_display_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, assignRef, yaf_view_simple_assign_by_ref_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, setScriptPath, yaf_view_setpath_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, getScriptPath, yaf_view_getpath_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_view_simple, __get, get, yaf_view_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_view_simple, __set, assign, yaf_view_assign_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION 
*/
YAF_STARTUP_FUNCTION(view_simple) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_View_Simple", "Yaf\\View\\Simple", yaf_view_simple_methods);
	yaf_view_simple_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	zend_declare_property_null(yaf_view_simple_ce, YAF_STRL(YAF_VIEW_PROPERTY_NAME_TPLVARS), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_view_simple_ce, YAF_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR),  ZEND_ACC_PROTECTED TSRMLS_CC);

	yaf_view_simple_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_class_implements(yaf_view_simple_ce TSRMLS_CC, 1, yaf_view_interface_ce);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

