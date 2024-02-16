/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Arnaud Le Blanc <arnaud.lb@gmail.com>                       |
   +----------------------------------------------------------------------+
*/

/* Inspired from Chromium's stack_util.cc */

#include "zend.h"
#include "zend_globals.h"
#include "zend_portability.h"
#include "zend_call_stack.h"
#include <stdint.h>
#ifdef ZEND_WIN32
# include <processthreadsapi.h>
# include <memoryapi.h>
#else /* ZEND_WIN32 */
# include <sys/resource.h>
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif
#endif /* ZEND_WIN32 */
#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) || \
    defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || \
    defined(__sun)
# include <pthread.h>
#endif
#if defined(__FreeBSD__) || defined(__DragonFly__)
# include <pthread_np.h>
# include <sys/mman.h>
# include <sys/sysctl.h>
# include <sys/user.h>
#endif
#ifdef __OpenBSD__
typedef int boolean_t;
# include <tib.h>
# include <pthread_np.h>
# include <sys/sysctl.h>
# include <sys/user.h>
#endif
#ifdef __NetBSD__
# include <sys/sysctl.h>
# include <sys/syscall.h>
#endif
#ifdef __HAIKU__
# include <kernel/OS.h>
#endif
#ifdef __linux__
#include <sys/syscall.h>
#endif
#ifdef __sun
#define _STRUCTURED_PROC 1
#include <sys/lwp.h>
#include <sys/procfs.h>
#include <libproc.h>
#include <thread.h>
#endif

#ifdef ZEND_CHECK_STACK_LIMIT

/* Called once per process or thread */
ZEND_API void zend_call_stack_init(void) {
	if (!zend_call_stack_get(&EG(call_stack))) {
		EG(call_stack) = (zend_call_stack){0};
	}

	switch (EG(max_allowed_stack_size)) {
		case ZEND_MAX_ALLOWED_STACK_SIZE_DETECT: {
			void *base = EG(call_stack).base;
			size_t size = EG(call_stack).max_size;
			if (UNEXPECTED(base == (void*)0)) {
				base = zend_call_stack_position();
				size = zend_call_stack_default_size();
				/* base is not the actual stack base */
				size -= 32 * 1024;
			}
			EG(stack_base) = base;
			EG(stack_limit) = zend_call_stack_limit(base, size, EG(reserved_stack_size));
			break;
		}
		case ZEND_MAX_ALLOWED_STACK_SIZE_UNCHECKED: {
			EG(stack_base) = (void*)0;
			EG(stack_limit) = (void*)0;
			break;
		}
		default: {
			ZEND_ASSERT(EG(max_allowed_stack_size) > 0);
			void *base = EG(call_stack).base;
			if (UNEXPECTED(base == (void*)0)) {
				base = zend_call_stack_position();
			}
			EG(stack_base) = base;
			EG(stack_limit) = zend_call_stack_limit(base, EG(max_allowed_stack_size), EG(reserved_stack_size));
			break;
		}
	}
}

#ifdef __linux__
static bool zend_call_stack_is_main_thread(void) {
# ifdef HAVE_GETTID
	return getpid() == gettid();
# else
	return getpid() == syscall(SYS_gettid);
# endif
}

# if defined(HAVE_PTHREAD_GETATTR_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACK)
static bool zend_call_stack_get_linux_pthread(zend_call_stack *stack)
{
	pthread_attr_t attr;
	int error;
	void *addr;
	size_t max_size;

	/* pthread_getattr_np() will return bogus values for the main thread with
	 * musl or with some old glibc versions */
	ZEND_ASSERT(!zend_call_stack_is_main_thread());

	error = pthread_getattr_np(pthread_self(), &attr);
	if (error) {
		return false;
	}

	error = pthread_attr_getstack(&attr, &addr, &max_size);
	if (error) {
		return false;
	}

#  if defined(__GLIBC__) && (__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 8))
	{
		size_t guard_size;
		/* In glibc prior to 2.8, addr and size include the guard pages */
		error = pthread_attr_getguardsize(&attr, &guard_size);
		if (error) {
			return false;
		}

		addr = (int8_t*)addr + guard_size;
		max_size -= guard_size;
	}
#  endif /* glibc < 2.8 */

	stack->base = (int8_t*)addr + max_size;
	stack->max_size = max_size;

	return true;
}
# else /* defined(HAVE_PTHREAD_GETATTR_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACK) */
static bool zend_call_stack_get_linux_pthread(zend_call_stack *stack)
{
	return false;
}
# endif /* defined(HAVE_PTHREAD_GETATTR_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACK) */

