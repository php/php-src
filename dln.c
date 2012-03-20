/**********************************************************************

  dln.c -

  $Author$
  created at: Tue Jan 18 17:05:06 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#ifdef RUBY_EXPORT
#include "ruby/ruby.h"
#define dln_notimplement rb_notimplement
#define dln_memerror rb_memerror
#define dln_exit rb_exit
#define dln_loaderror rb_loaderror
#else
#define dln_notimplement --->>> dln not implemented <<<---
#define dln_memerror abort
#define dln_exit exit
static void dln_loaderror(const char *format, ...);
#endif
#include "dln.h"

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef USE_DLN_A_OUT
char *dln_argv0;
#endif

#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifndef xmalloc
void *xmalloc();
void *xcalloc();
void *xrealloc();
#endif

#define free(x) xfree(x)

#include <stdio.h>
#if defined(_WIN32)
#include "missing/file.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>

#ifndef S_ISDIR
#   define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifndef _WIN32
char *getenv();
#endif

#if defined(__APPLE__) && defined(__MACH__)   /* Mac OS X */
# if defined(HAVE_DLOPEN)
   /* Mac OS X with dlopen (10.3 or later) */
#  define MACOSX_DLOPEN
# else
#  define MACOSX_DYLD
# endif
#endif

#if defined(__BEOS__) || defined(__HAIKU__)
# include <image.h>
#endif

#ifndef dln_loaderror
static void
dln_loaderror(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    abort();
}
#endif

#if defined(HAVE_DLOPEN) && !defined(USE_DLN_A_OUT) && !defined(_AIX) && !defined(MACOSX_DYLD) && !defined(_UNICOSMP)
/* dynamic load with dlopen() */
# define USE_DLN_DLOPEN
#endif

#ifndef FUNCNAME_PATTERN
# if defined(__hp9000s300) || ((defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)) && !defined(__ELF__)) || defined(__BORLANDC__) || defined(NeXT) || defined(__WATCOMC__) || defined(MACOSX_DYLD)
#  define FUNCNAME_PREFIX "_Init_"
# else
#  define FUNCNAME_PREFIX "Init_"
# endif
#endif

#if defined __CYGWIN__ || defined DOSISH
#define isdirsep(x) ((x) == '/' || (x) == '\\')
#else
#define isdirsep(x) ((x) == '/')
#endif

static size_t
init_funcname_len(const char **file)
{
    const char *p = *file, *base, *dot = NULL;

    /* Load the file as an object one */
    for (base = p; *p; p++) { /* Find position of last '/' */
	if (*p == '.' && !dot) dot = p;
	if (isdirsep(*p)) base = p+1, dot = NULL;
    }
    *file = base;
    /* Delete suffix if it exists */
    return (dot ? dot : p) - base;
}

static const char funcname_prefix[sizeof(FUNCNAME_PREFIX) - 1] = FUNCNAME_PREFIX;

#define init_funcname(buf, file) do {\
    const char *base = (file);\
    const size_t flen = init_funcname_len(&base);\
    const size_t plen = sizeof(funcname_prefix);\
    char *const tmp = ALLOCA_N(char, plen+flen+1);\
    if (!tmp) {\
	dln_memerror();\
    }\
    memcpy(tmp, funcname_prefix, plen);\
    memcpy(tmp+plen, base, flen);\
    tmp[plen+flen] = '\0';\
    *(buf) = tmp;\
} while (0)

#ifdef USE_DLN_A_OUT

#ifndef LIBC_NAME
# define LIBC_NAME "libc.a"
#endif

#ifndef DLN_DEFAULT_LIB_PATH
#  define DLN_DEFAULT_LIB_PATH "/lib:/usr/lib:/usr/local/lib:."
#endif

#include <errno.h>

static int dln_errno;

#define DLN_ENOEXEC	ENOEXEC	/* Exec format error */
#define DLN_ECONFL	1201	/* Symbol name conflict */
#define DLN_ENOINIT	1202	/* No initializer given */
#define DLN_EUNDEF	1203	/* Undefine symbol remains */
#define DLN_ENOTLIB	1204	/* Not a library file */
#define DLN_EBADLIB	1205	/* Malformed library file */
#define DLN_EINIT	1206	/* Not initialized */

static int dln_init_p = 0;

#include <ar.h>
#include <a.out.h>
#ifndef N_COMM
# define N_COMM 0x12
#endif
#ifndef N_MAGIC
# define N_MAGIC(x) (x).a_magic
#endif

#define INVALID_OBJECT(h) (N_MAGIC(h) != OMAGIC)

#include "ruby/util.h"
#include "ruby/st.h"

static st_table *sym_tbl;
static st_table *undef_tbl;

static int load_lib();

static int
load_header(int fd, struct exec *hdrp, long disp)
{
    int size;

    lseek(fd, disp, 0);
    size = read(fd, hdrp, sizeof(struct exec));
    if (size == -1) {
	dln_errno = errno;
	return -1;
    }
    if (size != sizeof(struct exec) || N_BADMAG(*hdrp)) {
	dln_errno = DLN_ENOEXEC;
	return -1;
    }
    return 0;
}

#if defined(sequent)
#define RELOC_SYMBOL(r)			((r)->r_symbolnum)
#define RELOC_MEMORY_SUB_P(r)		((r)->r_bsr)
#define RELOC_PCREL_P(r)		((r)->r_pcrel || (r)->r_bsr)
#define RELOC_TARGET_SIZE(r)		((r)->r_length)
#endif

