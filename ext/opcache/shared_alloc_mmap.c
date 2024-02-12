/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

#ifdef __APPLE__
#include <mach/vm_statistics.h>
#endif

#include "zend_execute.h"
#ifdef HAVE_SYS_PROCCTL_H
#include <sys/procctl.h>
#endif

#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
# define MAP_ANONYMOUS MAP_ANON
#endif
#if defined(MAP_ALIGNED_SUPER)
# include <sys/types.h>
# include <sys/sysctl.h>
# include <sys/user.h>
# define MAP_HUGETLB MAP_ALIGNED_SUPER
#endif

#if (defined(__linux__) || defined(__FreeBSD__)) && (defined(__x86_64__) || defined (__aarch64__)) && !defined(__SANITIZE_ADDRESS__)
static void *find_prefered_mmap_base(size_t requested_size)
{
	size_t huge_page_size = 2 * 1024 * 1024;
	uintptr_t last_free_addr = 0;
	uintptr_t last_candidate = (uintptr_t)MAP_FAILED;
	uintptr_t start, end, text_start = 0;
#if defined(__linux__)
	FILE *f;
	char buffer[MAXPATHLEN];

	f = fopen("/proc/self/maps", "r");
	if (!f) {
		return MAP_FAILED;
	}

	while (fgets(buffer, MAXPATHLEN, f) && sscanf(buffer, "%lx-%lx", &start, &end) == 2) {
		/* Don't place the segment directly before or after the heap segment. Due to an selinux bug,
		 * a segment directly preceding or following the heap is interpreted as heap memory, which
		 * will result in an execheap violation for the JIT.
		 * See https://bugzilla.kernel.org/show_bug.cgi?id=218258. */
		bool heap_segment = strstr(buffer, "[heap]") != NULL;
		if (heap_segment) {
			uintptr_t start_base = start & ~(huge_page_size - 1);
			if (last_free_addr + requested_size >= start_base) {
				last_free_addr = ZEND_MM_ALIGNED_SIZE_EX(end + huge_page_size, huge_page_size);
				continue;
			}
		}
		if ((uintptr_t)execute_ex >= start) {
			/* the current segment lays before PHP .text segment or PHP .text segment itself */
			/*Search for candidates at the end of the free segment near the .text segment
			  to prevent candidates from being missed due to large hole*/
			if (last_free_addr + requested_size <= start) {
				last_candidate = ZEND_MM_ALIGNED_SIZE_EX(start - requested_size, huge_page_size);
				if (last_candidate + requested_size > start) {
					last_candidate -= huge_page_size;
				}
			}
			if ((uintptr_t)execute_ex < end) {
				/* the current segment is PHP .text segment itself */
				if (last_candidate != (uintptr_t)MAP_FAILED) {
					if (end - last_candidate < UINT32_MAX) {
						/* we have found a big enough hole before the text segment */
						break;
					}
					last_candidate = (uintptr_t)MAP_FAILED;
				}
				text_start = start;
			}
		} else {
			/* the current segment lays after PHP .text segment */
			if (last_free_addr + requested_size - text_start > UINT32_MAX) {
				/* the current segment and the following segments lay too far from PHP .text segment */
				break;
			}
			if (last_free_addr + requested_size <= start) {
				last_candidate = last_free_addr;
				break;
			}
		}
		last_free_addr = ZEND_MM_ALIGNED_SIZE_EX(end, huge_page_size);
		if (heap_segment) {
			last_free_addr += huge_page_size;
		}
	}
	fclose(f);
#elif defined(__FreeBSD__)
	size_t s = 0;
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_VMMAP, getpid()};
	if (sysctl(mib, 4, NULL, &s, NULL, 0) == 0) {
		s = s * 4 / 3;
		void *addr = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (addr != MAP_FAILED) {
			if (sysctl(mib, 4, addr, &s, NULL, 0) == 0) {
				start = (uintptr_t)addr;
				end = start + s;
				while (start < end) {
					struct kinfo_vmentry *entry = (struct kinfo_vmentry *)start;
					size_t sz = entry->kve_structsize;
					if (sz == 0) {
						break;
					}
					uintptr_t e_start = entry->kve_start;
					uintptr_t e_end = entry->kve_end;
					if ((uintptr_t)execute_ex >= e_start) {
						/* the current segment lays before PHP .text segment or PHP .text segment itself */
						if (last_free_addr + requested_size <= e_start) {
							last_candidate = ZEND_MM_ALIGNED_SIZE_EX(e_start - requested_size, huge_page_size);
							if (last_candidate + requested_size > e_start) {
								last_candidate -= huge_page_size;
							}
						}
						if ((uintptr_t)execute_ex < e_end) {
							/* the current segment is PHP .text segment itself */
							if (last_candidate != (uintptr_t)MAP_FAILED) {
								if (e_end - last_candidate < UINT32_MAX) {
									/* we have found a big enough hole before the text segment */
									break;
								}
								last_candidate = (uintptr_t)MAP_FAILED;
							}
							text_start = e_start;
						}
					} else {
						/* the current segment lays after PHP .text segment */
						if (last_free_addr + requested_size - text_start > UINT32_MAX) {
							/* the current segment and the following segments lay too far from PHP .text segment */
							break;
						}
						if (last_free_addr + requested_size <= e_start) {
							last_candidate = last_free_addr;
							break;
						}
					}
					last_free_addr = ZEND_MM_ALIGNED_SIZE_EX(e_end, huge_page_size);
					start += sz;
				}
			}
			munmap(addr, s);
		}
	}
