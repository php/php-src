
	/* $Id: fpm_atomic.h,v 1.3 2008/09/18 23:34:11 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_ATOMIC_H
#define FPM_ATOMIC_H 1

#include <stdint.h>
#include <sched.h>

#if ( __i386__ || __i386 )

typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;


static inline atomic_int_t atomic_fetch_add(atomic_t *value, atomic_int_t add)
{
	__asm__ volatile ( "lock;" "xaddl %0, %1;" :
		"+r" (add) : "m" (*value) : "memory");

	return add;
}

static inline atomic_uint_t atomic_cmp_set(atomic_t *lock, atomic_uint_t old, atomic_uint_t set)
{
	unsigned char res;

	__asm__ volatile ( "lock;" "cmpxchgl %3, %1;" "sete %0;" :
		"=a" (res) : "m" (*lock), "a" (old), "r" (set) : "memory");

    return res;
}

#elif ( __amd64__ || __amd64 )

typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;

static inline atomic_int_t atomic_fetch_add(atomic_t *value, atomic_int_t add)
{
	__asm__ volatile ( "lock;" "xaddq %0, %1;" :
		"+r" (add) : "m" (*value) : "memory");

	return add;
}

static inline atomic_uint_t atomic_cmp_set(atomic_t *lock, atomic_uint_t old, atomic_uint_t set)
{
	unsigned char res;

	__asm__ volatile ( "lock;" "cmpxchgq %3, %1;" "sete %0;" :
		"=a" (res) : "m" (*lock), "a" (old), "r" (set) : "memory");

	return res;
}

#else

#error unsupported processor. please write a patch and send it to me

#endif

static inline int fpm_spinlock(atomic_t *lock, int try_once)
{
	if (try_once) {
		return atomic_cmp_set(lock, 0, 1) ? 0 : -1;
	}

	for (;;) {

		if (atomic_cmp_set(lock, 0, 1)) {
			break;
		}

		sched_yield();
	}

	return 0;
}

#endif

