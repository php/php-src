/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_shared_alloc.h"

#ifdef USE_MMAP

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
# define MAP_ANONYMOUS MAP_ANON
#endif
#if defined(MAP_ALIGNED_SUPER)
# define MAP_HUGETLB MAP_ALIGNED_SUPER
#endif

static int create_segments(size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	zend_shared_segment *shared_segment;
  int flags = PROT_READ | PROT_WRITE;
	void *p;
#ifdef PROT_MPROTECT
  flags |= PROT_MPROTECT(PROT_EXEC);
#endif
#ifdef PROT_MAX
  flags |= PROT_MAX(PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
#ifdef MAP_HUGETLB
	size_t huge_page_size = 2 * 1024 * 1024;

	/* Try to allocate huge pages first to reduce dTLB misses.
	 * OSes has to be configured properly
	 * on Linux
	 * (e.g. https://wiki.debian.org/Hugepages#Enabling_HugeTlbPage)
	 * You may verify huge page usage with the following command:
	 * `grep "Huge" /proc/meminfo`
	 * on FreeBSD
	 * sysctl vm.pmap.pg_ps_enabled entry
	 * (boot time config only, but enabled by default on most arches).
	 */
	if (requested_size >= huge_page_size && requested_size % huge_page_size == 0) {
# if defined(__x86_64__) && defined(MAP_32BIT)
		/* to got HUGE PAGES in low 32-bit address we have to reserve address
		   space and then remap it using MAP_HUGETLB */

		p = mmap(NULL, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT, -1, 0);
		if (p != MAP_FAILED) {
			munmap(p, requested_size);
			p = (void*)(ZEND_MM_ALIGNED_SIZE_EX((ptrdiff_t)p, huge_page_size));
			p = mmap(p, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT|MAP_HUGETLB|MAP_FIXED, -1, 0);
			if (p != MAP_FAILED) {
				goto success;
			} else {
				p = mmap(NULL, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT, -1, 0);
				if (p != MAP_FAILED) {
					goto success;
				}
			}
		}
# endif
		p = mmap(0, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
		if (p != MAP_FAILED) {
			goto success;
		}
	}
#elif defined(PREFER_MAP_32BIT) && defined(__x86_64__) && defined(MAP_32BIT)
	p = mmap(NULL, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT, -1, 0);
	if (p != MAP_FAILED) {
		goto success;
	}
#endif

	p = mmap(0, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (p == MAP_FAILED) {
		*error_in = "mmap";
		return ALLOC_FAILURE;
	}

success: ZEND_ATTRIBUTE_UNUSED;
	*shared_segments_count = 1;
	*shared_segments_p = (zend_shared_segment **) calloc(1, sizeof(zend_shared_segment) + sizeof(void *));
	if (!*shared_segments_p) {
		munmap(p, requested_size);
		*error_in = "calloc";
		return ALLOC_FAILURE;
	}
	shared_segment = (zend_shared_segment *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segment;

	shared_segment->p = p;
	shared_segment->pos = 0;
	shared_segment->size = requested_size;

	return ALLOC_SUCCESS;
}

static int detach_segment(zend_shared_segment *shared_segment)
{
	munmap(shared_segment->p, shared_segment->size);
	return 0;
}

static size_t segment_type_size(void)
{
	return sizeof(zend_shared_segment);
}

zend_shared_memory_handlers zend_alloc_mmap_handlers = {
	create_segments,
	detach_segment,
	segment_type_size
};

#endif /* USE_MMAP */
