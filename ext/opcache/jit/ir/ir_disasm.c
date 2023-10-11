/*
 * IR - Lightweight JIT Compilation Framework
 * (Disassembler based on libcapstone)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#ifndef _WIN32
# include <dlfcn.h>
# include <unistd.h>
# include <fcntl.h>
#endif

#include "ir.h"
#include "ir_private.h"

#ifndef _WIN32
# include "ir_elf.h"
#endif

#include <capstone/capstone.h>
#define HAVE_CAPSTONE_ITER

typedef struct _ir_sym_node {
	uint64_t             addr;
	uint64_t             end;
	struct _ir_sym_node *parent;
	struct _ir_sym_node *child[2];
	unsigned char        info;
	char                 name[1];
} ir_sym_node;

static ir_sym_node *_symbols = NULL;

static void ir_syms_rotateleft(ir_sym_node *p)
{
	ir_sym_node *r = p->child[1];
	p->child[1] = r->child[0];
	if (r->child[0]) {
		r->child[0]->parent = p;
	}
	r->parent = p->parent;
	if (p->parent == NULL) {
		_symbols = r;
	} else if (p->parent->child[0] == p) {
		p->parent->child[0] = r;
	} else {
		p->parent->child[1] = r;
	}
	r->child[0] = p;
	p->parent = r;
}

static void ir_syms_rotateright(ir_sym_node *p)
{
	ir_sym_node *l = p->child[0];
	p->child[0] = l->child[1];
	if (l->child[1]) {
		l->child[1]->parent = p;
	}
	l->parent = p->parent;
	if (p->parent == NULL) {
		_symbols = l;
	} else if (p->parent->child[1] == p) {
		p->parent->child[1] = l;
	} else {
		p->parent->child[0] = l;
	}
	l->child[1] = p;
	p->parent = l;
}

void ir_disasm_add_symbol(const char *name,
                          uint64_t    addr,
                          uint64_t    size)
{
	ir_sym_node *sym;
	size_t len = strlen(name);

	sym = ir_mem_pmalloc(sizeof(ir_sym_node) + len + 1);
	if (!sym) {
		return;
	}
	sym->addr = addr;
	sym->end  = (addr + size - 1);
	memcpy((char*)&sym->name, name, len + 1);
	sym->parent = sym->child[0] = sym->child[1] = NULL;
	sym->info = 1;
	if (_symbols) {
		ir_sym_node *node = _symbols;

		/* insert it into rbtree */
		do {
			if (sym->addr > node->addr) {
				IR_ASSERT(sym->addr > (node->end));
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
				IR_ASSERT(sym->addr == node->addr);
				if (strcmp(name, node->name) == 0 && sym->end < node->end) {
					/* reduce size of the existing symbol */
					node->end = sym->end;
				}
				ir_mem_pfree(sym);
				return;
			}
		} while (1);

		/* fix rbtree after inserting */
		while (sym && sym != _symbols && sym->parent->info == 1) {
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
						ir_syms_rotateleft(sym);
					}
					sym->parent->info = 0;
					sym->parent->parent->info = 1;
					ir_syms_rotateright(sym->parent->parent);
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
						ir_syms_rotateright(sym);
					}
					sym->parent->info = 0;
					sym->parent->parent->info = 1;
					ir_syms_rotateleft(sym->parent->parent);
				}
			}
		}
	} else {
		_symbols = sym;
	}
	_symbols->info = 0;
}

static void ir_disasm_destroy_symbols(ir_sym_node *n)
{
	if (n) {
		if (n->child[0]) {
			ir_disasm_destroy_symbols(n->child[0]);
		}
		if (n->child[1]) {
			ir_disasm_destroy_symbols(n->child[1]);
		}
		ir_mem_pfree(n);
	}
}

