/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#define HAVE_DISASM 1
#define DISASM_INTEL_SYNTAX 0

#include "jit/libudis86/itab.c"
#include "jit/libudis86/decode.c"
#include "jit/libudis86/syn.c"
#if DISASM_INTEL_SYNTAX
# include "jit/libudis86/syn-intel.c"
#else
# include "jit/libudis86/syn-att.c"
#endif
#include "jit/libudis86/udis86.c"

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <dlfcn.h>

static struct ud ud;

static const char* zend_jit_disasm_resolver(struct ud *ud, 
                                            uint64_t   addr,
                                            int64_t   *offset)
{
	void *a = (void*)(zend_uintptr_t)(addr);
	Dl_info info;	

	if (dladdr(a, &info)
	 && info.dli_sname != NULL
	 && info.dli_saddr == a) {
		return info.dli_sname;
	}

	return NULL;
}

static int zend_jit_disasm(zend_op_array *op_array,
                           const void    *start,
                           const void    *end)
{
	if (op_array) {
		if (op_array->function_name) {
			if (op_array->scope) {
				fprintf(stderr, "$_%s::%s: ; (%s)\n", ZSTR_VAL(op_array->scope->name), ZSTR_VAL(op_array->function_name), op_array->filename ? ZSTR_VAL(op_array->filename) : "unknown");
			} else {
				fprintf(stderr, "$_%s: ; (%s)\n", ZSTR_VAL(op_array->function_name), ZSTR_VAL(op_array->filename), op_array->filename ? ZSTR_VAL(op_array->filename) : "unknown");
			}
		} else if (op_array->filename) {
			fprintf(stderr, "$_: ; (%s)\n", ZSTR_VAL(op_array->filename));
		}
	}
				
	ud_set_input_buffer(&ud, (uint8_t*)start, (uint8_t*)end - (uint8_t*)start);
	ud_set_pc(&ud, (uint64_t)(uintptr_t)start);

	while (ud_disassemble(&ud)) {
		fprintf(stderr, "0x%" PRIx64 ":\t%s\n", ud_insn_off(&ud), ud_insn_asm(&ud));
	}
	fprintf(stderr, "\n");
}

static int zend_jit_disasm_init(void)
{
	ud_init(&ud);
#ifdef __x86_64__
	ud_set_mode(&ud, 64);
#else
	ud_set_mode(&ud, 32);
#endif
#if DISASM_INTEL_SYNTAX
	ud_set_syntax(&ud, UD_SYN_INTEL);
#else
	ud_set_syntax(&ud, UD_SYN_ATT);
#endif
	ud_set_sym_resolver(&ud, zend_jit_disasm_resolver);

	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
