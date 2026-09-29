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

#include "Zend/zend_fibers.h"

typedef struct {
	struct php_serialize_data *serialize_data;
	struct php_unserialize_data *unserialize_data;
	unsigned serialize_level;
	unsigned unserialize_level;
	unsigned serialize_lock;
} ucache_serdes_native_ctxs;

static zend_always_inline void ucache_serdes_native_ctxs_isolate(ucache_serdes_native_ctxs *outer)
{
	zend_fiber_switch_block();

	outer->serialize_data = BG(serialize).data;
	outer->serialize_level = BG(serialize).level;
	outer->unserialize_data = BG(unserialize).data;
	outer->unserialize_level = BG(unserialize).level;
	outer->serialize_lock = BG(serialize_lock);

	BG(serialize).data = NULL;
	BG(serialize).level = 0;
	BG(unserialize).data = NULL;
	BG(unserialize).level = 0;
	BG(serialize_lock) = 0;
}

static zend_always_inline void ucache_serdes_native_ctxs_restore(const ucache_serdes_native_ctxs *outer)
{
	BG(serialize).data = outer->serialize_data;
	BG(serialize).level = outer->serialize_level;
	BG(unserialize).data = outer->unserialize_data;
	BG(unserialize).level = outer->unserialize_level;
	BG(serialize_lock) = outer->serialize_lock;

	zend_fiber_switch_unblock();
}

static zend_always_inline void ucache_serdes_put_bytes(smart_str *buf, const char *bytes, uint32_t len)
{
	smart_str_appendl(buf, (const char *) &len, sizeof(len));
	smart_str_appendl(buf, bytes, len);
}

static zend_always_inline bool ucache_serdes_get_bytes(
		const uint8_t *data,
		size_t len,
		size_t *pos,
		const uint8_t **bytes,
		uint32_t *bytes_len)
{
	if (sizeof(*bytes_len) > len - *pos) {
		return false;
	}

	memcpy(bytes_len, data + *pos, sizeof(*bytes_len));
	*pos += sizeof(*bytes_len);

	if (*bytes_len > len - *pos) {
		return false;
	}

	*bytes = data + *pos;
	*pos += *bytes_len;

	return true;
}

static zend_class_entry *ucache_serdes_lookup_class(const uint8_t *name, uint32_t name_len)
{
	zend_string *class_name = zend_string_init((const char *) name, name_len, false);
	zend_class_entry *ce;

	BG(serialize_lock)++;
	ce = zend_lookup_class(class_name);
	BG(serialize_lock)--;

	zend_string_release(class_name);

	if (ce == NULL ||
		ce->unserialize == NULL ||
		(ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) != 0 ||
		!ucache_owned_decode_validate_proc()
	) {
		return NULL;
	}

	return ce;
}

bool ucache_serdes_encode(zval *val, smart_str *buf)
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	ucache_serdes_native_ctxs outer;
	php_serialize_data_t ser_data;
	uint8_t *ser_buf = NULL;
	size_t ser_len = 0;
	bool result;

	ZEND_ASSERT(ce->serialize != NULL && ce->unserialize != NULL);

	ucache_serdes_native_ctxs_isolate(&outer);

	PHP_VAR_SERIALIZE_INIT(ser_data);
	zend_try {
		result = ce->serialize(val, &ser_buf, &ser_len, (zend_serialize_data *) ser_data) == SUCCESS;
	} zend_catch {
		ucache_serdes_native_ctxs_restore(&outer);

		zend_bailout();
	} zend_end_try();
	PHP_VAR_SERIALIZE_DESTROY(ser_data);

	ucache_serdes_native_ctxs_restore(&outer);

	if (!result || ser_len > UINT32_MAX || ZSTR_LEN(ce->name) > UINT32_MAX) {
		if (ser_buf != NULL) {
			efree(ser_buf);
		}

		if (!EG(exception)) {
			zend_type_error(
				"The %s object could not be serialized for the user cache",
				ZSTR_VAL(ce->name)
			);
		}

		return false;
	}

	ucache_serdes_put_bytes(buf, ZSTR_VAL(ce->name), (uint32_t) ZSTR_LEN(ce->name));
	ucache_serdes_put_bytes(buf, (const char *) ser_buf, (uint32_t) ser_len);

	if (ser_buf != NULL) {
		efree(ser_buf);
	}

	return true;
}

bool ucache_serdes_decode(const uint8_t *data, size_t len, zval *dst)
{
	const uint8_t *class_name, *payload;
	zend_string *owned_payload;
	zend_class_entry *ce;
	ucache_serdes_native_ctxs outer;
	php_unserialize_data_t unser_data;
	uint32_t class_name_len, payload_len;
	size_t pos = 0;
	bool result;

	ZVAL_UNDEF(dst);

	if (!ucache_serdes_get_bytes(data, len, &pos, &class_name, &class_name_len) ||
		!ucache_serdes_get_bytes(data, len, &pos, &payload, &payload_len) ||
		pos != len
	) {
		return false;
	}

	owned_payload = zend_string_init((const char *) payload, payload_len, false);

	ce = ucache_serdes_lookup_class(class_name, class_name_len);
	if (ce == NULL) {
		zend_string_release(owned_payload);

		return false;
	}

	ucache_serdes_native_ctxs_isolate(&outer);

	PHP_VAR_UNSERIALIZE_INIT(unser_data);
	zend_try {
		result = ce->unserialize(
			dst,
			ce,
			(const unsigned char *) ZSTR_VAL(owned_payload),
			ZSTR_LEN(owned_payload),
			(zend_unserialize_data *) &unser_data
		) == SUCCESS;
	} zend_catch {
		ucache_serdes_native_ctxs_restore(&outer);

		zend_bailout();
	} zend_end_try();
	PHP_VAR_UNSERIALIZE_DESTROY(unser_data);

	ucache_serdes_native_ctxs_restore(&outer);

	zend_string_release(owned_payload);

	if (!result || EG(exception) || !ucache_owned_decode_validate_proc()) {
		zval_ptr_dtor(dst);

		ZVAL_UNDEF(dst);

		return false;
	}

	return true;
}
