#include "gd.h"
#include "gd_intern.h"
#include "gd_filter_simd.h"

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
#include <emmintrin.h>

static void gdImageSmooth_sse2_row(gdImagePtr im, gdImagePtr srcback, int y, float weight)
{
	int x;
	const int sx = im->sx;
	const float div = weight + 8.0f;
	const __m128 div_ps = _mm_set1_ps(div);
	const __m128 weight_ps = _mm_set1_ps(weight - 1.0f);

	for (x = 0; x < sx - 3; x+=4) {
		int i, j;
		__m128 sum_r = _mm_setzero_ps();
		__m128 sum_g = _mm_setzero_ps();
		__m128 sum_b = _mm_setzero_ps();
		__m128i center_pixels = _mm_loadu_si128((__m128i*)&srcback->tpixels[y][x]);

		for (j = -1; j <= 1; j++) {
			for (i = -1; i <= 1; i++) {
				__m128i pixels = _mm_loadu_si128((__m128i*)&srcback->tpixels[y+j][x+i]);
				__m128i r = _mm_and_si128(_mm_srli_epi32(pixels, 16), _mm_set1_epi32(0xff));
				__m128i g = _mm_and_si128(_mm_srli_epi32(pixels, 8), _mm_set1_epi32(0xff));
				__m128i b = _mm_and_si128(pixels, _mm_set1_epi32(0xff));

				sum_r = _mm_add_ps(sum_r, _mm_cvtepi32_ps(r));
				sum_g = _mm_add_ps(sum_g, _mm_cvtepi32_ps(g));
				sum_b = _mm_add_ps(sum_b, _mm_cvtepi32_ps(b));
			}
		}

		__m128i center_r = _mm_and_si128(_mm_srli_epi32(center_pixels, 16), _mm_set1_epi32(0xff));
		__m128i center_g = _mm_and_si128(_mm_srli_epi32(center_pixels, 8), _mm_set1_epi32(0xff));
		__m128i center_b = _mm_and_si128(center_pixels, _mm_set1_epi32(0xff));

		sum_r = _mm_add_ps(sum_r, _mm_mul_ps(_mm_cvtepi32_ps(center_r), weight_ps));
		sum_g = _mm_add_ps(sum_g, _mm_mul_ps(_mm_cvtepi32_ps(center_g), weight_ps));
		sum_b = _mm_add_ps(sum_b, _mm_mul_ps(_mm_cvtepi32_ps(center_b), weight_ps));

		sum_r = _mm_div_ps(sum_r, div_ps);
		sum_g = _mm_div_ps(sum_g, div_ps);
		sum_b = _mm_div_ps(sum_b, div_ps);

		sum_r = _mm_max_ps(_mm_setzero_ps(), _mm_min_ps(sum_r, _mm_set1_ps(255.0f)));
		sum_g = _mm_max_ps(_mm_setzero_ps(), _mm_min_ps(sum_g, _mm_set1_ps(255.0f)));
		sum_b = _mm_max_ps(_mm_setzero_ps(), _mm_min_ps(sum_b, _mm_set1_ps(255.0f)));

		__m128i r_int = _mm_cvtps_epi32(sum_r);
		__m128i g_int = _mm_cvtps_epi32(sum_g);
		__m128i b_int = _mm_cvtps_epi32(sum_b);

		__m128i alpha = _mm_and_si128(_mm_srli_epi32(center_pixels, 24), _mm_set1_epi32(0xff));
		r_int = _mm_slli_epi32(r_int, 16);
		g_int = _mm_slli_epi32(g_int, 8);
		alpha = _mm_slli_epi32(alpha, 24);

		__m128i final_pixels = _mm_or_si128(_mm_or_si128(r_int, g_int), _mm_or_si128(b_int, alpha));
		_mm_storeu_si128((__m128i*)&im->tpixels[y][x], final_pixels);
	}

	for (; x < sx; x++) {
		float new_r = 0.0f, new_g = 0.0f, new_b = 0.0f;
		int i, j;

		for (j = -1; j <= 1; j++) {
			for (i = -1; i <= 1; i++) {
				int pxl = im->tpixels[y+j][x+i];
				new_r += gdTrueColorGetRed(pxl);
				new_g += gdTrueColorGetGreen(pxl);
				new_b += gdTrueColorGetBlue(pxl);
			}
		}

		int pxl = im->tpixels[y][x];
		new_r += gdTrueColorGetRed(pxl) * (weight - 1.0f);
		new_g += gdTrueColorGetGreen(pxl) * (weight - 1.0f);
		new_b += gdTrueColorGetBlue(pxl) * (weight - 1.0f);

		new_r /= div;
		new_g /= div;
		new_b /= div;

		new_r = new_r < 0 ? 0 : (new_r > 255 ? 255 : new_r);
		new_g = new_g < 0 ? 0 : (new_g > 255 ? 255 : new_g);
		new_b = new_b < 0 ? 0 : (new_b > 255 ? 255 : new_b);

		im->tpixels[y][x] = gdTrueColorAlpha((int)new_r, (int)new_g, (int)new_b, gdTrueColorGetAlpha(pxl));
	}
}


int gdImageSmooth_sse2(gdImagePtr im, float weight)
{
	if (!im->trueColor) {
		return 0;
	}

	gdImagePtr srcback = gdImageCreateTrueColor(im->sx, im->sy);
	if (!srcback) {
		return 0;
	}
	gdImageCopy(srcback, im, 0, 0, 0, 0, im->sx, im->sy);

	for (int y = 1; y < im->sy - 1; y++) {
		gdImageSmooth_sse2_row(im, srcback, y, weight);
	}

	gdImageDestroy(srcback);
	return 1;
}
#else
int gdImageSmooth_sse2(gdImagePtr im, float weight)
{
	return 0;
}
#endif

int gdImageSmooth_avx(gdImagePtr im, float weight)
{
	/* TODO: Add AVX implementation */
	return 0;
}
