/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991, 2000, 2001 by Lucent Technologies.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/

/* Please send bug reports to David M. Gay (dmg at acm dot org,
 * with " at " changed at "@" and " dot " changed to ".").	*/

/* On a machine with IEEE extended-precision registers, it is
 * necessary to specify double-precision (53-bit) rounding precision
 * before invoking strtod or dtoa.  If the machine uses (the equivalent
 * of) Intel 80x87 arithmetic, the call
 *	_control87(PC_53, MCW_PC);
 * does this with many compilers.  Whether this or another call is
 * appropriate depends on the compiler; for this to work, it may be
 * necessary to #include "float.h" or another system-dependent header
 * file.  When needed, this call avoids double rounding, which can
 * cause one bit errors, e.g., with strtod on 8.3e26 or 6.3876e-16.
 */

/* strtod for IEEE-, VAX-, and IBM-arithmetic machines.
 * (Note that IEEE arithmetic is disabled by gcc's -ffast-math flag.)
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets errno to ERANGE).  With IEEE arithmetic, ties are
 * broken by the IEEE round-even rule.  Otherwise ties are broken by
 * biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 92-101].
 *
 * Modifications:
 *
 *	1. We only require IEEE, IBM, or VAX double-precision
 *		arithmetic (not IEEE double-extended).
 *	2. We get by with floating-point arithmetic in a case that
 *		Clinger missed -- when we're computing d * 10^n
 *		for a small integer d and the integer n is not too
 *		much larger than 22 (the maximum integer k for which
 *		we can represent 10^k exactly), we may be able to
 *		compute (d*10^k) * 10^(e-k) with just one roundoff.
 *	3. Rather than a bit-at-a-time adjustment of the binary
 *		result in the hard case, we use floating-point
 *		arithmetic to determine the adjustment to within
 *		one bit; only in really hard cases do we need to
 *		compute a second residual.
 *	4. Because of 3., we don't need a large table of powers of 10
 *		for ten-to-e (just some small tables, e.g. of 10^k
 *		for 0 <= k <= 22).
 */

/*
 * #define IEEE_8087 for IEEE-arithmetic machines where the least
 *	significant byte has the lowest address.
 * #define IEEE_MC68k for IEEE-arithmetic machines where the most
 *	significant byte has the lowest address.
 * #define Long int on machines with 32-bit ints and 64-bit longs.
 * #define IBM for IBM mainframe-style floating-point arithmetic.
 * #define VAX for VAX-style floating-point arithmetic (D_floating).
 * #define No_leftright to omit left-right logic in fast floating-point
 *	computation of dtoa.  This will cause dtoa modes 4 and 5 to be
 *	treated the same as modes 2 and 3 for some inputs.
 * #define Honor_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3
 *	and strtod and dtoa should round accordingly.  Unless Trust_FLT_ROUNDS
 *	is also #defined, fegetround() will be queried for the rounding mode.
 *	Note that both FLT_ROUNDS and fegetround() are specified by the C99
 *	standard (and are specified to be consistent, with fesetround()
 *	affecting the value of FLT_ROUNDS), but that some (Linux) systems
 *	do not work correctly in this regard, so using fegetround() is more
 *	portable than using FLT_ROUNDS directly.
 * #define Check_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3
 *	and Honor_FLT_ROUNDS is not #defined.
 * #define RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
 *	that use extended-precision instructions to compute rounded
 *	products and quotients) with IBM.
 * #define ROUND_BIASED for IEEE-format with biased rounding and arithmetic
 *	that rounds toward +Infinity.
 * #define ROUND_BIASED_without_Round_Up for IEEE-format with biased
 *	rounding when the underlying floating-point arithmetic uses
 *	unbiased rounding.  This prevent using ordinary floating-point
 *	arithmetic when the result could be computed with one rounding error.
 * #define Inaccurate_Divide for IEEE-format with correctly rounded
 *	products but inaccurate quotients, e.g., for Intel i860.
 * #define NO_LONG_LONG on machines that do not have a "long long"
 *	integer type (of >= 64 bits).  On such machines, you can
 *	#define Just_16 to store 16 bits per 32-bit Long when doing
 *	high-precision integer arithmetic.  Whether this speeds things
 *	up or slows things down depends on the machine and the number
 *	being converted.  If long long is available and the name is
 *	something other than "long long", #define Llong to be the name,
 *	and if "unsigned Llong" does not work as an unsigned version of
 *	Llong, #define #ULLong to be the corresponding unsigned type.
 * #define Bad_float_h if your system lacks a float.h or if it does not
 *	define some or all of DBL_DIG, DBL_MAX_10_EXP, DBL_MAX_EXP,
 *	FLT_RADIX, FLT_ROUNDS, and DBL_MAX.
 * #define MALLOC your_malloc, where your_malloc(n) acts like malloc(n)
 *	if memory is available and otherwise does something you deem
 *	appropriate.  If MALLOC is undefined, malloc will be invoked
 *	directly -- and assumed always to succeed.  Similarly, if you
 *	want something other than the system's free() to be called to
 *	recycle memory acquired from MALLOC, #define FREE to be the
 *	name of the alternate routine.  (FREE or free is only called in
 *	pathological cases, e.g., in a dtoa call after a dtoa return in
 *	mode 3 with thousands of digits requested.)
 * #define Omit_Private_Memory to omit logic (added Jan. 1998) for making
 *	memory allocations from a private pool of memory when possible.
 *	When used, the private pool is PRIVATE_MEM bytes long:  2304 bytes,
 *	unless #defined to be a different length.  This default length
 *	suffices to get rid of MALLOC calls except for unusual cases,
 *	such as decimal-to-binary conversion of a very long string of
 *	digits.  The longest string dtoa can return is about 751 bytes
 *	long.  For conversions by strtod of strings of 800 digits and
 *	all dtoa conversions in single-threaded executions with 8-byte
 *	pointers, PRIVATE_MEM >= 7400 appears to suffice; with 4-byte
 *	pointers, PRIVATE_MEM >= 7112 appears adequate.
 * #define NO_INFNAN_CHECK if you do not wish to have INFNAN_CHECK
 *	#defined automatically on IEEE systems.  On such systems,
 *	when INFNAN_CHECK is #defined, strtod checks
 *	for Infinity and NaN (case insensitively).  On some systems
 *	(e.g., some HP systems), it may be necessary to #define NAN_WORD0
 *	appropriately -- to the most significant word of a quiet NaN.
 *	(On HP Series 700/800 machines, -DNAN_WORD0=0x7ff40000 works.)
 *	When INFNAN_CHECK is #defined and No_Hex_NaN is not #defined,
 *	strtod also accepts (case insensitively) strings of the form
 *	NaN(x), where x is a string of hexadecimal digits and spaces;
 *	if there is only one string of hexadecimal digits, it is taken
 *	for the 52 fraction bits of the resulting NaN; if there are two
 *	or more strings of hex digits, the first is for the high 20 bits,
 *	the second and subsequent for the low 32 bits, with intervening
 *	white space ignored; but if this results in none of the 52
 *	fraction bits being on (an IEEE Infinity symbol), then NAN_WORD0
 *	and NAN_WORD1 are used instead.
 * #define MULTIPLE_THREADS if the system offers preemptively scheduled
 *	multiple threads.  In this case, you must provide (or suitably
 *	#define) two locks, acquired by ACQUIRE_DTOA_LOCK(n) and freed
 *	by FREE_DTOA_LOCK(n) for n = 0 or 1.  (The second lock, accessed
 *	in pow5mult, ensures lazy evaluation of only one copy of high
 *	powers of 5; omitting this lock would introduce a small
 *	probability of wasting memory, but would otherwise be harmless.)
 *	You must also invoke freedtoa(s) to free the value s returned by
 *	dtoa.  You may do so whether or not MULTIPLE_THREADS is #defined.

 *	When MULTIPLE_THREADS is #defined, this source file provides
 *		void set_max_dtoa_threads(unsigned int n);
 *	and expects
 *		unsigned int dtoa_get_threadno(void);
 *	to be available (possibly provided by
 *		#define dtoa_get_threadno omp_get_thread_num
 *	if OpenMP is in use or by
 *		#define dtoa_get_threadno pthread_self
 *	if Pthreads is in use), to return the current thread number.
 *	If set_max_dtoa_threads(n) was called and the current thread
 *	number is k with k < n, then calls on ACQUIRE_DTOA_LOCK(...) and
 *	FREE_DTOA_LOCK(...) are avoided; instead each thread with thread
 *	number < n has a separate copy of relevant data structures.
 *	After set_max_dtoa_threads(n), a call set_max_dtoa_threads(m)
 *	with m <= n has has no effect, but a call with m > n is honored.
 *	Such a call invokes REALLOC (assumed to be "realloc" if REALLOC
 *	is not #defined) to extend the size of the relevant array.

 * #define NO_IEEE_Scale to disable new (Feb. 1997) logic in strtod that
 *	avoids underflows on inputs whose result does not underflow.
 *	If you #define NO_IEEE_Scale on a machine that uses IEEE-format
 *	floating-point numbers and flushes underflows to zero rather
 *	than implementing gradual underflow, then you must also #define
 *	Sudden_Underflow.
 * #define USE_LOCALE to use the current locale's decimal_point value.
 * #define SET_INEXACT if IEEE arithmetic is being used and extra
 *	computation should be done to set the inexact flag when the
 *	result is inexact and avoid setting inexact when the result
 *	is exact.  In this case, dtoa.c must be compiled in
 *	an environment, perhaps provided by #include "dtoa.c" in a
 *	suitable wrapper, that defines two functions,
 *		int get_inexact(void);
 *		void clear_inexact(void);
 *	such that get_inexact() returns a nonzero value if the
 *	inexact bit is already set, and clear_inexact() sets the
 *	inexact bit to 0.  When SET_INEXACT is #defined, strtod
 *	also does extra computations to set the underflow and overflow
 *	flags when appropriate (i.e., when the result is tiny and
 *	inexact or when it is a numeric value rounded to +-infinity).
 * #define NO_ERRNO if strtod should not assign errno = ERANGE when
 *	the result overflows to +-Infinity or underflows to 0.
 *	When errno should be assigned, under seemingly rare conditions
 *	it may be necessary to define Set_errno(x) suitably, e.g., in
 *	a local errno.h, such as
 *		#include <errno.h>
 *		#define Set_errno(x) _set_errno(x)
 * #define NO_HEX_FP to omit recognition of hexadecimal floating-point
 *	values by strtod.
 * #define NO_STRTOD_BIGCOMP (on IEEE-arithmetic systems only for now)
 *	to disable logic for "fast" testing of very long input strings
 *	to strtod.  This testing proceeds by initially truncating the
 *	input string, then if necessary comparing the whole string with
 *	a decimal expansion to decide close cases. This logic is only
 *	used for input more than STRTOD_DIGLIM digits long (default 40).
 */

#include <zend_operators.h>
#include <zend_strtod.h>
#include "zend_strtod_int.h"
#include "zend_globals.h"

#ifndef Long
#define Long int32_t
#endif
#ifndef ULong
#define ULong uint32_t
#endif

#undef Bigint
#undef freelist
#undef p5s
#undef dtoa_result

#define Bigint      _zend_strtod_bigint
#define freelist    (EG(strtod_state).freelist)
#define p5s         (EG(strtod_state).p5s)
#define dtoa_result (EG(strtod_state).result)

#ifdef DEBUG
static void Bug(const char *message) {
	fprintf(stderr, "%s\n", message);
}
#define Debug(x) x
static int dtoa_stats[7]; /* strtod_{64,96,bigcomp},dtoa_{exact,64,96,bigcomp} */
#else
#define Debug(x) /*nothing*/
#endif

#include "stdlib.h"
#include "string.h"

#ifdef USE_LOCALE
#include "locale.h"
#endif

#ifdef Honor_FLT_ROUNDS
#ifndef Trust_FLT_ROUNDS
#include <fenv.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef MALLOC
extern void *MALLOC(size_t);
#else
#define MALLOC malloc
#define FREE   free
#endif

#ifdef REALLOC
extern void *REALLOC(void*,size_t);
#else
#define REALLOC realloc
#endif

#ifndef FREE
#define FREE free
#endif

#ifdef __cplusplus
	}
#endif

#ifndef Omit_Private_Memory
#ifndef PRIVATE_MEM
#define PRIVATE_MEM 2304
#endif
#define PRIVATE_mem ((PRIVATE_MEM+sizeof(double)-1)/sizeof(double))
static double private_mem[PRIVATE_mem], *pmem_next = private_mem;
#endif

#undef IEEE_Arith
#undef Avoid_Underflow
#ifdef IEEE_MC68k
#define IEEE_Arith
#endif
#ifdef IEEE_8087
#define IEEE_Arith
#endif

#ifdef IEEE_Arith
#ifndef NO_INFNAN_CHECK
#undef INFNAN_CHECK
#define INFNAN_CHECK
#endif
#else
#undef INFNAN_CHECK
#define NO_STRTOD_BIGCOMP
#endif

#include "errno.h"

#ifdef NO_ERRNO /*{*/
#undef Set_errno
#define Set_errno(x)
#else
#ifndef Set_errno
#define Set_errno(x) errno = x
#endif
#endif /*}*/

#ifdef Bad_float_h

#ifdef IEEE_Arith
#define DBL_DIG 15
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP 1024
#define FLT_RADIX 2
#endif /*IEEE_Arith*/

#ifdef IBM
#define DBL_DIG 16
#define DBL_MAX_10_EXP 75
#define DBL_MAX_EXP 63
#define FLT_RADIX 16
#define DBL_MAX 7.2370055773322621e+75
#endif

#ifdef VAX
#define DBL_DIG 16
#define DBL_MAX_10_EXP 38
#define DBL_MAX_EXP 127
#define FLT_RADIX 2
#define DBL_MAX 1.7014118346046923e+38
#endif

#else /* ifndef Bad_float_h */
#include "float.h"
#endif /* Bad_float_h */

#ifndef __MATH_H__
#include "math.h"
#endif

#if defined(IEEE_8087) + defined(IEEE_MC68k) + defined(VAX) + defined(IBM) != 1
Exactly one of IEEE_8087, IEEE_MC68k, VAX, or IBM should be defined.
#endif

#undef USE_BF96

#ifdef NO_LONG_LONG /*{{*/
#undef ULLong
#ifdef Just_16
#undef Pack_32
/* When Pack_32 is not defined, we store 16 bits per 32-bit Long.
 * This makes some inner loops simpler and sometimes saves work
 * during multiplications, but it often seems to make things slightly
 * slower.  Hence the default is now to store 32 bits per Long.
 */
#endif
#else	/*}{ long long available */
#ifndef Llong
#define Llong long long
#endif
#ifndef ULLong
#define ULLong unsigned Llong
#endif
#ifndef NO_BF96 /*{*/
#define USE_BF96

#ifdef SET_INEXACT
#define dtoa_divmax 27
#else
static const int dtoa_divmax = 2;	/* Permit experimenting: on some systems, 64-bit integer */
			/* division is slow enough that we may sometimes want to */
			/* avoid using it.   We assume (but do not check) that   */
			/* dtoa_divmax <= 27.*/
#endif