static bool zend_call_stack_get_linux_proc_maps(zend_call_stack *stack)
{
	FILE *f;
	char buffer[4096];
	uintptr_t addr_on_stack = (uintptr_t)&buffer;
	uintptr_t start, end, prev_end = 0;
	size_t max_size;
	bool found = false;
	struct rlimit rlim;
	int error;

	/* This method is relevant only for the main thread */
	ZEND_ASSERT(zend_call_stack_is_main_thread());

	/* Scan the process memory mappings to find the one containing the stack.
	 *
	 * The end of the stack mapping is the base of the stack. The start is
	 * adjusted by the kernel as the stack grows. The maximum stack size is
	 * determined by RLIMIT_STACK and the previous mapping.
	 *
	 *
	 *                   ^ Higher addresses  ^
	 *                   :                   :
	 *                   :                   :
	 *   Mapping end --> |-------------------| <-- Stack base (stack start)
	 *                   |                   |   ^
	 *                   | Stack Mapping     |   | Stack size
	 *                   |                   |   v
	 * Mapping start --> |-------------------| <-- Current stack end
	 * (adjusted         :                   :
	 *  downwards as the .                   .
	 *  stack grows)     :                   :
	 *                   |-------------------|
	 *                   | Some Mapping      | The previous mapping may prevent
	 *                   |-------------------| stack growth
	 *                   :                   :
	 *                   :                   :
	 *                   v Lower addresses   v
	 */

	f = fopen("/proc/self/maps", "r");
	if (!f) {
		return false;
	}

	while (fgets(buffer, sizeof(buffer), f) && sscanf(buffer, "%" SCNxPTR "-%" SCNxPTR, &start, &end) == 2) {
		if (start <= addr_on_stack && end >= addr_on_stack) {
			found = true;
			break;
		}
		prev_end = end;
	}

	fclose(f);

	if (!found) {
		return false;
	}

	error = getrlimit(RLIMIT_STACK, &rlim);
	if (error || rlim.rlim_cur == RLIM_INFINITY) {
		return false;
	}

	max_size = rlim.rlim_cur;

	/* Previous mapping may prevent the stack from growing */
	if (end - max_size < prev_end) {
		max_size = prev_end - end;
	}

	stack->base = (void*)end;
	stack->max_size = max_size;

	return true;
}

static bool zend_call_stack_get_linux(zend_call_stack *stack)
{
	if (zend_call_stack_is_main_thread()) {
		return zend_call_stack_get_linux_proc_maps(stack);
	}

	return zend_call_stack_get_linux_pthread(stack);
}
#else /* __linux__ */
static bool zend_call_stack_get_linux(zend_call_stack *stack)
{
	return false;
}
#endif /* __linux__ */

#if defined(__FreeBSD__) || defined(__DragonFly__)
static bool zend_call_stack_is_main_thread(void)
{
	int is_main = pthread_main_np();
	return is_main == -1 || is_main == 1;
}

