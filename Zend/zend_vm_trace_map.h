/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend_vm_handlers.h"
#include "zend_sort.h"

#define GEN_MAP(n, name) do { \
		ZVAL_LONG(&tmp, (zend_long)(uintptr_t)zend_opcode_handlers[n]); \
		zend_hash_str_add(&vm_trace_ht, #name, sizeof(#name) - 1, &tmp); \
	} while (0);

#define VM_TRACE_START() do { \
		zval tmp; \
		zend_hash_init(&vm_trace_ht, 0, NULL, NULL, 1); \
		VM_HANDLERS(GEN_MAP) \
		zend_vm_trace_init(); \
	} while (0)

#ifdef _WIN64
# define ADDR_FMT "%016I64x"
#elif SIZEOF_SIZE_T == 4
# define ADDR_FMT "%08zx"
#elif SIZEOF_SIZE_T == 8
# define ADDR_FMT "%016zx"
#else
# error "Unknown SIZEOF_SIZE_T"
#endif

static HashTable vm_trace_ht;

static int zend_vm_trace_compare(const Bucket *p1, const Bucket *p2)
{
	if (Z_LVAL(p1->val) > Z_LVAL(p2->val)) {
		return 1;
	} else if (Z_LVAL(p1->val) < Z_LVAL(p2->val)) {
		return -1;
	} else {
		return 0;
	}
}

static void zend_vm_trace_init(void)
{
	FILE *f;
	zend_string *key, *prev_key;
	zval *val;
	zend_long prev_addr;

	f = fopen("zend_vm.map", "w+");
	if (f) {
		zend_hash_sort(&vm_trace_ht, (compare_func_t)zend_vm_trace_compare, 0);
		prev_key = NULL;
		ZEND_HASH_FOREACH_STR_KEY_VAL(&vm_trace_ht, key, val) {
			if (prev_key) {
				fprintf(f, ADDR_FMT" "ADDR_FMT" t %s\n", prev_addr, Z_LVAL_P(val) - prev_addr, ZSTR_VAL(prev_key));
			}
			prev_key  = key;
			prev_addr = Z_LVAL_P(val);
		} ZEND_HASH_FOREACH_END();
		if (prev_key) {
			fprintf(f, ADDR_FMT" "ADDR_FMT" t %s\n", prev_addr, 0, ZSTR_VAL(prev_key));
		}
		fclose(f);
	}
	zend_hash_destroy(&vm_trace_ht);
}
