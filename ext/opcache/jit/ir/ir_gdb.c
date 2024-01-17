/*
 * IR - Lightweight JIT Compilation Framework
 * (GDB interface)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * Based on Mike Pall's implementation of GDB interface for LuaJIT.
 */

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __FreeBSD__
# include <sys/types.h>
# include <sys/sysctl.h>
# include <sys/user.h>
#endif

#include "ir.h"
#include "ir_private.h"
#include "ir_elf.h"

/* DWARF definitions. */
#define DW_CIE_VERSION  1

/* CFA (Canonical frame address) */
enum {
	DW_CFA_nop = 0x0,
	DW_CFA_offset_extended = 0x5,
	DW_CFA_def_cfa = 0xc,
	DW_CFA_def_cfa_offset = 0xe,
	DW_CFA_offset_extended_sf = 0x11,
	DW_CFA_advance_loc = 0x40,
	DW_CFA_offset = 0x80
};

enum {
	DW_EH_PE_udata4 = 0x03,
	DW_EH_PE_textrel = 0x20
};

enum {
	DW_TAG_compile_unit = 0x11
};

enum {
	DW_children_no = 0,
	DW_children_yes = 1
};

enum {
	DW_AT_name = 0x03,
	DW_AT_stmt_list = 0x10,
	DW_AT_low_pc = 0x11,
	DW_AT_high_pc = 0x12
};

enum {
	DW_FORM_addr = 0x01,
	DW_FORM_data4 = 0x06,
	DW_FORM_string = 0x08
};

enum {
	DW_LNS_extended_op = 0,
	DW_LNS_copy = 1,
	DW_LNS_advance_pc = 2,
	DW_LNS_advance_line = 3
};

enum {
	DW_LNE_end_sequence = 1,
	DW_LNE_set_address = 2
};

enum {
#if defined(IR_TARGET_X86)
	DW_REG_AX, DW_REG_CX, DW_REG_DX, DW_REG_BX,
	DW_REG_SP, DW_REG_BP, DW_REG_SI, DW_REG_DI,
	DW_REG_RA,
#elif defined(IR_TARGET_X64)
	/* Yes, the order is strange, but correct. */
	DW_REG_AX, DW_REG_DX, DW_REG_CX, DW_REG_BX,
	DW_REG_SI, DW_REG_DI, DW_REG_BP, DW_REG_SP,
	DW_REG_8, DW_REG_9, DW_REG_10, DW_REG_11,
	DW_REG_12, DW_REG_13, DW_REG_14, DW_REG_15,
	DW_REG_RA,
#elif defined(IR_TARGET_AARCH64)
	DW_REG_SP = 31,
	DW_REG_RA = 30,
	DW_REG_X29 = 29,
#else
#error "Unsupported target architecture"
#endif
};

enum {
	GDBJIT_SECT_NULL,
	GDBJIT_SECT_text,
	GDBJIT_SECT_eh_frame,
	GDBJIT_SECT_shstrtab,
	GDBJIT_SECT_strtab,
	GDBJIT_SECT_symtab,
	GDBJIT_SECT_debug_info,
	GDBJIT_SECT_debug_abbrev,
	GDBJIT_SECT_debug_line,
	GDBJIT_SECT__MAX
};

enum {
	GDBJIT_SYM_UNDEF,
	GDBJIT_SYM_FILE,
	GDBJIT_SYM_FUNC,
	GDBJIT_SYM__MAX
};

typedef struct _ir_gdbjit_obj {
	ir_elf_header     hdr;
	ir_elf_sectheader sect[GDBJIT_SECT__MAX];
	ir_elf_symbol     sym[GDBJIT_SYM__MAX];
	uint8_t           space[4096];
} ir_gdbjit_obj;