typedef struct BF96 {		/* Normalized 96-bit software floating point numbers */
	unsigned int b0,b1,b2;	/* b0 = most significant, binary point just to its left */
	int e;			/* number represented = b * 2^e, with .5 <= b < 1 */
	} BF96;

 static const BF96 pten[667] = {
	{ 0xeef453d6, 0x923bd65a, 0x113faa29, -1136 },
	{ 0x9558b466, 0x1b6565f8, 0x4ac7ca59, -1132 },
	{ 0xbaaee17f, 0xa23ebf76, 0x5d79bcf0, -1129 },
	{ 0xe95a99df, 0x8ace6f53, 0xf4d82c2c, -1126 },
	{ 0x91d8a02b, 0xb6c10594, 0x79071b9b, -1122 },
	{ 0xb64ec836, 0xa47146f9, 0x9748e282, -1119 },
	{ 0xe3e27a44, 0x4d8d98b7, 0xfd1b1b23, -1116 },
	{ 0x8e6d8c6a, 0xb0787f72, 0xfe30f0f5, -1112 },
	{ 0xb208ef85, 0x5c969f4f, 0xbdbd2d33, -1109 },
	{ 0xde8b2b66, 0xb3bc4723, 0xad2c7880, -1106 },
	{ 0x8b16fb20, 0x3055ac76, 0x4c3bcb50, -1102 },
	{ 0xaddcb9e8, 0x3c6b1793, 0xdf4abe24, -1099 },
	{ 0xd953e862, 0x4b85dd78, 0xd71d6dad, -1096 },
	{ 0x87d4713d, 0x6f33aa6b, 0x8672648c, -1092 },
	{ 0xa9c98d8c, 0xcb009506, 0x680efdaf, -1089 },
	{ 0xd43bf0ef, 0xfdc0ba48, 0x0212bd1b, -1086 },
	{ 0x84a57695, 0xfe98746d, 0x014bb630, -1082 },
	{ 0xa5ced43b, 0x7e3e9188, 0x419ea3bd, -1079 },
	{ 0xcf42894a, 0x5dce35ea, 0x52064cac, -1076 },
	{ 0x818995ce, 0x7aa0e1b2, 0x7343efeb, -1072 },
	{ 0xa1ebfb42, 0x19491a1f, 0x1014ebe6, -1069 },
	{ 0xca66fa12, 0x9f9b60a6, 0xd41a26e0, -1066 },
	{ 0xfd00b897, 0x478238d0, 0x8920b098, -1063 },
	{ 0x9e20735e, 0x8cb16382, 0x55b46e5f, -1059 },
	{ 0xc5a89036, 0x2fddbc62, 0xeb2189f7, -1056 },
	{ 0xf712b443, 0xbbd52b7b, 0xa5e9ec75, -1053 },
	{ 0x9a6bb0aa, 0x55653b2d, 0x47b233c9, -1049 },
	{ 0xc1069cd4, 0xeabe89f8, 0x999ec0bb, -1046 },
	{ 0xf148440a, 0x256e2c76, 0xc00670ea, -1043 },
	{ 0x96cd2a86, 0x5764dbca, 0x38040692, -1039 },
	{ 0xbc807527, 0xed3e12bc, 0xc6050837, -1036 },
	{ 0xeba09271, 0xe88d976b, 0xf7864a44, -1033 },
	{ 0x93445b87, 0x31587ea3, 0x7ab3ee6a, -1029 },
	{ 0xb8157268, 0xfdae9e4c, 0x5960ea05, -1026 },
	{ 0xe61acf03, 0x3d1a45df, 0x6fb92487, -1023 },
	{ 0x8fd0c162, 0x06306bab, 0xa5d3b6d4, -1019 },
	{ 0xb3c4f1ba, 0x87bc8696, 0x8f48a489, -1016 },
	{ 0xe0b62e29, 0x29aba83c, 0x331acdab, -1013 },
	{ 0x8c71dcd9, 0xba0b4925, 0x9ff0c08b, -1009 },
	{ 0xaf8e5410, 0x288e1b6f, 0x07ecf0ae, -1006 },
	{ 0xdb71e914, 0x32b1a24a, 0xc9e82cd9, -1003 },
	{ 0x892731ac, 0x9faf056e, 0xbe311c08,  -999 },
	{ 0xab70fe17, 0xc79ac6ca, 0x6dbd630a,  -996 },
	{ 0xd64d3d9d, 0xb981787d, 0x092cbbcc,  -993 },
	{ 0x85f04682, 0x93f0eb4e, 0x25bbf560,  -989 },
	{ 0xa76c5823, 0x38ed2621, 0xaf2af2b8,  -986 },
	{ 0xd1476e2c, 0x07286faa, 0x1af5af66,  -983 },
	{ 0x82cca4db, 0x847945ca, 0x50d98d9f,  -979 },
	{ 0xa37fce12, 0x6597973c, 0xe50ff107,  -976 },
	{ 0xcc5fc196, 0xfefd7d0c, 0x1e53ed49,  -973 },
	{ 0xff77b1fc, 0xbebcdc4f, 0x25e8e89c,  -970 },
	{ 0x9faacf3d, 0xf73609b1, 0x77b19161,  -966 },
	{ 0xc795830d, 0x75038c1d, 0xd59df5b9,  -963 },
	{ 0xf97ae3d0, 0xd2446f25, 0x4b057328,  -960 },
	{ 0x9becce62, 0x836ac577, 0x4ee367f9,  -956 },
	{ 0xc2e801fb, 0x244576d5, 0x229c41f7,  -953 },
	{ 0xf3a20279, 0xed56d48a, 0x6b435275,  -950 },
	{ 0x9845418c, 0x345644d6, 0x830a1389,  -946 },
	{ 0xbe5691ef, 0x416bd60c, 0x23cc986b,  -943 },
	{ 0xedec366b, 0x11c6cb8f, 0x2cbfbe86,  -940 },
	{ 0x94b3a202, 0xeb1c3f39, 0x7bf7d714,  -936 },
	{ 0xb9e08a83, 0xa5e34f07, 0xdaf5ccd9,  -933 },
	{ 0xe858ad24, 0x8f5c22c9, 0xd1b3400f,  -930 },
	{ 0x91376c36, 0xd99995be, 0x23100809,  -926 },
	{ 0xb5854744, 0x8ffffb2d, 0xabd40a0c,  -923 },
	{ 0xe2e69915, 0xb3fff9f9, 0x16c90c8f,  -920 },
	{ 0x8dd01fad, 0x907ffc3b, 0xae3da7d9,  -916 },
	{ 0xb1442798, 0xf49ffb4a, 0x99cd11cf,  -913 },
	{ 0xdd95317f, 0x31c7fa1d, 0x40405643,  -910 },
	{ 0x8a7d3eef, 0x7f1cfc52, 0x482835ea,  -906 },
	{ 0xad1c8eab, 0x5ee43b66, 0xda324365,  -903 },
	{ 0xd863b256, 0x369d4a40, 0x90bed43e,  -900 },
	{ 0x873e4f75, 0xe2224e68, 0x5a7744a6,  -896 },
	{ 0xa90de353, 0x5aaae202, 0x711515d0,  -893 },
	{ 0xd3515c28, 0x31559a83, 0x0d5a5b44,  -890 },
	{ 0x8412d999, 0x1ed58091, 0xe858790a,  -886 },
	{ 0xa5178fff, 0x668ae0b6, 0x626e974d,  -883 },
	{ 0xce5d73ff, 0x402d98e3, 0xfb0a3d21,  -880 },
	{ 0x80fa687f, 0x881c7f8e, 0x7ce66634,  -876 },
	{ 0xa139029f, 0x6a239f72, 0x1c1fffc1,  -873 },
	{ 0xc9874347, 0x44ac874e, 0xa327ffb2,  -870 },
	{ 0xfbe91419, 0x15d7a922, 0x4bf1ff9f,  -867 },
	{ 0x9d71ac8f, 0xada6c9b5, 0x6f773fc3,  -863 },
	{ 0xc4ce17b3, 0x99107c22, 0xcb550fb4,  -860 },
	{ 0xf6019da0, 0x7f549b2b, 0x7e2a53a1,  -857 },
	{ 0x99c10284, 0x4f94e0fb, 0x2eda7444,  -853 },
	{ 0xc0314325, 0x637a1939, 0xfa911155,  -850 },
	{ 0xf03d93ee, 0xbc589f88, 0x793555ab,  -847 },
	{ 0x96267c75, 0x35b763b5, 0x4bc1558b,  -843 },
	{ 0xbbb01b92, 0x83253ca2, 0x9eb1aaed,  -840 },
	{ 0xea9c2277, 0x23ee8bcb, 0x465e15a9,  -837 },
	{ 0x92a1958a, 0x7675175f, 0x0bfacd89,  -833 },
	{ 0xb749faed, 0x14125d36, 0xcef980ec,  -830 },
	{ 0xe51c79a8, 0x5916f484, 0x82b7e127,  -827 },
	{ 0x8f31cc09, 0x37ae58d2, 0xd1b2ecb8,  -823 },
	{ 0xb2fe3f0b, 0x8599ef07, 0x861fa7e6,  -820 },
	{ 0xdfbdcece, 0x67006ac9, 0x67a791e0,  -817 },
	{ 0x8bd6a141, 0x006042bd, 0xe0c8bb2c,  -813 },
	{ 0xaecc4991, 0x4078536d, 0x58fae9f7,  -810 },
	{ 0xda7f5bf5, 0x90966848, 0xaf39a475,  -807 },
	{ 0x888f9979, 0x7a5e012d, 0x6d8406c9,  -803 },
	{ 0xaab37fd7, 0xd8f58178, 0xc8e5087b,  -800 },
	{ 0xd5605fcd, 0xcf32e1d6, 0xfb1e4a9a,  -797 },
	{ 0x855c3be0, 0xa17fcd26, 0x5cf2eea0,  -793 },
	{ 0xa6b34ad8, 0xc9dfc06f, 0xf42faa48,  -790 },
	{ 0xd0601d8e, 0xfc57b08b, 0xf13b94da,  -787 },
	{ 0x823c1279, 0x5db6ce57, 0x76c53d08,  -783 },
	{ 0xa2cb1717, 0xb52481ed, 0x54768c4b,  -780 },
	{ 0xcb7ddcdd, 0xa26da268, 0xa9942f5d,  -777 },
	{ 0xfe5d5415, 0x0b090b02, 0xd3f93b35,  -774 },
	{ 0x9efa548d, 0x26e5a6e1, 0xc47bc501,  -770 },
	{ 0xc6b8e9b0, 0x709f109a, 0x359ab641,  -767 },
	{ 0xf867241c, 0x8cc6d4c0, 0xc30163d2,  -764 },
	{ 0x9b407691, 0xd7fc44f8, 0x79e0de63,  -760 },
	{ 0xc2109436, 0x4dfb5636, 0x985915fc,  -757 },
	{ 0xf294b943, 0xe17a2bc4, 0x3e6f5b7b,  -754 },
	{ 0x979cf3ca, 0x6cec5b5a, 0xa705992c,  -750 },
	{ 0xbd8430bd, 0x08277231, 0x50c6ff78,  -747 },
	{ 0xece53cec, 0x4a314ebd, 0xa4f8bf56,  -744 },
	{ 0x940f4613, 0xae5ed136, 0x871b7795,  -740 },
	{ 0xb9131798, 0x99f68584, 0x28e2557b,  -737 },
	{ 0xe757dd7e, 0xc07426e5, 0x331aeada,  -734 },
	{ 0x9096ea6f, 0x3848984f, 0x3ff0d2c8,  -730 },
	{ 0xb4bca50b, 0x065abe63, 0x0fed077a,  -727 },
	{ 0xe1ebce4d, 0xc7f16dfb, 0xd3e84959,  -724 },
	{ 0x8d3360f0, 0x9cf6e4bd, 0x64712dd7,  -720 },
	{ 0xb080392c, 0xc4349dec, 0xbd8d794d,  -717 },
	{ 0xdca04777, 0xf541c567, 0xecf0d7a0,  -714 },
	{ 0x89e42caa, 0xf9491b60, 0xf41686c4,  -710 },
	{ 0xac5d37d5, 0xb79b6239, 0x311c2875,  -707 },
	{ 0xd77485cb, 0x25823ac7, 0x7d633293,  -704 },
	{ 0x86a8d39e, 0xf77164bc, 0xae5dff9c,  -700 },
	{ 0xa8530886, 0xb54dbdeb, 0xd9f57f83,  -697 },
	{ 0xd267caa8, 0x62a12d66, 0xd072df63,  -694 },
	{ 0x8380dea9, 0x3da4bc60, 0x4247cb9e,  -690 },
	{ 0xa4611653, 0x8d0deb78, 0x52d9be85,  -687 },
	{ 0xcd795be8, 0x70516656, 0x67902e27,  -684 },
	{ 0x806bd971, 0x4632dff6, 0x00ba1cd8,  -680 },
	{ 0xa086cfcd, 0x97bf97f3, 0x80e8a40e,  -677 },
	{ 0xc8a883c0, 0xfdaf7df0, 0x6122cd12,  -674 },
	{ 0xfad2a4b1, 0x3d1b5d6c, 0x796b8057,  -671 },
	{ 0x9cc3a6ee, 0xc6311a63, 0xcbe33036,  -667 },
	{ 0xc3f490aa, 0x77bd60fc, 0xbedbfc44,  -664 },
	{ 0xf4f1b4d5, 0x15acb93b, 0xee92fb55,  -661 },
	{ 0x99171105, 0x2d8bf3c5, 0x751bdd15,  -657 },
	{ 0xbf5cd546, 0x78eef0b6, 0xd262d45a,  -654 },
	{ 0xef340a98, 0x172aace4, 0x86fb8971,  -651 },
	{ 0x9580869f, 0x0e7aac0e, 0xd45d35e6,  -647 },
	{ 0xbae0a846, 0xd2195712, 0x89748360,  -644 },
	{ 0xe998d258, 0x869facd7, 0x2bd1a438,  -641 },
	{ 0x91ff8377, 0x5423cc06, 0x7b6306a3,  -637 },
	{ 0xb67f6455, 0x292cbf08, 0x1a3bc84c,  -634 },
	{ 0xe41f3d6a, 0x7377eeca, 0x20caba5f,  -631 },
	{ 0x8e938662, 0x882af53e, 0x547eb47b,  -627 },
	{ 0xb23867fb, 0x2a35b28d, 0xe99e619a,  -624 },
	{ 0xdec681f9, 0xf4c31f31, 0x6405fa00,  -621 },
	{ 0x8b3c113c, 0x38f9f37e, 0xde83bc40,  -617 },
	{ 0xae0b158b, 0x4738705e, 0x9624ab50,  -614 },
	{ 0xd98ddaee, 0x19068c76, 0x3badd624,  -611 },
	{ 0x87f8a8d4, 0xcfa417c9, 0xe54ca5d7,  -607 },
	{ 0xa9f6d30a, 0x038d1dbc, 0x5e9fcf4c,  -604 },
	{ 0xd47487cc, 0x8470652b, 0x7647c320,  -601 },
	{ 0x84c8d4df, 0xd2c63f3b, 0x29ecd9f4,  -597 },
	{ 0xa5fb0a17, 0xc777cf09, 0xf4681071,  -594 },
	{ 0xcf79cc9d, 0xb955c2cc, 0x7182148d,  -591 },
	{ 0x81ac1fe2, 0x93d599bf, 0xc6f14cd8,  -587 },
	{ 0xa21727db, 0x38cb002f, 0xb8ada00e,  -584 },
	{ 0xca9cf1d2, 0x06fdc03b, 0xa6d90811,  -581 },
	{ 0xfd442e46, 0x88bd304a, 0x908f4a16,  -578 },
	{ 0x9e4a9cec, 0x15763e2e, 0x9a598e4e,  -574 },
	{ 0xc5dd4427, 0x1ad3cdba, 0x40eff1e1,  -571 },
	{ 0xf7549530, 0xe188c128, 0xd12bee59,  -568 },
	{ 0x9a94dd3e, 0x8cf578b9, 0x82bb74f8,  -564 },
	{ 0xc13a148e, 0x3032d6e7, 0xe36a5236,  -561 },
	{ 0xf18899b1, 0xbc3f8ca1, 0xdc44e6c3,  -558 },
	{ 0x96f5600f, 0x15a7b7e5, 0x29ab103a,  -554 },
	{ 0xbcb2b812, 0xdb11a5de, 0x7415d448,  -551 },
	{ 0xebdf6617, 0x91d60f56, 0x111b495b,  -548 },
	{ 0x936b9fce, 0xbb25c995, 0xcab10dd9,  -544 },
	{ 0xb84687c2, 0x69ef3bfb, 0x3d5d514f,  -541 },
	{ 0xe65829b3, 0x046b0afa, 0x0cb4a5a3,  -538 },
	{ 0x8ff71a0f, 0xe2c2e6dc, 0x47f0e785,  -534 },
	{ 0xb3f4e093, 0xdb73a093, 0x59ed2167,  -531 },
	{ 0xe0f218b8, 0xd25088b8, 0x306869c1,  -528 },
	{ 0x8c974f73, 0x83725573, 0x1e414218,  -524 },
	{ 0xafbd2350, 0x644eeacf, 0xe5d1929e,  -521 },
	{ 0xdbac6c24, 0x7d62a583, 0xdf45f746,  -518 },
	{ 0x894bc396, 0xce5da772, 0x6b8bba8c,  -514 },
	{ 0xab9eb47c, 0x81f5114f, 0x066ea92f,  -511 },
	{ 0xd686619b, 0xa27255a2, 0xc80a537b,  -508 },
	{ 0x8613fd01, 0x45877585, 0xbd06742c,  -504 },
	{ 0xa798fc41, 0x96e952e7, 0x2c481138,  -501 },
	{ 0xd17f3b51, 0xfca3a7a0, 0xf75a1586,  -498 },
	{ 0x82ef8513, 0x3de648c4, 0x9a984d73,  -494 },
	{ 0xa3ab6658, 0x0d5fdaf5, 0xc13e60d0,  -491 },
	{ 0xcc963fee, 0x10b7d1b3, 0x318df905,  -488 },
	{ 0xffbbcfe9, 0x94e5c61f, 0xfdf17746,  -485 },
	{ 0x9fd561f1, 0xfd0f9bd3, 0xfeb6ea8b,  -481 },
	{ 0xc7caba6e, 0x7c5382c8, 0xfe64a52e,  -478 },
	{ 0xf9bd690a, 0x1b68637b, 0x3dfdce7a,  -475 },
	{ 0x9c1661a6, 0x51213e2d, 0x06bea10c,  -471 },
	{ 0xc31bfa0f, 0xe5698db8, 0x486e494f,  -468 },
	{ 0xf3e2f893, 0xdec3f126, 0x5a89dba3,  -465 },
	{ 0x986ddb5c, 0x6b3a76b7, 0xf8962946,  -461 },
	{ 0xbe895233, 0x86091465, 0xf6bbb397,  -458 },
	{ 0xee2ba6c0, 0x678b597f, 0x746aa07d,  -455 },
	{ 0x94db4838, 0x40b717ef, 0xa8c2a44e,  -451 },
	{ 0xba121a46, 0x50e4ddeb, 0x92f34d62,  -448 },
	{ 0xe896a0d7, 0xe51e1566, 0x77b020ba,  -445 },
	{ 0x915e2486, 0xef32cd60, 0x0ace1474,  -441 },
	{ 0xb5b5ada8, 0xaaff80b8, 0x0d819992,  -438 },
	{ 0xe3231912, 0xd5bf60e6, 0x10e1fff6,  -435 },
	{ 0x8df5efab, 0xc5979c8f, 0xca8d3ffa,  -431 },
	{ 0xb1736b96, 0xb6fd83b3, 0xbd308ff8,  -428 },
	{ 0xddd0467c, 0x64bce4a0, 0xac7cb3f6,  -425 },
	{ 0x8aa22c0d, 0xbef60ee4, 0x6bcdf07a,  -421 },
	{ 0xad4ab711, 0x2eb3929d, 0x86c16c98,  -418 },
	{ 0xd89d64d5, 0x7a607744, 0xe871c7bf,  -415 },
	{ 0x87625f05, 0x6c7c4a8b, 0x11471cd7,  -411 },
	{ 0xa93af6c6, 0xc79b5d2d, 0xd598e40d,  -408 },
	{ 0xd389b478, 0x79823479, 0x4aff1d10,  -405 },
	{ 0x843610cb, 0x4bf160cb, 0xcedf722a,  -401 },
	{ 0xa54394fe, 0x1eedb8fe, 0xc2974eb4,  -398 },
	{ 0xce947a3d, 0xa6a9273e, 0x733d2262,  -395 },
	{ 0x811ccc66, 0x8829b887, 0x0806357d,  -391 },
	{ 0xa163ff80, 0x2a3426a8, 0xca07c2dc,  -388 },
	{ 0xc9bcff60, 0x34c13052, 0xfc89b393,  -385 },
	{ 0xfc2c3f38, 0x41f17c67, 0xbbac2078,  -382 },
	{ 0x9d9ba783, 0x2936edc0, 0xd54b944b,  -378 },
	{ 0xc5029163, 0xf384a931, 0x0a9e795e,  -375 },
	{ 0xf64335bc, 0xf065d37d, 0x4d4617b5,  -372 },
	{ 0x99ea0196, 0x163fa42e, 0x504bced1,  -368 },
	{ 0xc06481fb, 0x9bcf8d39, 0xe45ec286,  -365 },
	{ 0xf07da27a, 0x82c37088, 0x5d767327,  -362 },
	{ 0x964e858c, 0x91ba2655, 0x3a6a07f8,  -358 },
	{ 0xbbe226ef, 0xb628afea, 0x890489f7,  -355 },
	{ 0xeadab0ab, 0xa3b2dbe5, 0x2b45ac74,  -352 },
	{ 0x92c8ae6b, 0x464fc96f, 0x3b0b8bc9,  -348 },
	{ 0xb77ada06, 0x17e3bbcb, 0x09ce6ebb,  -345 },
	{ 0xe5599087, 0x9ddcaabd, 0xcc420a6a,  -342 },
	{ 0x8f57fa54, 0xc2a9eab6, 0x9fa94682,  -338 },
	{ 0xb32df8e9, 0xf3546564, 0x47939822,  -335 },
	{ 0xdff97724, 0x70297ebd, 0x59787e2b,  -332 },
	{ 0x8bfbea76, 0xc619ef36, 0x57eb4edb,  -328 },
	{ 0xaefae514, 0x77a06b03, 0xede62292,  -325 },
	{ 0xdab99e59, 0x958885c4, 0xe95fab36,  -322 },
	{ 0x88b402f7, 0xfd75539b, 0x11dbcb02,  -318 },
	{ 0xaae103b5, 0xfcd2a881, 0xd652bdc2,  -315 },
	{ 0xd59944a3, 0x7c0752a2, 0x4be76d33,  -312 },
	{ 0x857fcae6, 0x2d8493a5, 0x6f70a440,  -308 },
	{ 0xa6dfbd9f, 0xb8e5b88e, 0xcb4ccd50,  -305 },
	{ 0xd097ad07, 0xa71f26b2, 0x7e2000a4,  -302 },
	{ 0x825ecc24, 0xc873782f, 0x8ed40066,  -298 },
	{ 0xa2f67f2d, 0xfa90563b, 0x72890080,  -295 },
	{ 0xcbb41ef9, 0x79346bca, 0x4f2b40a0,  -292 },
	{ 0xfea126b7, 0xd78186bc, 0xe2f610c8,  -289 },
	{ 0x9f24b832, 0xe6b0f436, 0x0dd9ca7d,  -285 },
	{ 0xc6ede63f, 0xa05d3143, 0x91503d1c,  -282 },
	{ 0xf8a95fcf, 0x88747d94, 0x75a44c63,  -279 },
	{ 0x9b69dbe1, 0xb548ce7c, 0xc986afbe,  -275 },
	{ 0xc24452da, 0x229b021b, 0xfbe85bad,  -272 },
	{ 0xf2d56790, 0xab41c2a2, 0xfae27299,  -269 },
	{ 0x97c560ba, 0x6b0919a5, 0xdccd879f,  -265 },
	{ 0xbdb6b8e9, 0x05cb600f, 0x5400e987,  -262 },
	{ 0xed246723, 0x473e3813, 0x290123e9,  -259 },
	{ 0x9436c076, 0x0c86e30b, 0xf9a0b672,  -255 },
	{ 0xb9447093, 0x8fa89bce, 0xf808e40e,  -252 },
	{ 0xe7958cb8, 0x7392c2c2, 0xb60b1d12,  -249 },
	{ 0x90bd77f3, 0x483bb9b9, 0xb1c6f22b,  -245 },
	{ 0xb4ecd5f0, 0x1a4aa828, 0x1e38aeb6,  -242 },
	{ 0xe2280b6c, 0x20dd5232, 0x25c6da63,  -239 },
	{ 0x8d590723, 0x948a535f, 0x579c487e,  -235 },
	{ 0xb0af48ec, 0x79ace837, 0x2d835a9d,  -232 },
	{ 0xdcdb1b27, 0x98182244, 0xf8e43145,  -229 },
	{ 0x8a08f0f8, 0xbf0f156b, 0x1b8e9ecb,  -225 },
	{ 0xac8b2d36, 0xeed2dac5, 0xe272467e,  -222 },
	{ 0xd7adf884, 0xaa879177, 0x5b0ed81d,  -219 },
	{ 0x86ccbb52, 0xea94baea, 0x98e94712,  -215 },
	{ 0xa87fea27, 0xa539e9a5, 0x3f2398d7,  -212 },
	{ 0xd29fe4b1, 0x8e88640e, 0x8eec7f0d,  -209 },
	{ 0x83a3eeee, 0xf9153e89, 0x1953cf68,  -205 },
	{ 0xa48ceaaa, 0xb75a8e2b, 0x5fa8c342,  -202 },
	{ 0xcdb02555, 0x653131b6, 0x3792f412,  -199 },
	{ 0x808e1755, 0x5f3ebf11, 0xe2bbd88b,  -195 },
	{ 0xa0b19d2a, 0xb70e6ed6, 0x5b6aceae,  -192 },
	{ 0xc8de0475, 0x64d20a8b, 0xf245825a,  -189 },
	{ 0xfb158592, 0xbe068d2e, 0xeed6e2f0,  -186 },
	{ 0x9ced737b, 0xb6c4183d, 0x55464dd6,  -182 },
	{ 0xc428d05a, 0xa4751e4c, 0xaa97e14c,  -179 },
	{ 0xf5330471, 0x4d9265df, 0xd53dd99f,  -176 },
	{ 0x993fe2c6, 0xd07b7fab, 0xe546a803,  -172 },
	{ 0xbf8fdb78, 0x849a5f96, 0xde985204,  -169 },
	{ 0xef73d256, 0xa5c0f77c, 0x963e6685,  -166 },
	{ 0x95a86376, 0x27989aad, 0xdde70013,  -162 },
	{ 0xbb127c53, 0xb17ec159, 0x5560c018,  -159 },
	{ 0xe9d71b68, 0x9dde71af, 0xaab8f01e,  -156 },
	{ 0x92267121, 0x62ab070d, 0xcab39613,  -152 },
	{ 0xb6b00d69, 0xbb55c8d1, 0x3d607b97,  -149 },
	{ 0xe45c10c4, 0x2a2b3b05, 0x8cb89a7d,  -146 },
	{ 0x8eb98a7a, 0x9a5b04e3, 0x77f3608e,  -142 },
	{ 0xb267ed19, 0x40f1c61c, 0x55f038b2,  -139 },
	{ 0xdf01e85f, 0x912e37a3, 0x6b6c46de,  -136 },
	{ 0x8b61313b, 0xbabce2c6, 0x2323ac4b,  -132 },
	{ 0xae397d8a, 0xa96c1b77, 0xabec975e,  -129 },
	{ 0xd9c7dced, 0x53c72255, 0x96e7bd35,  -126 },
	{ 0x881cea14, 0x545c7575, 0x7e50d641,  -122 },
	{ 0xaa242499, 0x697392d2, 0xdde50bd1,  -119 },
	{ 0xd4ad2dbf, 0xc3d07787, 0x955e4ec6,  -116 },
	{ 0x84ec3c97, 0xda624ab4, 0xbd5af13b,  -112 },
	{ 0xa6274bbd, 0xd0fadd61, 0xecb1ad8a,  -109 },
	{ 0xcfb11ead, 0x453994ba, 0x67de18ed,  -106 },
	{ 0x81ceb32c, 0x4b43fcf4, 0x80eacf94,  -102 },
	{ 0xa2425ff7, 0x5e14fc31, 0xa1258379,   -99 },
	{ 0xcad2f7f5, 0x359a3b3e, 0x096ee458,   -96 },
	{ 0xfd87b5f2, 0x8300ca0d, 0x8bca9d6e,   -93 },
	{ 0x9e74d1b7, 0x91e07e48, 0x775ea264,   -89 },
	{ 0xc6120625, 0x76589dda, 0x95364afe,   -86 },
	{ 0xf79687ae, 0xd3eec551, 0x3a83ddbd,   -83 },
	{ 0x9abe14cd, 0x44753b52, 0xc4926a96,   -79 },
	{ 0xc16d9a00, 0x95928a27, 0x75b7053c,   -76 },
	{ 0xf1c90080, 0xbaf72cb1, 0x5324c68b,   -73 },
	{ 0x971da050, 0x74da7bee, 0xd3f6fc16,   -69 },
	{ 0xbce50864, 0x92111aea, 0x88f4bb1c,   -66 },
	{ 0xec1e4a7d, 0xb69561a5, 0x2b31e9e3,   -63 },
	{ 0x9392ee8e, 0x921d5d07, 0x3aff322e,   -59 },
	{ 0xb877aa32, 0x36a4b449, 0x09befeb9,   -56 },
	{ 0xe69594be, 0xc44de15b, 0x4c2ebe68,   -53 },
	{ 0x901d7cf7, 0x3ab0acd9, 0x0f9d3701,   -49 },
	{ 0xb424dc35, 0x095cd80f, 0x538484c1,   -46 },
	{ 0xe12e1342, 0x4bb40e13, 0x2865a5f2,   -43 },
	{ 0x8cbccc09, 0x6f5088cb, 0xf93f87b7,   -39 },
	{ 0xafebff0b, 0xcb24aafe, 0xf78f69a5,   -36 },
	{ 0xdbe6fece, 0xbdedd5be, 0xb573440e,   -33 },
	{ 0x89705f41, 0x36b4a597, 0x31680a88,   -29 },
	{ 0xabcc7711, 0x8461cefc, 0xfdc20d2b,   -26 },
	{ 0xd6bf94d5, 0xe57a42bc, 0x3d329076,   -23 },
	{ 0x8637bd05, 0xaf6c69b5, 0xa63f9a49,   -19 },
	{ 0xa7c5ac47, 0x1b478423, 0x0fcf80dc,   -16 },
	{ 0xd1b71758, 0xe219652b, 0xd3c36113,   -13 },
	{ 0x83126e97, 0x8d4fdf3b, 0x645a1cac,    -9 },
	{ 0xa3d70a3d, 0x70a3d70a, 0x3d70a3d7,    -6 },
	{ 0xcccccccc, 0xcccccccc, 0xcccccccc,    -3 },
	{ 0x80000000, 0x00000000, 0x00000000,    1 },
	{ 0xa0000000, 0x00000000, 0x00000000,    4 },
	{ 0xc8000000, 0x00000000, 0x00000000,    7 },
	{ 0xfa000000, 0x00000000, 0x00000000,   10 },
	{ 0x9c400000, 0x00000000, 0x00000000,   14 },
	{ 0xc3500000, 0x00000000, 0x00000000,   17 },
	{ 0xf4240000, 0x00000000, 0x00000000,   20 },
	{ 0x98968000, 0x00000000, 0x00000000,   24 },
	{ 0xbebc2000, 0x00000000, 0x00000000,   27 },
	{ 0xee6b2800, 0x00000000, 0x00000000,   30 },
	{ 0x9502f900, 0x00000000, 0x00000000,   34 },
	{ 0xba43b740, 0x00000000, 0x00000000,   37 },
	{ 0xe8d4a510, 0x00000000, 0x00000000,   40 },
	{ 0x9184e72a, 0x00000000, 0x00000000,   44 },
	{ 0xb5e620f4, 0x80000000, 0x00000000,   47 },
	{ 0xe35fa931, 0xa0000000, 0x00000000,   50 },
	{ 0x8e1bc9bf, 0x04000000, 0x00000000,   54 },
	{ 0xb1a2bc2e, 0xc5000000, 0x00000000,   57 },
	{ 0xde0b6b3a, 0x76400000, 0x00000000,   60 },
	{ 0x8ac72304, 0x89e80000, 0x00000000,   64 },
	{ 0xad78ebc5, 0xac620000, 0x00000000,   67 },
	{ 0xd8d726b7, 0x177a8000, 0x00000000,   70 },
	{ 0x87867832, 0x6eac9000, 0x00000000,   74 },
	{ 0xa968163f, 0x0a57b400, 0x00000000,   77 },
	{ 0xd3c21bce, 0xcceda100, 0x00000000,   80 },
	{ 0x84595161, 0x401484a0, 0x00000000,   84 },
	{ 0xa56fa5b9, 0x9019a5c8, 0x00000000,   87 },
	{ 0xcecb8f27, 0xf4200f3a, 0x00000000,   90 },
	{ 0x813f3978, 0xf8940984, 0x40000000,   94 },
	{ 0xa18f07d7, 0x36b90be5, 0x50000000,   97 },
	{ 0xc9f2c9cd, 0x04674ede, 0xa4000000,  100 },
	{ 0xfc6f7c40, 0x45812296, 0x4d000000,  103 },
	{ 0x9dc5ada8, 0x2b70b59d, 0xf0200000,  107 },
	{ 0xc5371912, 0x364ce305, 0x6c280000,  110 },
	{ 0xf684df56, 0xc3e01bc6, 0xc7320000,  113 },
	{ 0x9a130b96, 0x3a6c115c, 0x3c7f4000,  117 },
	{ 0xc097ce7b, 0xc90715b3, 0x4b9f1000,  120 },
	{ 0xf0bdc21a, 0xbb48db20, 0x1e86d400,  123 },
	{ 0x96769950, 0xb50d88f4, 0x13144480,  127 },
	{ 0xbc143fa4, 0xe250eb31, 0x17d955a0,  130 },
	{ 0xeb194f8e, 0x1ae525fd, 0x5dcfab08,  133 },
	{ 0x92efd1b8, 0xd0cf37be, 0x5aa1cae5,  137 },
	{ 0xb7abc627, 0x050305ad, 0xf14a3d9e,  140 },
	{ 0xe596b7b0, 0xc643c719, 0x6d9ccd05,  143 },
	{ 0x8f7e32ce, 0x7bea5c6f, 0xe4820023,  147 },
	{ 0xb35dbf82, 0x1ae4f38b, 0xdda2802c,  150 },
	{ 0xe0352f62, 0xa19e306e, 0xd50b2037,  153 },
	{ 0x8c213d9d, 0xa502de45, 0x4526f422,  157 },
	{ 0xaf298d05, 0x0e4395d6, 0x9670b12b,  160 },
	{ 0xdaf3f046, 0x51d47b4c, 0x3c0cdd76,  163 },
	{ 0x88d8762b, 0xf324cd0f, 0xa5880a69,  167 },
	{ 0xab0e93b6, 0xefee0053, 0x8eea0d04,  170 },
	{ 0xd5d238a4, 0xabe98068, 0x72a49045,  173 },
	{ 0x85a36366, 0xeb71f041, 0x47a6da2b,  177 },
	{ 0xa70c3c40, 0xa64e6c51, 0x999090b6,  180 },
	{ 0xd0cf4b50, 0xcfe20765, 0xfff4b4e3,  183 },
	{ 0x82818f12, 0x81ed449f, 0xbff8f10e,  187 },
	{ 0xa321f2d7, 0x226895c7, 0xaff72d52,  190 },
	{ 0xcbea6f8c, 0xeb02bb39, 0x9bf4f8a6,  193 },
	{ 0xfee50b70, 0x25c36a08, 0x02f236d0,  196 },
	{ 0x9f4f2726, 0x179a2245, 0x01d76242,  200 },
	{ 0xc722f0ef, 0x9d80aad6, 0x424d3ad2,  203 },
	{ 0xf8ebad2b, 0x84e0d58b, 0xd2e08987,  206 },
	{ 0x9b934c3b, 0x330c8577, 0x63cc55f4,  210 },
	{ 0xc2781f49, 0xffcfa6d5, 0x3cbf6b71,  213 },
	{ 0xf316271c, 0x7fc3908a, 0x8bef464e,  216 },
	{ 0x97edd871, 0xcfda3a56, 0x97758bf0,  220 },
	{ 0xbde94e8e, 0x43d0c8ec, 0x3d52eeed,  223 },
	{ 0xed63a231, 0xd4c4fb27, 0x4ca7aaa8,  226 },
	{ 0x945e455f, 0x24fb1cf8, 0x8fe8caa9,  230 },
	{ 0xb975d6b6, 0xee39e436, 0xb3e2fd53,  233 },
	{ 0xe7d34c64, 0xa9c85d44, 0x60dbbca8,  236 },
	{ 0x90e40fbe, 0xea1d3a4a, 0xbc8955e9,  240 },
	{ 0xb51d13ae, 0xa4a488dd, 0x6babab63,  243 },
	{ 0xe264589a, 0x4dcdab14, 0xc696963c,  246 },
	{ 0x8d7eb760, 0x70a08aec, 0xfc1e1de5,  250 },
	{ 0xb0de6538, 0x8cc8ada8, 0x3b25a55f,  253 },
	{ 0xdd15fe86, 0xaffad912, 0x49ef0eb7,  256 },
	{ 0x8a2dbf14, 0x2dfcc7ab, 0x6e356932,  260 },
	{ 0xacb92ed9, 0x397bf996, 0x49c2c37f,  263 },
	{ 0xd7e77a8f, 0x87daf7fb, 0xdc33745e,  266 },
	{ 0x86f0ac99, 0xb4e8dafd, 0x69a028bb,  270 },
	{ 0xa8acd7c0, 0x222311bc, 0xc40832ea,  273 },
	{ 0xd2d80db0, 0x2aabd62b, 0xf50a3fa4,  276 },
	{ 0x83c7088e, 0x1aab65db, 0x792667c6,  280 },
	{ 0xa4b8cab1, 0xa1563f52, 0x577001b8,  283 },
	{ 0xcde6fd5e, 0x09abcf26, 0xed4c0226,  286 },
	{ 0x80b05e5a, 0xc60b6178, 0x544f8158,  290 },
	{ 0xa0dc75f1, 0x778e39d6, 0x696361ae,  293 },
	{ 0xc913936d, 0xd571c84c, 0x03bc3a19,  296 },
	{ 0xfb587849, 0x4ace3a5f, 0x04ab48a0,  299 },
	{ 0x9d174b2d, 0xcec0e47b, 0x62eb0d64,  303 },
	{ 0xc45d1df9, 0x42711d9a, 0x3ba5d0bd,  306 },
	{ 0xf5746577, 0x930d6500, 0xca8f44ec,  309 },
	{ 0x9968bf6a, 0xbbe85f20, 0x7e998b13,  313 },
	{ 0xbfc2ef45, 0x6ae276e8, 0x9e3fedd8,  316 },
	{ 0xefb3ab16, 0xc59b14a2, 0xc5cfe94e,  319 },
	{ 0x95d04aee, 0x3b80ece5, 0xbba1f1d1,  323 },
	{ 0xbb445da9, 0xca61281f, 0x2a8a6e45,  326 },
	{ 0xea157514, 0x3cf97226, 0xf52d09d7,  329 },
	{ 0x924d692c, 0xa61be758, 0x593c2626,  333 },
	{ 0xb6e0c377, 0xcfa2e12e, 0x6f8b2fb0,  336 },
	{ 0xe498f455, 0xc38b997a, 0x0b6dfb9c,  339 },
	{ 0x8edf98b5, 0x9a373fec, 0x4724bd41,  343 },
	{ 0xb2977ee3, 0x00c50fe7, 0x58edec91,  346 },
	{ 0xdf3d5e9b, 0xc0f653e1, 0x2f2967b6,  349 },
	{ 0x8b865b21, 0x5899f46c, 0xbd79e0d2,  353 },
	{ 0xae67f1e9, 0xaec07187, 0xecd85906,  356 },
	{ 0xda01ee64, 0x1a708de9, 0xe80e6f48,  359 },
	{ 0x884134fe, 0x908658b2, 0x3109058d,  363 },
	{ 0xaa51823e, 0x34a7eede, 0xbd4b46f0,  366 },
	{ 0xd4e5e2cd, 0xc1d1ea96, 0x6c9e18ac,  369 },
	{ 0x850fadc0, 0x9923329e, 0x03e2cf6b,  373 },
	{ 0xa6539930, 0xbf6bff45, 0x84db8346,  376 },
	{ 0xcfe87f7c, 0xef46ff16, 0xe6126418,  379 },
	{ 0x81f14fae, 0x158c5f6e, 0x4fcb7e8f,  383 },
	{ 0xa26da399, 0x9aef7749, 0xe3be5e33,  386 },
	{ 0xcb090c80, 0x01ab551c, 0x5cadf5bf,  389 },
	{ 0xfdcb4fa0, 0x02162a63, 0x73d9732f,  392 },
	{ 0x9e9f11c4, 0x014dda7e, 0x2867e7fd,  396 },
	{ 0xc646d635, 0x01a1511d, 0xb281e1fd,  399 },
	{ 0xf7d88bc2, 0x4209a565, 0x1f225a7c,  402 },
	{ 0x9ae75759, 0x6946075f, 0x3375788d,  406 },
	{ 0xc1a12d2f, 0xc3978937, 0x0052d6b1,  409 },
	{ 0xf209787b, 0xb47d6b84, 0xc0678c5d,  412 },
	{ 0x9745eb4d, 0x50ce6332, 0xf840b7ba,  416 },
	{ 0xbd176620, 0xa501fbff, 0xb650e5a9,  419 },
	{ 0xec5d3fa8, 0xce427aff, 0xa3e51f13,  422 },
	{ 0x93ba47c9, 0x80e98cdf, 0xc66f336c,  426 },
	{ 0xb8a8d9bb, 0xe123f017, 0xb80b0047,  429 },
	{ 0xe6d3102a, 0xd96cec1d, 0xa60dc059,  432 },
	{ 0x9043ea1a, 0xc7e41392, 0x87c89837,  436 },
	{ 0xb454e4a1, 0x79dd1877, 0x29babe45,  439 },
	{ 0xe16a1dc9, 0xd8545e94, 0xf4296dd6,  442 },
	{ 0x8ce2529e, 0x2734bb1d, 0x1899e4a6,  446 },
	{ 0xb01ae745, 0xb101e9e4, 0x5ec05dcf,  449 },
	{ 0xdc21a117, 0x1d42645d, 0x76707543,  452 },
	{ 0x899504ae, 0x72497eba, 0x6a06494a,  456 },
	{ 0xabfa45da, 0x0edbde69, 0x0487db9d,  459 },
	{ 0xd6f8d750, 0x9292d603, 0x45a9d284,  462 },
	{ 0x865b8692, 0x5b9bc5c2, 0x0b8a2392,  466 },
	{ 0xa7f26836, 0xf282b732, 0x8e6cac77,  469 },
	{ 0xd1ef0244, 0xaf2364ff, 0x3207d795,  472 },
	{ 0x8335616a, 0xed761f1f, 0x7f44e6bd,  476 },
	{ 0xa402b9c5, 0xa8d3a6e7, 0x5f16206c,  479 },
	{ 0xcd036837, 0x130890a1, 0x36dba887,  482 },
	{ 0x80222122, 0x6be55a64, 0xc2494954,  486 },
	{ 0xa02aa96b, 0x06deb0fd, 0xf2db9baa,  489 },
	{ 0xc83553c5, 0xc8965d3d, 0x6f928294,  492 },
	{ 0xfa42a8b7, 0x3abbf48c, 0xcb772339,  495 },
	{ 0x9c69a972, 0x84b578d7, 0xff2a7604,  499 },
	{ 0xc38413cf, 0x25e2d70d, 0xfef51385,  502 },
	{ 0xf46518c2, 0xef5b8cd1, 0x7eb25866,  505 },
	{ 0x98bf2f79, 0xd5993802, 0xef2f773f,  509 },
	{ 0xbeeefb58, 0x4aff8603, 0xaafb550f,  512 },
	{ 0xeeaaba2e, 0x5dbf6784, 0x95ba2a53,  515 },
	{ 0x952ab45c, 0xfa97a0b2, 0xdd945a74,  519 },
	{ 0xba756174, 0x393d88df, 0x94f97111,  522 },
	{ 0xe912b9d1, 0x478ceb17, 0x7a37cd56,  525 },
	{ 0x91abb422, 0xccb812ee, 0xac62e055,  529 },
	{ 0xb616a12b, 0x7fe617aa, 0x577b986b,  532 },
	{ 0xe39c4976, 0x5fdf9d94, 0xed5a7e85,  535 },
	{ 0x8e41ade9, 0xfbebc27d, 0x14588f13,  539 },
	{ 0xb1d21964, 0x7ae6b31c, 0x596eb2d8,  542 },
	{ 0xde469fbd, 0x99a05fe3, 0x6fca5f8e,  545 },
	{ 0x8aec23d6, 0x80043bee, 0x25de7bb9,  549 },
	{ 0xada72ccc, 0x20054ae9, 0xaf561aa7,  552 },
	{ 0xd910f7ff, 0x28069da4, 0x1b2ba151,  555 },
	{ 0x87aa9aff, 0x79042286, 0x90fb44d2,  559 },
	{ 0xa99541bf, 0x57452b28, 0x353a1607,  562 },
	{ 0xd3fa922f, 0x2d1675f2, 0x42889b89,  565 },
	{ 0x847c9b5d, 0x7c2e09b7, 0x69956135,  569 },
	{ 0xa59bc234, 0xdb398c25, 0x43fab983,  572 },
	{ 0xcf02b2c2, 0x1207ef2e, 0x94f967e4,  575 },
	{ 0x8161afb9, 0x4b44f57d, 0x1d1be0ee,  579 },
	{ 0xa1ba1ba7, 0x9e1632dc, 0x6462d92a,  582 },
	{ 0xca28a291, 0x859bbf93, 0x7d7b8f75,  585 },
	{ 0xfcb2cb35, 0xe702af78, 0x5cda7352,  588 },
	{ 0x9defbf01, 0xb061adab, 0x3a088813,  592 },
	{ 0xc56baec2, 0x1c7a1916, 0x088aaa18,  595 },
	{ 0xf6c69a72, 0xa3989f5b, 0x8aad549e,  598 },
	{ 0x9a3c2087, 0xa63f6399, 0x36ac54e2,  602 },
	{ 0xc0cb28a9, 0x8fcf3c7f, 0x84576a1b,  605 },
	{ 0xf0fdf2d3, 0xf3c30b9f, 0x656d44a2,  608 },
	{ 0x969eb7c4, 0x7859e743, 0x9f644ae5,  612 },
	{ 0xbc4665b5, 0x96706114, 0x873d5d9f,  615 },
	{ 0xeb57ff22, 0xfc0c7959, 0xa90cb506,  618 },
	{ 0x9316ff75, 0xdd87cbd8, 0x09a7f124,  622 },
	{ 0xb7dcbf53, 0x54e9bece, 0x0c11ed6d,  625 },
	{ 0xe5d3ef28, 0x2a242e81, 0x8f1668c8,  628 },
	{ 0x8fa47579, 0x1a569d10, 0xf96e017d,  632 },
	{ 0xb38d92d7, 0x60ec4455, 0x37c981dc,  635 },
	{ 0xe070f78d, 0x3927556a, 0x85bbe253,  638 },
	{ 0x8c469ab8, 0x43b89562, 0x93956d74,  642 },
	{ 0xaf584166, 0x54a6babb, 0x387ac8d1,  645 },
	{ 0xdb2e51bf, 0xe9d0696a, 0x06997b05,  648 },
	{ 0x88fcf317, 0xf22241e2, 0x441fece3,  652 },
	{ 0xab3c2fdd, 0xeeaad25a, 0xd527e81c,  655 },
	{ 0xd60b3bd5, 0x6a5586f1, 0x8a71e223,  658 },
	{ 0x85c70565, 0x62757456, 0xf6872d56,  662 },
	{ 0xa738c6be, 0xbb12d16c, 0xb428f8ac,  665 },
	{ 0xd106f86e, 0x69d785c7, 0xe13336d7,  668 },
	{ 0x82a45b45, 0x0226b39c, 0xecc00246,  672 },
	{ 0xa34d7216, 0x42b06084, 0x27f002d7,  675 },
	{ 0xcc20ce9b, 0xd35c78a5, 0x31ec038d,  678 },
	{ 0xff290242, 0xc83396ce, 0x7e670471,  681 },
	{ 0x9f79a169, 0xbd203e41, 0x0f0062c6,  685 },
	{ 0xc75809c4, 0x2c684dd1, 0x52c07b78,  688 },
	{ 0xf92e0c35, 0x37826145, 0xa7709a56,  691 },
	{ 0x9bbcc7a1, 0x42b17ccb, 0x88a66076,  695 },
	{ 0xc2abf989, 0x935ddbfe, 0x6acff893,  698 },
	{ 0xf356f7eb, 0xf83552fe, 0x0583f6b8,  701 },
	{ 0x98165af3, 0x7b2153de, 0xc3727a33,  705 },
	{ 0xbe1bf1b0, 0x59e9a8d6, 0x744f18c0,  708 },
	{ 0xeda2ee1c, 0x7064130c, 0x1162def0,  711 },
	{ 0x9485d4d1, 0xc63e8be7, 0x8addcb56,  715 },
	{ 0xb9a74a06, 0x37ce2ee1, 0x6d953e2b,  718 },
	{ 0xe8111c87, 0xc5c1ba99, 0xc8fa8db6,  721 },
	{ 0x910ab1d4, 0xdb9914a0, 0x1d9c9892,  725 },
	{ 0xb54d5e4a, 0x127f59c8, 0x2503beb6,  728 },
	{ 0xe2a0b5dc, 0x971f303a, 0x2e44ae64,  731 },
	{ 0x8da471a9, 0xde737e24, 0x5ceaecfe,  735 },
	{ 0xb10d8e14, 0x56105dad, 0x7425a83e,  738 },
	{ 0xdd50f199, 0x6b947518, 0xd12f124e,  741 },
	{ 0x8a5296ff, 0xe33cc92f, 0x82bd6b70,  745 },
	{ 0xace73cbf, 0xdc0bfb7b, 0x636cc64d,  748 },
	{ 0xd8210bef, 0xd30efa5a, 0x3c47f7e0,  751 },
	{ 0x8714a775, 0xe3e95c78, 0x65acfaec,  755 },
	{ 0xa8d9d153, 0x5ce3b396, 0x7f1839a7,  758 },
	{ 0xd31045a8, 0x341ca07c, 0x1ede4811,  761 },
	{ 0x83ea2b89, 0x2091e44d, 0x934aed0a,  765 },
	{ 0xa4e4b66b, 0x68b65d60, 0xf81da84d,  768 },
	{ 0xce1de406, 0x42e3f4b9, 0x36251260,  771 },
	{ 0x80d2ae83, 0xe9ce78f3, 0xc1d72b7c,  775 },
	{ 0xa1075a24, 0xe4421730, 0xb24cf65b,  778 },
	{ 0xc94930ae, 0x1d529cfc, 0xdee033f2,  781 },
	{ 0xfb9b7cd9, 0xa4a7443c, 0x169840ef,  784 },
	{ 0x9d412e08, 0x06e88aa5, 0x8e1f2895,  788 },
	{ 0xc491798a, 0x08a2ad4e, 0xf1a6f2ba,  791 },
	{ 0xf5b5d7ec, 0x8acb58a2, 0xae10af69,  794 },
	{ 0x9991a6f3, 0xd6bf1765, 0xacca6da1,  798 },
	{ 0xbff610b0, 0xcc6edd3f, 0x17fd090a,  801 },
	{ 0xeff394dc, 0xff8a948e, 0xddfc4b4c,  804 },
	{ 0x95f83d0a, 0x1fb69cd9, 0x4abdaf10,  808 },
	{ 0xbb764c4c, 0xa7a4440f, 0x9d6d1ad4,  811 },
	{ 0xea53df5f, 0xd18d5513, 0x84c86189,  814 },
	{ 0x92746b9b, 0xe2f8552c, 0x32fd3cf5,  818 },
	{ 0xb7118682, 0xdbb66a77, 0x3fbc8c33,  821 },
	{ 0xe4d5e823, 0x92a40515, 0x0fabaf3f,  824 },
	{ 0x8f05b116, 0x3ba6832d, 0x29cb4d87,  828 },
	{ 0xb2c71d5b, 0xca9023f8, 0x743e20e9,  831 },
	{ 0xdf78e4b2, 0xbd342cf6, 0x914da924,  834 },
	{ 0x8bab8eef, 0xb6409c1a, 0x1ad089b6,  838 },
	{ 0xae9672ab, 0xa3d0c320, 0xa184ac24,  841 },
	{ 0xda3c0f56, 0x8cc4f3e8, 0xc9e5d72d,  844 },
	{ 0x88658996, 0x17fb1871, 0x7e2fa67c,  848 },
	{ 0xaa7eebfb, 0x9df9de8d, 0xddbb901b,  851 },
	{ 0xd51ea6fa, 0x85785631, 0x552a7422,  854 },
	{ 0x8533285c, 0x936b35de, 0xd53a8895,  858 },
	{ 0xa67ff273, 0xb8460356, 0x8a892aba,  861 },
	{ 0xd01fef10, 0xa657842c, 0x2d2b7569,  864 },
	{ 0x8213f56a, 0x67f6b29b, 0x9c3b2962,  868 },
	{ 0xa298f2c5, 0x01f45f42, 0x8349f3ba,  871 },
	{ 0xcb3f2f76, 0x42717713, 0x241c70a9,  874 },
	{ 0xfe0efb53, 0xd30dd4d7, 0xed238cd3,  877 },
	{ 0x9ec95d14, 0x63e8a506, 0xf4363804,  881 },
	{ 0xc67bb459, 0x7ce2ce48, 0xb143c605,  884 },
	{ 0xf81aa16f, 0xdc1b81da, 0xdd94b786,  887 },
	{ 0x9b10a4e5, 0xe9913128, 0xca7cf2b4,  891 },
	{ 0xc1d4ce1f, 0x63f57d72, 0xfd1c2f61,  894 },
	{ 0xf24a01a7, 0x3cf2dccf, 0xbc633b39,  897 },
	{ 0x976e4108, 0x8617ca01, 0xd5be0503,  901 },
	{ 0xbd49d14a, 0xa79dbc82, 0x4b2d8644,  904 },
	{ 0xec9c459d, 0x51852ba2, 0xddf8e7d6,  907 },
	{ 0x93e1ab82, 0x52f33b45, 0xcabb90e5,  911 },
	{ 0xb8da1662, 0xe7b00a17, 0x3d6a751f,  914 },
	{ 0xe7109bfb, 0xa19c0c9d, 0x0cc51267,  917 },
	{ 0x906a617d, 0x450187e2, 0x27fb2b80,  921 },
	{ 0xb484f9dc, 0x9641e9da, 0xb1f9f660,  924 },
	{ 0xe1a63853, 0xbbd26451, 0x5e7873f8,  927 },
	{ 0x8d07e334, 0x55637eb2, 0xdb0b487b,  931 },
	{ 0xb049dc01, 0x6abc5e5f, 0x91ce1a9a,  934 },
	{ 0xdc5c5301, 0xc56b75f7, 0x7641a140,  937 },
	{ 0x89b9b3e1, 0x1b6329ba, 0xa9e904c8,  941 },
	{ 0xac2820d9, 0x623bf429, 0x546345fa,  944 },
	{ 0xd732290f, 0xbacaf133, 0xa97c1779,  947 },
	{ 0x867f59a9, 0xd4bed6c0, 0x49ed8eab,  951 },
	{ 0xa81f3014, 0x49ee8c70, 0x5c68f256,  954 },
	{ 0xd226fc19, 0x5c6a2f8c, 0x73832eec,  957 },
	{ 0x83585d8f, 0xd9c25db7, 0xc831fd53,  961 },
	{ 0xa42e74f3, 0xd032f525, 0xba3e7ca8,  964 },
	{ 0xcd3a1230, 0xc43fb26f, 0x28ce1bd2,  967 },
	{ 0x80444b5e, 0x7aa7cf85, 0x7980d163,  971 },
	{ 0xa0555e36, 0x1951c366, 0xd7e105bc,  974 },
	{ 0xc86ab5c3, 0x9fa63440, 0x8dd9472b,  977 },
	{ 0xfa856334, 0x878fc150, 0xb14f98f6,  980 },
	{ 0x9c935e00, 0xd4b9d8d2, 0x6ed1bf9a,  984 },
	{ 0xc3b83581, 0x09e84f07, 0x0a862f80,  987 },
	{ 0xf4a642e1, 0x4c6262c8, 0xcd27bb61,  990 },
	{ 0x98e7e9cc, 0xcfbd7dbd, 0x8038d51c,  994 },
	{ 0xbf21e440, 0x03acdd2c, 0xe0470a63,  997 },
	{ 0xeeea5d50, 0x04981478, 0x1858ccfc, 1000 },
	{ 0x95527a52, 0x02df0ccb, 0x0f37801e, 1004 },
	{ 0xbaa718e6, 0x8396cffd, 0xd3056025, 1007 },
	{ 0xe950df20, 0x247c83fd, 0x47c6b82e, 1010 },
	{ 0x91d28b74, 0x16cdd27e, 0x4cdc331d, 1014 },
	{ 0xb6472e51, 0x1c81471d, 0xe0133fe4, 1017 },
	{ 0xe3d8f9e5, 0x63a198e5, 0x58180fdd, 1020 },
	{ 0x8e679c2f, 0x5e44ff8f, 0x570f09ea, 1024 },
	{ 0xb201833b, 0x35d63f73, 0x2cd2cc65, 1027 },
	{ 0xde81e40a, 0x034bcf4f, 0xf8077f7e, 1030 },
	{ 0x8b112e86, 0x420f6191, 0xfb04afaf, 1034 },
	{ 0xadd57a27, 0xd29339f6, 0x79c5db9a, 1037 },
	{ 0xd94ad8b1, 0xc7380874, 0x18375281, 1040 },
	{ 0x87cec76f, 0x1c830548, 0x8f229391, 1044 },
	{ 0xa9c2794a, 0xe3a3c69a, 0xb2eb3875, 1047 },
	{ 0xd433179d, 0x9c8cb841, 0x5fa60692, 1050 },
	{ 0x849feec2, 0x81d7f328, 0xdbc7c41b, 1054 },
	{ 0xa5c7ea73, 0x224deff3, 0x12b9b522, 1057 },
	{ 0xcf39e50f, 0xeae16bef, 0xd768226b, 1060 },
	{ 0x81842f29, 0xf2cce375, 0xe6a11583, 1064 },
	{ 0xa1e53af4, 0x6f801c53, 0x60495ae3, 1067 },
	{ 0xca5e89b1, 0x8b602368, 0x385bb19c, 1070 },
	{ 0xfcf62c1d, 0xee382c42, 0x46729e03, 1073 },
	{ 0x9e19db92, 0xb4e31ba9, 0x6c07a2c2, 1077 }
	};
 static const short int Lhint[2098] = {
	   /*18,*/19,    19,    19,    19,    20,    20,    20,    21,    21,
	   21,    22,    22,    22,    23,    23,    23,    23,    24,    24,
	   24,    25,    25,    25,    26,    26,    26,    26,    27,    27,
	   27,    28,    28,    28,    29,    29,    29,    29,    30,    30,
	   30,    31,    31,    31,    32,    32,    32,    32,    33,    33,
	   33,    34,    34,    34,    35,    35,    35,    35,    36,    36,
	   36,    37,    37,    37,    38,    38,    38,    38,    39,    39,
	   39,    40,    40,    40,    41,    41,    41,    41,    42,    42,
	   42,    43,    43,    43,    44,    44,    44,    44,    45,    45,
	   45,    46,    46,    46,    47,    47,    47,    47,    48,    48,
	   48,    49,    49,    49,    50,    50,    50,    51,    51,    51,
	   51,    52,    52,    52,    53,    53,    53,    54,    54,    54,
	   54,    55,    55,    55,    56,    56,    56,    57,    57,    57,
	   57,    58,    58,    58,    59,    59,    59,    60,    60,    60,
	   60,    61,    61,    61,    62,    62,    62,    63,    63,    63,
	   63,    64,    64,    64,    65,    65,    65,    66,    66,    66,
	   66,    67,    67,    67,    68,    68,    68,    69,    69,    69,
	   69,    70,    70,    70,    71,    71,    71,    72,    72,    72,
	   72,    73,    73,    73,    74,    74,    74,    75,    75,    75,
	   75,    76,    76,    76,    77,    77,    77,    78,    78,    78,
	   78,    79,    79,    79,    80,    80,    80,    81,    81,    81,
	   82,    82,    82,    82,    83,    83,    83,    84,    84,    84,
	   85,    85,    85,    85,    86,    86,    86,    87,    87,    87,
	   88,    88,    88,    88,    89,    89,    89,    90,    90,    90,
	   91,    91,    91,    91,    92,    92,    92,    93,    93,    93,
	   94,    94,    94,    94,    95,    95,    95,    96,    96,    96,
	   97,    97,    97,    97,    98,    98,    98,    99,    99,    99,
	  100,   100,   100,   100,   101,   101,   101,   102,   102,   102,
	  103,   103,   103,   103,   104,   104,   104,   105,   105,   105,
	  106,   106,   106,   106,   107,   107,   107,   108,   108,   108,
	  109,   109,   109,   110,   110,   110,   110,   111,   111,   111,
	  112,   112,   112,   113,   113,   113,   113,   114,   114,   114,
	  115,   115,   115,   116,   116,   116,   116,   117,   117,   117,
	  118,   118,   118,   119,   119,   119,   119,   120,   120,   120,
	  121,   121,   121,   122,   122,   122,   122,   123,   123,   123,
	  124,   124,   124,   125,   125,   125,   125,   126,   126,   126,
	  127,   127,   127,   128,   128,   128,   128,   129,   129,   129,
	  130,   130,   130,   131,   131,   131,   131,   132,   132,   132,
	  133,   133,   133,   134,   134,   134,   134,   135,   135,   135,
	  136,   136,   136,   137,   137,   137,   137,   138,   138,   138,
	  139,   139,   139,   140,   140,   140,   141,   141,   141,   141,
	  142,   142,   142,   143,   143,   143,   144,   144,   144,   144,
	  145,   145,   145,   146,   146,   146,   147,   147,   147,   147,
	  148,   148,   148,   149,   149,   149,   150,   150,   150,   150,
	  151,   151,   151,   152,   152,   152,   153,   153,   153,   153,
	  154,   154,   154,   155,   155,   155,   156,   156,   156,   156,
	  157,   157,   157,   158,   158,   158,   159,   159,   159,   159,
	  160,   160,   160,   161,   161,   161,   162,   162,   162,   162,
	  163,   163,   163,   164,   164,   164,   165,   165,   165,   165,
	  166,   166,   166,   167,   167,   167,   168,   168,   168,   169,
	  169,   169,   169,   170,   170,   170,   171,   171,   171,   172,
	  172,   172,   172,   173,   173,   173,   174,   174,   174,   175,
	  175,   175,   175,   176,   176,   176,   177,   177,   177,   178,
	  178,   178,   178,   179,   179,   179,   180,   180,   180,   181,
	  181,   181,   181,   182,   182,   182,   183,   183,   183,   184,
	  184,   184,   184,   185,   185,   185,   186,   186,   186,   187,
	  187,   187,   187,   188,   188,   188,   189,   189,   189,   190,
	  190,   190,   190,   191,   191,   191,   192,   192,   192,   193,
	  193,   193,   193,   194,   194,   194,   195,   195,   195,   196,
	  196,   196,   197,   197,   197,   197,   198,   198,   198,   199,
	  199,   199,   200,   200,   200,   200,   201,   201,   201,   202,
	  202,   202,   203,   203,   203,   203,   204,   204,   204,   205,
	  205,   205,   206,   206,   206,   206,   207,   207,   207,   208,
	  208,   208,   209,   209,   209,   209,   210,   210,   210,   211,
	  211,   211,   212,   212,   212,   212,   213,   213,   213,   214,
	  214,   214,   215,   215,   215,   215,   216,   216,   216,   217,
	  217,   217,   218,   218,   218,   218,   219,   219,   219,   220,
	  220,   220,   221,   221,   221,   221,   222,   222,   222,   223,
	  223,   223,   224,   224,   224,   224,   225,   225,   225,   226,
	  226,   226,   227,   227,   227,   228,   228,   228,   228,   229,
	  229,   229,   230,   230,   230,   231,   231,   231,   231,   232,
	  232,   232,   233,   233,   233,   234,   234,   234,   234,   235,
	  235,   235,   236,   236,   236,   237,   237,   237,   237,   238,
	  238,   238,   239,   239,   239,   240,   240,   240,   240,   241,
	  241,   241,   242,   242,   242,   243,   243,   243,   243,   244,
	  244,   244,   245,   245,   245,   246,   246,   246,   246,   247,
	  247,   247,   248,   248,   248,   249,   249,   249,   249,   250,
	  250,   250,   251,   251,   251,   252,   252,   252,   252,   253,
	  253,   253,   254,   254,   254,   255,   255,   255,   256,   256,
	  256,   256,   257,   257,   257,   258,   258,   258,   259,   259,
	  259,   259,   260,   260,   260,   261,   261,   261,   262,   262,
	  262,   262,   263,   263,   263,   264,   264,   264,   265,   265,
	  265,   265,   266,   266,   266,   267,   267,   267,   268,   268,
	  268,   268,   269,   269,   269,   270,   270,   270,   271,   271,
	  271,   271,   272,   272,   272,   273,   273,   273,   274,   274,
	  274,   274,   275,   275,   275,   276,   276,   276,   277,   277,
	  277,   277,   278,   278,   278,   279,   279,   279,   280,   280,
	  280,   280,   281,   281,   281,   282,   282,   282,   283,   283,
	  283,   283,   284,   284,   284,   285,   285,   285,   286,   286,
	  286,   287,   287,   287,   287,   288,   288,   288,   289,   289,
	  289,   290,   290,   290,   290,   291,   291,   291,   292,   292,
	  292,   293,   293,   293,   293,   294,   294,   294,   295,   295,
	  295,   296,   296,   296,   296,   297,   297,   297,   298,   298,
	  298,   299,   299,   299,   299,   300,   300,   300,   301,   301,
	  301,   302,   302,   302,   302,   303,   303,   303,   304,   304,
	  304,   305,   305,   305,   305,   306,   306,   306,   307,   307,
	  307,   308,   308,   308,   308,   309,   309,   309,   310,   310,
	  310,   311,   311,   311,   311,   312,   312,   312,   313,   313,
	  313,   314,   314,   314,   315,   315,   315,   315,   316,   316,
	  316,   317,   317,   317,   318,   318,   318,   318,   319,   319,
	  319,   320,   320,   320,   321,   321,   321,   321,   322,   322,
	  322,   323,   323,   323,   324,   324,   324,   324,   325,   325,
	  325,   326,   326,   326,   327,   327,   327,   327,   328,   328,
	  328,   329,   329,   329,   330,   330,   330,   330,   331,   331,
	  331,   332,   332,   332,   333,   333,   333,   333,   334,   334,
	  334,   335,   335,   335,   336,   336,   336,   336,   337,   337,
	  337,   338,   338,   338,   339,   339,   339,   339,   340,   340,
	  340,   341,   341,   341,   342,   342,   342,   342,   343,   343,
	  343,   344,   344,   344,   345,   345,   345,   346,   346,   346,
	  346,   347,   347,   347,   348,   348,   348,   349,   349,   349,
	  349,   350,   350,   350,   351,   351,   351,   352,   352,   352,
	  352,   353,   353,   353,   354,   354,   354,   355,   355,   355,
	  355,   356,   356,   356,   357,   357,   357,   358,   358,   358,
	  358,   359,   359,   359,   360,   360,   360,   361,   361,   361,
	  361,   362,   362,   362,   363,   363,   363,   364,   364,   364,
	  364,   365,   365,   365,   366,   366,   366,   367,   367,   367,
	  367,   368,   368,   368,   369,   369,   369,   370,   370,   370,
	  370,   371,   371,   371,   372,   372,   372,   373,   373,   373,
	  374,   374,   374,   374,   375,   375,   375,   376,   376,   376,
	  377,   377,   377,   377,   378,   378,   378,   379,   379,   379,
	  380,   380,   380,   380,   381,   381,   381,   382,   382,   382,
	  383,   383,   383,   383,   384,   384,   384,   385,   385,   385,
	  386,   386,   386,   386,   387,   387,   387,   388,   388,   388,
	  389,   389,   389,   389,   390,   390,   390,   391,   391,   391,
	  392,   392,   392,   392,   393,   393,   393,   394,   394,   394,
	  395,   395,   395,   395,   396,   396,   396,   397,   397,   397,
	  398,   398,   398,   398,   399,   399,   399,   400,   400,   400,
	  401,   401,   401,   402,   402,   402,   402,   403,   403,   403,
	  404,   404,   404,   405,   405,   405,   405,   406,   406,   406,
	  407,   407,   407,   408,   408,   408,   408,   409,   409,   409,
	  410,   410,   410,   411,   411,   411,   411,   412,   412,   412,
	  413,   413,   413,   414,   414,   414,   414,   415,   415,   415,
	  416,   416,   416,   417,   417,   417,   417,   418,   418,   418,
	  419,   419,   419,   420,   420,   420,   420,   421,   421,   421,
	  422,   422,   422,   423,   423,   423,   423,   424,   424,   424,
	  425,   425,   425,   426,   426,   426,   426,   427,   427,   427,
	  428,   428,   428,   429,   429,   429,   429,   430,   430,   430,
	  431,   431,   431,   432,   432,   432,   433,   433,   433,   433,
	  434,   434,   434,   435,   435,   435,   436,   436,   436,   436,
	  437,   437,   437,   438,   438,   438,   439,   439,   439,   439,
	  440,   440,   440,   441,   441,   441,   442,   442,   442,   442,
	  443,   443,   443,   444,   444,   444,   445,   445,   445,   445,
	  446,   446,   446,   447,   447,   447,   448,   448,   448,   448,
	  449,   449,   449,   450,   450,   450,   451,   451,   451,   451,
	  452,   452,   452,   453,   453,   453,   454,   454,   454,   454,
	  455,   455,   455,   456,   456,   456,   457,   457,   457,   457,
	  458,   458,   458,   459,   459,   459,   460,   460,   460,   461,
	  461,   461,   461,   462,   462,   462,   463,   463,   463,   464,
	  464,   464,   464,   465,   465,   465,   466,   466,   466,   467,
	  467,   467,   467,   468,   468,   468,   469,   469,   469,   470,
	  470,   470,   470,   471,   471,   471,   472,   472,   472,   473,
	  473,   473,   473,   474,   474,   474,   475,   475,   475,   476,
	  476,   476,   476,   477,   477,   477,   478,   478,   478,   479,
	  479,   479,   479,   480,   480,   480,   481,   481,   481,   482,
	  482,   482,   482,   483,   483,   483,   484,   484,   484,   485,
	  485,   485,   485,   486,   486,   486,   487,   487,   487,   488,
	  488,   488,   488,   489,   489,   489,   490,   490,   490,   491,
	  491,   491,   492,   492,   492,   492,   493,   493,   493,   494,
	  494,   494,   495,   495,   495,   495,   496,   496,   496,   497,
	  497,   497,   498,   498,   498,   498,   499,   499,   499,   500,
	  500,   500,   501,   501,   501,   501,   502,   502,   502,   503,
	  503,   503,   504,   504,   504,   504,   505,   505,   505,   506,
	  506,   506,   507,   507,   507,   507,   508,   508,   508,   509,
	  509,   509,   510,   510,   510,   510,   511,   511,   511,   512,
	  512,   512,   513,   513,   513,   513,   514,   514,   514,   515,
	  515,   515,   516,   516,   516,   516,   517,   517,   517,   518,
	  518,   518,   519,   519,   519,   520,   520,   520,   520,   521,
	  521,   521,   522,   522,   522,   523,   523,   523,   523,   524,
	  524,   524,   525,   525,   525,   526,   526,   526,   526,   527,
	  527,   527,   528,   528,   528,   529,   529,   529,   529,   530,
	  530,   530,   531,   531,   531,   532,   532,   532,   532,   533,
	  533,   533,   534,   534,   534,   535,   535,   535,   535,   536,
	  536,   536,   537,   537,   537,   538,   538,   538,   538,   539,
	  539,   539,   540,   540,   540,   541,   541,   541,   541,   542,
	  542,   542,   543,   543,   543,   544,   544,   544,   544,   545,
	  545,   545,   546,   546,   546,   547,   547,   547,   548,   548,
	  548,   548,   549,   549,   549,   550,   550,   550,   551,   551,
	  551,   551,   552,   552,   552,   553,   553,   553,   554,   554,
	  554,   554,   555,   555,   555,   556,   556,   556,   557,   557,
	  557,   557,   558,   558,   558,   559,   559,   559,   560,   560,
	  560,   560,   561,   561,   561,   562,   562,   562,   563,   563,
	  563,   563,   564,   564,   564,   565,   565,   565,   566,   566,
	  566,   566,   567,   567,   567,   568,   568,   568,   569,   569,
	  569,   569,   570,   570,   570,   571,   571,   571,   572,   572,
	  572,   572,   573,   573,   573,   574,   574,   574,   575,   575,
	  575,   575,   576,   576,   576,   577,   577,   577,   578,   578,
	  578,   579,   579,   579,   579,   580,   580,   580,   581,   581,
	  581,   582,   582,   582,   582,   583,   583,   583,   584,   584,
	  584,   585,   585,   585,   585,   586,   586,   586,   587,   587,
	  587,   588,   588,   588,   588,   589,   589,   589,   590,   590,
	  590,   591,   591,   591,   591,   592,   592,   592,   593,   593,
	  593,   594,   594,   594,   594,   595,   595,   595,   596,   596,
	  596,   597,   597,   597,   597,   598,   598,   598,   599,   599,
	  599,   600,   600,   600,   600,   601,   601,   601,   602,   602,
	  602,   603,   603,   603,   603,   604,   604,   604,   605,   605,
	  605,   606,   606,   606,   607,   607,   607,   607,   608,   608,
	  608,   609,   609,   609,   610,   610,   610,   610,   611,   611,
	  611,   612,   612,   612,   613,   613,   613,   613,   614,   614,
	  614,   615,   615,   615,   616,   616,   616,   616,   617,   617,
	  617,   618,   618,   618,   619,   619,   619,   619,   620,   620,
	  620,   621,   621,   621,   622,   622,   622,   622,   623,   623,
	  623,   624,   624,   624,   625,   625,   625,   625,   626,   626,
	  626,   627,   627,   627,   628,   628,   628,   628,   629,   629,
	  629,   630,   630,   630,   631,   631,   631,   631,   632,   632,
	  632,   633,   633,   633,   634,   634,   634,   634,   635,   635,
	  635,   636,   636,   636,   637,   637,   637,   638,   638,   638,
	  638,   639,   639,   639,   640,   640,   640,   641,   641,   641,
	  641,   642,   642,   642,   643,   643,   643,   644,   644,   644,
	  644,   645,   645,   645,   646,   646,   646,   647,   647,   647,
	  647,   648,   648,   648,   649,   649,   649,   650,   650 };
 static const ULLong pfive[27] = {
		5ll,
		25ll,
		125ll,
		625ll,
		3125ll,
		15625ll,
		78125ll,
		390625ll,
		1953125ll,
		9765625ll,
		48828125ll,
		244140625ll,
		1220703125ll,
		6103515625ll,
		30517578125ll,
		152587890625ll,
		762939453125ll,
		3814697265625ll,
		19073486328125ll,
		95367431640625ll,
		476837158203125ll,
		2384185791015625ll,
		11920928955078125ll,
		59604644775390625ll,
		298023223876953125ll,
		1490116119384765625ll,
		7450580596923828125ll
		};

 static const int pfivebits[25] = {3, 5, 7, 10, 12, 14, 17, 19, 21, 24, 26, 28, 31,
			     33, 35, 38, 40, 42, 45, 47, 49, 52, 54, 56, 59};
