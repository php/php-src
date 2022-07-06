/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
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

static void zend_jit_disasm_add_symbol(const char *name,
                                       uint64_t    addr,
                                       uint64_t    size);

#ifndef _WIN32
# include "jit/zend_elf.c"
#endif

#include "zend_sort.h"

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#ifndef _WIN32
#include <dlfcn.h>
#endif

static struct ud ud;

struct _sym_node {
	uint64_t          addr;
	uint64_t          end;
	struct _sym_node *parent;
	struct _sym_node *child[2];
	unsigned char     info;
	char              name[1];
};

static void zend_syms_rotateleft(zend_sym_node *p) {
	zend_sym_node *r = p->child[1];
	p->child[1] = r->child[0];
	if (r->child[0]) {
		r->child[0]->parent = p;
	}
	r->parent = p->parent;
	if (p->parent == NULL) {
		JIT_G(symbols) = r;
	} else if (p->parent->child[0] == p) {
		p->parent->child[0] = r;
	} else {
		p->parent->child[1] = r;
	}
	r->child[0] = p;
	p->parent = r;
}

static void zend_syms_rotateright(zend_sym_node *p) {
	zend_sym_node *l = p->child[0];
	p->child[0] = l->child[1];
	if (l->child[1]) {
		l->child[1]->parent = p;
	}
	l->parent = p->parent;
	if (p->parent == NULL) {
		JIT_G(symbols) = l;
	} else if (p->parent->child[1] == p) {
		p->parent->child[1] = l;
	} else {
		p->parent->child[0] = l;
	}
	l->child[1] = p;
	p->parent = l;
}

static void zend_jit_disasm_add_symbol(const char *name,
                                       uint64_t    addr,
                                       uint64_t    size)
{
	zend_sym_node *sym;
	size_t len = strlen(name);

	sym = malloc(sizeof(zend_sym_node) + len + 1);
	if (!sym) {
		return;
	}
	sym->addr = addr;
	sym->end  = (addr + size - 1);
	memcpy((char*)&sym->name, name, len + 1);
	sym->parent = sym->child[0] = sym->child[1] = NULL;
	sym->info = 1;
	if (JIT_G(symbols)) {
		zend_sym_node *node = JIT_G(symbols);

		/* insert it into rbtree */
		do {
			if (sym->addr > node->addr) {
				ZEND_ASSERT(sym->addr > (node->end));
				if (node->child[1]) {
					node = node->child[1];
				} else {
					node->child[1] = sym;
					sym->parent = node;
					break;
				}
			} else if (sym->addr < node->addr) {
				if (node->child[0]) {
					node = node->child[0];
				} else {
					node->child[0] = sym;
					sym->parent = node;
					break;
				}
			} else {
				ZEND_ASSERT(sym->addr == node->addr);
				if (strcmp(name, node->name) == 0 && sym->end < node->end) {
					/* reduce size of the existing symbol */
					node->end = sym->end;
				}
				free(sym);
				return;
			}
		} while (1);

		/* fix rbtree after instering */
		while (sym && sym != JIT_G(symbols) && sym->parent->info == 1) {
			if (sym->parent == sym->parent->parent->child[0]) {
				node = sym->parent->parent->child[1];
				if (node && node->info == 1) {
					sym->parent->info = 0;
					node->info = 0;
					sym->parent->parent->info = 1;
					sym = sym->parent->parent;
				} else {
					if (sym == sym->parent->child[1]) {
						sym = sym->parent;
						zend_syms_rotateleft(sym);
					}
					sym->parent->info = 0;
					sym->parent->parent->info = 1;
					zend_syms_rotateright(sym->parent->parent);
				}
			} else {
				node = sym->parent->parent->child[0];
				if (node && node->info == 1) {
					sym->parent->info = 0;
					node->info = 0;
					sym->parent->parent->info = 1;
					sym = sym->parent->parent;
				} else {
					if (sym == sym->parent->child[0]) {
						sym = sym->parent;
						zend_syms_rotateright(sym);
					}
					sym->parent->info = 0;
					sym->parent->parent->info = 1;
					zend_syms_rotateleft(sym->parent->parent);
				}
			}
		}
	} else {
		JIT_G(symbols) = sym;
	}
	JIT_G(symbols)->info = 0;
}