const char* ir_disasm_find_symbol(uint64_t addr, int64_t *offset)
{
	ir_sym_node *node = _symbols;
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

static uint64_t ir_disasm_branch_target(csh cs, const cs_insn *insn)
{
	unsigned int i;

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
	if (cs_insn_group(cs, insn, X86_GRP_JUMP)) {
		for (i = 0; i < insn->detail->x86.op_count; i++) {
			if (insn->detail->x86.operands[i].type == X86_OP_IMM) {
				return insn->detail->x86.operands[i].imm;
			}
		}
	}
#elif defined(IR_TARGET_AARCH64)
	if (cs_insn_group(cs, insn, ARM64_GRP_JUMP)
	 || insn->id == ARM64_INS_BL
	 || insn->id == ARM64_INS_ADR) {
		for (i = 0; i < insn->detail->arm64.op_count; i++) {
			if (insn->detail->arm64.operands[i].type == ARM64_OP_IMM)
				return insn->detail->arm64.operands[i].imm;
		}
	}
#endif

	return 0;
}

static uint64_t ir_disasm_rodata_reference(csh cs, const cs_insn *insn)
{
#if defined(IR_TARGET_X86)
	unsigned int i;

	for (i = 0; i < insn->detail->x86.op_count; i++) {
		if (insn->detail->x86.operands[i].type == X86_OP_MEM
		 && insn->detail->x86.operands[i].mem.base == X86_REG_INVALID
		 && insn->detail->x86.operands[i].mem.segment == X86_REG_INVALID
		 && insn->detail->x86.operands[i].mem.index == X86_REG_INVALID
		 && insn->detail->x86.operands[i].mem.scale == 1) {
			return (uint32_t)insn->detail->x86.operands[i].mem.disp;
		}
	}
	if (cs_insn_group(cs, insn, X86_GRP_JUMP)) {
		for (i = 0; i < insn->detail->x86.op_count; i++) {
			if (insn->detail->x86.operands[i].type == X86_OP_MEM
			 && insn->detail->x86.operands[i].mem.disp) {
				return (uint32_t)insn->detail->x86.operands[i].mem.disp;
			}
		}
	}
	if (insn->id == X86_INS_MOV
	 && insn->detail->x86.op_count == 2
	 && insn->detail->x86.operands[0].type == X86_OP_IMM
	 && insn->detail->x86.operands[0].size == sizeof(void*)) {
		return (uint32_t)insn->detail->x86.operands[0].imm;
	}
#elif defined(IR_TARGET_X64)
	unsigned int i;

	for (i = 0; i < insn->detail->x86.op_count; i++) {
		if (insn->detail->x86.operands[i].type == X86_OP_MEM
		 && insn->detail->x86.operands[i].mem.base == X86_REG_RIP
		 && insn->detail->x86.operands[i].mem.segment == X86_REG_INVALID
		  // TODO: support for index and scale
		 && insn->detail->x86.operands[i].mem.index == X86_REG_INVALID
		 && insn->detail->x86.operands[i].mem.scale == 1) {
			return insn->detail->x86.operands[i].mem.disp + insn->address + insn->size;
		}
	}
#elif defined(IR_TARGET_AARCH64)
	unsigned int i;

	if (insn->id == ARM64_INS_ADR
	 || insn->id == ARM64_INS_LDRB
	 || insn->id == ARM64_INS_LDR
	 || insn->id == ARM64_INS_LDRH
	 || insn->id == ARM64_INS_LDRSB
	 || insn->id == ARM64_INS_LDRSH
	 || insn->id == ARM64_INS_LDRSW
	 || insn->id == ARM64_INS_STRB
	 || insn->id == ARM64_INS_STR
	 || insn->id == ARM64_INS_STRH) {
		for (i = 0; i < insn->detail->arm64.op_count; i++) {
			if (insn->detail->arm64.operands[i].type == ARM64_OP_IMM)
				return insn->detail->arm64.operands[i].imm;
		}
	}
	return 0;
#endif

	return 0;
}

static const char* ir_disasm_resolver(uint64_t   addr,
                                      int64_t   *offset)
{
#ifndef _WIN32
	const char *name;
	void *a = (void*)(uintptr_t)(addr);
	Dl_info info;

	name = ir_disasm_find_symbol(addr, offset);
	if (name) {
		return name;
	}

	if (dladdr(a, &info)
	 && info.dli_sname != NULL
	 && info.dli_saddr == a) {
		*offset = 0;
		return info.dli_sname;
	}
#else
	const char *name;
	name = ir_disasm_find_symbol(addr, offset);
	if (name) {
		return name;
	}
#endif

	return NULL;
}

int ir_disasm(const char    *name,
              const void    *start,
              size_t         size,
              bool           asm_addr,
              ir_ctx        *ctx,
              FILE          *f)
{
	size_t orig_size = size;
	const void *orig_end = (void *)((char *)start + size);
	const void *end;
	ir_hashtab labels;
	int32_t l, n;
	uint64_t addr;
	csh cs;
	cs_insn *insn;
# ifdef HAVE_CAPSTONE_ITER
	const uint8_t *cs_code;
	size_t cs_size;
	uint64_t cs_addr;
# else
	size_t count, i;
# endif
	const char *sym;
	int64_t offset = 0;
	char *p, *q, *r;
	uint32_t rodata_offset = 0;
	uint32_t jmp_table_offset = 0;
	ir_hashtab_bucket *b;
	int32_t entry;
	cs_err ret;

# if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
#  ifdef IR_TARGET_X64
	ret = cs_open(CS_ARCH_X86, CS_MODE_64, &cs);
	if (ret != CS_ERR_OK) {
		fprintf(stderr, "cs_open(CS_ARCH_X86, CS_MODE_64, ...) failed; [%d] %s\n", ret, cs_strerror(ret));
		return 0;
	}
#  else
	ret = cs_open(CS_ARCH_X86, CS_MODE_32, &cs);
	if (ret != CS_ERR_OK) {
		fprintf(stderr, "cs_open(CS_ARCH_X86, CS_MODE_32, ...) failed; [%d] %s\n", ret, cs_strerror(ret));
		return 0;
	}
#  endif
	cs_option(cs, CS_OPT_DETAIL, CS_OPT_ON);
#  if DISASM_INTEL_SYNTAX
	cs_option(cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);
#  else
	cs_option(cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
#  endif
# elif defined(IR_TARGET_AARCH64)
	ret = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &cs);
	if (ret != CS_ERR_OK) {
		fprintf(stderr, "cs_open(CS_ARCH_ARM64, CS_MODE_ARM, ...) failed; [%d] %s\n", ret, cs_strerror(ret));
		return 0;
	}
	cs_option(cs, CS_OPT_DETAIL, CS_OPT_ON);
	cs_option(cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
# endif

	if (name) {
		fprintf(f, "%s:\n", name);
	}

	ir_hashtab_init(&labels, 32);

	if (ctx) {
		if (ctx->entries_count) {
			int i = ctx->entries_count;
			do {
				ir_insn *insn = &ctx->ir_base[ctx->entries[--i]];
				ir_hashtab_add(&labels, insn->op3, insn->op2);
			} while (i != 0);
		}

		rodata_offset = ctx->rodata_offset;
		if (rodata_offset) {
			if (size > rodata_offset) {
				size = rodata_offset;
			}
		}
		jmp_table_offset = ctx->jmp_table_offset;
		if (jmp_table_offset) {
			uint32_t n;
			uintptr_t *p;

			IR_ASSERT(orig_size - jmp_table_offset <= 0xffffffff);
			n = (uint32_t)(orig_size - jmp_table_offset);
			if (size > jmp_table_offset) {
				size = jmp_table_offset;
			}
			while (n > 0 && IR_ALIGNED_SIZE(n, sizeof(void*)) != n) {
				jmp_table_offset++;
				n--;
			}
			IR_ASSERT(n > 0 && n % sizeof(void*) == 0 && jmp_table_offset % sizeof(void*) == 0);
			p = (uintptr_t*)((char*)start + jmp_table_offset);
			while (n > 0) {
				if (*p) {
					if ((uintptr_t)*p >= (uintptr_t)start && (uintptr_t)*p < (uintptr_t)orig_end) {
						ir_hashtab_add(&labels, (uint32_t)((uintptr_t)*p - (uintptr_t)start), -1);
					}
				}
				p++;
				n -= sizeof(void*);
			}
		}
	}
	end = (void *)((char *)start + size);

# ifdef HAVE_CAPSTONE_ITER
	cs_code = start;
	cs_size = (uint8_t*)end - (uint8_t*)start;
	cs_addr = (uint64_t)(uintptr_t)cs_code;
	insn = cs_malloc(cs);
	while (cs_disasm_iter(cs, &cs_code, &cs_size, &cs_addr, insn)) {
		if ((addr = ir_disasm_branch_target(cs, insn))
# else
	count = cs_disasm(cs, start, (uint8_t*)end - (uint8_t*)start, (uintptr_t)start, 0, &insn);
	for (i = 0; i < count; i++) {
		if ((addr = ir_disasm_branch_target(cs, &(insn[i])))
# endif
		 && (addr >= (uint64_t)(uintptr_t)start && addr < (uint64_t)(uintptr_t)end)) {
			ir_hashtab_add(&labels, (uint32_t)((uintptr_t)addr - (uintptr_t)start), -1);
# ifdef HAVE_CAPSTONE_ITER
		} else if ((addr = ir_disasm_rodata_reference(cs, insn))) {
# else
		} else if ((addr = ir_disasm_rodata_reference(cs, &(insn[i])))) {
# endif
			if (addr >= (uint64_t)(uintptr_t)end && addr < (uint64_t)(uintptr_t)orig_end) {
				ir_hashtab_add(&labels, (uint32_t)((uintptr_t)addr - (uintptr_t)start), -1);
			}
		}
	}

	ir_hashtab_key_sort(&labels);

	/* renumber labels */
	l = 0;
	n = labels.count;
	b = labels.data;
	while (n > 0) {
		if (b->val < 0) {
			b->val = --l;
		}	
		b++;
		n--;
	}

# ifdef HAVE_CAPSTONE_ITER
	cs_code = start;
	cs_size = (uint8_t*)end - (uint8_t*)start;
	cs_addr = (uint64_t)(uintptr_t)cs_code;
	while (cs_disasm_iter(cs, &cs_code, &cs_size, &cs_addr, insn)) {
		entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)insn->address - (uintptr_t)start));
# else
	for (i = 0; i < count; i++) {
		entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)insn->address - (uintptr_t)start));
# endif
		if (entry != (ir_ref)IR_INVALID_VAL) {
			if (entry >= 0) {
				fprintf(f, ".ENTRY_%d:\n", entry);
			} else {
				fprintf(f, ".L%d:\n", -entry);
			}
		}

# ifdef HAVE_CAPSTONE_ITER
		if (asm_addr) {
			fprintf(f, "    %" PRIx64 ":", insn->address);
		}
		p = insn->op_str;
#if defined(IR_TARGET_X64) && (CS_API_MAJOR < 5)
		/* Fix capstone MOVD/MOVQ disassemble mismatch */
		if (insn->id == X86_INS_MOVQ && strcmp(insn->mnemonic, "movd") == 0) {
			insn->mnemonic[3] = 'q';
		}
#endif
		if (strlen(p) == 0) {
			fprintf(f, "\t%s\n", insn->mnemonic);
			continue;
		} else  {
			fprintf(f, "\t%s ", insn->mnemonic);
		}
# else
		if (asm_addr) {
			fprintf(f, "    %" PRIx64 ":", insn[i].address);
		}
		p = insn[i].op_str;
		if (strlen(p) == 0) {
			fprintf(f, "\t%s\n", insn[i].mnemonic);
			continue;
		} else {
			fprintf(f, "\t%s ", insn[i].mnemonic);
		}
# endif
		/* Try to replace the target addresses with a symbols */
#if defined(IR_TARGET_X64)
# ifdef HAVE_CAPSTONE_ITER
		if ((addr = ir_disasm_rodata_reference(cs, insn))) {
# else
		if ((addr = ir_disasm_rodata_reference(cs, &(insn[i])))) {
# endif
			if (addr >= (uint64_t)(uintptr_t)end && addr < (uint64_t)(uintptr_t)orig_end) {
				entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)addr - (uintptr_t)start));
				if (entry != (ir_ref)IR_INVALID_VAL) {
					r = q = strstr(p, "(%rip)");
					if (r && r > p) {
						r--;
						while (r > p && ((*r >= '0' && *r <= '9') || (*r >= 'a' && *r <= 'f') || (*r >= 'A' && *r <= 'F'))) {
							r--;
						}
						if (r > p && *r == 'x' && *(r - 1) == '0') {
							r -= 2;
						}
						if (r > p) {
							fwrite(p, 1, r - p, f);
						}
						if (entry >= 0) {
							fprintf(f, ".ENTRY_%d%s\n", entry, q);
						} else {
							fprintf(f, ".L%d%s\n", -entry, q);
						}
						continue;
					}
				}
			}
		}