/* Default macros */
#ifndef RELOC_ADDRESS
#define RELOC_ADDRESS(r)		((r)->r_address)
#define RELOC_EXTERN_P(r)		((r)->r_extern)
#define RELOC_SYMBOL(r)			((r)->r_symbolnum)
#define RELOC_MEMORY_SUB_P(r)		0
#define RELOC_PCREL_P(r)		((r)->r_pcrel)
#define RELOC_TARGET_SIZE(r)		((r)->r_length)
#endif

#if defined(__sun) && defined(__sparc)
/* Sparc (Sun 4) macros */
#  undef relocation_info
#  define relocation_info reloc_info_sparc
#  define R_RIGHTSHIFT(r)	(reloc_r_rightshift[(r)->r_type])
#  define R_BITSIZE(r) 		(reloc_r_bitsize[(r)->r_type])
#  define R_LENGTH(r)		(reloc_r_length[(r)->r_type])
static int reloc_r_rightshift[] = {
  0, 0, 0, 0, 0, 0, 2, 2, 10, 0, 0, 0, 0, 0, 0,
};
static int reloc_r_bitsize[] = {
  8, 16, 32, 8, 16, 32, 30, 22, 22, 22, 13, 10, 32, 32, 16,
};
static int reloc_r_length[] = {
  0, 1, 2, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};
#  define R_PCREL(r) \
    ((r)->r_type >= RELOC_DISP8 && (r)->r_type <= RELOC_WDISP22)
#  define R_SYMBOL(r) ((r)->r_index)
#endif

#if defined(sequent)
#define R_SYMBOL(r)		((r)->r_symbolnum)
#define R_MEMORY_SUB(r)		((r)->r_bsr)
#define R_PCREL(r)		((r)->r_pcrel || (r)->r_bsr)
#define R_LENGTH(r)		((r)->r_length)
#endif

#ifndef R_SYMBOL
#  define R_SYMBOL(r) 		((r)->r_symbolnum)
#  define R_MEMORY_SUB(r)	0
#  define R_PCREL(r)  		((r)->r_pcrel)
#  define R_LENGTH(r) 		((r)->r_length)
#endif

static struct relocation_info *
load_reloc(int fd, struct exec *hdrp, long disp)
{
    struct relocation_info *reloc;
    int size;

    lseek(fd, disp + N_TXTOFF(*hdrp) + hdrp->a_text + hdrp->a_data, 0);
    size = hdrp->a_trsize + hdrp->a_drsize;
    reloc = (struct relocation_info*)xmalloc(size);
    if (reloc == NULL) {
	dln_errno = errno;
	return NULL;
    }

    if (read(fd, reloc, size) !=  size) {
	dln_errno = errno;
	free(reloc);
	return NULL;
    }

    return reloc;
}

static struct nlist *
load_sym(int fd, struct exec *hdrp, long disp)
{
    struct nlist * buffer;
    struct nlist * sym;
    struct nlist * end;
    long displ;
    int size;

    lseek(fd, N_SYMOFF(*hdrp) + hdrp->a_syms + disp, 0);
    if (read(fd, &size, sizeof(int)) != sizeof(int)) {
	goto err_noexec;
    }

    buffer = (struct nlist*)xmalloc(hdrp->a_syms + size);
    if (buffer == NULL) {
	dln_errno = errno;
	return NULL;
    }

    lseek(fd, disp + N_SYMOFF(*hdrp), 0);
    if (read(fd, buffer, hdrp->a_syms + size) != hdrp->a_syms + size) {
	free(buffer);
	goto err_noexec;
    }

    sym = buffer;
    end = sym + hdrp->a_syms / sizeof(struct nlist);
    displ = (long)buffer + (long)(hdrp->a_syms);

    while (sym < end) {
	sym->n_un.n_name = (char*)sym->n_un.n_strx + displ;
	sym++;
    }
    return buffer;

  err_noexec:
    dln_errno = DLN_ENOEXEC;
    return NULL;
}

static st_table *
sym_hash(struct exec *hdrp, struct nlist *syms)
{
    st_table *tbl;
    struct nlist *sym = syms;
    struct nlist *end = syms + (hdrp->a_syms / sizeof(struct nlist));

    tbl = st_init_strtable();
    if (tbl == NULL) {
	dln_errno = errno;
	return NULL;
    }

    while (sym < end) {
	st_insert(tbl, sym->n_un.n_name, sym);
	sym++;
    }
    return tbl;
}

