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
   | Based on Mike Pall's implementation of GDB interface for LuaJIT.     |
   | LuaJIT -- a Just-In-Time Compiler for Lua. http://luajit.org/        |
   +----------------------------------------------------------------------+
*/

#define HAVE_GDB

#ifdef HAVE_GDB

#include "zend_elf.h"
#include "zend_gdb.h"

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
#if defined(__i386__)
	DW_REG_AX, DW_REG_CX, DW_REG_DX, DW_REG_BX,
	DW_REG_SP, DW_REG_BP, DW_REG_SI, DW_REG_DI,
	DW_REG_RA,
#elif defined(__x86_64__)
	/* Yes, the order is strange, but correct. */
	DW_REG_AX, DW_REG_DX, DW_REG_CX, DW_REG_BX,
	DW_REG_SI, DW_REG_DI, DW_REG_BP, DW_REG_SP,
	DW_REG_8, DW_REG_9, DW_REG_10, DW_REG_11,
	DW_REG_12, DW_REG_13, DW_REG_14, DW_REG_15,
	DW_REG_RA,
	/* TODO: ARM supports? */
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

typedef struct _zend_gdbjit_obj {
	zend_elf_header     hdr;
	zend_elf_sectheader sect[GDBJIT_SECT__MAX];
	zend_elf_symbol     sym[GDBJIT_SYM__MAX];
	uint8_t             space[4096];
} zend_gdbjit_obj;

static const zend_elf_header zend_elfhdr_template = {
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
	.eosabi      = 0,  /* Nope, it's not 3. ??? */
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
#if defined(__i386__)
	.machine     = 3,
#elif defined(__x86_64__)
	.machine     = 62,
#else
# error "Unsupported target architecture"
#endif
	.version     = 1,
	.entry       = 0,
	.phofs       = 0,
	.shofs       = offsetof(zend_gdbjit_obj, sect),
	.flags       = 0,
	.ehsize      = sizeof(zend_elf_header),
	.phentsize   = 0,
	.phnum       = 0,
	.shentsize   = sizeof(zend_elf_sectheader),
	.shnum       = GDBJIT_SECT__MAX,
	.shstridx    = GDBJIT_SECT_shstrtab
};

/* Context for generating the ELF object for the GDB JIT API. */
typedef struct _zend_gdbjit_ctx {
	uint8_t *p;              /* Pointer to next address in obj.space. */
	uint8_t *startp;         /* Pointer to start address in obj.space. */
	uintptr_t mcaddr;        /* Machine code address. */
	uint32_t szmcode;        /* Size of machine code. */
	int32_t  lineno;         /* Starting line number. */
	const char *name;        /* JIT function name */
	const char *filename;    /* Starting file name. */
	size_t objsize;          /* Final size of ELF object. */
	zend_gdbjit_obj obj;     /* In-memory ELF object. */
} zend_gdbjit_ctx;

/* Add a zero-terminated string */
static uint32_t zend_gdbjit_strz(zend_gdbjit_ctx *ctx, const char *str)
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
static void zend_gdbjit_uleb128(zend_gdbjit_ctx *ctx, uint32_t v)
{
	uint8_t *p = ctx->p;
	for (; v >= 0x80; v >>= 7)
		*p++ = (uint8_t)((v & 0x7f) | 0x80);
	*p++ = (uint8_t)v;
	ctx->p = p;
}

/* Add a SLEB128 value */
static void zend_gdbjit_sleb128(zend_gdbjit_ctx *ctx, int32_t v)
{
	uint8_t *p = ctx->p;
	for (; (uint32_t)(v+0x40) >= 0x80; v >>= 7)
		*p++ = (uint8_t)((v & 0x7f) | 0x80);
	*p++ = (uint8_t)(v & 0x7f);
	ctx->p = p;
}