#endif /*}*/
#endif /*}} NO_LONG_LONG */

typedef union { double d; ULong L[2];
#ifdef USE_BF96
	ULLong LL;
#endif
	} U;

#ifdef IEEE_8087
#define word0(x) (x)->L[1]
#define word1(x) (x)->L[0]
#else
#define word0(x) (x)->L[0]
#define word1(x) (x)->L[1]
#endif
#define dval(x) (x)->d
#define LLval(x) (x)->LL

#ifndef STRTOD_DIGLIM
#define STRTOD_DIGLIM 40
#endif

#ifdef DIGLIM_DEBUG
extern int strtod_diglim;
#else
#define strtod_diglim STRTOD_DIGLIM
#endif

/* The following definition of Storeinc is appropriate for MIPS processors.
 * An alternative that might be better on some machines is
 * #define Storeinc(a,b,c) (*a++ = b << 16 | c & 0xffff)
 */
#if defined(IEEE_8087) + defined(VAX) + defined(__arm__)
#define Storeinc(a,b,c) (((unsigned short *)a)[1] = (unsigned short)b, \
((unsigned short *)a)[0] = (unsigned short)c, a++)
#else
#define Storeinc(a,b,c) (((unsigned short *)a)[0] = (unsigned short)b, \
((unsigned short *)a)[1] = (unsigned short)c, a++)
#endif

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#ifdef IEEE_Arith
#define Exp_shift  20
#define Exp_shift1 20
#define Exp_msk1    0x100000
#define Exp_msk11   0x100000
#define Exp_mask  0x7ff00000
#define P 53
#define Nbits 53
#define Bias 1023
#define Emax 1023
#define Emin (-1022)
#define Exp_1  0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask  0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask  0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14
#ifndef NO_IEEE_Scale
#define Avoid_Underflow
#ifdef Flush_Denorm	/* debugging option */
#undef Sudden_Underflow
#endif
#endif

