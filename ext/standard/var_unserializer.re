#include "php.h"
#include "ext/standard/php_var.h"
#include "php_incomplete_class.h"

/* {{{ reference-handling for unserializer: var_* */
#define VAR_ENTRIES_MAX 1024

typedef struct {
	zval *data[VAR_ENTRIES_MAX];
	int used_slots;
	void *next;
} var_entries;

static inline void var_push(php_unserialize_data_t *var_hashx, zval **rval)
{
	var_entries *var_hash = var_hashx->first, *prev = NULL;

	while (var_hash && var_hash->used_slots == VAR_ENTRIES_MAX) {
		prev = var_hash;
		var_hash = var_hash->next;
	}

	if (!var_hash) {
		var_hash = emalloc(sizeof(var_entries));
		var_hash->used_slots = 0;
		var_hash->next = 0;

		if (!var_hashx->first)
			var_hashx->first = var_hash;
		else
			prev->next = var_hash;
	}

	var_hash->data[var_hash->used_slots++] = *rval;
}

PHPAPI void var_replace(php_unserialize_data_t *var_hashx, zval *ozval, zval **nzval)
{
	int i;
	var_entries *var_hash = var_hashx->first;
	
	while (var_hash) {
		for (i = 0; i < var_hash->used_slots; i++) {
			if (var_hash->data[i] == ozval) {
				var_hash->data[i] = *nzval;
				return;
			}
		}
		var_hash = var_hash->next;
	}
}

static int var_access(php_unserialize_data_t *var_hashx, int id, zval ***store)
{
	var_entries *var_hash = var_hashx->first;
	
	while (id >= VAR_ENTRIES_MAX && var_hash && var_hash->used_slots == VAR_ENTRIES_MAX) {
		var_hash = var_hash->next;
		id -= VAR_ENTRIES_MAX;
	}

	if (!var_hash) return !SUCCESS;

	if (id >= var_hash->used_slots) return !SUCCESS;

	*store = &var_hash->data[id];

	return SUCCESS;
}

PHPAPI void var_destroy(php_unserialize_data_t *var_hashx)
{
	void *next;
	var_entries *var_hash = var_hashx->first;
	
	while (var_hash) {
		next = var_hash->next;
		efree(var_hash);
		var_hash = next;
	}
}

/* }}} */

#define YYFILL(n) do { } while (0)
#define YYCTYPE unsigned char
#define YYCURSOR cursor
#define YYLIMIT limit
#define YYMARKER marker


/*!re2c
iv = [+-]? [0-9]+;
nv = [+-]? ([0-9]* "." [0-9]+|[0-9]+ "." [0-9]*);
nvexp = (iv | nv) [eE] [+-]? iv;
any = [\000-\277];
*/



static inline int parse_iv2(const char *p, const char **q)
{
	char cursor;
	int result = 0;
	int neg = 0;

	switch (*p) {
		case '-':
			neg++;
			/* fall-through */
		case '+':
			p++;
	}
	
	while (1) {
		cursor = *p;
		if (cursor >= '0' && cursor <= '9') {
			result = result * 10 + cursor - '0';
		} else {
			break;
		}
		p++;
	}
	if (q) *q = p;
	if (neg) return -result;
	return result;
}

static inline int parse_iv(const char *p)
{
	return parse_iv2(p, NULL);
}

#define UNSERIALIZE_PARAMETER zval **rval, const char **p, const char *max, php_unserialize_data_t *var_hash TSRMLS_DC
#define UNSERIALIZE_PASSTHRU rval, p, max, var_hash TSRMLS_CC