# if defined(HAVE_PTHREAD_ATTR_GET_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACK)
static bool zend_call_stack_get_freebsd_pthread(zend_call_stack *stack)
{
	pthread_attr_t attr;
	int error;
	void *addr;
	size_t max_size;

	/* pthread will return bogus values for the main thread */
	ZEND_ASSERT(!zend_call_stack_is_main_thread());

	pthread_attr_init(&attr);

	error = pthread_attr_get_np(pthread_self(), &attr);
	if (error) {
		goto fail;
	}

	error = pthread_attr_getstack(&attr, &addr, &max_size);
	if (error) {
		goto fail;
	}

	stack->base = (int8_t*)addr + max_size;
	stack->max_size = max_size;

	pthread_attr_destroy(&attr);
	return true;

fail:
	pthread_attr_destroy(&attr);
	return false;
}
# else /* defined(HAVE_PTHREAD_ATTR_GET_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACK) */
static bool zend_call_stack_get_freebsd_pthread(zend_call_stack *stack)
{
	return false;
}
# endif /* defined(HAVE_PTHREAD_ATTR_GET_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACK) */

static bool zend_call_stack_get_freebsd_sysctl(zend_call_stack *stack)
{
	void *stack_base;
	int mib[2] = {CTL_KERN, KERN_USRSTACK};
	size_t len = sizeof(stack_base);
	struct rlimit rlim;

	/* This method is relevant only for the main thread */
	ZEND_ASSERT(zend_call_stack_is_main_thread());

	if (sysctl(mib, sizeof(mib)/sizeof(*mib), &stack_base, &len, NULL, 0) != 0) {
		return false;
	}

	if (getrlimit(RLIMIT_STACK, &rlim) != 0) {
		return false;
	}

	if (rlim.rlim_cur == RLIM_INFINITY) {
		return false;
	}

	size_t guard_size = getpagesize();

	stack->base = stack_base;
	stack->max_size = rlim.rlim_cur - guard_size;

	return true;
}

static bool zend_call_stack_get_freebsd(zend_call_stack *stack)
{
	if (zend_call_stack_is_main_thread()) {
		return zend_call_stack_get_freebsd_sysctl(stack);
	}

	return zend_call_stack_get_freebsd_pthread(stack);
}
#else
static bool zend_call_stack_get_freebsd(zend_call_stack *stack)
{
	return false;
}
#endif /* __FreeBSD__ */

#ifdef ZEND_WIN32
static bool zend_call_stack_get_win32(zend_call_stack *stack)
{
	ULONG_PTR low_limit, high_limit;
	ULONG size;
	MEMORY_BASIC_INFORMATION guard_region = {0}, uncommitted_region = {0};
	size_t result_size, page_size;

	/* The stack consists of three regions: committed, guard, and uncommitted.
	 * Memory is committed when the guard region is accessed. If only one page
	 * is left in the uncommitted region, a stack overflow error is raised
	 * instead.
	 *
	 * The total useable stack size is the size of the committed and uncommitted
	 * regions less one page.
	 *
	 * http://blogs.msdn.com/b/satyem/archive/2012/08/13/thread-s-stack-memory-management.aspx
	 * https://learn.microsoft.com/en-us/windows/win32/procthread/thread-stack-size
	 *
	 *                ^  Higher addresses  ^
	 *                :                    :
	 *                :                    :
	 * high_limit --> |--------------------|
	 *            ^   |                    |
	 *            |   | Committed region   |
	 *            |   |                    |
	 *            |   |------------------- | <-- guard_region.BaseAddress
	 *   reserved |   |                    |     + guard_region.RegionSize
	 *       size |   | Guard region       |
	 *            |   |                    |
	 *            |   |--------------------| <-- guard_region.BaseAddress,
	 *            |   |                    |     uncommitted_region.BaseAddress
	 *            |   | Uncommitted region |     + uncommitted_region.RegionSize
	 *            v   |                    |
	 * low_limit  --> |------------------- | <-- uncommitted_region.BaseAddress
	 *                :                    :
	 *                :                    :
	 *                v  Lower addresses   v
	 */

	GetCurrentThreadStackLimits(&low_limit, &high_limit);

	result_size = VirtualQuery((void*)low_limit,
			&uncommitted_region, sizeof(uncommitted_region));
	ZEND_ASSERT(result_size >= sizeof(uncommitted_region));

	result_size = VirtualQuery((int8_t*)uncommitted_region.BaseAddress + uncommitted_region.RegionSize,
			&guard_region, sizeof(guard_region));
	ZEND_ASSERT(result_size >= sizeof(uncommitted_region));

	stack->base = (void*)high_limit;
	stack->max_size = (uintptr_t)high_limit - (uintptr_t)low_limit;

	ZEND_ASSERT(stack->max_size > guard_region.RegionSize);
	stack->max_size -= guard_region.RegionSize;

	/* The uncommitted region does not shrink below 1 page */
	page_size = zend_get_page_size();
	ZEND_ASSERT(stack->max_size > page_size);
	stack->max_size -= page_size;

	return true;
}
#else /* ZEND_WIN32 */
static bool zend_call_stack_get_win32(zend_call_stack *stack)
{
	return false;
}
#endif /* ZEND_WIN32 */