#ifndef Flt_Rounds
#ifdef FLT_ROUNDS
#define Flt_Rounds FLT_ROUNDS
#else
#define Flt_Rounds 1
#endif
#endif /*Flt_Rounds*/

#ifdef Honor_FLT_ROUNDS
#undef Check_FLT_ROUNDS
#define Check_FLT_ROUNDS
#else
/* PHP doesn't define Honor_FLT_ROUNDS, so dtoa() must not depend on the
 * rounding mode. Some compilers evaluate FLT_ROUNDS at run time. */
#define Rounding 1
#endif

#else /* ifndef IEEE_Arith */
#undef Check_FLT_ROUNDS
#undef Honor_FLT_ROUNDS
#undef SET_INEXACT
#undef  Sudden_Underflow
#define Sudden_Underflow
#ifdef IBM
#undef Flt_Rounds
#define Flt_Rounds 0
#define Exp_shift  24
#define Exp_shift1 24
#define Exp_msk1   0x1000000
#define Exp_msk11  0x1000000
#define Exp_mask  0x7f000000
#define P 14
#define Nbits 56
#define Bias 65
#define Emax 248
#define Emin (-260)
#define Exp_1  0x41000000
#define Exp_11 0x41000000
#define Ebits 8	/* exponent has 7 bits, but 8 is the right value in b2d */
#define Frac_mask  0xffffff
#define Frac_mask1 0xffffff
#define Bletch 4
#define Ten_pmax 22
#define Bndry_mask  0xefffff
#define Bndry_mask1 0xffffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 4
#define Tiny0 0x100000
#define Tiny1 0
#define Quick_max 14
#define Int_max 15
#else /* VAX */
#undef Flt_Rounds
#define Flt_Rounds 1
#define Exp_shift  23
#define Exp_shift1 7
#define Exp_msk1    0x80
#define Exp_msk11   0x800000
#define Exp_mask  0x7f80
#define P 56
#define Nbits 56
#define Bias 129
#define Emax 126
#define Emin (-129)
#define Exp_1  0x40800000
#define Exp_11 0x4080
#define Ebits 8
#define Frac_mask  0x7fffff
#define Frac_mask1 0xffff007f
#define Ten_pmax 24
#define Bletch 2
#define Bndry_mask  0xffff007f
#define Bndry_mask1 0xffff007f
#define LSB 0x10000
#define Sign_bit 0x8000
#define Log2P 1
#define Tiny0 0x80
#define Tiny1 0
#define Quick_max 15
#define Int_max 15
#endif /* IBM, VAX */
#endif /* IEEE_Arith */

#ifndef IEEE_Arith
#define ROUND_BIASED
#else
#ifdef ROUND_BIASED_without_Round_Up
#undef  ROUND_BIASED
#define ROUND_BIASED
#endif
#endif

#ifdef RND_PRODQUOT
#define rounded_product(a,b) a = rnd_prod(a, b)
#define rounded_quotient(a,b) a = rnd_quot(a, b)
extern double rnd_prod(double, double), rnd_quot(double, double);
#else
#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1*(DBL_MAX_EXP+Bias-1))
#define Big1 0xffffffff

#ifndef Pack_32
#define Pack_32
#endif

typedef struct BCinfo BCinfo;
 struct
BCinfo { int dp0, dp1, dplen, dsign, e0, inexact, nd, nd0, rounding, scale, uflchk; };

#define FFFFFFFF 0xffffffffUL

#ifdef MULTIPLE_THREADS
#define MTa , PTI
#define MTb , &TI
#define MTd , ThInfo **PTI
static unsigned int maxthreads = 0;
#else
#define MTa /*nothing*/
#define MTb /*nothing*/
#define MTd /*nothing*/
#endif

#define Kmax ZEND_STRTOD_K_MAX

 struct
Bigint {
	struct Bigint *next;
	int k, maxwds, sign, wds;
	ULong x[1];
	};

 typedef struct Bigint Bigint;
 typedef struct
ThInfo {
	Bigint *Freelist[Kmax+1];
	Bigint *P5s;
	} ThInfo;

static void destroy_freelist(void);
static void free_p5s(void);

ZEND_API int zend_shutdown_strtod(void) /* {{{ */
{
	destroy_freelist();
	free_p5s();

	return 1;
}
/* }}} */

#ifndef freelist
 static ThInfo TI0;
#endif

#ifdef MULTIPLE_THREADS
 static ThInfo *TI1;
 static int TI0_used;

 void
set_max_dtoa_threads(unsigned int n)
{
	size_t L;

	if (n > maxthreads) {
		L = n*sizeof(ThInfo);
		if (TI1) {
			TI1 = (ThInfo*)REALLOC(TI1, L);
			memset(TI1 + maxthreads, 0, (n-maxthreads)*sizeof(ThInfo));
			}
		else {
			TI1 = (ThInfo*)MALLOC(L);
			if (TI0_used) {
				memcpy(TI1, &TI0, sizeof(ThInfo));
				if (n > 1)
					memset(TI1 + 1, 0, L - sizeof(ThInfo));
				memset(&TI0, 0, sizeof(ThInfo));
				}
			else
				memset(TI1, 0, L);
			}
		maxthreads = n;
		}
	}

 static ThInfo*
get_TI(void)
{
	unsigned int thno = dtoa_get_threadno();
	if (thno < maxthreads)
		return TI1 + thno;
	if (thno == 0)
		TI0_used = 1;
	return &TI0;
	}
#define freelist TI->Freelist
#define p5s TI->P5s
#else
#ifndef freelist
#define freelist TI0.Freelist
#define p5s TI0.P5s
#endif
#endif

 static Bigint *
Balloc(int k MTd)
{
	int x;
	Bigint *rv;
#ifndef Omit_Private_Memory
	unsigned int len;
#endif
#ifdef MULTIPLE_THREADS
	ThInfo *TI;

	if (!(TI = *PTI))
		*PTI = TI = get_TI();
	if (TI == &TI0)
		ACQUIRE_DTOA_LOCK(0);
#endif
	/* The k > Kmax case does not need ACQUIRE_DTOA_LOCK(0), */
	/* but this case seems very unlikely. */
	if (k <= Kmax && (rv = freelist[k]))
		freelist[k] = rv->next;
	else {
		x = 1 << k;
#ifdef Omit_Private_Memory
		rv = (Bigint *)MALLOC(sizeof(Bigint) + (x-1)*sizeof(ULong));
		if (!rv) {
			zend_mm_unrecoverable_error("Balloc() failed to allocate memory");
		}
#else
		len = (sizeof(Bigint) + (x-1)*sizeof(ULong) + sizeof(double) - 1)
			/sizeof(double);
		if (k <= Kmax && pmem_next - private_mem + len <= PRIVATE_mem
#ifdef MULTIPLE_THREADS
			&& TI == TI1
#endif
			) {
			rv = (Bigint*)pmem_next;
			pmem_next += len;
			}
		else {
			rv = (Bigint*)MALLOC(len*sizeof(double));
			if (!rv) {
				zend_mm_unrecoverable_error("Balloc() failed to allocate memory");
				}
			}
#endif
		rv->k = k;
		rv->maxwds = x;
		}
#ifdef MULTIPLE_THREADS
	if (TI == &TI0)
		FREE_DTOA_LOCK(0);
#endif
	rv->sign = rv->wds = 0;
	rv->next = 0;
	return rv;
	}

 static void
Bfree(Bigint *v MTd)
{
#ifdef MULTIPLE_THREADS
	ThInfo *TI;
#endif
	if (v) {
		if (v->k > Kmax)
			FREE((void*)v);
		else {
#ifdef MULTIPLE_THREADS
			if (!(TI = *PTI))
				*PTI = TI = get_TI();
			if (TI == &TI0)
				ACQUIRE_DTOA_LOCK(0);
#endif
			v->next = freelist[v->k];
			freelist[v->k] = v;
#ifdef MULTIPLE_THREADS
			if (TI == &TI0)
				FREE_DTOA_LOCK(0);
#endif
			}
		}
	}

#define Bcopy(x,y) memcpy((char *)&x->sign, (char *)&y->sign, \
y->wds*sizeof(Long) + 2*sizeof(int))

 static Bigint *
multadd(Bigint *b, int m, int a MTd)	/* multiply by m and add a */
{
	int i, wds;
#ifdef ULLong
	ULong *x;
	ULLong carry, y;
#else
	ULong carry, *x, y;
#ifdef Pack_32
	ULong xi, z;
#endif
#endif
	Bigint *b1;

	wds = b->wds;
	x = b->x;
	i = 0;
	carry = a;
	do {
#ifdef ULLong
		y = *x * (ULLong)m + carry;
		carry = y >> 32;
		*x++ = y & FFFFFFFF;
#else
#ifdef Pack_32
		xi = *x;
		y = (xi & 0xffff) * m + carry;
		z = (xi >> 16) * m + (y >> 16);
		carry = z >> 16;
		*x++ = (z << 16) + (y & 0xffff);
#else
		y = *x * m + carry;
		carry = y >> 16;
		*x++ = y & 0xffff;
#endif
#endif
		}
		while(++i < wds);
	if (carry) {
		if (wds >= b->maxwds) {
			b1 = Balloc(b->k+1 MTa);
			Bcopy(b1, b);
			Bfree(b MTa);
			b = b1;
			}
		b->x[wds++] = carry;
		b->wds = wds;
		}
	return b;
	}

 static Bigint *
s2b(const char *s, int nd0, int nd, ULong y9, int dplen MTd)
{
	Bigint *b;
	int i, k;
	Long x, y;

	x = (nd + 8) / 9;
	for(k = 0, y = 1; x > y; y <<= 1, k++) ;
#ifdef Pack_32
	b = Balloc(k MTa);
	b->x[0] = y9;
	b->wds = 1;
#else
	b = Balloc(k+1 MTa);
	b->x[0] = y9 & 0xffff;
	b->wds = (b->x[1] = y9 >> 16) ? 2 : 1;
#endif

	i = 9;
	if (9 < nd0) {
		s += 9;
		do b = multadd(b, 10, *s++ - '0' MTa);
			while(++i < nd0);
		s += dplen;
		}
	else
		s += dplen + 9;
	for(; i < nd; i++)
		b = multadd(b, 10, *s++ - '0' MTa);
	return b;
	}

 static int
hi0bits(ULong x)
{
	int k = 0;

	if (!(x & 0xffff0000)) {
		k = 16;
		x <<= 16;
		}
	if (!(x & 0xff000000)) {
		k += 8;
		x <<= 8;
		}
	if (!(x & 0xf0000000)) {
		k += 4;
		x <<= 4;
		}
	if (!(x & 0xc0000000)) {
		k += 2;
		x <<= 2;
		}
	if (!(x & 0x80000000)) {
		k++;
		if (!(x & 0x40000000))
			return 32;
		}
	return k;
	}

 static int
lo0bits(ULong *y)
{
	int k;
	ULong x = *y;

	if (x & 7) {
		if (x & 1)
			return 0;
		if (x & 2) {
			*y = x >> 1;
			return 1;
			}
		*y = x >> 2;
		return 2;
		}
	k = 0;
	if (!(x & 0xffff)) {
		k = 16;
		x >>= 16;
		}
	if (!(x & 0xff)) {
		k += 8;
		x >>= 8;
		}
	if (!(x & 0xf)) {
		k += 4;
		x >>= 4;
		}
	if (!(x & 0x3)) {
		k += 2;
		x >>= 2;
		}
	if (!(x & 1)) {
		k++;
		x >>= 1;
		if (!x)
			return 32;
		}
	*y = x;
	return k;
	}

 static Bigint *
i2b(int i MTd)
{
	Bigint *b;

	b = Balloc(1 MTa);
	b->x[0] = i;
	b->wds = 1;
	return b;
	}

 static Bigint *
mult(Bigint *a, Bigint *b MTd)
{
	Bigint *c;
	int k, wa, wb, wc;
	ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
	ULong y;
#ifdef ULLong
	ULLong carry, z;
#else
	ULong carry, z;
#ifdef Pack_32
	ULong z2;
#endif
#endif

	if (a->wds < b->wds) {
		c = a;
		a = b;
		b = c;
		}
	k = a->k;
	wa = a->wds;
	wb = b->wds;
	wc = wa + wb;
	if (wc > a->maxwds)
		k++;
	c = Balloc(k MTa);
	for(x = c->x, xa = x + wc; x < xa; x++)
		*x = 0;
	xa = a->x;
	xae = xa + wa;
	xb = b->x;
	xbe = xb + wb;
	xc0 = c->x;
#ifdef ULLong
	for(; xb < xbe; xc0++) {
		if ((y = *xb++)) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = *x++ * (ULLong)y + *xc + carry;
				carry = z >> 32;
				*xc++ = z & FFFFFFFF;
				}
				while(x < xae);
			*xc = carry;
			}
		}
#else
#ifdef Pack_32
	for(; xb < xbe; xb++, xc0++) {
		if ((y = *xb & 0xffff)) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
				carry = z >> 16;
				z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
				carry = z2 >> 16;
				Storeinc(xc, z2, z);
				}
				while(x < xae);
			*xc = carry;
			}
		if ((y = *xb >> 16)) {
			x = xa;
			xc = xc0;
			carry = 0;
			z2 = *xc;
			do {
				z = (*x & 0xffff) * y + (*xc >> 16) + carry;
				carry = z >> 16;
				Storeinc(xc, z, z2);
				z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
				carry = z2 >> 16;
				}
				while(x < xae);
			*xc = z2;
			}
		}
#else
	for(; xb < xbe; xc0++) {
		if (y = *xb++) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = *x++ * y + *xc + carry;
				carry = z >> 16;
				*xc++ = z & 0xffff;
				}
				while(x < xae);
			*xc = carry;
			}
		}
#endif
#endif
	for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
	c->wds = wc;
	return c;
	}

 static Bigint *
pow5mult(Bigint *b, int k MTd)
{
	Bigint *b1, *p5, *p51;
#ifdef MULTIPLE_THREADS
	ThInfo *TI;
#endif
	int i;
	static const int p05[3] = { 5, 25, 125 };

	if ((i = k & 3))
		b = multadd(b, p05[i-1], 0 MTa);

	if (!(k >>= 2))
		return b;
#ifdef  MULTIPLE_THREADS
	if (!(TI = *PTI))
		*PTI = TI = get_TI();
#endif
	if (!(p5 = p5s)) {
		/* first time */
#ifdef MULTIPLE_THREADS
		if (!(TI = *PTI))
			*PTI = TI = get_TI();
		if (TI == &TI0)
			ACQUIRE_DTOA_LOCK(1);
		if (!(p5 = p5s))
			p5 = p5s = i2b(625 MTa);
		if (TI == &TI0)
			FREE_DTOA_LOCK(1);
#else
		p5 = p5s = i2b(625 MTa);
#endif
		}
	for(;;) {
		if (k & 1) {
			b1 = mult(b, p5 MTa);
			Bfree(b MTa);
			b = b1;
			}
		if (!(k >>= 1))
			break;
		if (!(p51 = p5->next)) {
#ifdef MULTIPLE_THREADS
			if (!TI && !(TI = *PTI))
				*PTI = TI = get_TI();
			if (TI == &TI0)
				ACQUIRE_DTOA_LOCK(1);
			if (!(p51 = p5->next))
				p51 = p5->next = mult(p5,p5 MTa);
			if (TI == &TI0)
				FREE_DTOA_LOCK(1);
#else
			p51 = p5->next = mult(p5,p5);
#endif
			}
		p5 = p51;
		}
	return b;
	}

 static Bigint *
lshift(Bigint *b, int k MTd)
{
	int i, k1, n, n1;
	Bigint *b1;
	ULong *x, *x1, *xe, z;

#ifdef Pack_32
	n = k >> 5;
#else
	n = k >> 4;
#endif
	k1 = b->k;
	n1 = n + b->wds + 1;
	for(i = b->maxwds; n1 > i; i <<= 1)
		k1++;
	b1 = Balloc(k1 MTa);
	x1 = b1->x;
	for(i = 0; i < n; i++)
		*x1++ = 0;
	x = b->x;
	xe = x + b->wds;
#ifdef Pack_32
	if (k &= 0x1f) {
		k1 = 32 - k;
		z = 0;
		do {
			*x1++ = *x << k | z;
			z = *x++ >> k1;
			}
			while(x < xe);
		if ((*x1 = z))
			++n1;
		}
#else
	if (k &= 0xf) {
		k1 = 16 - k;
		z = 0;
		do {
			*x1++ = *x << k  & 0xffff | z;
			z = *x++ >> k1;
			}
			while(x < xe);
		if (*x1 = z)
			++n1;
		}
#endif
	else do
		*x1++ = *x++;
		while(x < xe);
	b1->wds = n1 - 1;
	Bfree(b MTa);
	return b1;
	}

 static int
cmp(Bigint *a, Bigint *b)
{
	ULong *xa, *xa0, *xb, *xb0;
	int i, j;

	i = a->wds;
	j = b->wds;
#ifdef DEBUG
	if (i > 1 && !a->x[i-1])
		Bug("cmp called with a->x[a->wds-1] == 0");
	if (j > 1 && !b->x[j-1])
		Bug("cmp called with b->x[b->wds-1] == 0");
#endif
	if (i -= j)
		return i;
	xa0 = a->x;
	xa = xa0 + j;
	xb0 = b->x;
	xb = xb0 + j;
	for(;;) {
		if (*--xa != *--xb)
			return *xa < *xb ? -1 : 1;
		if (xa <= xa0)
			break;
		}
	return 0;
	}

 static Bigint *
diff(Bigint *a, Bigint *b MTd)
{
	Bigint *c;
	int i, wa, wb;
	ULong *xa, *xae, *xb, *xbe, *xc;
#ifdef ULLong
	ULLong borrow, y;
#else
	ULong borrow, y;
#ifdef Pack_32
	ULong z;
#endif
#endif

	i = cmp(a,b);
	if (!i) {
		c = Balloc(0 MTa);
		c->wds = 1;
		c->x[0] = 0;
		return c;
		}
	if (i < 0) {
		c = a;
		a = b;
		b = c;
		i = 1;
		}
	else
		i = 0;
	c = Balloc(a->k MTa);
	c->sign = i;
	wa = a->wds;
	xa = a->x;
	xae = xa + wa;
	wb = b->wds;
	xb = b->x;
	xbe = xb + wb;
	xc = c->x;
	borrow = 0;
#ifdef ULLong
	do {
		y = (ULLong)*xa++ - *xb++ - borrow;
		borrow = y >> 32 & (ULong)1;
		*xc++ = y & FFFFFFFF;
		}
		while(xb < xbe);
	while(xa < xae) {
		y = *xa++ - borrow;
		borrow = y >> 32 & (ULong)1;
		*xc++ = y & FFFFFFFF;
		}
#else
#ifdef Pack_32
	do {
		y = (*xa & 0xffff) - (*xb & 0xffff) - borrow;
		borrow = (y & 0x10000) >> 16;
		z = (*xa++ >> 16) - (*xb++ >> 16) - borrow;
		borrow = (z & 0x10000) >> 16;
		Storeinc(xc, z, y);
		}
		while(xb < xbe);
	while(xa < xae) {
		y = (*xa & 0xffff) - borrow;
		borrow = (y & 0x10000) >> 16;
		z = (*xa++ >> 16) - borrow;
		borrow = (z & 0x10000) >> 16;
		Storeinc(xc, z, y);
		}
#else
	do {
		y = *xa++ - *xb++ - borrow;
		borrow = (y & 0x10000) >> 16;
		*xc++ = y & 0xffff;
		}
		while(xb < xbe);
	while(xa < xae) {
		y = *xa++ - borrow;
		borrow = (y & 0x10000) >> 16;
		*xc++ = y & 0xffff;
		}
#endif
#endif
	while(!*--xc)
		wa--;
	c->wds = wa;
	return c;
	}

 static double
ulp(U *x)
{
	Long L;
	U u;

	L = (word0(x) & Exp_mask) - (P-1)*Exp_msk1;
#ifndef Avoid_Underflow
#ifndef Sudden_Underflow
	if (L > 0) {
#endif
#endif
#ifdef IBM
		L |= Exp_msk1 >> 4;
#endif
		word0(&u) = L;
		word1(&u) = 0;
#ifndef Avoid_Underflow
#ifndef Sudden_Underflow
		}
	else {
		L = -L >> Exp_shift;
		if (L < Exp_shift) {
			word0(&u) = 0x80000 >> L;
			word1(&u) = 0;
			}
		else {
			word0(&u) = 0;
			L -= Exp_shift;
			word1(&u) = L >= 31 ? 1 : 1 << 31 - L;
			}
		}
#endif
#endif
	return dval(&u);
	}

 static double
b2d(Bigint *a, int *e)
{
	ULong *xa, *xa0, w, y, z;
	int k;
	U d;
#ifdef VAX
	ULong d0, d1;
#else
#define d0 word0(&d)
#define d1 word1(&d)
#endif

	xa0 = a->x;
	xa = xa0 + a->wds;
	y = *--xa;
#ifdef DEBUG
	if (!y) Bug("zero y in b2d");
#endif
	k = hi0bits(y);
	*e = 32 - k;
#ifdef Pack_32
	if (k < Ebits) {
		d0 = Exp_1 | y >> (Ebits - k);
		w = xa > xa0 ? *--xa : 0;
		d1 = y << ((32-Ebits) + k) | w >> (Ebits - k);
		goto ret_d;
		}
	z = xa > xa0 ? *--xa : 0;
	if (k -= Ebits) {
		d0 = Exp_1 | y << k | z >> (32 - k);
		y = xa > xa0 ? *--xa : 0;
		d1 = z << k | y >> (32 - k);
		}
	else {
		d0 = Exp_1 | y;
		d1 = z;
		}
#else
	if (k < Ebits + 16) {
		z = xa > xa0 ? *--xa : 0;
		d0 = Exp_1 | y << k - Ebits | z >> Ebits + 16 - k;
		w = xa > xa0 ? *--xa : 0;
		y = xa > xa0 ? *--xa : 0;
		d1 = z << k + 16 - Ebits | w << k - Ebits | y >> 16 + Ebits - k;
		goto ret_d;
		}
	z = xa > xa0 ? *--xa : 0;
	w = xa > xa0 ? *--xa : 0;
	k -= Ebits + 16;
	d0 = Exp_1 | y << k + 16 | z << k | w >> 16 - k;
	y = xa > xa0 ? *--xa : 0;
	d1 = w << k + 16 | y << k;
#endif
 ret_d:
#ifdef VAX
	word0(&d) = d0 >> 16 | d0 << 16;
	word1(&d) = d1 >> 16 | d1 << 16;
#else
#undef d0
#undef d1
#endif
	return dval(&d);
	}

 static Bigint *