static inline int process_nested_data(UNSERIALIZE_PARAMETER, HashTable *ht, int elements)
{
	while (elements-- > 0) {
		zval *key, *data;

		ALLOC_INIT_ZVAL(key);

		if (!php_var_unserialize(&key, p, max, NULL TSRMLS_CC)) {
			zval_dtor(key);
			FREE_ZVAL(key);
			return 0;
		}

		ALLOC_INIT_ZVAL(data);

		if (!php_var_unserialize(&data, p, max, var_hash TSRMLS_CC)) {
			zval_dtor(key);
			FREE_ZVAL(key);
			zval_dtor(data);
			FREE_ZVAL(data);
			return 0;
		}

		switch (Z_TYPE_P(key)) {
			case IS_LONG:
				zend_hash_index_update(ht, Z_LVAL_P(key), &data, sizeof(data), NULL);
				break;
			case IS_STRING:
				zend_hash_update(ht, Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, &data, sizeof(data), NULL);
				break;

		}
		
		zval_dtor(key);
		FREE_ZVAL(key);
	}

	return 1;
}

static inline int finish_nested_data(UNSERIALIZE_PARAMETER)
{
	if (*((*p)++) == '}') 
		return 1;

#if SOMETHING_NEW_MIGHT_LEAD_TO_CRASH_ENABLE_IF_YOU_ARE_BRAVE
	zval_ptr_dtor(rval);
#endif
	return 0;
}

static inline int object_common1(UNSERIALIZE_PARAMETER, zend_class_entry *ce)
{
	int elements;

	elements = parse_iv2((*p) + 2, p);

	(*p) += 2;
	
	object_init_ex(*rval, ce);
	return elements;
}

static inline int object_common2(UNSERIALIZE_PARAMETER, int elements)
{
	zval *retval_ptr = NULL;
	zval fname;

	if (!process_nested_data(UNSERIALIZE_PASSTHRU, Z_OBJPROP_PP(rval), elements)) {
		return 0;
	}

	INIT_PZVAL(&fname);
	ZVAL_STRINGL(&fname, "__wakeup", sizeof("__wakeup") - 1, 0);
	call_user_function_ex(CG(function_table), rval, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);

	if (retval_ptr)
		zval_ptr_dtor(&retval_ptr);

	return finish_nested_data(UNSERIALIZE_PASSTHRU);

}

