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

#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
# define MAP_ANONYMOUS MAP_ANON
#endif
#if defined(MAP_ALIGNED_SUPER)
# define MAP_HUGETLB MAP_ALIGNED_SUPER
#endif

#if defined(__x86_64__) && defined(__linux__)
#define max(a,b) ((a) > (b) ? (a):(b))

#if defined(MAP_HUGETLB)
static const size_t huge_page_size = 2 * 1024 * 1024; /* 2MB page size */
#else
static const size_t ordinary_page_size = 4 * 1024;    /* 4KB page size */
#endif /* MAP_HUGETLB */

static const ptrdiff_t addr_4GB_distance = UINT32_MAX; /* 4GB address distance */
static const uintptr_t addr_4GB = UINT32_MAX;          /* 4GB memory address */
static const size_t    size_4GB = UINT32_MAX;          /* 4GB memory size */

/* Store the segment address information */
struct map_seg_addresses {
    uintptr_t start; /* segment start address (included) */
    uintptr_t end;   /* segment end address (excluded) */
};

/* Count segments in /proc/self/maps file; each line represents one segment. */
static size_t map_segment_count(void)
{
    FILE *f = NULL;
    f = fopen("/proc/self/maps", "r");
    if (!f) return 0;

    size_t count = 0; /* segment count */
    char buffer[MAXPATHLEN];
    while ((fgets(buffer, MAXPATHLEN, f)) != NULL) {
        count++;
    }

    if (!feof(f)) { // fgets error
        count = 0;
    }

    fclose(f);
    f = NULL;
    return count;
}

/* Parse start/end address of all segments from maps file and
   locate the php .text segment.
   Return TRUE if succeed; FALSE if fail to parse maps file.
*/
static bool parse_map_file(struct map_seg_addresses* map_segments,
                           size_t map_segments_count,
                           struct map_seg_addresses* php_text_segment)
{
    char buffer[MAXPATHLEN];
    FILE *f = NULL;
    f = fopen("/proc/self/maps", "r");
    if (!f) return false;

    size_t index = 0;
    uintptr_t start; /* segment start address */
    uintptr_t end;   /* segment end address */
    bool ret = true; /* function return value */

    /* Get PHP .text segment lighthouse address used
       to locate PHP .text segment */
    uintptr_t php_lighthouse_addr = (uintptr_t) php_text_lighthouse();

    /* Store each line in tha map_segments array */
    while (fgets(buffer, MAXPATHLEN, f)) {
        /* get first two fields: start-end address */
        if (sscanf(buffer, "%lx-%lx", &start, &end) == 2 &&
            index < map_segments_count)
        {
            map_segments[index].start = start;
            map_segments[index].end = end;
            index++;
        } else { /* fle error, or index overflow (maps file changed) */
            ret = false;
            break;
        }

        /* check if it is PHP .text segment */
        if (start <= php_lighthouse_addr && php_lighthouse_addr < end) {
            php_text_segment->start = start;
            php_text_segment->end = end;
        }
    }

    if (!feof(f)) {
        ret = false;
    }

    fclose(f);
    f = NULL;
    return ret;
}

/* Sort all segments in ascendng order by start address
   using insert sort algorithm.
   We do not have too many segments, so I don't care efficiency or performance.
*/
static void sort_segments(struct map_seg_addresses* segments, size_t seg_count)
{
    struct map_seg_addresses temp;

    for (size_t i = 1; i < seg_count; i++) {
        for (size_t j = i;
             (j > 0) && (segments[j-1].start > segments[j].start);
             j--)
        {
            /* save smaller-address segment to temp */
            temp.start = segments[j].start;
            temp.end = segments[j].end;

            /* move bigger-address segment to bigger index */
            segments[j].start = segments[j-1].start;
            segments[j].end = segments[j-1].end;

            /* move smaller-address segment to smaller index */
            segments[j-1].start = temp.start;
            segments[j-1].end = temp.end;
        }
    }
}

/* Combine adjacent/overlapped segments into one bigger segment,
   and the result is placed in res array and its count is returned.
*/
static size_t combine_segments(struct map_seg_addresses* segments,
                               size_t seg_count,
                               struct map_seg_addresses* res,
                               size_t max_res_count) {
    size_t res_count = 0;

    for (size_t i = 0; i < seg_count;) {
        res[res_count].start = segments[i].start;

        /* temp used to remember the biggest end address of adjacent segments. */
        uintptr_t temp = segments[i].end;
        size_t j = i + 1; /* next segment to check */

        /* As long as segments[j] is adjacent or overlapped, we update temp value */
        while (j < seg_count && segments[j].start <= temp) {
            temp = max(temp, segments[j].end);
            j++; /* check next segment */
        }

        if (res_count < max_res_count) {
            res[res_count].end = temp;
            res_count++;
        } else { /* error: index overflow */
            return 0;
        }

        i = j; /* jump to the next unchecked segment */
    }

    return res_count;
}

