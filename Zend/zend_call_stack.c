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
#include "zend_fibers.h"
#include <signal.h>
#include <stdint.h>
#ifdef ZEND_WIN32
# include <winnt.h>
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
#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
# include <pthread.h>
#endif
#ifdef __FreeBSD__
# include <pthread_np.h>
# include <sys/mman.h>
# include <sys/sysctl.h>
# include <sys/user.h>
#endif
#ifdef __linux__
#include <sys/syscall.h>
#endif

static bool zend_sigsegv_handler_installed = false;

static bool zend_addr_in_stack(zend_call_stack *stack, void *addr)
{
	if (stack->base == NULL) {
		return false;
	}

	return (uintptr_t)addr <= (uintptr_t)stack->base
		&& (uintptr_t)addr >= (uintptr_t)stack->base - stack->max_size;
}

static bool zend_restore_default_sigsegv_action(void)
{
	struct sigaction sa = {
		.sa_handler = SIG_DFL,
	};

	return sigaction(SIGSEGV, &sa, NULL) == 0;
}

static void zend_sigsegv_handler(int signo, siginfo_t *siginfo, void *context)
{
	(void) context;

	if (EG(stack_overflow)) {
		/* The handler is called while handling an other potential stack
		 * overflow, fallback to the default SIGSEGV action */
		if (!zend_restore_default_sigsegv_action()) {
			abort();
		}
		return;
	}

	EG(stack_overflow) = true;

	zend_call_stack stack;
	if (EG(active_fiber)) {
		zend_fiber_stack_get_layout(&stack, EG(active_fiber)->context.stack);
	} else {
		stack = EG(call_stack);
	}

	if (!zend_addr_in_stack(&stack, siginfo->si_addr)) {
		/* The addr is not in the stack, fallback to the default SIGSEGV action */
		if (!zend_restore_default_sigsegv_action()) {
			abort();
		}
		EG(stack_overflow) = false;
		return;
	}

	EG(stack_overflow_in_compilation) = CG(in_compilation);
	EG(stack_overflow_execute_data) = EG(current_execute_data);
	zend_bailout();
}

ZEND_API void zend_handle_stack_overflow(void)
{
	const char *msg;

	/* Temporarily restore state for zend_error() */
	CG(in_compilation) = EG(stack_overflow_in_compilation);
	EG(current_execute_data) = EG(stack_overflow_execute_data);

	if (CG(in_compilation)) {
		msg = "Stack overflow during compilation. Try splitting expression";
	} else {
		msg = "Stack overflow. Infinite recursion?";
	}

	zend_try {
		/* TODO: print stack trace */
		zend_error_noreturn(E_ERROR, "%s", msg);
	} zend_end_try();

	EG(stack_overflow_in_compilation) = false;
	EG(stack_overflow_execute_data) = NULL;
	EG(stack_overflow) = false;
}

ZEND_API void zend_stack_overflow_handler_thread_startup(void)
{
	stack_t oldss;

	zend_call_stack_get(&EG(call_stack));

	if (sigaltstack(NULL, &oldss) != 0) {
		return;
	}

	if (oldss.ss_sp == NULL) {
		EG(alt_stack).ss_sp = malloc(SIGSTKSZ);
		EG(alt_stack).ss_size = SIGSTKSZ;

		if (sigaltstack(&EG(alt_stack), NULL) != 0) {
			return;
		}
	}
}

ZEND_API void zend_stack_overflow_handler_thread_shutdown(void)
{
	if (EG(alt_stack).ss_sp) {
		stack_t ss = {
			.ss_flags = SS_DISABLE,
		};
		if (sigaltstack(&ss, NULL) != 0) {
			return;
		}

		free(EG(alt_stack).ss_sp);
		EG(alt_stack).ss_sp = NULL;
	}
}

ZEND_API void zend_stack_overflow_handler_startup(void)
{
	struct sigaction oldsa;

	if (sigaction(SIGSEGV, NULL, &oldsa) != 0) {
		return;
	}

	if (oldsa.sa_handler != SIG_DFL) {
		return;
	}

	struct sigaction sa = {
		.sa_sigaction = zend_sigsegv_handler,
		/* SA_NODEFER: Do not mask SIGSEGV before entering the signal handler.
		 * Otherwise we have to unblock it manually since we do not execute the
		 * signal trampoline. Also, if we generated a SIGSEGV while it is
		 * blocked, the result would be undefined */
		.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER,
	};

	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGSEGV, &sa, NULL) != 0) {
		return;
	}

	zend_sigsegv_handler_installed = true;
}