#endif
#if defined(IR_TARGET_AARCH64)
		while ((q = strstr(p, "#0x")) != NULL) {
				r = q + 3;
#else
		while ((q = strstr(p, "0x")) != NULL) {
				r = q + 2;
#endif
			addr = 0;
			while (1) {
				if (*r >= '0' && *r <= '9') {
					addr = addr * 16 + (*r - '0');
				} else if (*r >= 'A' && *r <= 'F') {
					addr = addr * 16 + (*r - 'A' + 10);
				} else if (*r >= 'a' && *r <= 'f') {
					addr = addr * 16 + (*r - 'a' + 10);
				} else {
					break;
				}
				r++;
			}
			if (p != q && *(q-1) == '-') {
				q--;
				addr = (uint32_t)(-(int64_t)addr);
			}
			if (addr >= (uint64_t)(uintptr_t)start && addr < (uint64_t)(uintptr_t)orig_end) {
				entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)addr - (uintptr_t)start));
				if (entry != (ir_ref)IR_INVALID_VAL) {
					fwrite(p, 1, q - p, f);
					if (entry >= 0) {
						fprintf(f, ".ENTRY_%d", entry);
					} else {
						fprintf(f, ".L%d", -entry);
					}
				} else if (r > p) {
					fwrite(p, 1, r - p, f);
				}
			} else if ((sym = ir_disasm_resolver(addr, &offset))) {
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
				if (offset && p != q && *(q-1) == '$') {
					if (r > p) {
						fwrite(p, 1, r - p, f);
					}
					p = r;
					continue;
				}
#endif
				if (q > p) {
					fwrite(p, 1, q - p, f);
				}
				fputs(sym, f);
				if (offset != 0) {
					if (offset > 0) {
						fprintf(f, "+0x%" PRIx64, offset);
					} else {
						fprintf(f, "-0x%" PRIx64, -offset);
					}
				}
			} else if (r > p) {
				fwrite(p, 1, r - p, f);
			}
			p = r;
		}
		fprintf(f, "%s\n", p);
	}
