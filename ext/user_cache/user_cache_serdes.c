/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "user_cache_internal.h"

#include "Zend/zend_closures.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_interfaces.h"

/* Position-independent tagged stream using native byte order. */
#define PHP_USER_CACHE_SERDES_TAG_NULL				1
#define PHP_USER_CACHE_SERDES_TAG_FALSE				2
#define PHP_USER_CACHE_SERDES_TAG_TRUE				3
#define PHP_USER_CACHE_SERDES_TAG_LONG				4
#define PHP_USER_CACHE_SERDES_TAG_DOUBLE			5
#define PHP_USER_CACHE_SERDES_TAG_STRING			6
#define PHP_USER_CACHE_SERDES_TAG_EMPTY_ARRAY		7
#define PHP_USER_CACHE_SERDES_TAG_PACKED_ARRAY		8
#define PHP_USER_CACHE_SERDES_TAG_HASHED_ARRAY		9
#define PHP_USER_CACHE_SERDES_TAG_OBJECT			10
#define PHP_USER_CACHE_SERDES_TAG_SERIALIZED_OBJECT	11
#define PHP_USER_CACHE_SERDES_TAG_CUSTOM_OBJECT		12
#define PHP_USER_CACHE_SERDES_TAG_ENUM				13
#define PHP_USER_CACHE_SERDES_TAG_REFERENCE			14
#define PHP_USER_CACHE_SERDES_TAG_BACKREF			15

#define PHP_USER_CACHE_SERDES_KEY_LONG		0
#define PHP_USER_CACHE_SERDES_KEY_STRING	1

#define PHP_USER_CACHE_SERDES_PROPERTY_INDEX_NONE	0

#define PHP_USER_CACHE_SERDES_STRING_RAW	0
#define PHP_USER_CACHE_SERDES_STRING_INLINE	1
#define PHP_USER_CACHE_SERDES_STRING_REF	2

#define PHP_USER_CACHE_SERDES_INLINE_IDS		8
#define PHP_USER_CACHE_SERDES_INLINE_STRINGS	32

typedef struct {
	smart_str buf;
	HashTable seen;
	HashTable strings;
	uint32_t string_count;
	const char *failure_message;
} php_user_cache_serdes_encoder;

typedef struct {
	const uint8_t *data;
	size_t len;
	size_t pos;
	zval *ids;
	uint32_t id_count;
	uint32_t id_capacity;
	zend_string **strings;
	uint32_t string_count;
	uint32_t string_capacity;
	zval inline_ids[PHP_USER_CACHE_SERDES_INLINE_IDS];
	zend_string *inline_strings[PHP_USER_CACHE_SERDES_INLINE_STRINGS];
} php_user_cache_serdes_decoder;

static bool user_cache_serdes_encode_value(
	php_user_cache_serdes_encoder *enc,
	zval *value
);

static bool user_cache_serdes_decode_value(
	php_user_cache_serdes_decoder *dec,
	zval *dst
);

static bool user_cache_serdes_decoder_register_string(
	php_user_cache_serdes_decoder *dec,
	zend_string *str
);

/* Fixed-width values are self-aligned; the padding is part of the wire
 * format. */
static zend_always_inline void user_cache_serdes_put_pad(smart_str *buf, size_t align)
{
	size_t len = smart_str_get_len(buf), pad = (align - (len & (align - 1))) & (align - 1);

	if (pad != 0) {
		memset(smart_str_extend(buf, pad), 0, pad);
	}
}

static zend_always_inline void user_cache_serdes_put_u8(smart_str *buf, uint8_t value)
{
	smart_str_appendc(buf, (char) value);
}

static zend_always_inline void user_cache_serdes_put_fixed(smart_str *buf, const void *value, size_t size)
{
	user_cache_serdes_put_pad(buf, size);
	smart_str_appendl(buf, (const char *) value, size);
}

static zend_always_inline void user_cache_serdes_put_u32(smart_str *buf, uint32_t value)
{
	user_cache_serdes_put_fixed(buf, &value, sizeof(value));
}

static zend_always_inline void user_cache_serdes_put_u64(smart_str *buf, uint64_t value)
{
	user_cache_serdes_put_fixed(buf, &value, sizeof(value));
}

static zend_always_inline void user_cache_serdes_put_long(smart_str *buf, zend_long value)
{
	user_cache_serdes_put_fixed(buf, &value, sizeof(value));
}

static zend_always_inline void user_cache_serdes_put_double(smart_str *buf, double value)
{
	user_cache_serdes_put_fixed(buf, &value, sizeof(value));
}

static zend_always_inline size_t user_cache_serdes_reserve_u32(smart_str *buf)
{
	user_cache_serdes_put_pad(buf, sizeof(uint32_t));
	memset(smart_str_extend(buf, sizeof(uint32_t)), 0, sizeof(uint32_t));

	return smart_str_get_len(buf) - sizeof(uint32_t);
}

static zend_always_inline void user_cache_serdes_patch_u32(
		smart_str *buf,
		size_t pos,
		uint32_t value)
{
	ZEND_ASSERT(buf->s != NULL && pos + sizeof(uint32_t) <= ZSTR_LEN(buf->s));

	memcpy(ZSTR_VAL(buf->s) + pos, &value, sizeof(value));
}

/* Back-ref ids are assigned in traversal order and must match the decoder's
 * registration order; ids are stored biased by one so an empty slot stays
 * distinguishable from id 0. */