static int
dln_init(const char *prog)
{
    char *file, fbuf[MAXPATHLEN];
    int fd;
    struct exec hdr;
    struct nlist *syms;

    if (dln_init_p == 1) return 0;

    file = dln_find_exe_r(prog, NULL, fbuf, sizeof(fbuf));
    if (file == NULL || (fd = open(file, O_RDONLY)) < 0) {
	dln_errno = errno;
	return -1;
    }

    if (load_header(fd, &hdr, 0) == -1) return -1;
    syms = load_sym(fd, &hdr, 0);
    if (syms == NULL) {
	close(fd);
	return -1;
    }
    sym_tbl = sym_hash(&hdr, syms);
    if (sym_tbl == NULL) {	/* file may be start with #! */
	char c = '\0';
	char buf[MAXPATHLEN];
	char *p;

	free(syms);
	lseek(fd, 0L, 0);
	if (read(fd, &c, 1) == -1) {
	    dln_errno = errno;
	    return -1;
	}
	if (c != '#') goto err_noexec;
	if (read(fd, &c, 1) == -1) {
	    dln_errno = errno;
	    return -1;
	}
	if (c != '!') goto err_noexec;

	p = buf;
	/* skip forwarding spaces */
	while (read(fd, &c, 1) == 1) {
	    if (c == '\n') goto err_noexec;
	    if (c != '\t' && c != ' ') {
		*p++ = c;
		break;
	    }
	}
	/* read in command name */
	while (read(fd, p, 1) == 1) {
	    if (*p == '\n' || *p == '\t' || *p == ' ') break;
	    p++;
	    if (p-buf >= MAXPATHLEN) {
		dln_errno = ENAMETOOLONG;
		return -1;
	    }
	}
	*p = '\0';

	return dln_init(buf);
    }
    dln_init_p = 1;
    undef_tbl = st_init_strtable();
    close(fd);
    return 0;

  err_noexec:
    close(fd);
    dln_errno = DLN_ENOEXEC;
    return -1;
}

static long
load_text_data(int fd, struct exec *hdrp, int bss, long disp)
{
    int size;
    unsigned char* addr;

    lseek(fd, disp + N_TXTOFF(*hdrp), 0);
    size = hdrp->a_text + hdrp->a_data;

    if (bss == -1) size += hdrp->a_bss;
    else if (bss > 1) size += bss;

    addr = (unsigned char*)xmalloc(size);
    if (addr == NULL) {
	dln_errno = errno;
	return 0;
    }

    if (read(fd, addr, size) !=  size) {
	dln_errno = errno;
	free(addr);
	return 0;
    }

    if (bss == -1) {
	memset(addr +  hdrp->a_text + hdrp->a_data, 0, hdrp->a_bss);
    }
    else if (bss > 0) {
	memset(addr +  hdrp->a_text + hdrp->a_data, 0, bss);
    }

    return (long)addr;
}

static int
undef_print(char *key, char *value)
{
    fprintf(stderr, "  %s\n", key);
    return ST_CONTINUE;
}

static void
dln_print_undef(void)
{
    fprintf(stderr, " Undefined symbols:\n");
    st_foreach(undef_tbl, undef_print, NULL);
}

static void
dln_undefined(void)
{
    if (undef_tbl->num_entries > 0) {
	fprintf(stderr, "dln: Calling undefined function\n");
	dln_print_undef();
	dln_exit(1);
    }
}

struct undef {
    char *name;
    struct relocation_info reloc;
    long base;
    char *addr;
    union {
	char c;
	short s;
	long l;
    } u;
};

static st_table *reloc_tbl = NULL;
static void
link_undef(const char *name, long base, struct relocation_info *reloc)
{
    static int u_no = 0;
    struct undef *obj;
    char *addr = (char*)(reloc->r_address + base);

    obj = (struct undef*)xmalloc(sizeof(struct undef));
    obj->name = strdup(name);
    obj->reloc = *reloc;
    obj->base = base;
    switch (R_LENGTH(reloc)) {
      case 0:		/* byte */
	obj->u.c = *addr;
	break;
      case 1:		/* word */
	obj->u.s = *(short*)addr;
	break;
      case 2:		/* long */
	obj->u.l = *(long*)addr;
	break;
    }
    if (reloc_tbl == NULL) {
	reloc_tbl = st_init_numtable();
    }
    st_insert(reloc_tbl, u_no++, obj);
}

struct reloc_arg {
    const char *name;
    long value;
};

static int
reloc_undef(int no, struct undef *undef, struct reloc_arg *arg)
{
    int datum;
    char *address;
#if defined(__sun) && defined(__sparc)
    unsigned int mask = 0;
#endif

    if (strcmp(arg->name, undef->name) != 0) return ST_CONTINUE;
    address = (char*)(undef->base + undef->reloc.r_address);
    datum = arg->value;

    if (R_PCREL(&(undef->reloc))) datum -= undef->base;
#if defined(__sun) && defined(__sparc)
    datum += undef->reloc.r_addend;
    datum >>= R_RIGHTSHIFT(&(undef->reloc));
    mask = (1 << R_BITSIZE(&(undef->reloc))) - 1;
    mask |= mask -1;
    datum &= mask;
    switch (R_LENGTH(&(undef->reloc))) {
      case 0:
	*address = undef->u.c;
	*address &= ~mask;
	*address |= datum;
	break;
      case 1:
	*(short *)address = undef->u.s;
	*(short *)address &= ~mask;
	*(short *)address |= datum;
	break;
      case 2:
	*(long *)address = undef->u.l;
	*(long *)address &= ~mask;
	*(long *)address |= datum;
	break;
    }
#else
    switch (R_LENGTH(&(undef->reloc))) {
      case 0:		/* byte */
	if (R_MEMORY_SUB(&(undef->reloc)))
	    *address = datum - *address;
	else *address = undef->u.c + datum;
	break;
      case 1:		/* word */
	if (R_MEMORY_SUB(&(undef->reloc)))
	    *(short*)address = datum - *(short*)address;
	else *(short*)address = undef->u.s + datum;
	break;
      case 2:		/* long */
	if (R_MEMORY_SUB(&(undef->reloc)))
	    *(long*)address = datum - *(long*)address;
	else *(long*)address = undef->u.l + datum;
	break;
    }
#endif
    free(undef->name);
    free(undef);
    return ST_DELETE;
}

