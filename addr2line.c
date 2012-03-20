/**********************************************************************

  addr2line.h -

  $Author$

  Copyright (C) 2010 Shinichiro Hamaji

**********************************************************************/

#include "ruby/config.h"
#include "addr2line.h"

#include <stdio.h>
#include <errno.h>

#ifdef USE_ELF

#ifdef __OpenBSD__
#include <elf_abi.h>
#else
#include <elf.h>
#endif
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif

#ifdef HAVE_DL_ITERATE_PHDR
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <link.h>
#endif

#define DW_LNS_copy                     0x01
#define DW_LNS_advance_pc               0x02
#define DW_LNS_advance_line             0x03
#define DW_LNS_set_file                 0x04
#define DW_LNS_set_column               0x05
#define DW_LNS_negate_stmt              0x06
#define DW_LNS_set_basic_block          0x07
#define DW_LNS_const_add_pc             0x08
#define DW_LNS_fixed_advance_pc         0x09
#define DW_LNS_set_prologue_end         0x0a /* DWARF3 */
#define DW_LNS_set_epilogue_begin       0x0b /* DWARF3 */
#define DW_LNS_set_isa                  0x0c /* DWARF3 */

/* Line number extended opcode name. */
#define DW_LNE_end_sequence             0x01
#define DW_LNE_set_address              0x02
#define DW_LNE_define_file              0x03
#define DW_LNE_set_discriminator        0x04  /* DWARF4 */

#ifndef ElfW
# if SIZEOF_VOIDP == 8
#  define ElfW(x) Elf64##_##x
# else
#  define ElfW(x) Elf32##_##x
# endif
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct {
    const char *dirname;
    const char *filename;
    int line;

    int fd;
    void *mapped;
    size_t mapped_size;
    unsigned long base_addr;
} line_info_t;

/* Avoid consuming stack as this module may be used from signal handler */
static char binary_filename[PATH_MAX];

static unsigned long
uleb128(char **p)
{
    unsigned long r = 0;
    int s = 0;
    for (;;) {
	unsigned char b = *(unsigned char *)(*p)++;
	if (b < 0x80) {
	    r += (unsigned long)b << s;
	    break;
	}
	r += (b & 0x7f) << s;
	s += 7;
    }
    return r;
}

static long
sleb128(char **p)
{
    long r = 0;
    int s = 0;
    for (;;) {
	unsigned char b = *(unsigned char *)(*p)++;
	if (b < 0x80) {
	    if (b & 0x40) {
		r -= (0x80 - b) << s;
	    }
	    else {
		r += (b & 0x3f) << s;
	    }
	    break;
	}
	r += (b & 0x7f) << s;
	s += 7;
    }
    return r;
}

static const char *
get_nth_dirname(unsigned long dir, char *p)
{
    if (!dir--) {
	return "";
    }
    while (dir--) {
	while (*p) p++;
	p++;
	if (!*p) {
	    fprintf(stderr, "Unexpected directory number %lu in %s\n",
		    dir, binary_filename);
	    return "";
	}
    }
    return p;
}

static void
fill_filename(int file, char *include_directories, char *filenames,
	      line_info_t *line)
{
    int i;
    char *p = filenames;
    char *filename;
    unsigned long dir;
    for (i = 1; i <= file; i++) {
	filename = p;
	if (!*p) {
	    /* Need to output binary file name? */
	    fprintf(stderr, "Unexpected file number %d in %s\n",
		    file, binary_filename);
	    return;
	}
	while (*p) p++;
	p++;
	dir = uleb128(&p);
	/* last modified. */
	uleb128(&p);
	/* size of the file. */
	uleb128(&p);

	if (i == file) {
	    line->filename = filename;
	    line->dirname = get_nth_dirname(dir, include_directories);
	}
    }
}

static int
get_path_from_symbol(const char *symbol, const char **p, size_t *len)
{
    if (symbol[0] == '0') {
	/* libexecinfo */
	*p   = strchr(symbol, '/');
	if (*p == NULL) return 0;
	*len = strlen(*p);
    }
    else {
	/* glibc */
	const char *q;
	*p   = symbol;
	q   = strchr(symbol, '(');
	if (q == NULL) return 0;
	*len = q - symbol;
    }
    return 1;
}