static zend_always_inline bool user_cache_serdes_encode_backref_or_register(
		php_user_cache_serdes_encoder *enc,
		const void *container,
		bool *is_backref)
{
	zval *slot;
	uintptr_t id;

	slot = zend_hash_index_lookup(&enc->seen, (zend_ulong) (uintptr_t) container);

	if (Z_TYPE_P(slot) == IS_PTR) {
		*is_backref = true;

		user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_BACKREF);
		user_cache_serdes_put_u32(&enc->buf, (uint32_t) ((uintptr_t) Z_PTR_P(slot) - 1));

		return true;
	}

	*is_backref = false;
	id = (uintptr_t) zend_hash_num_elements(&enc->seen);

	if (id > UINT32_MAX) {
		enc->failure_message = "value contains too many arrays or objects to be stored in the user cache";

		return false;
	}

	ZVAL_PTR(slot, (void *) id);

	return true;
}

static zend_always_inline bool user_cache_serdes_skip_pad(
		php_user_cache_serdes_decoder *dec,
		size_t align)
{
	size_t pad = (align - (dec->pos & (align - 1))) & (align - 1);

	if (pad > dec->len - dec->pos) {
		return false;
	}

	dec->pos += pad;

	return true;
}

static zend_always_inline bool user_cache_serdes_get_u8(
		php_user_cache_serdes_decoder *dec,
		uint8_t *value)
{
	if (dec->pos >= dec->len) {
		return false;
	}

	*value = dec->data[dec->pos++];

	return true;
}

static zend_always_inline bool user_cache_serdes_get_fixed(
		php_user_cache_serdes_decoder *dec,
		void *value,
		size_t size)
{
	if (!user_cache_serdes_skip_pad(dec, size) ||
		size > dec->len - dec->pos
	) {
		return false;
	}

	memcpy(value, dec->data + dec->pos, size);
	dec->pos += size;

	return true;
}

static zend_always_inline bool user_cache_serdes_get_u32(
		php_user_cache_serdes_decoder *dec,
		uint32_t *value)
{
	return user_cache_serdes_get_fixed(dec, value, sizeof(*value));
}

static zend_always_inline bool user_cache_serdes_get_u64(
		php_user_cache_serdes_decoder *dec,
		uint64_t *value)
{
	return user_cache_serdes_get_fixed(dec, value, sizeof(*value));
}

static zend_always_inline bool user_cache_serdes_get_long(
		php_user_cache_serdes_decoder *dec,
		zend_long *value)
{
	return user_cache_serdes_get_fixed(dec, value, sizeof(*value));
}

static zend_always_inline bool user_cache_serdes_get_double(
		php_user_cache_serdes_decoder *dec,
		double *value)
{
	return user_cache_serdes_get_fixed(dec, value, sizeof(*value));
}

static zend_always_inline bool user_cache_serdes_get_bytes(
		php_user_cache_serdes_decoder *dec,
		size_t len,
		const uint8_t **bytes)
{
	if (len > dec->len - dec->pos) {
		return false;
	}

	*bytes = dec->data + dec->pos;
	dec->pos += len;

	return true;
}

static zend_always_inline bool user_cache_serdes_get_str(
		php_user_cache_serdes_decoder *dec,
		zend_string **value)
{
	const uint8_t *bytes;
	zend_string *str;
	uint32_t byte_len, string_id;
	uint8_t kind;

	if (!user_cache_serdes_get_u8(dec, &kind)) {
		return false;
	}

	if (kind == PHP_USER_CACHE_SERDES_STRING_REF) {
		if (!user_cache_serdes_get_u32(dec, &string_id) ||
			string_id >= dec->string_count
		) {
			return false;
		}

		*value = zend_string_copy(dec->strings[string_id]);

		return true;
	}

	if ((kind != PHP_USER_CACHE_SERDES_STRING_INLINE &&
		kind != PHP_USER_CACHE_SERDES_STRING_RAW) ||
		!user_cache_serdes_get_u32(dec, &byte_len) ||
		!user_cache_serdes_get_bytes(dec, byte_len, &bytes)
	) {
		return false;
	}

	str = zend_string_init((const char *) bytes, byte_len, 0);
	if (kind == PHP_USER_CACHE_SERDES_STRING_RAW) {
		*value = str;

		return true;
	}

	if (!user_cache_serdes_decoder_register_string(dec, str)) {
		zend_string_release(str);

		return false;
	}

	*value = zend_string_copy(str);

	return true;
}

static bool user_cache_serdes_put_str(
		php_user_cache_serdes_encoder *enc,
		const char *value,
		size_t len)
{
	zval *string_id, new_string_id;
	uint32_t id;

	string_id = zend_hash_str_find(&enc->strings, value, len);
	if (string_id != NULL) {
		if (Z_LVAL_P(string_id) == 0) {
			if (enc->string_count >= UINT32_MAX) {
				enc->failure_message = "value is too large to be stored in the user cache";

				return false;
			}
			id = enc->string_count++;

			ZVAL_LONG(string_id, (zend_long) id + 1);

			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_STRING_INLINE);
			user_cache_serdes_put_u32(&enc->buf, (uint32_t) len);
			if (len != 0) {
				smart_str_appendl(&enc->buf, value, len);
			}

			return true;
		}

		id = (uint32_t) (Z_LVAL_P(string_id) - 1);
		user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_STRING_REF);
		user_cache_serdes_put_u32(&enc->buf, id);

		return true;
	}

	if (len > UINT32_MAX) {
		enc->failure_message = "value is too large to be stored in the user cache";

		return false;
	}

	ZVAL_LONG(&new_string_id, 0);
	if (zend_hash_str_add_new(&enc->strings, value, len, &new_string_id) == NULL) {
		return false;
	}

	user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_STRING_RAW);
	user_cache_serdes_put_u32(&enc->buf, (uint32_t) len);
	if (len != 0) {
		smart_str_appendl(&enc->buf, value, len);
	}

	return true;
}

