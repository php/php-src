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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "token_list.h"
#include "infra.h"
#include "zend_interfaces.h"

#define TOKEN_LIST_GET_INTERNAL() php_dom_token_list_from_obj(Z_OBJ_P(ZEND_THIS))
#define TOKEN_LIST_GET_SET(intern) (&(intern)->token_set)
#define Z_TOKEN_LIST_P(zv) php_dom_token_list_from_obj(Z_OBJ_P(zv))

typedef struct dom_token_list_it {
	zend_object_iterator it;
	/* Store the hash position here to allow multiple (e.g. nested) iterations of the same token list. */
	HashPosition pos;
	php_libxml_cache_tag cache_tag;
} dom_token_list_it;

static zend_always_inline bool dom_contains_ascii_whitespace(const char *data)
{
	return strpbrk(data, ascii_whitespace) != NULL;
}

static zend_always_inline void dom_add_token(HashTable *ht, zend_string *token)
{
	/* Key outlives the value's lifetime because as long as the entry is in the table it is kept alive. */
	zval zv;
	ZVAL_STR(&zv, token);
	zend_hash_add(ht, token, &zv);
}

/* https://dom.spec.whatwg.org/#concept-ordered-set-parser
 * and https://infra.spec.whatwg.org/#split-on-ascii-whitespace */
static void dom_ordered_set_parser(HashTable *token_set, const char *position)
{
	/* Adapted steps from "split on ASCII whitespace" such that that loop directly appends to the token set. */

	/* 1. Let position be a position variable for input, initially pointing at the start of input.
	 *    => That's the position pointer. */
	/* 2. Let tokens be a list of strings, initially empty.
	 *    => That's the token set. */

	/* 3. Skip ASCII whitespace within input given position. */
	position += strspn(position, ascii_whitespace);

	/* 4. While position is not past the end of input: */
	while (*position != '\0') {
		/* 4.1. Let token be the result of collecting a sequence of code points that are not ASCII whitespace from input */
		const char *start = position;
		position += strcspn(position, ascii_whitespace);
		size_t length = position - start;

		/* 4.2. Append token to tokens. */
		zend_string *token = zend_string_init(start, length, false);
		dom_add_token(token_set, token);
		zend_string_release_ex(token, false);

		/* 4.3. Skip ASCII whitespace within input given position. */
		position += strspn(position, ascii_whitespace);
	}

	/* 5. Return tokens.
	 *    => That's the token set. */
}

/* https://dom.spec.whatwg.org/#concept-ordered-set-serializer */
static char *dom_ordered_set_serializer(HashTable *token_set)
{
	size_t length = 0;
	zend_string *token;
	ZEND_HASH_MAP_FOREACH_STR_KEY(token_set, token) {
		size_t needed_size = ZSTR_LEN(token) + 1; /* +1 for the space (or \0 at the end) */
		if (UNEXPECTED(ZSTR_MAX_LEN - length < needed_size)) {
			/* Shouldn't really be able to happen in practice. */
			zend_throw_error(NULL, "Token set too large");
			return NULL;
		}
		length += needed_size;
	} ZEND_HASH_FOREACH_END();

	if (length == 0) {
		char *ret = emalloc(1);
		*ret = '\0';
		return ret;
	}

	char *ret = emalloc(length);
	char *ptr = ret;
	ZEND_HASH_MAP_FOREACH_STR_KEY(token_set, token) {
		memcpy(ptr, ZSTR_VAL(token), ZSTR_LEN(token));
		ptr += ZSTR_LEN(token);
		*ptr++ = ' ';
	} ZEND_HASH_FOREACH_END();
	ptr[-1] = '\0'; /* replace last space with \0 */
	return ret;
}

static zend_always_inline xmlNode *dom_token_list_get_element(dom_token_list_object *intern)
{
	php_libxml_node_ptr *element_ptr = intern->dom.ptr;
	return element_ptr->node;
}

static zend_always_inline const xmlAttr *dom_token_list_get_attr(dom_token_list_object *intern)
{
	const xmlNode *element_node = dom_token_list_get_element(intern);
	return xmlHasNsProp(element_node, BAD_CAST "class", NULL);
}