d2b(U *d, int *e, int *bits MTd)
{
	Bigint *b;
	int de, k;
	ULong *x, y, z;
#ifndef Sudden_Underflow
	int i;
#endif
#ifdef VAX
	ULong d0, d1;
	d0 = word0(d) >> 16 | word0(d) << 16;
	d1 = word1(d) >> 16 | word1(d) << 16;
#else
#define d0 word0(d)
#define d1 word1(d)
#endif

#ifdef Pack_32
	b = Balloc(1 MTa);
#else
	b = Balloc(2 MTa);
#endif
	x = b->x;

	z = d0 & Frac_mask;
	d0 &= 0x7fffffff;	/* clear sign bit, which we ignore */
#ifdef Sudden_Underflow
	de = (int)(d0 >> Exp_shift);
#ifndef IBM
	z |= Exp_msk11;
#endif
#else
	if ((de = (int)(d0 >> Exp_shift)))
		z |= Exp_msk1;
#endif
#ifdef Pack_32
	if ((y = d1)) {
		if ((k = lo0bits(&y))) {
			x[0] = y | z << (32 - k);
			z >>= k;
			}
		else
			x[0] = y;
#ifndef Sudden_Underflow
		i =
#endif
		    b->wds = (x[1] = z) ? 2 : 1;
		}
	else {
		k = lo0bits(&z);
		x[0] = z;
#ifndef Sudden_Underflow
		i =
#endif
		    b->wds = 1;
		k += 32;
		}
#else
	if (y = d1) {
		if (k = lo0bits(&y))
			if (k >= 16) {
				x[0] = y | z << 32 - k & 0xffff;
				x[1] = z >> k - 16 & 0xffff;
				x[2] = z >> k;
				i = 2;
				}
			else {
				x[0] = y & 0xffff;
				x[1] = y >> 16 | z << 16 - k & 0xffff;
				x[2] = z >> k & 0xffff;
				x[3] = z >> k+16;
				i = 3;
				}
		else {
			x[0] = y & 0xffff;
			x[1] = y >> 16;
			x[2] = z & 0xffff;
			x[3] = z >> 16;
			i = 3;
			}
		}
	else {
#ifdef DEBUG
		if (!z)
			Bug("Zero passed to d2b");
#endif
		k = lo0bits(&z);
		if (k >= 16) {
			x[0] = z;
			i = 0;
			}
		else {
			x[0] = z & 0xffff;
			x[1] = z >> 16;
			i = 1;
			}
		k += 32;
		}
	while(!x[i])
		--i;
	b->wds = i + 1;
#endif
#ifndef Sudden_Underflow
	if (de) {
#endif
#ifdef IBM
		*e = (de - Bias - (P-1) << 2) + k;
		*bits = 4*P + 8 - k - hi0bits(word0(d) & Frac_mask);
#else
		*e = de - Bias - (P-1) + k;
		*bits = P - k;
#endif
#ifndef Sudden_Underflow
		}
	else {
		*e = de - Bias - (P-1) + 1 + k;
#ifdef Pack_32
		*bits = 32*i - hi0bits(x[i-1]);
#else
		*bits = (i+2)*16 - hi0bits(x[i]);
#endif
		}
#endif
	return b;
	}
#undef d0
#undef d1

 static double
ratio(Bigint *a, Bigint *b)
{
	U da, db;
	int k, ka, kb;

	dval(&da) = b2d(a, &ka);
	dval(&db) = b2d(b, &kb);
#ifdef Pack_32
	k = ka - kb + 32*(a->wds - b->wds);
#else
	k = ka - kb + 16*(a->wds - b->wds);
#endif
#ifdef IBM
	if (k > 0) {
		word0(&da) += (k >> 2)*Exp_msk1;
		if (k &= 3)
			dval(&da) *= 1 << k;
		}
	else {
		k = -k;
		word0(&db) += (k >> 2)*Exp_msk1;
		if (k &= 3)
			dval(&db) *= 1 << k;
		}
#else
	if (k > 0)
		word0(&da) += k*Exp_msk1;
	else {
		k = -k;
		word0(&db) += k*Exp_msk1;
		}
#endif
	return dval(&da) / dval(&db);
	}

 static const double
tens[] = {
		1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
		1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
		1e20, 1e21, 1e22
#ifdef VAX
		, 1e23, 1e24
#endif
		};

 static const double
#ifdef IEEE_Arith
bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };
static const double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128,
#ifdef Avoid_Underflow
		9007199254740992.*9007199254740992.e-256
		/* = 2^106 * 1e-256 */
#else
		1e-256
#endif
		};
/* The factor of 2^53 in tinytens[4] helps us avoid setting the underflow */
/* flag unnecessarily.  It leads to a song and dance at the end of strtod. */
#define Scale_Bit 0x10
#define n_bigtens 5
#else
#ifdef IBM
bigtens[] = { 1e16, 1e32, 1e64 };
static const double tinytens[] = { 1e-16, 1e-32, 1e-64 };
#define n_bigtens 3
#else
bigtens[] = { 1e16, 1e32 };
static const double tinytens[] = { 1e-16, 1e-32 };
#define n_bigtens 2
#endif
#endif

#undef Need_Hexdig
#ifdef INFNAN_CHECK
#ifndef No_Hex_NaN
#define Need_Hexdig
#endif
#endif

#ifndef Need_Hexdig
#ifndef NO_HEX_FP
#define Need_Hexdig
#endif
#endif

#ifdef Need_Hexdig /*{*/
#if 0
static unsigned char hexdig[256];

 static void
htinit(unsigned char *h, unsigned char *s, int inc)
{
	int i, j;
	for(i = 0; (j = s[i]) !=0; i++)
		h[j] = i + inc;
	}

 static void
hexdig_init(void)	/* Use of hexdig_init omitted 20121220 to avoid a */
			/* race condition when multiple threads are used. */
{
#define USC (unsigned char *)
	htinit(hexdig, USC "0123456789", 0x10);
	htinit(hexdig, USC "abcdef", 0x10 + 10);
	htinit(hexdig, USC "ABCDEF", 0x10 + 10);
	}
#else
static const unsigned char hexdig[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0,
	0,26,27,28,29,30,31,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,26,27,28,29,30,31,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
#endif
#endif /* } Need_Hexdig */

#ifdef INFNAN_CHECK

#ifndef NAN_WORD0
#define NAN_WORD0 0x7ff80000
#endif

#ifndef NAN_WORD1
#define NAN_WORD1 0
#endif

 static int
match(const char **sp, const char *t)
{
	int c, d;
	const char *s = *sp;

	while((d = *t++)) {
		if ((c = *++s) >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		if (c != d)
			return 0;
		}
	*sp = s + 1;
	return 1;
	}

#ifndef No_Hex_NaN
 static void
hexnan(U *rvp, const char **sp)
{
	ULong c, x[2];
	const char *s;
	int c1, havedig, udx0, xshift;

	/**** if (!hexdig['0']) hexdig_init(); ****/
	x[0] = x[1] = 0;
	havedig = xshift = 0;
	udx0 = 1;
	s = *sp;
	/* allow optional initial 0x or 0X */
	while((c = *(const unsigned char*)(s+1)) && c <= ' ')
		++s;
	if (s[1] == '0' && (s[2] == 'x' || s[2] == 'X'))
		s += 2;
	while((c = *(const unsigned char*)++s)) {
		if ((c1 = hexdig[c]))
			c  = c1 & 0xf;
		else if (c <= ' ') {
			if (udx0 && havedig) {
				udx0 = 0;
				xshift = 1;
				}
			continue;
			}
#ifdef GDTOA_NON_PEDANTIC_NANCHECK
		else if (/*(*/ c == ')' && havedig) {
			*sp = s + 1;
			break;
			}
		else
			return;	/* invalid form: don't change *sp */
#else
		else {
			do {
				if (/*(*/ c == ')') {
					*sp = s + 1;
					break;
					}
				} while((c = *++s));
			break;
			}
#endif
		havedig = 1;
		if (xshift) {
			xshift = 0;
			x[0] = x[1];
			x[1] = 0;
			}
		if (udx0)
			x[0] = (x[0] << 4) | (x[1] >> 28);
		x[1] = (x[1] << 4) | c;
		}
	if ((x[0] &= 0xfffff) || x[1]) {
		word0(rvp) = Exp_mask | x[0];
		word1(rvp) = x[1];
		}
	}
#endif /*No_Hex_NaN*/
#endif /* INFNAN_CHECK */

#ifdef Pack_32
#define ULbits 32
#define kshift 5
#define kmask 31
#else
#define ULbits 16
#define kshift 4
#define kmask 15
#endif

#if !defined(NO_HEX_FP) || defined(Honor_FLT_ROUNDS) /*{*/
 static Bigint *
increment(Bigint *b MTd)
{
	ULong *x, *xe;
	Bigint *b1;

	x = b->x;
	xe = x + b->wds;
	do {
		if (*x < (ULong)0xffffffffL) {
			++*x;
			return b;
			}
		*x++ = 0;
		} while(x < xe);
	{
		if (b->wds >= b->maxwds) {
			b1 = Balloc(b->k+1 MTa);
			Bcopy(b1,b);
			Bfree(b MTa);
			b = b1;
			}
		b->x[b->wds++] = 1;
		}
	return b;
	}

#endif /*}*/

#ifndef NO_HEX_FP /*{*/

 static void
rshift(Bigint *b, int k)
{
	ULong *x, *x1, *xe, y;
	int n;

	x = x1 = b->x;
	n = k >> kshift;
	if (n < b->wds) {
		xe = x + b->wds;
		x += n;
		if (k &= kmask) {
			n = 32 - k;
			y = *x++ >> k;
			while(x < xe) {
				*x1++ = (y | (*x << n)) & 0xffffffff;
				y = *x++ >> k;
				}
			if ((*x1 = y) !=0)
				x1++;
			}
		else
			while(x < xe)
				*x1++ = *x++;
		}
	if ((b->wds = x1 - b->x) == 0)
		b->x[0] = 0;
	}

 static ULong
any_on(Bigint *b, int k)
{
	int n, nwds;
	ULong *x, *x0, x1, x2;

	x = b->x;
	nwds = b->wds;
	n = k >> kshift;
	if (n > nwds)
		n = nwds;
	else if (n < nwds && (k &= kmask)) {
		x1 = x2 = x[n];
		x1 >>= k;
		x1 <<= k;
		if (x1 != x2)
			return 1;
		}
	x0 = x;
	x += n;
	while(x > x0)
		if (*--x)
			return 1;
	return 0;
	}

enum {	/* rounding values: same as FLT_ROUNDS */
	Round_zero = 0,
	Round_near = 1,
	Round_up = 2,
	Round_down = 3
	};

 void
gethex(const char **sp, U *rvp, int rounding, int sign MTd)
{
	Bigint *b;
	char d;
	const unsigned char *decpt, *s0, *s, *s1;
	Long e, e1;
	ULong L, lostbits, *x;
	int big, denorm, esign, havedig, k, n, nb, nbits, nz, up, zret;
#ifdef IBM
	int j;
#endif
	enum {
#ifdef IEEE_Arith /*{{*/
		emax = 0x7fe - Bias - P + 1,
		emin = Emin - P + 1
#else /*}{*/
		emin = Emin - P,
#ifdef VAX
		emax = 0x7ff - Bias - P + 1
#endif
#ifdef IBM
		emax = 0x7f - Bias - P
#endif
#endif /*}}*/
		};
#ifdef IEEE_Arith
	int check_denorm = 0;
#endif
#ifdef USE_LOCALE
	int i;
#ifdef NO_LOCALE_CACHE
	const unsigned char *decimalpoint = (unsigned char*)
		localeconv()->decimal_point;
#else
	const unsigned char *decimalpoint;
	static unsigned char *decimalpoint_cache;
	if (!(s0 = decimalpoint_cache)) {
		s0 = (unsigned char*)localeconv()->decimal_point;
		if ((decimalpoint_cache = (unsigned char*)
				MALLOC(strlen((const char*)s0) + 1))) {
			strcpy((char*)decimalpoint_cache, (const char*)s0);
			s0 = decimalpoint_cache;
			}
		}
	decimalpoint = s0;
#endif
#endif

	/**** if (!hexdig['0']) hexdig_init(); ****/
	havedig = 0;
	s0 = *(const unsigned char **)sp + 2;
	while(s0[havedig] == '0')
		havedig++;
	s0 += havedig;
	s = s0;
	decpt = 0;
	zret = 0;
	e = 0;
	if (hexdig[*s])
		havedig++;
	else {
		zret = 1;
#ifdef USE_LOCALE
		for(i = 0; decimalpoint[i]; ++i) {
			if (s[i] != decimalpoint[i])
				goto pcheck;
			}
		decpt = s += i;
#else
		if (*s != '.')
			goto pcheck;
		decpt = ++s;
#endif
		if (!hexdig[*s])
			goto pcheck;
		while(*s == '0')
			s++;
		if (hexdig[*s])
			zret = 0;
		havedig = 1;
		s0 = s;
		}
	while(hexdig[*s])
		s++;
#ifdef USE_LOCALE
	if (*s == *decimalpoint && !decpt) {
		for(i = 1; decimalpoint[i]; ++i) {
			if (s[i] != decimalpoint[i])
				goto pcheck;
			}
		decpt = s += i;
#else
	if (*s == '.' && !decpt) {
		decpt = ++s;
#endif
		while(hexdig[*s])
			s++;
		}/*}*/
	if (decpt)
		e = -(((Long)(s-decpt)) << 2);
 pcheck:
	s1 = s;
	big = esign = 0;
	switch(*s) {
	  case 'p':
	  case 'P':
		switch(*++s) {
		  case '-':
			esign = 1;
			ZEND_FALLTHROUGH;
		  case '+':
			s++;
		  }
		if ((n = hexdig[*s]) == 0 || n > 0x19) {
			s = s1;
			break;
			}
		e1 = n - 0x10;
		while((n = hexdig[*++s]) !=0 && n <= 0x19) {
			if (e1 & 0xf8000000)
				big = 1;
			e1 = 10*e1 + n - 0x10;
			}
		if (esign)
			e1 = -e1;
		e += e1;
	  }
	*sp = (char*)s;
	if (!havedig)
		*sp = (char*)s0 - 1;
	if (zret)
		goto retz1;
	if (big) {
		if (esign) {
#ifdef IEEE_Arith
			switch(rounding) {
			  case Round_up:
				if (sign)
					break;
				goto ret_tiny;
			  case Round_down:
				if (!sign)
					break;
				goto ret_tiny;
			  }
#endif
			goto retz;
#ifdef IEEE_Arith
 ret_tinyf:
			Bfree(b MTa);
 ret_tiny:
			Set_errno(ERANGE);
			word0(rvp) = 0;
			word1(rvp) = 1;
			return;
#endif /* IEEE_Arith */
			}
		switch(rounding) {
		  case Round_near:
			goto ovfl1;
		  case Round_up:
			if (!sign)
				goto ovfl1;
			goto ret_big;
		  case Round_down:
			if (sign)
				goto ovfl1;
			goto ret_big;
		  }
 ret_big:
		word0(rvp) = Big0;
		word1(rvp) = Big1;
		return;
		}
	n = s1 - s0 - 1;
	for(k = 0; n > (1 << (kshift-2)) - 1; n >>= 1)
		k++;
	b = Balloc(k MTa);
	x = b->x;
	havedig = n = nz = 0;
	L = 0;
#ifdef USE_LOCALE
	for(i = 0; decimalpoint[i+1]; ++i);
#endif
	while(s1 > s0) {
#ifdef USE_LOCALE
		if (*--s1 == decimalpoint[i]) {
			s1 -= i;
			continue;
			}
#else
		if (*--s1 == '.')
			continue;
#endif
		if ((d = hexdig[*s1]))
			havedig = 1;
		else if (!havedig) {
			e += 4;
			continue;
			}
		if (n == ULbits) {
			*x++ = L;
			L = 0;
			n = 0;
			}
		L |= (d & 0x0f) << n;
		n += 4;
		}
	*x++ = L;
	b->wds = n = x - b->x;
	nb = ULbits*n - hi0bits(L);
	nbits = Nbits;
	lostbits = 0;
	x = b->x;
	if (nb > nbits) {
		n = nb - nbits;
		if (any_on(b,n)) {
			lostbits = 1;
			k = n - 1;
			if (x[k>>kshift] & 1 << (k & kmask)) {
				lostbits = 2;
				if (k > 0 && any_on(b,k))
					lostbits = 3;
				}
			}
		rshift(b, n);
		e += n;
		}
	else if (nb < nbits) {
		n = nbits - nb;
		b = lshift(b, n MTa);
		e -= n;
		x = b->x;
		}
	if (e > emax) {
 ovfl:
		Bfree(b MTa);
 ovfl1:
		Set_errno(ERANGE);
#ifdef Honor_FLT_ROUNDS
		switch (rounding) {
		  case Round_zero:
			goto ret_big;
		  case Round_down:
			if (!sign)
				goto ret_big;
			break;
		  case Round_up:
			if (sign)
				goto ret_big;
		  }
#endif
		word0(rvp) = Exp_mask;
		word1(rvp) = 0;
		return;
		}
	denorm = 0;
	if (e < emin) {
		denorm = 1;
		n = emin - e;
		if (n >= nbits) {
#ifdef IEEE_Arith /*{*/
			switch (rounding) {
			  case Round_near:
				if (n == nbits && (n < 2 || lostbits || any_on(b,n-1)))
					goto ret_tinyf;
				break;
			  case Round_up:
				if (!sign)
					goto ret_tinyf;
				break;
			  case Round_down:
				if (sign)
					goto ret_tinyf;
			  }
#endif /* } IEEE_Arith */
			Bfree(b MTa);
 retz:
			Set_errno(ERANGE);
 retz1:
			rvp->d = 0.;
			return;
			}
		k = n - 1;
#ifdef IEEE_Arith
		if (!k) {
			switch(rounding) {
			  case Round_near:
				if (((b->x[0] & 3) == 3) || (lostbits && (b->x[0] & 1))) {
					multadd(b, 1, 1 MTa);
 emin_check:
					if (b->x[1] == (1 << (Exp_shift + 1))) {
						rshift(b,1);
						e = emin;
						goto normal;
						}
					}
				break;
			  case Round_up:
				if (!sign && (lostbits || (b->x[0] & 1))) {
 incr_denorm:
					multadd(b, 1, 2 MTa);
					check_denorm = 1;
					lostbits = 0;
					goto emin_check;
					}
				break;
			  case Round_down:
				if (sign && (lostbits || (b->x[0] & 1)))
					goto incr_denorm;
				break;
			  }
			}
#endif
		if (lostbits)
			lostbits = 1;
		else if (k > 0)
			lostbits = any_on(b,k);
#ifdef IEEE_Arith
		else if (check_denorm)
			goto no_lostbits;
#endif
		if (x[k>>kshift] & 1 << (k & kmask))
			lostbits |= 2;
#ifdef IEEE_Arith
 no_lostbits:
#endif
		nbits -= n;
		rshift(b,n);
		e = emin;
		}
	if (lostbits) {
		up = 0;
		switch(rounding) {
		  case Round_zero:
			break;
		  case Round_near:
			if (lostbits & 2
			 && (lostbits & 1) | (x[0] & 1))
				up = 1;
			break;
		  case Round_up:
			up = 1 - sign;
			break;
		  case Round_down:
			up = sign;
		  }
		if (up) {
			k = b->wds;
			b = increment(b MTa);
			x = b->x;
			if (!denorm && (b->wds > k
			 || ((n = nbits & kmask) !=0
			     && hi0bits(x[k-1]) < 32-n))) {
				rshift(b,1);
				if (++e > Emax)
					goto ovfl;
				}
			}
		}
#ifdef IEEE_Arith
	if (denorm)
		word0(rvp) = b->wds > 1 ? b->x[1] & ~0x100000 : 0;
	else {
 normal:
		word0(rvp) = (b->x[1] & ~0x100000) | ((e + 0x3ff + 52) << 20);
		}
	word1(rvp) = b->x[0];
#endif
#ifdef IBM
	if ((j = e & 3)) {
		k = b->x[0] & ((1 << j) - 1);
		rshift(b,j);
		if (k) {
			switch(rounding) {
			  case Round_up:
				if (!sign)
					increment(b);
				break;
			  case Round_down:
				if (sign)
					increment(b);
				break;
			  case Round_near:
				j = 1 << (j-1);
				if (k & j && ((k & (j-1)) | lostbits))
					increment(b);
			  }
			}
		}
	e >>= 2;
	word0(rvp) = b->x[1] | ((e + 65 + 13) << 24);
	word1(rvp) = b->x[0];
#endif
#ifdef VAX
	/* The next two lines ignore swap of low- and high-order 2 bytes. */
	/* word0(rvp) = (b->x[1] & ~0x800000) | ((e + 129 + 55) << 23); */
	/* word1(rvp) = b->x[0]; */
	word0(rvp) = ((b->x[1] & ~0x800000) >> 16) | ((e + 129 + 55) << 7) | (b->x[1] << 16);
	word1(rvp) = (b->x[0] >> 16) | (b->x[0] << 16);
#endif
	Bfree(b MTa);
	}
#endif /*!NO_HEX_FP}*/

 static int
dshift(Bigint *b, int p2)
{
	int rv = hi0bits(b->x[b->wds-1]) - 4;
	if (p2 > 0)
		rv -= p2;
	return rv & kmask;
	}

 static int
quorem(Bigint *b, Bigint *S)
{
	int n;
	ULong *bx, *bxe, q, *sx, *sxe;
#ifdef ULLong
	ULLong borrow, carry, y, ys;
#else
	ULong borrow, carry, y, ys;
#ifdef Pack_32
	ULong si, z, zs;
#endif
#endif

	n = S->wds;
#ifdef DEBUG
	/*debug*/ if (b->wds > n)
	/*debug*/	Bug("oversize b in quorem");
#endif
	if (b->wds < n)
		return 0;
	sx = S->x;
	sxe = sx + --n;
	bx = b->x;
	bxe = bx + n;
	q = *bxe / (*sxe + 1);	/* ensure q <= true quotient */
#ifdef DEBUG
#ifdef NO_STRTOD_BIGCOMP
	/*debug*/ if (q > 9)
#else
	/* An oversized q is possible when quorem is called from bigcomp and */
	/* the input is near, e.g., twice the smallest denormalized number. */
	/*debug*/ if (q > 15)
#endif
	/*debug*/	Bug("oversized quotient in quorem");
#endif
	if (q) {
		borrow = 0;
		carry = 0;
		do {
#ifdef ULLong
			ys = *sx++ * (ULLong)q + carry;
			carry = ys >> 32;
			y = *bx - (ys & FFFFFFFF) - borrow;
			borrow = y >> 32 & (ULong)1;
			*bx++ = y & FFFFFFFF;
#else
#ifdef Pack_32
			si = *sx++;
			ys = (si & 0xffff) * q + carry;
			zs = (si >> 16) * q + (ys >> 16);
			carry = zs >> 16;
			y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
			borrow = (y & 0x10000) >> 16;
			z = (*bx >> 16) - (zs & 0xffff) - borrow;
			borrow = (z & 0x10000) >> 16;
			Storeinc(bx, z, y);
#else
			ys = *sx++ * q + carry;
			carry = ys >> 16;
			y = *bx - (ys & 0xffff) - borrow;
			borrow = (y & 0x10000) >> 16;
			*bx++ = y & 0xffff;
#endif
#endif
			}
			while(sx <= sxe);
		if (!*bxe) {
			bx = b->x;
			while(--bxe > bx && !*bxe)
				--n;
			b->wds = n;
			}
		}
	if (cmp(b, S) >= 0) {
		q++;
		borrow = 0;
		carry = 0;
		bx = b->x;
		sx = S->x;
		do {
#ifdef ULLong
			ys = *sx++ + carry;
			carry = ys >> 32;
			y = *bx - (ys & FFFFFFFF) - borrow;
			borrow = y >> 32 & (ULong)1;
			*bx++ = y & FFFFFFFF;
#else
#ifdef Pack_32
			si = *sx++;
			ys = (si & 0xffff) + carry;
			zs = (si >> 16) + (ys >> 16);
			carry = zs >> 16;
			y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
			borrow = (y & 0x10000) >> 16;
			z = (*bx >> 16) - (zs & 0xffff) - borrow;
			borrow = (z & 0x10000) >> 16;
			Storeinc(bx, z, y);
#else
			ys = *sx++ + carry;
			carry = ys >> 16;
			y = *bx - (ys & 0xffff) - borrow;
			borrow = (y & 0x10000) >> 16;
			*bx++ = y & 0xffff;
#endif
#endif
			}
			while(sx <= sxe);
		bx = b->x;
		bxe = bx + n;
		if (!*bxe) {
			while(--bxe > bx && !*bxe)
				--n;
			b->wds = n;
			}
		}
	return q;
	}

#if defined(Avoid_Underflow) || !defined(NO_STRTOD_BIGCOMP) /*{*/
 static double
sulp(U *x, BCinfo *bc)
{
	U u;
	double rv;
	int i;

	rv = ulp(x);
	if (!bc->scale || (i = 2*P + 1 - ((word0(x) & Exp_mask) >> Exp_shift)) <= 0)
		return rv; /* Is there an example where i <= 0 ? */
	word0(&u) = Exp_1 + (i << Exp_shift);
	word1(&u) = 0;
	return rv * u.d;
	}
#endif /*}*/

#ifndef NO_STRTOD_BIGCOMP
 static void
bigcomp(U *rv, const char *s0, BCinfo *bc MTd)
{
	Bigint *b, *d;
	int b2, bbits, d2, dd, dig, dsign, i, j, nd, nd0, p2, p5, speccase;

	dsign = bc->dsign;
	nd = bc->nd;
	nd0 = bc->nd0;
	p5 = nd + bc->e0 - 1;
	speccase = 0;
#ifndef Sudden_Underflow
	if (rv->d == 0.) {	/* special case: value near underflow-to-zero */
				/* threshold was rounded to zero */
		b = i2b(1 MTa);
		p2 = Emin - P + 1;
		bbits = 1;
#ifdef Avoid_Underflow
		word0(rv) = (P+2) << Exp_shift;
#else
		word1(rv) = 1;
#endif
		i = 0;
#ifdef Honor_FLT_ROUNDS
		if (bc->rounding == 1)
#endif
			{
			speccase = 1;
			--p2;
			dsign = 0;
			goto have_i;
			}
		}
	else
#endif
		b = d2b(rv, &p2, &bbits MTa);
#ifdef Avoid_Underflow
	p2 -= bc->scale;
#endif
	/* floor(log2(rv)) == bbits - 1 + p2 */
	/* Check for denormal case. */
	i = P - bbits;
	if (i > (j = P - Emin - 1 + p2)) {
#ifdef Sudden_Underflow
		Bfree(b MTa);
		b = i2b(1 MTa);
		p2 = Emin;
		i = P - 1;
#ifdef Avoid_Underflow
		word0(rv) = (1 + bc->scale) << Exp_shift;
#else
		word0(rv) = Exp_msk1;
#endif
		word1(rv) = 0;
#else
		i = j;
#endif
		}
#ifdef Honor_FLT_ROUNDS
	if (bc->rounding != 1) {
		if (i > 0)
			b = lshift(b, i MTa);
		if (dsign)
			b = increment(b MTa);
		}
	else
#endif
		{
		b = lshift(b, ++i MTa);
		b->x[0] |= 1;
		}
#ifndef Sudden_Underflow
 have_i:
#endif
	p2 -= p5 + i;
	d = i2b(1 MTa);
	/* Arrange for convenient computation of quotients:
	 * shift left if necessary so divisor has 4 leading 0 bits.
	 */
	if (p5 > 0)
		d = pow5mult(d, p5 MTa);
	else if (p5 < 0)
		b = pow5mult(b, -p5 MTa);
	if (p2 > 0) {
		b2 = p2;
		d2 = 0;
		}
	else {
		b2 = 0;
		d2 = -p2;
		}
	i = dshift(d, d2);
	if ((b2 += i) > 0)
		b = lshift(b, b2 MTa);
	if ((d2 += i) > 0)
		d = lshift(d, d2 MTa);

	/* Now b/d = exactly half-way between the two floating-point values */
	/* on either side of the input string.  Compute first digit of b/d. */

	if (!(dig = quorem(b,d))) {
		b = multadd(b, 10, 0 MTa);	/* very unlikely */
		dig = quorem(b,d);
		}

	/* Compare b/d with s0 */

	for(i = 0; i < nd0; ) {
		if ((dd = s0[i++] - '0' - dig))
			goto ret;
		if (!b->x[0] && b->wds == 1) {
			if (i < nd)
				dd = 1;
			goto ret;
			}
		b = multadd(b, 10, 0 MTa);
		dig = quorem(b,d);
		}
	for(j = bc->dp1; i++ < nd;) {
		if ((dd = s0[j++] - '0' - dig))
			goto ret;
		if (!b->x[0] && b->wds == 1) {
			if (i < nd)
				dd = 1;
			goto ret;
			}
		b = multadd(b, 10, 0 MTa);
		dig = quorem(b,d);
		}
	if (dig > 0 || b->x[0] || b->wds > 1)
		dd = -1;
 ret:
	Bfree(b MTa);
	Bfree(d MTa);
#ifdef Honor_FLT_ROUNDS
	if (bc->rounding != 1) {
		if (dd < 0) {
			if (bc->rounding == 0) {
				if (!dsign)
					goto retlow1;
				}
			else if (dsign)
				goto rethi1;
			}
		else if (dd > 0) {
			if (bc->rounding == 0) {
				if (dsign)
					goto rethi1;
				goto ret1;
				}
			if (!dsign)
				goto rethi1;
			dval(rv) += 2.*sulp(rv,bc);
			}
		else {
			bc->inexact = 0;
			if (dsign)
				goto rethi1;
			}
		}
	else
#endif
	if (speccase) {
		if (dd <= 0)
			rv->d = 0.;
		}
	else if (dd < 0) {
		if (!dsign)	/* does not happen for round-near */
retlow1:
			dval(rv) -= sulp(rv,bc);
		}
	else if (dd > 0) {
		if (dsign) {
 rethi1:
			dval(rv) += sulp(rv,bc);
			}
		}
	else {
		/* Exact half-way case:  apply round-even rule. */
		if ((j = ((word0(rv) & Exp_mask) >> Exp_shift) - bc->scale) <= 0) {
			i = 1 - j;
			if (i <= 31) {
				if (word1(rv) & (0x1 << i))
					goto odd;
				}
			else if (word0(rv) & (0x1 << (i-32)))
				goto odd;
			}
		else if (word1(rv) & 1) {
 odd:
			if (dsign)
				goto rethi1;
			goto retlow1;
			}
		}

#ifdef Honor_FLT_ROUNDS
 ret1:
#endif
	return;
	}
#endif /* NO_STRTOD_BIGCOMP */

ZEND_API double
zend_strtod(const char *s00, const char **se)
{
	int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, e, e1;
	int esign, i, j, k, nd, nd0, nf, nz, nz0, nz1, sign;
	const char *s, *s0, *s1;
	volatile double aadj, aadj1;
	Long L;
	U aadj2, adj, rv, rv0;
	ULong y, z;
	BCinfo bc;
	Bigint *bb, *bb1, *bd, *bd0, *bs, *delta;
#ifdef USE_BF96
	ULLong bhi, blo, brv, t00, t01, t02, t10, t11, terv, tg, tlo, yz;
	const BF96 *p10;
	int bexact, erv;
#endif
#ifdef Avoid_Underflow
	ULong Lsb, Lsb1;
#endif
#ifdef SET_INEXACT
	int oldinexact;
#endif
#ifndef NO_STRTOD_BIGCOMP
	int req_bigcomp = 0;
#endif
#ifdef MULTIPLE_THREADS
	ThInfo *TI = 0;
#endif
#ifdef Honor_FLT_ROUNDS /*{*/
#ifdef Trust_FLT_ROUNDS /*{{ only define this if FLT_ROUNDS really works! */
	bc.rounding = Flt_Rounds;
#else /*}{*/
	bc.rounding = 1;
	switch(fegetround()) {
	  case FE_TOWARDZERO:	bc.rounding = 0; break;
	  case FE_UPWARD:	bc.rounding = 2; break;
	  case FE_DOWNWARD:	bc.rounding = 3;
	  }
#endif /*}}*/
#endif /*}*/
#ifdef USE_LOCALE
	const char *s2;
#endif

	sign = nz0 = nz1 = nz = bc.dplen = bc.uflchk = 0;
	dval(&rv) = 0.;
	for(s = s00;;s++) switch(*s) {
		case '-':
			sign = 1;
			ZEND_FALLTHROUGH;
		case '+':
			if (*++s)
				goto break2;
			ZEND_FALLTHROUGH;
		case 0:
			goto ret0;
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
			continue;
		default:
			goto break2;
		}
 break2:
	if (*s == '0') {
#ifndef NO_HEX_FP /*{*/
		switch(s[1]) {
		  case 'x':
		  case 'X':
#ifdef Honor_FLT_ROUNDS
			gethex(&s, &rv, bc.rounding, sign MTb);
#else
			gethex(&s, &rv, 1, sign MTb);
#endif
			goto ret;
		  }
#endif /*}*/
		nz0 = 1;
		while(*++s == '0') ;
		if (!*s)
			goto ret;
		}
	s0 = s;
	nd = nf = 0;
#ifdef USE_BF96
	yz = 0;
	for(; (c = *s) >= '0' && c <= '9'; nd++, s++)
		if (nd < 19)
			yz = 10*yz + c - '0';
#else
	y = z = 0;
	for(; (c = *s) >= '0' && c <= '9'; nd++, s++)
		if (nd < 9)
			y = 10*y + c - '0';
		else if (nd < DBL_DIG + 2)
			z = 10*z + c - '0';
#endif
	nd0 = nd;
	bc.dp0 = bc.dp1 = s - s0;
	for(s1 = s; s1 > s0 && *--s1 == '0'; )
		++nz1;
#ifdef USE_LOCALE
	s1 = localeconv()->decimal_point;
	if (c == *s1) {
		c = '.';
		if (*++s1) {
			s2 = s;
			for(;;) {
				if (*++s2 != *s1) {
					c = 0;
					break;
					}
				if (!*++s1) {
					s = s2;
					break;
					}
				}
			}
		}
#endif
	if (c == '.') {
		c = *++s;
		bc.dp1 = s - s0;
		bc.dplen = bc.dp1 - bc.dp0;
		if (!nd) {
			for(; c == '0'; c = *++s)
				nz++;
			if (c > '0' && c <= '9') {
				bc.dp0 = s0 - s;
				bc.dp1 = bc.dp0 + bc.dplen;
				s0 = s;
				nf += nz;
				nz = 0;
				goto have_dig;
				}
			goto dig_done;
			}
		for(; c >= '0' && c <= '9'; c = *++s) {
 have_dig:
			nz++;
			if (c -= '0') {
				nf += nz;
				i = 1;
#ifdef USE_BF96
				for(; i < nz; ++i) {
					if (++nd <= 19)
						yz *= 10;
					}
				if (++nd <= 19)
					yz = 10*yz + c;
#else
				for(; i < nz; ++i) {
					if (nd++ < 9)
						y *= 10;
					else if (nd <= DBL_DIG + 2)
						z *= 10;
					}
				if (nd++ < 9)
					y = 10*y + c;
				else if (nd <= DBL_DIG + 2)
					z = 10*z + c;
#endif
				nz = nz1 = 0;
				}
			}
		}
 dig_done:
 	if (nd < 0) {
 		/* overflow */
 		nd = DBL_DIG + 2;
 	}
 	if (nf < 0) {
 		/* overflow */
 		nf = DBL_DIG + 2;
 	}
	e = 0;
	if (c == 'e' || c == 'E') {
		if (!nd && !nz && !nz0) {
			goto ret0;
			}
		s00 = s;
		esign = 0;
		switch(c = *++s) {
			case '-':
				esign = 1;
				ZEND_FALLTHROUGH;
			case '+':
				c = *++s;
			}
		if (c >= '0' && c <= '9') {
			while(c == '0')
				c = *++s;
			if (c > '0' && c <= '9') {
				L = c - '0';
				while((c = *++s) >= '0' && c <= '9') {
					if (L <= 19999)
						L = 10*L + c - '0';
					}
				if (L > 19999)
					/* Avoid confusion from exponents
					 * so large that e might overflow.
					 */
					e = 19999; /* safe for 16 bit ints */
				else
					e = (int)L;
				if (esign)
					e = -e;
				}
			else
				e = 0;
			}
		else
			s = s00;
		}
	if (!nd) {
		if (!nz && !nz0) {
#ifdef INFNAN_CHECK /*{*/
			/* Check for Nan and Infinity */
			if (!bc.dplen)
			 switch(c) {
			  case 'i':
			  case 'I':
				if (match(&s,"nf")) {
					--s;
					if (!match(&s,"inity"))
						++s;
					word0(&rv) = 0x7ff00000;
					word1(&rv) = 0;
					goto ret;
					}
				break;
			  case 'n':
			  case 'N':
				if (match(&s, "an")) {
					word0(&rv) = NAN_WORD0;
					word1(&rv) = NAN_WORD1;
#ifndef No_Hex_NaN
					if (*s == '(') /*)*/
						hexnan(&rv, &s);
#endif
					goto ret;
					}
			  }
#endif /*} INFNAN_CHECK */
 ret0:
			s = s00;
			sign = 0;
			}
		goto ret;
		}
	bc.e0 = e1 = e -= nf;

	/* Now we have nd0 digits, starting at s0, followed by a
	 * decimal point, followed by nd-nd0 digits.  The number we're
	 * after is the integer represented by those digits times
	 * 10**e */

	if (!nd0)
		nd0 = nd;
#ifndef USE_BF96
	k = nd < DBL_DIG + 2 ? nd : DBL_DIG + 2;
	dval(&rv) = y;
	if (k > 9) {
#ifdef SET_INEXACT
		if (k > DBL_DIG)
			oldinexact = get_inexact();
#endif
		dval(&rv) = tens[k - 9] * dval(&rv) + z;
		}
#endif
	bd0 = 0;
	if (nd <= DBL_DIG
#ifndef RND_PRODQUOT
#ifndef Honor_FLT_ROUNDS
		&& Flt_Rounds == 1
#endif
#endif
			) {
#ifdef USE_BF96
		dval(&rv) = yz;
#endif
		if (!e)
			goto ret;
#ifndef ROUND_BIASED_without_Round_Up
		if (e > 0) {
			if (e <= Ten_pmax) {
#ifdef SET_INEXACT
				bc.inexact = 0;
				oldinexact = 1;
#endif
#ifdef VAX
				goto vax_ovfl_check;
#else
#ifdef Honor_FLT_ROUNDS
				/* round correctly FLT_ROUNDS = 2 or 3 */
				if (sign) {
					rv.d = -rv.d;
					sign = 0;
					}
#endif
				/* rv = */ rounded_product(dval(&rv), tens[e]);
				goto ret;
#endif
				}
			i = DBL_DIG - nd;
			if (e <= Ten_pmax + i) {
				/* A fancier test would sometimes let us do
				 * this for larger i values.
				 */
#ifdef SET_INEXACT
				bc.inexact = 0;
				oldinexact = 1;
#endif
#ifdef Honor_FLT_ROUNDS
				/* round correctly FLT_ROUNDS = 2 or 3 */
				if (sign) {
					rv.d = -rv.d;
					sign = 0;
					}
#endif
				e -= i;
				dval(&rv) *= tens[i];
#ifdef VAX
				/* VAX exponent range is so narrow we must
				 * worry about overflow here...
				 */
 vax_ovfl_check:
				word0(&rv) -= P*Exp_msk1;
				/* rv = */ rounded_product(dval(&rv), tens[e]);
				if ((word0(&rv) & Exp_mask)
				 > Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
					goto ovfl;
				word0(&rv) += P*Exp_msk1;
#else
				/* rv = */ rounded_product(dval(&rv), tens[e]);
#endif
				goto ret;
				}
			}
#ifndef Inaccurate_Divide
		else if (e >= -Ten_pmax) {
#ifdef SET_INEXACT
				bc.inexact = 0;
				oldinexact = 1;
#endif
#ifdef Honor_FLT_ROUNDS
			/* round correctly FLT_ROUNDS = 2 or 3 */
			if (sign) {
				rv.d = -rv.d;
				sign = 0;
				}
#endif
			/* rv = */ rounded_quotient(dval(&rv), tens[-e]);
			goto ret;
			}
#endif
#endif /* ROUND_BIASED_without_Round_Up */
		}
#ifdef USE_BF96
	k = nd < 19 ? nd : 19;
#endif
	e1 += nd - k;	/* scale factor = 10^e1 */

#ifdef IEEE_Arith
#ifdef SET_INEXACT
	bc.inexact = 1;
#ifndef USE_BF96
	if (k <= DBL_DIG)
#endif
		oldinexact = get_inexact();
#endif
#ifdef Honor_FLT_ROUNDS
	if (bc.rounding >= 2) {
		if (sign)
			bc.rounding = bc.rounding == 2 ? 0 : 2;
		else
			if (bc.rounding != 2)
				bc.rounding = 0;
		}
#endif
#endif /*IEEE_Arith*/

#ifdef USE_BF96 /*{*/
	Debug(++dtoa_stats[0]);
	i = e1 + 342;
	if (i < 0)
		goto undfl;
	if (i > 650)
		goto ovfl;
	p10 = &pten[i];
	brv = yz;
	/* shift brv left, with i =  number of bits shifted */
	i = 0;
	if (!(brv & 0xffffffff00000000ull)) {
		i = 32;
		brv <<= 32;
		}
	if (!(brv & 0xffff000000000000ull)) {
		i += 16;
		brv <<= 16;
		}
	if (!(brv & 0xff00000000000000ull)) {
		i += 8;
		brv <<= 8;
		}
	if (!(brv & 0xf000000000000000ull)) {
		i += 4;
		brv <<= 4;
		}
	if (!(brv & 0xc000000000000000ull)) {
		i += 2;
		brv <<= 2;
		}
	if (!(brv & 0x8000000000000000ull)) {
		i += 1;
		brv <<= 1;
		}
	erv = (64 + 0x3fe) + p10->e - i;
	if (erv <= 0 && nd > 19)
		goto many_digits; /* denormal: may need to look at all digits */
	bhi = brv >> 32;
	blo = brv & 0xffffffffull;
	/* Unsigned 32-bit ints lie in [0,2^32-1] and */
	/* unsigned 64-bit ints lie in [0, 2^64-1].  The product of two unsigned */
	/* 32-bit ints is <= 2^64 - 2*2^32-1 + 1 = 2^64 - 1 - 2*(2^32 - 1), so */
	/* we can add two unsigned 32-bit ints to the product of two such ints, */
	/* and 64 bits suffice to contain the result. */
	t01 = bhi * p10->b1;
	t10 = blo * p10->b0 + (t01 & 0xffffffffull);
	t00 = bhi * p10->b0 + (t01 >> 32) + (t10 >> 32);
	if (t00 & 0x8000000000000000ull) {
		if ((t00 & 0x3ff) && (~t00 & 0x3fe)) { /* unambiguous result? */
			if (nd > 19 && ((t00 + (1<<i) + 2) & 0x400) ^ (t00 & 0x400))
				goto many_digits;
			if (erv <= 0)
				goto denormal;
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 0: goto noround;
			  case 2: goto roundup;
			  }
#endif
			if (t00 & 0x400 && t00 & 0xbff)
				goto roundup;
			goto noround;
			}
		}
	else {
		if ((t00 & 0x1ff) && (~t00 & 0x1fe)) { /* unambiguous result? */
			if (nd > 19 && ((t00 + (1<<i) + 2) & 0x200) ^ (t00 & 0x200))
				goto many_digits;
			if (erv <= 1)
				goto denormal1;
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 0: goto noround1;
			  case 2: goto roundup1;
			  }
#endif
			if (t00 & 0x200)
				goto roundup1;
			goto noround1;
			}
		}
	/* 3 multiplies did not suffice; try a 96-bit approximation */
	Debug(++dtoa_stats[1]);
	t02 = bhi * p10->b2;
	t11 = blo * p10->b1 + (t02 & 0xffffffffull);
	bexact = 1;
	if (e1 < 0 || e1 > 41 || (t10 | t11) & 0xffffffffull || nd > 19)
		bexact = 0;
	tlo = (t10 & 0xffffffffull) + (t02 >> 32) + (t11 >> 32);
	if (!bexact && (tlo + 0x10) >> 32 > tlo >> 32)
		goto many_digits;
	t00 += tlo >> 32;
	if (t00 & 0x8000000000000000ull) {
		if (erv <= 0) { /* denormal result */
			if (nd >= 20 || !((tlo & 0xfffffff0) | (t00 & 0x3ff)))
				goto many_digits;
 denormal:
			if (erv <= -52) {
#ifdef Honor_FLT_ROUNDS
				switch(bc.rounding) {
				  case 0: goto undfl;
				  case 2: goto tiniest;
				  }
#endif
				if (erv < -52 || !(t00 & 0x7fffffffffffffffull))
					goto undfl;
				goto tiniest;
				}
			tg = 1ull << (11 - erv);
			t00 &= ~(tg - 1); /* clear low bits */
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 0: goto noround_den;
			  case 2: goto roundup_den;
			  }
#endif
			if (t00 & tg) {
#ifdef Honor_FLT_ROUNDS
 roundup_den:
#endif
				t00 += tg << 1;
				if (!(t00 & 0x8000000000000000ull)) {
					if (++erv > 0)
						goto smallest_normal;
					t00 = 0x8000000000000000ull;
					}
				}
#ifdef Honor_FLT_ROUNDS
 noround_den:
#endif
			LLval(&rv) = t00 >> (12 - erv);
			Set_errno(ERANGE);
			goto ret;
			}
		if (bexact) {
#ifdef SET_INEXACT
			if (!(t00 & 0x7ff) && !(tlo & 0xffffffffull)) {
				bc.inexact = 0;
				goto noround;
				}
#endif
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 2:
				if (t00 & 0x7ff)
					goto roundup;
			  case 0: goto noround;
			  }
#endif
			if (t00 & 0x400 && (tlo & 0xffffffff) | (t00 & 0xbff))
				goto roundup;
			goto noround;
			}
		if ((tlo & 0xfffffff0) | (t00 & 0x3ff)
		 && (nd <= 19 ||  ((t00 + (1ull << i)) & 0xfffffffffffffc00ull)
				== (t00 & 0xfffffffffffffc00ull))) {
			/* Unambiguous result. */
			/* If nd > 19, then incrementing the 19th digit */
			/* does not affect rv. */
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 0: goto noround;
			  case 2: goto roundup;
			  }
#endif
			if (t00 & 0x400) { /* round up */
 roundup:
				t00 += 0x800;
				if (!(t00 & 0x8000000000000000ull)) {
					/* rounded up to a power of 2 */
					if (erv >= 0x7fe)
						goto ovfl;
					terv = erv + 1;
					LLval(&rv) = terv << 52;
					goto ret;
					}
				}
 noround:
			if (erv >= 0x7ff)
				goto ovfl;
			terv = erv;
			LLval(&rv) = (terv << 52) | ((t00 & 0x7ffffffffffff800ull) >> 11);
			goto ret;
			}
		}
	else {
		if (erv <= 1) { /* denormal result */
			if (nd >= 20 || !((tlo & 0xfffffff0) | (t00 & 0x1ff)))
				goto many_digits;
 denormal1:
			if (erv <= -51) {
#ifdef Honor_FLT_ROUNDS
				switch(bc.rounding) {
				  case 0: goto undfl;
				  case 2: goto tiniest;
				  }
#endif
				if (erv < -51 || !(t00 & 0x3fffffffffffffffull))
					goto undfl;
 tiniest:
				LLval(&rv) = 1;
				Set_errno(ERANGE);
				goto ret;
				}
			tg = 1ull << (11 - erv);
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 0: goto noround1_den;
			  case 2: goto roundup1_den;
			  }
#endif
			if (t00 & tg) {
#ifdef Honor_FLT_ROUNDS
 roundup1_den:
#endif
				if (0x8000000000000000ull & (t00 += (tg<<1)) && erv == 1) {

 smallest_normal:
					LLval(&rv) = 0x0010000000000000ull;
					goto ret;
					}
				}
#ifdef Honor_FLT_ROUNDS
 noround1_den:
#endif
			if (erv <= -52)
				goto undfl;
			LLval(&rv) = t00 >> (12 - erv);
			Set_errno(ERANGE);
			goto ret;
			}
		if (bexact) {
#ifdef SET_INEXACT
			if (!(t00 & 0x3ff) && !(tlo & 0xffffffffull)) {
				bc.inexact = 0;
				goto noround1;
				}
#endif
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 2:
				if (t00 & 0x3ff)
					goto roundup1;
			  case 0: goto noround1;
			  }