static zend_result user_cache_serdes_try_add_sleep_prop(
		zval *obj_zv,
		HashTable *props,
		zend_string *name,
		zend_string *error_name,
		HashTable *ht)
{
	zval *val = zend_hash_find(props, name);

	if (val == NULL) {
		return FAILURE;
	}

	if (Z_TYPE_P(val) == IS_INDIRECT) {
		val = Z_INDIRECT_P(val);
		if (Z_TYPE_P(val) == IS_UNDEF) {
			if (zend_get_typed_property_info_for_slot(Z_OBJ_P(obj_zv), val) != NULL) {
				return SUCCESS;
			}

			return FAILURE;
		}
	}

	if (!zend_hash_add(ht, name, val)) {
		php_error_docref(NULL, E_WARNING,
			"\"%s\" is returned from __sleep() multiple times",
			ZSTR_VAL(error_name)
		);

		return SUCCESS;
	}

	Z_TRY_ADDREF_P(val);

	return SUCCESS;
}

static bool user_cache_serdes_get_sleep_props(
		zval *obj_zv,
		HashTable *sleep_retval,
		zval *state)
{
	zend_result added;
	zend_string *name, *tmp_name, *candidate;
	zend_class_entry *ce = Z_OBJCE_P(obj_zv);
	zval *name_val;
	HashTable *props = zend_get_properties_for(obj_zv, ZEND_PROP_PURPOSE_SERIALIZE);
	bool result = true;

	array_init_size(state, zend_hash_num_elements(sleep_retval));

	ZEND_HASH_FOREACH_VAL_IND(sleep_retval, name_val) {
		ZVAL_DEREF(name_val);
		if (Z_TYPE_P(name_val) != IS_STRING) {
			php_error_docref(NULL, E_WARNING,
				"%s::__sleep() should return an array only containing the names of instance-variables to serialize",
				ZSTR_VAL(ce->name)
			);
		}

		name = zval_get_tmp_string(name_val, &tmp_name);

		/* Try unmangled, private and protected names in order. */
		added = user_cache_serdes_try_add_sleep_prop(obj_zv, props, name, name, Z_ARRVAL_P(state));

		if (added == FAILURE && !EG(exception)) {
			candidate = zend_mangle_property_name(
				ZSTR_VAL(ce->name), ZSTR_LEN(ce->name),
				ZSTR_VAL(name), ZSTR_LEN(name), ce->type == ZEND_INTERNAL_CLASS
			);

			added = user_cache_serdes_try_add_sleep_prop(obj_zv, props, candidate, name, Z_ARRVAL_P(state));

			zend_string_release(candidate);
		}

		if (added == FAILURE && !EG(exception)) {
			candidate = zend_mangle_property_name(
				"*", 1, ZSTR_VAL(name), ZSTR_LEN(name), ce->type == ZEND_INTERNAL_CLASS
			);

			added = user_cache_serdes_try_add_sleep_prop(obj_zv, props, candidate, name, Z_ARRVAL_P(state));

			zend_string_release(candidate);
		}

		if (added == FAILURE) {
			if (EG(exception)) {
				zend_tmp_string_release(tmp_name);
				result = false;

				break;
			}

			php_error_docref(
				NULL,
				E_WARNING,
				"\"%s\" returned as member variable from __sleep() but does not exist",
				ZSTR_VAL(name)
			);
		}

		zend_tmp_string_release(tmp_name);
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);

	return result;
}

static bool user_cache_serdes_encode_property(
		php_user_cache_serdes_encoder *enc,
		zend_class_entry *ce,
		zend_string *name,
		zval *value)
{
	if (!user_cache_serdes_put_str(enc, ZSTR_VAL(name), ZSTR_LEN(name))) {
		return false;
	}

	user_cache_serdes_put_u32(
		&enc->buf,
		php_user_cache_serdes_declared_property_index_plus_one(ce, name)
	);

	return user_cache_serdes_encode_value(enc, value);
}

static bool user_cache_serdes_encode_property_table(
		php_user_cache_serdes_encoder *enc,
		zend_class_entry *ce,
		HashTable *props,
		size_t count_pos)
{
	zend_ulong h;
	zend_string *name;
	zval *value;
	uint32_t count = 0;
	size_t digits_len;
	char numeric_key_buf[32], *digits;

	ZEND_HASH_FOREACH_KEY_VAL(props, h, name, value) {
		if (Z_TYPE_P(value) == IS_INDIRECT) {
			value = Z_INDIRECT_P(value);
		}

		if (Z_TYPE_P(value) == IS_UNDEF) {
			continue;
		}

		if (name != NULL) {
			if (!user_cache_serdes_encode_property(enc, ce, name, value)) {
				return false;
			}
		} else {
			digits = zend_print_long_to_buf(numeric_key_buf + sizeof(numeric_key_buf) - 1, (zend_long) h);
			digits_len = numeric_key_buf + sizeof(numeric_key_buf) - 1 - digits;

			if (!user_cache_serdes_put_str(enc, digits, digits_len)) {
				return false;
			}

			user_cache_serdes_put_u32(&enc->buf, PHP_USER_CACHE_SERDES_PROPERTY_INDEX_NONE);

			if (!user_cache_serdes_encode_value(enc, value)) {
				return false;
			}
		}

		count++;
	} ZEND_HASH_FOREACH_END();

	user_cache_serdes_patch_u32(&enc->buf, count_pos, count);

	return true;
}

static bool user_cache_serdes_encode_serialized_object(
		php_user_cache_serdes_encoder *enc,
		zval *value)
{
	zend_object *obj = Z_OBJ_P(value);
	zend_class_entry *ce = obj->ce;
	zend_string *class_name = ce->name;
	zval retval;
	bool result;

	user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_SERIALIZED_OBJECT);
	if (!user_cache_serdes_put_str(enc, ZSTR_VAL(class_name), ZSTR_LEN(class_name))) {
		return false;
	}

	if (!php_user_cache_serdes_call_magic_serialize(obj, &retval)) {
		return false;
	}

	result = user_cache_serdes_encode_value(enc, &retval);

	zval_ptr_dtor(&retval);

	return result;
}