static void
fill_line(int num_traces, void **traces,
	  unsigned long addr, int file, int line,
	  char *include_directories, char *filenames, line_info_t *lines)
{
    int i;
    for (i = 0; i < num_traces; i++) {
	unsigned long a = (unsigned long)traces[i] - lines[i].base_addr;
	/* We assume one line code doesn't result >100 bytes of native code.
       We may want more reliable way eventually... */
	if (addr < a && a < addr + 100) {
	    fill_filename(file, include_directories, filenames, &lines[i]);
	    lines[i].line = line;
	}
    }
}

static void
parse_debug_line_cu(int num_traces, void **traces,
		    char **debug_line, line_info_t *lines)
{
    char *p, *cu_end, *cu_start, *include_directories, *filenames;
    unsigned long unit_length;
    int default_is_stmt, line_base;
    unsigned int header_length, minimum_instruction_length, line_range,
		 opcode_base;
    unsigned char *standard_opcode_lengths;

    /* The registers. */
    unsigned long addr = 0;
    unsigned int file = 1;
    unsigned int line = 1;
    unsigned int column = 0;
    int is_stmt;
    int basic_block = 0;
    int end_sequence = 0;
    int prologue_end = 0;
    int epilogue_begin = 0;
    unsigned int isa = 0;

    p = *debug_line;

    unit_length = *(unsigned int *)p;
    p += sizeof(unsigned int);
    if (unit_length == 0xffffffff) {
	unit_length = *(unsigned long *)p;
	p += sizeof(unsigned long);
    }

    cu_end = p + unit_length;

    /*dwarf_version = *(unsigned short *)p;*/
    p += 2;

    header_length = *(unsigned int *)p;
    p += sizeof(unsigned int);

    cu_start = p + header_length;

    minimum_instruction_length = *(unsigned char *)p;
    p++;

    is_stmt = default_is_stmt = *(unsigned char *)p;
    p++;

    line_base = *(char *)p;
    p++;

    line_range = *(unsigned char *)p;
    p++;

    opcode_base = *(unsigned char *)p;
    p++;

    standard_opcode_lengths = (unsigned char *)p - 1;
    p += opcode_base - 1;

    include_directories = p;

    /* skip include directories */
    while (*p) {
	while (*p) p++;
	p++;
    }
    p++;

    filenames = p;

    p = cu_start;

#define FILL_LINE()						    \
    do {							    \
	fill_line(num_traces, traces, addr, file, line,		    \
		  include_directories, filenames, lines);	    \
	basic_block = prologue_end = epilogue_begin = 0;	    \
    } while (0)

    while (p < cu_end) {
	unsigned long a;
	unsigned char op = *p++;
	switch (op) {
	case DW_LNS_copy:
	    FILL_LINE();
	    break;
	case DW_LNS_advance_pc:
	    a = uleb128(&p);
	    addr += a;
	    break;
	case DW_LNS_advance_line: {
	    long a = sleb128(&p);
	    line += a;
	    break;
	}
	case DW_LNS_set_file:
	    file = (unsigned int)uleb128(&p);
	    break;
	case DW_LNS_set_column:
	    column = (unsigned int)uleb128(&p);
	    break;
	case DW_LNS_negate_stmt:
	    is_stmt = !is_stmt;
	    break;
	case DW_LNS_set_basic_block:
	    basic_block = 1;
	    break;
	case DW_LNS_const_add_pc:
	    a = ((255 - opcode_base) / line_range) *
		minimum_instruction_length;
	    addr += a;
	    break;
	case DW_LNS_fixed_advance_pc:
	    a = *(unsigned char *)p++;
	    addr += a;
	    break;
	case DW_LNS_set_prologue_end:
	    prologue_end = 1;
	    break;
	case DW_LNS_set_epilogue_begin:
	    epilogue_begin = 1;
	    break;
	case DW_LNS_set_isa:
	    isa = (unsigned int)uleb128(&p);
	    break;
	case 0:
	    a = *(unsigned char *)p++;
	    op = *p++;
	    switch (op) {
	    case DW_LNE_end_sequence:
		end_sequence = 1;
		FILL_LINE();
		addr = 0;
		file = 1;
		line = 1;
		column = 0;
		is_stmt = default_is_stmt;
		end_sequence = 0;
		isa = 0;
		break;
	    case DW_LNE_set_address:
		addr = *(unsigned long *)p;
		p += sizeof(unsigned long);
		break;
	    case DW_LNE_define_file:
		fprintf(stderr, "Unsupported operation in %s\n",
			binary_filename);
		break;
	    case DW_LNE_set_discriminator:
		/* TODO:currently ignore */
		uleb128(&p);
		break;
	    default:
		fprintf(stderr, "Unknown extended opcode: %d in %s\n",
			op, binary_filename);
	    }
	    break;
	default: {
	    unsigned long addr_incr;
	    unsigned long line_incr;
	    a = op - opcode_base;
	    addr_incr = (a / line_range) * minimum_instruction_length;
	    line_incr = line_base + (a % line_range);
	    addr += (unsigned int)addr_incr;
	    line += (unsigned int)line_incr;
	    FILL_LINE();
	}
	}
    }
    *debug_line = p;
}