#endif
			if (t00 & 0x200 && (t00 & 0x5ff || tlo))
				goto roundup1;
			goto noround1;
			}
		if ((tlo & 0xfffffff0) | (t00 & 0x1ff)
		 && (nd <= 19 ||  ((t00 + (1ull << i)) & 0x7ffffffffffffe00ull)
				== (t00 & 0x7ffffffffffffe00ull))) {
			/* Unambiguous result. */
#ifdef Honor_FLT_ROUNDS
			switch(bc.rounding) {
			  case 0: goto noround1;
			  case 2: goto roundup1;
			  }
#endif
			if (t00 & 0x200) { /* round up */
 roundup1:
				t00 += 0x400;
				if (!(t00 & 0x4000000000000000ull)) {
					/* rounded up to a power of 2 */
					if (erv >= 0x7ff)
						goto ovfl;
					terv = erv;
					LLval(&rv) = terv << 52;
					goto ret;
					}
				}
 noround1:
			if (erv >= 0x800)
				goto ovfl;
			terv = erv - 1;
			LLval(&rv) = (terv << 52) | ((t00 & 0x3ffffffffffffc00ull) >> 10);
			goto ret;
			}
		}
 many_digits:
	Debug(++dtoa_stats[2]);
	if (nd > 17) {
		if (nd > 18) {
			yz /= 100;
			e1 += 2;
			}
		else {
			yz /= 10;
			e1 += 1;
			}
		y = yz / 100000000;
		}
	else if (nd > 9) {
		i = nd - 9;
		y = (yz >> i) / pfive[i-1];
		}
	else
		y = yz;
	dval(&rv) = yz;
#endif /*}*/

#ifdef IEEE_Arith
#ifdef Avoid_Underflow
	bc.scale = 0;
#endif
#endif /*IEEE_Arith*/

	/* Get starting approximation = rv * 10**e1 */

	if (e1 > 0) {
		if ((i = e1 & 15))
			dval(&rv) *= tens[i];
		if (e1 &= ~15) {
			if (e1 > DBL_MAX_10_EXP) {
 ovfl:
				/* Can't trust HUGE_VAL */
#ifdef IEEE_Arith
#ifdef Honor_FLT_ROUNDS
				switch(bc.rounding) {
				  case 0: /* toward 0 */
				  case 3: /* toward -infinity */
					word0(&rv) = Big0;
					word1(&rv) = Big1;
					break;
				  default:
					word0(&rv) = Exp_mask;
					word1(&rv) = 0;
				  }
#else /*Honor_FLT_ROUNDS*/
				word0(&rv) = Exp_mask;
				word1(&rv) = 0;
#endif /*Honor_FLT_ROUNDS*/
#ifdef SET_INEXACT
				/* set overflow bit */
				dval(&rv0) = 1e300;
				dval(&rv0) *= dval(&rv0);
#endif
#else /*IEEE_Arith*/
				word0(&rv) = Big0;
				word1(&rv) = Big1;
#endif /*IEEE_Arith*/
 range_err:
				if (bd0) {
					Bfree(bb MTb);
					Bfree(bd MTb);
					Bfree(bs MTb);
					Bfree(bd0 MTb);
					Bfree(delta MTb);
					}
				Set_errno(ERANGE);
				goto ret;
				}
			e1 >>= 4;
			for(j = 0; e1 > 1; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= bigtens[j];
		/* The last multiplication could overflow. */
			word0(&rv) -= P*Exp_msk1;
			dval(&rv) *= bigtens[j];
			if ((z = word0(&rv) & Exp_mask)
			 > Exp_msk1*(DBL_MAX_EXP+Bias-P))
				goto ovfl;
			if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) {
				/* set to largest number */
				/* (Can't trust DBL_MAX) */
				word0(&rv) = Big0;
				word1(&rv) = Big1;
				}
			else
				word0(&rv) += P*Exp_msk1;
			}
		}
	else if (e1 < 0) {
		e1 = -e1;
		if ((i = e1 & 15))
			dval(&rv) /= tens[i];
		if (e1 >>= 4) {
			if (e1 >= 1 << n_bigtens)
				goto undfl;
#ifdef Avoid_Underflow
			if (e1 & Scale_Bit)
				bc.scale = 2*P;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= tinytens[j];
			if (bc.scale && (j = 2*P + 1 - ((word0(&rv) & Exp_mask)
						>> Exp_shift)) > 0) {
				/* scaled rv is denormal; clear j low bits */
				if (j >= 32) {
					if (j > 54)
						goto undfl;
					word1(&rv) = 0;
					if (j >= 53)
					 word0(&rv) = (P+2)*Exp_msk1;
					else
					 word0(&rv) &= 0xffffffff << (j-32);
					}
				else
					word1(&rv) &= 0xffffffff << j;
				}
#else
			for(j = 0; e1 > 1; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= tinytens[j];
			/* The last multiplication could underflow. */
			dval(&rv0) = dval(&rv);
			dval(&rv) *= tinytens[j];
			if (!dval(&rv)) {
				dval(&rv) = 2.*dval(&rv0);
				dval(&rv) *= tinytens[j];
#endif
				if (!dval(&rv)) {
 undfl:
					dval(&rv) = 0.;
#ifdef Honor_FLT_ROUNDS
					if (bc.rounding == 2)
						word1(&rv) = 1;
#endif
					goto range_err;
					}
#ifndef Avoid_Underflow
				word0(&rv) = Tiny0;
				word1(&rv) = Tiny1;
				/* The refinement below will clean
				 * this approximation up.
				 */
				}
#endif
			}
		}

	/* Now the hard part -- adjusting rv to the correct value.*/

	/* Put digits into bd: true value = bd * 10^e */

	bc.nd = nd - nz1;
#ifndef NO_STRTOD_BIGCOMP
	bc.nd0 = nd0;	/* Only needed if nd > strtod_diglim, but done here */
			/* to silence an erroneous warning about bc.nd0 */
			/* possibly not being initialized. */
	if (nd > strtod_diglim) {
		/* ASSERT(strtod_diglim >= 18); 18 == one more than the */
		/* minimum number of decimal digits to distinguish double values */
		/* in IEEE arithmetic. */
		i = j = 18;
		if (i > nd0)
			j += bc.dplen;
		for(;;) {
			if (--j < bc.dp1 && j >= bc.dp0)
				j = bc.dp0 - 1;
			if (s0[j] != '0')
				break;
			--i;
			}
		e += nd - i;
		nd = i;
		if (nd0 > nd)
			nd0 = nd;
		if (nd < 9) { /* must recompute y */
			y = 0;
			for(i = 0; i < nd0; ++i)
				y = 10*y + s0[i] - '0';
			for(j = bc.dp1; i < nd; ++i)
				y = 10*y + s0[j++] - '0';
			}
		}
#endif
	bd0 = s2b(s0, nd0, nd, y, bc.dplen MTb);

	for(;;) {
		bd = Balloc(bd0->k MTb);
		Bcopy(bd, bd0);
		bb = d2b(&rv, &bbe, &bbbits MTb);	/* rv = bb * 2^bbe */
		bs = i2b(1 MTb);

		if (e >= 0) {
			bb2 = bb5 = 0;
			bd2 = bd5 = e;
			}
		else {
			bb2 = bb5 = -e;
			bd2 = bd5 = 0;
			}
		if (bbe >= 0)
			bb2 += bbe;
		else
			bd2 -= bbe;
		bs2 = bb2;
#ifdef Honor_FLT_ROUNDS
		if (bc.rounding != 1)
			bs2++;
#endif
#ifdef Avoid_Underflow
		Lsb = LSB;
		Lsb1 = 0;
		j = bbe - bc.scale;
		i = j + bbbits - 1;	/* logb(rv) */
		j = P + 1 - bbbits;
		if (i < Emin) {	/* denormal */
			i = Emin - i;
			j -= i;
			if (i < 32)
				Lsb <<= i;
			else if (i < 52)
				Lsb1 = Lsb << (i-32);
			else
				Lsb1 = Exp_mask;
			}
#else /*Avoid_Underflow*/
#ifdef Sudden_Underflow
#ifdef IBM
		j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
		j = P + 1 - bbbits;
#endif
#else /*Sudden_Underflow*/
		j = bbe;
		i = j + bbbits - 1;	/* logb(rv) */
		if (i < Emin)	/* denormal */
			j += P - Emin;
		else
			j = P + 1 - bbbits;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
		bb2 += j;
		bd2 += j;
#ifdef Avoid_Underflow
		bd2 += bc.scale;
#endif
		i = bb2 < bd2 ? bb2 : bd2;
		if (i > bs2)
			i = bs2;
		if (i > 0) {
			bb2 -= i;
			bd2 -= i;
			bs2 -= i;
			}
		if (bb5 > 0) {
			bs = pow5mult(bs, bb5 MTb);
			bb1 = mult(bs, bb MTb);
			Bfree(bb MTb);
			bb = bb1;
			}
		if (bb2 > 0)
			bb = lshift(bb, bb2 MTb);
		if (bd5 > 0)
			bd = pow5mult(bd, bd5 MTb);
		if (bd2 > 0)
			bd = lshift(bd, bd2 MTb);
		if (bs2 > 0)
			bs = lshift(bs, bs2 MTb);
		delta = diff(bb, bd MTb);
		bc.dsign = delta->sign;
		delta->sign = 0;
		i = cmp(delta, bs);
#ifndef NO_STRTOD_BIGCOMP /*{*/
		if (bc.nd > nd && i <= 0) {
			if (bc.dsign) {
				/* Must use bigcomp(). */
				req_bigcomp = 1;
				break;
				}
#ifdef Honor_FLT_ROUNDS
			if (bc.rounding != 1) {
				if (i < 0) {
					req_bigcomp = 1;
					break;
					}
				}
			else
#endif
				i = -1;	/* Discarded digits make delta smaller. */
			}
#endif /*}*/
#ifdef Honor_FLT_ROUNDS /*{*/
		if (bc.rounding != 1) {
			if (i < 0) {
				/* Error is less than an ulp */
				if (!delta->x[0] && delta->wds <= 1) {
					/* exact */
#ifdef SET_INEXACT
					bc.inexact = 0;
#endif
					break;
					}
				if (bc.rounding) {
					if (bc.dsign) {
						adj.d = 1.;
						goto apply_adj;
						}
					}
				else if (!bc.dsign) {
					adj.d = -1.;
					if (!word1(&rv)
					 && !(word0(&rv) & Frac_mask)) {
						y = word0(&rv) & Exp_mask;
#ifdef Avoid_Underflow
						if (!bc.scale || y > 2*P*Exp_msk1)
#else
						if (y)
#endif
						  {
						  delta = lshift(delta,Log2P MTb);
						  if (cmp(delta, bs) <= 0)
							adj.d = -0.5;
						  }
						}
 apply_adj:
#ifdef Avoid_Underflow /*{*/
					if (bc.scale && (y = word0(&rv) & Exp_mask)
						<= 2*P*Exp_msk1)
					  word0(&adj) += (2*P+1)*Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
					if ((word0(&rv) & Exp_mask) <=
							P*Exp_msk1) {
						word0(&rv) += P*Exp_msk1;
						dval(&rv) += adj.d*ulp(dval(&rv));
						word0(&rv) -= P*Exp_msk1;
						}
					else
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow}*/
					dval(&rv) += adj.d*ulp(&rv);
					}
				break;
				}
			adj.d = ratio(delta, bs);
			if (adj.d < 1.)
				adj.d = 1.;
			if (adj.d <= 0x7ffffffe) {
				/* adj = rounding ? ceil(adj) : floor(adj); */
				y = adj.d;
				if (y != adj.d) {
					if (!((bc.rounding>>1) ^ bc.dsign))
						y++;
					adj.d = y;
					}
				}
#ifdef Avoid_Underflow /*{*/
			if (bc.scale && (y = word0(&rv) & Exp_mask) <= 2*P*Exp_msk1)
				word0(&adj) += (2*P+1)*Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
			if ((word0(&rv) & Exp_mask) <= P*Exp_msk1) {
				word0(&rv) += P*Exp_msk1;
				adj.d *= ulp(dval(&rv));
				if (bc.dsign)
					dval(&rv) += adj.d;
				else
					dval(&rv) -= adj.d;
				word0(&rv) -= P*Exp_msk1;
				goto cont;
				}
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow}*/
			adj.d *= ulp(&rv);
			if (bc.dsign) {
				if (word0(&rv) == Big0 && word1(&rv) == Big1)
					goto ovfl;
				dval(&rv) += adj.d;
				}
			else
				dval(&rv) -= adj.d;
			goto cont;
			}