static bool user_cache_serdes_encode_custom_object(
		php_user_cache_serdes_encoder *enc,
		zval *value)
{
	zend_class_entry *ce = Z_OBJCE_P(value);
	zend_string *class_name = ce->name;
	php_serialize_data_t ser_data;
	uint8_t *ser_buf = NULL;
	size_t ser_len = 0;
	bool result;

	if (ce->serialize == NULL || ce->unserialize == NULL) {
		enc->failure_message = PHP_USER_CACHE_MSG_OPAQUE_OBJECT_UNSTORABLE;

		return false;
	}

	PHP_VAR_SERIALIZE_INIT(ser_data);
	result = ce->serialize(value, &ser_buf, &ser_len, (zend_serialize_data *) ser_data) == SUCCESS;
	PHP_VAR_SERIALIZE_DESTROY(ser_data);

	if (!result || ser_len > UINT32_MAX) {
		if (ser_buf != NULL) {
			efree(ser_buf);
		}

		enc->failure_message = "the object could not be serialized for the user cache";

		return false;
	}

	user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_CUSTOM_OBJECT);
	if (!user_cache_serdes_put_str(enc, ZSTR_VAL(class_name), ZSTR_LEN(class_name))) {
		if (ser_buf != NULL) {
			efree(ser_buf);
		}

		return false;
	}

	user_cache_serdes_put_u32(&enc->buf, (uint32_t) ser_len);
	if (ser_len != 0) {
		smart_str_appendl(&enc->buf, (const char *) ser_buf, ser_len);
	}

	if (ser_buf != NULL) {
		efree(ser_buf);
	}

	return true;
}

static bool user_cache_serdes_put_object_header(
		php_user_cache_serdes_encoder *enc,
		zend_string *class_name,
		bool has_wakeup,
		size_t *count_pos)
{
	user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_OBJECT);

	if (!user_cache_serdes_put_str(enc, ZSTR_VAL(class_name), ZSTR_LEN(class_name))) {
		return false;
	}

	user_cache_serdes_put_u8(&enc->buf, has_wakeup ? 1 : 0);

	*count_pos = user_cache_serdes_reserve_u32(&enc->buf);

	return true;
}

static bool user_cache_serdes_encode_sleep_object(
		php_user_cache_serdes_encoder *enc,
		zval *value,
		bool has_wakeup)
{
	zend_class_entry *ce = Z_OBJCE_P(value);
	zend_string *class_name = ce->name;
	zval sleep_state;
	size_t count_pos;
	bool result;

	if (!user_cache_serdes_put_object_header(enc, class_name, has_wakeup, &count_pos)) {
		return false;
	}

	if (!php_user_cache_serdes_get_sleep_state(value, &sleep_state, &enc->failure_message)) {
		return false;
	}

	result = user_cache_serdes_encode_property_table(enc, ce, Z_ARRVAL(sleep_state), count_pos);

	zval_ptr_dtor(&sleep_state);

	return result;
}

static bool user_cache_serdes_encode_plain_object(
		php_user_cache_serdes_encoder *enc,
		zval *value,
		bool has_wakeup)
{
	zend_class_entry *ce = Z_OBJCE_P(value);
	zend_string *class_name = ce->name;
	HashTable *props;
	size_t count_pos;
	bool result;

	if (ce->type != ZEND_USER_CLASS && ce->create_object != NULL && !has_wakeup) {
		enc->failure_message = PHP_USER_CACHE_MSG_OPAQUE_OBJECT_UNSTORABLE;

		return false;
	}

	if (!user_cache_serdes_put_object_header(enc, class_name, has_wakeup, &count_pos)) {
		return false;
	}

	props = zend_get_properties_for(value, ZEND_PROP_PURPOSE_SERIALIZE);
	if (props == NULL) {
		user_cache_serdes_patch_u32(&enc->buf, count_pos, 0);

		return true;
	}

	result = user_cache_serdes_encode_property_table(enc, ce, props, count_pos);

	zend_release_properties(props);

	return result;
}

static bool user_cache_serdes_encode_object(
		php_user_cache_serdes_encoder *enc,
		zval *value)
{
	zend_object *obj = Z_OBJ_P(value);
	zend_class_entry *ce = obj->ce;
	zend_string *class_name = ce->name, *case_name;
	bool has_sleep, has_wakeup, is_backref;

	if (ce == zend_ce_closure) {
		enc->failure_message = PHP_USER_CACHE_MSG_CLOSURE_UNSTORABLE;

		return false;
	}

	if (zend_object_is_lazy(obj)) {
		enc->failure_message = PHP_USER_CACHE_MSG_LAZY_OBJECT_UNSTORABLE;

		return false;
	}

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_ENUM);

		if (!user_cache_serdes_put_str(enc, ZSTR_VAL(class_name), ZSTR_LEN(class_name))) {
			return false;
		}

		case_name = Z_STR_P(zend_enum_fetch_case_name(obj));
		if (!user_cache_serdes_put_str(enc, ZSTR_VAL(case_name), ZSTR_LEN(case_name))) {
			return false;
		}

		return true;
	}

	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		enc->failure_message = PHP_USER_CACHE_MSG_OPAQUE_OBJECT_UNSTORABLE;

		return false;
	}

	if (!user_cache_serdes_encode_backref_or_register(enc, obj, &is_backref)) {
		return false;
	}

	if (is_backref) {
		return true;
	}

	if (ce->__serialize != NULL && ce->__unserialize != NULL) {
		return user_cache_serdes_encode_serialized_object(enc, value);
	}

	/* Match native serialization precedence. */
	if (ce->serialize != NULL || ce->unserialize != NULL) {
		return user_cache_serdes_encode_custom_object(enc, value);
	}

	has_sleep = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL;
	has_wakeup = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL;

	if (has_sleep) {
		return user_cache_serdes_encode_sleep_object(enc, value, has_wakeup);
	}

	return user_cache_serdes_encode_plain_object(enc, value, has_wakeup);
}