#endif

	return (void*)last_candidate;
}
#endif

static int create_segments(size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, const char **error_in)
{
	zend_shared_segment *shared_segment;
	int flags = PROT_READ | PROT_WRITE, fd = -1;
	int mmap_flags = MAP_SHARED | MAP_ANONYMOUS;
	void *p;
#if defined(HAVE_PROCCTL) && defined(PROC_WXMAP_CTL)
	int enable_wxmap = PROC_WX_MAPPINGS_PERMIT;
	if (procctl(P_PID, getpid(), PROC_WXMAP_CTL, &enable_wxmap) == -1) {
		return ALLOC_FAILURE;
	}
#endif
#ifdef PROT_MPROTECT
	flags |= PROT_MPROTECT(PROT_EXEC);
#endif
#ifdef VM_MAKE_TAG
	/* allows tracking segments via tools such as vmmap */
	fd = VM_MAKE_TAG(251U);
#endif
#ifdef PROT_MAX
	flags |= PROT_MAX(PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
#ifdef MAP_JIT
	mmap_flags |= MAP_JIT;
#endif
#if (defined(__linux__) || defined(__FreeBSD__)) && (defined(__x86_64__) || defined (__aarch64__)) && !defined(__SANITIZE_ADDRESS__)
	void *hint = find_prefered_mmap_base(requested_size);
	if (hint != MAP_FAILED) {
# ifdef MAP_HUGETLB
		size_t huge_page_size = 2 * 1024 * 1024;
		if (requested_size >= huge_page_size && requested_size % huge_page_size == 0) {
			p = mmap(hint, requested_size, flags, mmap_flags|MAP_HUGETLB|MAP_FIXED, -1, 0);
			if (p != MAP_FAILED) {
				goto success;
			}
		}
#endif
		p = mmap(hint, requested_size, flags, mmap_flags|MAP_FIXED, -1, 0);
		if (p != MAP_FAILED) {
			goto success;
		}
	}
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

		p = mmap(NULL, requested_size, flags, mmap_flags|MAP_32BIT, fd, 0);
		if (p != MAP_FAILED) {
			munmap(p, requested_size);
			p = (void*)(ZEND_MM_ALIGNED_SIZE_EX((ptrdiff_t)p, huge_page_size));
			p = mmap(p, requested_size, flags, mmap_flags|MAP_32BIT|MAP_HUGETLB|MAP_FIXED, -1, 0);
			if (p != MAP_FAILED) {
				goto success;
			} else {
				p = mmap(NULL, requested_size, flags, mmap_flags|MAP_32BIT, fd, 0);
				if (p != MAP_FAILED) {
					goto success;
				}
			}
		}
# endif
		p = mmap(0, requested_size, flags, mmap_flags|MAP_HUGETLB, fd, 0);
		if (p != MAP_FAILED) {
			goto success;
		}
	}
#elif defined(PREFER_MAP_32BIT) && defined(__x86_64__) && defined(MAP_32BIT)
	p = mmap(NULL, requested_size, flags, mmap_flags|MAP_32BIT, fd, 0);
	if (p != MAP_FAILED) {
		goto success;
	}
#endif

	p = mmap(0, requested_size, flags, mmap_flags, fd, 0);
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

const zend_shared_memory_handlers zend_alloc_mmap_handlers = {
	create_segments,
	detach_segment,
	segment_type_size
};

#endif /* USE_MMAP */