#if defined(__APPLE__) && defined(HAVE_PTHREAD_GET_STACKADDR_NP)
static bool zend_call_stack_get_macos(zend_call_stack *stack)
{
	void *base = pthread_get_stackaddr_np(pthread_self());
	size_t max_size;

#if !defined(__aarch64__)
	if (pthread_main_np())
	{
		/* pthread_get_stacksize_np() returns a too low value for the main
		 * thread in OSX 10.9, 10.10:
		 * https://mail.openjdk.org/pipermail/hotspot-dev/2013-October/011353.html
		 * https://github.com/rust-lang/rust/issues/43347
		 */

		/* Stack size is 8MiB by default for main threads
		 * https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Multithreading/CreatingThreads/CreatingThreads.html */
		max_size = 8 * 1024 * 1024;
	}
	else
#endif
	{
		max_size = pthread_get_stacksize_np(pthread_self());
	}

	stack->base = base;
	stack->max_size = max_size;

	return true;
}
#else /* defined(__APPLE__) && defined(HAVE_PTHREAD_GET_STACKADDR_NP) */
static bool zend_call_stack_get_macos(zend_call_stack *stack)
{
	return false;
}
#endif /* defined(__APPLE__) && defined(HAVE_PTHREAD_GET_STACKADDR_NP) */

#if defined(__OpenBSD__)
#if defined(HAVE_PTHREAD_STACKSEG_NP)
static bool zend_call_stack_get_openbsd_pthread(zend_call_stack *stack)
{
	stack_t ss;

	if (pthread_stackseg_np(pthread_self(), &ss) != 0) {
		return false;
	}

	stack->base = (char *)ss.ss_sp - ss.ss_size;
	stack->max_size = ss.ss_size - sysconf(_SC_PAGE_SIZE);

	return true;
}
#else
static bool zend_call_stack_get_openbsd_pthread(zend_call_stack *stack)
{
	return false;
}
#endif /* defined(HAVE_PTHREAD_STACKSEG_NP) */

static bool zend_call_stack_get_openbsd_vm(zend_call_stack *stack)
{
	struct _ps_strings ps;
	struct rlimit rlim;
	int mib[2] = {CTL_VM, VM_PSSTRINGS };
	size_t len = sizeof(ps), pagesize;

	if (sysctl(mib, 2, &ps, &len, NULL, 0) != 0) {
		return false;
	}

	if (getrlimit(RLIMIT_STACK, &rlim) != 0) {
		return false;
	}

	if (rlim.rlim_cur == RLIM_INFINITY) {
		return false;
	}

	pagesize = sysconf(_SC_PAGE_SIZE);

	stack->base = (void *)((uintptr_t)ps.val + (pagesize - 1) & ~(pagesize - 1));
	stack->max_size = rlim.rlim_cur - pagesize;

	return true;
}