static bool user_cache_serdes_encode_array(
		php_user_cache_serdes_encoder *enc,
		zval *value)
{
	zend_string *key;
	zend_ulong h;
	zval *elem;
	HashTable *arr = Z_ARRVAL_P(value);
	uint32_t count = zend_hash_num_elements(arr);
	bool is_backref;

	if (count == 0 && arr->nNextFreeElement == 0) {
		user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_EMPTY_ARRAY);

		return true;
	}

	if (!user_cache_serdes_encode_backref_or_register(enc, arr, &is_backref)) {
		return false;
	}

	if (is_backref) {
		return true;
	}

	if (HT_IS_PACKED(arr) && arr->nNumUsed == count) {
		user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_PACKED_ARRAY);
		user_cache_serdes_put_u64(&enc->buf, (uint64_t) arr->nNextFreeElement);
		user_cache_serdes_put_u32(&enc->buf, count);

		ZEND_HASH_PACKED_FOREACH_VAL(arr, elem) {
			if (!user_cache_serdes_encode_value(enc, elem)) {
				return false;
			}
		} ZEND_HASH_FOREACH_END();

		return true;
	}

	user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_HASHED_ARRAY);
	user_cache_serdes_put_u64(&enc->buf, (uint64_t) arr->nNextFreeElement);
	user_cache_serdes_put_u32(&enc->buf, count);

	ZEND_HASH_FOREACH_KEY_VAL(arr, h, key, elem) {
		if (key != NULL) {
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_KEY_STRING);
			if (!user_cache_serdes_put_str(enc, ZSTR_VAL(key), ZSTR_LEN(key))) {
				return false;
			}
		} else {
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_KEY_LONG);
			user_cache_serdes_put_u64(&enc->buf, (uint64_t) h);
		}

		if (!user_cache_serdes_encode_value(enc, elem)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static bool user_cache_serdes_encode_value(
		php_user_cache_serdes_encoder *enc,
		zval *value)
{
	zend_reference *ref;
	bool is_backref;

	if (php_user_cache_stack_overflowed()) {
		enc->failure_message = "value is nested too deeply to be stored in the user cache";

		return false;
	}

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_NULL);

			return true;
		case IS_FALSE:
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_FALSE);

			return true;
		case IS_TRUE:
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_TRUE);

			return true;
		case IS_LONG:
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_LONG);
			user_cache_serdes_put_long(&enc->buf, Z_LVAL_P(value));

			return true;
		case IS_DOUBLE:
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_DOUBLE);
			user_cache_serdes_put_double(&enc->buf, Z_DVAL_P(value));

			return true;
		case IS_STRING:
			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_STRING);
			if (!user_cache_serdes_put_str(enc, Z_STRVAL_P(value), Z_STRLEN_P(value))) {
				return false;
			}

			return true;
		case IS_ARRAY:
			return user_cache_serdes_encode_array(enc, value);
		case IS_OBJECT:
			return user_cache_serdes_encode_object(enc, value);
		case IS_REFERENCE:
			ref = Z_REF_P(value);

			if (!user_cache_serdes_encode_backref_or_register(enc, ref, &is_backref)) {
				return false;
			}

			if (is_backref) {
				return true;
			}

			user_cache_serdes_put_u8(&enc->buf, PHP_USER_CACHE_SERDES_TAG_REFERENCE);

			return user_cache_serdes_encode_value(enc, &ref->val);
		case IS_RESOURCE:
			enc->failure_message = PHP_USER_CACHE_MSG_RESOURCE_UNSTORABLE;

			return false;
		default:
			enc->failure_message = "value cannot be stored in the user cache";

			return false;
	}
}

static void *user_cache_serdes_registry_reserve(
		void *items,
		void *inline_items,
		uint32_t count,
		uint32_t *cap,
		size_t elem_size)
{
	void *new_items;
	uint32_t new_cap;

	if (count != *cap) {
		return items;
	}

	if (*cap > UINT32_MAX / 2) {
		return NULL;
	}

	new_cap = *cap * 2;
	if (items == inline_items) {
		new_items = safe_emalloc(new_cap, elem_size, 0);
		memcpy(new_items, inline_items, count * elem_size);
	} else {
		new_items = safe_erealloc(items, new_cap, elem_size, 0);
	}

	*cap = new_cap;

	return new_items;
}

static bool user_cache_serdes_decoder_register_string(
		php_user_cache_serdes_decoder *dec,
		zend_string *str)
{
	zend_string **strings;

	if (dec->string_count == UINT32_MAX) {
		return false;
	}

	strings = user_cache_serdes_registry_reserve(
		dec->strings, dec->inline_strings, dec->string_count,
		&dec->string_capacity, sizeof(zend_string *)
	);
	if (strings == NULL) {
		return false;
	}

	dec->strings = strings;
	dec->strings[dec->string_count] = str;
	dec->string_count++;

	return true;
}

static bool user_cache_serdes_decoder_register(
		php_user_cache_serdes_decoder *dec,
		zval *container)
{
	zval *ids;

	if (dec->id_count == UINT32_MAX) {
		return false;
	}

	ids = user_cache_serdes_registry_reserve(
		dec->ids, dec->inline_ids, dec->id_count,
		&dec->id_capacity, sizeof(zval)
	);
	if (ids == NULL) {
		return false;
	}

	dec->ids = ids;
	ZVAL_COPY(&dec->ids[dec->id_count], container);
	dec->id_count++;

	return true;
}