static void
unlink_undef(const char *name, long value)
{
    struct reloc_arg arg;

    arg.name = name;
    arg.value = value;
    st_foreach(reloc_tbl, reloc_undef, &arg);
}

#ifdef N_INDR
struct indr_data {
    char *name0, *name1;
};

static int
reloc_repl(int no, struct undef *undef, struct indr_data *data)
{
    if (strcmp(data->name0, undef->name) == 0) {
	free(undef->name);
	undef->name = strdup(data->name1);
    }
    return ST_CONTINUE;
}
#endif

static int
load_1(int fd, long disp, const char *need_init)
{
    static const char *libc = LIBC_NAME;
    struct exec hdr;
    struct relocation_info *reloc = NULL;
    long block = 0;
    long new_common = 0; /* Length of new common */
    struct nlist *syms = NULL;
    struct nlist *sym;
    struct nlist *end;
    int init_p = 0;

    if (load_header(fd, &hdr, disp) == -1) return -1;
    if (INVALID_OBJECT(hdr)) {
	dln_errno = DLN_ENOEXEC;
	return -1;
    }
    reloc = load_reloc(fd, &hdr, disp);
    if (reloc == NULL) return -1;

    syms = load_sym(fd, &hdr, disp);
    if (syms == NULL) {
	free(reloc);
	return -1;
    }

    sym = syms;
    end = syms + (hdr.a_syms / sizeof(struct nlist));
    while (sym < end) {
	struct nlist *old_sym;
	int value = sym->n_value;

#ifdef N_INDR
	if (sym->n_type == (N_INDR | N_EXT)) {
	    char *key = sym->n_un.n_name;

	    if (st_lookup(sym_tbl, sym[1].n_un.n_name, &old_sym)) {
		if (st_delete(undef_tbl, (st_data_t*)&key, NULL)) {
		    unlink_undef(key, old_sym->n_value);
		    free(key);
		}
	    }
	    else {
		struct indr_data data;

		data.name0 = sym->n_un.n_name;
		data.name1 = sym[1].n_un.n_name;
		st_foreach(reloc_tbl, reloc_repl, &data);

		st_insert(undef_tbl, strdup(sym[1].n_un.n_name), NULL);
		if (st_delete(undef_tbl, (st_data_t*)&key, NULL)) {
		    free(key);
		}
	    }
	    sym += 2;
	    continue;
	}
#endif
	if (sym->n_type == (N_UNDF | N_EXT)) {
	    if (st_lookup(sym_tbl, sym->n_un.n_name, &old_sym) == 0) {
		old_sym = NULL;
	    }

	    if (value) {
		if (old_sym) {
		    sym->n_type = N_EXT | N_COMM;
		    sym->n_value = old_sym->n_value;
		}
		else {
		    int rnd =
			value >= sizeof(double) ? sizeof(double) - 1
			    : value >= sizeof(long) ? sizeof(long) - 1
				: sizeof(short) - 1;

		    sym->n_type = N_COMM;
		    new_common += rnd;
		    new_common &= ~(long)rnd;
		    sym->n_value = new_common;
		    new_common += value;
		}
	    }
	    else {
		if (old_sym) {
		    sym->n_type = N_EXT | N_COMM;
		    sym->n_value = old_sym->n_value;
		}
		else {
		    sym->n_value = (long)dln_undefined;
		    st_insert(undef_tbl, strdup(sym->n_un.n_name), NULL);
		}
	    }
	}
	sym++;
    }

    block = load_text_data(fd, &hdr, hdr.a_bss + new_common, disp);
    if (block == 0) goto err_exit;

    sym = syms;
    while (sym < end) {
	struct nlist *new_sym;
	char *key;

	switch (sym->n_type) {
	  case N_COMM:
	    sym->n_value += hdr.a_text + hdr.a_data;
	  case N_TEXT|N_EXT:
	  case N_DATA|N_EXT:

	    sym->n_value += block;

	    if (st_lookup(sym_tbl, sym->n_un.n_name, &new_sym) != 0
		&& new_sym->n_value != (long)dln_undefined) {
		dln_errno = DLN_ECONFL;
		goto err_exit;
	    }

	    key = sym->n_un.n_name;
	    if (st_delete(undef_tbl, (st_data_t*)&key, NULL) != 0) {
		unlink_undef(key, sym->n_value);
		free(key);
	    }

	    new_sym = (struct nlist*)xmalloc(sizeof(struct nlist));
	    *new_sym = *sym;
	    new_sym->n_un.n_name = strdup(sym->n_un.n_name);
	    st_insert(sym_tbl, new_sym->n_un.n_name, new_sym);
	    break;

	  case N_TEXT:
	  case N_DATA:
	    sym->n_value += block;
	    break;
	}
	sym++;
    }

    /*
     * First comes the text-relocation
     */
    {
	struct relocation_info * rel = reloc;
	struct relocation_info * rel_beg = reloc +
	    (hdr.a_trsize/sizeof(struct relocation_info));
	struct relocation_info * rel_end = reloc +
	    (hdr.a_trsize+hdr.a_drsize)/sizeof(struct relocation_info);

	while (rel < rel_end) {
	    char *address = (char*)(rel->r_address + block);
	    long datum = 0;
#if defined(__sun) && defined(__sparc)
	    unsigned int mask = 0;
#endif

	    if(rel >= rel_beg)
		address += hdr.a_text;

	    if (rel->r_extern) { /* Look it up in symbol-table */
		sym = &(syms[R_SYMBOL(rel)]);
		switch (sym->n_type) {
		  case N_EXT|N_UNDF:
		    link_undef(sym->n_un.n_name, block, rel);
		  case N_EXT|N_COMM:
		  case N_COMM:
		    datum = sym->n_value;
		    break;
		  default:
		    goto err_exit;
		}
	    } /* end.. look it up */
	    else { /* is static */
		switch (R_SYMBOL(rel)) {
		  case N_TEXT:
		  case N_DATA:
		    datum = block;
		    break;
		  case N_BSS:
		    datum = block +  new_common;
		    break;
		  case N_ABS:
		    break;
		}
	    } /* end .. is static */
	    if (R_PCREL(rel)) datum -= block;

#if defined(__sun) && defined(__sparc)
	    datum += rel->r_addend;
	    datum >>= R_RIGHTSHIFT(rel);
	    mask = (1 << R_BITSIZE(rel)) - 1;
	    mask |= mask -1;
	    datum &= mask;

	    switch (R_LENGTH(rel)) {
	      case 0:
		*address &= ~mask;
		*address |= datum;
		break;
	      case 1:
		*(short *)address &= ~mask;
		*(short *)address |= datum;
		break;
	      case 2:
		*(long *)address &= ~mask;
		*(long *)address |= datum;
		break;
	    }
#else
	    switch (R_LENGTH(rel)) {
	      case 0:		/* byte */
		if (datum < -128 || datum > 127) goto err_exit;
		*address += datum;
		break;
	      case 1:		/* word */
		*(short *)address += datum;
		break;
	      case 2:		/* long */
		*(long *)address += datum;
		break;
	    }
#endif
	    rel++;
	}
    }

    if (need_init) {
	int len;
	char **libs_to_be_linked = 0;
	char *buf;

	if (undef_tbl->num_entries > 0) {
	    if (load_lib(libc) == -1) goto err_exit;
	}

	init_funcname(&buf, need_init);
	len = strlen(buf);

	for (sym = syms; sym<end; sym++) {
	    char *name = sym->n_un.n_name;
	    if (name[0] == '_' && sym->n_value >= block) {
		if (strcmp(name+1, "dln_libs_to_be_linked") == 0) {
		    libs_to_be_linked = (char**)sym->n_value;
		}
		else if (strcmp(name+1, buf) == 0) {
		    init_p = 1;
		    ((int (*)())sym->n_value)();
		}
	    }
	}
	if (libs_to_be_linked && undef_tbl->num_entries > 0) {
	    while (*libs_to_be_linked) {
		load_lib(*libs_to_be_linked);
		libs_to_be_linked++;
	    }
	}
    }
    free(reloc);
    free(syms);
    if (need_init) {
	if (init_p == 0) {
	    dln_errno = DLN_ENOINIT;
	    return -1;
	}
	if (undef_tbl->num_entries > 0) {
	    if (load_lib(libc) == -1) goto err_exit;
	    if (undef_tbl->num_entries > 0) {
		dln_errno = DLN_EUNDEF;
		return -1;
	    }
	}
    }
    return 0;

  err_exit:
    if (syms) free(syms);
    if (reloc) free(reloc);
    if (block) free((char*)block);
    return -1;
}

