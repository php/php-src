/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_sort.h"

#define VM_TRACE(op)     zend_vm_trace(#op, sizeof(#op)-1);
#define VM_TRACE_START() zend_vm_trace_init();
#define VM_TRACE_END()   zend_vm_trace_finish();

static HashTable vm_trace_ht;

static void zend_vm_trace(const char *op, size_t op_len)
{
	static const char *last = NULL;
	static size_t last_len = 0;
	char buf[256];
	size_t len;
	zval tmp, *zv;

	if (EXPECTED(last)) {
		len = last_len + 1 + op_len;
		memcpy(buf, last, last_len);
		buf[last_len] = ' ';
		memcpy(buf + last_len + 1, op, op_len + 1);
		zv = zend_hash_str_find(&vm_trace_ht, buf, len);
		if (EXPECTED(zv)) {
			if (EXPECTED(Z_LVAL_P(zv) < ZEND_LONG_MAX)) {
				Z_LVAL_P(zv)++;
			}
		} else {
			ZVAL_LONG(&tmp, 1);
			zend_hash_str_add_new(&vm_trace_ht, buf, len, &tmp);
		}
	}
	last = op;
	last_len = op_len;
}

static int zend_vm_trace_compare(const Bucket *p1, const Bucket *p2)
{
	if (Z_LVAL(p1->val) < Z_LVAL(p2->val)) {
		return 1;
	} else if (Z_LVAL(p1->val) > Z_LVAL(p2->val)) {
		return -1;
	} else {
		return 0;
	}
}

static void zend_vm_trace_finish(void)
{
	zend_string *key;
	zval *val;
	FILE *f;

	f = fopen("zend_vm_trace.log", "w+");
	if (f) {
		zend_hash_sort(&vm_trace_ht, (compare_func_t)zend_vm_trace_compare, 0);
		ZEND_HASH_FOREACH_STR_KEY_VAL(&vm_trace_ht, key, val) {
			fprintf(f, "%s "ZEND_LONG_FMT"\n", ZSTR_VAL(key), Z_LVAL_P(val));
		} ZEND_HASH_FOREACH_END();
		fclose(f);
	}
	zend_hash_destroy(&vm_trace_ht);
}

static void zend_vm_trace_init(void)
{
	FILE *f;

	zend_hash_init(&vm_trace_ht, 0, NULL, NULL, 1);
	f = fopen("zend_vm_trace.log", "r");
	if (f) {
		char buf[256];
		size_t len;
		zval tmp;

		while (!feof(f)) {
			if (fgets(buf, sizeof(buf)-1, f)) {
				len = strlen(buf);
				while (len > 0 && buf[len-1] <= ' ') {
					len--;
					buf[len] = 0;
				}
				while (len > 0 && buf[len-1] >= '0' && buf[len-1] <= '9') {
					len--;
				}
				if (len > 1) {
					buf[len-1] = 0;
					ZVAL_LONG(&tmp, ZEND_STRTOL(buf + len, NULL, 10));
					zend_hash_str_add(&vm_trace_ht, buf, len - 1, &tmp);
				}
			}
		}
		fclose(f);
	}
}