static zend_class_entry *user_cache_serdes_decode_class(
		php_user_cache_serdes_decoder *dec)
{
	zend_string *class_name;
	zend_class_entry *ce;

	if (!user_cache_serdes_get_str(dec, &class_name)) {
		return NULL;
	}

	ce = zend_lookup_class(class_name);
	zend_string_release(class_name);

	return ce;
}

static bool user_cache_serdes_decode_object_properties(
		php_user_cache_serdes_decoder *dec,
		bool call_wakeup,
		zval *dst)
{
	zend_class_entry *ce = Z_OBJCE_P(dst);
	zend_string *prop_name;
	zval *wakeup_zv, prop_val, retval;
	uint32_t count, i, prop_idx_plus_one;

	if (!user_cache_serdes_get_u32(dec, &count)) {
		return false;
	}

	for (i = 0; i < count; i++) {
		if (!user_cache_serdes_get_str(dec, &prop_name)) {
			return false;
		}

		if (!user_cache_serdes_get_u32(dec, &prop_idx_plus_one)) {
			zend_string_release(prop_name);

			return false;
		}

		ZVAL_UNDEF(&prop_val);

		if (!user_cache_serdes_decode_value(dec, &prop_val)) {
			zval_ptr_dtor(&prop_val);
			zend_string_release(prop_name);

			return false;
		}

		if (prop_idx_plus_one != PHP_USER_CACHE_SERDES_PROPERTY_INDEX_NONE) {
			if (!php_user_cache_shared_graph_update_object_property_at(
					dst,
					prop_name,
					prop_idx_plus_one - 1,
					&prop_val
				)
			) {
				zval_ptr_dtor(&prop_val);
				zend_string_release(prop_name);

				return false;
			}
		} else if (!php_user_cache_shared_graph_update_object_property(
				dst,
				prop_name,
				&prop_val
			)
		) {
			zval_ptr_dtor(&prop_val);

			zend_string_release(prop_name);

			return false;
		}

		zval_ptr_dtor(&prop_val);

		zend_string_release(prop_name);
	}

	if (call_wakeup) {
		wakeup_zv = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP));
		if (wakeup_zv != NULL) {
			zend_call_known_instance_method(Z_FUNC_P(wakeup_zv), Z_OBJ_P(dst), &retval, 0, NULL);

			zval_ptr_dtor(&retval);

			if (EG(exception)) {
				return false;
			}
		}
	}

	return true;
}

static bool user_cache_serdes_decode_array(
		php_user_cache_serdes_decoder *dec,
		uint8_t tag,
		zval *dst)
{
	zend_string *key = NULL;
	zval elem;
	uint64_t next_free, key_h = 0;
	uint32_t count, i;
	uint8_t key_kind;
	bool result;

	if (!user_cache_serdes_get_u64(dec, &next_free) ||
		!user_cache_serdes_get_u32(dec, &count)
	) {
		return false;
	}

	/* Reject impossible counts before allocating. */
	if (count > dec->len - dec->pos) {
		return false;
	}

	array_init_size(dst, count);

	/* Initialize before registering the array: registration raises its refcount,
	 * but the first insertion requires a refcount of one. */
	if (tag == PHP_USER_CACHE_SERDES_TAG_PACKED_ARRAY) {
		zend_hash_real_init_packed(Z_ARRVAL_P(dst));
	} else {
		zend_hash_real_init_mixed(Z_ARRVAL_P(dst));
	}

	HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	if (!user_cache_serdes_decoder_register(dec, dst)) {
		zval_ptr_dtor(dst);
		ZVAL_UNDEF(dst);

		return false;
	}

	for (i = 0; i < count; i++) {
		key = NULL;
		if (tag == PHP_USER_CACHE_SERDES_TAG_HASHED_ARRAY) {
			if (!user_cache_serdes_get_u8(dec, &key_kind)) {
				goto failure;
			}

			if (key_kind == PHP_USER_CACHE_SERDES_KEY_STRING) {
				if (!user_cache_serdes_get_str(dec, &key)) {
					goto failure;
				}
			} else if (key_kind == PHP_USER_CACHE_SERDES_KEY_LONG) {
				if (!user_cache_serdes_get_u64(dec, &key_h)) {
					goto failure;
				}
			} else {
				goto failure;
			}
		}

		ZVAL_UNDEF(&elem);
		if (!user_cache_serdes_decode_value(dec, &elem)) {
			zval_ptr_dtor(&elem);
			if (key != NULL) {
				zend_string_release(key);
			}

			goto failure;
		}

		if (key != NULL) {
			/* Reject a corrupt blob with duplicate keys rather than trusting
			 * the encoder's uniqueness; add_new would assert or double-insert. */
			result = zend_hash_add(Z_ARRVAL_P(dst), key, &elem) != NULL;
			zend_string_release(key);
		} else if (tag == PHP_USER_CACHE_SERDES_TAG_HASHED_ARRAY) {
			result = zend_hash_index_add(Z_ARRVAL_P(dst), (zend_ulong) key_h, &elem) != NULL;
		} else {
			result = zend_hash_next_index_insert_new(Z_ARRVAL_P(dst), &elem) != NULL;
		}

		if (!result) {
			zval_ptr_dtor(&elem);

			goto failure;
		}
	}

	Z_ARRVAL_P(dst)->nNextFreeElement = (zend_long) next_free;
	PHP_USER_CACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
	return true;

failure:
	zval_ptr_dtor(dst);
	ZVAL_UNDEF(dst);

	return false;
}

static bool user_cache_serdes_decode_object(
		php_user_cache_serdes_decoder *dec,
		zval *dst)
{
	zend_class_entry *ce;
	uint8_t call_wakeup;

	ce = user_cache_serdes_decode_class(dec);
	if (ce == NULL ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		object_init_ex(dst, ce) != SUCCESS
	) {
		return false;
	}

	if (!user_cache_serdes_get_u8(dec, &call_wakeup) ||
		!user_cache_serdes_decoder_register(dec, dst) ||
		!user_cache_serdes_decode_object_properties(dec, call_wakeup != 0, dst)
	) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	return true;
}

