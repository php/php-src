
	/* $Id: fpm_shm.c,v 1.3 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "fpm_shm.h"
#include "zlog.h"


/* MAP_ANON is deprecated, but not in macosx */
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

struct fpm_shm_s *fpm_shm_alloc(size_t sz) /* {{{ */
{
	struct fpm_shm_s *shm;

	shm = malloc(sizeof(*shm));

	if (!shm) {
		return 0;
	}

	shm->mem = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	if (!shm->mem) {
		zlog(ZLOG_STUFF, ZLOG_SYSERROR, "mmap(MAP_ANONYMOUS | MAP_SHARED) failed");
		free(shm);
		return 0;
	}

	shm->used = 0;
	shm->sz = sz;
	return shm;
}
/* }}} */

void fpm_shm_free(struct fpm_shm_s *shm, int do_unmap) /* {{{ */
{
	if (do_unmap) {
		munmap(shm->mem, shm->sz);
	}
	free(shm);	
}
/* }}} */

void fpm_shm_free_list(struct fpm_shm_s *shm, void *mem) /* {{{ */
{
	struct fpm_shm_s *next;

	for (; shm; shm = next) {
		next = shm->next;
		fpm_shm_free(shm, mem != shm->mem);
	}
}
/* }}} */

void *fpm_shm_alloc_chunk(struct fpm_shm_s **head, size_t sz, void **mem) /* {{{ */
{
	size_t pagesize = getpagesize();
	static const size_t cache_line_size = 16;
	size_t aligned_sz;
	struct fpm_shm_s *shm;
	void *ret;

	sz = (sz + cache_line_size - 1) & -cache_line_size;
	shm = *head;

	if (0 == shm || shm->sz - shm->used < sz) {
		/* allocate one more shm segment */

		aligned_sz = (sz + pagesize - 1) & -pagesize;
		shm = fpm_shm_alloc(aligned_sz);

		if (!shm) {
			return 0;
		}

		shm->next = *head;

		if (shm->next) {
			shm->next->prev = shm;
		}

		shm->prev = 0;
		*head = shm;
	}

	*mem = shm->mem;
	ret = (char *) shm->mem + shm->used;
	shm->used += sz;
	return ret;
}
/* }}} */