static int target_offset;
static int
search_undef(const char *key, int value, st_table *lib_tbl)
{
    long offset;

    if (st_lookup(lib_tbl, key, &offset) == 0) return ST_CONTINUE;
    target_offset = offset;
    return ST_STOP;
}

struct symdef {
    int rb_str_index;
    int lib_offset;
};

const char *dln_librrb_ary_path = DLN_DEFAULT_LIB_PATH;

static int
load_lib(const char *lib)
{
    char *path, *file, fbuf[MAXPATHLEN];
    char *envpath = 0;
    char armagic[SARMAG];
    int fd, size;
    struct ar_hdr ahdr;
    st_table *lib_tbl = NULL;
    int *data, nsym;
    struct symdef *base;
    char *name_base;

    if (dln_init_p == 0) {
	dln_errno = DLN_ENOINIT;
	return -1;
    }

    if (undef_tbl->num_entries == 0) return 0;
    dln_errno = DLN_EBADLIB;

    if (lib[0] == '-' && lib[1] == 'l') {
	long len = strlen(lib) + 4;
	char *p = alloca(len);
	snprintf(p, len, "lib%s.a", lib+2);
	lib = p;
    }

    /* library search path: */
    /* look for environment variable DLN_LIBRARY_PATH first. */
    /* then variable dln_librrb_ary_path. */
    /* if path is still NULL, use "." for path. */
    path = getenv("DLN_LIBRARY_PATH");
    if (path == NULL) path = dln_librrb_ary_path;
    else path = envpath = strdup(path);

    file = dln_find_file_r(lib, path, fbuf, sizeof(fbuf));
    if (envpath) free(envpath);
    fd = open(file, O_RDONLY);
    if (fd == -1) goto syserr;
    size = read(fd, armagic, SARMAG);
    if (size == -1) goto syserr;

    if (size != SARMAG) {
	dln_errno = DLN_ENOTLIB;
	goto badlib;
    }
    size = read(fd, &ahdr, sizeof(ahdr));
    if (size == -1) goto syserr;
    if (size != sizeof(ahdr) || sscanf(ahdr.ar_size, "%d", &size) != 1) {
	goto badlib;
    }

    if (strncmp(ahdr.ar_name, "__.SYMDEF", 9) == 0) {
	/* make hash table from __.SYMDEF */

	lib_tbl = st_init_strtable();
	data = (int*)xmalloc(size);
	if (data == NULL) goto syserr;
	size = read(fd, data, size);
	nsym = *data / sizeof(struct symdef);
	base = (struct symdef*)(data + 1);
	name_base = (char*)(base + nsym) + sizeof(int);
	while (nsym > 0) {
	    char *name = name_base + base->rb_str_index;

	    st_insert(lib_tbl, name, base->lib_offset + sizeof(ahdr));
	    nsym--;
	    base++;
	}
	for (;;) {
	    target_offset = -1;
	    st_foreach(undef_tbl, search_undef, lib_tbl);
	    if (target_offset == -1) break;
	    if (load_1(fd, target_offset, 0) == -1) {
		st_free_table(lib_tbl);
		free(data);
		goto badlib;
	    }
	    if (undef_tbl->num_entries == 0) break;
	}
	free(data);
	st_free_table(lib_tbl);
    }
    else {
	/* linear library, need to scan (FUTURE) */

	for (;;) {
	    int offset = SARMAG;
	    int found = 0;
	    struct exec hdr;
	    struct nlist *syms, *sym, *end;

	    while (undef_tbl->num_entries > 0) {
		found = 0;
		lseek(fd, offset, 0);
		size = read(fd, &ahdr, sizeof(ahdr));
		if (size == -1) goto syserr;
		if (size == 0) break;
		if (size != sizeof(ahdr)
		    || sscanf(ahdr.ar_size, "%d", &size) != 1) {
		    goto badlib;
		}
		offset += sizeof(ahdr);
		if (load_header(fd, &hdr, offset) == -1)
		    goto badlib;
		syms = load_sym(fd, &hdr, offset);
		if (syms == NULL) goto badlib;
		sym = syms;
		end = syms + (hdr.a_syms / sizeof(struct nlist));
		while (sym < end) {
		    if (sym->n_type == N_EXT|N_TEXT
			&& st_lookup(undef_tbl, sym->n_un.n_name, NULL)) {
			break;
		    }
		    sym++;
		}
		if (sym < end) {
		    found++;
		    free(syms);
		    if (load_1(fd, offset, 0) == -1) {
			goto badlib;
		    }
		}
		offset += size;
		if (offset & 1) offset++;
	    }
	    if (found) break;
	}
    }
    close(fd);
    return 0;

  syserr:
    dln_errno = errno;
  badlib:
    if (fd >= 0) close(fd);
    return -1;
}

