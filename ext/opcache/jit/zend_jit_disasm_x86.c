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

#include "zend_elf.h"

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
	((void)ud);
	void *a = (void*)(zend_uintptr_t)(addr);
	Dl_info info;

#ifndef ZTS
	if (a > (void*)&executor_globals && a < (void*)((char*)&executor_globals + sizeof(executor_globals))) {
		*offset = (int64_t)((char*)a - (char*)&executor_globals);
		return "executor_globals";
	}
	if (a > (void*)&compiler_globals && a < (void*)((char*)&compiler_globals + sizeof(compiler_globals))) {
		*offset = (int64_t)((char*)a - (char*)&compiler_globals);
		return "compiler_globals";
	}
#endif

	if (dladdr(a, &info)
	 && info.dli_sname != NULL
	 && info.dli_saddr == a) {
		return info.dli_sname;
	}

	return zend_elf_resolve_sym((void*)addr);
}

static int zend_jit_disasm(const char *name,
                           const char *filename,
                           const void *start,
                           size_t      size)
{
	const void *end = (void *)((char *)start + size);
	if (name) {
		fprintf(stderr, "%s: ; (%s)\n", name, filename ? filename : "unknown");
	}

	ud_set_input_buffer(&ud, (uint8_t*)start, (uint8_t*)end - (uint8_t*)start);
	ud_set_pc(&ud, (uint64_t)(uintptr_t)start);

	while (ud_disassemble(&ud)) {
		fprintf(stderr, "0x%" PRIx64 ":\t%s\n", ud_insn_off(&ud), ud_insn_asm(&ud));
	}
	fprintf(stderr, "\n");

	return 1;
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