# ifdef HAVE_CAPSTONE_ITER
	cs_free(insn, 1);
# else
	cs_free(insn, count);
# endif

	if (rodata_offset || jmp_table_offset) {
		fprintf(f, ".rodata\n");
	}
	if (rodata_offset) {
		const unsigned char *p = (unsigned char*)start + rodata_offset;
		uint32_t n = jmp_table_offset ?
			(uint32_t)(jmp_table_offset - rodata_offset) :
			(uint32_t)(orig_size - rodata_offset);
		uint32_t j;

		while (n > 0) {
			entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)p - (uintptr_t)start));
			if (entry != (ir_ref)IR_INVALID_VAL) {
				if (entry >= 0) {
					fprintf(f, ".ENTRY_%d:\n", entry);
				} else {
					fprintf(f, ".L%d:\n", -entry);
				}
			}
			fprintf(f, "\t.db 0x%02x", (int)*p);
			p++;
			n--;
			j = 15;
			while (n > 0 && j > 0) {
				entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)p - (uintptr_t)start));
				if (entry != (ir_ref)IR_INVALID_VAL) {
					break;
				}
				fprintf(f, ", 0x%02x", (int)*p);
				p++;
				n--;
				j--;
			}
			fprintf(f, "\n");
		}
	}
	if (jmp_table_offset) {
		uintptr_t *p = (uintptr_t*)(unsigned char*)start + jmp_table_offset;
		uint32_t n = (uint32_t)(orig_size - jmp_table_offset);

		fprintf(f, ".align %d\n", (int)sizeof(void*));

		p = (uintptr_t*)((char*)start + jmp_table_offset);
		while (n > 0) {
			entry = ir_hashtab_find(&labels, (uint32_t)((uintptr_t)p - (uintptr_t)start));
			if (entry != (ir_ref)IR_INVALID_VAL) {
				if (entry >= 0) {
					fprintf(f, ".ENTRY_%d:\n", entry);
				} else {
					fprintf(f, ".L%d:\n", -entry);
				}
			}
			if (*p) {
				if ((uintptr_t)*p >= (uintptr_t)start && (uintptr_t)*p < (uintptr_t)orig_end) {
					entry = ir_hashtab_find(&labels, (uint32_t)(*p - (uintptr_t)start));
					IR_ASSERT(entry != (ir_ref)IR_INVALID_VAL);
					if (entry >= 0) {
						if (sizeof(void*) == 8) {
							fprintf(f, "\t.qword .ENTRY_%d\n", entry);
						} else {
							fprintf(f, "\t.dword .ENTRY_%d\n", entry);
						}
					} else {
						if (sizeof(void*) == 8) {
							fprintf(f, "\t.qword .L%d\n", -entry);
						} else {
							fprintf(f, "\t.dword .L%d\n", -entry);
						}
					}
				} else {
					int64_t offset;
					const char *name = ir_disasm_find_symbol(*p, &offset);

					if (name && offset == 0) {
						if (sizeof(void*) == 8) {
							fprintf(f, "\t.qword %s\n", name);
						} else {
							fprintf(f, "\t.dword %s\n", name);
						}
					} else {
						if (sizeof(void*) == 8) {
							fprintf(f, "\t.qword 0x%0llx\n", (long long)*p);
						} else {
							fprintf(f, "\t.dword 0x%0x\n", (int)*p);
						}
					}
				}
			} else {
				if (sizeof(void*) == 8) {
					fprintf(f, "\t.qword 0\n");
				} else {
					fprintf(f, "\t.dword 0\n");
				}
			}
			p++;
			n -= sizeof(void*);
		}
	}

	fprintf(f, "\n");

	ir_hashtab_free(&labels);

	cs_close(&cs);

	return 1;
}