static int
load(const char *file)
{
    int fd;
    int result;

    if (dln_init_p == 0) {
	if (dln_init(dln_argv0) == -1) return -1;
    }
    result = strlen(file);
    if (file[result-1] == 'a') {
	return load_lib(file);
    }

    fd = open(file, O_RDONLY);
    if (fd == -1) {
	dln_errno = errno;
	return -1;
    }
    result = load_1(fd, 0, file);
    close(fd);

    return result;
}

void*
dln_sym(const char *name)
{
    struct nlist *sym;

    if (st_lookup(sym_tbl, name, &sym))
	return (void*)sym->n_value;
    return NULL;
}

#endif /* USE_DLN_A_OUT */

#ifdef USE_DLN_DLOPEN
# include <dlfcn.h>
#endif

#ifdef __hpux
#include <errno.h>
#include "dl.h"
#endif

#if defined(_AIX)
#include <ctype.h>	/* for isdigit()	*/
#include <errno.h>	/* for global errno	*/
#include <sys/ldr.h>
#endif

#ifdef NeXT
#if NS_TARGET_MAJOR < 4
#include <mach-o/rld.h>
#else
#include <mach-o/dyld.h>
#ifndef NSLINKMODULE_OPTION_BINDNOW
#define NSLINKMODULE_OPTION_BINDNOW 1
#endif
#endif
#else
#ifdef MACOSX_DYLD
#include <mach-o/dyld.h>
#endif
#endif

#if defined _WIN32 && !defined __CYGWIN__
#include <windows.h>
#include <imagehlp.h>
#endif

#if defined _WIN32 && !defined __CYGWIN__
static const char *
dln_strerror(char *message, size_t size)
{
    int error = GetLastError();
    char *p = message;
    size_t len = snprintf(message, size, "%d: ", error);

#define format_message(sublang) FormatMessage(\
	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,	\
	NULL, error, MAKELANGID(LANG_NEUTRAL, (sublang)),		\
	message + len, size - len, NULL)
    if (format_message(SUBLANG_ENGLISH_US) == 0)
	format_message(SUBLANG_DEFAULT);
    for (p = message + len; *p; p++) {
	if (*p == '\n' || *p == '\r')
	    *p = ' ';
    }
    return message;
}
#define dln_strerror() dln_strerror(message, sizeof message)
#elif ! defined _AIX
static const char *
dln_strerror(void)
{
#ifdef USE_DLN_A_OUT
    char *strerror();

    switch (dln_errno) {
      case DLN_ECONFL:
	return "Symbol name conflict";
      case DLN_ENOINIT:
	return "No initializer given";
      case DLN_EUNDEF:
	return "Unresolved symbols";
      case DLN_ENOTLIB:
	return "Not a library file";
      case DLN_EBADLIB:
	return "Malformed library file";
      case DLN_EINIT:
	return "Not initialized";
      default:
	return strerror(dln_errno);
    }
#endif

#ifdef USE_DLN_DLOPEN
    return (char*)dlerror();
#endif
}
#endif