static void zend_jit_disasm_destroy_symbols(zend_sym_node *n) {
	if (n) {
		if (n->child[0]) {
			zend_jit_disasm_destroy_symbols(n->child[0]);
		}
		if (n->child[1]) {
			zend_jit_disasm_destroy_symbols(n->child[1]);
		}
		free(n);
	}
}

static const char* zend_jit_disasm_find_symbol(uint64_t  addr,
                                               int64_t  *offset) {
	zend_sym_node *node = JIT_G(symbols);
	while (node) {
		if (addr < node->addr) {
			node = node->child[0];
		} else if (addr > node->end) {
			node = node->child[1];
		} else {
			*offset = addr - node->addr;
			return node->name;
		}
	}
	return NULL;
}

static const char* zend_jit_disasm_resolver(struct ud *ud,
                                            uint64_t   addr,
                                            int64_t   *offset)
{
#ifndef _WIN32
	((void)ud);
	const char *name;
	void *a = (void*)(zend_uintptr_t)(addr);
	Dl_info info;

	name = zend_jit_disasm_find_symbol(addr, offset);
	if (name) {
		return name;
	}

	if (dladdr(a, &info)
	 && info.dli_sname != NULL
	 && info.dli_saddr == a) {
		return info.dli_sname;
	}
#else
	const char *name;
	name = zend_jit_disasm_find_symbol(addr, offset);
	if (name) {
		return name;
	}
#endif

	return NULL;
}

static int zend_jit_cmp_labels(Bucket *b1, Bucket *b2)
{
	return ((b1->h > b2->h) > 0) ? 1 : -1;
}