static bool zend_call_stack_get_openbsd(zend_call_stack *stack)
{
	// TIB_THREAD_INITIAL_STACK is private and here we avoid using pthread's api (ie pthread_main_np)
	if (!TIB_GET()->tib_thread || (TIB_GET()->tib_thread_flags & 0x002) != 0) {
		return zend_call_stack_get_openbsd_vm(stack);
	}

	return zend_call_stack_get_openbsd_pthread(stack);
}

#else
static bool zend_call_stack_get_openbsd(zend_call_stack *stack)
{
	return false;
}
#endif /* defined(__OpenBSD__) */
#if defined(__HAIKU__)
static bool zend_call_stack_get_haiku(zend_call_stack *stack)
{
	thread_id id;
	thread_info ti;
	size_t guard_size;

	// unlikely, main thread ought to be always available but we never know
	if ((id = find_thread(NULL)) == B_NAME_NOT_FOUND || get_thread_info(id, &ti) != B_OK) {
		return false;
	}

	// USER_STACK_GUARD_SIZE
	guard_size = sysconf(_SC_PAGESIZE) * 4;

	stack->base = ti.stack_end;
	stack->max_size = ((size_t)ti.stack_end - (size_t)ti.stack_base) - guard_size;

	return true;
}
#else
static bool zend_call_stack_get_haiku(zend_call_stack *stack)
{
	return false;
}
#endif /* defined(__HAIKU__) */

#if defined(__NetBSD__)
# ifdef HAVE_PTHREAD_GETATTR_NP
static bool zend_call_stack_get_netbsd_pthread(zend_call_stack *stack)
{
	pthread_attr_t attr;
	int error;
	void *addr;
	size_t max_size, guard_size;

	error = pthread_getattr_np(pthread_self(), &attr);
	if (error) {
		return false;
	}

	error = pthread_attr_getstack(&attr, &addr, &max_size);
	if (error) {
		return false;
	}

	error = pthread_attr_getguardsize(&attr, &guard_size);
	if (error) {
		return false;
	}

	addr = (char *)addr + guard_size;
	max_size -= guard_size;

	stack->base = (char *)addr + max_size;
	stack->max_size = max_size;

	return true;
}
# else
static bool zend_call_stack_get_netbsd_pthread(zend_call_stack *stack)
{
	return false;
}
# endif /* HAVE_PTHREAD_GETATTR_NP */
static bool zend_call_stack_get_netbsd_vm(zend_call_stack *stack, void **ptr)
{
	/**
	 * NetBSD supports procfs in a similar fashion as Linux
	 * however NetBSD's mid/long term plan is to remove it completely.
	 */
	char *start, *end;
	struct kinfo_vmentry *entry;
	size_t len, max_size;
	char buffer[4096];
	uintptr_t addr_on_stack = (uintptr_t)&buffer;
	int mib[5] = { CTL_VM, VM_PROC, VM_PROC_MAP, getpid(), sizeof(struct kinfo_vmentry) };
	bool found = false;
	struct rlimit rlim;

	if (sysctl(mib, 5, NULL, &len, NULL, 0) != 0) {
		return false;
	}

	// kinfo_getvmmap uses the same formula, only we do not want to rely on libkvm
	len = len * 4 / 3 ;
	*ptr = malloc(len);

	if (sysctl(mib, 5, *ptr, &len, NULL, 0) != 0) {
		return false;
	}

	start = (char *)*ptr;
	end = start + len;

	while (start < end) {
		entry = (struct kinfo_vmentry *)start;
		if (entry->kve_start <= addr_on_stack && entry->kve_end >= addr_on_stack) {
			found = true;
			break;
		}

		start += sizeof(struct kinfo_vmentry);
	}

	if (!found) {
		return false;
	}

	if (getrlimit(RLIMIT_STACK, &rlim) || rlim.rlim_cur == RLIM_INFINITY) {
		return false;
	}

	max_size = rlim.rlim_cur;

	stack->base = (void *)entry->kve_end;
	stack->max_size = max_size;

	return true;
}