PHPAPI int php_var_unserialize(UNSERIALIZE_PARAMETER)
{
	const unsigned char *cursor, *limit, *marker, *start;
	zval **rval_ref;

	limit = cursor = *p;
	
	if (var_hash && cursor[0] != 'R') {
		var_push(var_hash, rval);
	}

	start = cursor;

	
	
/*!re2c

"R:" iv ";"		{
	int id;

 	*p = YYCURSOR;
	if (!var_hash) return 0;

	id = parse_iv(start + 2) - 1;
	if (id == -1 || var_access(var_hash, id, &rval_ref) != SUCCESS) {
		return 0;
	}

	zval_ptr_dtor(rval);
	*rval = *rval_ref;
	(*rval)->refcount++;
	(*rval)->is_ref = 1;
	
	return 1;
}

"N;"	{
	*p = YYCURSOR;
	INIT_PZVAL(*rval);
	ZVAL_NULL(*rval);
	return 1;
}

"b:" iv ";"	{
	*p = YYCURSOR;
	INIT_PZVAL(*rval);
	ZVAL_BOOL(*rval, parse_iv(start + 2));
	return 1;
}

"i:" iv ";"	{
	*p = YYCURSOR;
	INIT_PZVAL(*rval);
	ZVAL_LONG(*rval, parse_iv(start + 2));
	return 1;
}

"d:" (iv | nv | nvexp) ";"	{
	*p = YYCURSOR;
	INIT_PZVAL(*rval);
	ZVAL_DOUBLE(*rval, atof(start + 2));
	return 1;
}

"s:" iv ":" ["] 	{
	int len;
	char *str;

	len = parse_iv(start + 2);

	if (len == 0) {
		str = empty_string;
	} else {
		str = estrndup(YYCURSOR, len);
	}

	YYCURSOR += len + 2;
	*p = YYCURSOR;

	INIT_PZVAL(*rval);
	ZVAL_STRINGL(*rval, str, len, 0);
	return 1;
}

"a:" iv ":" "{" {
	int elements = parse_iv(start + 2);

	*p = YYCURSOR;

	INIT_PZVAL(*rval);
	Z_TYPE_PP(rval) = IS_ARRAY;
	ALLOC_HASHTABLE(Z_ARRVAL_PP(rval));

	zend_hash_init(Z_ARRVAL_PP(rval), elements + 1, NULL, ZVAL_PTR_DTOR, 0);

	if (!process_nested_data(UNSERIALIZE_PASSTHRU, Z_ARRVAL_PP(rval), elements)) {
		return 0;
	}

	return finish_nested_data(UNSERIALIZE_PASSTHRU);
}

"o:" iv ":" ["] {

	INIT_PZVAL(*rval);
	
	return object_common2(UNSERIALIZE_PASSTHRU,
			object_common1(UNSERIALIZE_PASSTHRU, ZEND_STANDARD_CLASS_DEF_PTR));
}

"O:" iv ":" ["]	{
	int len;
	int elements;
	int len2;
	char *class_name;
	zend_class_entry *ce;
	int incomplete_class = 0;
	
	zval *user_func;
	zval *retval_ptr;
	zval **args[1];
	zval *arg_func_name;
	
	INIT_PZVAL(*rval);
	len2 = len = parse_iv(start + 2);
	if (len == 0)
		return 0;

	class_name = estrndup(YYCURSOR, len);
	YYCURSOR += len;

	while (len-- > 0) {
		if (class_name[len] >= 'A' && class_name[len] <= 'Z') {
			class_name[len] = class_name[len] - 'A' + 'a';
		}
	}

	if (zend_hash_find(CG(class_table), class_name, len2 + 1, (void **) &ce) != SUCCESS) {
		if ((PG(unserialize_callback_func) == NULL) || (PG(unserialize_callback_func)[0] == '\0')) {
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
		} else {
			MAKE_STD_ZVAL(user_func);
			ZVAL_STRING(user_func, PG(unserialize_callback_func), 1);

			args[0] = &arg_func_name;
			MAKE_STD_ZVAL(arg_func_name);
			ZVAL_STRING(arg_func_name, class_name, 1);
				
			if (call_user_function_ex(CG(function_table), NULL, user_func, &retval_ptr, 1, args, 0, NULL TSRMLS_CC) != SUCCESS) {
				zend_error(E_WARNING, "'unserialize_callback_func' defined (%s) but not found", user_func->value.str.val);
				incomplete_class = 1;
				ce = PHP_IC_ENTRY;
			} else {
				if (zend_hash_find(CG(class_table), class_name, len2 + 1, (void **) &ce) != SUCCESS) {
					zend_error(E_WARNING, "'unserialize_callback_func' (%s) hasn't defined the class it was called for", user_func->value.str.val);
					incomplete_class = 1;
					ce = PHP_IC_ENTRY;
				} else {
#ifdef ZEND_ENGINE_2
					ce = *(zend_class_entry **)ce; /* Bad hack, TBF! */
#endif	
					efree(class_name);
				}
			}
		}
	} else {
#ifdef ZEND_ENGINE_2
		ce = *(zend_class_entry **)ce; /* Bad hack, TBF! */
#endif	
		efree(class_name);
	}

	*p = YYCURSOR;
	elements = object_common1(UNSERIALIZE_PASSTHRU, ce);

	if (incomplete_class) {
		php_store_class_name(*rval, class_name, len2 TSRMLS_CC);
		efree(class_name);
	}

	return object_common2(UNSERIALIZE_PASSTHRU, elements);
}

"}" {
	/* this is the case where we have less data than planned */
	zend_error(E_NOTICE, "Unexpected end of serialized data");
	return 0; /* not sure if it should be 0 or 1 here? */
}

any	{ return 0; }

*/

	return 0;
}