static void
parse_debug_line(int num_traces, void **traces,
		 char *debug_line, unsigned long size, line_info_t *lines)
{
    char *debug_line_end = debug_line + size;
    while (debug_line < debug_line_end) {
	parse_debug_line_cu(num_traces, traces, &debug_line, lines);
    }
    if (debug_line != debug_line_end) {
	fprintf(stderr, "Unexpected size of .debug_line in %s\n",
		binary_filename);
    }
}

/* read file and fill lines */
static void
fill_lines(int num_traces, void **traces, char **syms, int check_debuglink,
	   line_info_t *current_line, line_info_t *lines);

static void
follow_debuglink(char *debuglink, int num_traces, void **traces, char **syms,
		 line_info_t *current_line, line_info_t *lines)
{
    /* Ideally we should check 4 paths to follow gnu_debuglink,
       but we handle only one case for now as this format is used
       by some linux distributions. See GDB's info for detail. */
    static const char global_debug_dir[] = "/usr/lib/debug";
    char *p, *subdir;

    p = strrchr(binary_filename, '/');
    if (!p) {
	return;
    }
    p[1] = '\0';

    subdir = (char *)alloca(strlen(binary_filename) + 1);
    strcpy(subdir, binary_filename);
    strcpy(binary_filename, global_debug_dir);
    strncat(binary_filename, subdir,
	    PATH_MAX - strlen(binary_filename) - 1);
    strncat(binary_filename, debuglink,
	    PATH_MAX - strlen(binary_filename) - 1);

    munmap(current_line->mapped, current_line->mapped_size);
    close(current_line->fd);
    fill_lines(num_traces, traces, syms, 0, current_line, lines);
}

/* read file and fill lines */
static void
fill_lines(int num_traces, void **traces, char **syms, int check_debuglink,
	   line_info_t *current_line, line_info_t *lines)
{
    int i;
    char *shstr;
    char *section_name;
    ElfW(Ehdr) *ehdr;
    ElfW(Shdr) *shdr, *shstr_shdr;
    ElfW(Shdr) *debug_line_shdr = NULL, *gnu_debuglink_shdr = NULL;
    int fd;
    off_t filesize;
    char *file;

    fd = open(binary_filename, O_RDONLY);
    if (fd < 0) {
	return;
    }
    filesize = lseek(fd, 0, SEEK_END);
    if (filesize < 0) {
	int e = errno;
	close(fd);
	fprintf(stderr, "lseek: %s\n", strerror(e));
	return;
    }
    lseek(fd, 0, SEEK_SET);
    /* async-signal unsafe */
    file = (char *)mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) {
	int e = errno;
	close(fd);
	fprintf(stderr, "mmap: %s\n", strerror(e));
	return;
    }

    current_line->fd = fd;
    current_line->mapped = file;
    current_line->mapped_size = filesize;

    for (i = 0; i < num_traces; i++) {
	const char *path;
	size_t len;
	if (get_path_from_symbol(syms[i], &path, &len) &&
		!strncmp(path, binary_filename, len)) {
	    lines[i].line = -1;
	}
    }

    ehdr = (ElfW(Ehdr) *)file;
    shdr = (ElfW(Shdr) *)(file + ehdr->e_shoff);

    shstr_shdr = shdr + ehdr->e_shstrndx;
    shstr = file + shstr_shdr->sh_offset;

    for (i = 0; i < ehdr->e_shnum; i++) {
	section_name = shstr + shdr[i].sh_name;
	if (!strcmp(section_name, ".debug_line")) {
	    debug_line_shdr = shdr + i;
	    break;
	} else if (!strcmp(section_name, ".gnu_debuglink")) {
	    gnu_debuglink_shdr = shdr + i;
	}
    }

    if (!debug_line_shdr) {
	/* This file doesn't have .debug_line section,
	   let's check .gnu_debuglink section instead. */
	if (gnu_debuglink_shdr && check_debuglink) {
	    follow_debuglink(file + gnu_debuglink_shdr->sh_offset,
			     num_traces, traces, syms,
			     current_line, lines);
	}
	return;
    }

    parse_debug_line(num_traces, traces,
		     file + debug_line_shdr->sh_offset,
		     debug_line_shdr->sh_size,
		     lines);
}