#if defined(_AIX) && ! defined(_IA64)
static void
aix_loaderror(const char *pathname)
{
  char *message[1024], errbuf[1024];
  int i;
#define ERRBUF_APPEND(s) strncat(errbuf, (s), sizeof(errbuf)-strlen(errbuf)-1)
  snprintf(errbuf, sizeof(errbuf), "load failed - %s. ", pathname);

  if (loadquery(L_GETMESSAGES, &message[0], sizeof(message)) != -1) {
    ERRBUF_APPEND("Please issue below command for detailed reasons:\n\t");
    ERRBUF_APPEND("/usr/sbin/execerror ruby ");
    for (i=0; message[i]; i++) {
      ERRBUF_APPEND("\"");
      ERRBUF_APPEND(message[i]);
      ERRBUF_APPEND("\" ");
    }
    ERRBUF_APPEND("\n");
  } else {
    ERRBUF_APPEND(strerror(errno));
    ERRBUF_APPEND("[loadquery failed]");
  }
  dln_loaderror("%s", errbuf);
}
#endif

#if defined _WIN32 && defined RUBY_EXPORT
HANDLE rb_libruby_handle(void);

static int
rb_w32_check_imported(HMODULE ext, HMODULE mine)
{
    ULONG size;
    const IMAGE_IMPORT_DESCRIPTOR *desc;

    desc = ImageDirectoryEntryToData(ext, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);
    if (!desc) return 0;
    while (desc->Name) {
	PIMAGE_THUNK_DATA pint = (PIMAGE_THUNK_DATA)((char *)ext + desc->Characteristics);
	PIMAGE_THUNK_DATA piat = (PIMAGE_THUNK_DATA)((char *)ext + desc->FirstThunk);
	while (piat->u1.Function) {
	    PIMAGE_IMPORT_BY_NAME pii = (PIMAGE_IMPORT_BY_NAME)((char *)ext + (size_t)pint->u1.AddressOfData);
	    static const char prefix[] = "rb_";
	    const char *name = (const char *)pii->Name;
	    if (strncmp(name, prefix, sizeof(prefix) - 1) == 0) {
		FARPROC addr = GetProcAddress(mine, name);
		if (addr) return (FARPROC)piat->u1.Function == addr;
	    }
	    piat++;
	    pint++;
	}
	desc++;
    }
    return 1;
}
#endif

#if defined(DLN_NEEDS_ALT_SEPARATOR) && DLN_NEEDS_ALT_SEPARATOR
#define translit_separator(src) do { \
	char *tmp = ALLOCA_N(char, strlen(src) + 1), *p = tmp, c; \
	do { \
	    *p++ = ((c = *file++) == '/') ? DLN_NEEDS_ALT_SEPARATOR : c; \
	} while (c); \
	(src) = tmp; \
    } while (0)
#else
#define translit_separator(str) (void)(str)
#endif