static int zend_jit_disasm(const char    *name,
                           const char    *filename,
                           const zend_op_array *op_array,
                           zend_cfg      *cfg,
                           const void    *start,
                           size_t         size)
{
	const void *end = (void *)((char *)start + size);
	zval zv, *z;
	zend_long n, m;
	HashTable labels;
	const struct ud_operand *op;
	uint64_t addr;
	int b;

	if (name) {
		fprintf(stderr, "%s: ; (%s)\n", name, filename ? filename : "unknown");
	}

	ud_set_input_buffer(&ud, (uint8_t*)start, (uint8_t*)end - (uint8_t*)start);
	ud_set_pc(&ud, (uint64_t)(uintptr_t)start);

	zend_hash_init(&labels, 8, NULL, NULL, 0);
	if (op_array && cfg) {
		ZVAL_FALSE(&zv);
		for (b = 0; b < cfg->blocks_count; b++) {
			if (cfg->blocks[b].flags & (ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY)) {
				addr = (uint64_t)(uintptr_t)op_array->opcodes[cfg->blocks[b].start].handler;
				if (addr >= (uint64_t)(uintptr_t)start && addr < (uint64_t)(uintptr_t)end) {
					zend_hash_index_add(&labels, addr, &zv);
				}
			}
		}
	}
	ZVAL_TRUE(&zv);
	while (ud_disassemble(&ud)) {
		op = ud_insn_opr(&ud, 0);
		if (op && op->type == UD_OP_JIMM) {
			addr = ud_syn_rel_target(&ud, (struct ud_operand*)op);
			if (addr >= (uint64_t)(uintptr_t)start && addr < (uint64_t)(uintptr_t)end) {
				zend_hash_index_add(&labels, addr, &zv);
			}
		}
	}

	zend_hash_sort(&labels, zend_jit_cmp_labels, 0);

	/* label numbering */
	n = 0; m = 0;
	ZEND_HASH_FOREACH_VAL(&labels, z) {
		if (Z_TYPE_P(z) == IS_FALSE) {
			m--;
			ZVAL_LONG(z, m);
		} else {
			n++;
			ZVAL_LONG(z, n);
		}
	} ZEND_HASH_FOREACH_END();

	ud_set_input_buffer(&ud, (uint8_t*)start, (uint8_t*)end - (uint8_t*)start);
	ud_set_pc(&ud, (uint64_t)(uintptr_t)start);

	while (ud_disassemble(&ud)) {
		addr = ud_insn_off(&ud);
		z = zend_hash_index_find(&labels, addr);
		if (z) {
			if (Z_LVAL_P(z) < 0) {
				fprintf(stderr, ".ENTRY" ZEND_LONG_FMT ":\n", -Z_LVAL_P(z));
			} else {
				fprintf(stderr, ".L" ZEND_LONG_FMT ":\n", Z_LVAL_P(z));
			}
		}
		op = ud_insn_opr(&ud, 0);
		if (op && op->type == UD_OP_JIMM) {
			addr = ud_syn_rel_target(&ud, (struct ud_operand*)op);
			if (addr >= (uint64_t)(uintptr_t)start && addr < (uint64_t)(uintptr_t)end) {
				z = zend_hash_index_find(&labels, addr);
				if (z) {
					const char *str = ud_insn_asm(&ud);
					int len;

					len = 0;
					while (str[len] != 0 && str[len] != ' ' && str[len] != '\t') {
						len++;
					}
					if (str[len] != 0) {
						while (str[len] == ' ' || str[len] == '\t') {
							len++;
						}
						if (Z_LVAL_P(z) < 0) {
							fprintf(stderr, "\t%.*s.ENTRY" ZEND_LONG_FMT "\n", len, str, -Z_LVAL_P(z));
						} else {
							fprintf(stderr, "\t%.*s.L" ZEND_LONG_FMT "\n", len, str, Z_LVAL_P(z));
						}
						continue;
					}
				}
			}
		}
		fprintf(stderr, "\t%s\n", ud_insn_asm(&ud));
	}
	fprintf(stderr, "\n");

	zend_hash_destroy(&labels);

	return 1;
}