/* https://dom.spec.whatwg.org/#concept-dtl-update */
static void dom_token_list_update(dom_token_list_object *intern)
{
	const xmlAttr *attr = dom_token_list_get_attr(intern);
	HashTable *token_set = TOKEN_LIST_GET_SET(intern);

	php_libxml_invalidate_cache_tag(&intern->cache_tag);

	/* 1. If the associated element does not have an associated attribute and token set is empty, then return. */
	if (attr == NULL && zend_hash_num_elements(token_set) == 0) {
		return;
	}

	/* 2. Set an attribute value for the associated element using associated attribute’s local name and the result of
	 *    running the ordered set serializer for token set. */
	char *value = dom_ordered_set_serializer(token_set);
	xmlSetNsProp(dom_token_list_get_element(intern), NULL, BAD_CAST "class", BAD_CAST value);
	efree(intern->cached_string);
	intern->cached_string = value;
}

static xmlChar *dom_token_list_get_class_value(const xmlAttr *attr, bool *should_free)
{
	if (attr != NULL && attr->children != NULL) {
		return php_libxml_attr_value(attr, should_free);
	}
	*should_free = false;
	return NULL;
}

static void dom_token_list_update_set(dom_token_list_object *intern, HashTable *token_set)
{
	/* https://dom.spec.whatwg.org/#ref-for-domtokenlist%E2%91%A0%E2%91%A1 */
	bool should_free;
	const xmlAttr *attr = dom_token_list_get_attr(intern);
	/* 1. If the data is null, the token set remains empty. */
	xmlChar *value = dom_token_list_get_class_value(attr, &should_free);
	if (value != NULL) {
		/* 2. Otherwise, parse the token set. */
		dom_ordered_set_parser(token_set, (const char *) value);
		intern->cached_string = estrdup((const char *) value);
	} else {
		intern->cached_string = NULL;
	}

	if (should_free) {
		xmlFree(value);
	}
}

static void dom_token_list_ensure_set_up_to_date(dom_token_list_object *intern)
{
	bool should_free;
	const xmlAttr *attr = dom_token_list_get_attr(intern);
	xmlChar *value = dom_token_list_get_class_value(attr, &should_free);

	/* xmlStrEqual will automatically handle equality rules of NULL vs "" (etc.) correctly. */
	if (!xmlStrEqual(value, (const xmlChar *) intern->cached_string)) {
		php_libxml_invalidate_cache_tag(&intern->cache_tag);
		efree(intern->cached_string);
		HashTable *token_set = TOKEN_LIST_GET_SET(intern);
		zend_hash_destroy(token_set);
		zend_hash_init(token_set, 0, NULL, NULL, false);
		dom_token_list_update_set(intern, token_set);
	}

	if (should_free) {
		xmlFree(value);
	}
}

void dom_token_list_ctor(dom_token_list_object *intern, dom_object *element_obj)
{
	php_libxml_node_ptr *ptr = element_obj->ptr;
	ptr->refcount++;
	intern->dom.ptr = ptr;
	element_obj->document->refcount++;
	intern->dom.document = element_obj->document;

	intern->cache_tag.modification_nr = 0;

	HashTable *token_set = TOKEN_LIST_GET_SET(intern);
	zend_hash_init(token_set, 0, NULL, NULL, false);

	dom_token_list_update_set(intern, token_set);
}

void dom_token_list_free_obj(zend_object *object)
{
	dom_token_list_object *intern = php_dom_token_list_from_obj(object);

	zend_object_std_dtor(object);

	if (EXPECTED(intern->dom.ptr != NULL)) { /* Object initialized? */
		xmlNodePtr node = dom_token_list_get_element(intern);
		if (php_libxml_decrement_node_ptr_ref(intern->dom.ptr) == 0) {
			php_libxml_node_free_resource(node);
		}
		php_libxml_decrement_doc_ref((php_libxml_node_object *) &intern->dom);
		HashTable *token_set = TOKEN_LIST_GET_SET(intern);
		zend_hash_destroy(token_set);
		efree(intern->cached_string);
	}
}

static bool dom_token_list_item_exists(dom_token_list_object *token_list, zend_long index)
{
	dom_token_list_ensure_set_up_to_date(token_list);

	HashTable *token_set = TOKEN_LIST_GET_SET(token_list);
	return index >= 0 && index < zend_hash_num_elements(token_set);
}