static void zend_gdbjit_secthdr(zend_gdbjit_ctx *ctx)
{
	zend_elf_sectheader *sect;

	*ctx->p++ = '\0';

#define SECTDEF(id, tp, al)                       \
	sect = &ctx->obj.sect[GDBJIT_SECT_##id];      \
	sect->name = zend_gdbjit_strz(ctx, "." #id);  \
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
	sect->ofs = offsetof(zend_gdbjit_obj, sym);
	sect->size = sizeof(ctx->obj.sym);
	sect->link = GDBJIT_SECT_strtab;
	sect->entsize = sizeof(zend_elf_symbol);
	sect->info = GDBJIT_SYM_FUNC;

	SECTDEF(debug_info, PROGBITS, 1);
	SECTDEF(debug_abbrev, PROGBITS, 1);
	SECTDEF(debug_line, PROGBITS, 1);

#undef SECTDEF
}

static void zend_gdbjit_symtab(zend_gdbjit_ctx *ctx)
{
	zend_elf_symbol *sym;

	*ctx->p++ = '\0';

	sym = &ctx->obj.sym[GDBJIT_SYM_FILE];
	sym->name = zend_gdbjit_strz(ctx, "JIT code");
	sym->sectidx = ELFSECT_IDX_ABS;
	sym->info = ELFSYM_INFO(ELFSYM_BIND_LOCAL, ELFSYM_TYPE_FILE);

	sym = &ctx->obj.sym[GDBJIT_SYM_FUNC];
	sym->name = zend_gdbjit_strz(ctx, ctx->name);
	sym->sectidx = GDBJIT_SECT_text;
	sym->value = 0;
	sym->size = ctx->szmcode;
	sym->info = ELFSYM_INFO(ELFSYM_BIND_GLOBAL, ELFSYM_TYPE_FUNC);
}

#define SECTALIGN(p, a) \
	  ((p) = (uint8_t *)(((uintptr_t)(p) + ((a)-1)) & ~(uintptr_t)((a)-1)))

/* Shortcuts to generate DWARF structures. */
#define DB(x)       (*p++ = (x))
#define DI8(x)      (*(int8_t *)p = (x), p++)
#define DU16(x)     (*(uint16_t *)p = (x), p += 2)
#define DU32(x)     (*(uint32_t *)p = (x), p += 4)
#define DADDR(x)    (*(uintptr_t *)p = (x), p += sizeof(uintptr_t))
#define DUV(x)      (ctx->p = p, zend_gdbjit_uleb128(ctx, (x)), p = ctx->p)
#define DSV(x)      (ctx->p = p, zend_gdbjit_sleb128(ctx, (x)), p = ctx->p)
#define DSTR(str)   (ctx->p = p, zend_gdbjit_strz(ctx, (str)), p = ctx->p)
#define DALIGNNOP(s)    while ((uintptr_t)p & ((s)-1)) *p++ = DW_CFA_nop
#define DSECT(name, stmt) \
	{ uint32_t *szp_##name = (uint32_t *)p; p += 4; stmt \
		*szp_##name = (uint32_t)((p-(uint8_t *)szp_##name)-4); }

static void zend_gdbjit_ehframe(zend_gdbjit_ctx *ctx)
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
		DB(DW_CFA_def_cfa); DUV(DW_REG_SP); DUV(sizeof(uintptr_t));
		DB(DW_CFA_offset|DW_REG_RA); DUV(1);
		DALIGNNOP(sizeof(uintptr_t));
	)

	/* DWARF EH FDE (Frame Description Entry). */
	DSECT(FDE,
		DU32((uint32_t)(p-framep)); /* Offset to CIE Pointer. */
		DU32(0);                    /* Machine code offset relative to .text. */
		DU32(ctx->szmcode);         /* Machine code length. */
		DB(0);                      /* Augmentation data. */
		DB(DW_CFA_def_cfa_offset); DUV(sizeof(uintptr_t));
#if defined(__i386__)
		DB(DW_CFA_advance_loc|3);            /* sub $0xc,%esp */
		DB(DW_CFA_def_cfa_offset); DUV(16);  /* Aligned stack frame size. */
#elif defined(__x86_64__)
		DB(DW_CFA_advance_loc|4);            /* sub $0x8,%rsp */
		DB(DW_CFA_def_cfa_offset); DUV(16);  /* Aligned stack frame size. */
#else
# error "Unsupported target architecture"
#endif
		DALIGNNOP(sizeof(uintptr_t));
	)

	ctx->p = p;
}

static void zend_gdbjit_debuginfo(zend_gdbjit_ctx *ctx)
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

static void zend_gdbjit_debugabbrev(zend_gdbjit_ctx *ctx)
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

	ctx->p = p;
}

