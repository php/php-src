/*
 * IR - Lightweight JIT Compilation Framework
 * (String table)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#include "ir.h"
#include "ir_private.h"

typedef struct _ir_strtab_bucket {
	uint32_t    h;
	uint32_t    len;
	const char *str;
	uint32_t    next;
	ir_ref      val;
} ir_strtab_bucket;

static uint32_t ir_str_hash(const char *str, size_t len)
{
	size_t i;
	uint32_t h = 5381;

    for (i = 0; i < len; i++) {
        h = ((h << 5) + h) + *str;
        str++;
    }
    return h | 0x10000000;
}

static uint32_t ir_strtab_hash_size(uint32_t size)
{
	/* Use big enough power of 2 */
	size -= 1;
	size |= (size >> 1);
	size |= (size >> 2);
	size |= (size >> 4);
	size |= (size >> 8);
	size |= (size >> 16);
	return size + 1;
}

static void ir_strtab_resize(ir_strtab *strtab)
{
	uint32_t old_hash_size = (uint32_t)(-(int32_t)strtab->mask);
	char *old_data = strtab->data;
	uint32_t size = strtab->size * 2;
	uint32_t hash_size = ir_strtab_hash_size(size);
	char *data = ir_mem_malloc(hash_size * sizeof(uint32_t) + size * sizeof(ir_strtab_bucket));
	ir_strtab_bucket *p;
	uint32_t pos, i;

	memset(data, IR_INVALID_IDX, hash_size * sizeof(uint32_t));
	strtab->data = data + (hash_size * sizeof(uint32_t));
	strtab->mask = (uint32_t)(-(int32_t)hash_size);
	strtab->size = size;

	memcpy(strtab->data, old_data, strtab->count * sizeof(ir_strtab_bucket));
	ir_mem_free(old_data - (old_hash_size * sizeof(uint32_t)));

	i = strtab->count;
	pos = 0;
	p = (ir_strtab_bucket*)strtab->data;
	do {
		uint32_t h = p->h | strtab->mask;
		p->next = ((uint32_t*)strtab->data)[(int32_t)h];
		((uint32_t*)strtab->data)[(int32_t)h] = pos;
		pos += sizeof(ir_strtab_bucket);
		p++;
	} while (--i);
}

static void ir_strtab_grow_buf(ir_strtab *strtab, uint32_t len)
{
	size_t old = (size_t)strtab->buf;

	do {
		strtab->buf_size *= 2;
	} while (UNEXPECTED(strtab->buf_size - strtab->buf_top < len + 1));

	strtab->buf = ir_mem_realloc(strtab->buf, strtab->buf_size);
	if ((size_t)strtab->buf != old) {
		size_t offset = (size_t)strtab->buf - old;
		ir_strtab_bucket *p = (ir_strtab_bucket*)strtab->data;
		uint32_t i;
		for (i = strtab->count; i > 0; i--) {
			p->str += offset;
			p++;
		}
	}
}

void ir_strtab_init(ir_strtab *strtab, uint32_t size, uint32_t buf_size)
{
	IR_ASSERT(size > 0);
	uint32_t hash_size = ir_strtab_hash_size(size);
	char *data = ir_mem_malloc(hash_size * sizeof(uint32_t) + size * sizeof(ir_strtab_bucket));
	memset(data, IR_INVALID_IDX, hash_size * sizeof(uint32_t));
	strtab->data = (data + (hash_size * sizeof(uint32_t)));
	strtab->mask = (uint32_t)(-(int32_t)hash_size);
	strtab->size = size;
	strtab->count = 0;
	strtab->pos = 0;
	if (buf_size) {
		strtab->buf = ir_mem_malloc(buf_size);
		strtab->buf_size = buf_size;
		strtab->buf_top = 0;
	} else {
		strtab->buf = NULL;
		strtab->buf_size = 0;
		strtab->buf_top = 0;
	}
}

ir_ref ir_strtab_find(const ir_strtab *strtab, const char *str, uint32_t len)
{
	uint32_t h = ir_str_hash(str, len);
	const char *data = (const char*)strtab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(h | strtab->mask)];
	ir_strtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_strtab_bucket*)(data + pos);
		if (p->h == h
		 && p->len == len
		 && memcmp(p->str, str, len) == 0) {
			return p->val;
		}
		pos = p->next;
	}
	return 0;
}

ir_ref ir_strtab_lookup(ir_strtab *strtab, const char *str, uint32_t len, ir_ref val)
{
	uint32_t h = ir_str_hash(str, len);
	char *data = (char*)strtab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(h | strtab->mask)];
	ir_strtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_strtab_bucket*)(data + pos);
		if (p->h == h
		 && p->len == len
		 && memcmp(p->str, str, len) == 0) {
			return p->val;
		}
		pos = p->next;
	}

	IR_ASSERT(val != 0);

	if (UNEXPECTED(strtab->count >= strtab->size)) {
		ir_strtab_resize(strtab);
		data = strtab->data;
	}

	if (strtab->buf) {
		if (UNEXPECTED(strtab->buf_size - strtab->buf_top < len + 1)) {
			ir_strtab_grow_buf(strtab, len + 1);
		}

		memcpy(strtab->buf + strtab->buf_top, str, len);
		strtab->buf[strtab->buf_top + len] = 0;
		str = (const char*)strtab->buf + strtab->buf_top;
		strtab->buf_top += len + 1;
	}

	pos = strtab->pos;
	strtab->pos += sizeof(ir_strtab_bucket);
	strtab->count++;
	p = (ir_strtab_bucket*)(data + pos);
	p->h = h;
	p->len = len;
	p->str = str;
	h |= strtab->mask;
	p->next = ((uint32_t*)data)[(int32_t)h];
	((uint32_t*)data)[(int32_t)h] = pos;
	p->val = val;
	return val;
}

ir_ref ir_strtab_update(ir_strtab *strtab, const char *str, uint32_t len, ir_ref val)
{
	uint32_t h = ir_str_hash(str, len);
	char *data = (char*)strtab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(h | strtab->mask)];
	ir_strtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_strtab_bucket*)(data + pos);
		if (p->h == h
		 && p->len == len
		 && memcmp(p->str, str, len) == 0) {
			return p->val = val;
		}
		pos = p->next;
	}
	return 0;
}

const char *ir_strtab_str(const ir_strtab *strtab, ir_ref idx)
{
	IR_ASSERT(idx >= 0 && (uint32_t)idx < strtab->count);
	return ((const ir_strtab_bucket*)strtab->data)[idx].str;
}

const char *ir_strtab_strl(const ir_strtab *strtab, ir_ref idx, size_t *len)
{
	const ir_strtab_bucket *b = ((const ir_strtab_bucket*)strtab->data) + idx;
	IR_ASSERT(idx >= 0 && (uint32_t)idx < strtab->count);
	*len = b->len;
	return b->str;
}

void ir_strtab_free(ir_strtab *strtab)
{
	uint32_t hash_size = (uint32_t)(-(int32_t)strtab->mask);
	char *data = (char*)strtab->data - (hash_size * sizeof(uint32_t));
	ir_mem_free(data);
	strtab->data = NULL;
	if (strtab->buf) {
		ir_mem_free(strtab->buf);
		strtab->buf = NULL;
	}
}

void ir_strtab_apply(const ir_strtab *strtab, ir_strtab_apply_t func)
{
	uint32_t i;

	for (i = 0; i < strtab->count; i++) {
		const ir_strtab_bucket *b = &((ir_strtab_bucket*)strtab->data)[i];
		func(b->str, b->len, b->val);
	}
}