/* Search all candidate buffer addresses in order for opcache/jit buffer.
   All available addresses are stored in candidates array; candidate count
   is returned.
*/
static size_t search_candidates(struct map_seg_addresses* combined_segments,
                    size_t seg_count,
                    size_t requested_size,
                    struct map_seg_addresses* php_text_segment,
                    uintptr_t* candidates,
                    size_t max_candidates_count)
{
    uintptr_t prev_seg_end = 0;   /* previous segment end address */
    uintptr_t candidate = 0;      /* candidate address */
    size_t candidate_count = 0;

#if defined(MAP_HUGETLB)
    size_t reserved_align_space = huge_page_size;
#else
    size_t reserved_align_space = ordinary_page_size;
#endif /* MAP_HUGETLB */

    /* No need to move buffer closer to PHP .text segment, if
       1) requested buffer size is equal to or greater than 4GB,
       2) PHP .text segment is under 4GB memory address
    */
    if (requested_size >= size_4GB || php_text_segment->start < addr_4GB) {
        return 0;
    }

    /*  PHP segments diagram

               [seg0]     [seg1]      [php .text][heap]      [seg3]
        [hole0]     [hole1]     [hole2]                [hole3]

        We only search any candidates BEFORE PHP .text segment. E.g., [hole0],
        [hole1], and [hole2] might be good candidates. Here is why:
        for standalone PHP, such as PHP command, we find that [heap] is
        closely following PHP .text segment and our buffer might block heap
        growth if we use [hole3]. Although for module PHP, such as libphp.so
        loaded into webserver, [heap] is usually before libphp [.text] segment,
        our search algorithm still works.
    */

    /* go through all segments for available buffer candidates address */
    for (int i = 0; i < seg_count; i++) {
        struct map_seg_addresses* seg = combined_segments + i;

        /* check if there is enough space for buffer in the hole before this segment */
        size_t hole_size = seg->start - prev_seg_end;

        /* requested_size is surely less than 4GB, so no overflow */
        if (hole_size >= requested_size + reserved_align_space) {
           /* ensure (candidate) address is before PHP .text segment */
           if (seg->start <= php_text_segment->start) {
                /* calculate and align buffer start address */
                candidate = seg->start - reserved_align_space - requested_size;
                candidate = ZEND_MM_ALIGNED_SIZE_EX(candidate, reserved_align_space);

                /* check if buffer is close enough to PHP .text (with 4GB chunk) */
                if (php_text_segment->end - candidate <= addr_4GB_distance) {
                    if (candidate_count < max_candidates_count) {
                        candidates[candidate_count] = candidate;
                        candidate_count++;
                    }
                }
            }

            /* This code part is disabled before so far we do NOT search any
               candidate holes after PHP .text segment in case blocking
               heap growth. */
            /*
            if (combined_segments[i].start > php_text_segment->end) {
                candidate = last_end;
                candidate = ZEND_MM_ALIGNED_SIZE_EX(candidate, reserved_align_space);
                if (candidate + requested_size + reserved_align_space - php_text_segment->start < addr_4GB_distance) {
                    candidates[candidate_count] = candidate;
                    candidate_count++;
                }
            }
            */
        }

        /* update in order to calculate next hole size */
        prev_seg_end = seg->end;
    }

    return candidate_count;
}