#define DLNE(op, s) (DB(DW_LNS_extended_op), DUV(1+(s)), DB((op)))

static void zend_gdbjit_debugline(zend_gdbjit_ctx *ctx)
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

typedef void (*zend_gdbjit_initf) (zend_gdbjit_ctx *ctx);

static void zend_gdbjit_initsect(zend_gdbjit_ctx *ctx, int sect, zend_gdbjit_initf initf)
{
	ctx->startp = ctx->p;
	ctx->obj.sect[sect].ofs = (uintptr_t)((char *)ctx->p - (char *)&ctx->obj);
	initf(ctx);
	ctx->obj.sect[sect].size = (uintptr_t)(ctx->p - ctx->startp);
}

static void zend_gdbjit_buildobj(zend_gdbjit_ctx *ctx)
{
	zend_gdbjit_obj *obj = &ctx->obj;

	/* Fill in ELF header and clear structures. */
	memcpy(&obj->hdr, &zend_elfhdr_template, sizeof(zend_elf_header));
	memset(&obj->sect, 0, sizeof(zend_elf_sectheader) * GDBJIT_SECT__MAX);
	memset(&obj->sym, 0, sizeof(zend_elf_symbol) * GDBJIT_SYM__MAX);

	/* Initialize sections. */
	ctx->p = obj->space;
	zend_gdbjit_initsect(ctx, GDBJIT_SECT_shstrtab, zend_gdbjit_secthdr);
	zend_gdbjit_initsect(ctx, GDBJIT_SECT_strtab, zend_gdbjit_symtab);
	zend_gdbjit_initsect(ctx, GDBJIT_SECT_debug_info, zend_gdbjit_debuginfo);
	zend_gdbjit_initsect(ctx, GDBJIT_SECT_debug_abbrev, zend_gdbjit_debugabbrev);
	zend_gdbjit_initsect(ctx, GDBJIT_SECT_debug_line, zend_gdbjit_debugline);
	SECTALIGN(ctx->p, sizeof(uintptr_t));
	zend_gdbjit_initsect(ctx, GDBJIT_SECT_eh_frame, zend_gdbjit_ehframe);
	ctx->objsize = (size_t)((char *)ctx->p - (char *)obj);

	ZEND_ASSERT(ctx->objsize < sizeof(zend_gdbjit_obj));
}

static int zend_jit_gdb_register(const char    *name,
                                 const zend_op_array *op_array,
                                 const void    *start,
                                 size_t         size)
{
	zend_gdbjit_ctx ctx;

	ctx.mcaddr = (uintptr_t)start;
	ctx.szmcode = (uint32_t)size;
	ctx.name = name;
	ctx.filename = op_array ? ZSTR_VAL(op_array->filename) : "unknown";
	ctx.lineno = op_array ? op_array->line_start : 0;

	zend_gdbjit_buildobj(&ctx);

	return zend_gdb_register_code(&ctx.obj, ctx.objsize);
}

static int zend_jit_gdb_unregister(void)
{
	zend_gdb_unregister_all();
	return 1;
}

static void zend_jit_gdb_init(void)
{
#if 0
	/* This might enable registration of all JIT-ed code, but unfortunately,
	 * in case of many functions, this takes enormous time. */
	if (zend_gdb_present()) {
		JIT_G(debug) |= ZEND_JIT_DEBUG_GDB;
	}
#endif
}

#endif
