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
   |          Xinchen Hui <xinchen.h@zend.com>                            |
   +----------------------------------------------------------------------+
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "zend_API.h"
#include "zend_elf.h"

static zend_array *symbols_table = NULL;

static zend_elf_header *zend_elf_read_elfhdr(int fd, void *buf) {
	if (read(fd, buf, sizeof(zend_elf_header)) == sizeof(zend_elf_header)) {
		return (zend_elf_header*)buf;
	}
	return NULL;
}

static int zend_elf_parse_sym_entry(zend_array *sym_table, char *str_tbl, zend_elf_symbol *sym) {
	if (sym->name) {
		zval zv;
		zend_string *str = zend_string_init(str_tbl + sym->name, strlen(str_tbl + sym->name), 1);

		ZVAL_STR(&zv, str);
		zend_hash_index_update(sym_table, sym->value, &zv);
	}
	return 1;
}

static void* zend_elf_read_sect(int fd, zend_elf_sectheader *sect) {
	void *s = emalloc(sect->size);

	lseek(fd, sect->ofs, SEEK_SET);
	if (read(fd, s, sect->size) != sect->size) {
		efree(s);
		return NULL;
	}

	return s;
}

static zend_elf_sectheader* zend_elf_load_sects(int fd, zend_elf_header *hdr) {
	uint32_t i;
	zend_elf_sectheader *sects = (zend_elf_sectheader *)emalloc(hdr->shentsize * hdr->shnum);

	lseek(fd, hdr->shofs, SEEK_SET);
	for (i = 0; i < hdr->shnum; i++) {
		read(fd, &sects[i], hdr->shentsize);
	}

	return sects;
}

static zend_array* zend_elf_load_symbols(int fd, zend_elf_header *hdr) {
	uint32_t i;
	zend_array *symbols;
	zend_elf_sectheader *sects;

	symbols = malloc(sizeof(zend_array));
	zend_hash_init(symbols, 8, NULL, ZVAL_PTR_DTOR, 1);

	sects = zend_elf_load_sects(fd, hdr);
	for (i = 0; i < hdr->shnum; i++) {
		zend_elf_symbol *syms;
		if (sects[i].type != ELFSECT_TYPE_SYMTAB) {
			continue;
		}
		if ((syms = (zend_elf_symbol*)zend_elf_read_sect(fd, &sects[i]))) {
			uint32_t n, count = (sects[i].size) / sizeof(zend_elf_symbol);
			char *str_tbl = (char*)zend_elf_read_sect(fd, &sects[sects[i].link]);
			ZEND_ASSERT(sects[i].entsize == sizeof(zend_elf_symbol));
			for (n = 0; n < count; n++) {
				if ((syms[n].info & ELFSYM_TYPE_FUNC) &&
					!(syms[n].info & ELFSYM_BIND_GLOBAL)) {
					zend_elf_parse_sym_entry(symbols, str_tbl, &syms[n]);
				}
		   	}
			efree(str_tbl);
			efree(syms);
		}
	}

	efree(sects);

	return symbols;
}

void zend_elf_init(void) {
	int fd = open("/proc/self/exe", O_RDONLY);

	if (fd) {
		char buf[sizeof(zend_elf_header)];
		zend_elf_header *elfhdr = zend_elf_read_elfhdr(fd, buf);

		if (elfhdr) {
			ZEND_ASSERT(strncmp((char*)elfhdr->emagic, "\177ELF", 4)  == 0);
			symbols_table = zend_elf_load_symbols(fd, elfhdr);
		}
		close(fd);
	}
}

void zend_elf_shutdown(void) {
	zend_array_destroy(symbols_table);
}

const char* zend_elf_resolve_sym(void *addr) {
	zval *zv;
	if (!symbols_table) {
		return NULL;
	}
	zv = zend_hash_index_find(symbols_table, (size_t)addr);
	return zv ? Z_STRVAL_P(zv) : NULL;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
