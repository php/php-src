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

#define HAVE_GDB

#if SIZEOF_SIZE_T == 8
# define ELF64
#else
# undef ELF64
#endif

struct _elf_header {
	uint8_t   emagic[4];
	uint8_t   eclass;
	uint8_t   eendian;
	uint8_t   eversion;
	uint8_t   eosabi;
	uint8_t   eabiversion;
	uint8_t   epad[7];
	uint16_t  type;
	uint16_t  machine;
	uint32_t  version;
	uintptr_t entry;
	uintptr_t phofs;
	uintptr_t shofs;
	uint32_t  flags;
	uint16_t  ehsize;
	uint16_t  phentsize;
	uint16_t  phnum;
	uint16_t  shentsize;
	uint16_t  shnum;
	uint16_t  shstridx;
};

struct _elf_sectheader {
	uint32_t  name;
	uint32_t  type;
	uintptr_t flags;
	uintptr_t addr;
	uintptr_t ofs;
	uintptr_t size;
	uint32_t  link;
	uint32_t  info;
	uintptr_t align;
	uintptr_t entsize;
};

struct _elf_symbol {
#ifdef ELF64
	uint32_t  name;
	uint8_t   info;
	uint8_t   other;
	uint16_t  sectidx;
	uintptr_t value;
	uint64_t  size;
#else
	uint32_t  name;
	uintptr_t value;
	uint32_t  size;
	uint8_t   info;
	uint8_t   other;
	uint16_t  sectidx;
#endif
};

enum {
	GDB_JIT_SECT_NULL,
	GDB_JIT_SECT_text,
	GDB_JIT_SECT_eh_frame,
	GDB_JIT_SECT_shstrtab,
	GDB_JIT_SECT_strtab,
	GDB_JIT_SECT_symtab,
	GDB_JIT_SECT_debug_info,
	GDB_JIT_SECT_debug_abbrev,
	GDB_JIT_SECT_debug_line,
	GDB_JIT_SECT__MAX
};

enum {
	GDB_JIT_SYM_UNDEF,
	GDB_JIT_SYM_FILE,
	GDB_JIT_SYM_FUNC,
	GDB_JIT_SYM__MAX
};

struct _gdb_jit_obj {
	struct _elf_header     hdr;
	struct _elf_sectheader sect[GDB_JIT_SECT__MAX];
	struct _elf_symbol     sym[GDB_JIT_SYM__MAX];
	uint8_t                space[4096];
};

static const struct _elf_header template = {
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
#elif defined(__NetBSD__)
	.eosabi      = 2,
#elif defined(__OpenBSD__)
	.eosabi      = 12,
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
	.shofs       = offsetof(struct _gdb_jit_obj, sect),
	.flags       = 0,
	.ehsize      = sizeof(struct _elf_header),
	.phentsize   = 0,
	.phnum       = 0,
	.shentsize   = sizeof(struct _elf_sectheader),
	.shnum       = GDB_JIT_SECT__MAX,
	.shstridx    = GDB_JIT_SECT_shstrtab
};

#define GDB_JIT_NOACTION    0
#define GDB_JIT_REGISTER    1
#define GDB_JIT_UNREGISTYER 2

struct _gdb_jit_code_entry {
	struct _gdb_jit_code_entry *next_entry;
	struct _gdb_jit_code_entry *prev_entry;
	const char                 *symfile_addr;
	uint64_t                    symfile_size;
};

struct _gdb_jit_descriptor {
	uint32_t                    version;
	uint32_t                    action_flag;
	struct _gdb_jit_code_entry *relevant_entry;
	struct _gdb_jit_code_entry *first_entry;
};

struct _gdb_jit_descriptor __jit_debug_descriptor = {
	1, GDB_JIT_NOACTION, NULL, NULL
};

zend_never_inline void __jit_debug_register_code()
{
	__asm__ __volatile__("");
}

static int zend_jit_gdb_register(const char *name,
                                 const void *start,
                                 size_t      size)
{
	struct _gdb_jit_code_entry *entry;
	struct _gdb_jit_obj	obj;
	size_t obj_size;

	// TODO: create in-memory ELF object file ???
	return 0;

	entry = malloc(sizeof(struct _gdb_jit_code_entry) + size);
	if (!entry) {
		return 0;
	}

	entry->symfile_addr = ((char*)entry) + sizeof(struct _gdb_jit_code_entry);
	entry->symfile_size = obj_size;
	memcpy(entry->symfile_addr, &obj, obj_size);

	entry->prev_entry = NULL;
	entry->next_entry = __jit_debug_descriptor.first_entry;
	if (__jit_debug_descriptor.first_entry) {
		__jit_debug_descriptor.first_entry->prev_entry = entry;
	}
	__jit_debug_descriptor.first_entry = entry;
	__jit_debug_descriptor.relevant_entry = entry;

	__jit_debug_descriptor.action_flag = GDB_JIT_REGISTER;
	__jit_debug_register_code();

	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