static void dom_token_list_item_read(dom_token_list_object *token_list, zval *retval, zend_long index)
{
	dom_token_list_ensure_set_up_to_date(token_list);

	HashTable *token_set = TOKEN_LIST_GET_SET(token_list);
	if (index >= 0 && index < zend_hash_num_elements(token_set)) {
		HashPosition position;
		zend_hash_internal_pointer_reset_ex(token_set, &position);
		while (index > 0) {
			zend_hash_move_forward_ex(token_set, &position);
			index--;
		}
		zend_string *str_index;
		zend_hash_get_current_key_ex(token_set, &str_index, NULL, &position);
		ZVAL_STR_COPY(retval, str_index);
	} else {
		/* Not an out of bounds ValueError, but NULL, as according to spec.
		 * This design choice allows for constructs like `item(x) ?? ...`
		 *
		 * In particular:
		 * https://dom.spec.whatwg.org/#interface-domtokenlist states DOMTokenList implements iterable<DOMString>.
		 * From https://webidl.spec.whatwg.org/#idl-iterable:
		 *   If a single type parameter is given,
		 *   then the interface has a value iterator and provides values of the specified type.
		 * This applies, and reading the definition of value iterator means we should support indexed properties.
		 * From https://webidl.spec.whatwg.org/#dfn-support-indexed-properties:
		 *   An interface that defines an indexed property getter is said to support indexed properties.
		 * And indexed property getter is defined here: https://webidl.spec.whatwg.org/#dfn-indexed-property-getter
		 * Down below in their note they give an example of how an out-of-bounds access evaluates to undefined,
		 * which would map to NULL for us.
		 * This would also be consistent with how out-of-bounds array accesses in PHP result in NULL. */
		ZVAL_NULL(retval);
	}
}

/* Adapted from spl_offset_convert_to_long */
static zend_long dom_token_list_offset_convert_to_long(zval *offset, bool *failed)
{
	*failed = false;

	while (true) {
		switch (Z_TYPE_P(offset)) {
			case IS_STRING: {
				zend_ulong index;
				if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), index)) {
					return (zend_long) index;
				}
				ZEND_FALLTHROUGH;
			}
			default:
				*failed = true;
				return 0;
			case IS_DOUBLE:
				return zend_dval_to_lval_safe(Z_DVAL_P(offset));
			case IS_LONG:
				return Z_LVAL_P(offset);
			case IS_FALSE:
				return 0;
			case IS_TRUE:
				return 1;
			case IS_REFERENCE:
				offset = Z_REFVAL_P(offset);
				break;
			case IS_RESOURCE:
				zend_use_resource_as_offset(offset);
				return Z_RES_HANDLE_P(offset);
		}
	}
}

zval *dom_token_list_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (!offset) {
		zend_throw_error(NULL, "Cannot append to Dom\\TokenList");
		return NULL;
	}

	bool failed;
	zend_long index = dom_token_list_offset_convert_to_long(offset, &failed);
	if (UNEXPECTED(failed)) {
		zend_illegal_container_offset(object->ce->name, offset, type);
		return NULL;
	} else {
		dom_token_list_item_read(php_dom_token_list_from_obj(object), rv, index);
		return rv;
	}
}