static const ir_elf_header ir_elfhdr_template = {
	.emagic      = { 0x7f, 'E', 'L', 'F' },
#ifdef ELF64
	.eclass      = 2,
#else
	.eclass      = 1,
#endif
#ifdef WORDS_BIGENDIAN
	.eendian     = 2,
#else
	.eendian     = 1,
#endif
	.eversion    = 1,
#if defined(Linux)
	.eosabi      = 0,  /* TODO: Nope, it's not 3. ??? */
#elif defined(__FreeBSD__)
	.eosabi      = 9,
#elif defined(__OpenBSD__)
	.eosabi      = 12,
#elif defined(__NetBSD__)
	.eosabi      = 2,
#elif defined(__DragonFly__)
	.eosabi      = 0,
#elif (defined(__sun__) && defined(__svr4__))
	.eosabi      = 6,
#else
	.eosabi      = 0,
#endif
	.eabiversion = 0,
	.epad        = { 0, 0, 0, 0, 0, 0, 0 },
	.type        = 1,
#if defined(IR_TARGET_X86)
	.machine     = 3,
#elif defined(IR_TARGET_X64)
	.machine     = 62,
#elif defined(IR_TARGET_AARCH64)
	.machine     = 183,
#else
# error "Unsupported target architecture"
#endif
	.version     = 1,
	.entry       = 0,
	.phofs       = 0,
	.shofs       = offsetof(ir_gdbjit_obj, sect),
	.flags       = 0,
	.ehsize      = sizeof(ir_elf_header),
	.phentsize   = 0,
	.phnum       = 0,
	.shentsize   = sizeof(ir_elf_sectheader),
	.shnum       = GDBJIT_SECT__MAX,
	.shstridx    = GDBJIT_SECT_shstrtab
};

/* Context for generating the ELF object for the GDB JIT API. */
typedef struct _ir_gdbjit_ctx {
	uint8_t *p;              /* Pointer to next address in obj.space. */
	uint8_t *startp;         /* Pointer to start address in obj.space. */
	uintptr_t mcaddr;        /* Machine code address. */
	uint32_t szmcode;        /* Size of machine code. */
	int32_t  lineno;         /* Starting line number. */
	const char *name;        /* JIT function name */
	const char *filename;    /* Starting file name. */
	size_t objsize;          /* Final size of ELF object. */
	ir_gdbjit_obj obj;       /* In-memory ELF object. */
} ir_gdbjit_ctx;

/* Add a zero-terminated string */
static uint32_t ir_gdbjit_strz(ir_gdbjit_ctx *ctx, const char *str)
{
	uint8_t *p = ctx->p;
	uint32_t ofs = (uint32_t)(p - ctx->startp);
	do {
		*p++ = (uint8_t)*str;
	} while (*str++);
	ctx->p = p;
	return ofs;
}

/* Add a ULEB128 value */
static void ir_gdbjit_uleb128(ir_gdbjit_ctx *ctx, uint32_t v)
{
	uint8_t *p = ctx->p;
	for (; v >= 0x80; v >>= 7)
		*p++ = (uint8_t)((v & 0x7f) | 0x80);
	*p++ = (uint8_t)v;
	ctx->p = p;
}

/* Add a SLEB128 value */
static void ir_gdbjit_sleb128(ir_gdbjit_ctx *ctx, int32_t v)
{
	uint8_t *p = ctx->p;
	for (; (uint32_t)(v+0x40) >= 0x80; v >>= 7)
		*p++ = (uint8_t)((v & 0x7f) | 0x80);
	*p++ = (uint8_t)(v & 0x7f);
	ctx->p = p;
}

