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

#ifndef UCACHE_SHARED_GRAPH_H
#define UCACHE_SHARED_GRAPH_H

#include "user_cache_internal.h"

#define UCACHE_SGRAPH_SHAPE_NEXT_FREE_UNSET	UINT32_MAX
#define UCACHE_SGRAPH_ROOT_OFFSET(pin_word_count) \
	((uint32_t) UCACHE_ALIGNED_SIZE(UCACHE_SGRAPH_HDR_SIZE(pin_word_count)))

typedef struct {
	uint8_t type;
	uint8_t flags;
	uint16_t sleep_slot_plus_one;
	union {
		int32_t long_val;
		uint32_t offset;
	};
} ucache_sgraph_val;

typedef struct {
	uint32_t name_offset;
	ucache_sgraph_val val;
} ucache_sgraph_prop;

typedef struct {
	ucache_sgraph_val val;
	uint32_t key;
	uint32_t h_hi;
} ucache_sgraph_arr_elem;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t elems_offset;
	uint32_t flags;
} ucache_sgraph_arr;

typedef struct {
	uint32_t key_offset;
} ucache_sgraph_arr_shape_elem;

typedef struct {
	uint32_t count;
	uint32_t elems_offset;
} ucache_sgraph_arr_shape;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t shape_offset;
} ucache_sgraph_shaped_arr;

typedef struct {
	uint32_t class_name_offset;
	uint32_t prop_count;
	uint32_t props_offset;
	uint32_t flags;
} ucache_sgraph_obj;

typedef struct {
	uint32_t class_name_offset;
	uint32_t shape_offset;
	uint32_t count;
} ucache_sgraph_state_schema;

typedef struct {
	uint32_t state_schema_offset;
	uint32_t flags;
	uint32_t state_vals_offset;
	uint32_t state_next_free;
} ucache_sgraph_shaped_state_obj;

typedef struct {
	uint32_t class_name_offset;
	uint32_t prop_count;
	uint32_t props_offset;
	uint32_t flags;
	ucache_sgraph_val state;
} ucache_sgraph_safe_direct_obj;

typedef struct {
	uint32_t class_name_offset;
	uint32_t flags;
	ucache_sgraph_val state;
} ucache_sgraph_serialized_obj;

typedef struct {
	uint32_t blob_len;
	uint32_t flags;
} ucache_sgraph_serdes_obj;

typedef struct {
	uint32_t flags;
	ucache_sgraph_val inner;
} ucache_sgraph_ref;

typedef struct {
	uint32_t class_name_offset;
	uint32_t case_name_offset;
} ucache_sgraph_enum;

zend_property_info *ucache_sgraph_declared_prop_info(
		zend_class_entry *ce,
		zend_string *name);
#if ZEND_DEBUG
void ucache_sgraph_check_rebase_complete(
		const uint8_t *dst_base,
		size_t glen,
		const uint8_t *src_base);
#endif

static zend_always_inline size_t ucache_sgraph_alignment_padding(const void *buf)
{
	uintptr_t raw_addr, aligned_addr;

	raw_addr = (uintptr_t) buf;
	aligned_addr = (uintptr_t) ZEND_MM_ALIGNED_SIZE(raw_addr);

	return (size_t) (aligned_addr - raw_addr);
}

static zend_always_inline const uint8_t *ucache_sgraph_locate(
		const uint8_t *buf,
		size_t buf_len,
		size_t *glen)
{
	size_t padding;

	padding = ucache_sgraph_alignment_padding(buf);
	if (padding > buf_len || buf_len - padding < UCACHE_SGRAPH_HDR_SIZE(0)) {
		return NULL;
	}

	buf += padding;
	buf_len -= padding;

	if (glen != NULL) {
		*glen = buf_len;
	}

	return buf;
}

static zend_always_inline uint32_t ucache_sgraph_shaped_arr_vals_offset(uint32_t arr_offset)
{
	return arr_offset + (uint32_t) UCACHE_ALIGNED_SIZE(sizeof(ucache_sgraph_shaped_arr));
}

static zend_always_inline bool ucache_sgraph_can_restore_direct(zend_class_entry *ce)
{
	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		return false;
	}

	if (ce->type != ZEND_USER_CLASS && ce->create_object != NULL) {
		return false;
	}

	return true;
}

static zend_always_inline bool ucache_class_has_serialize_handlers(const zend_class_entry *ce)
{
	return ce->serialize != NULL && ce->unserialize != NULL;
}

static zend_always_inline bool ucache_sgraph_wakeup_rebuilds_state(zend_class_entry *ce)
{
	if ((ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		ce->__unserialize != NULL ||
		ucache_class_has_serialize_handlers(ce)
	) {
		return false;
	}

	return zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL;
}

static zend_always_inline bool ucache_sgraph_can_restore_props(zend_class_entry *ce, bool sleep_obj)
{
	return ucache_sgraph_can_restore_direct(ce) ||
		(sleep_obj && ucache_sgraph_wakeup_rebuilds_state(ce))
	;
}

static zend_always_inline bool ucache_sgraph_ptr_in_range(
		const void *ptr,
		const uint8_t *base,
		size_t len)
{
	uintptr_t addr, start;

	if (ptr == NULL || base == NULL || len == 0) {
		return false;
	}

	addr = (uintptr_t) ptr;
	start = (uintptr_t) base;

	return addr >= start && addr - start < len;
}

static zend_always_inline bool ucache_stack_overflowed(void)
{
	bool overflowed = ucache_stack_exhausted();

	if (overflowed) {
		UC_G(stack_overflowed) = true;
	}

	return overflowed;
}

static inline bool ucache_class_overrides_safe_direct_magic_serialize(
		zend_class_entry *ce,
		const php_ucache_safe_direct_handlers *handlers,
		zend_class_entry *base_ce)
{
	if (ce->type != ZEND_USER_CLASS ||
		ce->__serialize == NULL ||
		ce->__unserialize == NULL ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		handlers == NULL ||
		base_ce == ce
	) {
		return false;
	}

	return
		(
			ce->__serialize->type == ZEND_USER_FUNCTION ||
			ce->__unserialize->type == ZEND_USER_FUNCTION
		) &&
		(
			ce->__serialize->common.scope != base_ce ||
			ce->__unserialize->common.scope != base_ce
		)
	;
}

#endif /* UCACHE_SHARED_GRAPH_H */