int dom_token_list_has_dimension(zend_object *object, zval *offset, int check_empty)
{
	bool failed;
	zend_long index = dom_token_list_offset_convert_to_long(offset, &failed);
	if (UNEXPECTED(failed)) {
		zend_illegal_container_offset(object->ce->name, offset, BP_VAR_IS);
		return 0;
	} else {
		dom_token_list_object *token_list = php_dom_token_list_from_obj(object);
		if (check_empty) {
			/* Need to perform an actual read to have the correct empty() semantics. */
			zval rv;
			dom_token_list_item_read(token_list, &rv, index);
			int is_true = zend_is_true(&rv);
			zval_ptr_dtor_nogc(&rv);
			return is_true;
		} else {
			return dom_token_list_item_exists(token_list, index);
		}
	}
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-length */
zend_result dom_token_list_length_read(dom_object *obj, zval *retval)
{
	dom_token_list_object *token_list = php_dom_token_list_from_dom_obj(obj);
	dom_token_list_ensure_set_up_to_date(token_list);
	ZVAL_LONG(retval, zend_hash_num_elements(TOKEN_LIST_GET_SET(token_list)));
	return SUCCESS;
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-value
 * and https://dom.spec.whatwg.org/#concept-dtl-serialize */
zend_result dom_token_list_value_read(dom_object *obj, zval *retval)
{
	bool should_free;
	dom_token_list_object *intern = php_dom_token_list_from_dom_obj(obj);
	const xmlAttr *attr = dom_token_list_get_attr(intern);
	xmlChar *value = dom_token_list_get_class_value(attr, &should_free);
	ZVAL_STRING(retval, value ? (const char *) value : "");
	if (should_free) {
		xmlFree(value);
	}
	return SUCCESS;
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-value */
zend_result dom_token_list_value_write(dom_object *obj, zval *newval)
{
	dom_token_list_object *intern = php_dom_token_list_from_dom_obj(obj);
	if (UNEXPECTED(zend_str_has_nul_byte(Z_STR_P(newval)))) {
		zend_value_error("Value must not contain any null bytes");
		return FAILURE;
	}
	xmlSetNsProp(dom_token_list_get_element(intern), NULL, BAD_CAST "class", BAD_CAST Z_STRVAL_P(newval));
	/* Note: we don't update the set here, the set is always lazily updated for performance reasons. */
	return SUCCESS;
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-item */
PHP_METHOD(Dom_TokenList, item)
{
	zend_long index;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();

	/* 1. If index is equal to or greater than this’s token set’s size, then return null. */
	/* 2. Return this’s token set[index]. */
	dom_token_list_item_read(TOKEN_LIST_GET_INTERNAL(), return_value, index);
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-contains */
PHP_METHOD(Dom_TokenList, contains)
{
	zend_string *token;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(token)
	ZEND_PARSE_PARAMETERS_END();

	dom_token_list_object *token_list = TOKEN_LIST_GET_INTERNAL();
	dom_token_list_ensure_set_up_to_date(token_list);
	HashTable *token_set = TOKEN_LIST_GET_SET(token_list);
	RETURN_BOOL(zend_hash_exists(token_set, token));
}

/* Steps taken from the add, remove, toggle, replace methods. */
static bool dom_validate_token(const zend_string *str)
{
	/* 1. If token is the empty string, then throw a "SyntaxError" DOMException. */
	if (ZSTR_LEN(str) == 0) {
		php_dom_throw_error_with_message(SYNTAX_ERR, "The empty string is not a valid token", true);
		return false;
	}

	/* 2. If token contains any ASCII whitespace, then throw an "InvalidCharacterError" DOMException. */
	if (dom_contains_ascii_whitespace(ZSTR_VAL(str))) {
		php_dom_throw_error_with_message(INVALID_CHARACTER_ERR, "The token must not contain any ASCII whitespace", true);
		return false;
	}

	return true;
}

static bool dom_validate_tokens_varargs(const zval *args, uint32_t argc)
{
	for (uint32_t i = 0; i < argc; i++) {
		if (Z_TYPE(args[i]) != IS_STRING) {
			zend_argument_type_error(i + 1, "must be of type string, %s given", zend_zval_value_name(&args[i]));
			return false;
		}

		if (zend_str_has_nul_byte(Z_STR(args[i]))) {
			zend_argument_value_error(i + 1, "must not contain any null bytes");
			return false;
		}

		if (!dom_validate_token(Z_STR(args[i]))) {
			return false;
		}
	}

	return true;
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-add */
PHP_METHOD(Dom_TokenList, add)
{
	zval *args;
	uint32_t argc;
	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* 1. For each token in tokens (...) */
	if (!dom_validate_tokens_varargs(args, argc)) {
		RETURN_THROWS();
	}

	/* 2. For each token in tokens, append token to this’s token set. */
	dom_token_list_object *intern = TOKEN_LIST_GET_INTERNAL();
	dom_token_list_ensure_set_up_to_date(intern);
	HashTable *token_set = TOKEN_LIST_GET_SET(intern);
	for (uint32_t i = 0; i < argc; i++) {
		dom_add_token(token_set, Z_STR(args[i]));
	}

	/* 3. Run the update steps. */
	dom_token_list_update(intern);
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-remove */
PHP_METHOD(Dom_TokenList, remove)
{
	zval *args;
	uint32_t argc;
	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* 1. For each token in tokens (...) */
	if (!dom_validate_tokens_varargs(args, argc)) {
		RETURN_THROWS();
	}

	/* 2. For each token in tokens, remove token from this’s token set. */
	dom_token_list_object *intern = TOKEN_LIST_GET_INTERNAL();
	dom_token_list_ensure_set_up_to_date(intern);
	HashTable *token_set = TOKEN_LIST_GET_SET(intern);
	for (uint32_t i = 0; i < argc; i++) {
		zend_hash_del(token_set, Z_STR(args[i]));
	}

	/* 3. Run the update steps. */
	dom_token_list_update(intern);
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-toggle */
PHP_METHOD(Dom_TokenList, toggle)
{
	zend_string *token;
	bool force, force_not_given = true;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(token)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(force, force_not_given)
	ZEND_PARSE_PARAMETERS_END();

	/* Steps 1 - 2 */
	if (!dom_validate_token(token)) {
		RETURN_THROWS();
	}

	/* 3. If this’s token set[token] exists, then: */
	dom_token_list_object *intern = TOKEN_LIST_GET_INTERNAL();
	dom_token_list_ensure_set_up_to_date(intern);
	HashTable *token_set = TOKEN_LIST_GET_SET(intern);
	zval *found_token = zend_hash_find(token_set, token);
	if (found_token != NULL) {
		ZEND_ASSERT(XtOffsetOf(Bucket, val) == 0 && "the cast only works if this is true");
		Bucket *bucket = (Bucket *) found_token;

		/* 3.1. If force is either not given or is false, then remove token from this’s token set,
		 *      run the update steps and return false. */
		if (force_not_given || !force) {
			zend_hash_del_bucket(token_set, bucket);
			dom_token_list_update(intern);
			RETURN_FALSE;
		}

		/* 3.2. Return true. */
		RETURN_TRUE;
	}
	/* 4. Otherwise, if force not given or is true, append token to this’s token set,
	 *    run the update steps, and return true. */
	else if (force_not_given || force) {
		dom_add_token(token_set, token);
		dom_token_list_update(intern);
		RETURN_TRUE;
	}

	/* 5. Return false. */
	RETURN_FALSE;
}

/* https://dom.spec.whatwg.org/#dom-domtokenlist-replace */
PHP_METHOD(Dom_TokenList, replace)
{
	zend_string *token, *new_token;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH_STR(token)
		Z_PARAM_PATH_STR(new_token)
	ZEND_PARSE_PARAMETERS_END();

	/* Steps 1 - 2 */
	if (!dom_validate_token(token) || !dom_validate_token(new_token)) {
		RETURN_THROWS();
	}

	/* 3. If this’s token set does not contain token, then return false. */
	dom_token_list_object *intern = TOKEN_LIST_GET_INTERNAL();
	dom_token_list_ensure_set_up_to_date(intern);
	HashTable *token_set = TOKEN_LIST_GET_SET(intern);
	zval *found_token = zend_hash_find(token_set, token);
	if (found_token == NULL) {
		RETURN_FALSE;
	}

	/* 4. Replace token in this’s token set with newToken. */
	ZEND_ASSERT(XtOffsetOf(Bucket, val) == 0 && "the cast only works if this is true");
	Bucket *bucket = (Bucket *) found_token;
	if (zend_hash_set_bucket_key(token_set, bucket, new_token) == NULL) {
		/* It already exists, remove token instead. */
		zend_hash_del_bucket(token_set, bucket);
	} else {
		Z_STR(bucket->val) = new_token;
	}

	/* 5. Run the update steps. */
	dom_token_list_update(intern);

	/* 6. Return true. */
	RETURN_TRUE;
}

/* https://dom.spec.whatwg.org/#concept-domtokenlist-validation */
PHP_METHOD(Dom_TokenList, supports)
{
	zend_string *token;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(token)
	ZEND_PARSE_PARAMETERS_END();

	/* The spec designers have designed the TokenList API with future usages in mind.
	 * But right now, this should just always throw a TypeError because the only user is classList, which
	 * does not define a supported token set. */
	zend_throw_error(zend_ce_type_error, "Attribute \"class\" does not define any supported tokens");
}

PHP_METHOD(Dom_TokenList, count)
{
	ZEND_PARSE_PARAMETERS_NONE();
	dom_token_list_object *intern = TOKEN_LIST_GET_INTERNAL();
	dom_token_list_ensure_set_up_to_date(intern);
	RETURN_LONG(zend_hash_num_elements(TOKEN_LIST_GET_SET(intern)));
}

PHP_METHOD(Dom_TokenList, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void dom_token_list_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void dom_token_list_it_rewind(zend_object_iterator *iter)
{
	dom_token_list_it     *iterator = (dom_token_list_it *) iter;
	dom_token_list_object *object   = Z_TOKEN_LIST_P(&iter->data);
	zend_hash_internal_pointer_reset_ex(TOKEN_LIST_GET_SET(object), &iterator->pos);
}

static zend_result dom_token_list_it_valid(zend_object_iterator *iter)
{
	dom_token_list_it     *iterator  = (dom_token_list_it *) iter;
	dom_token_list_object *object    = Z_TOKEN_LIST_P(&iter->data);
	HashTable             *token_set = TOKEN_LIST_GET_SET(object);

	dom_token_list_ensure_set_up_to_date(object);

	iterator->pos = zend_hash_get_current_pos_ex(token_set, iterator->pos);

	return iterator->pos >= token_set->nNumUsed ? FAILURE : SUCCESS;
}

static zval *dom_token_list_it_get_current_data(zend_object_iterator *iter)
{
	dom_token_list_it     *iterator  = (dom_token_list_it *) iter;
	dom_token_list_object *object    = Z_TOKEN_LIST_P(&iter->data);
	dom_token_list_ensure_set_up_to_date(object);
	/* Caller manages the refcount of the data. */
	return zend_hash_get_current_data_ex(TOKEN_LIST_GET_SET(object), &iterator->pos);
}

static void dom_token_list_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	dom_token_list_it	  *iterator  = (dom_token_list_it *) iter;
	dom_token_list_object *object    = Z_TOKEN_LIST_P(&iter->data);

	dom_token_list_ensure_set_up_to_date(object);

	if (UNEXPECTED(php_libxml_is_cache_tag_stale(&object->cache_tag, &iterator->cache_tag))) {
		iter->index = 0;
		HashPosition pos;
		HashTable *token_set = TOKEN_LIST_GET_SET(object);
		zend_hash_internal_pointer_reset_ex(token_set, &pos);
		while (pos != iterator->pos) {
			iter->index++;
			zend_hash_move_forward_ex(token_set, &pos);
		}
	}

	ZVAL_LONG(key, iter->index);
}

static void dom_token_list_it_move_forward(zend_object_iterator *iter)
{
	dom_token_list_it     *iterator  = (dom_token_list_it *) iter;
	dom_token_list_object *object    = Z_TOKEN_LIST_P(&iter->data);
	HashTable             *token_set = TOKEN_LIST_GET_SET(object);

	dom_token_list_ensure_set_up_to_date(object);

	HashPosition current = iterator->pos;
	HashPosition validated = zend_hash_get_current_pos_ex(token_set, iterator->pos);

	/* Check if already moved due to user operations, if so don't move again but reset to the first valid position,
	 * otherwise move one forward. */
	if (validated != current) {
		iterator->pos = validated;
	} else {
		zend_hash_move_forward_ex(token_set, &iterator->pos);
	}
}

static const zend_object_iterator_funcs dom_token_list_it_funcs = {
	dom_token_list_it_dtor,
	dom_token_list_it_valid,
	dom_token_list_it_get_current_data,
	dom_token_list_it_get_current_key,
	dom_token_list_it_move_forward,
	dom_token_list_it_rewind,
	NULL, /* invalidate_current */
	NULL, /* get_gc */
};

zend_object_iterator *dom_token_list_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	dom_token_list_object *intern = Z_TOKEN_LIST_P(object);
	dom_token_list_ensure_set_up_to_date(intern);
	HashTable *token_set = TOKEN_LIST_GET_SET(intern);

	dom_token_list_it *iterator = emalloc(sizeof(*iterator));
	zend_iterator_init(&iterator->it);
	zend_hash_internal_pointer_reset_ex(token_set, &iterator->pos);
	ZVAL_OBJ_COPY(&iterator->it.data, Z_OBJ_P(object));

	iterator->it.funcs = &dom_token_list_it_funcs;
	iterator->cache_tag = intern->cache_tag;

	return &iterator->it;
}

#endif
