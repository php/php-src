#include "gd_simd.h"

#if defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#elif defined(_MSC_VER)
#include <intrin.h>
#endif

static int gd_simd_features = -1;

#define GD_SIMD_AVX (1 << 0)
#define GD_SIMD_SSE2 (1 << 1)

static void gd_check_simd_features(void)
{
	gd_simd_features = 0;

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
	unsigned int eax, ebx, ecx, edx;

	if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
		if (edx & (1 << 26)) {
			gd_simd_features |= GD_SIMD_SSE2;
		}
		if (ecx & (1 << 28)) {
			gd_simd_features |= GD_SIMD_AVX;
		}
	}
#elif defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
	int info[4];
	__cpuid(info, 1);
	if (info[3] & (1 << 26)) {
		gd_simd_features |= GD_SIMD_SSE2;
	}
	if (info[2] & (1 << 28)) {
		gd_simd_features |= GD_SIMD_AVX;
	}
#endif
}

int gd_have_avx(void)
{
	if (gd_simd_features < 0) {
		gd_check_simd_features();
	}
	return (gd_simd_features & GD_SIMD_AVX) != 0;
}

int gd_have_sse2(void)
{
	if (gd_simd_features < 0) {
		gd_check_simd_features();
	}
	return (gd_simd_features & GD_SIMD_SSE2) != 0;
}