/*
	This function tries to create segment starting from address close to
    PHP .text segment (with 4GB chunk) and returns the newly-allocated address,
	or MAP_FAILED for any failure.

    WARNING:
    DO NOT return NULL for failure, NULL is not the same thing as MAP_FAILED.

  	If huge page is supported and requested_size meets conditions, we will first
	try to get memory from huge pages; if failed, fall back to try 4KB pages.
*/
static void* create_preferred_segments(size_t requested_size)
{
    /* If requested_size is larger than 4GB, do not move opcache and jit buffer */
    if (requested_size >= size_4GB ||
        requested_size < huge_page_size ||
        requested_size % huge_page_size != 0)
    {
        return MAP_FAILED;
    }

    /* We first count the segments in maps file and then use an array in stack
       to store these segments and do further actions. Using malloc() to
       allocate memory might change maps file, so here we have to use array.
    */

    /* Get total segments count */
    int segment_count = 0;
    segment_count = map_segment_count();
    if (segment_count == 0) return MAP_FAILED;

    /* Store segments in array and find the php text segment */
    struct map_seg_addresses map_segments[segment_count];
    struct map_seg_addresses php_text_segment;

    if (false == parse_map_file(map_segments, segment_count, &php_text_segment)) {
        return MAP_FAILED;
    }

    /* Sort segments by the start address */
    sort_segments(map_segments, segment_count);

    /* Combine adjacent/overlapped segments */
    int combined_segments_count = 0;
    struct map_seg_addresses combined_segments[segment_count];
    combined_segments_count = combine_segments(map_segments, segment_count,
                                               combined_segments, segment_count);
    if (0 == combined_segments_count) return MAP_FAILED;

    /* Search unallocated address space to get the preferred mmap start addresses*/
    int candidate_count = 0;
    size_t max_candidate_count = combined_segments_count + 1;
    uintptr_t candidates[max_candidate_count];
    candidate_count = search_candidates(combined_segments,
                              combined_segments_count,
                              requested_size,
                              &php_text_segment,
                              candidates,
                              sizeof(candidates)/sizeof(uintptr_t));
    if (candidate_count == 0) return MAP_FAILED;

    /* Create segments by trying all candidate addresses
       and return immediately if succeed. */
    void *res;
    int flags = PROT_READ | PROT_WRITE, fd = -1;

    /* Try to get memory from huge pages */
#if defined(MAP_HUGETLB)
    for(int i = 0; i < candidate_count; i++) {
		res = mmap((void*)candidates[i], requested_size, flags,
                   MAP_SHARED|MAP_ANONYMOUS|MAP_HUGETLB|MAP_FIXED, fd, 0);
		if (MAP_FAILED != res) {
			return res;
		}
    }
#endif /* MAP_HUGETLB */

    /* Try 4KB pages, e.g., huge page allocation is failed or not supported */
    for(int i = 0; i < candidate_count; i++) {
        res = mmap((void*)candidates[i], requested_size, flags,
                   MAP_SHARED|MAP_ANONYMOUS|MAP_FIXED, fd, 0);
		if (MAP_FAILED != res) {
			return res;
		}
    }
    /* not able to do mmap, e.g.,
    * 1) fail to parse maps file
    * 2) not enough space for allocation, etc. */
    return MAP_FAILED;
}
#endif /* __x86_64__ && __linux__ */

/* In this function, we first try to allocate segment memory with a
   preferred start address which near PHP text segment.
   In this way, it can benefit 'near jump' efficiency between JIT buffer and
   other .text segments, and potentially offer PHP ~2% more performance both
   on 2MB huge pages and ordinary 4KB pages. So fare, we only support Linux.
   FIXME(tony): consider support *BSD in future.
*/
static int create_segments(size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	zend_shared_segment *shared_segment;
	int flags = PROT_READ | PROT_WRITE, fd = -1;
	void *p;
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

#if defined(__x86_64__) && defined(__linux__)
	/* On 64b Linux, try to allocate segment using preferred address, first try
       huge pages, then try 4KB pages; if both failed, fall back and continue
       the previous allocation logic.

       If fallback happens, We cannot do JIT buffer relocation and have no
       performance gain, but program will go on without failure.
    */
	p = create_preferred_segments(requested_size);
	if (p != MAP_FAILED) {
		goto success;
	}
#endif /* __x86_64__&& __linux__ */

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

		p = mmap(NULL, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT, fd, 0);
		if (p != MAP_FAILED) {
			munmap(p, requested_size);
			p = (void*)(ZEND_MM_ALIGNED_SIZE_EX((uintptr_t)p, huge_page_size));
			p = mmap(p, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT|MAP_HUGETLB|MAP_FIXED, -1, 0);
			if (p != MAP_FAILED) {
				goto success;
			} else {
				p = mmap(NULL, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT, fd, 0);
				if (p != MAP_FAILED) {
					goto success;
				}
			}
		}
# endif /* __x86_64__ && MAP_32BIT */
		p = mmap(0, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_HUGETLB, fd, 0);
		if (p != MAP_FAILED) {
			goto success;
		}
	}
#elif defined(PREFER_MAP_32BIT) && defined(__x86_64__) && defined(MAP_32BIT)
	p = mmap(NULL, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS|MAP_32BIT, fd, 0);
	if (p != MAP_FAILED) {
		goto success;
	}
#endif /* MAP_HUGETLB */

	p = mmap(0, requested_size, flags, MAP_SHARED|MAP_ANONYMOUS, fd, 0);
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