static int zend_jit_disasm_init(void)
{
	ud_init(&ud);
#if defined(__x86_64__) || defined(_WIN64)
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

#ifndef ZTS
#define REGISTER_EG(n)  \
	zend_jit_disasm_add_symbol("EG("#n")", \
		(uint64_t)(uintptr_t)&executor_globals.n, sizeof(executor_globals.n))
	REGISTER_EG(uninitialized_zval);
	REGISTER_EG(exception);
	REGISTER_EG(vm_interrupt);
	REGISTER_EG(exception_op);
	REGISTER_EG(timed_out);
	REGISTER_EG(current_execute_data);
	REGISTER_EG(vm_stack_top);
	REGISTER_EG(vm_stack_end);
	REGISTER_EG(symbol_table);
	REGISTER_EG(jit_trace_num);
#undef  REGISTER_EG
#endif

	/* Register JIT helper functions */
#define REGISTER_HELPER(n)  \
	zend_jit_disasm_add_symbol(#n, \
		(uint64_t)(uintptr_t)n, sizeof(void*));
	REGISTER_HELPER(memcmp);
	REGISTER_HELPER(zend_jit_init_func_run_time_cache_helper);
	REGISTER_HELPER(zend_jit_find_func_helper);
	REGISTER_HELPER(zend_jit_find_ns_func_helper);
	REGISTER_HELPER(zend_jit_find_method_helper);
	REGISTER_HELPER(zend_jit_find_method_tmp_helper);
	REGISTER_HELPER(zend_jit_push_static_metod_call_frame);
	REGISTER_HELPER(zend_jit_push_static_metod_call_frame_tmp);
	REGISTER_HELPER(zend_jit_invalid_method_call);
	REGISTER_HELPER(zend_jit_invalid_method_call_tmp);
	REGISTER_HELPER(zend_jit_unref_helper);
	REGISTER_HELPER(zend_jit_extend_stack_helper);
	REGISTER_HELPER(zend_jit_int_extend_stack_helper);
	REGISTER_HELPER(zend_jit_leave_nested_func_helper);
	REGISTER_HELPER(zend_jit_leave_top_func_helper);
	REGISTER_HELPER(zend_jit_leave_func_helper);
	REGISTER_HELPER(zend_jit_symtable_find);
	REGISTER_HELPER(zend_jit_hash_index_lookup_rw_no_packed);
	REGISTER_HELPER(zend_jit_hash_index_lookup_rw);
	REGISTER_HELPER(zend_jit_hash_index_lookup_w);
	REGISTER_HELPER(zend_jit_hash_lookup_rw);
	REGISTER_HELPER(zend_jit_hash_lookup_w);
	REGISTER_HELPER(zend_jit_symtable_lookup_rw);
	REGISTER_HELPER(zend_jit_symtable_lookup_w);
	REGISTER_HELPER(zend_jit_undefined_op_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_is_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_isset_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_str_offset_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_str_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_str_is_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_is_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_rw_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_w_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_rw_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_w_helper);
//	REGISTER_HELPER(zend_jit_fetch_dim_obj_unset_helper);
	REGISTER_HELPER(zend_jit_assign_dim_helper);
	REGISTER_HELPER(zend_jit_assign_dim_op_helper);
	REGISTER_HELPER(zend_jit_fast_assign_concat_helper);
	REGISTER_HELPER(zend_jit_fast_concat_helper);
	REGISTER_HELPER(zend_jit_isset_dim_helper);
	REGISTER_HELPER(zend_jit_free_call_frame);
	REGISTER_HELPER(zend_jit_fetch_global_helper);
	REGISTER_HELPER(zend_jit_verify_arg_slow);
	REGISTER_HELPER(zend_jit_verify_return_slow);
	REGISTER_HELPER(zend_jit_fetch_obj_r_slow);
	REGISTER_HELPER(zend_jit_fetch_obj_r_dynamic);
	REGISTER_HELPER(zend_jit_fetch_obj_is_slow);
	REGISTER_HELPER(zend_jit_fetch_obj_is_dynamic);
	REGISTER_HELPER(zend_jit_fetch_obj_w_slow);
	REGISTER_HELPER(zend_jit_check_array_promotion);
	REGISTER_HELPER(zend_jit_create_typed_ref);
	REGISTER_HELPER(zend_jit_extract_helper);
	REGISTER_HELPER(zend_jit_vm_stack_free_args_helper);
	REGISTER_HELPER(zend_jit_copy_extra_args_helper);
	REGISTER_HELPER(zend_jit_deprecated_helper);
	REGISTER_HELPER(zend_jit_assign_const_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_tmp_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_var_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_cv_to_typed_ref);
	REGISTER_HELPER(zend_jit_pre_inc_typed_ref);
	REGISTER_HELPER(zend_jit_pre_dec_typed_ref);
	REGISTER_HELPER(zend_jit_post_inc_typed_ref);
	REGISTER_HELPER(zend_jit_post_dec_typed_ref);
	REGISTER_HELPER(zend_jit_assign_op_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_op_to_typed_ref_tmp);
	REGISTER_HELPER(zend_jit_only_vars_by_reference);
	REGISTER_HELPER(zend_jit_invalid_array_access);
	REGISTER_HELPER(zend_jit_invalid_property_read);
	REGISTER_HELPER(zend_jit_invalid_property_write);
	REGISTER_HELPER(zend_jit_invalid_property_incdec);
	REGISTER_HELPER(zend_jit_invalid_property_assign);
	REGISTER_HELPER(zend_jit_invalid_property_assign_op);
	REGISTER_HELPER(zend_jit_prepare_assign_dim_ref);
	REGISTER_HELPER(zend_jit_pre_inc);
	REGISTER_HELPER(zend_jit_pre_dec);
	REGISTER_HELPER(zend_runtime_jit);
	REGISTER_HELPER(zend_jit_hot_func);
	REGISTER_HELPER(zend_jit_check_constant);
	REGISTER_HELPER(zend_jit_get_constant);
	REGISTER_HELPER(zend_jit_array_free);
	REGISTER_HELPER(zend_jit_zval_array_dup);
	REGISTER_HELPER(zend_jit_add_arrays_helper);
	REGISTER_HELPER(zend_jit_assign_obj_helper);
	REGISTER_HELPER(zend_jit_assign_obj_op_helper);
	REGISTER_HELPER(zend_jit_assign_to_typed_prop);
	REGISTER_HELPER(zend_jit_assign_op_to_typed_prop);
	REGISTER_HELPER(zend_jit_inc_typed_prop);
	REGISTER_HELPER(zend_jit_dec_typed_prop);
	REGISTER_HELPER(zend_jit_pre_inc_typed_prop);
	REGISTER_HELPER(zend_jit_pre_dec_typed_prop);
	REGISTER_HELPER(zend_jit_post_inc_typed_prop);
	REGISTER_HELPER(zend_jit_post_dec_typed_prop);
	REGISTER_HELPER(zend_jit_pre_inc_obj_helper);
	REGISTER_HELPER(zend_jit_pre_dec_obj_helper);
	REGISTER_HELPER(zend_jit_post_inc_obj_helper);
	REGISTER_HELPER(zend_jit_post_dec_obj_helper);
#if (PHP_VERSION_ID <= 80100) && (SIZEOF_SIZE_T == 4)
	REGISTER_HELPER(zval_jit_update_constant_ex);
#endif
	REGISTER_HELPER(zend_jit_free_trampoline_helper);
	REGISTER_HELPER(zend_jit_exception_in_interrupt_handler_helper);
#undef  REGISTER_HELPER

#ifndef _WIN32
	zend_elf_load_symbols();
#endif

	if (zend_vm_kind() == ZEND_VM_KIND_HYBRID) {
		zend_op opline;

		memset(&opline, 0, sizeof(opline));

		opline.opcode = ZEND_DO_UCALL;
		opline.result_type = IS_UNUSED;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_DO_UCALL_SPEC_RETVAL_UNUSED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_UCALL;
		opline.result_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_DO_UCALL_SPEC_RETVAL_USED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL_BY_NAME;
		opline.result_type = IS_UNUSED;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_DO_FCALL_BY_NAME_SPEC_RETVAL_UNUSED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL_BY_NAME;
		opline.result_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_DO_FCALL_BY_NAME_SPEC_RETVAL_USED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL;
		opline.result_type = IS_UNUSED;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_DO_FCALL_SPEC_RETVAL_UNUSED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL;
		opline.result_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_DO_FCALL_SPEC_RETVAL_USED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_CONST;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_RETURN_SPEC_CONST_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_TMP_VAR;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_RETURN_SPEC_TMP_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_RETURN_SPEC_VAR_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_CV;
		zend_vm_set_opcode_handler(&opline);
		zend_jit_disasm_add_symbol("ZEND_RETURN_SPEC_CV_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		zend_jit_disasm_add_symbol("ZEND_HYBRID_HALT_LABEL", (uint64_t)(uintptr_t)zend_jit_halt_op->handler, sizeof(void*));
	}

	return 1;
}

static void zend_jit_disasm_shutdown(void)
{
	if (JIT_G(symbols)) {
		zend_jit_disasm_destroy_symbols(JIT_G(symbols));
		JIT_G(symbols) = NULL;
	}
}