#ifdef HAVE_DL_ITERATE_PHDR

typedef struct {
    int num_traces;
    char **syms;
    line_info_t *lines;
} fill_base_addr_state_t;

static int
fill_base_addr(struct dl_phdr_info *info, size_t size, void *data)
{
    int i;
    fill_base_addr_state_t *st = (fill_base_addr_state_t *)data;
    for (i = 0; i < st->num_traces; i++) {
	const char *path;
	size_t len;
	size_t name_len = strlen(info->dlpi_name);

	if (get_path_from_symbol(st->syms[i], &path, &len) &&
		(len == name_len || (len > name_len && path[len-name_len-1] == '/')) &&
		!strncmp(path+len-name_len, info->dlpi_name, name_len)) {
	    st->lines[i].base_addr = info->dlpi_addr;
	}
    }
    return 0;
}

#endif /* HAVE_DL_ITERATE_PHDR */

void
rb_dump_backtrace_with_lines(int num_traces, void **trace, char **syms)
{
    int i;
    /* async-signal unsafe */
    line_info_t *lines = (line_info_t *)calloc(num_traces,
					       sizeof(line_info_t));

    /* Note that line info of shared objects might not be shown
       if we don't have dl_iterate_phdr */
#ifdef HAVE_DL_ITERATE_PHDR
    fill_base_addr_state_t fill_base_addr_state;

    fill_base_addr_state.num_traces = num_traces;
    fill_base_addr_state.syms = syms;
    fill_base_addr_state.lines = lines;
    /* maybe async-signal unsafe */
    dl_iterate_phdr(fill_base_addr, &fill_base_addr_state);
#endif /* HAVE_DL_ITERATE_PHDR */

    for (i = 0; i < num_traces; i++) {
	const char *path;
	size_t len;
	if (lines[i].line) {
	    continue;
	}

	if (!get_path_from_symbol(syms[i], &path, &len)) {
	    continue;
	}

	strncpy(binary_filename, path, len);
	binary_filename[len] = '\0';

	fill_lines(num_traces, trace, syms, 1, &lines[i], lines);
    }

    /* fprintf may not be async-signal safe */
    for (i = 0; i < num_traces; i++) {
	line_info_t *line = &lines[i];

	if (line->line > 0) {
	    fprintf(stderr, "%s ", syms[i]);
	    if (line->filename) {
		if (line->dirname && line->dirname[0]) {
		    fprintf(stderr, "%s/", line->dirname);
		}
		fprintf(stderr, "%s", line->filename);
	    } else {
		fprintf(stderr, "???");
	    }
	    fprintf(stderr, ":%d\n", line->line);
	} else {
	    fprintf(stderr, "%s\n", syms[i]);
	}
    }

    for (i = 0; i < num_traces; i++) {
	line_info_t *line = &lines[i];
	if (line->fd) {
	    munmap(line->mapped, line->mapped_size);
	    close(line->fd);
	}
    }
    free(lines);
}

#else /* defined(USE_ELF) */
#error not supported
#endif