static void ir_gdbjit_secthdr(ir_gdbjit_ctx *ctx)
{
	ir_elf_sectheader *sect;

	*ctx->p++ = '\0';

#define SECTDEF(id, tp, al)                       \
	sect = &ctx->obj.sect[GDBJIT_SECT_##id];      \
	sect->name = ir_gdbjit_strz(ctx, "." #id);  \
	sect->type = ELFSECT_TYPE_##tp;               \
	sect->align = (al)

	SECTDEF(text, NOBITS, 16);
	sect->flags = ELFSECT_FLAGS_ALLOC|ELFSECT_FLAGS_EXEC;
	sect->addr = ctx->mcaddr;
	sect->ofs = 0;
	sect->size = ctx->szmcode;

	SECTDEF(eh_frame, PROGBITS, sizeof(uintptr_t));
	sect->flags = ELFSECT_FLAGS_ALLOC;

	SECTDEF(shstrtab, STRTAB, 1);
	SECTDEF(strtab, STRTAB, 1);

	SECTDEF(symtab, SYMTAB, sizeof(uintptr_t));
	sect->ofs = offsetof(ir_gdbjit_obj, sym);
	sect->size = sizeof(ctx->obj.sym);
	sect->link = GDBJIT_SECT_strtab;
	sect->entsize = sizeof(ir_elf_symbol);
	sect->info = GDBJIT_SYM_FUNC;

	SECTDEF(debug_info, PROGBITS, 1);
	SECTDEF(debug_abbrev, PROGBITS, 1);
	SECTDEF(debug_line, PROGBITS, 1);

#undef SECTDEF
}

static void ir_gdbjit_symtab(ir_gdbjit_ctx *ctx)
{
	ir_elf_symbol *sym;

	*ctx->p++ = '\0';

	sym = &ctx->obj.sym[GDBJIT_SYM_FILE];
	sym->name = ir_gdbjit_strz(ctx, "JIT code");
	sym->sectidx = ELFSECT_IDX_ABS;
	sym->info = ELFSYM_INFO(ELFSYM_BIND_LOCAL, ELFSYM_TYPE_FILE);

	sym = &ctx->obj.sym[GDBJIT_SYM_FUNC];
	sym->name = ir_gdbjit_strz(ctx, ctx->name);
	sym->sectidx = GDBJIT_SECT_text;
	sym->value = 0;
	sym->size = ctx->szmcode;
	sym->info = ELFSYM_INFO(ELFSYM_BIND_GLOBAL, ELFSYM_TYPE_FUNC);
}

typedef IR_SET_ALIGNED(1, uint16_t unaligned_uint16_t);
typedef IR_SET_ALIGNED(1, uint32_t unaligned_uint32_t);
typedef IR_SET_ALIGNED(1, uintptr_t unaligned_uintptr_t);

#define SECTALIGN(p, a) \
	  ((p) = (uint8_t *)(((uintptr_t)(p) + ((a)-1)) & ~(uintptr_t)((a)-1)))

/* Shortcuts to generate DWARF structures. */
#define DB(x)       (*p++ = (x))
#define DI8(x)      (*(int8_t *)p = (x), p++)
#define DU16(x)     (*(unaligned_uint16_t *)p = (x), p += 2)
#define DU32(x)     (*(unaligned_uint32_t *)p = (x), p += 4)
#define DADDR(x)    (*(unaligned_uintptr_t *)p = (x), p += sizeof(uintptr_t))
#define DUV(x)      (ctx->p = p, ir_gdbjit_uleb128(ctx, (x)), p = ctx->p)
#define DSV(x)      (ctx->p = p, ir_gdbjit_sleb128(ctx, (x)), p = ctx->p)
#define DSTR(str)   (ctx->p = p, ir_gdbjit_strz(ctx, (str)), p = ctx->p)
#define DALIGNNOP(s)    while ((uintptr_t)p & ((s)-1)) *p++ = DW_CFA_nop
#define DSECT(name, stmt) \
	{ unaligned_uint32_t *szp_##name = (uint32_t *)p; p += 4; stmt \
		*szp_##name = (uint32_t)((p-(uint8_t *)szp_##name)-4); }

static void ir_gdbjit_ehframe(ir_gdbjit_ctx *ctx, uint32_t sp_offset, uint32_t sp_adjustment)
{
	uint8_t *p = ctx->p;
	uint8_t *framep = p;

	/* DWARF EH CIE (Common Information Entry) */
	DSECT(CIE,
		DU32(0);                                       /* CIE ID. */
		DB(DW_CIE_VERSION);                            /* Version */
		DSTR("zR");                                    /* Augmentation String. */
		DUV(1);                                        /* Code alignment factor. */
		DSV(-(int32_t)sizeof(uintptr_t));              /* Data alignment factor. */
		DB(DW_REG_RA);                                 /* Return address register. */
		DB(1); DB(DW_EH_PE_textrel|DW_EH_PE_udata4);   /* Augmentation data. */
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
		DB(DW_CFA_def_cfa); DUV(DW_REG_SP); DUV(sizeof(uintptr_t));
		DB(DW_CFA_offset|DW_REG_RA); DUV(1);
#elif defined(IR_TARGET_AARCH64)
		DB(DW_CFA_def_cfa); DUV(DW_REG_SP); DUV(0);
#endif
		DALIGNNOP(sizeof(uintptr_t));
	)

	/* DWARF EH FDE (Frame Description Entry). */
	DSECT(FDE,
		DU32((uint32_t)(p-framep)); /* Offset to CIE Pointer. */
		DU32(0);                    /* Machine code offset relative to .text. */
		DU32(ctx->szmcode);         /* Machine code length. */
		DB(0);                      /* Augmentation data. */
		DB(DW_CFA_def_cfa_offset); DUV(sp_offset);
#if defined(IR_TARGET_AARCH64)
		if (sp_offset) {
			if (sp_adjustment && sp_adjustment < sp_offset) {
				DB(DW_CFA_offset|DW_REG_X29); DUV(sp_adjustment / sizeof(uintptr_t));
				DB(DW_CFA_offset|DW_REG_RA); DUV((sp_adjustment / sizeof(uintptr_t)) - 1);
			} else {
				DB(DW_CFA_offset|DW_REG_X29); DUV(sp_offset / sizeof(uintptr_t));
				DB(DW_CFA_offset|DW_REG_RA); DUV((sp_offset / sizeof(uintptr_t)) - 1);
			}
		}
#endif
		if (sp_adjustment && sp_adjustment > sp_offset) {
			DB(DW_CFA_advance_loc|1); DB(DW_CFA_def_cfa_offset); DUV(sp_adjustment);
#if defined(IR_TARGET_AARCH64)
			if (!sp_offset) {
				DB(DW_CFA_offset|DW_REG_X29); DUV(sp_adjustment / sizeof(uintptr_t));
				DB(DW_CFA_offset|DW_REG_RA); DUV((sp_adjustment / sizeof(uintptr_t)) - 1);
			}
#endif
		}
		DALIGNNOP(sizeof(uintptr_t));
	)

	ctx->p = p;
}

static void ir_gdbjit_debuginfo(ir_gdbjit_ctx *ctx)
{
	uint8_t *p = ctx->p;

	DSECT(info,
		DU16(2);                 /* DWARF version. */
		DU32(0);                 /* Abbrev offset. */
		DB(sizeof(uintptr_t));   /* Pointer size. */

		DUV(1);                  /* Abbrev #1: DW_TAG_compile_unit. */
		DSTR(ctx->filename);    /* DW_AT_name. */
		DADDR(ctx->mcaddr);     /* DW_AT_low_pc. */
		DADDR(ctx->mcaddr + ctx->szmcode);  /* DW_AT_high_pc. */
		DU32(0);                /* DW_AT_stmt_list. */
	 );

	ctx->p = p;
}

static void ir_gdbjit_debugabbrev(ir_gdbjit_ctx *ctx)
{
	uint8_t *p = ctx->p;

	/* Abbrev #1: DW_TAG_compile_unit. */
	DUV(1);
	DUV(DW_TAG_compile_unit);
	DB(DW_children_no);
	DUV(DW_AT_name);
	DUV(DW_FORM_string);
	DUV(DW_AT_low_pc);
	DUV(DW_FORM_addr);
	DUV(DW_AT_high_pc);
	DUV(DW_FORM_addr);
	DUV(DW_AT_stmt_list);
	DUV(DW_FORM_data4);
	DB(0);
	DB(0);
	DB(0);

	ctx->p = p;
}

#define DLNE(op, s) (DB(DW_LNS_extended_op), DUV(1+(s)), DB((op)))

static void ir_gdbjit_debugline(ir_gdbjit_ctx *ctx)
{
	uint8_t *p = ctx->p;

	DSECT(line,
		DU16(2);            /* DWARF version. */
		DSECT(header,
			DB(1);            /* Minimum instruction length. */
			DB(1);            /* is_stmt. */
			DI8(0);           /* Line base for special opcodes. */
			DB(2);            /* Line range for special opcodes. */
			DB(3+1);          /* Opcode base at DW_LNS_advance_line+1. */
			DB(0); DB(1); DB(1);  /* Standard opcode lengths. */
			/* Directory table. */
			DB(0);
			/* File name table. */
			DSTR(ctx->filename); DUV(0); DUV(0); DUV(0);
			DB(0);
		);
		DLNE(DW_LNE_set_address, sizeof(uintptr_t));
		DADDR(ctx->mcaddr);
		if (ctx->lineno) (DB(DW_LNS_advance_line), DSV(ctx->lineno-1));
		DB(DW_LNS_copy);
		DB(DW_LNS_advance_pc); DUV(ctx->szmcode);
		DLNE(DW_LNE_end_sequence, 0);
	);

	ctx->p = p;
}


#undef DLNE

/* Undef shortcuts. */
#undef DB
#undef DI8
#undef DU16
#undef DU32
#undef DADDR
#undef DUV
#undef DSV
#undef DSTR
#undef DALIGNNOP
#undef DSECT

typedef void (*ir_gdbjit_initf) (ir_gdbjit_ctx *ctx);

static void ir_gdbjit_initsect(ir_gdbjit_ctx *ctx, int sect)
{
	ctx->startp = ctx->p;
	ctx->obj.sect[sect].ofs = (uintptr_t)((char *)ctx->p - (char *)&ctx->obj);
}

static void ir_gdbjit_initsect_done(ir_gdbjit_ctx *ctx, int sect)
{
	ctx->obj.sect[sect].size = (uintptr_t)(ctx->p - ctx->startp);
}

static void ir_gdbjit_buildobj(ir_gdbjit_ctx *ctx, uint32_t sp_offset, uint32_t sp_adjustment)
{
	ir_gdbjit_obj *obj = &ctx->obj;

	/* Fill in ELF header and clear structures. */
	memcpy(&obj->hdr, &ir_elfhdr_template, sizeof(ir_elf_header));
	memset(&obj->sect, 0, sizeof(ir_elf_sectheader) * GDBJIT_SECT__MAX);
	memset(&obj->sym, 0, sizeof(ir_elf_symbol) * GDBJIT_SYM__MAX);

	/* Initialize sections. */
	ctx->p = obj->space;
	ir_gdbjit_initsect(ctx, GDBJIT_SECT_shstrtab); ir_gdbjit_secthdr(ctx); ir_gdbjit_initsect_done(ctx, GDBJIT_SECT_shstrtab);
	ir_gdbjit_initsect(ctx, GDBJIT_SECT_strtab); ir_gdbjit_symtab(ctx); ir_gdbjit_initsect_done(ctx, GDBJIT_SECT_strtab);
	ir_gdbjit_initsect(ctx, GDBJIT_SECT_debug_info); ir_gdbjit_debuginfo(ctx); ir_gdbjit_initsect_done(ctx, GDBJIT_SECT_debug_info);
	ir_gdbjit_initsect(ctx, GDBJIT_SECT_debug_abbrev); ir_gdbjit_debugabbrev(ctx); ir_gdbjit_initsect_done(ctx, GDBJIT_SECT_debug_abbrev);
	ir_gdbjit_initsect(ctx, GDBJIT_SECT_debug_line); ir_gdbjit_debugline(ctx); ir_gdbjit_initsect_done(ctx, GDBJIT_SECT_debug_line);
	SECTALIGN(ctx->p, sizeof(uintptr_t));
	ir_gdbjit_initsect(ctx, GDBJIT_SECT_eh_frame); ir_gdbjit_ehframe(ctx, sp_offset, sp_adjustment); ir_gdbjit_initsect_done(ctx, GDBJIT_SECT_eh_frame);
	ctx->objsize = (size_t)((char *)ctx->p - (char *)obj);

	IR_ASSERT(ctx->objsize < sizeof(ir_gdbjit_obj));
}

enum {
	IR_GDBJIT_NOACTION,
	IR_GDBJIT_REGISTER,
	IR_GDBJIT_UNREGISTER
};

typedef struct _ir_gdbjit_code_entry {
	struct _ir_gdbjit_code_entry *next_entry;
	struct _ir_gdbjit_code_entry *prev_entry;
	const char                   *symfile_addr;
	uint64_t                      symfile_size;
} ir_gdbjit_code_entry;

typedef struct _ir_gdbjit_descriptor {
	uint32_t                      version;
	uint32_t                      action_flag;
	struct _ir_gdbjit_code_entry *relevant_entry;
	struct _ir_gdbjit_code_entry *first_entry;
} ir_gdbjit_descriptor;

ir_gdbjit_descriptor __jit_debug_descriptor = {
	1, IR_GDBJIT_NOACTION, NULL, NULL
};

#ifdef IR_EXTERNAL_GDB_ENTRY
void __jit_debug_register_code(void);
#else
IR_NEVER_INLINE void __jit_debug_register_code(void)
{
	__asm__ __volatile__("");
}
#endif

static bool ir_gdb_register_code(const void *object, size_t size)
{
	ir_gdbjit_code_entry *entry;

	entry = malloc(sizeof(ir_gdbjit_code_entry) + size);
	if (entry == NULL) {
		return 0;
	}

	entry->symfile_addr = ((char*)entry) + sizeof(ir_gdbjit_code_entry);
	entry->symfile_size = size;

	memcpy((char *)entry->symfile_addr, object, size);

	entry->prev_entry = NULL;
	entry->next_entry = __jit_debug_descriptor.first_entry;

	if (entry->next_entry) {
		entry->next_entry->prev_entry = entry;
	}
	__jit_debug_descriptor.first_entry = entry;

	/* Notify GDB */
	__jit_debug_descriptor.relevant_entry = entry;
	__jit_debug_descriptor.action_flag = IR_GDBJIT_REGISTER;
	__jit_debug_register_code();

	return 1;
}

void ir_gdb_unregister_all(void)
{
	ir_gdbjit_code_entry *entry;

	__jit_debug_descriptor.action_flag = IR_GDBJIT_UNREGISTER;
	while ((entry = __jit_debug_descriptor.first_entry)) {
		__jit_debug_descriptor.first_entry = entry->next_entry;
		if (entry->next_entry) {
			entry->next_entry->prev_entry = NULL;
		}
		/* Notify GDB */
		__jit_debug_descriptor.relevant_entry = entry;
		__jit_debug_register_code();

		free(entry);
	}
}

#if defined(__FreeBSD__)
static bool ir_gdb_info_proc(pid_t pid, struct kinfo_proc *proc)
{
	size_t len, plen;
	len = plen = sizeof(*proc);
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};

	if (sysctl(mib, 4, proc, &len, NULL, 0) < 0 || len != plen ||
            proc->ki_structsize != (int)plen || proc->ki_pid != pid) {
		return false;
	}

	return true;
}
#endif

bool ir_gdb_present(void)
{
	bool ret = 0;
#if defined(__linux__) /* netbsd while having this procfs part, does not hold the tracer pid */
	int fd = open("/proc/self/status", O_RDONLY);

	if (fd > 0) {
		char buf[1024];
		ssize_t n = read(fd, buf, sizeof(buf) - 1);
		char *s;
		pid_t pid;

		if (n > 0) {
			buf[n] = 0;
			s = strstr(buf, "TracerPid:");
			if (s) {
				s += sizeof("TracerPid:") - 1;
				while (*s == ' ' || *s == '\t') {
					s++;
				}
				pid = atoi(s);
				if (pid) {
					char out[1024];
					sprintf(buf, "/proc/%d/exe", (int)pid);
					if (readlink(buf, out, sizeof(out) - 1) > 0) {
						if (strstr(out, "gdb")) {
							ret = 1;
						}
					}
				}
			}
		}

		close(fd);
	}
#elif defined(__FreeBSD__)
    struct kinfo_proc proc, dbg;

    if (ir_gdb_info_proc(getpid(), &proc)) {
        if ((proc.ki_flag & P_TRACED) != 0) {
            if (ir_gdb_info_proc(proc.ki_tracer, &dbg)) {
            	ret = strstr(dbg.ki_comm, "gdb");
	    }
        }
    }
#endif

	return ret;
}

int ir_gdb_register(const char    *name,
                    const void    *start,
                    size_t         size,
                    uint32_t       sp_offset,
                    uint32_t       sp_adjustment)
{
	ir_gdbjit_ctx ctx;

	ctx.mcaddr = (uintptr_t)start;
	ctx.szmcode = (uint32_t)size;
	ctx.name = name;
	ctx.filename = "unknown";
	ctx.lineno = 0;

	ir_gdbjit_buildobj(&ctx, sp_offset, sp_adjustment);

	return ir_gdb_register_code(&ctx.obj, ctx.objsize);
}

void ir_gdb_init(void)
{
	/* This might enable registration of all JIT-ed code, but unfortunately,
	 * in case of many functions, this takes enormous time. */
	if (ir_gdb_present()) {
#if 0
		_debug |= IR_DEBUG_GDB;
#endif
	}
}