#endif /*}Honor_FLT_ROUNDS*/

		if (i < 0) {
			/* Error is less than half an ulp -- check for
			 * special case of mantissa a power of two.
			 */
			if (bc.dsign || word1(&rv) || word0(&rv) & Bndry_mask
#ifdef IEEE_Arith /*{*/
#ifdef Avoid_Underflow
			 || (word0(&rv) & Exp_mask) <= (2*P+1)*Exp_msk1
#else
			 || (word0(&rv) & Exp_mask) <= Exp_msk1
#endif
#endif /*}*/
				) {
#ifdef SET_INEXACT
				if (!delta->x[0] && delta->wds <= 1)
					bc.inexact = 0;
#endif
				break;
				}
			if (!delta->x[0] && delta->wds <= 1) {
				/* exact result */
#ifdef SET_INEXACT
				bc.inexact = 0;
#endif
				break;
				}
			delta = lshift(delta,Log2P MTb);
			if (cmp(delta, bs) > 0)
				goto drop_down;
			break;
			}
		if (i == 0) {
			/* exactly half-way between */
			if (bc.dsign) {
				if ((word0(&rv) & Bndry_mask1) == Bndry_mask1
				 &&  word1(&rv) == (
#ifdef Avoid_Underflow
			(bc.scale && (y = word0(&rv) & Exp_mask) <= 2*P*Exp_msk1)
		? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
#endif
						   0xffffffff)) {
					/*boundary case -- increment exponent*/
					if (word0(&rv) == Big0 && word1(&rv) == Big1)
						goto ovfl;
					word0(&rv) = (word0(&rv) & Exp_mask)
						+ Exp_msk1
#ifdef IBM
						| Exp_msk1 >> 4
#endif
						;
					word1(&rv) = 0;
#ifdef Avoid_Underflow
					bc.dsign = 0;
#endif
					break;
					}
				}
			else if (!(word0(&rv) & Bndry_mask) && !word1(&rv)) {
 drop_down:
				/* boundary case -- decrement exponent */
#ifdef Sudden_Underflow /*{{*/
				L = word0(&rv) & Exp_mask;
#ifdef IBM
				if (L <  Exp_msk1)
#else
#ifdef Avoid_Underflow
				if (L <= (bc.scale ? (2*P+1)*Exp_msk1 : Exp_msk1))
#else
				if (L <= Exp_msk1)
#endif /*Avoid_Underflow*/
#endif /*IBM*/
					{
					if (bc.nd >nd) {
						bc.uflchk = 1;
						break;
						}
					goto undfl;
					}
				L -= Exp_msk1;
#else /*Sudden_Underflow}{*/
#ifdef Avoid_Underflow
				if (bc.scale) {
					L = word0(&rv) & Exp_mask;
					if (L <= (2*P+1)*Exp_msk1) {
						if (L > (P+2)*Exp_msk1)
							/* round even ==> */
							/* accept rv */
							break;
						/* rv = smallest denormal */
						if (bc.nd >nd) {
							bc.uflchk = 1;
							break;
							}
						goto undfl;
						}
					}
#endif /*Avoid_Underflow*/
				L = (word0(&rv) & Exp_mask) - Exp_msk1;
#endif /*Sudden_Underflow}}*/
				word0(&rv) = L | Bndry_mask1;
				word1(&rv) = 0xffffffff;
#ifdef IBM
				goto cont;
#else
#ifndef NO_STRTOD_BIGCOMP
				if (bc.nd > nd)
					goto cont;
#endif
				break;
#endif
				}
#ifndef ROUND_BIASED
#ifdef Avoid_Underflow
			if (Lsb1) {
				if (!(word0(&rv) & Lsb1))
					break;
				}
			else if (!(word1(&rv) & Lsb))
				break;
#else
			if (!(word1(&rv) & LSB))
				break;
#endif
#endif
			if (bc.dsign)
#ifdef Avoid_Underflow
				dval(&rv) += sulp(&rv, &bc);
#else
				dval(&rv) += ulp(&rv);
#endif
#ifndef ROUND_BIASED
			else {
#ifdef Avoid_Underflow
				dval(&rv) -= sulp(&rv, &bc);
#else
				dval(&rv) -= ulp(&rv);
#endif
#ifndef Sudden_Underflow
				if (!dval(&rv)) {
					if (bc.nd >nd) {
						bc.uflchk = 1;
						break;
						}
					goto undfl;
					}
#endif
				}
#ifdef Avoid_Underflow
			bc.dsign = 1 - bc.dsign;
#endif
#endif
			break;
			}
		if ((aadj = ratio(delta, bs)) <= 2.) {
			if (bc.dsign)
				aadj = aadj1 = 1.;
			else if (word1(&rv) || word0(&rv) & Bndry_mask) {
#ifndef Sudden_Underflow
				if (word1(&rv) == Tiny1 && !word0(&rv)) {
					if (bc.nd >nd) {
						bc.uflchk = 1;
						break;
						}
					goto undfl;
					}
#endif
				aadj = 1.;
				aadj1 = -1.;
				}
			else {
				/* special case -- power of FLT_RADIX to be */
				/* rounded down... */

				if (aadj < 2./FLT_RADIX)
					aadj = 1./FLT_RADIX;
				else
					aadj *= 0.5;
				aadj1 = -aadj;
				}
			}
		else {
			aadj *= 0.5;
			aadj1 = bc.dsign ? aadj : -aadj;
#ifdef Check_FLT_ROUNDS
			switch(bc.rounding) {
				case 2: /* towards +infinity */
					aadj1 -= 0.5;
					break;
				case 0: /* towards 0 */
				case 3: /* towards -infinity */
					aadj1 += 0.5;
				}
#else
			if (Flt_Rounds == 0)
				aadj1 += 0.5;
#endif /*Check_FLT_ROUNDS*/
			}
		y = word0(&rv) & Exp_mask;

		/* Check for overflow */

		if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1)) {
			dval(&rv0) = dval(&rv);
			word0(&rv) -= P*Exp_msk1;
			adj.d = aadj1 * ulp(&rv);
			dval(&rv) += adj.d;
			if ((word0(&rv) & Exp_mask) >=
					Exp_msk1*(DBL_MAX_EXP+Bias-P)) {
				if (word0(&rv0) == Big0 && word1(&rv0) == Big1)
					goto ovfl;
				word0(&rv) = Big0;
				word1(&rv) = Big1;
				goto cont;
				}
			else
				word0(&rv) += P*Exp_msk1;
			}
		else {
#ifdef Avoid_Underflow
			if (bc.scale && y <= 2*P*Exp_msk1) {
				if (aadj <= 0x7fffffff) {
					if ((z = aadj) <= 0)
						z = 1;
					aadj = z;
					aadj1 = bc.dsign ? aadj : -aadj;
					}
				dval(&aadj2) = aadj1;
				word0(&aadj2) += (2*P+1)*Exp_msk1 - y;
				aadj1 = dval(&aadj2);
				adj.d = aadj1 * ulp(&rv);
				dval(&rv) += adj.d;
				if (rv.d == 0.)
#ifdef NO_STRTOD_BIGCOMP
					goto undfl;
#else
					{
					req_bigcomp = 1;
					break;
					}
#endif
				}
			else {
				adj.d = aadj1 * ulp(&rv);
				dval(&rv) += adj.d;
				}
#else
#ifdef Sudden_Underflow
			if ((word0(&rv) & Exp_mask) <= P*Exp_msk1) {
				dval(&rv0) = dval(&rv);
				word0(&rv) += P*Exp_msk1;
				adj.d = aadj1 * ulp(&rv);
				dval(&rv) += adj.d;
#ifdef IBM
				if ((word0(&rv) & Exp_mask) <  P*Exp_msk1)
#else
				if ((word0(&rv) & Exp_mask) <= P*Exp_msk1)
#endif
					{
					if (word0(&rv0) == Tiny0
					 && word1(&rv0) == Tiny1) {
						if (bc.nd >nd) {
							bc.uflchk = 1;
							break;
							}
						goto undfl;
						}
					word0(&rv) = Tiny0;
					word1(&rv) = Tiny1;
					goto cont;
					}
				else
					word0(&rv) -= P*Exp_msk1;
				}
			else {
				adj.d = aadj1 * ulp(&rv);
				dval(&rv) += adj.d;
				}
#else /*Sudden_Underflow*/
			/* Compute adj so that the IEEE rounding rules will
			 * correctly round rv + adj in some half-way cases.
			 * If rv * ulp(rv) is denormalized (i.e.,
			 * y <= (P-1)*Exp_msk1), we must adjust aadj to avoid
			 * trouble from bits lost to denormalization;
			 * example: 1.2e-307 .
			 */
			if (y <= (P-1)*Exp_msk1 && aadj > 1.) {
				aadj1 = (double)(int)(aadj + 0.5);
				if (!bc.dsign)
					aadj1 = -aadj1;
				}
			adj.d = aadj1 * ulp(&rv);
			dval(&rv) += adj.d;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
			}
		z = word0(&rv) & Exp_mask;
#ifndef SET_INEXACT
		if (bc.nd == nd) {
#ifdef Avoid_Underflow
		if (!bc.scale)
#endif
		if (y == z) {
			/* Can we stop now? */
			L = (Long)aadj;
			aadj -= L;
			/* The tolerances below are conservative. */
			if (bc.dsign || word1(&rv) || word0(&rv) & Bndry_mask) {
				if (aadj < .4999999 || aadj > .5000001)
					break;
				}
			else if (aadj < .4999999/FLT_RADIX)
				break;
			}
		}
#endif
 cont:
		Bfree(bb MTb);
		Bfree(bd MTb);
		Bfree(bs MTb);
		Bfree(delta MTb);
		}
	Bfree(bb MTb);
	Bfree(bd MTb);
	Bfree(bs MTb);
	Bfree(bd0 MTb);
	Bfree(delta MTb);
#ifndef NO_STRTOD_BIGCOMP
	if (req_bigcomp) {
		bd0 = 0;
		bc.e0 += nz1;
		bigcomp(&rv, s0, &bc MTb);
		y = word0(&rv) & Exp_mask;
		if (y == Exp_mask)
			goto ovfl;
		if (y == 0 && rv.d == 0.)
			goto undfl;
		}
#endif
#ifdef Avoid_Underflow
	if (bc.scale) {
		word0(&rv0) = Exp_1 - 2*P*Exp_msk1;
		word1(&rv0) = 0;
		dval(&rv) *= dval(&rv0);
#ifndef NO_ERRNO
		/* try to avoid the bug of testing an 8087 register value */
#ifdef IEEE_Arith
		if (!(word0(&rv) & Exp_mask))
#else
		if (word0(&rv) == 0 && word1(&rv) == 0)
#endif
			Set_errno(ERANGE);
#endif
		}
#endif /* Avoid_Underflow */
 ret:
#ifdef SET_INEXACT
	if (bc.inexact) {
		if (!(word0(&rv) & Exp_mask)) {
			/* set underflow and inexact bits */
			dval(&rv0) = 1e-300;
			dval(&rv0) *= dval(&rv0);
			}
		else if (!oldinexact) {
			word0(&rv0) = Exp_1 + (70 << Exp_shift);
			word1(&rv0) = 0;
			dval(&rv0) += 1.;
			}
		}
	else if (!oldinexact)
		clear_inexact();
#endif
	if (se)
		*se = (char *)s;
	return sign ? -dval(&rv) : dval(&rv);
	}

#if !defined(MULTIPLE_THREADS) && !defined(dtoa_result)
 ZEND_TLS char *dtoa_result;
#endif

 static char *
rv_alloc(int i MTd)
{

	int j, k, *r;
	size_t rem;

	rem = sizeof(Bigint) - sizeof(ULong) - sizeof(int);


	j = sizeof(ULong);
	if (i > ((INT_MAX >> 2) + rem))
		i = (INT_MAX >> 2) + rem;
	for(k = 0;
		rem + j <= (size_t)i; j <<= 1)
			k++;
	r = (int*)Balloc(k MTa);
	*r = k;
	return
#ifndef MULTIPLE_THREADS
	dtoa_result =
#endif
		(char *)(r+1);
	}

 static char *
nrv_alloc(const char *s, char *s0, size_t s0len, char **rve, int n MTd)
{
	char *rv, *t;

	if (!s0)
		s0 = rv_alloc(n MTa);
	else if (s0len <= n) {
		rv = 0;
		t = rv + n;
		goto rve_chk;
		}
	t = rv = s0;
	while((*t = *s++))
		++t;
 rve_chk:
	if (rve)
		*rve = t;
	return rv;
	}

/* freedtoa(s) must be used to free values s returned by dtoa
 * when MULTIPLE_THREADS is #defined.  It should be used in all cases,
 * but for consistency with earlier versions of dtoa, it is optional
 * when MULTIPLE_THREADS is not defined.
 */

ZEND_API void
zend_freedtoa(char *s)
{
#ifdef MULTIPLE_THREADS
	ThInfo *TI = 0;
#endif
	Bigint *b = (Bigint *)((int *)s - 1);
	b->maxwds = 1 << (b->k = *(int*)b);
	Bfree(b MTb);
#ifndef MULTIPLE_THREADS
	if (s == dtoa_result)
		dtoa_result = 0;
#endif
	}

/* dtoa for IEEE arithmetic (dmg): convert double to ASCII string.
 *
 * Inspired by "How to Print Floating-Point Numbers Accurately" by
 * Guy L. Steele, Jr. and Jon L. White [Proc. ACM SIGPLAN '90, pp. 112-126].
 *
 * Modifications:
 *	1. Rather than iterating, we use a simple numeric overestimate
 *	   to determine k = floor(log10(d)).  We scale relevant
 *	   quantities using O(log2(k)) rather than O(k) multiplications.
 *	2. For some modes > 2 (corresponding to ecvt and fcvt), we don't
 *	   try to generate digits strictly left to right.  Instead, we
 *	   compute with fewer bits and propagate the carry if necessary
 *	   when rounding the final digit up.  This is often faster.
 *	3. Under the assumption that input will be rounded nearest,
 *	   mode 0 renders 1e23 as 1e23 rather than 9.999999999999999e22.
 *	   That is, we allow equality in stopping tests when the
 *	   round-nearest rule will give the same floating-point value
 *	   as would satisfaction of the stopping test with strict
 *	   inequality.
 *	4. We remove common factors of powers of 2 from relevant
 *	   quantities.
 *	5. When converting floating-point integers less than 1e16,
 *	   we use floating-point arithmetic rather than resorting
 *	   to multiple-precision integers.
 *	6. When asked to produce fewer than 15 digits, we first try
 *	   to get by with floating-point arithmetic; we resort to
 *	   multiple-precision integer arithmetic only if we cannot
 *	   guarantee that the floating-point calculation has given
 *	   the correctly rounded result.  For k requested digits and
 *	   "uniformly" distributed input, the probability is
 *	   something like 10^(k-15) that we must resort to the Long
 *	   calculation.
 */

 static char *
