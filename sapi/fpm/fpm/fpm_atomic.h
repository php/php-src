	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_ATOMIC_H
#define FPM_ATOMIC_H 1

#include <inttypes.h>
#include <sched.h>

#ifdef HAVE_BUILTIN_ATOMIC

/**
 * all the cases below (as provided by upstream) define:
 * word as atomic_int_t, and
 * unsigned word as atomic_uint_t
 * and only use volatile atomic_uint_t as atomic_t
 */

typedef volatile unsigned long atomic_t;
#define atomic_cmp_set(a,b,c) __sync_bool_compare_and_swap(a,b,c)

#elif ( __i386__ || __i386 )

typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;


static inline atomic_int_t atomic_fetch_add(atomic_t *value, atomic_int_t add) /* {{{ */
{
	__asm__ volatile ( "lock;" "xaddl %0, %1;" :
		"+r" (add) : "m" (*value) : "memory");

	return add;
}
/* }}} */

static inline atomic_uint_t atomic_cmp_set(atomic_t *lock, atomic_uint_t old, atomic_uint_t set) /* {{{ */
{
	unsigned char res;

	__asm__ volatile ( "lock;" "cmpxchgl %3, %1;" "sete %0;" :
		"=a" (res) : "m" (*lock), "a" (old), "r" (set) : "memory");

	return res;
}
/* }}} */

#elif ( __amd64__ || __amd64 || __x86_64__ )

typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;

static inline atomic_int_t atomic_fetch_add(atomic_t *value, atomic_int_t add) /* {{{ */
{
	__asm__ volatile ( "lock;" "xaddq %0, %1;" :
		"+r" (add) : "m" (*value) : "memory");

	return add;
}
/* }}} */

static inline atomic_uint_t atomic_cmp_set(atomic_t *lock, atomic_uint_t old, atomic_uint_t set) /* {{{ */
{
	unsigned char res;

	__asm__ volatile ( "lock;" "cmpxchgq %3, %1;" "sete %0;" :
		"=a" (res) : "m" (*lock), "a" (old), "r" (set) : "memory");

	return res;
}
/* }}} */

#if (__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 2))

#elif ( __arm__ || __arm ) /* W-Mark Kubacki */

#if (__arch64__ || __arch64)
typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
#else
typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
#endif

#define atomic_cmp_set(a,b,c) __sync_bool_compare_and_swap(a,b,c)

#endif /* defined (__GNUC__) &&... */

#elif ( __sparc__ || __sparc ) /* Marcin Ochab */

#if (__sparcv9 || __sparcv9__)

#if (__arch64__ || __arch64)
typedef uint64_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;

static inline atomic_uint_t atomic_cas_64(atomic_t *lock, atomic_uint_t old, atomic_uint_t new) /* {{{ */
{
	__asm__ __volatile__("casx [%2], %3, %0 " : "=&r"(new)  : "0"(new), "r"(lock), "r"(old): "memory");

	return new;
}
/* }}} */

static inline atomic_uint_t atomic_cmp_set(atomic_t *lock, atomic_uint_t old, atomic_uint_t set) /* {{{ */
{
	return (atomic_cas_64(lock, old, set)==old);
}
/* }}} */
#else
typedef uint32_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;

static inline atomic_uint_t atomic_cas_32(atomic_t *lock, atomic_uint_t old, atomic_uint_t new) /* {{{ */
{
	__asm__ __volatile__("cas [%2], %3, %0 " : "=&r"(new)  : "0"(new), "r"(lock), "r"(old): "memory");

	return new;
}
/* }}} */

static inline atomic_uint_t atomic_cmp_set(atomic_t *lock, atomic_uint_t old, atomic_uint_t set) /* {{{ */
{
	return (atomic_cas_32(lock, old, set)==old);
}
/* }}} */
#endif

#else /* #if (__sparcv9 || __sparcv9__) */
#error Sparc v8 and predecessors are not and will not be supported (see bug report 53310)
#endif /* #if (__sparcv9 || __sparcv9__) */

#else

#error Unsupported processor. Please open a bug report (bugs.php.net).

#endif

static inline int fpm_spinlock(atomic_t *lock, int try_once) /* {{{ */
{
	if (try_once) {
		return atomic_cmp_set(lock, 0, 1) ? 1 : 0;
	}

	for (;;) {

		if (atomic_cmp_set(lock, 0, 1)) {
			break;
		}

		sched_yield();
	}

	return 1;
}
/* }}} */

#define fpm_unlock(lock) lock = 0

#endif