void*
dln_load(const char *file)
{
#if !defined(_AIX) && !defined(NeXT)
    const char *error = 0;
#define DLN_ERROR() (error = dln_strerror(), strcpy(ALLOCA_N(char, strlen(error) + 1), error))
#endif

#if defined _WIN32 && !defined __CYGWIN__
    HINSTANCE handle;
    char winfile[MAXPATHLEN];
    char message[1024];
    void (*init_fct)();
    char *buf;

    if (strlen(file) >= MAXPATHLEN) dln_loaderror("filename too long");

    /* Load the file as an object one */
    init_funcname(&buf, file);

    strlcpy(winfile, file, sizeof(winfile));

    /* Load file */
    if ((handle = LoadLibrary(winfile)) == NULL) {
	error = dln_strerror();
	goto failed;
    }

#if defined _WIN32 && defined RUBY_EXPORT
    if (!rb_w32_check_imported(handle, rb_libruby_handle())) {
	FreeLibrary(handle);
	error = "incompatible library version";
	goto failed;
    }
#endif

    if ((init_fct = (void(*)())GetProcAddress(handle, buf)) == NULL) {
	dln_loaderror("%s - %s\n%s", dln_strerror(), buf, file);
    }

    /* Call the init code */
    (*init_fct)();
    return handle;
#else
#ifdef USE_DLN_A_OUT
    if (load(file) == -1) {
	error = dln_strerror();
	goto failed;
    }
    return 0;
#else

    char *buf;
    /* Load the file as an object one */
    init_funcname(&buf, file);
    translit_separator(file);

#ifdef USE_DLN_DLOPEN
#define DLN_DEFINED
    {
	void *handle;
	void (*init_fct)();

#ifndef RTLD_LAZY
# define RTLD_LAZY 1
#endif
#ifdef __INTERIX
# undef RTLD_GLOBAL
#endif
#ifndef RTLD_GLOBAL
# define RTLD_GLOBAL 0
#endif

	/* Load file */
	if ((handle = (void*)dlopen(file, RTLD_LAZY|RTLD_GLOBAL)) == NULL) {
	    error = dln_strerror();
	    goto failed;
	}

	init_fct = (void(*)())(VALUE)dlsym(handle, buf);
#if defined __SYMBIAN32__
	if (init_fct == NULL) {
	    init_fct = (void(*)())dlsym(handle, "1"); /* Some Symbian versions do not support symbol table in DLL, ordinal numbers only */
	}
#endif
	if (init_fct == NULL) {
	    error = DLN_ERROR();
	    dlclose(handle);
	    goto failed;
	}
	/* Call the init code */
	(*init_fct)();

	return handle;
    }
#endif /* USE_DLN_DLOPEN */

#ifdef __hpux
#define DLN_DEFINED
    {
	shl_t lib = NULL;
	int flags;
	void (*init_fct)();

	flags = BIND_DEFERRED;
	lib = shl_load(file, flags, 0);
	if (lib == NULL) {
	    extern int errno;
	    dln_loaderror("%s - %s", strerror(errno), file);
	}
	shl_findsym(&lib, buf, TYPE_PROCEDURE, (void*)&init_fct);
	if (init_fct == NULL) {
	    shl_findsym(&lib, buf, TYPE_UNDEFINED, (void*)&init_fct);
	    if (init_fct == NULL) {
		errno = ENOSYM;
		dln_loaderror("%s - %s", strerror(ENOSYM), file);
	    }
	}
	(*init_fct)();
	return (void*)lib;
    }
#endif /* hpux */

#if defined(_AIX) && ! defined(_IA64)
#define DLN_DEFINED
    {
	void (*init_fct)();

	init_fct = (void(*)())load((char*)file, 1, 0);
	if (init_fct == NULL) {
	    aix_loaderror(file);
	}
	if (loadbind(0, (void*)dln_load, (void*)init_fct) == -1) {
	    aix_loaderror(file);
	}
	(*init_fct)();
	return (void*)init_fct;
    }
#endif /* _AIX */

#if defined(MACOSX_DYLD)
#define DLN_DEFINED
/*----------------------------------------------------
   By SHIROYAMA Takayuki Psi@fortune.nest.or.jp

   Special Thanks...
    Yu tomoak-i@is.aist-nara.ac.jp,
    Mi hisho@tasihara.nest.or.jp,
    sunshine@sunshineco.com,
    and... Miss ARAI Akino(^^;)
 ----------------------------------------------------*/
    {
	int dyld_result;
	NSObjectFileImage obj_file; /* handle, but not use it */
	/* "file" is module file name .
	   "buf" is pointer to initial function name with "_" . */

	void (*init_fct)();


	dyld_result = NSCreateObjectFileImageFromFile(file, &obj_file);

	if (dyld_result != NSObjectFileImageSuccess) {
	    dln_loaderror("Failed to load %.200s", file);
	}

	NSLinkModule(obj_file, file, NSLINKMODULE_OPTION_BINDNOW);

	/* lookup the initial function */
	if(!NSIsSymbolNameDefined(buf)) {
	    dln_loaderror("Failed to lookup Init function %.200s",file);
	}
	init_fct = NSAddressOfSymbol(NSLookupAndBindSymbol(buf));
	(*init_fct)();

	return (void*)init_fct;
    }
#endif

#if defined(__BEOS__) || defined(__HAIKU__)
# define DLN_DEFINED
    {
      status_t err_stat;  /* BeOS error status code */
      image_id img_id;    /* extension module unique id */
      void (*init_fct)(); /* initialize function for extension module */

      /* load extension module */
      img_id = load_add_on(file);
      if (img_id <= 0) {
	dln_loaderror("Failed to load add_on %.200s error_code=%x",
	  file, img_id);
      }

      /* find symbol for module initialize function. */
      /* The Be Book KernelKit Images section described to use
	 B_SYMBOL_TYPE_TEXT for symbol of function, not
	 B_SYMBOL_TYPE_CODE. Why ? */
      /* strcat(init_fct_symname, "__Fv"); */  /* parameter nothing. */
      /* "__Fv" dont need! The Be Book Bug ? */
      err_stat = get_image_symbol(img_id, buf,
				  B_SYMBOL_TYPE_TEXT, (void **)&init_fct);

      if (err_stat != B_NO_ERROR) {
	char real_name[MAXPATHLEN];

	strlcpy(real_name, buf, MAXPATHLEN);
	strlcat(real_name, "__Fv", MAXPATHLEN);
        err_stat = get_image_symbol(img_id, real_name,
				    B_SYMBOL_TYPE_TEXT, (void **)&init_fct);
      }

      if ((B_BAD_IMAGE_ID == err_stat) || (B_BAD_INDEX == err_stat)) {
	unload_add_on(img_id);
	dln_loaderror("Failed to lookup Init function %.200s", file);
      }
      else if (B_NO_ERROR != err_stat) {
	char errmsg[] = "Internal of BeOS version. %.200s (symbol_name = %s)";
	unload_add_on(img_id);
	dln_loaderror(errmsg, strerror(err_stat), buf);
      }

      /* call module initialize function. */
      (*init_fct)();
      return (void*)img_id;
    }
#endif /* __BEOS__ || __HAIKU__ */

#ifndef DLN_DEFINED
    dln_notimplement();
#endif

#endif /* USE_DLN_A_OUT */
#endif
#if !defined(_AIX) && !defined(NeXT)
  failed:
    dln_loaderror("%s - %s", error, file);
#endif

    return 0;			/* dummy return */
}