dtoa_r(double dd, int mode, int ndigits, int *decpt, bool *sign, char **rve, char *buf, size_t blen)
{
 /*	Arguments ndigits, decpt, sign are similar to those
	of ecvt and fcvt; trailing zeros are suppressed from
	the returned string.  If not null, *rve is set to point
	to the end of the return value.  If d is +-Infinity or NaN,
	then *decpt is set to 9999.

	mode:
		0 ==> shortest string that yields d when read in
			and rounded to nearest.
		1 ==> like 0, but with Steele & White stopping rule;
			e.g. with IEEE P754 arithmetic , mode 0 gives
			1e23 whereas mode 1 gives 9.999999999999999e22.
		2 ==> max(1,ndigits) significant digits.  This gives a
			return value similar to that of ecvt, except
			that trailing zeros are suppressed.
		3 ==> through ndigits past the decimal point.  This
			gives a return value similar to that from fcvt,
			except that trailing zeros are suppressed, and
			ndigits can be negative.
		4,5 ==> similar to 2 and 3, respectively, but (in
			round-nearest mode) with the tests of mode 0 to
			possibly return a shorter string that rounds to d.
			With IEEE arithmetic and compilation with
			-DHonor_FLT_ROUNDS, modes 4 and 5 behave the same
			as modes 2 and 3 when FLT_ROUNDS != 1.
		6-9 ==> Debugging modes similar to mode - 4:  don't try
			fast floating-point estimate (if applicable).

		Values of mode other than 0-9 are treated as mode 0.

	When not NULL, buf is an output buffer of length blen, which must
	be large enough to accommodate suppressed trailing zeros and a trailing
	null byte.  If blen is too small, rv = NULL is returned, in which case
	if rve is not NULL, a subsequent call with blen >= (*rve - rv) + 1
	should succeed in returning buf.

	When buf is NULL, sufficient space is allocated for the return value,
	which, when done using, the caller should pass to freedtoa().

	USE_BF is automatically defined when neither NO_LONG_LONG nor NO_BF96
	is defined.
	*/

#ifdef MULTIPLE_THREADS
	ThInfo *TI = 0;
#endif
	int bbits, b2, b5, be, dig, i, ilim, ilim1,
		j, j1, k, leftright, m2, m5, s2, s5, spec_case;
#if !defined(Sudden_Underflow) || defined(USE_BF96)
	int denorm;
#endif
	Bigint *b, *b1, *delta, *mlo, *mhi, *S;
	U u;
	char *s;
#ifdef SET_INEXACT
	int inexact, oldinexact;
#endif
#ifdef USE_BF96 /*{{*/
	const BF96 *p10;
	ULLong dbhi, dbits, dblo, den, hb, rb, rblo, res, res0, res3, reslo, sres,
		sulp, tv0, tv1, tv2, tv3, ulp, ulplo, ulpmask, ures, ureslo, zb;
	int eulp, k1, n2, ulpadj, ulpshift;
#else /*}{*/
#ifndef Sudden_Underflow
	ULong x;
#endif
	Long L;
	U d2, eps;
	double ds;
	int ieps, ilim0, k0, k_check, try_quick;
#ifndef No_leftright
#ifdef IEEE_Arith
	U eps1;
#endif
#endif
#endif /*}}*/
#ifdef Honor_FLT_ROUNDS /*{*/
	int Rounding;
#ifdef Trust_FLT_ROUNDS /*{{ only define this if FLT_ROUNDS really works! */
	Rounding = Flt_Rounds;
#else /*}{*/
	Rounding = 1;
	switch(fegetround()) {
	  case FE_TOWARDZERO:	Rounding = 0; break;
	  case FE_UPWARD:	Rounding = 2; break;
	  case FE_DOWNWARD:	Rounding = 3;
	  }
#endif /*}}*/
#endif /*}*/

	u.d = dd;
	if (word0(&u) & Sign_bit) {
		/* set sign for everything, including 0's and NaNs */
		*sign = 1;
		word0(&u) &= ~Sign_bit;	/* clear sign bit */
		}
	else
		*sign = 0;

#if defined(IEEE_Arith) + defined(VAX)
#ifdef IEEE_Arith
	if ((word0(&u) & Exp_mask) == Exp_mask)
#else
	if (word0(&u)  == 0x8000)
#endif
		{
		/* Infinity or NaN */
		*decpt = 9999;
#ifdef IEEE_Arith
		if (!word1(&u) && !(word0(&u) & 0xfffff))
			return nrv_alloc("Infinity", buf, blen, rve, 8 MTb);
#endif
		return nrv_alloc("NaN", buf, blen, rve, 3 MTb);
		}
#endif
#ifdef IBM
	dval(&u) += 0; /* normalize */
#endif
	if (!dval(&u)) {
		*decpt = 1;
		return nrv_alloc("0", buf, blen, rve, 1 MTb);
		}

#ifdef SET_INEXACT
#ifndef USE_BF96
	try_quick =
#endif
	oldinexact = get_inexact();
	inexact = 1;
#endif
#ifdef Honor_FLT_ROUNDS
	if (Rounding >= 2) {
		if (*sign)
			Rounding = Rounding == 2 ? 0 : 2;
		else
			if (Rounding != 2)
				Rounding = 0;
		}
#endif
#ifdef USE_BF96 /*{{*/
	dbits = (u.LL & 0xfffffffffffffull) << 11;	/* fraction bits */
	if ((be = u.LL >> 52)) /* biased exponent; nonzero ==> normal */ {
		dbits |= 0x8000000000000000ull;
		denorm = ulpadj = 0;
		}
	else {
		denorm = 1;
		ulpadj = be + 1;
		dbits <<= 1;
		if (!(dbits & 0xffffffff00000000ull)) {
			dbits <<= 32;
			be -= 32;
			}
		if (!(dbits & 0xffff000000000000ull)) {
			dbits <<= 16;
			be -= 16;
			}
		if (!(dbits & 0xff00000000000000ull)) {
			dbits <<= 8;
			be -= 8;
			}
		if (!(dbits & 0xf000000000000000ull)) {
			dbits <<= 4;
			be -= 4;
			}
		if (!(dbits & 0xc000000000000000ull)) {
			dbits <<= 2;
			be -= 2;
			}
		if (!(dbits & 0x8000000000000000ull)) {
			dbits <<= 1;
			be -= 1;
			}
		ZEND_ASSERT(be >= -51);
		ulpadj -= be;
		}
	j = Lhint[be + 51];
	p10 = &pten[j];
	dbhi = dbits >> 32;
	dblo = dbits & 0xffffffffull;
	i = be - 0x3fe;
	if (i < p10->e
	|| (i == p10->e && (dbhi < p10->b0 || (dbhi == p10->b0 && dblo < p10->b1))))
		--j;
	k = j - 342;

	/* now 10^k <= dd < 10^(k+1) */

#else /*}{*/

	b = d2b(&u, &be, &bbits MTb);
#ifdef Sudden_Underflow
	i = (int)(word0(&u) >> Exp_shift1 & (Exp_mask>>Exp_shift1));
#else
	if ((i = (int)(word0(&u) >> Exp_shift1 & (Exp_mask>>Exp_shift1)))) {
#endif
		dval(&d2) = dval(&u);
		word0(&d2) &= Frac_mask1;
		word0(&d2) |= Exp_11;
#ifdef IBM
		if (j = 11 - hi0bits(word0(&d2) & Frac_mask))
			dval(&d2) /= 1 << j;
#endif

		/* log(x)	~=~ log(1.5) + (x-1.5)/1.5
		 * log10(x)	 =  log(x) / log(10)
		 *		~=~ log(1.5)/log(10) + (x-1.5)/(1.5*log(10))
		 * log10(d) = (i-Bias)*log(2)/log(10) + log10(d2)
		 *
		 * This suggests computing an approximation k to log10(d) by
		 *
		 * k = (i - Bias)*0.301029995663981
		 *	+ ( (d2-1.5)*0.289529654602168 + 0.176091259055681 );
		 *
		 * We want k to be too large rather than too small.
		 * The error in the first-order Taylor series approximation
		 * is in our favor, so we just round up the constant enough
		 * to compensate for any error in the multiplication of
		 * (i - Bias) by 0.301029995663981; since |i - Bias| <= 1077,
		 * and 1077 * 0.30103 * 2^-52 ~=~ 7.2e-14,
		 * adding 1e-13 to the constant term more than suffices.
		 * Hence we adjust the constant term to 0.1760912590558.
		 * (We could get a more accurate k by invoking log10,
		 *  but this is probably not worthwhile.)
		 */

		i -= Bias;
#ifdef IBM
		i <<= 2;
		i += j;
#endif
#ifndef Sudden_Underflow
		denorm = 0;
		}
	else {
		/* d is denormalized */

		i = bbits + be + (Bias + (P-1) - 1);
		x = i > 32  ? word0(&u) << (64 - i) | word1(&u) >> (i - 32)
			    : word1(&u) << (32 - i);
		dval(&d2) = x;
		word0(&d2) -= 31*Exp_msk1; /* adjust exponent */
		i -= (Bias + (P-1) - 1) + 1;
		denorm = 1;
		}
#endif
	ds = (dval(&d2)-1.5)*0.289529654602168 + 0.1760912590558 + i*0.301029995663981;
	k = (int)ds;
	if (ds < 0. && ds != k)
		k--;	/* want k = floor(ds) */
	k_check = 1;
	if (k >= 0 && k <= Ten_pmax) {
		if (dval(&u) < tens[k])
			k--;
		k_check = 0;
		}
	j = bbits - i - 1;
	if (j >= 0) {
		b2 = 0;
		s2 = j;
		}
	else {
		b2 = -j;
		s2 = 0;
		}
	if (k >= 0) {
		b5 = 0;
		s5 = k;
		s2 += k;
		}
	else {
		b2 -= k;
		b5 = -k;
		s5 = 0;
		}
#endif /*}}*/
	if (mode < 0 || mode > 9)
		mode = 0;

#ifndef USE_BF96
#ifndef SET_INEXACT
#ifdef Check_FLT_ROUNDS
	try_quick = Rounding == 1;
#else
	try_quick = 1;
#endif
#endif /*SET_INEXACT*/
#endif /*USE_BF96*/

	if (mode > 5) {
		mode -= 4;
#ifndef USE_BF96
		try_quick = 0;
#endif
		}
	leftright = 1;
	ilim = ilim1 = -1;	/* Values for cases 0 and 1; done here to */
				/* silence erroneous "gcc -Wall" warning. */
	switch(mode) {
		case 0:
		case 1:
			i = 18;
			ndigits = 0;
			break;
		case 2:
			leftright = 0;
			ZEND_FALLTHROUGH;
		case 4:
			if (ndigits <= 0)
				ndigits = 1;
			ilim = ilim1 = i = ndigits;
			break;
		case 3:
			leftright = 0;
			ZEND_FALLTHROUGH;
		case 5:
			i = ndigits + k + 1;
			ilim = i;
			ilim1 = i - 1;
			if (i <= 0)
				i = 1;
		}
	if (!buf) {
		buf = rv_alloc(i MTb);
		blen = sizeof(Bigint) + ((1 << ((int*)buf)[-1]) - 1)*sizeof(ULong) - sizeof(int);
		}
	else if (blen <= i) {
		buf = 0;
		if (rve)
			*rve = buf + i;
		return buf;
		}
	s = buf;

	/* Check for special case that d is a normalized power of 2. */

	spec_case = 0;
	if (mode < 2 || (leftright
#ifdef Honor_FLT_ROUNDS
			&& Rounding == 1
#endif
				)) {
		if (!word1(&u) && !(word0(&u) & Bndry_mask)
#ifndef Sudden_Underflow
		 && word0(&u) & (Exp_mask & ~Exp_msk1)
#endif
				) {
			/* The special case */
			spec_case = 1;
			}
		}

#ifdef USE_BF96 /*{*/
	b = 0;
	if (ilim < 0 && (mode == 3 || mode == 5)) {
		S = mhi = 0;
		goto no_digits;
		}
	i = 1;
	j = 52 + 0x3ff - be;
	ulpshift = 0;
	ulplo = 0;
	/* Can we do an exact computation with 64-bit integer arithmetic? */
	if (k < 0) {
		if (k < -25)
			goto toobig;
		res = dbits >> 11;
		n2 = pfivebits[k1 = -(k + 1)] + 53;
		j1 = j;
		if (n2 > 61) {
			ulpshift = n2 - 61;
			if (res & (ulpmask = (1ull << ulpshift) - 1))
				goto toobig;
			j -= ulpshift;
			res >>= ulpshift;
			}
		/* Yes. */
		res *= ulp = pfive[k1];
		if (ulpshift) {
			ulplo = ulp;
			ulp >>= ulpshift;
			}
		j += k;
		if (ilim == 0) {
			S = mhi = 0;
			if (res > (5ull << j))
				goto one_digit;
			goto no_digits;
			}
		goto no_div;
		}
	if (ilim == 0 && j + k >= 0) {
		S = mhi = 0;
		if ((dbits >> 11) > (pfive[k-1] << j))
			goto one_digit;
		goto no_digits;
		}
	if (k <= dtoa_divmax && j + k >= 0) {
		/* Another "yes" case -- we will use exact integer arithmetic. */
 use_exact:
		Debug(++dtoa_stats[3]);
		res = dbits >> 11;	/* residual */
		ulp = 1;
		if (k <= 0)
			goto no_div;
		j1 = j + k + 1;
		den = pfive[k-i] << (j1 - i);
		for(;;) {
			dig = res / den;
			*s++ = '0' + dig;
			if (!(res -= dig*den)) {
#ifdef SET_INEXACT
				inexact = 0;
				oldinexact = 1;
#endif
				goto retc;
				}
			if (ilim < 0) {
				ures = den - res;
				if (2*res <= ulp
				&& (spec_case ? 4*res <= ulp : (2*res < ulp || dig & 1)))
					goto ulp_reached;
				if (2*ures < ulp)
					goto Roundup;
				}
			else if (i == ilim) {
				switch(Rounding) {
				  case 0: goto retc;
				  case 2: goto Roundup;
				  }
				ures = 2*res;
				if (ures > den
				|| (ures == den && dig & 1)
				|| (spec_case && res <= ulp && 2*res >= ulp))
					goto Roundup;
				goto retc;
				}
			if (j1 < ++i) {
				res *= 10;
				ulp *= 10;
				}
			else {
				if (i > k)
					break;
				den = pfive[k-i] << (j1 - i);
				}
			}
 no_div:
		for(;;) {
			dig = den = res >> j;
			*s++ = '0' + dig;
			if (!(res -= den << j)) {
#ifdef SET_INEXACT
				inexact = 0;
				oldinexact = 1;
#endif
				goto retc;
				}
			if (ilim < 0) {
				ures = (1ull << j) - res;
				if (2*res <= ulp
				&& (spec_case ? 4*res <= ulp : (2*res < ulp || dig & 1))) {
 ulp_reached:
					if (ures < res
					|| (ures == res && dig & 1)
					|| (dig == 9 && 2*ures <= ulp))
						goto Roundup;
					goto retc;
					}
				if (2*ures < ulp)
					goto Roundup;
				}
			--j;
			if (i == ilim) {
#ifdef Honor_FLT_ROUNDS
				switch(Rounding) {
				  case 0: goto retc;
				  case 2: goto Roundup;
				  }
#endif
				hb = 1ull << j;
				if (res & hb && (dig & 1 || res & (hb-1)))
					goto Roundup;
				if (spec_case && res <= ulp && 2*res >= ulp) {
 Roundup:
					while(*--s == '9')
						if (s == buf) {
							++k;
							*s++ = '1';
							goto ret1;
							}
					++*s++;
					goto ret1;
					}
				goto retc;
				}
			++i;
			res *= 5;
			if (ulpshift) {
				ulplo = 5*(ulplo & ulpmask);
				ulp = 5*ulp + (ulplo >> ulpshift);
				}
			else
				ulp *= 5;
			}
		}
 toobig:
	if (ilim > 28)
		goto Fast_failed1;
	/* Scale by 10^-k */
	p10 = &pten[342-k];
	tv0 = p10->b2 * dblo; /* rarely matters, but does, e.g., for 9.862818194192001e18 */
	tv1 = p10->b1 * dblo + (tv0 >> 32);
	tv2 = p10->b2 * dbhi + (tv1 & 0xffffffffull);
	tv3 = p10->b0 * dblo + (tv1>>32) + (tv2>>32);
	res3 = p10->b1 * dbhi + (tv3 & 0xffffffffull);
	res = p10->b0 * dbhi + (tv3>>32) + (res3>>32);
	be += p10->e - 0x3fe;
	eulp = j1 = be - 54 + ulpadj;
	if (!(res & 0x8000000000000000ull)) {
		--be;
		res3 <<= 1;
		res = (res << 1) | ((res3 & 0x100000000ull) >> 32);
		}
	res0 = res; /* save for Fast_failed */
#if !defined(SET_INEXACT) && !defined(NO_DTOA_64) /*{*/
	if (ilim > 19)
		goto Fast_failed;
	Debug(++dtoa_stats[4]);
	ZEND_ASSERT(be >= 0 && be <= 4); /* be = 0 is rare, but possible, e.g., for 1e20 */
	res >>= 4 - be;
	ulp = p10->b0;	/* ulp */
	ulp = (ulp << 29) | (p10->b1 >> 3);
	/* scaled ulp = ulp * 2^(eulp - 60) */
	/* We maintain 61 bits of the scaled ulp. */
	if (ilim == 0) {
		if (!(res & 0x7fffffffffffffeull)
		 || !((~res) & 0x7fffffffffffffeull))
			goto Fast_failed1;
		S = mhi = 0;
		if (res >= 0x5000000000000000ull)
			goto one_digit;
		goto no_digits;
		}
	rb = 1;	/* upper bound on rounding error */
	for(;;++i) {
		dig = res >> 60;
		*s++ = '0' + dig;
		res &= 0xfffffffffffffffull;
		if (ilim < 0) {
			ures = 0x1000000000000000ull - res;
			if (eulp > 0) {
				ZEND_ASSERT(eulp <= 4);
				sulp = ulp << (eulp - 1);
				if (res <= ures) {
					if (res + rb > ures - rb)
						goto Fast_failed;
					if (res < sulp)
						goto retc;
					}
				else {
					if (res - rb <= ures + rb)
						goto Fast_failed;
					if (ures < sulp)
						goto Roundup;
					}
				}
			else {
				zb = -(1ull << (eulp + 63));
				if (!(zb & res)) {
					sres = res << (1 - eulp);
					if (sres < ulp && (!spec_case || 2*sres < ulp)) {
						if ((res+rb) << (1 - eulp) >= ulp)
							goto Fast_failed;
						if (ures < res) {
							if (ures + rb >= res - rb)
								goto Fast_failed;
							goto Roundup;
							}
						if (ures - rb < res + rb)
							goto Fast_failed;
						goto retc;
						}
					}
				if (!(zb & ures) && ures << -eulp < ulp) {
					if (ures << (1 - eulp) < ulp)
						goto  Roundup;
					goto Fast_failed;
					}
				}
			}
		else if (i == ilim) {
			ures = 0x1000000000000000ull - res;
			if (ures < res) {
				if (ures <= rb || res - rb <= ures + rb) {
					if (j + k >= 0 && k >= 0 && k <= 27)
						goto use_exact1;
					goto Fast_failed;
					}
#ifdef Honor_FLT_ROUNDS
				if (Rounding == 0)
					goto retc;
#endif
				goto Roundup;
				}
			if (res <= rb || ures - rb <= res + rb) {
				if (j + k >= 0 && k >= 0 && k <= 27) {
 use_exact1:
					s = buf;
					i = 1;
					goto use_exact;
					}
				goto Fast_failed;
				}
#ifdef Honor_FLT_ROUNDS
			if (Rounding == 2)
				goto Roundup;
#endif
			goto retc;
			}
		rb *= 10;
		if (rb >= 0x1000000000000000ull)
			goto Fast_failed;
		res *= 10;
		ulp *= 5;
		if (ulp & 0x8000000000000000ull) {
			eulp += 4;
			ulp >>= 3;
			}
		else {
			eulp += 3;
			ulp >>= 2;
			}
		}
#endif /*}*/
#ifndef NO_BF96
 Fast_failed:
#endif
	Debug(++dtoa_stats[5]);
	s = buf;
	i = 4 - be;
	res = res0 >> i;
	reslo = 0xffffffffull & res3;
	if (i)
		reslo = (res0 << (64 - i)) >> 32 | (reslo >> i);
	rb = 0;
	rblo = 4; /* roundoff bound */
	ulp = p10->b0;	/* ulp */
	ulp = (ulp << 29) | (p10->b1 >> 3);
	eulp = j1;
	for(i = 1;;++i) {
		dig = res >> 60;
		*s++ = '0' + dig;
		res &= 0xfffffffffffffffull;
#ifdef SET_INEXACT
		if (!res && !reslo) {
			if (!(res3 & 0xffffffffull)) {
				inexact = 0;
				oldinexact = 1;
				}
			goto retc;
			}
#endif
		if (ilim < 0) {
			ures = 0x1000000000000000ull - res;
			ureslo = 0;
			if (reslo) {
				ureslo = 0x100000000ull - reslo;
				--ures;
				}
			if (eulp > 0) {
				ZEND_ASSERT(eulp <= 4);
				sulp = (ulp << (eulp - 1)) - rb;
				if (res <= ures) {
					if (res < sulp) {
						if (res+rb < ures-rb)
							goto retc;
						}
					}
				else if (ures < sulp) {
					if (res-rb > ures+rb)
						goto Roundup;
					}
				goto Fast_failed1;
				}
			else {
				zb = -(1ull << (eulp + 60));
				if (!(zb & (res + rb))) {
					sres = (res - rb) << (1 - eulp);
					if (sres < ulp && (!spec_case || 2*sres < ulp)) {
						sres = res << (1 - eulp);
						if ((j = eulp + 31) > 0)
							sres += (rblo + reslo) >> j;
						else
							sres += (rblo + reslo) << -j;
						if (sres + (rb << (1 - eulp)) >= ulp)
							goto Fast_failed1;
						if (sres >= ulp)
							goto more96;
						if (ures < res
						|| (ures == res && ureslo < reslo)) {
							if (ures + rb >= res - rb)
								goto Fast_failed1;
							goto Roundup;
							}
						if (ures - rb <= res + rb)
							goto Fast_failed1;
						goto retc;
						}
					}
				if (!(zb & ures) && (ures-rb) << (1 - eulp) < ulp) {
					if ((ures + rb) << (2 - eulp) < ulp)
						goto Roundup;
					goto Fast_failed1;
					}
				}
			}
		else if (i == ilim) {
			ures = 0x1000000000000000ull - res;
			sres = ureslo = 0;
			if (reslo) {
				ureslo = 0x100000000ull - reslo;
				--ures;
				sres = (reslo + rblo) >> 31;
				}
			sres += 2*rb;
			if (ures <= res) {
				if (ures <=sres || res - ures <= sres)
					goto Fast_failed1;
#ifdef Honor_FLT_ROUNDS
				if (Rounding == 0)
					goto retc;
#endif
				goto Roundup;
				}
			if (res <= sres || ures - res <= sres)
				goto Fast_failed1;
#ifdef Honor_FLT_ROUNDS
			if (Rounding == 2)
				goto Roundup;
#endif
			goto retc;
			}
 more96:
		rblo *= 10;
		rb = 10*rb + (rblo >> 32);
		rblo &= 0xffffffffull;
		if (rb >= 0x1000000000000000ull)
			goto Fast_failed1;
		reslo *= 10;
		res = 10*res + (reslo >> 32);
		reslo &= 0xffffffffull;
		ulp *= 5;
		if (ulp & 0x8000000000000000ull) {
			eulp += 4;
			ulp >>= 3;
			}
		else {
			eulp += 3;
			ulp >>= 2;
			}
		}
 Fast_failed1:
	Debug(++dtoa_stats[6]);
	S = mhi = mlo = 0;
#ifdef USE_BF96
	b = d2b(&u, &be, &bbits MTb);
#endif
	s = buf;
	i = (int)(word0(&u) >> Exp_shift1 & (Exp_mask>>Exp_shift1));
	i -= Bias;
	if (ulpadj)
		i -= ulpadj - 1;
	j = bbits - i - 1;
	if (j >= 0) {
		b2 = 0;
		s2 = j;
		}
	else {
		b2 = -j;
		s2 = 0;
		}
	if (k >= 0) {
		b5 = 0;
		s5 = k;
		s2 += k;
		}
	else {
		b2 -= k;
		b5 = -k;
		s5 = 0;
		}
#endif /*}*/

#ifdef Honor_FLT_ROUNDS
	if (mode > 1 && Rounding != 1)
		leftright = 0;
#endif

#ifndef USE_BF96 /*{*/
	if (ilim >= 0 && ilim <= Quick_max && try_quick) {

		/* Try to get by with floating-point arithmetic. */

		i = 0;
		dval(&d2) = dval(&u);
		j1 = -(k0 = k);
		ilim0 = ilim;
		ieps = 2; /* conservative */
		if (k > 0) {
			ds = tens[k&0xf];
			j = k >> 4;
			if (j & Bletch) {
				/* prevent overflows */
				j &= Bletch - 1;
				dval(&u) /= bigtens[n_bigtens-1];
				ieps++;
				}
			for(; j; j >>= 1, i++)
				if (j & 1) {
					ieps++;
					ds *= bigtens[i];
					}
			dval(&u) /= ds;
			}
		else if (j1 > 0) {
			dval(&u) *= tens[j1 & 0xf];
			for(j = j1 >> 4; j; j >>= 1, i++)
				if (j & 1) {
					ieps++;
					dval(&u) *= bigtens[i];
					}
			}
		if (k_check && dval(&u) < 1. && ilim > 0) {
			if (ilim1 <= 0)
				goto fast_failed;
			ilim = ilim1;
			k--;
			dval(&u) *= 10.;
			ieps++;
			}
		dval(&eps) = ieps*dval(&u) + 7.;
		word0(&eps) -= (P-1)*Exp_msk1;
		if (ilim == 0) {
			S = mhi = 0;
			dval(&u) -= 5.;
			if (dval(&u) > dval(&eps))
				goto one_digit;
			if (dval(&u) < -dval(&eps))
				goto no_digits;
			goto fast_failed;
			}
#ifndef No_leftright
		if (leftright) {
			/* Use Steele & White method of only
			 * generating digits needed.
			 */
			dval(&eps) = 0.5/tens[ilim-1] - dval(&eps);
#ifdef IEEE_Arith
			if (j1 >= 307) {
				eps1.d = 1.01e256; /* 1.01 allows roundoff in the next few lines */
				word0(&eps1) -= Exp_msk1 * (Bias+P-1);
				dval(&eps1) *= tens[j1 & 0xf];
				for(i = 0, j = (j1-256) >> 4; j; j >>= 1, i++)
					if (j & 1)
						dval(&eps1) *= bigtens[i];
				if (eps.d < eps1.d)
					eps.d = eps1.d;
				if (10. - u.d < 10.*eps.d && eps.d < 1.) {
					/* eps.d < 1. excludes trouble with the tiniest denormal */
					*s++ = '1';
					++k;
					goto ret1;
					}
				}
#endif
			for(i = 0;;) {
				L = dval(&u);
				dval(&u) -= L;
				*s++ = '0' + (int)L;
				if (1. - dval(&u) < dval(&eps))
					goto bump_up;
				if (dval(&u) < dval(&eps))
					goto retc;
				if (++i >= ilim)
					break;
				dval(&eps) *= 10.;
				dval(&u) *= 10.;
				}
			}
		else {
#endif
			/* Generate ilim digits, then fix them up. */
			dval(&eps) *= tens[ilim-1];
			for(i = 1;; i++, dval(&u) *= 10.) {
				L = (Long)(dval(&u));
				if (!(dval(&u) -= L))
					ilim = i;
				*s++ = '0' + (int)L;
				if (i == ilim) {
					if (dval(&u) > 0.5 + dval(&eps))
						goto bump_up;
					else if (dval(&u) < 0.5 - dval(&eps))
						goto retc;
					break;
					}
				}
#ifndef No_leftright
			}
#endif
 fast_failed:
		s = buf;
		dval(&u) = dval(&d2);
		k = k0;
		ilim = ilim0;
		}

	/* Do we have a "small" integer? */

	if (be >= 0 && k <= Int_max) {
		/* Yes. */
		ds = tens[k];
		if (ndigits < 0 && ilim <= 0) {
			S = mhi = 0;
			if (ilim < 0 || dval(&u) <= 5*ds)
				goto no_digits;
			goto one_digit;
			}
		for(i = 1;; i++, dval(&u) *= 10.) {
			L = (Long)(dval(&u) / ds);
			dval(&u) -= L*ds;
#ifdef Check_FLT_ROUNDS
			/* If FLT_ROUNDS == 2, L will usually be high by 1 */
			if (dval(&u) < 0) {
				L--;
				dval(&u) += ds;
				}
#endif
			*s++ = '0' + (int)L;
			if (!dval(&u)) {
#ifdef SET_INEXACT
				inexact = 0;
#endif
				break;
				}
			if (i == ilim) {
#ifdef Honor_FLT_ROUNDS
				if (mode > 1)
				switch(Rounding) {
				  case 0: goto retc;
				  case 2: goto bump_up;
				  }
#endif
				dval(&u) += dval(&u);
#ifdef ROUND_BIASED
				if (dval(&u) >= ds)
#else
				if (dval(&u) > ds || (dval(&u) == ds && L & 1))
#endif
					{
 bump_up:
					while(*--s == '9')
						if (s == buf) {
							k++;
							*s = '0';
							break;
							}
					++*s++;
					}
				break;
				}
			}
		goto retc;
		}

#endif /*}*/
	m2 = b2;
	m5 = b5;
	mhi = mlo = 0;
	if (leftright) {
		i =
#ifndef Sudden_Underflow
			denorm ? be + (Bias + (P-1) - 1 + 1) :
#endif
#ifdef IBM
			1 + 4*P - 3 - bbits + ((bbits + be - 1) & 3);
#else
			1 + P - bbits;
#endif
		b2 += i;
		s2 += i;
		mhi = i2b(1 MTb);
		}
	if (m2 > 0 && s2 > 0) {
		i = m2 < s2 ? m2 : s2;
		b2 -= i;
		m2 -= i;
		s2 -= i;
		}
	if (b5 > 0) {
		if (leftright) {
			if (m5 > 0) {
				mhi = pow5mult(mhi, m5 MTb);
				b1 = mult(mhi, b MTb);
				Bfree(b MTb);
				b = b1;
				}
			if ((j = b5 - m5))
				b = pow5mult(b, j MTb);
			}
		else
			b = pow5mult(b, b5 MTb);
		}
	S = i2b(1 MTb);
	if (s5 > 0)
		S = pow5mult(S, s5 MTb);

	if (spec_case) {
		b2 += Log2P;
		s2 += Log2P;
		}

	/* Arrange for convenient computation of quotients:
	 * shift left if necessary so divisor has 4 leading 0 bits.
	 *
	 * Perhaps we should just compute leading 28 bits of S once
	 * and for all and pass them and a shift to quorem, so it
	 * can do shifts and ORs to compute the numerator for q.
	 */
	i = dshift(S, s2);
	b2 += i;
	m2 += i;
	s2 += i;
	if (b2 > 0)
		b = lshift(b, b2 MTb);
	if (s2 > 0)
		S = lshift(S, s2 MTb);
#ifndef USE_BF96
	if (k_check) {
		if (cmp(b,S) < 0) {
			k--;
			b = multadd(b, 10, 0 MTb);	/* we botched the k estimate */
			if (leftright)
				mhi = multadd(mhi, 10, 0 MTb);
			ilim = ilim1;
			}
		}
#endif
	if (ilim <= 0 && (mode == 3 || mode == 5)) {
		if (ilim < 0 || cmp(b,S = multadd(S,5,0 MTb)) <= 0) {
			/* no digits, fcvt style */
 no_digits:
			k = -1 - ndigits;
			goto ret;
			}
 one_digit:
		*s++ = '1';
		++k;
		goto ret;
		}
	if (leftright) {
		if (m2 > 0)
			mhi = lshift(mhi, m2 MTb);

		/* Compute mlo -- check for special case
		 * that d is a normalized power of 2.
		 */

		mlo = mhi;
		if (spec_case) {
			mhi = Balloc(mhi->k MTb);
			Bcopy(mhi, mlo);
			mhi = lshift(mhi, Log2P MTb);
			}

		for(i = 1;;i++) {
			dig = quorem(b,S) + '0';
			/* Do we yet have the shortest decimal string
			 * that will round to d?
			 */
			j = cmp(b, mlo);
			delta = diff(S, mhi MTb);
			j1 = delta->sign ? 1 : cmp(b, delta);
			Bfree(delta MTb);
#ifndef ROUND_BIASED
			if (j1 == 0 && mode != 1 && !(word1(&u) & 1)
#ifdef Honor_FLT_ROUNDS
				&& (mode <= 1 || Rounding >= 1)
#endif
								   ) {
				if (dig == '9')
					goto round_9_up;
				if (j > 0)
					dig++;
#ifdef SET_INEXACT
				else if (!b->x[0] && b->wds <= 1)
					inexact = 0;
#endif
				*s++ = dig;
				goto ret;
				}
#endif
			if (j < 0 || (j == 0 && mode != 1
#ifndef ROUND_BIASED
							&& !(word1(&u) & 1)
#endif
					)) {
				if (!b->x[0] && b->wds <= 1) {
#ifdef SET_INEXACT
					inexact = 0;
#endif
					goto accept_dig;
					}
#ifdef Honor_FLT_ROUNDS
				if (mode > 1)
				 switch(Rounding) {
				  case 0: goto accept_dig;
				  case 2: goto keep_dig;
				  }
#endif /*Honor_FLT_ROUNDS*/
				if (j1 > 0) {
					b = lshift(b, 1 MTb);
					j1 = cmp(b, S);
#ifdef ROUND_BIASED
					if (j1 >= 0 /*)*/
#else
					if ((j1 > 0 || (j1 == 0 && dig & 1))
#endif
					&& dig++ == '9')
						goto round_9_up;
					}
 accept_dig:
				*s++ = dig;
				goto ret;
				}
			if (j1 > 0) {
#ifdef Honor_FLT_ROUNDS
				if (!Rounding && mode > 1)
					goto accept_dig;
#endif
				if (dig == '9') { /* possible if i == 1 */
 round_9_up:
					*s++ = '9';
					goto roundoff;
					}
				*s++ = dig + 1;
				goto ret;
				}
#ifdef Honor_FLT_ROUNDS
 keep_dig:
#endif
			*s++ = dig;
			if (i == ilim)
				break;
			b = multadd(b, 10, 0 MTb);
			if (mlo == mhi)
				mlo = mhi = multadd(mhi, 10, 0 MTb);
			else {
				mlo = multadd(mlo, 10, 0 MTb);
				mhi = multadd(mhi, 10, 0 MTb);
				}
			}
		}
	else
		for(i = 1;; i++) {
			dig = quorem(b,S) + '0';
			*s++ = dig;
			if (!b->x[0] && b->wds <= 1) {
#ifdef SET_INEXACT
				inexact = 0;
#endif
				goto ret;
				}
			if (i >= ilim)
				break;
			b = multadd(b, 10, 0 MTb);
			}

	/* Round off last digit */

#ifdef Honor_FLT_ROUNDS
	if (mode > 1)
		switch(Rounding) {
		  case 0: goto ret;
		  case 2: goto roundoff;
		  }
#endif
	b = lshift(b, 1 MTb);
	j = cmp(b, S);
#ifdef ROUND_BIASED
	if (j >= 0)
#else
	if (j > 0 || (j == 0 && dig & 1))
#endif
		{
 roundoff:
		while(*--s == '9')
			if (s == buf) {
				k++;
				*s++ = '1';
				goto ret;
				}
		++*s++;
		}
 ret:
	Bfree(S MTb);
	if (mhi) {
		if (mlo && mlo != mhi)
			Bfree(mlo MTb);
		Bfree(mhi MTb);
		}
 retc:
	while(s > buf && s[-1] == '0')
		--s;
 ret1:
	if (b)
		Bfree(b MTb);
	*s = 0;
	*decpt = k + 1;
	if (rve)
		*rve = s;
#ifdef SET_INEXACT
	if (inexact) {
		if (!oldinexact) {
			word0(&u) = Exp_1 + (70 << Exp_shift);
			word1(&u) = 0;
			dval(&u) += 1.;
			}
		}
	else if (!oldinexact)
		clear_inexact();
#endif
	return buf;
	}

ZEND_API char *
zend_dtoa(double dd, int mode, int ndigits, int *decpt, bool *sign, char **rve)
{
	/*	Sufficient space is allocated to the return value
		to hold the suppressed trailing zeros.
		See dtoa_r() above for details on the other arguments.
	*/
#ifndef MULTIPLE_THREADS
	if (dtoa_result)
		zend_freedtoa(dtoa_result);
#endif
	return dtoa_r(dd, mode, ndigits, decpt, sign, rve, 0, 0);
	}

ZEND_API double zend_hex_strtod(const char *str, const char **endptr)
{
	const char *s = str;
	char c;
	int any = 0;
	double value = 0;

	if (*s == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
	}

	while ((c = *s++)) {
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c -= 'A' - 10;
		} else if (c >= 'a' && c <= 'f') {
			c -= 'a' - 10;
		} else {
			break;
		}

		any = 1;
		value = value * 16 + c;
	}

	if (endptr != NULL) {
		*endptr = any ? s - 1 : str;
	}

	return value;
}

ZEND_API double zend_oct_strtod(const char *str, const char **endptr)
{
	const char *s = str;
	char c;
	double value = 0;
	int any = 0;

	if (str[0] == '\0') {
		if (endptr != NULL) {
			*endptr = str;
		}
		return 0.0;
	}

	while ((c = *s++)) {
		if (c < '0' || c > '7') {
			/* break and return the current value if the number is not well-formed
			 * that's what Linux strtol() does
			 */
			break;
		}
		value = value * 8 + c - '0';
		any = 1;
	}

	if (endptr != NULL) {
		*endptr = any ? s - 1 : str;
	}

	return value;
}

ZEND_API double zend_bin_strtod(const char *str, const char **endptr)
{
	const char *s = str;
	char 		c;
	double 		value = 0;
	int 		any = 0;

	if ('0' == *s && ('b' == s[1] || 'B' == s[1])) {
		s += 2;
	}

	while ((c = *s++)) {
		/*
		 * Verify the validity of the current character as a base-2 digit.  In
		 * the event that an invalid digit is found, halt the conversion and
		 * return the portion which has been converted thus far.
		 */
		if ('0' == c || '1' == c)
			value = value * 2 + c - '0';
		else
			break;

		any = 1;
	}

	/*
	 * As with many strtoX implementations, should the subject sequence be
	 * empty or not well-formed, no conversion is performed and the original
	 * value of str is stored in *endptr, provided that endptr is not a null
	 * pointer.
	 */
	if (NULL != endptr) {
		*endptr = (char *)(any ? s - 1 : str);
	}

	return value;
}

ZEND_API char *zend_gcvt(double value, int ndigit, char dec_point, char exponent, char *buf)
{
	char *digits, *dst, *src;
	int i, decpt;
	bool sign;
	int mode = ndigit >= 0 ? 2 : 0;

	if (mode == 0) {
		ndigit = 17;
	}
	digits = zend_dtoa(value, mode, ndigit, &decpt, &sign, NULL);
	if (decpt == 9999) {
		/*
		 * Infinity or NaN, convert to inf or nan with sign.
		 * We assume the buffer is at least ndigit long.
		 */
		snprintf(buf, ndigit + 1, "%s%s", (sign && *digits == 'I') ? "-" : "", *digits == 'I' ? "INF" : "NAN");
		zend_freedtoa(digits);
		return (buf);
	}

	dst = buf;
	if (sign) {
		*dst++ = '-';
	}

	if ((decpt >= 0 && decpt > ndigit) || decpt < -3) { /* use E-style */
		/* exponential format (e.g. 1.2345e+13) */
		if (--decpt < 0) {
			sign = true;
			decpt = -decpt;
		} else {
			sign = false;
		}
		src = digits;
		*dst++ = *src++;
		*dst++ = dec_point;
		if (*src == '\0') {
			*dst++ = '0';
		} else {
			do {
				*dst++ = *src++;
			} while (*src != '\0');
		}
		*dst++ = exponent;
		if (sign) {
			*dst++ = '-';
		} else {
			*dst++ = '+';
		}
		if (decpt < 10) {
			*dst++ = '0' + decpt;
			*dst = '\0';
		} else {
			/* XXX - optimize */
			int n;
			for (n = decpt, i = 0; (n /= 10) != 0; i++);
			dst[i + 1] = '\0';
			while (decpt != 0) {
				dst[i--] = '0' + decpt % 10;
				decpt /= 10;
			}
		}
	} else if (decpt < 0) {
		/* standard format 0. */
		*dst++ = '0';   /* zero before decimal point */
		*dst++ = dec_point;
		do {
			*dst++ = '0';
		} while (++decpt < 0);
		src = digits;
		while (*src != '\0') {
			*dst++ = *src++;
		}
		*dst = '\0';
	} else {
		/* standard format */
		for (i = 0, src = digits; i < decpt; i++) {
			if (*src != '\0') {
				*dst++ = *src++;
			} else {
				*dst++ = '0';
			}
		}
		if (*src != '\0') {
			if (src == digits) {
				*dst++ = '0';   /* zero before decimal point */
			}
			*dst++ = dec_point;
			for (i = decpt; digits[i] != '\0'; i++) {
				*dst++ = digits[i];
			}
		}
		*dst = '\0';
	}
	zend_freedtoa(digits);
	return (buf);
}

static void destroy_freelist(void)
{
	int i;
	Bigint *tmp;

	for (i = 0; i <= Kmax; i++) {
		Bigint **listp = &freelist[i];
		while ((tmp = *listp) != NULL) {
			*listp = tmp->next;
			FREE(tmp);
		}
		freelist[i] = NULL;
	}
}

static void free_p5s(void)
{
	Bigint **listp, *tmp;

	listp = &p5s;
	while ((tmp = *listp) != NULL) {
		*listp = tmp->next;
		FREE(tmp);
	}
	p5s = NULL;
}