static bool zend_call_stack_get_netbsd(zend_call_stack *stack)
{
	if (syscall(SYS__lwp_self) == 1) {
		void *ptr = NULL;
		bool r = zend_call_stack_get_netbsd_vm(stack, &ptr);
		free(ptr);
		return r;
	}

	return zend_call_stack_get_netbsd_pthread(stack);
}
#else
static bool zend_call_stack_get_netbsd(zend_call_stack *stack)
{
	return false;
}
#endif /* defined(__NetBSD__) */

#if defined(__sun)
static bool zend_call_stack_get_solaris_pthread(zend_call_stack *stack)
{
	stack_t s;
	if (thr_stksegment(&s) < 0) {
		return false;
	}

	stack->max_size = s.ss_size;
	stack->base = s.ss_sp;
	return true;
}

static bool zend_call_stack_get_solaris_proc_maps(zend_call_stack *stack)
{
	char buffer[4096];
	uintptr_t addr_on_stack = (uintptr_t)&buffer;
	bool found = false, r = false;
	struct ps_prochandle *proc;
	prmap_t *map, *orig;
	struct rlimit rlim;
	char path[PATH_MAX];
	size_t size;
	ssize_t len;
	pid_t pid;
	int error, fd;

	pid = getpid();
	proc = Pgrab(pid, PGRAB_RDONLY, &error);
	if (!proc) {
		return false;
	}

	size = (1 << 20);
	snprintf(path, sizeof(path), "/proc/%d/map", (int)pid);

	if ((fd = open(path, O_RDONLY)) == -1) {
		Prelease(proc, 0);
		return false;
	}

	orig = malloc(size);
	if (!orig) {
		Prelease(proc, 0);
		close(fd);
		return false;
	}

	while (size > 0 && (len = pread(fd, orig, size, 0)) == size) {
		prmap_t *tmp;
		size <<= 1;
		tmp = realloc(orig, size);
		if (!tmp) {
			goto end;
		}
		orig = tmp;
	}

	for (map = orig; len > 0; ++map) {
		if ((uintptr_t)map->pr_vaddr <= addr_on_stack && (uintptr_t)map->pr_vaddr + map->pr_size >= addr_on_stack) {
			found = true;
			break;
		}
		len -= sizeof(*map);
	}

	if (!found) {
		goto end;
	}

	error = getrlimit(RLIMIT_STACK, &rlim);
	if (error || rlim.rlim_cur == RLIM_INFINITY) {
		goto end;
	}

	stack->base = (void *)map->pr_vaddr + map->pr_size;
	stack->max_size = rlim.rlim_cur;
	r = true;

end:
	free(orig);
	Prelease(proc, 0);
	close(fd);
	return r;
}

static bool zend_call_stack_get_solaris(zend_call_stack *stack)
{
	if (_lwp_self() == 1) {
		return zend_call_stack_get_solaris_proc_maps(stack);
	}
	return zend_call_stack_get_solaris_pthread(stack);
}
#else
static bool zend_call_stack_get_solaris(zend_call_stack *stack)
{
	return false;
}
#endif /* defined(__sun) */

/** Get the stack information for the calling thread */
ZEND_API bool zend_call_stack_get(zend_call_stack *stack)
{
	if (zend_call_stack_get_linux(stack)) {
		return true;
	}

	if (zend_call_stack_get_freebsd(stack)) {
		return true;
	}

	if (zend_call_stack_get_win32(stack)) {
		return true;
	}

	if (zend_call_stack_get_macos(stack)) {
		return true;
	}

	if (zend_call_stack_get_openbsd(stack)) {
		return true;
	}

	if (zend_call_stack_get_netbsd(stack)) {
		return true;
	}

	if (zend_call_stack_get_haiku(stack)) {
		return true;
	}

	if (zend_call_stack_get_solaris(stack)) {
		return true;
	}

	return false;
}

#endif /* ZEND_CHECK_STACK_LIMIT */