#ifndef _WIN32
static void* ir_elf_read_sect(int fd, ir_elf_sectheader *sect)
{
	void *s = ir_mem_malloc(sect->size);

	if (lseek(fd, sect->ofs, SEEK_SET) < 0) {
		ir_mem_free(s);
		return NULL;
	}
	if (read(fd, s, sect->size) != (ssize_t)sect->size) {
		ir_mem_free(s);
		return NULL;
	}

	return s;
}

static void ir_elf_load_symbols(void)
{
	ir_elf_header hdr;
	ir_elf_sectheader sect;
	int i;
#if defined(__linux__)
	int fd = open("/proc/self/exe", O_RDONLY);
#elif defined(__NetBSD__)
	int fd = open("/proc/curproc/exe", O_RDONLY);
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	char path[PATH_MAX];
	size_t pathlen = sizeof(path);
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
	if (sysctl(mib, 4, path, &pathlen, NULL, 0) == -1) {
		return;
	}
	int fd = open(path, O_RDONLY);
#elif defined(__sun)
	int fd = open("/proc/self/path/a.out", O_RDONLY);
#elif defined(__HAIKU__)
	char path[PATH_MAX];
	if (find_path(B_APP_IMAGE_SYMBOL, B_FIND_PATH_IMAGE_PATH,
		NULL, path, sizeof(path)) != B_OK) {
		return;
	}

	int fd = open(path, O_RDONLY);
#else
	// To complete eventually for other ELF platforms.
	// Otherwise APPLE is Mach-O
	int fd = -1;
#endif

	if (fd >= 0) {
		if (read(fd, &hdr, sizeof(hdr)) == sizeof(hdr)
		 && hdr.emagic[0] == '\177'
		 && hdr.emagic[1] == 'E'
		 && hdr.emagic[2] == 'L'
		 && hdr.emagic[3] == 'F'
		 && lseek(fd, hdr.shofs, SEEK_SET) >= 0) {
			for (i = 0; i < hdr.shnum; i++) {
				if (read(fd, &sect, sizeof(sect)) == sizeof(sect)
				 && sect.type == ELFSECT_TYPE_SYMTAB) {
					uint32_t n, count = sect.size / sizeof(ir_elf_symbol);
					ir_elf_symbol *syms = ir_elf_read_sect(fd, &sect);
					char *str_tbl;

					if (syms) {
						if (lseek(fd, hdr.shofs + sect.link * sizeof(sect), SEEK_SET) >= 0
						 && read(fd, &sect, sizeof(sect)) == sizeof(sect)
						 && (str_tbl = (char*)ir_elf_read_sect(fd, &sect)) != NULL) {
							for (n = 0; n < count; n++) {
								if (syms[n].name
								 && (ELFSYM_TYPE(syms[n].info) == ELFSYM_TYPE_FUNC
								  /*|| ELFSYM_TYPE(syms[n].info) == ELFSYM_TYPE_DATA*/)
								 && (ELFSYM_BIND(syms[n].info) == ELFSYM_BIND_LOCAL
								  /*|| ELFSYM_BIND(syms[n].info) == ELFSYM_BIND_GLOBAL*/)) {
									ir_disasm_add_symbol(str_tbl + syms[n].name, syms[n].value, syms[n].size);
								}
							}
							ir_mem_free(str_tbl);
						}
						ir_mem_free(syms);
					}
					if (lseek(fd, hdr.shofs + (i + 1) * sizeof(sect), SEEK_SET) < 0) {
						break;
					}
				}
			}
		}
		close(fd);
	}
}
#endif

int ir_disasm_init(void)
{
#ifndef _WIN32
	ir_elf_load_symbols();
#endif
	return 1;
}

void ir_disasm_free(void)
{
	if (_symbols) {
		ir_disasm_destroy_symbols(_symbols);
		_symbols = NULL;
	}
}