ZEND_API void zend_stack_overflow_handler_shutdown(void)
{
	struct sigaction oldsa;

	if (zend_sigsegv_handler_installed) {
		if (sigaction(SIGSEGV, NULL, &oldsa) != 0) {
			return;
		}

		if (oldsa.sa_sigaction != zend_sigsegv_handler) {
			return;
		}

		if (!zend_restore_default_sigsegv_action()) {
			return;
		}

		zend_sigsegv_handler_installed = false;
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

# ifdef HAVE_PTHREAD_GETATTR_NP
static bool zend_call_stack_get_linux_pthread(zend_call_stack *stack)
{
	pthread_attr_t attr;
	int error;
	void *addr;
	size_t max_useable_size;
	size_t guard_size;

	/* pthread_getattr_np() will return bogus values for the main thread with
	 * musl or with some old glibc versions */
	ZEND_ASSERT(!zend_call_stack_is_main_thread());

	error = pthread_getattr_np(pthread_self(), &attr);
	if (error) {
		return false;
	}

	error = pthread_attr_getstack(&attr, &addr, &max_useable_size);
	if (error) {
		return false;
	}

	error = pthread_attr_getguardsize(&attr, &guard_size);
	if (error) {
		return false;
	}


#  if defined(__GLIBC__) && (__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 8))
	/* In glibc prior to 2.8, addr and size include the guard pages */
	stack->max_size = max_useable_size;
	stack->max_useable_size = stack->max_size - guard_size;
	stack->base = (int8_t*)addr + stack->max_size;
#  else /* glibc < 2.8 */
	stack->max_size = max_useable_size + guard_size;
	stack->max_useable_size = max_useable_size;
	stack->base = (int8_t*)addr + max_useable_size;
#  endif /* glibc < 2.8 */


	return true;
}
# else /* HAVE_PTHREAD_GETATTR_NP */
static bool zend_call_stack_get_linux_pthread(zend_call_stack *stack)
{
	return false;
}
# endif /* HAVE_PTHREAD_GETATTR_NP */

static bool zend_call_stack_get_linux_proc_maps(zend_call_stack *stack)
{
	FILE *f;
	char buffer[4096];
	uintptr_t addr_on_stack = (uintptr_t)&buffer;
	uintptr_t start, end, prev_end = 0;
	size_t max_useable_size;
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

	max_useable_size = rlim.rlim_cur;

	/* stack_guard_gap is 256 by default */
	size_t guard_size = zend_get_page_size() * 256;

	stack->base = (void*)end;
	stack->max_useable_size = max_useable_size;
	stack->max_size = stack->max_useable_size + guard_size;

	/* Previous mapping may prevent the stack from growing */
	if (end - stack->max_useable_size < prev_end) {
		stack->max_useable_size = prev_end - end;
		stack->max_size = stack->max_useable_size;
	}

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

#ifdef __FreeBSD__
static bool zend_call_stack_is_main_thread(void)
{
	int is_main = pthread_main_np();
	return is_main == -1 || is_main == 1;
}

# if defined(HAVE_PTHREAD_ATTR_GET_NP) && defined(HAVE_PTHREAD_ATTR_GET_STACK)
static bool zend_call_stack_get_freebsd_pthread(zend_call_stack *stack)
{
	pthread_attr_t attr;
	int error;
	void *addr;
	size_t max_useable_size;
	size_t guard_size;

	/* pthread will return bogus values for the main thread */
	ZEND_ASSERT(!zend_call_stack_is_main_thread());

	pthread_attr_init(&attr);

	error = pthread_attr_get_np(pthread_self(), &attr);
	if (error) {
		goto fail;
	}

	error = pthread_attr_getstack(&attr, &addr, &max_useable_size);
	if (error) {
		goto fail;
	}

	error = pthread_attr_getguardsize(&attr, &guard_size);
	if (error) {
		return false;
	}

	stack->base = (int8_t*)addr + max_useable_size;
	stack->max_useable_size = max_useable_size;
	stack->max_size = max_useable_size + guard_size;

	pthread_attr_destroy(&attr);
	return true;

fail:
	pthread_attr_destroy(&attr);
	return false;
}
# else /* defined(HAVE_PTHREAD_ATTR_GET_NP) && defined(HAVE_PTHREAD_ATTR_GET_STACK) */
static bool zend_call_stack_get_freebsd_pthread(zend_call_stack *stack)
{
	return false;
}
# endif /* defined(HAVE_PTHREAD_ATTR_GET_NP) && defined(HAVE_PTHREAD_ATTR_GET_STACK) */

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

	size_t guard_size = zend_get_page_size();

	stack->base = stack_base;
	stack->max_useable_size = rlim.rlim_cur - guard_size;
	stack->max_size = rlim.rlim_cur;

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
	MEMORY_BASIC_INFORMATION stack_info;
	int8_t *base;

#ifdef _M_ARM64
	return false;
#endif

#ifdef _M_X64
	base = (void*)((NT_TIB64*)NtCurrentTeb())->StackBase;
#else
	base = (void*)((NT_TIB*)NtCurrentTeb())->StackBase;
#endif

	memset(&stack_info, 0, sizeof(MEMORY_BASIC_INFORMATION));
	size_t result_size = VirtualQuery(&stack_info, &stack_info, sizeof(MEMORY_BASIC_INFORMATION));
	ZEND_ASSERT(result_size >= sizeof(MEMORY_BASIC_INFORMATION));

	int8_t* end = (int8_t*)stack_info.AllocationBase;
	ZEND_ASSERT(base > end);

	size_t max_size = (size_t)(base - end);

	// Last pages are not usable
	// http://blogs.msdn.com/b/satyem/archive/2012/08/13/thread-s-stack-memory-management.aspx
	ZEND_ASSERT(max_size > 4*4096);
	size_t max_useable_size -= 4*4096;

	stack->base = base;
	stack->max_useable_size = max_size;
	stack->max_size = max_size;

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
	// TODO

	void *base = pthread_get_stackaddr_np(pthread_self());
	size_t max_size;

	if (pthread_main_np()) {
		/* pthread_get_stacksize_np() returns a too low value for the main
		 * thread in OSX 10.9, 10.10:
		 * https://mail.openjdk.org/pipermail/hotspot-dev/2013-October/011353.html
		 * https://github.com/rust-lang/rust/issues/43347
		 */

		/* Stack size is 8MiB by default for main threads
		 * https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Multithreading/CreatingThreads/CreatingThreads.html */
		max_size = 8 * 1024 * 1024;
	} else {
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

	return false;
}