static bool user_cache_serdes_decode_serialized_object(
		php_user_cache_serdes_decoder *dec,
		zval *dst)
{
	zend_class_entry *ce;
	zval state;

	ce = user_cache_serdes_decode_class(dec);
	if (ce == NULL ||
		ce->__unserialize == NULL ||
		(ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) != 0 ||
		object_init_ex(dst, ce) != SUCCESS
	) {
		return false;
	}

	if (!user_cache_serdes_decoder_register(dec, dst)) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	ZVAL_UNDEF(&state);
	if (!user_cache_serdes_decode_value(dec, &state) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	zend_call_known_instance_method_with_1_params(ce->__unserialize, Z_OBJ_P(dst), NULL, &state);

	zval_ptr_dtor(&state);

	if (EG(exception)) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	return true;
}

static bool user_cache_serdes_decode_custom_object(
		php_user_cache_serdes_decoder *dec,
		zval *dst)
{
	const uint8_t *payload;
	zend_class_entry *ce;
	php_unserialize_data_t unser_data;
	uint32_t payload_len;
	bool result;

	ce = user_cache_serdes_decode_class(dec);
	if (ce == NULL ||
		ce->unserialize == NULL ||
		(ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) != 0 ||
		!user_cache_serdes_get_u32(dec, &payload_len) ||
		!user_cache_serdes_get_bytes(dec, payload_len, &payload)
	) {
		return false;
	}

	PHP_VAR_UNSERIALIZE_INIT(unser_data);
	result = ce->unserialize(dst, ce, payload, payload_len, (zend_unserialize_data *) unser_data) == SUCCESS;
	PHP_VAR_UNSERIALIZE_DESTROY(unser_data);

	if (!result || EG(exception)) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	if (!user_cache_serdes_decoder_register(dec, dst)) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	return true;
}

static bool user_cache_serdes_decode_enum(
		php_user_cache_serdes_decoder *dec,
		zval *dst)
{
	zend_string *case_name;
	zend_class_entry *ce;
	zend_object *case_obj;

	ce = user_cache_serdes_decode_class(dec);
	if (ce == NULL ||
		(ce->ce_flags & ZEND_ACC_ENUM) == 0 ||
		!user_cache_serdes_get_str(dec, &case_name)
	) {
		return false;
	}

	case_obj = zend_enum_get_case(ce, case_name);

	zend_string_release(case_name);

	if (case_obj == NULL) {
		return false;
	}

	ZVAL_OBJ_COPY(dst, case_obj);

	return true;
}

static bool user_cache_serdes_decode_reference(
		php_user_cache_serdes_decoder *dec,
		zval *dst)
{
	zend_reference *ref;
	zval val;

	ZVAL_NEW_EMPTY_REF(dst);
	ref = Z_REF_P(dst);
	ZVAL_NULL(&ref->val);

	if (!user_cache_serdes_decoder_register(dec, dst)) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	ZVAL_UNDEF(&val);

	if (!user_cache_serdes_decode_value(dec, &val)) {
		zval_ptr_dtor(&val);
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	zval_ptr_dtor(&ref->val);

	ZVAL_COPY_VALUE(&ref->val, &val);

	return true;
}

static bool user_cache_serdes_decode_value(
		php_user_cache_serdes_decoder *dec,
		zval *dst)
{
	zend_long lval;
	zend_string *str;
	uint32_t backref_id;
	uint8_t tag;
	double dval;

	ZVAL_UNDEF(dst);

	if (php_user_cache_stack_overflowed()) {
		return false;
	}

	if (!user_cache_serdes_get_u8(dec, &tag)) {
		return false;
	}

	switch (tag) {
		case PHP_USER_CACHE_SERDES_TAG_NULL:
			ZVAL_NULL(dst);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_FALSE:
			ZVAL_FALSE(dst);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_TRUE:
			ZVAL_TRUE(dst);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_LONG:
			if (!user_cache_serdes_get_long(dec, &lval)) {
				return false;
			}

			ZVAL_LONG(dst, lval);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_DOUBLE:
			if (!user_cache_serdes_get_double(dec, &dval)) {
				return false;
			}

			ZVAL_DOUBLE(dst, dval);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_STRING:
			if (!user_cache_serdes_get_str(dec, &str)) {
				return false;
			}

			ZVAL_STR(dst, str);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_EMPTY_ARRAY:
			ZVAL_EMPTY_ARRAY(dst);

			return true;
		case PHP_USER_CACHE_SERDES_TAG_PACKED_ARRAY:
		case PHP_USER_CACHE_SERDES_TAG_HASHED_ARRAY:
			return user_cache_serdes_decode_array(dec, tag, dst);
		case PHP_USER_CACHE_SERDES_TAG_OBJECT:
			return user_cache_serdes_decode_object(dec, dst);
		case PHP_USER_CACHE_SERDES_TAG_SERIALIZED_OBJECT:
			return user_cache_serdes_decode_serialized_object(dec, dst);
		case PHP_USER_CACHE_SERDES_TAG_CUSTOM_OBJECT:
			return user_cache_serdes_decode_custom_object(dec, dst);
		case PHP_USER_CACHE_SERDES_TAG_ENUM:
			return user_cache_serdes_decode_enum(dec, dst);
		case PHP_USER_CACHE_SERDES_TAG_REFERENCE:
			return user_cache_serdes_decode_reference(dec, dst);
		case PHP_USER_CACHE_SERDES_TAG_BACKREF:
			if (!user_cache_serdes_get_u32(dec, &backref_id) ||
				backref_id >= dec->id_count
			) {
				return false;
			}

			ZVAL_COPY(dst, &dec->ids[backref_id]);

			return true;
		default:
			return false;
	}
}

uint32_t php_user_cache_serdes_declared_property_index_plus_one(
		zend_class_entry *ce,
		zend_string *name)
{
	zend_property_info *prop_info;
	uint32_t prop_idx;

	if (ZSTR_LEN(name) == 0 ||
		ZSTR_VAL(name)[0] == '\0' ||
		ce->type != ZEND_USER_CLASS ||
		ce->properties_info_table == NULL
	) {
		return PHP_USER_CACHE_SERDES_PROPERTY_INDEX_NONE;
	}

	prop_info = zend_get_property_info(ce, name, true);
	if (prop_info == NULL ||
		prop_info == ZEND_WRONG_PROPERTY_INFO ||
		(prop_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_VIRTUAL)) != 0 ||
		prop_info->offset == ZEND_VIRTUAL_PROPERTY_OFFSET
	) {
		return PHP_USER_CACHE_SERDES_PROPERTY_INDEX_NONE;
	}

	prop_idx = OBJ_PROP_TO_NUM(prop_info->offset);
	if (prop_idx >= ce->default_properties_count || prop_idx == UINT32_MAX) {
		return PHP_USER_CACHE_SERDES_PROPERTY_INDEX_NONE;
	}

	return prop_idx + 1;
}

bool php_user_cache_serdes_get_sleep_state(
		zval *obj_zv,
		zval *state,
		const char **failure_msg)
{
	zend_object *obj = Z_OBJ_P(obj_zv);
	zend_class_entry *ce = obj->ce;
	zval *sleep_zv, retval;
	bool result;

	ZVAL_UNDEF(state);

	if (failure_msg != NULL) {
		*failure_msg = NULL;
	}

	sleep_zv = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP));
	if (sleep_zv == NULL) {
		return false;
	}

	GC_ADDREF(obj);
	zend_call_known_instance_method(Z_FUNC_P(sleep_zv), obj, &retval, 0, NULL);
	OBJ_RELEASE(obj);

	if (Z_ISUNDEF(retval) || EG(exception)) {
		zval_ptr_dtor(&retval);

		return false;
	}

	if (Z_TYPE(retval) != IS_ARRAY) {
		zval_ptr_dtor(&retval);

		php_error_docref(NULL, E_WARNING,
			"%s::__sleep() should return an array only containing the names of instance-variables to serialize",
			ZSTR_VAL(ce->name)
		);

		if (failure_msg != NULL) {
			*failure_msg = "__sleep() did not return an array of member names; the object cannot be stored in the user cache";
		}

		return false;
	}

	/* Failure may leave a partially initialized state array. */
	result = user_cache_serdes_get_sleep_props(obj_zv, Z_ARRVAL(retval), state);
	zval_ptr_dtor(&retval);

	if (!result) {
		zval_ptr_dtor(state);

		ZVAL_UNDEF(state);
	}

	return result;
}

bool php_user_cache_serdes_call_magic_serialize(zend_object *obj, zval *state)
{
	zend_call_known_instance_method_with_0_params(obj->ce->__serialize, obj, state);
	if (EG(exception)) {
		zval_ptr_dtor(state);

		ZVAL_UNDEF(state);

		return false;
	}

	if (Z_TYPE_P(state) != IS_ARRAY) {
		zval_ptr_dtor(state);

		ZVAL_UNDEF(state);

		zend_type_error("%s::__serialize() must return an array", ZSTR_VAL(obj->ce->name));

		return false;
	}

	return true;
}

bool php_user_cache_serdes_encode(zval *value, smart_str *buf, const char **failure_msg)
{
	php_user_cache_serdes_encoder enc;
	bool result;

	if (failure_msg != NULL) {
		*failure_msg = NULL;
	}

	enc.buf = *buf;
	enc.failure_message = NULL;
	enc.string_count = 0;

	zend_hash_init(&enc.seen, 8, NULL, NULL, 0);
	zend_hash_init(&enc.strings, 32, NULL, NULL, 0);

	result = user_cache_serdes_encode_value(&enc, value);

	zend_hash_destroy(&enc.strings);
	zend_hash_destroy(&enc.seen);

	*buf = enc.buf;

	if (!result && failure_msg != NULL) {
		*failure_msg = enc.failure_message;
	}

	return result;
}

bool php_user_cache_serdes_decode(const uint8_t *data, size_t len, zval *dst)
{
	php_user_cache_serdes_decoder dec;
	uint32_t i;
	bool result;

	ZVAL_UNDEF(dst);

	if (data == NULL || len == 0) {
		return false;
	}

	dec.data = data;
	dec.len = len;
	dec.pos = 0;
	dec.ids = dec.inline_ids;
	dec.id_count = 0;
	dec.id_capacity = PHP_USER_CACHE_SERDES_INLINE_IDS;
	dec.strings = dec.inline_strings;
	dec.string_count = 0;
	dec.string_capacity = PHP_USER_CACHE_SERDES_INLINE_STRINGS;

	result = user_cache_serdes_decode_value(&dec, dst);

	if (result && dec.pos != dec.len) {
		result = false;
	}

	for (i = 0; i < dec.id_count; i++) {
		zval_ptr_dtor(&dec.ids[i]);
	}
	for (i = 0; i < dec.string_count; i++) {
		zend_string_release(dec.strings[i]);
	}

	if (dec.ids != dec.inline_ids) {
		efree(dec.ids);
	}
	if (dec.strings != dec.inline_strings) {
		efree(dec.strings);
	}

	if (!result && !Z_ISUNDEF_P(dst)) {
		zval_ptr_dtor(dst);
		ZVAL_UNDEF(dst);
	}

	return result;
}
