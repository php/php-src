#include "mbfilter_cjk.h"

#include "unicode_table_jis.h"
#include "unicode_table_jis2004.h"
#include "unicode_table_big5.h"
#include "unicode_table_cns11643.h"
#include "unicode_table_cp932_ext.h"
#include "unicode_table_cp936.h"
#include "unicode_table_gb18030.h"
#include "unicode_table_gb2312.h"
#include "unicode_table_uhc.h"
#include "cp932_table.h"
#include "sjis_mac2uni.h"
#include "translit_kana_jisx0201_jisx0208.h"
#include "emoji2uni.h"

/* Regional Indicator Unicode codepoints are from 0x1F1E6-0x1F1FF
 * These correspond to the letters A-Z
 * To display the flag emoji for a country, two unicode codepoints are combined,
 * which correspond to the two-letter code for that country
 * This macro converts uppercase ASCII values to Regional Indicator codepoints */
#define NFLAGS(c) (0x1F1A5+((unsigned int)(c)))

static const char nflags_s[10][2] = {"CN", "DE", "ES", "FR", "GB", "IT", "JP", "KR", "RU", "US"};
static const int nflags_code_kddi[10] = { 0x2549, 0x2546, 0x24C0, 0x2545, 0x2548, 0x2547, 0x2750, 0x254A, 0x24C1, 0x27F7 };
static const int nflags_code_sb[10] = { 0x2B0A, 0x2B05, 0x2B08, 0x2B04, 0x2B07, 0x2B06, 0x2B02, 0x2B0B, 0x2B09, 0x2B03 };

#define EMIT_KEYPAD_EMOJI(c) do { *snd = (c); return 0x20E3; } while(0)
#define EMIT_FLAG_EMOJI(country) do { *snd = NFLAGS((country)[0]); return NFLAGS((country)[1]); } while(0)

static const char nflags_kddi[6][2] = {"FR", "DE", "IT", "GB", "CN", "KR"};
static const char nflags_sb[10][2] = {"JP", "US", "FR", "DE", "IT", "GB", "ES", "RU", "CN", "KR"};

/* number -> (ku*94)+ten value for telephone keypad character */
#define DOCOMO_KEYPAD(n) ((n) == 0 ? 0x296F : (0x2965 + (n)))
#define DOCOMO_KEYPAD_HASH 0x2964

/* `tbl` contains inclusive ranges, each represented by a pair of unsigned shorts */
static int mbfl_bisec_srch(int w, const unsigned short *tbl, int n)
{
	int l = 0, r = n-1;
	while (l <= r) {
		int probe = (l + r) >> 1;
		unsigned short lo = tbl[2 * probe], hi = tbl[(2 * probe) + 1];
		if (w < lo) {
			r = probe - 1;
		} else if (w > hi) {
			l = probe + 1;
		} else {
			return probe;
		}
	}
	return -1;
}

/* `tbl` contains single values, not ranges */
int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n)
{
	int l = 0, r = n-1;
	while (l <= r) {
		int probe = (l + r) >> 1;
		unsigned short val = tbl[probe];
		if (w < val) {
			r = probe - 1;
		} else if (w > val) {
			l = probe + 1;
		} else {
			return probe;
		}
	}
	return -1;
}

#define SJIS_ENCODE(c1,c2,s1,s2) \
	do { \
		s1 = ((c1 - 1) >> 1) + ((c1) < 0x5F ? 0x71 : 0xB1); \
		s2 = c2; \
		if ((c1) & 1) { \
			if ((c2) < 0x60) { \
				s2--; \
			} \
			s2 += 0x20; \
		} else { \
			s2 += 0x7e; \
		} \
	} while (0)

#define SJIS_DECODE(c1,c2,s1,s2) \
	do { \
		if (c1 < 0xa0) { \
			s1 = ((c1 - 0x81) << 1) + 0x21; \
		} else { \
			s1 = ((c1 - 0xc1) << 1) + 0x21; \
		} \
		s2 = c2; \
		if (c2 < 0x9f) { \
			if (c2 < 0x7f) { \
				s2++; \
			} \
			s2 -= 0x20; \
		} else { \
			s1++; \
			s2 -= 0x7e; \
		} \
	} while (0)

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * ISO-2022 variants
 */

#define ASCII          0
#define JISX0201_KANA  0x20
#define JISX0208_KANJI 0x80

static int mbfl_filt_conv_jis_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x20:	 X 0201 kana */
/*	case 0x80:	 X 0208 */
/*	case 0x90:	 X 0212 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (c == 0x0e) {		/* "kana in" */
			filter->status = 0x20;
		} else if (c == 0x0f) {		/* "kana out" */
			filter->status = 0;
		} else if (filter->status == 0x10 && c == 0x5c) {	/* YEN SIGN */
			CK((*filter->output_function)(0xa5, filter->data));
		} else if (filter->status == 0x10 && c == 0x7e) {	/* OVER LINE */
			CK((*filter->output_function)(0x203e, filter->data));
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x60) {		/* kana */
			CK((*filter->output_function)(0xff40 + c, filter->data));
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* GR kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

/*	case 0x81:	 X 0208 second char */
/*	case 0x91:	 X 0212 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (filter->status == 0x80) {
				if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else {
					w = 0;
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			} else {
				if (s >= 0 && s < jisx0212_ucs_table_size) {
					w = jisx0212_ucs_table[s];
				} else {
					w = 0;
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC */
/*	case 0x02:	*/
/*	case 0x12:	*/
/*	case 0x22:	*/
/*	case 0x82:	*/
/*	case 0x92:	*/
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			goto retry;
		}
		break;

	/* ESC $ */
/*	case 0x03:	*/
/*	case 0x13:	*/
/*	case 0x23:	*/
/*	case 0x83:	*/
/*	case 0x93:	*/
	case 3:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {			/* '(' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			goto retry;
		}
		break;

	/* ESC $ ( */
/*	case 0x04:	*/
/*	case 0x14:	*/
/*	case 0x24:	*/
/*	case 0x84:	*/
/*	case 0x94:	*/
	case 4:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x44) {			/* 'D' */
			filter->status = 0x90;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

	/* ESC ( */
/*	case 0x05:	*/
/*	case 0x15:	*/
/*	case 0x25:	*/
/*	case 0x85:	*/
/*	case 0x95:	*/
	case 5:
		if (c == 0x42 || c == 0x48) {		/* 'B' or 'H' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_jis_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* 2-byte (JIS X 0208 or 0212) character was truncated,
		 * or else escape sequence was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_jis(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s <= 0) {
		if (c == 0xa5) {		/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x8080) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else if (s < 0x10000) { /* X 0212 */
			if ((filter->status & 0xff00) != 0x300) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x44, filter->data));		/* 'D' */
			}
			filter->status = 0x300;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_2022jp(int c, mbfl_convert_filter *filter)
{
	int s;

	s = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}

	if (s <= 0) {
		if (c == 0xa5) {			/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
		s = -1;
	}
	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x10000) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	}

	return 0;
}

#define ASCII 0
#define JISX_0201_LATIN 1
#define JISX_0201_KANA 2
#define JISX_0208 3
#define JISX_0212 4

static size_t mb_iso2022jp_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			/* ESC seen; this is an escape sequence */
			if ((e - p) < 2) {
				*out++ = MBFL_BAD_INPUT;
				if (p != e && (*p == '$' || *p == '('))
					p++;
				continue;
			}

			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					*state = JISX_0208;
				} else if (c3 == '(') {
					if (p == e) {
						*out++ = MBFL_BAD_INPUT;
						break;
					}
					unsigned char c4 = *p++;
					if (c4 == '@' || c4 == 'B') {
						*state = JISX_0208;
					} else if (c4 == 'D') {
						*state = JISX_0212;
					} else {
						if ((limit - out) < 3) {
							p -= 4;
							break;
						}
						*out++ = MBFL_BAD_INPUT;
						*out++ = '$';
						*out++ = '(';
						p--;
					}
				} else {
					if ((limit - out) < 2) {
						p -= 3;
						break;
					}
					*out++ = MBFL_BAD_INPUT;
					*out++ = '$';
					p--;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				if (c3 == 'B' || c3 == 'H') {
					*state = ASCII;
				} else if (c3 == 'J') {
					*state = JISX_0201_LATIN;
				} else if (c3 == 'I') {
					*state = JISX_0201_KANA;
				} else {
					if ((limit - out) < 2) {
						p -= 3;
						break;
					}
					*out++ = MBFL_BAD_INPUT;
					*out++ = '(';
					p--;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				p--;
			}
		} else if (c == 0xE) {
			/* "Kana In" marker; this is just for JIS-7/8, but we also accept it for ISO-2022-JP */
			*state = JISX_0201_KANA;
		} else if (c == 0xF) {
			/* "Kana Out" marker */
			*state = ASCII;
		} else if (*state == JISX_0201_LATIN && c == 0x5C) { /* YEN SIGN */
			*out++ = 0xA5;
		} else if (*state == JISX_0201_LATIN && c == 0x7E) { /* OVER LINE */
			*out++ = 0x203E;
		} else if (*state == JISX_0201_KANA && c > 0x20 && c < 0x60) {
			*out++ = 0xFF40 + c;
		} else if (*state >= JISX_0208 && c > 0x20 && c < 0x7F) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				uint32_t w = 0;
				if (*state == JISX_0208) {
					if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					}
					if (!w) {
						w = MBFL_BAD_INPUT;
					}
				} else {
					if (s < jisx0212_ucs_table_size) {
						w = jisx0212_ucs_table[s];
					}
					if (!w) {
						w = MBFL_BAD_INPUT;
					}
				}
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* GR-invoked Kana; "GR" stands for "graphics right" and refers to bytes
			 * with the MSB bit (in the context of ISO-2022 encoding).
			 *
			 * In this regard, Wikipedia states:
			 * "Other, older variants known as JIS7 and JIS8 build directly on the 7-bit and 8-bit
			 * encodings defined by JIS X 0201 and allow use of JIS X 0201 kana from G1 without
			 * escape sequences, using Shift Out and Shift In or setting the eighth bit
			 * (GR-invoked), respectively."
			 *
			 * Note that we support both the 'JIS7' use of 0xE/0xF Shift In/Shift Out codes
			 * and the 'JIS8' use of GR-invoked Kana */
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jp(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x1005C;
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w != 0) {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

		if (s < 0x80) { /* ASCII */
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x8080) { /* JIS X 0208 */
			if (buf->state != JISX_0208) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else if (s < 0x10000) { /* JIS X 0212 */
			if (buf->state != JISX_0212) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'D');
				buf->state = JISX_0212;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else { /* X 0201 Latin */
			if (buf->state != JISX_0201_LATIN) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_jis(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w == 0x203E) { /* OVERLINE */
			s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x1005C;
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w != 0) {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		}

		if (s < 0x80) { /* ASCII */
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA1 && s <= 0xDF) {
			if (buf->state != JISX_0201_KANA) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else if (s < 0x8080) { /* JIS X 0208 */
			if (buf->state != JISX_0208) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else if (s < 0x10000) { /* JIS X 0212 */
			if (buf->state != JISX_0212) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'D');
				buf->state = JISX_0212;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else { /* X 0201 Latin */
			if (buf->state != JISX_0201_LATIN) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

#define JISX_0201_KANA_SO 5

static bool mb_check_jis(unsigned char *in, size_t in_len)
{
	unsigned char *p = in, *e = p + in_len;
	unsigned int state = ASCII;

	while (p < e) {
		unsigned char c = *p++;
		if (c == 0x1B) {
			/* ESC seen; this is an escape sequence */
			if (state == JISX_0201_KANA_SO) {
				return false;
			}
			if ((e - p) < 2) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					state = JISX_0208;
				} else if (c3 == '(') {
					if (p == e) {
						return false;
					}
					unsigned char c4 = *p++;
					if (c4 == '@' || c4 == 'B') {
						state = JISX_0208;
					} else if (c4 == 'D') {
						state = JISX_0212;
					} else {
						return false;
					}
				} else {
					return false;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				/* ESC ( H is treated as a sequence transitioning to ASCII for historical reasons.
				 * see https://github.com/php/php-src/pull/10828#issuecomment-1478342432. */
				if (c3 == 'B' || c3 == 'H') {
					state = ASCII;
				} else if (c3 == 'J') {
					state = JISX_0201_LATIN;
				} else if (c3 == 'I') {
					state = JISX_0201_KANA;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if (c == 0xE) {
			/* "Kana In" marker */
			if (state != ASCII) {
				return false;
			}
			state = JISX_0201_KANA_SO;
		} else if (c == 0xF) {
			/* "Kana Out" marker */
			if (state != JISX_0201_KANA_SO) {
				return false;
			}
			state = ASCII;
		} else if ((state == JISX_0208 || state == JISX_0212) && (c > 0x20 && c < 0x7F)) {
			if (p == e) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				if (state == JISX_0208) {
					if (s < jisx0208_ucs_table_size && jisx0208_ucs_table[s]) {
						continue;
					}
				} else {
					if (s < jisx0212_ucs_table_size && jisx0212_ucs_table[s]) {
						continue;
					}
				}
				return false;
			} else {
				return false;
			}
		} else if (c < 0x80) {
			continue;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* GR-invoked Kana */
			continue;
		} else {
			return false;
		}
	}

	return state == ASCII;
}

static bool mb_check_iso2022jp(unsigned char *in, size_t in_len)
{
	unsigned char *p = in, *e = p + in_len;
	unsigned int state = ASCII;

	while (p < e) {
		unsigned char c = *p++;
		if (c == 0x1B) {
			/* ESC seen; this is an escape sequence */
			if ((e - p) < 2) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					state = JISX_0208;
				} else {
					return false;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				if (c3 == 'B') {
					state = ASCII;
				} else if (c3 == 'J') {
					state = JISX_0201_LATIN;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if (c == 0xE || c == 0xF) {
			/* "Kana In" or "Kana Out" marker; ISO-2022-JP is not accepted. */
			return false;
		} else if (state == JISX_0208 && (c > 0x20 && c < 0x7F)) {
			if (p == e) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				if (s < jisx0208_ucs_table_size && jisx0208_ucs_table[s]) {
					continue;
				}
				return false;
			} else {
				return false;
			}
		} else if (c < 0x80) {
			continue;
		} else {
			return false;
		}
	}

	return state == ASCII;
}

/* Unicode codepoints for emoji are above 0x1F000, but we only store 16-bits
 * in our tables. Therefore, add 0x10000 to recover the true values.
 *
 * Again, for some emoji which are not supported by Unicode, we use codepoints
 * in the Private Use Area above 0xFE000. Again, add 0xF0000 to recover the
 * true value. */
static inline int convert_emoji_cp(int cp)
{
	if (cp > 0xF000)
		return cp + 0x10000;
	else if (cp > 0xE000)
		return cp + 0xF0000;
	return cp;
}

int mbfilter_sjis_emoji_kddi2unicode(int s, int *snd)
{
	if (s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi1_max) {
		if (s == 0x24C0) { /* Spain */
			EMIT_FLAG_EMOJI("ES");
		} else if (s == 0x24C1) { /* Russia */
			EMIT_FLAG_EMOJI("RU");
		} else if (s >= 0x2545 && s <= 0x254A) {
			EMIT_FLAG_EMOJI(nflags_kddi[s - 0x2545]);
		} else if (s == 0x25BC) {
			EMIT_KEYPAD_EMOJI('#');
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_kddi1[s - mb_tbl_code2uni_kddi1_min]);
		}
	} else if (s >= mb_tbl_code2uni_kddi2_min && s <= mb_tbl_code2uni_kddi2_max) {
		if (s == 0x2750) { /* Japan */
			EMIT_FLAG_EMOJI("JP");
		} else if (s >= 0x27A6 && s <= 0x27AE) {
			EMIT_KEYPAD_EMOJI(s - 0x27A6 + '1');
		} else if (s == 0x27F7) { /* United States */
			EMIT_FLAG_EMOJI("US");
		} else if (s == 0x2830) {
			EMIT_KEYPAD_EMOJI('0');
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_kddi2[s - mb_tbl_code2uni_kddi2_min]);
		}
	}
	return 0;
}

static int mbfl_filt_conv_2022jp_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w, snd = 0;

	switch (filter->status & 0xF) {
	case 0:
		if (c == 0x1B) {
			filter->status += 2;
		} else if (filter->status == JISX0201_KANA && c > 0x20 && c < 0x60) {
			CK((*filter->output_function)(0xFF40 + c, filter->data));
		} else if (filter->status == JISX0208_KANJI && c > 0x20 && c < 0x80) {
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* JISX 0208, second byte */
	case 1:
		w = 0;
		filter->status &= ~0xF;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7F) {
			s = ((c1 - 0x21) * 94) + c - 0x21;

			if (s <= 137) {
				if (s == 31) {
					w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xFF5E; /* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225; /* PARALLEL TO */
				} else if (s == 60) {
					w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xFFE0; /* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xFFE1; /* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xFFE2; /* FULLWIDTH NOT SIGN */
				}
			}

			if (s >= (84 * 94) && s < (91 * 94)) {
				s += 22 * 94;
				w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
				if (w > 0 && snd > 0) {
					(*filter->output_function)(snd, filter->data);
				}
			}

			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC */
	case 2:
		if (c == '$') {
			filter->status++;
		} else if (c == '(') {
			filter->status += 3;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ */
	case 3:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else if (c == '(') {
			filter->status++;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 'B' || c == 'J') {
			filter->status = 0; /* ASCII mode */
		} else if (c == 'I') {
			filter->status = JISX0201_KANA;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
	}

	return 0;
}

static int mbfl_filt_conv_2022jp_mobile_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfilter_unicode2sjis_emoji_kddi(int c, int *s1, mbfl_convert_filter *filter)
{
	if ((filter->status & 0xF) == 1) {
		int c1 = filter->cache;
		filter->cache = 0;
		filter->status &= ~0xFF;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x25BC;
			} else if (c1 == '0') {
				*s1 = 0x2830;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x27A6 + (c1 - '1');
			}
			return 1;
		} else {
			if (filter->status & 0xFF00) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			CK((*filter->output_function)(c1, filter->data));
			filter->status = 0;
		}
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status |= 1;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x27DC;
		return 1;
	} else if (c == 0xAE) { /* Registered sign */
		*s1 = 0x27DD;
		return 1;
	} else if (c >= mb_tbl_uni_kddi2code2_min && c <= mb_tbl_uni_kddi2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code3_min && c <= mb_tbl_uni_kddi2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code5_min && c <= mb_tbl_uni_kddi2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code5_val[i];
			return 1;
		}
	}
	return 0;
}

/* (ku*94)+ten value -> Shift-JIS byte sequence */
#define CODE2JIS(c1,c2,s1,s2) \
	c1 = (s1)/94+0x21; \
	c2 = (s1)-94*((c1)-0x21)+0x21; \
	s1 = ((c1) << 8) | (c2); \
	s2 = 1

static int mbfl_filt_conv_wchar_2022jp_mobile(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0, s2 = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}

	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		}
	}

	if (mbfilter_unicode2sjis_emoji_kddi(c, &s1, filter) > 0) {
		/* A KDDI emoji was detected and stored in s1 */
		CODE2JIS(c1,c2,s1,s2);
		s1 -= 0x1600;
	} else if ((filter->status & 0xFF) == 1 && filter->cache) {
		/* We are just processing one of KDDI's special emoji for a phone keypad button */
		return 0;
	}

	if ((s1 <= 0) || (s1 >= 0xa1a1 && s2 == 0)) { /* not found or X 0212 */
		s1 = -1;
		for (c1 = 0; c1 < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; c1++) {
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = (((c1 / 94) + 0x2D) << 8) + (c1 % 94) + 0x21;
				break;
			}
		}

		if (c == 0) {
			s1 = 0;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* ASCII */
			if (filter->status & 0xFF00) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			CK((*filter->output_function)(s1, filter->data));
			filter->status = 0;
		} else if (s1 > 0xA0 && s1 < 0xE0) { /* Kana */
			if ((filter->status & 0xFF00) != 0x100) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('I', filter->data));
			}
			filter->status = 0x100;
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		} else if (s1 < 0x7E7F) { /* JIS X 0208 */
			if ((filter->status & 0xFF00) != 0x200) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s1 >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_2022jp_mobile_flush(mbfl_convert_filter *filter)
{
	/* Go back to ASCII mode (so strings can be safely concatenated) */
	if (filter->status & 0xFF00) {
		(*filter->output_function)(0x1B, filter->data); /* ESC */
		(*filter->output_function)('(', filter->data);
		(*filter->output_function)('B', filter->data);
	}

	int c1 = filter->cache;
	if ((filter->status & 0xFF) == 1 && (c1 == '#' || (c1 >= '0' && c1 <= '9'))) {
		(*filter->output_function)(c1, filter->data);
	}
	filter->status = filter->cache = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_iso2022jp_kddi_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			if ((e - p) < 2) {
				p = e;
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			unsigned char c3 = *p++;

			if (c2 == '$') {
				if (c3 == '@' || c3 == 'B') {
					*state = JISX0208_KANJI;
				} else if (c3 == '(') {
					if (p == e) {
						*out++ = MBFL_BAD_INPUT;
						break;
					}
					unsigned char c4 = *p++;

					if (c4 == '@' || c4 == 'B') {
						*state = JISX0208_KANJI;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (c2 == '(') {
				if (c3 == 'B' || c3 == 'J') {
					*state = ASCII;
				} else if (c3 == 'I') {
					*state = JISX0201_KANA;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				p--;
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (*state == JISX0201_KANA && c >= 0x21 && c <= 0x5F) {
			*out++ = 0xFF40 + c;
		} else if (*state == JISX0208_KANJI && c >= 0x21 && c <= 0x7F) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 >= 0x21 && c2 <= 0x7E) {
				unsigned int s = ((c - 0x21) * 94) + c2 - 0x21;
				uint32_t w = 0;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (s >= (84 * 94) && s < (91 * 94)) {
					int snd = 0;
					s += 22 * 94;
					w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
					if (w && snd) {
						*out++ = snd;
					}
				}

				if (!w) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					}
				}

				*out++ = w ? w : MBFL_BAD_INPUT;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jp_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if ((w == '#' || (w >= '0' && w <= '9')) && len) {
			uint32_t w2 = *in++; len--;

			if (w2 == 0x20E3) {
				unsigned int s1 = 0;
				if (w == '#') {
					s1 = 0x25BC;
				} else if (w == '0') {
					s1 = 0x2830;
				} else { /* Previous character was '1'-'9' */
					s1 = 0x27A6 + (w - '1');
				}
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			} else {
				in--; len++;
			}
		} else if (w >= NFLAGS('C') && w <= NFLAGS('U') && len) { /* C for CN, U for US */
			uint32_t w2 = *in++; len--;

			if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
				for (int i = 0; i < 10; i++) {
					if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
						unsigned int s1 = nflags_code_kddi[i];
						s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
						goto found_flag_emoji;
					}
				}
			}

			in--; len++;
found_flag_emoji: ;
		}

		if (w == 0xA9) { /* Copyright sign */
			unsigned int s1 = 0x27DC;
			s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
		} else if (w == 0xAE) { /* Registered sign */
			unsigned int s1 = 0x27DD;
			s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
		} else if (w >= mb_tbl_uni_kddi2code2_min && w <= mb_tbl_uni_kddi2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
			if (i >= 0) {
				unsigned int s1 = mb_tbl_uni_kddi2code2_value[i];
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			}
		} else if (w >= mb_tbl_uni_kddi2code3_min && w <= mb_tbl_uni_kddi2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
			if (i >= 0) {
				unsigned int s1 = mb_tbl_uni_kddi2code3_value[i];
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			}
		} else if (w >= mb_tbl_uni_kddi2code5_min && w <= mb_tbl_uni_kddi2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
			if (i >= 0) {
				unsigned int s1 = mb_tbl_uni_kddi2code5_val[i];
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			}
		}

		if (!s || s >= 0xA1A1) {
			s = 0;
			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					break;
				}
			}
			if (w == 0)
				s = 0;
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA1 && s <= 0xDF) {
			if (buf->state != JISX0201_KANA) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX0201_KANA;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else if (s <= 0x7E7E) {
			if (buf->state != JISX0208_KANJI) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX0208_KANJI;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_jis2004_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, s, s1 = 0, s2 = 0, w = 0, w1;

	switch (filter->status & 0xf) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
				CK((*filter->output_function)(c, filter->data));
			} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
				if (c == 0x5c) {
					CK((*filter->output_function)(0x00a5, filter->data));
				} else if (c == 0x7e) {
					CK((*filter->output_function)(0x203e, filter->data));
				} else {
					CK((*filter->output_function)(c, filter->data));
				}
			} else { /* ISO-2022-JP-2004 */
				if (c == 0x1b) {
					filter->status += 6;
				} else if ((filter->status == 0x80 || filter->status == 0x90 || filter->status == 0xa0)
				   && c > 0x20 && c < 0x7f) { /* kanji first char */
					filter->cache = c;
					if (filter->status == 0x90) {
						filter->status += 1; /* JIS X 0213 plane 1 */
					} else if (filter->status == 0xa0) {
						filter->status += 4; /* JIS X 0213 plane 2 */
					} else {
						filter->status += 5; /* JIS X 0208 */
					}
				} else {
					CK((*filter->output_function)(c, filter->data));
				}
			}
		} else {
			if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
				if (c > 0xa0 && c < 0xff) { /* X 0213 plane 1 first char */
					filter->status = 1;
					filter->cache = c;
				} else if (c == 0x8e) { /* kana first char */
					filter->cache = 0x8E; /* So error will be reported if input is truncated right here */
					filter->status = 2;
				} else if (c == 0x8f) { /* X 0213 plane 2 first char */
					filter->status = 3;
				} else {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				}
			} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
				if (c > 0xa0 && c < 0xe0) { /* kana */
					CK((*filter->output_function)(0xfec0 + c, filter->data));
				} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
					filter->status = 1;
					filter->cache = c;
				} else {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				}
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

	case 1: /* kanji second char */
		filter->status &= ~0xf;
		c1 = filter->cache;

		if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
			if (c > 0xa0 && c < 0xff) {
				s1 = c1 - 0x80;
				s2 = c - 0x80;
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				break;
			}
		} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
			if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
				SJIS_DECODE(c1, c, s1, s2);
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				break;
			}
		} else { /* ISO-2022-JP-2004 */
			if (c >= 0x21 && c <= 0x7E) {
				s1 = c1;
				s2 = c;
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				break;
			}
		}
		w1 = (s1 << 8) | s2;

		/* conversion for combining characters */
		if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) ||
			(w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 ||
			(w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
			k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
			if (k >= 0) {
				w = jisx0213_u2_tbl[2*k];
				CK((*filter->output_function)(w, filter->data));
				w = jisx0213_u2_tbl[2*k+1];
			}
		}

		/* conversion for BMP  */
		if (w <= 0) {
			w1 = (s1 - 0x21)*94 + s2 - 0x21;
			if (w1 >= 0 && w1 < jisx0213_ucs_table_size) {
				w = jisx0213_ucs_table[w1];
			}
		}

		/* conversion for CJK Unified Ideographs ext.B (U+2XXXX) */
		if (w <= 0) {
			k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				w = jisx0213_jis_u5_tbl[k] + 0x20000;
			}
		}

		if (w <= 0) {
			w = MBFL_BAD_INPUT;
		}
		CK((*filter->output_function)(w, filter->data));
		break;

	case 2: /* got 0x8e: EUC-JP-2004 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* X 0213 plane 2 first char: EUC-JP-2004 (0x8f) */
		if (c == 0xA1 || (c >= 0xA3 && c <= 0xA5) || c == 0xA8 || (c >= 0xAC && c <= 0xAF) || (c >= 0xEE && c <= 0xFE)) {
			filter->cache = c - 0x80;
			filter->status++;
		} else {
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 4: /* X 0213 plane 2 second char: EUC-JP-2004, ISO-2022-JP-2004 */
		filter->status &= ~0xF;
		c1 = filter->cache;
		if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
			c2 = c - 0x80;
		} else {
			c2 = c;
		}

		if (c2 < 0x21 || c2 > 0x7E) {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			break;
		}

		s1 = c1 - 0x21;
		s2 = c2 - 0x21;

		if (((s1 >= 0 && s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) ||
			(s1 >= 77 && s1 < 94)) && s2 >= 0 && s2 < 94) {
			/* calc offset from ku */
			for (k = 0; k < jisx0213_p2_ofst_len; k++) {
				if (s1 == jisx0213_p2_ofst[k]) {
					break;
				}
			}
			k -= jisx0213_p2_ofst[k];

			/* check for japanese chars in BMP */
			s = (s1 + 94 + k)*94 + s2;
			ZEND_ASSERT(s < jisx0213_ucs_table_size);
			w = jisx0213_ucs_table[s];

			/* check for japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
			if (w <= 0) {
				k = mbfl_bisec_srch2(s, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
				if (k >= 0) {
					w = jisx0213_jis_u5_tbl[k] + 0x20000;
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 5: /* X 0208: ISO-2022-JP-2004 */
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (s >= 0 && s < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[s];
			}
		}

		if (w <= 0) {
			w = MBFL_BAD_INPUT;
		}

		CK((*filter->output_function)(w, filter->data));
		break;

	/* ESC: ISO-2022-JP-2004 */
/*	case 0x06:	*/
/*	case 0x16:	*/
/*	case 0x26:	*/
/*	case 0x86:	*/
/*	case 0x96:	*/
/*	case 0xa6:	*/
	case 6:
		if (c == '$') {
			filter->status++;
		} else if (c == '(') {
			filter->status += 3;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $: ISO-2022-JP-2004 */
/*	case 0x07:	*/
/*	case 0x17:	*/
/*	case 0x27:	*/
/*	case 0x87:	*/
/*	case 0x97:	*/
/*	case 0xa7:	*/
	case 7:
		if (c == 'B') { /* JIS X 0208-1983 */
			filter->status = 0x80;
		} else if (c == '(') {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ (: ISO-2022-JP-2004 */
/*	case 0x08:	*/
/*	case 0x18:	*/
/*	case 0x28:	*/
/*	case 0x88:	*/
/*	case 0x98:	*/
/*	case 0xa8:	*/
	case 8:
		if (c == 'Q') { /* JIS X 0213 plane 1 */
			filter->status = 0x90;
		} else if (c == 'P') { /* JIS X 0213 plane 2 */
			filter->status = 0xa0;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC (: ISO-2022-JP-2004 */
/*	case 0x09:	*/
/*	case 0x19:	*/
/*	case 0x29:	*/
/*	case 0x89:	*/
/*	case 0x99:	*/
	case 9:
		if (c == 'B') {
			filter->status = 0;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_jis2004_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_jis2004(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, s1, s2;

retry:
	s1 = 0;
	/* check for 1st char of combining characters */
	if ((filter->status & 0xf) == 0 && (
			c == 0x00E6 ||
			(c >= 0x0254 && c <= 0x02E9) ||
			(c >= 0x304B && c <= 0x3053) ||
			(c >= 0x30AB && c <= 0x30C8) ||
			c == 0x31F7)) {
		for (k = 0; k < jisx0213_u2_tbl_len; k++) {
			if (c == jisx0213_u2_tbl[2*k]) {
				filter->status++;
				filter->cache = k;
				return 0;
			}
		}
	}

	/* check for 2nd char of combining characters */
	if ((filter->status & 0xf) == 1 && filter->cache >= 0 && filter->cache < jisx0213_u2_tbl_len) {
		k = filter->cache;
		filter->status &= ~0xf;
		filter->cache = 0;

		c1 = jisx0213_u2_tbl[2*k];
		if ((c1 == 0x0254 || c1 == 0x028C || c1 == 0x0259 || c1 == 0x025A) && c == 0x0301) {
			k++;
		}
		if (c == jisx0213_u2_tbl[2*k+1]) {
			s1 = jisx0213_u2_key[k];
		} else { /* fallback */
			s1 = jisx0213_u2_fb_tbl[k];

			if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
			} else if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				s2 = (s1 & 0xff) + 0x80;
				s1 = ((s1 >> 8) & 0xff) + 0x80;
			} else {
				if (filter->status != 0x200) {
					CK((*filter->output_function)(0x1b, filter->data));
					CK((*filter->output_function)('$', filter->data));
					CK((*filter->output_function)('(', filter->data));
					CK((*filter->output_function)('Q', filter->data));
				}
				filter->status = 0x200;

				s2 = s1 & 0x7f;
				s1 = (s1 >> 8) & 0x7f;
			}

			/* Flush out cached data */
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
			goto retry;
		}
	}

	/* check for major japanese chars: U+4E00 - U+9FFF */
	if (s1 <= 0) {
		for (k = 0; k < uni2jis_tbl_len; k++) {
			if (c >= uni2jis_tbl_range[k][0] && c <= uni2jis_tbl_range[k][1]) {
				s1 = uni2jis_tbl[k][c-uni2jis_tbl_range[k][0]];
				break;
			}
		}
	}

	/* check for japanese chars in compressed mapping area: U+1E00 - U+4DBF */
	if (s1 <= 0 && c >= ucs_c1_jisx0213_min && c <= ucs_c1_jisx0213_max) {
		k = mbfl_bisec_srch(c, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
		if (k >= 0) {
			s1 = ucs_c1_jisx0213_ofst[k] + c - ucs_c1_jisx0213_tbl[2*k];
		}
	}

	/* check for japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
	if (s1 <= 0 && c >= jisx0213_u5_tbl_min && c <= jisx0213_u5_tbl_max) {
		k = mbfl_bisec_srch2(c - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
		if (k >= 0) {
			s1 = jisx0213_u5_jis_tbl[k];
		}
	}

	if (s1 <= 0) {
		/* CJK Compatibility Forms: U+FE30 - U+FE4F */
		if (c == 0xfe45) {
			s1 = 0x233e;
		} else if (c == 0xfe46) {
			s1 = 0x233d;
		} else if (c >= 0xf91d && c <= 0xf9dc) {
			/* CJK Compatibility Ideographs: U+F900 - U+F92A */
			k = mbfl_bisec_srch2(c, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
			if (k >= 0) {
				s1 = ucs_r2b_jisx0213_cmap_val[k];
			}
		}
	}

	if (s1 <= 0) {
		if (c == 0) {
			s1 = 0;
		} else {
			s1 = -1;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* ASCII */
			if (filter->to->no_encoding == mbfl_no_encoding_2022jp_2004 && (filter->status & 0xff00)) {
				CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			filter->status = 0;
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x100) { /* latin or kana */
			if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				CK((*filter->output_function)(0x8e, filter->data));
				CK((*filter->output_function)(s1, filter->data));
			} else if (filter->to->no_encoding == mbfl_no_encoding_sjis2004 && (s1 >= 0xA1 && s1 <= 0xDF)) {
				CK((*filter->output_function)(s1, filter->data));
			} else {
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		} else if (s1 < 0x7f00) { /* X 0213 plane 1 */
			if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
			} else if  (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				s2 = (s1 & 0xff) + 0x80;
				s1 = ((s1 >> 8) & 0xff) + 0x80;
			} else {
				if ((filter->status & 0xff00) != 0x200) {
					CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
					CK((*filter->output_function)('$', filter->data));
					CK((*filter->output_function)('(', filter->data));
					CK((*filter->output_function)('Q', filter->data));
				}
				filter->status = 0x200;
				s2 = s1 & 0xff;
				s1 = (s1 >> 8) & 0xff;
			}
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		} else { /* X 0213 plane 2 */
			if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
			} else {
				s2 = s1 & 0xff;
				k = ((s1 >> 8) & 0xff) - 0x7f;
				if (k >= 0 && k < jisx0213_p2_ofst_len) {
					s1 = jisx0213_p2_ofst[k] + 0x21;
				}
				if  (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
					s2 |= 0x80;
					s1 |= 0x80;
					CK((*filter->output_function)(0x8f, filter->data));
				} else {
					if ((filter->status & 0xff00) != 0x200) {
						CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
						CK((*filter->output_function)('$', filter->data));
						CK((*filter->output_function)('(', filter->data));
						CK((*filter->output_function)('P', filter->data));
					}
					filter->status = 0x200;
				}
			}

			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_jis2004_flush(mbfl_convert_filter *filter)
{
	int k, c1, c2, s1, s2;

	k = filter->cache;
	filter->cache = 0;

	if (filter->status == 1 && k >= 0 && k <= jisx0213_u2_tbl_len) {
		s1 = jisx0213_u2_fb_tbl[k];

		if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
		} else if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
			s2 = (s1 & 0xff) | 0x80;
			s1 = ((s1 >> 8) & 0xff) | 0x80;
		} else {
			s2 = s1 & 0x7f;
			s1 = (s1 >> 8) & 0x7f;
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('Q', filter->data));
			}
			filter->status = 0x200;
		}

		CK((*filter->output_function)(s1, filter->data));
		CK((*filter->output_function)(s2, filter->data));
	}

	/* If we had switched to a different charset, go back to ASCII mode
	 * This makes it possible to concatenate arbitrary valid strings
	 * together and get a valid string */
	if (filter->status & 0xff00) {
		CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
		CK((*filter->output_function)('(', filter->data));
		CK((*filter->output_function)('B', filter->data));
	}

	filter->status = 0;

	if (filter->flush_function) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

#define ASCII 0
#define JISX0208 1
#define JISX0213_PLANE1 2
#define JISX0213_PLANE2 3

static size_t mb_iso2022jp2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			if (c == 0x1B) {
				if ((e - p) < 2) {
					*out++ = MBFL_BAD_INPUT;
					p = e;
					break;
				}
				unsigned char c2 = *p++;
				unsigned char c3 = *p++;
				if (c2 == '$') {
					if (c3 == 'B') {
						*state = JISX0208;
					} else if (c3 == '(') {
						if (p == e) {
							*out++ = MBFL_BAD_INPUT;
							break;
						}
						unsigned char c4 = *p++;
						if (c4 == 'Q') {
							*state = JISX0213_PLANE1;
						} else if (c4 == 'P') {
							*state = JISX0213_PLANE2;
						} else {
							*out++ = MBFL_BAD_INPUT;
						}
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else if (c2 == '(') {
					if (c3 == 'B') {
						*state = ASCII;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					p--;
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (*state >= JISX0208 && c > 0x20 && c < 0x7F) {
				if (p == e) {
					*out++ = MBFL_BAD_INPUT;
					break;
				}
				unsigned char c2 = *p++;
				if (c2 < 0x21 || c2 > 0x7E) {
					*out++ = MBFL_BAD_INPUT;
					continue;
				}

				if (*state == JISX0213_PLANE1) {
					unsigned int w1 = (c << 8) | c2;

					/* Conversion for combining characters */
					if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) || (w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 || (w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
						int k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
						if (k >= 0) {
							*out++ = jisx0213_u2_tbl[2*k];
							*out++ = jisx0213_u2_tbl[2*k+1];
							continue;
						}
					}

					/* Conversion for BMP */
					uint32_t w = 0;
					w1 = (c - 0x21)*94 + c2 - 0x21;
					if (w1 < jisx0213_ucs_table_size) {
						w = jisx0213_ucs_table[w1];
					}

					/* Conversion for CJK Unified Ideographs ext.B (U+2XXXX) */
					if (!w) {
						int k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
						if (k >= 0) {
							w = jisx0213_jis_u5_tbl[k] + 0x20000;
						}
					}

					*out++ = w ? w : MBFL_BAD_INPUT;
				} else if (*state == JISX0213_PLANE2) {

					unsigned int s1 = c - 0x21, s2 = c2 - 0x21;

					if (((s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) || (s1 >= 77 && s1 < 94)) && s2 < 94) {
						int k;
						for (k = 0; k < jisx0213_p2_ofst_len; k++) {
							if (s1 == jisx0213_p2_ofst[k]) {
								break;
							}
						}
						k -= jisx0213_p2_ofst[k];

						/* Check for Japanese chars in BMP */
						unsigned int s = (s1 + 94 + k)*94 + s2;
						ZEND_ASSERT(s < jisx0213_ucs_table_size);
						uint32_t w = jisx0213_ucs_table[s];

						/* Check for Japanese chars in CJK Unified Ideographs ext B (U+2XXXX) */
						if (!w) {
							k = mbfl_bisec_srch2(s, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
							if (k >= 0) {
								w = jisx0213_jis_u5_tbl[k] + 0x20000;
							}
						}

						*out++ = w ? w : MBFL_BAD_INPUT;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else { /* state == JISX0208 */
					unsigned int s = (c - 0x21)*94 + c2 - 0x21;
					uint32_t w = 0;
					if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					}
					*out++ = w ? w : MBFL_BAD_INPUT;
				}
			} else {
				*out++ = c;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jp2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;
	if (buf->state & 0xFF00) {
		int k = (buf->state >> 8) - 1;
		w = jisx0213_u2_tbl[2*k];
		buf->state &= 0xFF;
		goto process_codepoint;
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		if (w == 0xE6 || (w >= 0x254 && w <= 0x2E9) || (w >= 0x304B && w <= 0x3053) || (w >= 0x30AB && w <= 0x30C8) || w == 0x31F7) {
			for (int k = 0; k < jisx0213_u2_tbl_len; k++) {
				if (w == jisx0213_u2_tbl[2*k]) {
					if (!len) {
						if (!end) {
							buf->state |= (k+1) << 8;
							MB_CONVERT_BUF_STORE(buf, out, limit);
							return;
						}
					}	else {
						uint32_t w2 = *in++; len--;
						if ((w == 0x254 || w == 0x28C || w == 0x259 || w == 0x25A) && w2 == 0x301) {
							k++;
						}
						if (w2 == jisx0213_u2_tbl[2*k+1]) {
							s = jisx0213_u2_key[k];
							break;
						}
						in--; len++;
					}

					s = jisx0213_u2_fb_tbl[k];
					break;
				}
			}
		}

		/* Check for major Japanese chars: U+4E00-U+9FFF */
		if (!s) {
			for (int k = 0; k < uni2jis_tbl_len; k++) {
				if (w >= uni2jis_tbl_range[k][0] && w <= uni2jis_tbl_range[k][1]) {
					s = uni2jis_tbl[k][w - uni2jis_tbl_range[k][0]];
					break;
				}
			}
		}

		/* Check for Japanese chars in compressed mapping area: U+1E00-U+4DBF */
		if (!s && w >= ucs_c1_jisx0213_min && w <= ucs_c1_jisx0213_max) {
			int k = mbfl_bisec_srch(w, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
			if (k >= 0) {
				s = ucs_c1_jisx0213_ofst[k] + w - ucs_c1_jisx0213_tbl[2*k];
			}
		}

		/* Check for Japanese chars in CJK Unified Ideographs ext B (U+2XXXX) */
		if (!s && w >= jisx0213_u5_tbl_min && w <= jisx0213_u5_tbl_max) {
			int k = mbfl_bisec_srch2(w - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				s = jisx0213_u5_jis_tbl[k];
			}
		}

		if (!s) {
			/* CJK Compatibility Forms: U+FE30-U+FE4F */
			if (w == 0xFE45) {
				s = 0x233E;
			} else if (w == 0xFE46) {
				s = 0x233D;
			} else if (w >= 0xF91D && w <= 0xF9DC) {
				/* CJK Compatibility Ideographs: U+F900-U+F92A */
				int k = mbfl_bisec_srch2(w, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
				if (k >= 0) {
					s = ucs_r2b_jisx0213_cmap_val[k];
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s <= 0xFF) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7EFF) {
			if (buf->state != JISX0213_PLANE1) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'Q');
				buf->state = JISX0213_PLANE1;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else {
			if (buf->state != JISX0213_PLANE2) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'P');
				buf->state = JISX0213_PLANE2;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			unsigned int s2 = s & 0xFF;
			int k = ((s >> 8) & 0xFF) - 0x7F;
			ZEND_ASSERT(k < jisx0213_p2_ofst_len);
			s = jisx0213_p2_ofst[k] + 0x21;
			out = mb_convert_buf_add2(out, s, s2);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_cp5022x_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x20:	 X 0201 kana */
/*	case 0x80:	 X 0208 */
/*	case 0x90:	 X 0212 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (c == 0x0e) {		/* "kana in" */
			filter->status = 0x20;
		} else if (c == 0x0f) {		/* "kana out" */
			filter->status = 0;
		} else if (filter->status == 0x10 && c == 0x5c) {	/* YEN SIGN */
			CK((*filter->output_function)(0xa5, filter->data));
		} else if (filter->status == 0x10 && c == 0x7e) {	/* OVER LINE */
			CK((*filter->output_function)(0x203e, filter->data));
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x60) {		/* kana */
			CK((*filter->output_function)(0xff40 + c, filter->data));
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c <= 0x97) { /* kanji first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* GR kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

/*	case 0x81:	 X 0208 second char */
/*	case 0x91:	 X 0212 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (filter->status == 0x80) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= 94 * 94 && s < 114 * 94) {
					/* user-defined => PUA (Microsoft extended) */
					w = s - 94*94 + 0xe000;
				} else {
					w = 0;
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			} else {
				if (s >= 0 && s < jisx0212_ucs_table_size) {
					w = jisx0212_ucs_table[s];
				} else {
					w = 0;
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC */
/*	case 0x02:	*/
/*	case 0x12:	*/
/*	case 0x22:	*/
/*	case 0x82:	*/
/*	case 0x92:	*/
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			goto retry;
		}
		break;

	/* ESC $ */
/*	case 0x03:	*/
/*	case 0x13:	*/
/*	case 0x23:	*/
/*	case 0x83:	*/
/*	case 0x93:	*/
	case 3:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {			/* '(' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			goto retry;
		}
		break;

	/* ESC $ ( */
/*	case 0x04:	*/
/*	case 0x14:	*/
/*	case 0x24:	*/
/*	case 0x84:	*/
/*	case 0x94:	*/
	case 4:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x44) {			/* 'D' */
			filter->status = 0x90;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

	/* ESC ( */
/*	case 0x05:	*/
/*	case 0x15:	*/
/*	case 0x25:	*/
/*	case 0x85:	*/
/*	case 0x95:	*/
	case 5:
		if (c == 0x42 || c == 0x48) {		/* 'B' or 'H' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp5022x_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* 2-byte (JIS X 0208 or 0212) character was truncated, or else
		 * escape sequence was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static const unsigned char hankana2zenkana_table[64] = {
	0x00,0x02,0x0C,0x0D,0x01,0xFB,0xF2,0xA1,0xA3,0xA5,
	0xA7,0xA9,0xE3,0xE5,0xE7,0xC3,0xFC,0xA2,0xA4,0xA6,
	0xA8,0xAA,0xAB,0xAD,0xAF,0xB1,0xB3,0xB5,0xB7,0xB9,
	0xBB,0xBD,0xBF,0xC1,0xC4,0xC6,0xC8,0xCA,0xCB,0xCC,
	0xCD,0xCE,0xCF,0xD2,0xD5,0xD8,0xDB,0xDE,0xDF,0xE0,
	0xE1,0xE2,0xE4,0xE6,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,
	0xEF,0xF3,0x9B,0x9C
};

static const unsigned char hankana2zenhira_table[64] = {
	0x00,0x02,0x0C,0x0D,0x01,0xFB,0x92,0x41,0x43,0x45,
	0x47,0x49,0x83,0x85,0x87,0x63,0xFC,0x42,0x44,0x46,
	0x48,0x4A,0x4B,0x4D,0x4F,0x51,0x53,0x55,0x57,0x59,
	0x5B,0x5D,0x5F,0x61,0x64,0x66,0x68,0x6A,0x6B,0x6C,
	0x6D,0x6E,0x6F,0x72,0x75,0x78,0x7B,0x7E,0x7F,0x80,
	0x81,0x82,0x84,0x86,0x88,0x89,0x8A,0x8B,0x8C,0x8D,
	0x8F,0x93,0x9B,0x9C
};

static const unsigned char zenkana2hankana_table[84][2] = {
	{0x67,0x00},{0x71,0x00},{0x68,0x00},{0x72,0x00},{0x69,0x00},
	{0x73,0x00},{0x6A,0x00},{0x74,0x00},{0x6B,0x00},{0x75,0x00},
	{0x76,0x00},{0x76,0x9E},{0x77,0x00},{0x77,0x9E},{0x78,0x00},
	{0x78,0x9E},{0x79,0x00},{0x79,0x9E},{0x7A,0x00},{0x7A,0x9E},
	{0x7B,0x00},{0x7B,0x9E},{0x7C,0x00},{0x7C,0x9E},{0x7D,0x00},
	{0x7D,0x9E},{0x7E,0x00},{0x7E,0x9E},{0x7F,0x00},{0x7F,0x9E},
	{0x80,0x00},{0x80,0x9E},{0x81,0x00},{0x81,0x9E},{0x6F,0x00},
	{0x82,0x00},{0x82,0x9E},{0x83,0x00},{0x83,0x9E},{0x84,0x00},
	{0x84,0x9E},{0x85,0x00},{0x86,0x00},{0x87,0x00},{0x88,0x00},
	{0x89,0x00},{0x8A,0x00},{0x8A,0x9E},{0x8A,0x9F},{0x8B,0x00},
	{0x8B,0x9E},{0x8B,0x9F},{0x8C,0x00},{0x8C,0x9E},{0x8C,0x9F},
	{0x8D,0x00},{0x8D,0x9E},{0x8D,0x9F},{0x8E,0x00},{0x8E,0x9E},
	{0x8E,0x9F},{0x8F,0x00},{0x90,0x00},{0x91,0x00},{0x92,0x00},
	{0x93,0x00},{0x6C,0x00},{0x94,0x00},{0x6D,0x00},{0x95,0x00},
	{0x6E,0x00},{0x96,0x00},{0x97,0x00},{0x98,0x00},{0x99,0x00},
	{0x9A,0x00},{0x9B,0x00},{0x9C,0x00},{0x9C,0x00},{0x72,0x00},
	{0x74,0x00},{0x66,0x00},{0x9D,0x00},{0x73,0x9E}
};

/* Apply various transforms to input codepoint, such as converting halfwidth katakana
 * to fullwidth katakana. `mode` is a bitfield which controls which transforms are
 * actually performed. The bit values are defined in translit_kana_jisx0201_jisx0208.h.
 * `mode` must not call for transforms which are inverses (i.e. which would cancel
 * each other out).
 *
 * In some cases, successive input codepoints may be merged into one output codepoint.
 * (That is the purpose of the `next` parameter.) If the `next` codepoint is consumed
 * and should be skipped over, `*consumed` will be set to true. Otherwise, `*consumed`
 * will not be modified. If there is no following codepoint, `next` should be zero.
 *
 * Again, in some cases, one input codepoint may convert to two output codepoints.
 * If so, the second output codepoint will be stored in `*second`.
 *
 * Return the resulting codepoint. If none of the requested transforms apply, return
 * the input codepoint unchanged.
 */
uint32_t mb_convert_kana_codepoint(uint32_t c, uint32_t next, bool *consumed, uint32_t *second, unsigned int mode)
{
	if ((mode & MBFL_HAN2ZEN_ALL) && c >= 0x21 && c <= 0x7D && c != '"' && c != '\'' && c != '\\') {
		return c + 0xFEE0;
	}
	if ((mode & MBFL_HAN2ZEN_ALPHA) && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
		return c + 0xFEE0;
	}
	if ((mode & MBFL_HAN2ZEN_NUMERIC) && c >= '0' && c <= '9') {
		return c + 0xFEE0;
	}
	if ((mode & MBFL_HAN2ZEN_SPACE) && c == ' ') {
		return 0x3000;
	}

	if (mode & (MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_HIRAGANA)) {
		/* Convert Hankaku kana to Zenkaku kana
		 * Either all Hankaku kana (including katakana and hiragana) will be converted
		 * to Zenkaku katakana, or to Zenkaku hiragana */
		if ((mode & MBFL_HAN2ZEN_KATAKANA) && (mode & MBFL_HAN2ZEN_GLUE)) {
			if (c >= 0xFF61 && c <= 0xFF9F) {
				int n = c - 0xFF60;

				if (next >= 0xFF61 && next <= 0xFF9F) {
					if (next == 0xFF9E && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						*consumed = true;
						return 0x3001 + hankana2zenkana_table[n];
					}
					if (next == 0xFF9E && n == 19) {
						*consumed = true;
						return 0x30F4;
					}
					if (next == 0xFF9F && n >= 42 && n <= 46) {
						*consumed = true;
						return 0x3002 + hankana2zenkana_table[n];
					}
				}

				return 0x3000 + hankana2zenkana_table[n];
			}
		}
		if ((mode & MBFL_HAN2ZEN_HIRAGANA) && (mode & MBFL_HAN2ZEN_GLUE)) {
			if (c >= 0xFF61 && c <= 0xFF9F) {
				int n = c - 0xFF60;

				if (next >= 0xFF61 && next <= 0xFF9F) {
					if (next == 0xFF9E && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						*consumed = true;
						return 0x3001 + hankana2zenhira_table[n];
					}
					if (next == 0xFF9F && n >= 42 && n <= 46) {
						*consumed = true;
						return 0x3002 + hankana2zenhira_table[n];
					}
				}

				return 0x3000 + hankana2zenhira_table[n];
			}
		}
		if ((mode & MBFL_HAN2ZEN_KATAKANA) && c >= 0xFF61 && c <= 0xFF9F) {
			return 0x3000 + hankana2zenkana_table[c - 0xFF60];
		}
		if ((mode & MBFL_HAN2ZEN_HIRAGANA) && c >= 0xFF61 && c <= 0xFF9F) {
			return 0x3000 + hankana2zenhira_table[c - 0xFF60];
		}
	}

	if (mode & MBFL_HAN2ZEN_SPECIAL) { /* special ascii to symbol */
		if (c == '\\' || c == 0xA5) { /* YEN SIGN */
			return 0xFFE5; /* FULLWIDTH YEN SIGN */
		}
		if (c == 0x7E || c == 0x203E) {
			return 0xFFE3; /* FULLWIDTH MACRON */
		}
		if (c == '\'') {
			return 0x2019; /* RIGHT SINGLE QUOTATION MARK */
		}
		if (c == '"') {
			return 0x201D; /* RIGHT DOUBLE QUOTATION MARK */
		}
	}

	if (mode & (MBFL_ZEN2HAN_ALL | MBFL_ZEN2HAN_ALPHA | MBFL_ZEN2HAN_NUMERIC | MBFL_ZEN2HAN_SPACE)) {
		/* Zenkaku to Hankaku */
		if ((mode & MBFL_ZEN2HAN_ALL) && c >= 0xFF01 && c <= 0xFF5D && c != 0xFF02 && c != 0xFF07 && c != 0xFF3C) {
			/* all except " ' \ ~ */
			return c - 0xFEE0;
		}
		if ((mode & MBFL_ZEN2HAN_ALPHA) && ((c >= 0xFF21 && c <= 0xFF3A) || (c >= 0xFF41 && c <= 0xFF5A))) {
			return c - 0xFEE0;
		}
		if ((mode & MBFL_ZEN2HAN_NUMERIC) && (c >= 0xFF10 && c <= 0xFF19)) {
			return c - 0xFEE0;
		}
		if ((mode & MBFL_ZEN2HAN_SPACE) && (c == 0x3000)) {
			return ' ';
		}
		if ((mode & MBFL_ZEN2HAN_ALL) && (c == 0x2212)) { /* MINUS SIGN */
			return '-';
		}
	}

	if (mode & (MBFL_ZEN2HAN_KATAKANA | MBFL_ZEN2HAN_HIRAGANA)) {
		/* Zenkaku kana to hankaku kana */
		if ((mode & MBFL_ZEN2HAN_KATAKANA) && c >= 0x30A1 && c <= 0x30F4) {
			/* Zenkaku katakana to hankaku kana */
			int n = c - 0x30A1;
			if (zenkana2hankana_table[n][1]) {
				*second = 0xFF00 + zenkana2hankana_table[n][1];
			}
			return 0xFF00 + zenkana2hankana_table[n][0];
		}
		if ((mode & MBFL_ZEN2HAN_HIRAGANA) && c >= 0x3041 && c <= 0x3093) {
			/* Zenkaku hiragana to hankaku kana */
			int n = c - 0x3041;
			if (zenkana2hankana_table[n][1]) {
				*second = 0xFF00 + zenkana2hankana_table[n][1];
			}
			return 0xFF00 + zenkana2hankana_table[n][0];
		}
		if (c == 0x3001) {
			return 0xFF64; /* HALFWIDTH IDEOGRAPHIC COMMA */
		}
		if (c == 0x3002) {
			return 0xFF61; /* HALFWIDTH IDEOGRAPHIC FULL STOP */
		}
		if (c == 0x300C) {
			return 0xFF62; /* HALFWIDTH LEFT CORNER BRACKET */
		}
		if (c == 0x300D) {
			return 0xFF63; /* HALFWIDTH RIGHT CORNER BRACKET */
		}
		if (c == 0x309B) {
			return 0xFF9E; /* HALFWIDTH KATAKANA VOICED SOUND MARK */
		}
		if (c == 0x309C) {
			return 0xff9f; /* HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK */
		}
		if (c == 0x30FC) {
			return 0xFF70; /* HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK */
		}
		if (c == 0x30FB) {
			return 0xFF65; /* HALFWIDTH KATAKANA MIDDLE DOT */
		}
	}

	if (mode & (MBFL_ZENKAKU_HIRA2KATA | MBFL_ZENKAKU_KATA2HIRA)) {
		if ((mode & MBFL_ZENKAKU_HIRA2KATA) && ((c >= 0x3041 && c <= 0x3093) || c == 0x309D || c == 0x309E)) {
			/* Zenkaku hiragana to Zenkaku katakana */
			return c + 0x60;
		}
		if ((mode & MBFL_ZENKAKU_KATA2HIRA) && ((c >= 0x30A1 && c <= 0x30F3) || c == 0x30FD || c == 0x30FE)) {
			/* Zenkaku katakana to Zenkaku hiragana */
			return c - 0x60;
		}
	}

	if (mode & MBFL_ZEN2HAN_SPECIAL) { /* special symbol to ascii */
		if (c == 0xFFE5 || c == 0xFF3C) { /* FULLWIDTH YEN SIGN/FULLWIDTH REVERSE SOLIDUS */
			return '\\';
		}
		if (c == 0xFFE3 || c == 0x203E) { /* FULLWIDTH MACRON/OVERLINE */
			return '~';
		}
		if (c == 0x2018 || c == 0x2019) { /* LEFT/RIGHT SINGLE QUOTATION MARK*/
			return '\'';
		}
		if (c == 0x201C || c == 0x201D) { /* LEFT/RIGHT DOUBLE QUOTATION MARK */
			return '"';
		}
	}

	return c;
}

static int mbfl_filt_conv_wchar_cp50221(int c, mbfl_convert_filter *filter);

static int mbfl_filt_conv_wchar_cp50220(int c, mbfl_convert_filter *filter)
{
	int mode = MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE;
	bool consumed = false;

	if (filter->cache) {
		int s = mb_convert_kana_codepoint(filter->cache, c, &consumed, NULL, mode);
		filter->cache = consumed ? 0 : c;
		/* Terrible hack to get CP50220 to emit error markers in the proper
		 * position, not reordering them with subsequent characters */
		filter->filter_function = mbfl_filt_conv_wchar_cp50221;
		mbfl_filt_conv_wchar_cp50221(s, filter);
		filter->filter_function = mbfl_filt_conv_wchar_cp50220;
		if (c == 0 && !consumed) {
			(*filter->output_function)(0, filter->data);
		}
	} else if (c == 0) {
		/* This case has to be handled separately, since `filter->cache == 0` means
		 * no codepoint is cached */
		(*filter->output_function)(0, filter->data);
	} else {
		filter->cache = c;
	}

	return 0;
}

static int mbfl_filt_conv_any_jis_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
	}
	filter->status = 0;

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp50220_flush(mbfl_convert_filter *filter)
{
	int mode = MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE;

	if (filter->cache) {
		int s = mb_convert_kana_codepoint(filter->cache, 0, NULL, NULL, mode);
		filter->filter_function = mbfl_filt_conv_wchar_cp50221;
		mbfl_filt_conv_wchar_cp50221(s, filter);
		filter->filter_function = mbfl_filt_conv_wchar_cp50220;
		filter->cache = 0;
	}

	return mbfl_filt_conv_any_jis_flush(filter);
}

static int mbfl_filt_conv_wchar_cp50221(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c <= 0xE757) {
		/* 'private'/'user' codepoints */
		s = c - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
	}

	if (s <= 0) {
		if (c == 0xa5) {			/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		}
	}

	/* Above, we do a series of lookups in `ucs_*_jis_table` to find a
	 * corresponding kuten code for this Unicode codepoint
	 * If we get zero, that means the codepoint is not in JIS X 0208
	 * On the other hand, if we get a result with the high bits set on both
	 * upper and lower bytes, that is not a code in JIS X 0208 but rather
	 * in JIS X 0213
	 * In either case, check if this codepoint is one of the extensions added
	 * to JIS X 0208 by MicroSoft (to make CP932) */
	if (s == 0 || ((s & 0x8000) && (s & 0x80))) {
		int i;
		s = -1;

		for (i = 0;
				i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
				i++) {
			const int oh = cp932ext1_ucs_table_min / 94;

			if (c == cp932ext1_ucs_table[i]) {
				s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
				break;
			}
		}

		if (s < 0) {
			const int oh = cp932ext2_ucs_table_min / 94;
			const int cp932ext2_ucs_table_size =
					cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			for (i = 0; i < cp932ext2_ucs_table_size; i++) {
				if (c == cp932ext2_ucs_table[i]) {
					s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
					break;
				}
			}
		}

		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0;
			}
			CK((*filter->output_function)(s, filter->data));
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x49, filter->data));		/* 'I' */
				filter->status = 0x500;
			}
			CK((*filter->output_function)(s - 0x80, filter->data));
		} else if (s <= 0x927E) { /* X 0208 + extensions */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			CK(mbfl_filt_conv_illegal_output(c, filter));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp50222(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c <= 0xE757) {
		/* 'private'/'user' codepoints */
		s = c - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
	}

	if (s <= 0) {
		if (c == 0xa5) {			/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		}
	}
	if (s == 0 || ((s & 0x8000) && (s & 0x80))) {
		int i;
		s = -1;

		for (i = 0;
				i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
			const int oh = cp932ext1_ucs_table_min / 94;

			if (c == cp932ext1_ucs_table[i]) {
				s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
				break;
			}
		}

		if (s <= 0) {
			const int oh = cp932ext2_ucs_table_min / 94;
			const int cp932ext2_ucs_table_size =
					cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			for (i = 0; i < cp932ext2_ucs_table_size; i++) {
				if (c == cp932ext2_ucs_table[i]) {
					s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
					break;
				}
			}
		}

		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			} else if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0;
			}
			CK((*filter->output_function)(s, filter->data));
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				CK((*filter->output_function)(0x0e, filter->data));		/* SI */
				filter->status = 0x500;
			}
			CK((*filter->output_function)(s - 0x80, filter->data));
		} else if (s <= 0x927E) { /* X 0208 */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			CK(mbfl_filt_conv_illegal_output(c, filter));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp50222_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) == 0x500) {
		CK((*filter->output_function)(0x0f, filter->data));		/* SO */
	} else if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define ASCII 0
#define JISX_0201_LATIN 1
#define JISX_0201_KANA 2
#define JISX_0208 3
#define JISX_0212 4

static size_t mb_cp5022x_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			/* Escape sequence */
			if ((e - p) < 2) {
				*out++ = MBFL_BAD_INPUT;
				/* Duplicate error-handling behavior of legacy code */
				if (p < e && (*p == '(' || *p == '$'))
					p++;
				continue;
			}
			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					*state = JISX_0208;
				} else if (c3 == '(') {
					if (p == e) {
						*out++ = MBFL_BAD_INPUT;
						break;
					}
					unsigned char c4 = *p++;
					if (c4 == '@' || c4 == 'B') {
						*state = JISX_0208;
					} else if (c4 == 'D') {
						*state = JISX_0212;
					} else {
						if ((limit - out) < 3) {
							p -= 4;
							break;
						}
						*out++ = MBFL_BAD_INPUT;
						*out++ = '$';
						*out++ = '(';
						p--;
					}
				} else {
					if ((limit - out) < 2) {
						p -= 3;
						break;
					}
					*out++ = MBFL_BAD_INPUT;
					*out++ = '$';
					p--;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				if (c3 == 'B' || c3 == 'H') {
					*state = ASCII;
				} else if (c3 == 'J') {
					*state = JISX_0201_LATIN;
				} else if (c3 == 'I') {
					*state = JISX_0201_KANA;
				} else {
					if ((limit - out) < 2) {
						p -= 3;
						break;
					}
					*out++ = MBFL_BAD_INPUT;
					*out++ = '(';
					p--;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				p--;
			}
		} else if (c == 0xE) {
			*state = JISX_0201_KANA;
		} else if (c == 0xF) {
			*state = ASCII;
		} else if (*state == JISX_0201_LATIN && c == 0x5C) { /* YEN SIGN */
			*out++ = 0xA5;
		} else if (*state == JISX_0201_LATIN && c == 0x7E) { /* OVER LINE */
			*out++ = 0x203E;
		} else if (*state == JISX_0201_KANA && c > 0x20 && c < 0x60) {
			*out++ = 0xFF40 + c;
		} else if (*state >= JISX_0208 && c > 0x20 && c <= 0x97) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				uint32_t w = 0;
				if (*state == JISX_0208) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
						w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
					} else if (s >= 94*94 && s < 114*94) {
						/* MicroSoft extension */
						w = s - 94*94 + 0xE000;
					}
					if (!w)
						w = MBFL_BAD_INPUT;
				} else {
					if (s < jisx0212_ucs_table_size) {
						w = jisx0212_ucs_table[s];
					}
					if (!w)
						w = MBFL_BAD_INPUT;
				}
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static unsigned int lookup_wchar(uint32_t w)
{
	unsigned int s = 0;

	if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
	} else if (w == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
	} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[w - ucs_i_jis_table_min];
	} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[w - ucs_r_jis_table_min];
	} else if (w >= 0xE000 && w <= 0xE757) {
		/* Private Use Area codepoints */
		s = w - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
	}

	if (!s) {
		if (w == 0xA5) { /* YEN SIGN */
			s = 0x1005C;
		} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (w == 0x2225) { /* PARALLEL TO */
			s = 0x2142;
		} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s = 0x215D;
		} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s = 0x224C;
		} else if (w == 0) {
			return 0;
		}
	}

	/* Above, we do a series of lookups in `ucs_*_jis_table` to find a
	 * corresponding kuten code for this Unicode codepoint
	 * If we get zero, that means the codepoint is not in JIS X 0208
	 * On the other hand, if we get a result with the high bits set on both
	 * upper and lower bytes, that is not a code in JIS X 0208 but rather
	 * in JIS X 0213
	 * In either case, check if this codepoint is one of the extensions added
	 * to JIS X 0208 by MicroSoft (to make CP932) */
	if (!s || s >= 0x8080) {
		for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
			if (w == cp932ext1_ucs_table[i]) {
				return (((i / 94) + (cp932ext1_ucs_table_min / 94) + 0x21) << 8) + (i % 94) + 0x21;
			}
		}

		for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
			if (w == cp932ext2_ucs_table[i]) {
				return (((i / 94) + (cp932ext2_ucs_table_min / 94) + 0x21) << 8) + (i % 94) + 0x21;
			}
		}
	}

	return s;
}

static void mb_wchar_to_cp50221(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static void mb_wchar_to_cp50220(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;

	if (buf->state & 0xFFFF00) {
		/* Reprocess cached codepoint */
		w = buf->state >> 8;
		buf->state &= 0xFF;
		goto reprocess_codepoint;
	}

	while (len--) {
		w = *in++;
reprocess_codepoint:

		if (w >= 0xFF61 && w <= 0xFF9F && !len && !end) {
			/* This codepoint may need to combine with the next one,
			 * but the 'next one' will come in a separate buffer */
			buf->state |= w << 8;
			break;
		}

		bool consumed = false;
		w = mb_convert_kana_codepoint(w, len ? *in : 0, &consumed, NULL, MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE);
		if (consumed) {
			/* Two successive codepoints were converted into one */
			in++; len--; consumed = false;
		}

		unsigned int s = lookup_wchar(w);

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		} else if (s < 0x80) {
			/* ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != ASCII) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA0 && s < 0xE0) {
			/* JISX 0201 Kana */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_KANA) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s - 0x80);
		} else if (s <= 0x927E) {
			/* JISX 0208 Kanji */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
			if (buf->state != JISX_0208) {
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else if (s >= 0x10000) {
			/* JISX 0201 Latin; we 'tag' these by adding 0x10000 */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_LATIN) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_cp50221(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = lookup_wchar(w);

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		} else if (s < 0x80) {
			/* ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != ASCII) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA0 && s < 0xE0) {
			/* JISX 0201 Kana */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_KANA) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s - 0x80);
		} else if (s <= 0x927E) {
			/* JISX 0208 Kanji */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
			if (buf->state != JISX_0208) {
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else if (s >= 0x10000) {
			/* JISX 0201 Latin; we 'tag' these by adding 0x10000 */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_LATIN) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_cp50222(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = lookup_wchar(w);

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50222);
		} else if (s < 0x80) {
			/* ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state == JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xF);
				buf->state = ASCII;
			} else if (buf->state != ASCII) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA0 && s < 0xE0) {
			/* JISX 0201 Kana */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			if (buf->state != JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xE);
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s - 0x80);
		} else if (s <= 0x927E) {
			/* JISX 0208 Kanji */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
			if (buf->state == JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xF);
			}
			if (buf->state != JISX_0208) {
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else if (s >= 0x10000) {
			/* JISX 0201 Latin; we 'tag' these by adding 0x10000 */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
			if (buf->state == JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xF);
			}
			if (buf->state != JISX_0201_LATIN) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50222);
		}
	}

	if (end) {
		if (buf->state == JISX_0201_KANA) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, 1);
			out = mb_convert_buf_add(out, 0xF);
		} else if (buf->state != ASCII) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
			out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

#define ASCII          0
#define JISX0201_KANA  0x20
#define JISX0208_KANJI 0x80
#define UDC            0xA0

static int mbfl_filt_conv_2022jpms_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status & 0xF) {
	case 0:
		if (c == 0x1B) {
			filter->status += 2;
		} else if (filter->status == JISX0201_KANA && c > 0x20 && c < 0x60) {
			CK((*filter->output_function)(0xFF40 + c, filter->data));
		} else if ((filter->status == JISX0208_KANJI || filter->status == UDC) && c > 0x20 && c < 0x80) {
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* Kanji, second byte */
	case 1:
		w = 0;
		filter->status &= ~0xF;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7F) {
			s = ((c1 - 0x21) * 94) + c - 0x21;
			if (filter->status == JISX0208_KANJI) {
				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (w == 0) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s >= 0 && s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			} else {
				if (c1 > 0x20 && c1 < 0x35) {
					w = 0xE000 + ((c1 - 0x21) * 94) + c - 0x21;
				} else {
					w = MBFL_BAD_INPUT;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC */
	case 2:
		if (c == '$') {
			filter->status++;
		} else if (c == '(') {
			filter->status += 3;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ */
	case 3:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else if (c == '(') {
			filter->status++;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else if (c == '?') {
			filter->status = UDC;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 'B' || c == 'J') {
			filter->status = 0;
		} else if (c == 'I') {
			filter->status = JISX0201_KANA;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
	}

	return 0;
}

static int mbfl_filt_conv_2022jpms_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define sjistoidx(c1, c2) \
	(((c1) > 0x9f) ? (((c1) - 0xc1) * 188 + (c2) - (((c2) > 0x7e) ? 0x41 : 0x40)) : (((c1) - 0x81) * 188 + (c2) - (((c2) > 0x7e) ? 0x41 : 0x40)))
#define idxtojis1(c) (((c) / 94) + 0x21)
#define idxtojis2(c) (((c) % 94) + 0x21)

static int cp932ext3_cp932ext2_jis(int c)
{
	int idx;

	idx = sjistoidx(0xfa, 0x40) + c;
	if (idx >= sjistoidx(0xfa, 0x5c))
		idx -=  sjistoidx(0xfa, 0x5c) - sjistoidx(0xed, 0x40);
	else if (idx >= sjistoidx(0xfa, 0x55))
		idx -=  sjistoidx(0xfa, 0x55) - sjistoidx(0xee, 0xfa);
	else if (idx >= sjistoidx(0xfa, 0x40))
		idx -=  sjistoidx(0xfa, 0x40) - sjistoidx(0xee, 0xef);
	return idxtojis1(idx) << 8 | idxtojis2(idx);
}

static int mbfl_filt_conv_wchar_2022jpms(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0, s2 = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c < (0xE000 + 20*94)) {
		/* Private User Area (95ku - 114ku) */
		s1 = c - 0xE000;
		c1 = (s1 / 94) + 0x7f;
		c2 = (s1 % 94) + 0x21;
		s1 = (c1 << 8) | c2;
	}

	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		}
	}

	if ((s1 <= 0) || (s1 >= 0xa1a1 && s2 == 0)) { /* not found or X 0212 */
		s1 = -1;
		for (c1 = 0; c1 < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; c1++) {
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = (((c1 / 94) + 0x2D) << 8) + (c1 % 94) + 0x21;
				break;
			}
		}

		if (s1 <= 0) {
			for (c1 = 0; c1 < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; c1++) {
				if (c == cp932ext3_ucs_table[c1]) {
					s1 = cp932ext3_cp932ext2_jis(c1);
					break;
				}
			}
		}

		if (c == 0) {
			s1 = 0;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* latin */
			if (filter->status & 0xFF00) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			CK((*filter->output_function)(s1, filter->data));
			filter->status = 0;
		} else if (s1 > 0xA0 && s1 < 0xE0) { /* kana */
			if ((filter->status & 0xFF00) != 0x100) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('I', filter->data));
			}
			filter->status = 0x100;
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		} else if (s1 < 0x7E7F) { /* X 0208 */
			if ((filter->status & 0xFF00) != 0x200) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s1 >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		} else if (s1 < 0x927F) { /* UDC */
			if ((filter->status & 0xFF00) != 0x800) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('?', filter->data));
			}
			filter->status = 0x800;
			CK((*filter->output_function)(((s1 >> 8) - 0x5E) & 0x7F, filter->data));
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_any_2022jpms_flush(mbfl_convert_filter *filter)
{
	/* Go back to ASCII (so strings can be safely concatenated) */
	if ((filter->status & 0xFF00) != 0) {
		CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
		CK((*filter->output_function)('(', filter->data));
		CK((*filter->output_function)('B', filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_iso2022jpms_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			if ((e - p) < 2) {
				*out++ = MBFL_BAD_INPUT;
				p = e;
				break;
			}
			unsigned char c2 = *p++;
			unsigned char c3 = *p++;

			if (c2 == '$') {
				if (c3 == '@' || c3 == 'B') {
					*state = JISX0208_KANJI;
				} else if (c3 == '(' && p < e) {
					unsigned char c4 = *p++;

					if (c4 == '@' || c4 == 'B') {
						*state = JISX0208_KANJI;
					} else if (c4 == '?') {
						*state = UDC;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (c2 == '(') {
				if (c3 == 'B' || c3 == 'J') {
					*state = ASCII;
				} else if (c3 == 'I') {
					*state = JISX0201_KANA;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				p--;
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (*state == JISX0201_KANA && c >= 0x21 && c <= 0x5F) {
			*out++ = 0xFF40 + c;
		} else if ((*state == JISX0208_KANJI || *state == UDC) && c >= 0x21 && c <= 0x7F) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			unsigned int w = 0;

			if (c2 >= 0x21 && c2 <= 0x7E) {
				unsigned int s = ((c - 0x21) * 94) + c2 - 0x21;
				if (*state == JISX0208_KANJI) {
					if (s <= 137) {
						if (s == 31) {
							w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
						} else if (s == 32) {
							w = 0xFF5E; /* FULLWIDTH TILDE */
						} else if (s == 33) {
							w = 0x2225; /* PARALLEL TO */
						} else if (s == 60) {
							w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
						} else if (s == 80) {
							w = 0xFFE0; /* FULLWIDTH CENT SIGN */
						} else if (s == 81) {
							w = 0xFFE1; /* FULLWIDTH POUND SIGN */
						} else if (s == 137) {
							w = 0xFFE2; /* FULLWIDTH NOT SIGN */
						}
					}

					if (!w) {
						if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
							w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
						} else if (s < jisx0208_ucs_table_size) {
							w = jisx0208_ucs_table[s];
						} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
							w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
						}
					}
				} else if (c >= 0x21 && c <= 0x34) {
					w = 0xE000 + ((c - 0x21) * 94) + c2 - 0x21;
				}

				*out++ = w ? w : MBFL_BAD_INPUT;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jpms(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = ((((w - 0xE000) / 94) + 0x7F) << 8) | (((w - 0xE000) % 94) + 0x21);
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (s >= 0xA1A1) /* JISX 0212 */
			s = 0;

		if (!s && w) {
			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					break;
				}
			}

			if (!s) {
				for (int i = 0; i < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; i++) {
					if (w == cp932ext3_ucs_table[i]) {
						s = cp932ext3_cp932ext2_jis(i);
						break;
					}
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jpms);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA1 && s <= 0xDF) {
			if (buf->state != JISX0201_KANA) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX0201_KANA;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else if (s <= 0x7E7E) {
			if (buf->state != JISX0208_KANJI) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX0208_KANJI;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0x7F);
		} else if (s < 0x927F) {
			if (buf->state != UDC) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', '?');
				buf->state = UDC;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, ((s >> 8) - 0x5E) & 0x7F, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jpms);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_2022kr_wchar(int c, mbfl_convert_filter *filter)
{
	int w = 0;

	switch (filter->status & 0xf) {
	/* case 0x00: ASCII */
	/* case 0x10: KSC5601 */
	case 0:
		if (c == 0x1b) { /* ESC */
			filter->status += 2;
		} else if (c == 0x0f) { /* shift in (ASCII) */
			filter->status = 0;
		} else if (c == 0x0e) { /* shift out (KSC5601) */
			filter->status = 0x10;
		} else if ((filter->status & 0x10) && c > 0x20 && c < 0x7f) {
			/* KSC5601 lead byte */
			filter->cache = c;
			filter->status = 0x11;
		} else if ((filter->status & 0x10) == 0 && c >= 0 && c < 0x80) {
			/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0x10;
		int c1 = filter->cache;
		int flag = 0;

		if (c1 > 0x20 && c1 < 0x47) {
			flag = 1;
		} else if (c1 >= 0x47 && c1 <= 0x7e && c1 != 0x49) {
			flag = 2;
		}

		if (flag > 0 && c > 0x20 && c < 0x7f) {
			if (flag == 1) {
				if (c1 != 0x22 || c <= 0x65) {
					w = (c1 - 1)*190 + (c - 0x41) + 0x80;
					ZEND_ASSERT(w < uhc1_ucs_table_size);
					w = uhc1_ucs_table[w];
				}
			} else {
				w = (c1 - 0x47)*94 + c - 0x21;
				if (w < uhc3_ucs_table_size) {
					w = uhc3_ucs_table[w];
				} else {
					w = MBFL_BAD_INPUT;
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2: /* ESC */
		if (c == '$') {
			filter->status++;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* ESC $ */
		if (c == ')') {
			filter->status++;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 4: /* ESC $ ) */
		filter->status = 0;
		if (c != 'C') {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_2022kr_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* 2-byte character was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_2022kr(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s = 0;

	if ((filter->status & 0x100) == 0) {
		CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
		CK((*filter->output_function)('$', filter->data));
		CK((*filter->output_function)(')', filter->data));
		CK((*filter->output_function)('C', filter->data));
		filter->status |= 0x100;
	}

	if (c >= ucs_a1_uhc_table_min && c < ucs_a1_uhc_table_max) {
		s = ucs_a1_uhc_table[c - ucs_a1_uhc_table_min];
	} else if (c >= ucs_a2_uhc_table_min && c < ucs_a2_uhc_table_max) {
		s = ucs_a2_uhc_table[c - ucs_a2_uhc_table_min];
	} else if (c >= ucs_a3_uhc_table_min && c < ucs_a3_uhc_table_max) {
		s = ucs_a3_uhc_table[c - ucs_a3_uhc_table_min];
	} else if (c >= ucs_i_uhc_table_min && c < ucs_i_uhc_table_max) {
		s = ucs_i_uhc_table[c - ucs_i_uhc_table_min];
	} else if (c >= ucs_s_uhc_table_min && c < ucs_s_uhc_table_max) {
		s = ucs_s_uhc_table[c - ucs_s_uhc_table_min];
	} else if (c >= ucs_r1_uhc_table_min && c < ucs_r1_uhc_table_max) {
		s = ucs_r1_uhc_table[c - ucs_r1_uhc_table_min];
	} else if (c >= ucs_r2_uhc_table_min && c < ucs_r2_uhc_table_max) {
		s = ucs_r2_uhc_table[c - ucs_r2_uhc_table_min];
	}

	c1 = (s >> 8) & 0xff;
	c2 = s & 0xff;
	/* exclude UHC extension area */
	if (c1 < 0xa1 || c2 < 0xa1) {
		s = c;
	} else if (s & 0x8000) {
		s -= 0x8080;
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else {
			s = -1;
		}
	} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
		s = -1;
	}

	if (s >= 0) {
		if (s < 0x80 && s >= 0) { /* ASCII */
			if (filter->status & 0x10) {
				CK((*filter->output_function)(0x0f, filter->data)); /* shift in */
				filter->status &= ~0x10;
			}
			CK((*filter->output_function)(s, filter->data));
		} else {
			if ((filter->status & 0x10) == 0) {
				CK((*filter->output_function)(0x0e, filter->data)); /* shift out */
				filter->status |= 0x10;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_any_2022kr_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* Escape sequence or 2-byte character was truncated */
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}
	/* back to ascii */
	if (filter->status & 0x10) {
		CK((*filter->output_function)(0x0f, filter->data)); /* shift in */
	}

	filter->status = filter->cache = 0;

	if (filter->flush_function) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

#define ASCII 0
#define KSC5601 1

static size_t mb_iso2022kr_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			if ((e - p) < 3) {
				*out++ = MBFL_BAD_INPUT;
				if (p < e && *p++ == '$') {
					if (p < e) {
						p++;
					}
				}
				continue;
			}
			unsigned char c2 = *p++;
			unsigned char c3 = *p++;
			unsigned char c4 = *p++;
			if (c2 == '$' && c3 == ')' && c4 == 'C') {
				*state = ASCII;
			} else {
				if (c3 != ')') {
					p--;
					if (c2 != '$')
						p--;
				}
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0xF) {
			*state = ASCII;
		} else if (c == 0xE) {
			*state = KSC5601;
		} else if (c >= 0x21 && c <= 0x7E && *state == KSC5601) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			unsigned int w = 0;

			if (c2 < 0x21 || c2 > 0x7E) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			if (c < 0x47) {
				if (c != 0x22 || c2 <= 0x65) {
					w = (c - 1)*190 + c2 - 0x41 + 0x80;
					ZEND_ASSERT(w < uhc1_ucs_table_size);
					w = uhc1_ucs_table[w];
				}
			} else if (c != 0x49 && c <= 0x7D) {
				w = (c - 0x47)*94 + c2 - 0x21;
				ZEND_ASSERT(w < uhc3_ucs_table_size);
				w = uhc3_ucs_table[w];
			}

			if (!w)
				w = MBFL_BAD_INPUT;
			*out++ = w;
		} else if (c < 0x80 && *state == ASCII) {
			*out++ = c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

#define EMITTED_ESC_SEQUENCE 0x10

static void mb_wchar_to_iso2022kr(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);

	/* This escape sequence needs to come *somewhere* at the beginning of a line before
	 * we can use the Shift In/Shift Out bytes, but it only needs to come once in a string
	 * Rather than tracking newlines, we can just emit the sequence once at the beginning
	 * of the output string... since that will always be "the beginning of a line" */
	if (len && !(buf->state & EMITTED_ESC_SEQUENCE)) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 4 + len);
		out = mb_convert_buf_add4(out, 0x1B, '$', ')', 'C');
		buf->state |= EMITTED_ESC_SEQUENCE;
	} else {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
	}

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_uhc_table_min && w < ucs_a1_uhc_table_max) {
			s = ucs_a1_uhc_table[w - ucs_a1_uhc_table_min];
		} else if (w >= ucs_a2_uhc_table_min && w < ucs_a2_uhc_table_max) {
			s = ucs_a2_uhc_table[w - ucs_a2_uhc_table_min];
		} else if (w >= ucs_a3_uhc_table_min && w < ucs_a3_uhc_table_max) {
			s = ucs_a3_uhc_table[w - ucs_a3_uhc_table_min];
		} else if (w >= ucs_i_uhc_table_min && w < ucs_i_uhc_table_max) {
			s = ucs_i_uhc_table[w - ucs_i_uhc_table_min];
		} else if (w >= ucs_s_uhc_table_min && w < ucs_s_uhc_table_max) {
			s = ucs_s_uhc_table[w - ucs_s_uhc_table_min];
		} else if (w >= ucs_r1_uhc_table_min && w < ucs_r1_uhc_table_max) {
			s = ucs_r1_uhc_table[w - ucs_r1_uhc_table_min];
		} else if (w >= ucs_r2_uhc_table_min && w < ucs_r2_uhc_table_max) {
			s = ucs_r2_uhc_table[w - ucs_r2_uhc_table_min];
		}

		if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
			s = w;
		} else {
			s -= 0x8080;
		}

		if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022kr);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s < 0x80) {
			if ((buf->state & 1) != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				out = mb_convert_buf_add(out, 0xF);
				buf->state &= ~KSC5601;
			}
			out = mb_convert_buf_add(out, s);
		} else {
			if ((buf->state & 1) != KSC5601) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
				out = mb_convert_buf_add(out, 0xE);
				buf->state |= KSC5601;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	if (end && (buf->state & 1) != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 1);
		out = mb_convert_buf_add(out, 0xF);
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static const struct mbfl_convert_vtbl vtbl_jis_wchar = {
	mbfl_no_encoding_jis,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis_wchar,
	mbfl_filt_conv_jis_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_jis = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_jis,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_jis = {
	mbfl_no_encoding_jis,
	"JIS",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_jis_wchar,
	&vtbl_wchar_jis,
	mb_iso2022jp_to_wchar,
	mb_wchar_to_jis,
	mb_check_jis
};

static const struct mbfl_convert_vtbl vtbl_2022jp_wchar = {
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis_wchar,
	mbfl_filt_conv_jis_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_2022jp = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022jp,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_2022jp = {
	mbfl_no_encoding_2022jp,
	"ISO-2022-JP",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_wchar,
	&vtbl_wchar_2022jp,
	mb_iso2022jp_to_wchar,
	mb_wchar_to_iso2022jp,
	mb_check_iso2022jp
};

static const char *mbfl_encoding_2022jp_kddi_aliases[] = {"ISO-2022-JP-KDDI", NULL};

static const struct mbfl_convert_vtbl vtbl_2022jp_kddi_wchar = {
	mbfl_no_encoding_2022jp_kddi,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_2022jp_mobile_wchar,
	mbfl_filt_conv_2022jp_mobile_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_2022jp_kddi = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_kddi,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022jp_mobile,
	mbfl_filt_conv_wchar_2022jp_mobile_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_2022jp_kddi = {
	mbfl_no_encoding_2022jp_kddi,
	"ISO-2022-JP-MOBILE#KDDI",
	"ISO-2022-JP",
	mbfl_encoding_2022jp_kddi_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_kddi_wchar,
	&vtbl_wchar_2022jp_kddi,
	mb_iso2022jp_kddi_to_wchar,
	mb_wchar_to_iso2022jp_kddi,
	NULL
};

static const struct mbfl_convert_vtbl vtbl_2022jp_2004_wchar = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_2022jp_2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	"ISO-2022-JP-2004",
	"ISO-2022-JP-2004",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_2004_wchar,
	&vtbl_wchar_2022jp_2004,
	mb_iso2022jp2004_to_wchar,
	mb_wchar_to_iso2022jp2004,
	NULL
};

/* Previously, a dubious 'encoding' called 'cp50220raw' was supported
 * This was just CP50220, but the implementation was less strict regarding
 * invalid characters; it would silently pass some through
 * This 'encoding' only existed in mbstring. In case some poor, lost soul is
 * still using it, retain minimal support by aliasing it to CP50220
 *
 * Further, mbstring also had a made-up encoding called "JIS-ms"
 * This was the same as CP5022{0,1,2}, but without their special ways of
 * handling conversion of Unicode half-width katakana */
static const char *cp50220_aliases[] = {"cp50220raw", "cp50220-raw", "JIS-ms", NULL};

static const struct mbfl_convert_vtbl vtbl_cp50220_wchar = {
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp50220 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50220,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50220,
	mbfl_filt_conv_wchar_cp50220_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_cp50221_wchar = {
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp50221 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50221,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_cp50222_wchar = {
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp50222 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50222,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50222,
	mbfl_filt_conv_wchar_cp50222_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_cp50220 = {
	mbfl_no_encoding_cp50220,
	"CP50220",
	"ISO-2022-JP",
	cp50220_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50220_wchar,
	&vtbl_wchar_cp50220,
	mb_cp5022x_to_wchar,
	mb_wchar_to_cp50220,
	NULL
};

const mbfl_encoding mbfl_encoding_cp50221 = {
	mbfl_no_encoding_cp50221,
	"CP50221",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50221_wchar,
	&vtbl_wchar_cp50221,
	mb_cp5022x_to_wchar,
	mb_wchar_to_cp50221,
	NULL
};

const mbfl_encoding mbfl_encoding_cp50222 = {
	mbfl_no_encoding_cp50222,
	"CP50222",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50222_wchar,
	&vtbl_wchar_cp50222,
	mb_cp5022x_to_wchar,
	mb_wchar_to_cp50222,
	NULL
};

static const char *mbfl_encoding_2022jpms_aliases[] = {"ISO2022JPMS", NULL};

static const struct mbfl_convert_vtbl vtbl_2022jpms_wchar = {
	mbfl_no_encoding_2022jpms,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_2022jpms_wchar,
	mbfl_filt_conv_2022jpms_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_2022jpms = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jpms,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022jpms,
	mbfl_filt_conv_any_2022jpms_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_2022jpms = {
	mbfl_no_encoding_2022jpms,
	"ISO-2022-JP-MS",
	"ISO-2022-JP",
	mbfl_encoding_2022jpms_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jpms_wchar,
	&vtbl_wchar_2022jpms,
	mb_iso2022jpms_to_wchar,
	mb_wchar_to_iso2022jpms,
	NULL
};

/* ISO-2022-KR is defined in RFC 1557
 *
 * The RFC says that ESC $ ) C must appear once in a ISO-2022-KR string,
 * at the beginning of a line, before any instances of the Shift In or
 * Shift Out bytes which are used to switch between ASCII/KSC 5601 modes
 *
 * We don't enforce that for ISO-2022-KR input */

static const struct mbfl_convert_vtbl vtbl_wchar_2022kr = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022kr,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022kr,
	mbfl_filt_conv_any_2022kr_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_2022kr_wchar = {
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_2022kr_wchar,
	mbfl_filt_conv_2022kr_wchar_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_2022kr = {
	mbfl_no_encoding_2022kr,
	"ISO-2022-KR",
	"ISO-2022-KR",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022kr_wchar,
	&vtbl_wchar_2022kr,
	mb_iso2022kr_to_wchar,
	mb_wchar_to_iso2022kr,
	NULL
};

/*
 * SJIS variants
 */

static int mbfl_filt_conv_sjis_wchar(int c, mbfl_convert_filter *filter)
{
	int s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else if (c > 0x80 && c < 0xF0 && c != 0xA0) { /* Kanji, first byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* Kanji, second byte */
		filter->status = 0;
		int c1 = filter->cache;
		if (c >= 0x40 && c <= 0xFC && c != 0x7F) {
			SJIS_DECODE(c1, c, s1, s2);
			w = (s1 - 0x21)*94 + s2 - 0x21;
			if (w >= 0 && w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
			} else {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
	}

	return 0;
}

static int mbfl_filt_conv_sjis_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status && filter->status != 4) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_sjis(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0, s2;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xAF || c == 0x203E) { /* U+00AF is MACRON, U+203E is OVERLINE */
			s1 = 0x2131; /* FULLWIDTH MACRON */
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		} else if (c == 0) {
			s1 = 0;
		} else {
			s1 = -1;
		}
	} else if (s1 >= 0x8080) { /* JIS X 0212; not supported */
		s1 = -1;
	}

	if (s1 >= 0) {
		if (s1 < 0x100) { /* Latin/Kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* Kanji */
			c1 = (s1 >> 8) & 0xFF;
			c2 = s1 & 0xFF;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static const unsigned short sjis_decode_tbl1[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFFFF, 0, 188, 376, 564, 752, 940, 1128, 1316, 1504, 1692, 1880, 2068, 2256, 2444, 2632, 2820, 3008, 3196, 3384, 3572, 3760, 3948, 4136, 4324, 4512, 4700, 4888, 5076, 5264, 5452, 5640, 0xFFFF, -6016, -5828, -5640, -5452, -5264, -5076, -4888, -4700, -4512, -4324, -4136, -3948, -3760, -3572, -3384, -3196, -3008, -2820, -2632, -2444, -2256, -2068, -1880, -1692, -1504, -1316, -1128, -940, -752, -564, -376, -188, 0, 188, 376, 564, 752, 940, 1128, 1316, 1504, 1692, 1880, 2068, 2256, 2444, 2632, 2820, 3008, 3196, 3384, 3572, 3760, 3948, 4136, 4324, 4512, 4700, 4888, 5076, 5264, 5452, 5640, 5828, 6016, 6204, 6392, 6580, 6768, 6956, 7144, 7332, 7520, 7708, 7896, 8084, 8272, 8460, 8648, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

static const unsigned short sjis_decode_tbl2[] = {
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 0xFFFF, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 0xFFFF, 0xFFFF, 0xFFFF
};

static size_t mb_sjis_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	e--; /* Stop the main loop 1 byte short of the end of the input */

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) { /* Kana */
			*out++ = 0xFEC0 + c;
		} else {
			/* Don't need to check p < e; it's not possible to go out of bounds here, due to e-- above */
			unsigned char c2 = *p++;
			/* This is only legal if c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F
			 * But the values in the above conversion tables have been chosen such that
			 * illegal values of c2 will always result in w > jisx0208_ucs_table_size,
			 * so we don't need to do a separate bounds check on c2
			 * Likewise, the values in the conversion tables are such that illegal values
			 * for c will always result in w > jisx0208_ucs_table_size */
			uint32_t w = sjis_decode_tbl1[c] + sjis_decode_tbl2[c2];
			if (w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				if (c == 0x80 || c == 0xA0 || c > 0xEF) {
					p--;
				}
				*out++ = MBFL_BAD_INPUT;
			}
		}
	}

	/* Finish up last byte of input string if there is one */
	if (p == e && out < limit) {
		unsigned char c = *p++;
		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p + 1;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_sjis(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xAF || w == 0x203E) {
				s = 0x2131; /* FULLWIDTH MACRON */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w != 0) {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		} else if (s >= 0x8080) { /* JIS X 0212; not supported */
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

		if (s <= 0xFF) {
			/* Latin/Kana */
			out = mb_convert_buf_add(out, s);
		} else {
			/* Kanji */
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s2;
			SJIS_ENCODE(c1, c2, s, s2);
			out = mb_convert_buf_add2(out, s, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_sjis_mac_wchar(int c, mbfl_convert_filter *filter)
{
	int i, j, n;
	int c1, s, s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80 && c != 0x5c) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else if (c > 0x80 && c <= 0xed && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x5c) {
			CK((*filter->output_function)(0x00a5, filter->data));
		} else if (c == 0x80) {
			CK((*filter->output_function)(0x005c, filter->data));
		} else if (c == 0xa0) {
			CK((*filter->output_function)(0x00a0, filter->data));
		} else if (c == 0xfd) {
			CK((*filter->output_function)(0x00a9, filter->data));
		} else if (c == 0xfe) {
			CK((*filter->output_function)(0x2122, filter->data));
		} else if (c == 0xff) {
			CK((*filter->output_function)(0x2026, filter->data));
			CK((*filter->output_function)(0xf87f, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:		/* kanji second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = (s1 - 0x21)*94 + s2 - 0x21;
			if (s <= 0x89) {
				if (s == 0x1c) {
					w = 0x2014;		    /* EM DASH */
				} else if (s == 0x1f) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 0x20) {
					w = 0x301c;			/* FULLWIDTH TILDE */
				} else if (s == 0x21) {
					w = 0x2016;			/* PARALLEL TO */
				} else if (s == 0x3c) {
					w = 0x2212;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 0x50) {
					w = 0x00a2;			/* FULLWIDTH CENT SIGN */
				} else if (s == 0x51) {
					w = 0x00a3;			/* FULLWIDTH POUND SIGN */
				} else if (s == 0x89) {
					w = 0x00ac;			/* FULLWIDTH NOT SIGN */
				}
			}

			/* apple gaiji area 0x8540 - 0x886d */
			if (w == 0) {
				for (i=0; i<7; i++) {
					if (s >= code_tbl[i][0] && s <= code_tbl[i][1]) {
						w = s - code_tbl[i][0] + code_tbl[i][2];
						break;
					}
				}
			}

			if (w == 0) {

				for (i=0; i<code_tbl_m_len; i++) {
					if (s == code_tbl_m[i][0]) {
						if (code_tbl_m[i][1] == 0xf860) {
							n = 4;
						} else if (code_tbl_m[i][1] == 0xf861) {
							n = 5;
						} else {
							n = 6;
						}
						for (j=1; j<n-1; j++) {
							CK((*filter->output_function)(code_tbl_m[i][j], filter->data));
						}
						w = code_tbl_m[i][n-1];
						break;
					}
				}
			}

			if (w == 0) {
				for (i=0; i<8; i++) {
					if (s >= code_ofst_tbl[i][0] && s <= code_ofst_tbl[i][1]) {
						w = code_map[i][s - code_ofst_tbl[i][0]];
						if (w == 0) {
							CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
							return 0;
						}
						s2 = 0;
						if (s >= 0x043e && s <= 0x0441) {
							s2 = 0xf87a;
						} else if (s == 0x03b1 || s == 0x03b7) {
							s2 = 0xf87f;
						} else if (s == 0x04b8 || s == 0x04b9 || s == 0x04c4) {
							s2 = 0x20dd;
						} else if (s == 0x1ed9 || s == 0x1eda || s == 0x1ee8 || s == 0x1ef3 ||
								   (s >= 0x1ef5 && s <= 0x1efb) || s == 0x1f05 || s == 0x1f06 ||
								   s == 0x1f18 || (s >= 0x1ff2 && s <= 0x20a5)) {
							s2 = 0xf87e;
						}
						if (s2 > 0) {
							CK((*filter->output_function)(w, filter->data));
							w = s2;
						}
						break;
					}
				}
			}

			if (w == 0 && s >= 0 && s < jisx0208_ucs_table_size) {	/* X 0208 */
				w = jisx0208_ucs_table[s];
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_sjis_mac(int c, mbfl_convert_filter *filter)
{
	int i, c1, c2, s1 = 0, s2 = 0, mode;

	// a1: U+0000 -> U+046F
	// a2: U+2000 -> U+30FF
	//  i: U+4E00 -> U+9FFF
	//  r: U+FF00 -> U+FFFF

	switch (filter->status) {
	case 1:
		c1 = filter->cache;
		filter->cache = filter->status = 0;

		if (c == 0xf87a) {
			for (i = 0; i < 4; i++) {
				if (c1 == s_form_tbl[i+34+3+3]) {
					s1 = s_form_sjis_tbl[i+34+3+3];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
			}
		} else if (c == 0x20dd) {
			for (i = 0; i < 3; i++) {
				if (c1 == s_form_tbl[i+34+3]) {
					s1 = s_form_sjis_tbl[i+34+3];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
			}
		} else if (c == 0xf87f) {
			for (i = 0; i < 3; i++) {
				if (c1 == s_form_tbl[i+34]) {
					s1 = s_form_sjis_tbl[i+34];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
				s1 = -1;
			}
		} else if (c == 0xf87e) {
			for (i = 0; i < 34; i++) {
				if (c1 == s_form_tbl[i]) {
					s1 = s_form_sjis_tbl[i];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
				s1 = -1;
			}
		} else {
			s2 = c1;
			s1 = c;
		}

		if (s2 > 0) {
			for (i = 0; i < s_form_tbl_len; i++) {
				if (c1 == s_form_tbl[i]) {
					s1 = s_form_sjis_fallback_tbl[i];
					break;
				}
			}
		}

		if (s1 >= 0) {
			if (s1 < 0x100) {
				CK((*filter->output_function)(s1, filter->data));
			} else {
				CK((*filter->output_function)((s1 >> 8) & 0xff, filter->data));
				CK((*filter->output_function)(s1 & 0xff, filter->data));
			}
		} else {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}

		if (s2 <= 0 || s1 == -1) {
			break;
		}
		s1 = s2 = 0;
		ZEND_FALLTHROUGH;

	case 0:
		if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
			s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
			if (c == 0x5c) {
				s1 = 0x80;
			} else if (c == 0xa9) {
				s1 = 0xfd;
			}
		} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
			s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
			if (c == 0x2122) {
				s1 = 0xfe;
			} else if (c == 0x2014) {
				s1 = 0x213d;
			} else if (c == 0x2116) {
				s1 = 0x2c1d;
			}
		} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
			s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
		} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
			s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
		}

		if (c >= 0x2000) {
			for (i = 0; i < s_form_tbl_len; i++) {
				if (c == s_form_tbl[i]) {
					filter->status = 1;
					filter->cache = c;
					return 0;
				}
			}

			if (c == 0xf860 || c == 0xf861 || c == 0xf862) {
				/* Apple 'transcoding hint' codepoints (from private use area) */
				filter->status = 2;
				filter->cache = c;
				return 0;
			}
		}

		if (s1 <= 0) {
			if (c == 0xa0) {
				s1 = 0x00a0;
			} else if (c == 0xa5) { /* YEN SIGN */
				/* Unicode has codepoint 0xFFE5 for a fullwidth Yen sign;
				 * convert codepoint 0xA5 to halfwidth Yen sign */
				s1 = 0x5c; /* HALFWIDTH YEN SIGN */
			} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
				s1 = 0x2140;
			}
		}

		if (s1 <= 0) {
			for (i=0; i<wchar2sjis_mac_r_tbl_len; i++) {
				if (c >= wchar2sjis_mac_r_tbl[i][0] && c <= wchar2sjis_mac_r_tbl[i][1]) {
					s1 = c - wchar2sjis_mac_r_tbl[i][0] + wchar2sjis_mac_r_tbl[i][2];
					break;
				}
			}

			if (s1 <= 0) {
				for (i=0; i<wchar2sjis_mac_r_map_len; i++) {
					if (c >= wchar2sjis_mac_r_map[i][0] && c <= wchar2sjis_mac_r_map[i][1]) {
						s1 = wchar2sjis_mac_code_map[i][c-wchar2sjis_mac_r_map[i][0]];
						break;
					}
				}
			}

			if (s1 <= 0) {
				for (i=0; i<wchar2sjis_mac_wchar_tbl_len ; i++) {
					if ( c == wchar2sjis_mac_wchar_tbl[i][0]) {
						s1 = wchar2sjis_mac_wchar_tbl[i][1] & 0xffff;
						break;
					}
				}
			}

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				s1 = (c1 << 8) | c2;
				s2 = 1;
			}
		}

		if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
			s1 = -1;
			c1 = 0;

			if (c == 0) {
				s1 = 0;
			} else if (s1 <= 0) {
				s1 = -1;
			}
		}

		if (s1 >= 0) {
			if (s1 < 0x100) { /* latin or kana */
				CK((*filter->output_function)(s1, filter->data));
			} else { /* kanji */
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			}
		} else {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
		break;

	case 2:
		c1 = filter->cache;
		filter->cache = 0;
		filter->status = 0;
		if (c1 == 0xf860) {
			for (i = 0; i < 5; i++) {
				if (c == code_tbl_m[i][2]) {
					filter->cache = c | 0x10000;
					filter->status = 3;
					break;
				}
			}
		} else if (c1 == 0xf861) {
			for (i = 0; i < 3; i++) {
				if (c == code_tbl_m[i+5][2]) {
					filter->cache = c | 0x20000;
					filter->status = 3;
					break;
				}
			}
		} else if (c1 == 0xf862) {
			for (i = 0; i < 4; i++) {
				if (c == code_tbl_m[i+5+3][2]) {
					filter->cache = c | 0x40000;
					filter->status = 3;
					break;
				}
			}
		}

		if (filter->status == 0) {
			/* Didn't find any of expected codepoints after Apple transcoding hint */
			CK(mbfl_filt_conv_illegal_output(c1, filter));
			return mbfl_filt_conv_wchar_sjis_mac(c, filter);
		}
		break;

	case 3:
		s1 = 0;
		c1 = filter->cache & 0xffff;
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = filter->status = 0;

		if (mode == 0x1) {
			for (i = 0; i < 5; i++) {
				if (c1 == code_tbl_m[i][2] && c == code_tbl_m[i][3]) {
					s1 = code_tbl_m[i][0];
					break;
				}
			}

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			} else {
				CK(mbfl_filt_conv_illegal_output(0xf860, filter));
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		} else if (mode == 0x2) {
			for (i = 0; i < 3; i++) {
				if (c1 == code_tbl_m[i+5][2] && c == code_tbl_m[i+5][3]) {
					filter->cache = c | 0x20000;
					filter->status = 4;
					break;
				}
			}
		} else if (mode == 0x4) {
			for (i = 0; i < 4; i++) {
				if (c1 == code_tbl_m[i+8][2] && c == code_tbl_m[i+8][3]) {
					filter->cache = c | 0x40000;
					filter->status = 4;
					break;
				}
			}
		}
		break;

	case 4:
		s1 = 0;
		c1 = filter->cache & 0xffff;
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = 0;
		filter->status = 0;

		if (mode == 0x2) {
			for (i = 0; i < 3; i++) {
				if (c1 == code_tbl_m[i+5][3] && c == code_tbl_m[i+5][4]) {
					s1 = code_tbl_m[i+5][0];
					break;
				}
			}

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			} else {
				CK(mbfl_filt_conv_illegal_output(0xf861, filter));
				for (i = 0; i < 3; i++) {
					if (c1 == code_tbl_m[i+5][3]) {
						CK(mbfl_filt_conv_illegal_output(code_tbl_m[i+5][2], filter));
						break;
					}
				}
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		} else if (mode == 0x4) {
			for (i = 0; i < 4; i++) {
				if (c1 == code_tbl_m[i+8][3] && c == code_tbl_m[i+8][4]) {
					filter->cache = c | 0x40000;
					filter->status = 5;
					break;
				}
			}
		}
		break;

	case 5:
		s1 = 0;
		c1 = filter->cache & 0xffff;
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = filter->status = 0;

		if (mode == 0x4) {
			for (i = 0; i < 4; i++) {
				if (c1 == code_tbl_m[i+8][4] && c == code_tbl_m[i+8][5]) {
					s1 = code_tbl_m[i+8][0];
					break;
				}
			}

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			} else {
				CK(mbfl_filt_conv_illegal_output(0xf862, filter));
				for (i = 0; i < 4; i++) {
					if (c1 == code_tbl_m[i+8][4]) {
						CK(mbfl_filt_conv_illegal_output( code_tbl_m[i+8][2], filter));
						CK(mbfl_filt_conv_illegal_output( code_tbl_m[i+8][3], filter));
						break;
					}
				}
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_sjis_mac_flush(mbfl_convert_filter *filter)
{
	int i, c1, s1 = 0;
	if (filter->status == 1 && filter->cache > 0) {
		c1 = filter->cache;
		for (i=0;i<s_form_tbl_len;i++) {
			if (c1 == s_form_tbl[i]) {
				s1 = s_form_sjis_fallback_tbl[i];
				break;
			}
		}
		if (s1 > 0) {
			CK((*filter->output_function)((s1 >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s1 & 0xff, filter->data));
		}
	}
	filter->cache = 0;
	filter->status = 0;

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_sjismac_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	/* A single SJIS-Mac kuten code can convert to up to 5 Unicode codepoints, oh my! */
	ZEND_ASSERT(bufsize >= 5);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x80 || c == 0xA0) {
			if (c == 0x5C) {
				*out++ = 0xA5;
			} else if (c == 0x80) {
				*out++ = 0x5C;
			} else {
				*out++ = c;
			}
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else if (c <= 0xED) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			uint32_t w = sjis_decode_tbl1[c] + sjis_decode_tbl2[c2];

			if (w <= 0x89) {
				if (w == 0x1C) {
					*out++ = 0x2014; /* EM DASH */
					continue;
				} else if (w == 0x1F) {
					*out++ = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					continue;
				} else if (w == 0x20) {
					*out++ = 0x301C; /* FULLWIDTH TILDE */
					continue;
				} else if (w == 0x21) {
					*out++ = 0x2016; /* PARALLEL TO */
					continue;
				} else if (w == 0x3C) {
					*out++ = 0x2212; /* FULLWIDTH HYPHEN-MINUS */
					continue;
				} else if (w == 0x50) {
					*out++ = 0xA2; /* FULLWIDTH CENT SIGN */
					continue;
				} else if (w == 0x51) {
					*out++ = 0xA3; /* FULLWIDTH POUND SIGN */
					continue;
				} else if (w == 0x89) {
					*out++ = 0xAC; /* FULLWIDTH NOT SIGN */
					continue;
				}
			} else {
				if (w >= 0x2F0 && w <= 0x3A3) {
					for (int i = 0; i < 7; i++) {
						if (w >= code_tbl[i][0] && w <= code_tbl[i][1]) {
							*out++ = w - code_tbl[i][0] + code_tbl[i][2];
							goto next_iteration;
						}
					}
				}

				if (w >= 0x340 && w <= 0x523) {
					for (int i = 0; i < code_tbl_m_len; i++) {
						if (w == code_tbl_m[i][0]) {
							int n = 5;
							if (code_tbl_m[i][1] == 0xF860) {
								n = 3;
							} else if (code_tbl_m[i][1] == 0xF861) {
								n = 4;
							}
							if ((limit - out) < n) {
								p -= 2;
								goto finished;
							}
							for (int j = 1; j <= n; j++) {
								*out++ = code_tbl_m[i][j];
							}
							goto next_iteration;
						}
					}
				}

				if (w >= 0x3AC && w <= 0x20A5) {
					for (int i = 0; i < 8; i++) {
						if (w >= code_ofst_tbl[i][0] && w <= code_ofst_tbl[i][1]) {
							uint32_t w2 = code_map[i][w - code_ofst_tbl[i][0]];
							if (!w2) {
								*out++ = MBFL_BAD_INPUT;
								goto next_iteration;
							}
							if ((limit - out) < 2) {
								p -= 2;
								goto finished;
							}
							*out++ = w2;
							if (w >= 0x43E && w <= 0x441) {
								*out++ = 0xF87A;
							} else if (w == 0x3B1 || w == 0x3B7) {
								*out++ = 0xF87F;
							} else if (w == 0x4B8 || w == 0x4B9 || w == 0x4C4) {
								*out++ = 0x20DD;
							} else if (w == 0x1ED9 || w == 0x1EDA || w == 0x1EE8 || w == 0x1EF3 || (w >= 0x1EF5 && w <= 0x1EFB) || w == 0x1F05 || w == 0x1F06 || w == 0x1F18 || (w >= 0x1FF2 && w <= 0x20A5)) {
								*out++ = 0xF87E;
							}
							goto next_iteration;
						}
					}
				}
			}

			if (w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0xFD) {
			*out++ = 0xA9;
		} else if (c == 0xFE) {
			*out++ = 0x2122;
		} else if (c == 0xFF) {
			if ((limit - out) < 2) {
				p--;
				break;
			}
			*out++ = 0x2026;
			*out++ = 0xF87F;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
next_iteration: ;
	}

finished:
	*in_len = e - p;
	*in = p;
	return out - buf;
}

static bool process_s_form(uint32_t w, uint32_t w2, unsigned int *s)
{
	if (w2 == 0xF87A) {
		for (int i = 0; i < 4; i++) {
			if (w == s_form_tbl[i+34+3+3]) {
				*s = s_form_sjis_tbl[i+34+3+3];
				return true;
			}
		}
	} else if (w2 == 0x20DD) {
		for (int i = 0; i < 3; i++) {
			if (w == s_form_tbl[i+34+3]) {
				*s = s_form_sjis_tbl[i+34+3];
				return true;
			}
		}
	} else if (w2 == 0xF87F) {
		for (int i = 0; i < 3; i++) {
			if (w == s_form_tbl[i+34]) {
				*s = s_form_sjis_tbl[i+34];
				return true;
			}
		}
	} else if (w2 == 0xF87E) {
		for (int i = 0; i < 34; i++) {
			if (w == s_form_tbl[i]) {
				*s = s_form_sjis_tbl[i];
				return true;
			}
		}
	}

	return false;
}

/* For codepoints F860-F862, which are treated specially in MacJapanese */
static int transcoding_hint_cp_width[3] = { 3, 4, 5 };

static void mb_wchar_to_sjismac(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;

	if (buf->state) {
		w = buf->state & 0xFFFF;
		if (buf->state & 0xFF000000L) {
			goto resume_transcoding_hint;
		} else {
			buf->state = 0;
			goto process_codepoint;
		}
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			if (w == 0x5C) {
				s = 0x80;
			} else if (w == 0xA9) {
				s = 0xFD;
			} else {
				s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
			}
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			if (w == 0x2122) {
				s = 0xFE;
			} else if (w == 0x2014) {
				s = 0x213D;
			} else if (w == 0x2116) {
				s = 0x2C1D;
			} else {
				s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
			}
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (w >= 0x2000) {
			for (int i = 0; i < s_form_tbl_len; i++) {
				if (w == s_form_tbl[i]) {
					if (!len) {
						if (end) {
							s = s_form_sjis_fallback_tbl[i];
							if (s) {
								MB_CONVERT_BUF_ENSURE(buf, out, limit, 2);
								out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
							} else {
								MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
							}
						} else {
							buf->state = w;
						}
						MB_CONVERT_BUF_STORE(buf, out, limit);
						return;
					}
					uint32_t w2 = *in++;
					len--;

					if (!process_s_form(w, w2, &s)) {
						in--; len++;

						for (int i = 0; i < s_form_tbl_len; i++) {
							if (w == s_form_tbl[i]) {
								s = s_form_sjis_fallback_tbl[i];
								break;
							}
						}
					}

					if (s <= 0xFF) {
						out = mb_convert_buf_add(out, s);
					} else {
						MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
						out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
					}

					goto next_iteration;
				}
			}

			if (w == 0xF860 || w == 0xF861 || w == 0xF862) {
				/* Apple 'transcoding hint' codepoints (from private use area) */
				if (!len) {
					if (end) {
						MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
					} else {
						buf->state = w;
					}
					MB_CONVERT_BUF_STORE(buf, out, limit);
					return;
				}

				uint32_t w2 = *in++;
				len--;

				for (int i = 0; i < code_tbl_m_len; i++) {
					if (w == code_tbl_m[i][1] && w2 == code_tbl_m[i][2]) {
						/* This might be a valid transcoding hint sequence */
						int index = 3;

						if (buf->state) {
resume_transcoding_hint:
							i = buf->state >> 24;
							index = (buf->state >> 16) & 0xFF;
							buf->state = 0;
						}

						int expected = transcoding_hint_cp_width[w - 0xF860];

						while (index <= expected) {
							if (!len) {
								if (end) {
									for (int j = 1; j < index; j++) {
										MB_CONVERT_ERROR(buf, out, limit, code_tbl_m[i][j], mb_wchar_to_sjismac);
									}
								} else {
									buf->state = (i << 24) | (index << 16) | (w & 0xFFFF);
								}
								MB_CONVERT_BUF_STORE(buf, out, limit);
								return;
							}

							w2 = *in++;
							len--;

							if (w2 != code_tbl_m[i][index]) {
								/* Didn't match */
								for (int j = 1; j < index; j++) {
									MB_CONVERT_ERROR(buf, out, limit, code_tbl_m[i][j], mb_wchar_to_sjismac);
								}
								MB_CONVERT_ERROR(buf, out, limit, w2, mb_wchar_to_sjismac);
								MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
								goto next_iteration;
							}

							index++;
						}

						/* Successful match, emit SJIS-mac bytes */
						s = code_tbl_m[i][0];
						unsigned int c1 = (s / 94) + 0x21, c2 = (s % 94) + 0x21, s1, s2;
						SJIS_ENCODE(c1, c2, s1, s2);
						MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
						out = mb_convert_buf_add2(out, s1, s2);
						goto next_iteration;
					}
				}

				/* No valid transcoding hint sequence found */
				in--; len++;
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				continue;
			}
		}

		if (!s) {
			if (w == 0xA0) {
				s = 0xA0;
			} else if (w == 0xA5) { /* YEN SIGN */
				/* Unicode has codepoint 0xFFE5 for a fullwidth Yen sign;
				 * convert codepoint 0xA5 to halfwidth Yen sign */
				s = 0x5C; /* HALFWIDTH YEN SIGN */
			} else if (w == 0xFF3C) {	/* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else {
				for (int i = 0; i < wchar2sjis_mac_r_tbl_len; i++) {
					if (w >= wchar2sjis_mac_r_tbl[i][0] && w <= wchar2sjis_mac_r_tbl[i][1]) {
						s = w - wchar2sjis_mac_r_tbl[i][0] + wchar2sjis_mac_r_tbl[i][2];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto found_kuten_code;
					}
				}

				for (int i = 0; i < wchar2sjis_mac_r_map_len; i++) {
					if (w >= wchar2sjis_mac_r_map[i][0] && w <= wchar2sjis_mac_r_map[i][1]) {
						s = wchar2sjis_mac_code_map[i][w - wchar2sjis_mac_r_map[i][0]];
						if (s) {
							s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
							goto found_kuten_code;
						}
					}
				}

				for (int i = 0; i < wchar2sjis_mac_wchar_tbl_len; i++) {
					if (w == wchar2sjis_mac_wchar_tbl[i][0]) {
						s = wchar2sjis_mac_wchar_tbl[i][1];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto found_kuten_code;
					}
				}
			}
		}

found_kuten_code:
		if ((!s && w) || s >= 0x8080) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}

next_iteration: ;
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

int mbfilter_sjis_emoji_docomo2unicode(int s, int *snd)
{
	/* All three mobile vendors had emoji for numbers on a telephone keypad
	 * Unicode doesn't have those, but it has a combining character which puts
	 * a 'keypad button' around the following character, making it look like
	 * a key on a telephone or keyboard. That combining char is codepoint 0x20E3. */
	if (s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
		if ((s >= DOCOMO_KEYPAD(1) && s <= DOCOMO_KEYPAD(9)) || s == DOCOMO_KEYPAD(0) || s == DOCOMO_KEYPAD_HASH) {
			EMIT_KEYPAD_EMOJI(convert_emoji_cp(mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min]));
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min]);
		}
	}
	return 0;
}

int mbfilter_sjis_emoji_sb2unicode(int s, int *snd)
{
	if (s >= mb_tbl_code2uni_sb1_min && s <= mb_tbl_code2uni_sb1_max) {
		if (s == 0x2817 || (s >= 0x2823 && s <= 0x282C)) {
			EMIT_KEYPAD_EMOJI(mb_tbl_code2uni_sb1[s - mb_tbl_code2uni_sb1_min]);
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_sb1[s - mb_tbl_code2uni_sb1_min]);
		}
	} else if (s >= mb_tbl_code2uni_sb2_min && s <= mb_tbl_code2uni_sb2_max) {
		*snd = 0;
		return convert_emoji_cp(mb_tbl_code2uni_sb2[s - mb_tbl_code2uni_sb2_min]);
	} else if (s >= mb_tbl_code2uni_sb3_min && s <= mb_tbl_code2uni_sb3_max) {
		if (s >= 0x2B02 && s <= 0x2B0B) {
			EMIT_FLAG_EMOJI(nflags_sb[s - 0x2B02]);
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_sb3[s - mb_tbl_code2uni_sb3_min]);
		}
	}
	return 0;
}

int mbfilter_unicode2sjis_emoji_docomo(int c, int *s1, mbfl_convert_filter *filter)
{
	/* When converting SJIS-Mobile to Unicode, we convert keypad symbol emoji
	 * to a sequence of 2 codepoints, one of which is a combining character which
	 * adds the 'key' image around the other
	 *
	 * In the other direction, look for such sequences and convert them to a
	 * single emoji */
	if (filter->status == 1) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x2964;
			} else if (c1 == '0') {
				*s1 = 0x296F;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x2966 + (c1 - '1');
			}
			return 1;
		} else {
			/* This character wasn't combining character to make keypad symbol,
			 * so pass the previous character through... and proceed to process the
			 * current character as usual
			 * (Single-byte ASCII characters are valid in Shift-JIS...) */
			CK((*filter->output_function)(c1, filter->data));
		}
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status = 1;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x29B5;
		return 1;
	} else if (c == 0x00AE) { /* Registered sign */
		*s1 = 0x29BA;
		return 1;
	} else if (c >= mb_tbl_uni_docomo2code2_min && c <= mb_tbl_uni_docomo2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_docomo2code2_key, mb_tbl_uni_docomo2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_docomo2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_docomo2code3_min && c <= mb_tbl_uni_docomo2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_docomo2code3_key, mb_tbl_uni_docomo2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_docomo2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_docomo2code5_min && c <= mb_tbl_uni_docomo2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_docomo2code5_key, mb_tbl_uni_docomo2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_docomo2code5_val[i];
			return 1;
		}
	}
	return 0;
}

int mbfilter_unicode2sjis_emoji_kddi_sjis(int c, int *s1, mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x25BC;
			} else if (c1 == '0') {
				*s1 = 0x2830;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x27a6 + (c1 - '1');
			}
			return 1;
		} else {
			CK((*filter->output_function)(c1, filter->data));
		}
	} else if (filter->status == 2) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c >= NFLAGS('B') && c <= NFLAGS('U')) { /* B for GB, U for RU */
			for (int i = 0; i < 10; i++) {
				if (c1 == NFLAGS(nflags_s[i][0]) && c == NFLAGS(nflags_s[i][1])) {
					*s1 = nflags_code_kddi[i];
					return 1;
				}
			}
		}

		/* If none of the KDDI national flag emoji matched, then we have no way
		 * to convert the previous codepoint... */
		mbfl_filt_conv_illegal_output(c1, filter);
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status = 1;
		filter->cache = c;
		return 0;
	} else if (c >= NFLAGS('C') && c <= NFLAGS('U')) { /* C for CN, U for US */
		filter->status = 2;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x27DC;
		return 1;
	} else if (c == 0xAE) { /* Registered sign */
		*s1 = 0x27DD;
		return 1;
	} else if (c >= mb_tbl_uni_kddi2code2_min && c <= mb_tbl_uni_kddi2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code3_min && c <= mb_tbl_uni_kddi2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code5_min && c <= mb_tbl_uni_kddi2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code5_val[i];
			return 1;
		}
	}
	return 0;
}

int mbfilter_unicode2sjis_emoji_sb(int c, int *s1, mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x2817;
			} else if (c1 == '0') {
				*s1 = 0x282c;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x2823 + (c1 - '1');
			}
			return 1;
		} else {
			(*filter->output_function)(c1, filter->data);
		}
	} else if (filter->status == 2) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c >= NFLAGS('B') && c <= NFLAGS('U')) { /* B for GB, U for RU */
			for (int i = 0; i < 10; i++) {
				if (c1 == NFLAGS(nflags_s[i][0]) && c == NFLAGS(nflags_s[i][1])) {
					*s1 = nflags_code_sb[i];
					return 1;
				}
			}
		}

		/* If none of the SoftBank national flag emoji matched, then we have no way
		 * to convert the previous codepoint... */
		mbfl_filt_conv_illegal_output(c1, filter);
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status = 1;
		filter->cache = c;
		return 0;
	} else if (c >= NFLAGS('C') && c <= NFLAGS('U')) { /* C for CN, U for US */
		filter->status = 2;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x2855;
		return 1;
	} else if (c == 0xAE) { /* Registered sign */
		*s1 = 0x2856;
		return 1;
	} else if (c >= mb_tbl_uni_sb2code2_min && c <= mb_tbl_uni_sb2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_sb2code2_key, mb_tbl_uni_sb2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_sb2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_sb2code3_min && c <= mb_tbl_uni_sb2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_sb2code3_key, mb_tbl_uni_sb2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_sb2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_sb2code5_min && c <= mb_tbl_uni_sb2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_sb2code5_key, mb_tbl_uni_sb2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_sb2code5_val[i];
			return 1;
		}
	}
	return 0;
}

static int mbfl_filt_conv_sjis_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, s1, s2, w, snd = 0;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* ASCII */
			if (filter->from == &mbfl_encoding_sjis_sb && c == 0x1B) {
				/* ESC; escape sequences were used on older SoftBank phones for emoji */
				filter->cache = c;
				filter->status = 2;
			} else {
				CK((*filter->output_function)(c, filter->data));
			}
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else if (c > 0x80 && c < 0xFD && c != 0xA0) { /* Kanji, first byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* Kanji, second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0x40 && c <= 0xFC && c != 0x7F) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = ((s1 - 0x21) * 94) + s2 - 0x21;
			if (s <= 137) {
				if (s == 31) {
					w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xFF5E; /* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225; /* PARALLEL TO */
				} else if (s == 60) {
					w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xFFE0; /* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xFFE1; /* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xFFE2; /* FULLWIDTH NOT SIGN */
				}
			}
			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				}

				/* Emoji */
				if (filter->from == &mbfl_encoding_sjis_docomo && s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
					w = mbfilter_sjis_emoji_docomo2unicode(s, &snd);
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				} else if (filter->from == &mbfl_encoding_sjis_kddi && s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi2_max) {
					w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				} else if (filter->from == &mbfl_encoding_sjis_sb && s >= mb_tbl_code2uni_sb1_min && s <= mb_tbl_code2uni_sb3_max) {
					w = mbfilter_sjis_emoji_sb2unicode(s, &snd);
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				}

				if (w == 0) {
					if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) { /* vendor ext3 (115ku - 119ku) */
						w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
					} else if (s >= (94*94) && s < (114*94)) { /* user (95ku - 114ku) */
						w = s - (94*94) + 0xe000;
					}
				}
			}
			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC: Softbank Emoji */
	case 2:
		if (c == '$') {
			filter->cache = c;
			filter->status++;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			filter->status = filter->cache = 0;
		}
		break;

	/* ESC $: Softbank Emoji */
	case 3:
		if ((c >= 'E' && c <= 'G') || (c >= 'O' && c <= 'Q')) {
			filter->cache = c;
			filter->status++;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			filter->status = filter->cache = 0;
		}
		break;

	/* ESC $ [GEFOPQ]: Softbank Emoji */
	case 4:
		c1 = filter->cache;
		if (c == 0xF) { /* Terminate sequence of emoji */
			filter->status = filter->cache = 0;
			return 0;
		} else {
			if (c1 == 'G' && c >= 0x21 && c <= 0x7a) {
				s1 = (0x91 - 0x21) * 94;
			} else if (c1 == 'E' && c >= 0x21 && c <= 0x7A) {
				s1 = (0x8D - 0x21) * 94;
			} else if (c1 == 'F' && c >= 0x21 && c <= 0x7A) {
				s1 = (0x8E - 0x21) * 94;
			} else if (c1 == 'O' && c >= 0x21 && c <= 0x6D) {
				s1 = (0x92 - 0x21) * 94;
			} else if (c1 == 'P' && c >= 0x21 && c <= 0x6C) {
				s1 = (0x95 - 0x21) * 94;
			} else if (c1 == 'Q' && c >= 0x21 && c <= 0x5E) {
				s1 = (0x96 - 0x21) * 94;
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->status = filter->cache = 0;
				return 0;
			}

			w = mbfilter_sjis_emoji_sb2unicode(s1 + c - 0x21, &snd);
			if (w > 0) {
				if (snd > 0) {
					CK((*filter->output_function)(snd, filter->data));
				}
				CK((*filter->output_function)(w, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->status = filter->cache = 0;
			}
		}
	}

	return 0;
}

static int mbfl_filt_conv_wchar_sjis_mobile(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0, s2 = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c < (0xE000 + 20*94)) {
		/* Private User Area (95ku - 114ku) */
		s1 = c - 0xE000;
		c1 = (s1 / 94) + 0x7F;
		c2 = (s1 % 94) + 0x21;
		s1 = (c1 << 8) | c2;
		s2 = 1;
	}

	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xFF3c) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		}
	}

	if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
		s1 = -1;

		/* CP932 vendor ext1 (13ku) */
		for (c1 = 0; c1 < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; c1++) {
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = (((c1 / 94) + 0x2D) << 8) + (c1 % 94) + 0x21;
				break;
			}
		}

		if (s1 <= 0) {
			/* CP932 vendor ext2 (115ku - 119ku) */
			for (c1 = 0; c1 < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; c1++) {
				if (c == cp932ext2_ucs_table[c1]) {
					s1 = (((c1 / 94) + 0x79) << 8) + (c1 % 94) + 0x21;
					break;
				}
			}
		}

		if (c == 0) {
			s1 = 0;
		}
	}

	if ((filter->to == &mbfl_encoding_sjis_docomo && mbfilter_unicode2sjis_emoji_docomo(c, &s1, filter)) ||
		  (filter->to == &mbfl_encoding_sjis_kddi   && mbfilter_unicode2sjis_emoji_kddi_sjis(c, &s1, filter)) ||
		  (filter->to == &mbfl_encoding_sjis_sb     && mbfilter_unicode2sjis_emoji_sb(c, &s1, filter))) {
		s1 = (((s1 / 94) + 0x21) << 8) | ((s1 % 94) + 0x21);
	}

	if (filter->status) {
		return 0;
	}

	if (s1 >= 0) {
		if (s1 < 0x100) { /* Latin/Kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* Kanji */
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_sjis_mobile_flush(mbfl_convert_filter *filter)
{
	int c1 = filter->cache;
	if (filter->status == 1 && (c1 == '#' || (c1 >= '0' && c1 <= '9'))) {
		filter->cache = filter->status = 0;
		CK((*filter->output_function)(c1, filter->data));
	} else if (filter->status == 2) {
		/* First of a pair of Regional Indicator codepoints came at the end of a string */
		filter->cache = filter->status = 0;
		mbfl_filt_conv_illegal_output(c1, filter);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static const unsigned short sjis_mobile_decode_tbl1[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFFFF, 0, 188, 376, 564, 752, 940, 1128, 1316, 1504, 1692, 1880, 2068, 2256, 2444, 2632, 2820, 3008, 3196, 3384, 3572, 3760, 3948, 4136, 4324, 4512, 4700, 4888, 5076, 5264, 5452, 5640, 0xFFFF, -6016, -5828, -5640, -5452, -5264, -5076, -4888, -4700, -4512, -4324, -4136, -3948, -3760, -3572, -3384, -3196, -3008, -2820, -2632, -2444, -2256, -2068, -1880, -1692, -1504, -1316, -1128, -940, -752, -564, -376, -188, 0, 188, 376, 564, 752, 940, 1128, 1316, 1504, 1692, 1880, 2068, 2256, 2444, 2632, 2820, 3008, 3196, 3384, 3572, 3760, 3948, 4136, 4324, 4512, 4700, 4888, 5076, 5264, 5452, 5640, 5828, 6016, 6204, 6392, 6580, 6768, 6956, 7144, 7332, 7520, 7708, 7896, 8084, 8272, 8460, 8648, 8836, 9024, 9212, 9400, 9588, 9776, 9964, 10152, 10340, 10528, 10716, 10904, 11092, 0xFFFF, 0xFFFF, 0xFFFF
};

static size_t mb_sjis_docomo_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	/* Leave one extra space available in output buffer, since some iterations of
	 * main loop (below) may emit two wchars */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			uint32_t w = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];

			if (w <= 137) {
				if (w == 31) {
					*out++ = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					continue;
				} else if (w == 32) {
					*out++ =  0xFF5E; /* FULLWIDTH TILDE */
					continue;
				} else if (w == 33) {
					*out++ =  0x2225; /* PARALLEL TO */
					continue;
				} else if (w == 60) {
					*out++ =  0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					continue;
				} else if (w == 80) {
					*out++ =  0xFFE0; /* FULLWIDTH CENT SIGN */
					continue;
				} else if (w == 81) {
					*out++ =  0xFFE1; /* FULLWIDTH POUND SIGN */
					continue;
				} else if (w == 137) {
					*out++ =  0xFFE2; /* FULLWIDTH NOT SIGN */
					continue;
				}
			}

			if (w >= mb_tbl_code2uni_docomo1_min && w <= mb_tbl_code2uni_docomo1_max) {
				int snd = 0;
				w = mbfilter_sjis_emoji_docomo2unicode(w, &snd);
				if (snd) {
					*out++ = snd;
				}
			} else if (w >= cp932ext1_ucs_table_min && w < cp932ext1_ucs_table_max) {
				w = cp932ext1_ucs_table[w - cp932ext1_ucs_table_min];
			} else if (w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
			} else if (w >= cp932ext2_ucs_table_min && w < cp932ext2_ucs_table_max) {
				w = cp932ext2_ucs_table[w - cp932ext2_ucs_table_min];
			} else if (w >= cp932ext3_ucs_table_min && w < cp932ext3_ucs_table_max) {
				w = cp932ext3_ucs_table[w - cp932ext3_ucs_table_min];
			} else if (w >= (94*94) && w < (114*94)) {
				w = w - (94*94) + 0xE000;
			} else {
				if (c == 0x80 || c == 0xA0 || c >= 0xFD) {
					p--;
				}
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			*out++ = w ? w : MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_sjis_docomo(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len + (buf->state ? 1 : 0));

	uint32_t w;
	unsigned int s = 0;

	if (buf->state) {
		/* Continue what we were doing on the previous call */
		w = buf->state;
		buf->state = 0;
		goto reprocess_wchar;
	}

	while (len--) {
		w = *in++;
reprocess_wchar:
		s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = w - 0xE000;
			s = (((s / 94) + 0x7F) << 8) | ((s % 94) + 0x21);
			goto process_emoji;
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (w && (!s || s >= 0x8080)) {
			s = 0;

			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}

			for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
				if (w == cp932ext2_ucs_table[i]) {
					s = (((i / 94) + 0x79) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}
		}

process_emoji:
		/* When converting SJIS-Mobile to Unicode, we convert keypad symbol emoji
		 * to a sequence of 2 codepoints, one of which is a combining character which
		 * adds the 'key' image around the other
		 *
		 * In the other direction, look for such sequences and convert them to a
		 * single emoji */
		if (w == '#' || (w >= '0' && w <= '9')) {
			if (!len) {
				if (end) {
					goto emit_output;
				} else {
					/* If we are at the end of the current buffer of codepoints, but another
					 * buffer is coming, then remember that we have to reprocess `w` */
					buf->state = w;
					break;
				}
			}
			uint32_t w2 = *in++; len--;
			if (w2 == 0x20E3) {
				if (w == '#') {
					s = 0x2964;
				} else if (w == '0') {
					s = 0x296F;
				} else { /* Previous character was '1'-'9' */
					s = 0x2966 + (w - '1');
				}
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			} else {
				in--; len++;
			}
		} else if (w == 0xA9) { /* Copyright sign */
			s = (((0x29B5 / 94) + 0x21) << 8) | ((0x29B5 % 94) + 0x21);
		} else if (w == 0xAE) { /* Registered sign */
			s = (((0x29BA / 94) + 0x21) << 8) | ((0x29BA % 94) + 0x21);
		} else if (w >= mb_tbl_uni_docomo2code2_min && w <= mb_tbl_uni_docomo2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_docomo2code2_key, mb_tbl_uni_docomo2code2_len);
			if (i >= 0) {
				s = mb_tbl_uni_docomo2code2_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_docomo2code3_min && w <= mb_tbl_uni_docomo2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_docomo2code3_key, mb_tbl_uni_docomo2code3_len);
			if (i >= 0) {
				s = mb_tbl_uni_docomo2code3_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_docomo2code5_min && w <= mb_tbl_uni_docomo2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_docomo2code5_key, mb_tbl_uni_docomo2code5_len);
			if (i >= 0) {
				s = mb_tbl_uni_docomo2code5_val[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		}

emit_output:
		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_docomo);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_sjis_kddi_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			uint32_t w = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];

			if (w <= 137) {
				if (w == 31) {
					*out++ = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					continue;
				} else if (w == 32) {
					*out++ = 0xFF5E; /* FULLWIDTH TILDE */
					continue;
				} else if (w == 33) {
					*out++ = 0x2225; /* PARALLEL TO */
					continue;
				} else if (w == 60) {
					*out++ = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					continue;
				} else if (w == 80) {
					*out++ = 0xFFE0; /* FULLWIDTH CENT SIGN */
					continue;
				} else if (w == 81) {
					*out++ = 0xFFE1; /* FULLWIDTH POUND SIGN */
					continue;
				} else if (w == 137) {
					*out++ = 0xFFE2; /* FULLWIDTH NOT SIGN */
					continue;
				}
			}

			if (w >= mb_tbl_code2uni_kddi1_min && w <= mb_tbl_code2uni_kddi2_max) {
				int snd = 0;
				w = mbfilter_sjis_emoji_kddi2unicode(w, &snd);
				if (!w) {
					w = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];
					if (w >= (94*94) && w < (114*94)) {
						w = w - (94*94) + 0xE000;
					}
				} else if (snd) {
					*out++ = snd;
				}
			} else if (w >= cp932ext1_ucs_table_min && w < cp932ext1_ucs_table_max) {
				w = cp932ext1_ucs_table[w - cp932ext1_ucs_table_min];
			} else if (w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
			} else if (w >= cp932ext2_ucs_table_min && w < cp932ext2_ucs_table_max) {
				w = cp932ext2_ucs_table[w - cp932ext2_ucs_table_min];
			} else if (w >= cp932ext3_ucs_table_min && w < cp932ext3_ucs_table_max) {
				w = cp932ext3_ucs_table[w - cp932ext3_ucs_table_min];
			} else if (w >= (94*94) && w < (114*94)) {
				w = w - (94*94) + 0xE000;
			} else {
				if (c == 0x80 || c == 0xA0 || c >= 0xFD) {
					p--;
				}
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			*out++ = w ? w : MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_sjis_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len + (buf->state ? 1 : 0));

	uint32_t w;
	unsigned int s = 0;

	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		goto reprocess_wchar;
	}

	while (len--) {
		w = *in++;
reprocess_wchar:
		s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = w - 0xE000;
			s = (((s / 94) + 0x7F) << 8) | ((s % 94) + 0x21);
			goto process_emoji;
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3c) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (w && (!s || s >= 0x8080)) {
			s = 0;

			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}

			for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
				if (w == cp932ext2_ucs_table[i]) {
					s = (((i / 94) + 0x79) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}
		}

process_emoji:
		if (w == '#' || (w >= '0' && w <= '9')) {
			if (!len) {
				if (end) {
					goto emit_output;
				} else {
					/* If we are at the end of the current buffer of codepoints, but another
					 * buffer is coming, then remember that we have to reprocess `w` */
					buf->state = w;
					break;
				}
			}
			uint32_t w2 = *in++; len--;
			if (w2 == 0x20E3) {
				if (w == '#') {
					s = 0x25BC;
				} else if (w == '0') {
					s = 0x2830;
				} else { /* Previous character was '1'-'9' */
					s = 0x27A6 + (w - '1');
				}
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			} else {
				in--; len++;
			}
		} else if (w >= NFLAGS('C') && w <= NFLAGS('U')) { /* C for CN, U for US */
			if (!len) {
				if (end) {
					MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_kddi);
				} else {
					/* Reprocess `w` when this function is called again with another buffer
					 * of wchars */
					buf->state = w;
				}
				break;
			}
			uint32_t w2 = *in++; len--;
			if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
				for (int i = 0; i < 10; i++) {
					if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
						s = nflags_code_kddi[i];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto emit_output;
					}
				}
			}
			in--; len++;
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			continue;
		} else if (w == 0xA9) { /* Copyright sign */
			s = (((0x27DC / 94) + 0x21) << 8) | ((0x27DC % 94) + 0x21);
		} else if (w == 0xAE) { /* Registered sign */
			s = (((0x27DD / 94) + 0x21) << 8) | ((0x27DD % 94) + 0x21);
		} else if (w >= mb_tbl_uni_kddi2code2_min && w <= mb_tbl_uni_kddi2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
			if (i >= 0) {
				s = mb_tbl_uni_kddi2code2_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_kddi2code3_min && w <= mb_tbl_uni_kddi2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
			if (i >= 0) {
				s = mb_tbl_uni_kddi2code3_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_kddi2code5_min && w <= mb_tbl_uni_kddi2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
			if (i >= 0) {
				s = mb_tbl_uni_kddi2code5_val[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		}

emit_output:
		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_sjis_sb_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	if (*state) {
		goto softbank_emoji_escapes;
	}

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			/* Escape sequence */
			if (p == e || *p++ != '$' || p == e) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}
			unsigned char c2 = *p++;
			if ((c2 < 'E' || c2 > 'G') && (c2 < 'O' || c2 > 'Q')) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}
			/* Escape sequence was valid, next should be a series of specially
			 * encoded Softbank emoji */
			*state = c2;

softbank_emoji_escapes:
			while (p < e && out < limit) {
				c = *p++;
				if (c == 0xF) {
					*state = 0;
					break;
				}
				unsigned int s = 0;
				if (*state == 'G' && c >= 0x21 && c <= 0x7A) {
					s = (0x91 - 0x21) * 94;
				} else if (*state == 'E' && c >= 0x21 && c <= 0x7A) {
					s = (0x8D - 0x21) * 94;
				} else if (*state == 'F' && c >= 0x21 && c <= 0x7A) {
					s = (0x8E - 0x21) * 94;
				} else if (*state == 'O' && c >= 0x21 && c <= 0x6D) {
					s = (0x92 - 0x21) * 94;
				} else if (*state == 'P' && c >= 0x21 && c <= 0x6C) {
					s = (0x95 - 0x21) * 94;
				} else if (*state == 'Q' && c >= 0x21 && c <= 0x5E) {
					s = (0x96 - 0x21) * 94;
				} else {
					*out++ = MBFL_BAD_INPUT;
					*state = 0;
					break;
				}

				int snd = 0;
				uint32_t w = mbfilter_sjis_emoji_sb2unicode(s + c - 0x21, &snd);
				if (w) {
					if (snd) {
						*out++ = snd;
					}
					*out++ = w;
				} else {
					*out++ = MBFL_BAD_INPUT;
					*state = 0;
					break;
				}
			}
		} else if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			uint32_t w = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];

			if (w <= 137) {
				if (w == 31) {
					*out++ = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					continue;
				} else if (w == 32) {
					*out++ = 0xFF5E; /* FULLWIDTH TILDE */
					continue;
				} else if (w == 33) {
					*out++ = 0x2225; /* PARALLEL TO */
					continue;
				} else if (w == 60) {
					*out++ = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					continue;
				} else if (w == 80) {
					*out++ = 0xFFE0; /* FULLWIDTH CENT SIGN */
					continue;
				} else if (w == 81) {
					*out++ = 0xFFE1; /* FULLWIDTH POUND SIGN */
					continue;
				} else if (w == 137) {
					*out++ = 0xFFE2; /* FULLWIDTH NOT SIGN */
					continue;
				}
			}

			if (w >= mb_tbl_code2uni_sb1_min && w <= mb_tbl_code2uni_sb3_max) {
				int snd = 0;
				w = mbfilter_sjis_emoji_sb2unicode(w, &snd);
				if (!w) {
					w = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];
					if (w >= cp932ext3_ucs_table_min && w < cp932ext3_ucs_table_max) {
						w = cp932ext3_ucs_table[w - cp932ext3_ucs_table_min];
					} else if (w >= (94*94) && w < (114*94)) {
						w = w - (94*94) + 0xE000;
					}
				} else if (snd) {
					*out++ = snd;
				}
			} else if (w >= cp932ext1_ucs_table_min && w < cp932ext1_ucs_table_max) {
				w = cp932ext1_ucs_table[w - cp932ext1_ucs_table_min];
			} else if (w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
			} else if (w >= cp932ext2_ucs_table_min && w < cp932ext2_ucs_table_max) {
				w = cp932ext2_ucs_table[w - cp932ext2_ucs_table_min];
			} else if (w >= cp932ext3_ucs_table_min && w < cp932ext3_ucs_table_max) {
				w = cp932ext3_ucs_table[w - cp932ext3_ucs_table_min];
			} else if (w >= (94*94) && w < (114*94)) {
				w = w - (94*94) + 0xE000;
			} else {
				if (c == 0x80 || c == 0xA0 || c >= 0xFD) {
					p--;
				}
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			*out++ = w ? w : MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_sjis_sb(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len + (buf->state ? 1 : 0));

	uint32_t w;
	unsigned int s = 0;

	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		goto reprocess_wchar;
	}

	while (len--) {
		w = *in++;
reprocess_wchar:
		s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = w - 0xE000;
			s = (((s / 94) + 0x7F) << 8) | ((s % 94) + 0x21);
			goto process_emoji;
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (w && (!s || s >= 0x8080)) {
			s = 0;

			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}

			for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
				if (w == cp932ext2_ucs_table[i]) {
					s = (((i / 94) + 0x79) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}
		}

process_emoji:
		if (w == '#' || (w >= '0' && w <= '9')) {
			if (!len) {
				if (end) {
					goto emit_output;
				} else {
					/* If we are at the end of the current buffer of codepoints, but another
					 * buffer is coming, then remember that we have to reprocess `w` */
					buf->state = w;
					break;
				}
			}
			uint32_t w2 = *in++; len--;
			if (w2 == 0x20E3) {
				if (w == '#') {
					s = 0x2817;
				} else if (w == '0') {
					s = 0x282c;
				} else { /* Previous character was '1'-'9' */
					s = 0x2823 + (w - '1');
				}
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			} else {
				in--; len++;
			}
		} else if (w >= NFLAGS('C') && w <= NFLAGS('U')) { /* C for CN, U for US */
			if (!len) {
				if (end) {
					MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_sb);
				} else {
					/* Reprocess `w` when this function is called again with
					 * another buffer of wchars */
					buf->state = w;
				}
				break;
			}
			uint32_t w2 = *in++; len--;
			if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
				for (int i = 0; i < 10; i++) {
					if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
						s = nflags_code_sb[i];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto emit_output;
					}
				}
			}
			in--; len++;
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_sb);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			continue;
		} else if (w == 0xA9) { /* Copyright sign */
			s = (((0x2855 / 94) + 0x21) << 8) | ((0x2855 % 94) + 0x21);
		} else if (w == 0xAE) { /* Registered sign */
			s = (((0x2856 / 94) + 0x21) << 8) | ((0x2856 % 94) + 0x21);
		} else if (w >= mb_tbl_uni_sb2code2_min && w <= mb_tbl_uni_sb2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_sb2code2_key, mb_tbl_uni_sb2code2_len);
			if (i >= 0) {
				s = mb_tbl_uni_sb2code2_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_sb2code3_min && w <= mb_tbl_uni_sb2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_sb2code3_key, mb_tbl_uni_sb2code3_len);
			if (i >= 0) {
				s = mb_tbl_uni_sb2code3_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_sb2code5_min && w <= mb_tbl_uni_sb2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_sb2code5_key, mb_tbl_uni_sb2code5_len);
			if (i >= 0) {
				s = mb_tbl_uni_sb2code5_val[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		}

emit_output:
		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_sb);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_sjis2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			if (c == 0x5C) {
				*out++ = 0xA5;
			} else if (c == 0x7E) {
				*out++ = 0x203E;
			} else {
				*out++ = c;
			}
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			uint32_t w1 = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];

			/* Conversion for combining characters */
			if (w1 >= 0x0170 && w1 <= 0x03F1) {
				int k = mbfl_bisec_srch2(w1, jisx0213_u2_key_b, jisx0213_u2_tbl_len);
				if (k >= 0) {
					*out++ = jisx0213_u2_tbl[2*k];
					*out++ = jisx0213_u2_tbl[2*k+1];
					continue;
				}
			}

			/* Conversion for BMP */
			if (w1 < jisx0213_ucs_table_size) {
				uint32_t w = jisx0213_ucs_table[w1];
				if (w) {
					*out++ = w;
					continue;
				}
			}

			/* Conversion for CJK Unified Ideographs extension B (U+2XXXX) */
			int k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				*out++ = jisx0213_jis_u5_tbl[k] + 0x20000;
			} else {
				if (c == 0x80 || c == 0xA0 || c >= 0xFD) {
					p--;
				}
				*out++ = MBFL_BAD_INPUT;
			}
		}
	}

	 *in_len = e - p;
	 *in = p;
	 return out - buf;
}

static void mb_wchar_to_sjis2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;
	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		goto process_codepoint;
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		if (w == 0xE6 || (w >= 0x254 && w <= 0x2E9) || (w >= 0x304B && w <= 0x3053) || (w >= 0x30AB && w <= 0x30C8) || w == 0x31F7) {
			for (int k = 0; k < jisx0213_u2_tbl_len; k++) {
				if (w == jisx0213_u2_tbl[2*k]) {
					if (!len) {
						if (!end) {
							buf->state = w;
							MB_CONVERT_BUF_STORE(buf, out, limit);
							return;
						}
					} else {
						uint32_t w2 = *in++; len--;
						if ((w == 0x254 || w == 0x28C || w == 0x259 || w == 0x25A) && w2 == 0x301) {
							k++;
						}
						if (w2 == jisx0213_u2_tbl[2*k+1]) {
							s = jisx0213_u2_key[k];
							break;
						}
						in--; len++;
					}

					/* Fallback */
					s = jisx0213_u2_fb_tbl[k];
					break;
				}
			}
		}

		/* Check for major Japanese chars: U+4E00-U+9FFF */
		if (!s) {
			for (int k = 0; k < uni2jis_tbl_len; k++) {
				if (w >= uni2jis_tbl_range[k][0] && w <= uni2jis_tbl_range[k][1]) {
					s = uni2jis_tbl[k][w - uni2jis_tbl_range[k][0]];
					break;
				}
			}
		}

		/* Check for Japanese chars in compressed mapping area: U+1E00-U+4DBF */
		if (!s && w >= ucs_c1_jisx0213_min && w <= ucs_c1_jisx0213_max) {
			int k = mbfl_bisec_srch(w, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
			if (k >= 0) {
				s = ucs_c1_jisx0213_ofst[k] + w - ucs_c1_jisx0213_tbl[2*k];
			}
		}

		/* Check for Japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
		if (!s && w >= jisx0213_u5_tbl_min && w <= jisx0213_u5_tbl_max) {
			int k = mbfl_bisec_srch2(w - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				s = jisx0213_u5_jis_tbl[k];
			}
		}

		if (!s) {
			/* CJK Compatibility Forms: U+FE30-U+FE4F */
			if (w == 0xFE45) {
				s = 0x233E;
			} else if (w == 0xFE46) {
				s = 0x233D;
			} else if (w >= 0xF91D && w <= 0xF9DC) {
				/* CJK Compatibility Ideographs: U+F900-U+F92A */
				int k = mbfl_bisec_srch2(w, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
				if (k >= 0) {
					s = ucs_r2b_jisx0213_cmap_val[k];
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_cp932_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:		/* kanji second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = (s1 - 0x21)*94 + s2 - 0x21;
			if (s <= 137) {
				if (s == 31) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xff5e;			/* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225;			/* PARALLEL TO */
				} else if (s == 60) {
					w = 0xff0d;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xffe0;			/* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xffe1;			/* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xffe2;			/* FULLWIDTH NOT SIGN */
				}
			}
			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {		/* vendor ext3 (115ku - 119ku) */
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= (94*94) && s < (114*94)) {		/* user (95ku - 114ku) */
					w = s - (94*94) + 0xe000;
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp932_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp932(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1, s2;

	s1 = 0;
	s2 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) {
		s1 = 0x7E;
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 20*94)) {	/* user  (95ku - 114ku) */
		s1 = c - 0xe000;
		c1 = s1/94 + 0x7f;
		c2 = s1%94 + 0x21;
		s1 = (c1 << 8) | c2;
		s2 = 1;
	}
	if (s1 <= 0) {
		if (c == 0xa5) { /* YEN SIGN */
			s1 = 0x5C;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		}
	}
	if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
		s1 = -1;
		c1 = 0;
		c2 = cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
		while (c1 < c2) {		/* CP932 vendor ext1 (13ku) */
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = ((c1/94 + 0x2d) << 8) + (c1%94 + 0x21);
				break;
			}
			c1++;
		}
		if (s1 <= 0) {
			c1 = 0;
			c2 = cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
				if (c == cp932ext3_ucs_table[c1]) {
					s1 = ((c1/94 + 0x93) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}
	if (s1 >= 0) {
		if (s1 < 0x100) { /* latin or kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* kanji */
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_sjiswin(int c, mbfl_convert_filter *filter)
{
	if (c == 0xA5) {
		CK((*filter->output_function)(0x81, filter->data));
		CK((*filter->output_function)(0x8F, filter->data));
	} else if (c == 0x203E) {
		CK((*filter->output_function)(0x81, filter->data));
		CK((*filter->output_function)(0x50, filter->data));
	} else {
		return mbfl_filt_conv_wchar_cp932(c, filter);
	}
	return 0;
}

static size_t mb_cp932_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c > 0xA0 && c < 0xE0) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			unsigned int w = 0;
			unsigned int s = sjis_mobile_decode_tbl1[c] + sjis_decode_tbl2[c2];

			if (s <= 137) {
				if (s == 31) {
					w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xFF5E; /* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225; /* PARALLEL TO */
				} else if (s == 60) {
					w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xFFE0; /* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xFFE1; /* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xFFE2; /* FULLWIDTH NOT SIGN */
				}
			}

			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= (94*94) && s < (114*94)) {
					w = s - (94*94) + 0xE000;
				}
			}

			if (!w) {
				if (c == 0x80 || c == 0xA0 || c >= 0xFD) {
					p--;
				}
				w = MBFL_BAD_INPUT;
			}
			*out++ = w;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_cp932(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s1 = 0, s2 = 0, c1, c2;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s1 = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w == 0x203E) {
			s1 = 0x7E;
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s1 = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s1 = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s1 = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			s1 = w - 0xE000;
			c1 = s1/94 + 0x7F;
			c2 = s1%94 + 0x21;
			s1 = (c1 << 8) | c2;
			s2 = 1;
		}

		if (w == 0xA5) { /* YEN SIGN */
			s1 = 0x5C;
		} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (w == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		} else if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		}

		if (!s1 || (s1 >= 0x8080 && !s2)) { /* not found or X 0212 */
			for (unsigned int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (cp932ext1_ucs_table[i] == w) {
					s1 = ((i/94 + 0x2D) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			for (unsigned int i = 0; i < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; i++) {
				if (cp932ext3_ucs_table[i] == w) {
					s1 = ((i/94 + 0x93) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp932);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

emit_output:
		if (s1 < 0x100) {
			out = mb_convert_buf_add(out, s1);
		} else {
			c1 = (s1 >> 8) & 0xFF;
			c2 = s1 & 0xFF;
			SJIS_ENCODE(c1, c2, s1, s2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_sjiswin(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s1 = 0, s2 = 0, c1, c2;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s1 = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s1 = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s1 = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s1 = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			s1 = w - 0xE000;
			c1 = s1/94 + 0x7F;
			c2 = s1%94 + 0x21;
			s1 = (c1 << 8) | c2;
			s2 = 1;
		}

		if (w == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (w == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		} else if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		}

		if (!s1 || (s1 >= 0x8080 && !s2)) { /* not found or X 0212 */
			for (unsigned int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (cp932ext1_ucs_table[i] == w) {
					s1 = ((i/94 + 0x2D) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			for (unsigned int i = 0; i < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; i++) {
				if (cp932ext3_ucs_table[i] == w) {
					s1 = ((i/94 + 0x93) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp932);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

emit_output:
		if (s1 < 0x100) {
			out = mb_convert_buf_add(out, s1);
		} else {
			c1 = (s1 >> 8) & 0xFF;
			c2 = s1 & 0xFF;
			SJIS_ENCODE(c1, c2, s1, s2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static const unsigned char mblen_table_sjis[] = { /* 0x81-0x9F,0xE0-0xEF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static const unsigned char mblen_table_sjismac[] = { /* 0x81-0x9F,0xE0-0xED */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static const unsigned char mblen_table_sjis_mobile[] = { /* 0x81-0x9F,0xE0-0xFC */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1
};

static const char *mbfl_encoding_sjis_aliases[] = {"x-sjis", "SHIFT-JIS", NULL};

static const struct mbfl_convert_vtbl vtbl_sjis_wchar = {
	mbfl_no_encoding_sjis,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjis = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis,
	mbfl_filt_conv_common_flush,
	NULL
};

const mbfl_encoding mbfl_encoding_sjis = {
	mbfl_no_encoding_sjis,
	"SJIS",
	"Shift_JIS",
	mbfl_encoding_sjis_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_wchar,
	&vtbl_wchar_sjis,
	mb_sjis_to_wchar,
	mb_wchar_to_sjis,
	NULL
};

static const char *mbfl_encoding_sjis_mac_aliases[] = {"MacJapanese", "x-Mac-Japanese", NULL};

static const struct mbfl_convert_vtbl vtbl_sjis_mac_wchar = {
	mbfl_no_encoding_sjis_mac,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mac_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjis_mac = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_mac,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mac,
	mbfl_filt_conv_wchar_sjis_mac_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjis_mac = {
	mbfl_no_encoding_sjis_mac,
	"SJIS-mac",
	"Shift_JIS",
	mbfl_encoding_sjis_mac_aliases,
	mblen_table_sjismac,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_mac_wchar,
	&vtbl_wchar_sjis_mac,
	mb_sjismac_to_wchar,
	mb_wchar_to_sjismac,
	NULL
};

static const char *mbfl_encoding_sjis_docomo_aliases[] = {"SJIS-DOCOMO", "shift_jis-imode", "x-sjis-emoji-docomo", NULL};
static const char *mbfl_encoding_sjis_kddi_aliases[] = {"SJIS-KDDI", "shift_jis-kddi", "x-sjis-emoji-kddi", NULL};
static const char *mbfl_encoding_sjis_sb_aliases[] = {"SJIS-SOFTBANK", "shift_jis-softbank", "x-sjis-emoji-softbank", NULL};

static const struct mbfl_convert_vtbl vtbl_sjis_docomo_wchar = {
	mbfl_no_encoding_sjis_docomo,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mobile_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjis_docomo = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_docomo,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjis_docomo = {
	mbfl_no_encoding_sjis_docomo,
	"SJIS-Mobile#DOCOMO",
	"Shift_JIS",
	mbfl_encoding_sjis_docomo_aliases,
	mblen_table_sjis_mobile,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_docomo_wchar,
	&vtbl_wchar_sjis_docomo,
	mb_sjis_docomo_to_wchar,
	mb_wchar_to_sjis_docomo,
	NULL
};

static const struct mbfl_convert_vtbl vtbl_sjis_kddi_wchar = {
	mbfl_no_encoding_sjis_kddi,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mobile_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjis_kddi = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_kddi,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjis_kddi = {
	mbfl_no_encoding_sjis_kddi,
	"SJIS-Mobile#KDDI",
	"Shift_JIS",
	mbfl_encoding_sjis_kddi_aliases,
	mblen_table_sjis_mobile,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_kddi_wchar,
	&vtbl_wchar_sjis_kddi,
	mb_sjis_kddi_to_wchar,
	mb_wchar_to_sjis_kddi,
	NULL
};

static const struct mbfl_convert_vtbl vtbl_sjis_sb_wchar = {
	mbfl_no_encoding_sjis_sb,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mobile_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjis_sb = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_sb,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjis_sb = {
	mbfl_no_encoding_sjis_sb,
	"SJIS-Mobile#SOFTBANK",
	"Shift_JIS",
	mbfl_encoding_sjis_sb_aliases,
	mblen_table_sjis_mobile,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_sb_wchar,
	&vtbl_wchar_sjis_sb,
	mb_sjis_sb_to_wchar,
	mb_wchar_to_sjis_sb,
	NULL
};

/* Although the specification for Shift-JIS-2004 indicates that 0x5C and
 * 0x7E should (respectively) represent a Yen sign and an overbar, feedback
 * from Japanese PHP users indicates that they prefer 0x5C and 0x7E to be
 * treated as equivalent to U+005C and U+007E. This is the historical
 * behavior of mbstring, and promotes compatibility with other software
 * which handles Shift-JIS and Shift-JIS-2004 text in this way. */

static const char *mbfl_encoding_sjis2004_aliases[] = {"SJIS2004","Shift_JIS-2004", NULL};

static const struct mbfl_convert_vtbl vtbl_sjis2004_wchar = {
	mbfl_no_encoding_sjis2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjis2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjis2004 = {
	mbfl_no_encoding_sjis2004,
	"SJIS-2004",
	"Shift_JIS",
	mbfl_encoding_sjis2004_aliases,
	mblen_table_sjis_mobile, /* Leading byte values used for SJIS-2004 are the same as mobile SJIS variants */
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis2004_wchar,
	&vtbl_wchar_sjis2004,
	mb_sjis2004_to_wchar,
	mb_wchar_to_sjis2004,
	NULL
};

/* CP932 is Microsoft's version of Shift-JIS.
 *
 * What we call "SJIS-win" is a variant of CP932 which maps U+00A5
 * and U+203E the same way as eucJP-win; namely, instead of mapping
 * U+00A5 (YEN SIGN) to 0x5C and U+203E (OVERLINE) to 0x7E,
 * these codepoints are mapped to appropriate JIS X 0208 characters.
 *
 * When converting from Shift-JIS to Unicode, there is no difference
 * between CP932 and "SJIS-win".
 *
 * Additional facts:
 *
 * • In the libmbfl library which formed the base for mbstring, "CP932" and
 *   "SJIS-win" were originally aliases. The differing mappings were added in
 *   December 2002. The libmbfl author later stated that this was done so that
 *   "CP932" would comply with a certain specification, while "SJIS-win" would
 *   maintain the existing mappings. He does not remember which specification
 *   it was.
 * • The WHATWG specification for "Shift_JIS" (followed by web browsers)
 *   agrees with our mappings for "CP932".
 * • Microsoft Windows' "best-fit" mappings for CP932 (via the
 *   WideCharToMultiByte API) convert U+00A5 to 0x5C, which also agrees with
 *   our mappings for "CP932".
 * • glibc's iconv converts U+203E to CP932 0x7E, which again agrees with
 *   our mappings for "CP932".
 * • When converting Shift-JIS to CP932, the conversion goes through Unicode.
 *   Shift-JIS 0x7E converts to U+203E, so mapping U+203E to 0x7E means that
 *   0x7E will go to 0x7E when converting Shift-JIS to CP932.
 */

static const unsigned char mblen_table_sjiswin[] = { /* 0x81-0x9F,0xE0-0xFF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static const char *mbfl_encoding_cp932_aliases[] = {"MS932", "Windows-31J", "MS_Kanji", NULL};
static const char *mbfl_encoding_sjiswin_aliases[] = {"SJIS-ms", "SJIS-open", NULL};

static const struct mbfl_convert_vtbl vtbl_cp932_wchar = {
	mbfl_no_encoding_cp932,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp932_wchar,
	mbfl_filt_conv_cp932_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp932 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp932,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp932,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_cp932 = {
	mbfl_no_encoding_cp932,
	"CP932",
	"Shift_JIS",
	mbfl_encoding_cp932_aliases,
	mblen_table_sjiswin,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp932_wchar,
	&vtbl_wchar_cp932,
	mb_cp932_to_wchar,
	mb_wchar_to_cp932,
	NULL
};

static const struct mbfl_convert_vtbl vtbl_sjiswin_wchar = {
	mbfl_no_encoding_sjiswin,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp932_wchar,
	mbfl_filt_conv_cp932_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_sjiswin = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjiswin,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjiswin,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjiswin = {
	mbfl_no_encoding_sjiswin,
	"SJIS-win",
	"Shift_JIS",
	mbfl_encoding_sjiswin_aliases,
	mblen_table_sjiswin,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjiswin_wchar,
	&vtbl_wchar_sjiswin,
	mb_cp932_to_wchar,
	mb_wchar_to_sjiswin,
	NULL
};

/*
 * EUC variants
 */

static int mbfl_filt_conv_eucjp_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w = 0;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xff) {	/* X 0208 first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* kana first char */
			filter->status = 2;
		} else if (c == 0x8f) {	/* X 0212 first char */
			filter->status = 3;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[s];
				if (!w)
					w = MBFL_BAD_INPUT;
			} else {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2:	/* got 0x8e */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* got 0x8f, JIS X 0212 first byte */
		filter->status++;
		filter->cache = c;
		break;

	case 4: /* got 0x8f, JIS X 0212 second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xA0 && c < 0xFF && c1 > 0xA0 && c1 < 0xFF) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0212_ucs_table_size) {
				w = jisx0212_ucs_table[s];
				if (!w)
					w = MBFL_BAD_INPUT;
			} else {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_eucjp_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_eucjp(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c == 0xAF) { /* U+00AF is MACRON */
		s = 0xA2B4; /* Use JIS X 0212 overline */
	} else if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s <= 0) {
		if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		} else if (c == 0) {
			s = 0;
		} else {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s < 0x80) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data));
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s & 0xff) | 0x80, filter->data));
		} else {	/* X 0212 */
			CK((*filter->output_function)(0x8f, filter->data));
			CK((*filter->output_function)(((s >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s & 0xff) | 0x80, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_eucjp_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE && p < e) {
			/* JISX 0208 */
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int s = (c - 0xA1)*94 + c2 - 0xA1;
				if (s < jisx0208_ucs_table_size) {
					uint32_t w = jisx0208_ucs_table[s];
					if (!w)
						w = MBFL_BAD_INPUT;
					*out++ = w;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			/* Kana */
			unsigned char c2 = *p++;
			*out++ = (c2 >= 0xA1 && c2 <= 0xDF) ? 0xFEC0 + c2 : MBFL_BAD_INPUT;
		} else if (c == 0x8F) {
			/* JISX 0212 */
			if ((e - p) >= 2) {
				unsigned char c2 = *p++;
				unsigned char c3 = *p++;
				if (c3 >= 0xA1 && c3 <= 0xFE && c2 >= 0xA1 && c2 <= 0xFE) {
					unsigned int s = (c2 - 0xA1)*94 + c3 - 0xA1;
					if (s < jisx0212_ucs_table_size) {
						uint32_t w = jisx0212_ucs_table[s];
						if (!w)
							w = MBFL_BAD_INPUT;
						*out++ = w;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				p = e; /* Jump to end of string */
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_eucjp(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0xAF) { /* U+00AF is MACRON */
			s = 0xA2B4; /* Use JIS X 0212 overline */
		} else if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w == 0) {
				out = mb_convert_buf_add(out, 0);
				continue;
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_eucjp);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		}

		if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x100) {
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else if (s < 0x8080)  {
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 3);
			out = mb_convert_buf_add3(out, 0x8F, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_eucjpwin_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w, n;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c >= 0xa1 && c <= 0xfe) { /* CP932 first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) { /* kana first char */
			filter->status = 2;
		} else if (c == 0x8f) { /* X 0212 first char */
			filter->status = 3;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			w = 0;
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s <= 137) {
				if (s == 31) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xff5e;			/* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225;			/* PARALLEL TO */
				} else if (s == 60) {
					w = 0xff0d;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xffe0;			/* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xffe1;			/* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xffe2;			/* FULLWIDTH NOT SIGN */
				}
			}

			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= (84 * 94)) {		/* user (85ku - 94ku) */
					w = s - (84 * 94) + 0xe000;
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2:	/* got 0x8e, X0201 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3:	/* got 0x8f,  X 0212 first char */
		filter->status++;
		filter->cache = c;
		break;

	case 4:	/* got 0x8f,  X 0212 second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c1 > 0xa0 && c1 < 0xff && c > 0xa0 && c < 0xff) {
			s = (c1 - 0xa1)*94 + c - 0xa1;

			if (s >= 0 && s < jisx0212_ucs_table_size) {
				w = jisx0212_ucs_table[s];

				if (w == 0x007e) {
					w = 0xff5e;		/* FULLWIDTH TILDE */
				}
			} else if (s >= (82*94) && s < (84*94)) {	/* vender ext3 (83ku - 84ku) <-> CP932 (115ku -120ku) */
				s = (c1 << 8) | c;
				w = 0;
				n = 0;
				while (n < cp932ext3_eucjp_table_size) {
					if (s == cp932ext3_eucjp_table[n]) {
						if (n < (cp932ext3_ucs_table_max - cp932ext3_ucs_table_min)) {
							w = cp932ext3_ucs_table[n];
						}
						break;
					}
					n++;
				}
			} else if (s >= (84*94)) {		/* user (85ku - 94ku) */
				w = s - (84*94) + (0xe000 + (94*10));
			} else {
				w = 0;
			}

			if (w == 0x00A6) {
				w = 0xFFE4;		/* FULLWIDTH BROKEN BAR */
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_eucjpwin_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_eucjpwin(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0;

	if (c == 0xAF) { /* U+00AF is MACRON */
		s1 = 0xA2B4; /* Use JIS X 0212 overline */
	} else if (c == 0x203E) {
		s1 = 0x7E;
	} else if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 10*94)) { /* user (X0208 85ku - 94ku) */
		s1 = c - 0xe000;
		c1 = s1/94 + 0x75;
		c2 = s1%94 + 0x21;
		s1 = (c1 << 8) | c2;
	} else if (c >= (0xe000 + 10*94) && c < (0xe000 + 20*94)) { /* user (X0212 85ku - 94ku) */
		s1 = c - (0xe000 + 10*94);
		c1 = s1/94 + 0xf5;
		c2 = s1%94 + 0xa1;
		s1 = (c1 << 8) | c2;
	}

	if (s1 == 0xa2f1) {
		s1 = 0x2d62;		/* NUMERO SIGN */
	}

	if (s1 <= 0) {
		if (c == 0xa5) {		/* YEN SIGN */
			s1 = 0x5C;
		} else if (c == 0x2014) {
			s1 = 0x213D;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		} else {
			s1 = -1;
			c1 = 0;
			c2 = cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext1 (13ku) */
				const int oh = cp932ext1_ucs_table_min / 94;

				if (c == cp932ext1_ucs_table[c1]) {
					s1 = ((c1 / 94 + oh + 0x21) << 8) + (c1 % 94 + 0x21);
					break;
				}
				c1++;
			}
			if (s1 < 0) {
				c1 = 0;
				c2 = cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
				while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
					if (c == cp932ext3_ucs_table[c1]) {
						if (c1 < cp932ext3_eucjp_table_size) {
							s1 = cp932ext3_eucjp_table[c1];
						}
						break;
					}
					c1++;
				}
			}
		}

		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) {	/* latin */
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data));
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s1 & 0xff) | 0x80, filter->data));
		} else {	/* X 0212 */
			CK((*filter->output_function)(0x8f, filter->data));
			CK((*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s1 & 0xff) | 0x80, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_eucjpwin_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE && p < e) {
			unsigned char c2 = *p++;

			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int s = (c - 0xA1)*94 + c2 - 0xA1, w = 0;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (w == 0) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= (84 * 94)) {
						w = s - (84 * 94) + 0xE000;
					}
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xDF) {
				*out++ = 0xFEC0 + c2;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8F && p < e) {
			unsigned char c2 = *p++;
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}
			unsigned char c3 = *p++;

			if (c2 >= 0xA1 && c2 <= 0xFE && c3 >= 0xA1 && c3 <= 0xFE) {
				unsigned int s = (c2 - 0xA1)*94 + c3 - 0xA1, w = 0;

				if (s < jisx0212_ucs_table_size) {
					w = jisx0212_ucs_table[s];
					if (w == 0x7E)
						w = 0xFF5E; /* FULLWIDTH TILDE */
				} else if (s >= (82*94) && s < (84*94)) {
					s = (c2 << 8) | c3;
					for (int i = 0; i < cp932ext3_eucjp_table_size; i++) {
						if (cp932ext3_eucjp_table[i] == s) {
							w = cp932ext3_ucs_table[i];
							break;
						}
					}
				} else if (s >= (84*94)) {
					w = s - (84*94) + 0xE000 + (94*10);
				}

				if (w == 0xA6)
					w = 0xFFE4; /* FULLWIDTH BROKEN BAR */

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_eucjpwin(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		} else if (w == 0xAF) { /* U+00AF is MACRON */
			s = 0xA2B4; /* Use JIS X 0212 overline */
		} else if (w == 0x203E) {
			s = 0x7E;
		} else if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 10*94)) {
			s = w - 0xE000;
			s = ((s/94 + 0x75) << 8) + (s%94) + 0x21;
		} else if (w >= (0xE000 + 10*94) && w < (0xE000 + 20*94)) {
			s = w - (0xE000 + 10*94);
			s = ((s/94 + 0xF5) << 8) + (s%94) + 0xA1;
		}

		if (s == 0xA2F1)
			s = 0x2D62; /* NUMERO SIGN */

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x5C;
			} else if (w == 0x2014) { /* EM DASH */
				s = 0x213D;
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else {
				for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
					if (cp932ext1_ucs_table[i] == w) {
						s = (((i/94) + (cp932ext1_ucs_table_min/94) + 0x21) << 8) + (i%94) + 0x21;
						break;
					}
				}

				if (!s) {
					for (int i = 0; i < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; i++) {
						if (cp932ext3_ucs_table[i] == w) {
							s = cp932ext3_eucjp_table[i];
							break;
						}
					}
				}
			}
		}

		if (!s) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_eucjpwin);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x100) {
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else if (s < 0x8080) {
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 3);
			out = mb_convert_buf_add3(out, 0x8F, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_cp51932_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c >= 0xA1 && c <= 0xFE) { /* CP932, first byte */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) { /* kana first char */
			filter->status = 2;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			w = 0;
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s <= 137) {
				if (s == 31) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xff5e;			/* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225;			/* PARALLEL TO */
				} else if (s == 60) {
					w = 0xff0d;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xffe0;			/* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xffe1;			/* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xffe2;			/* FULLWIDTH NOT SIGN */
				}
			}
			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				}
			}
			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2:	/* got 0x8e, X0201 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp51932_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp51932(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1;

	s1 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s1 >= 0x8080) s1 = -1; /* we don't support JIS X0213 */
	if (s1 <= 0) {
		if (c == 0xa5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		} else {
			s1 = -1;
			c1 = 0;
			c2 = cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext1 (13ku) */
				if (c == cp932ext1_ucs_table[c1]) {
					s1 = ((c1/94 + 0x2d) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
			if (s1 < 0) {
				c1 = 0;
				c2 = cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
				while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
					if (c == cp932ext2_ucs_table[c1]) {
						s1 = ((c1/94 + 0x79) << 8) +(c1%94 + 0x21);
						break;
					}
					c1++;
				}
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) {	/* latin */
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data));
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s1 & 0xff) | 0x80, filter->data));
		} else {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_cp51932_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE && p < e) {
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int s = (c - 0xA1)*94 + c2 - 0xA1, w = 0;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (w == 0) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xDF) {
				*out++ = 0xFEC0 + c2;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_cp51932(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		} else if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s >= 0x8080) s = 0; /* We don't support JIS X0213 */

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else {
				for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
					if (cp932ext1_ucs_table[i] == w) {
						s = ((i/94 + 0x2D) << 8) + (i%94) + 0x21;
						goto found_it;
					}
				}

				for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
					if (cp932ext2_ucs_table[i] == w) {
						s = ((i/94 + 0x79) << 8) + (i%94) + 0x21;
						goto found_it;
					}
				}
			}
found_it: ;
		}

		if (!s || s >= 0x8080) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp51932);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x100) {
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else {
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_eucjp2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE) {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			if (c2 <= 0xA0 || c2 == 0xFF) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			unsigned int s1 = c - 0x80, s2 = c2 - 0x80;
			unsigned int w1 = (s1 << 8) | s2, w = 0;

			/* Conversion for combining characters */
			if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) || (w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 || (w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
				int k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
				if (k >= 0) {
					*out++ = jisx0213_u2_tbl[2*k];
					*out++ = jisx0213_u2_tbl[2*k+1];
					continue;
				}
			}

			/* Conversion for BMP  */
			w1 = (s1 - 0x21)*94 + s2 - 0x21;
			if (w1 < jisx0213_ucs_table_size) {
				w = jisx0213_ucs_table[w1];
			}

			/* Conversion for CJK Unified Ideographs ext.B (U+2XXXX) */
			if (!w) {
				int k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
				if (k >= 0) {
					w = jisx0213_jis_u5_tbl[k] + 0x20000;
				}
			}

			*out++ = w ? w : MBFL_BAD_INPUT;
		} else if (c == 0x8E && p < e) {
			/* Kana */
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xDF) {
				*out++ = 0xFEC0 + c2;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8F && p < e) {
			unsigned char c2 = *p++;
			if ((c2 == 0xA1 || (c2 >= 0xA3 && c2 <= 0xA5) || c2 == 0xA8 || (c2 >= 0xAC && c2 <= 0xAF) || (c2 >= 0xEE && c2 <= 0xFE)) && p < e) {
				unsigned char c3 = *p++;

				if (c3 < 0xA1 || c3 == 0xFF) {
					*out++ = MBFL_BAD_INPUT;
					continue;
				}

				unsigned int s1 = c2 - 0xA1, s2 = c3 - 0xA1;

				if (((s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) || (s1 >= 77 && s1 < 94)) && s2 < 94) {
					int k;
					for (k = 0; k < jisx0213_p2_ofst_len; k++) {
						if (s1 == jisx0213_p2_ofst[k]) {
							break;
						}
					}
					k -= jisx0213_p2_ofst[k];

					/* Check for Japanese chars in BMP */
					unsigned int s = (s1 + 94 + k)*94 + s2;
					ZEND_ASSERT(s < jisx0213_ucs_table_size);
					unsigned int w = jisx0213_ucs_table[s];

					/* Check for Japanese chars in CJK Unified Ideographs ext B (U+2XXXX) */
					if (!w) {
						k = mbfl_bisec_srch2(s, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
						if (k >= 0) {
							w = jisx0213_jis_u5_tbl[k] + 0x20000;
						}
					}

					*out++ = w ? w : MBFL_BAD_INPUT;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_eucjp2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;
	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		goto process_codepoint;
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		/* Check for 1st char of combining characters */
		if (w == 0xE6 || (w >= 0x254 && w <= 0x2E9) || (w >= 0x304B && w <= 0x3053) || (w >= 0x30AB && w <= 0x30C8) || w == 0x31F7) {
			for (int k = 0; k < jisx0213_u2_tbl_len; k++) {
				if (w == jisx0213_u2_tbl[2*k]) {
					if (!len) {
						if (!end) {
							buf->state = w;
							MB_CONVERT_BUF_STORE(buf, out, limit);
							return;
						}
					} else {
						uint32_t w2 = *in++; len--;
						if ((w == 0x254 || w == 0x28C || w == 0x259 || w == 0x25A) && w2 == 0x301) {
							k++;
						}
						if (w2 == jisx0213_u2_tbl[2*k+1]) {
							s = jisx0213_u2_key[k];
							break;
						}
						in--; len++;
					}

					/* Fallback */
					s = jisx0213_u2_fb_tbl[k];
					break;
				}
			}
		}

		/* Check for major Japanese chars: U+4E00-U+9FFF */
		if (!s) {
			for (int k = 0; k < uni2jis_tbl_len; k++) {
				if (w >= uni2jis_tbl_range[k][0] && w <= uni2jis_tbl_range[k][1]) {
					s = uni2jis_tbl[k][w - uni2jis_tbl_range[k][0]];
					break;
				}
			}
		}

		/* Check for Japanese chars in compressed mapping area: U+1E00-U+4DBF */
		if (!s && w >= ucs_c1_jisx0213_min && w <= ucs_c1_jisx0213_max) {
			int k = mbfl_bisec_srch(w, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
			if (k >= 0) {
				s = ucs_c1_jisx0213_ofst[k] + w - ucs_c1_jisx0213_tbl[2*k];
			}
		}

		/* Check for Japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
		if (!s && w >= jisx0213_u5_tbl_min && w <= jisx0213_u5_tbl_max) {
			int k = mbfl_bisec_srch2(w - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				s = jisx0213_u5_jis_tbl[k];
			}
		}

		if (!s) {
			/* CJK Compatibility Forms: U+FE30-U+FE4F */
			if (w == 0xFE45) {
				s = 0x233E;
			} else if (w == 0xFE46) {
				s = 0x233D;
			} else if (w >= 0xF91D && w <= 0xF9DC) {
				/* CJK Compatibility Ideographs: U+F900-U+F92A */
				int k = mbfl_bisec_srch2(w, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
				if (k >= 0) {
					s = ucs_r2b_jisx0213_cmap_val[k];
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_eucjp2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			out = mb_convert_buf_add(out, s);
		} else if (s <= 0xFF) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else if (s <= 0x7EFF) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) + 0x80, (s & 0xFF) + 0x80);
		} else {
			unsigned int s2 = s & 0xFF;
			int k = ((s >> 8) & 0xFF) - 0x7F;
			ZEND_ASSERT(k < jisx0213_p2_ofst_len);
			s = jisx0213_p2_ofst[k] + 0x21;
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
			out = mb_convert_buf_add3(out, 0x8F, s | 0x80, s2 | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_euccn_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if ((c >= 0xA1 && c <= 0xA9) || (c >= 0xB0 && c <= 0xF7)) { /* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xA0 && c < 0xFF) {
			w = (c1 - 0x81)*192 + c - 0x40;
			ZEND_ASSERT(w < cp936_ucs_table_size);
			if (w == 0x1864) {
				w = 0x30FB;
			} else if (w == 0x186A) {
				w = 0x2015;
			} else if ((w >= 0x1921 && w <= 0x192A) || w == 0x1963 || (w >= 0x1C59 && w <= 0x1C7E) || (w >= 0x1DBB && w <= 0x1DC4)) {
				w = 0;
			} else {
				w = cp936_ucs_table[w];
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_euccn(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		if (c == 0xB7 || c == 0x144 || c == 0x148 || c == 0x251 || c == 0x261) {
			s = 0;
		} else {
			s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
		}
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		if (c == 0x2015) {
			s = 0xA1AA;
		} else if (c == 0x2014 || (c >= 0x2170 && c <= 0x2179)) {
			s = 0;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		if (c == 0x30FB) {
			s = 0xA1A4;
		} else {
			s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
		}
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		if (c == 0xFF04) {
			s = 0xA1E7;
		} else if (c == 0xFF5E) {
			s = 0xA1AB;
		} else if (c >= 0xFF01 && c <= 0xFF5D) {
			s = c - 0xFF01 + 0xA3A1;
		} else if (c >= 0xFFE0 && c <= 0xFFE5) {
			s = ucs_hff_s_cp936_table[c - 0xFFE0];
		}
	}

	/* exclude CP936 extensions */
	if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
		s = 0;
	}

	if (s <= 0) {
		if (c < 0x80) {
			s = c;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s & 0xFF, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_euccn_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_euccn_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (((c >= 0xA1 && c <= 0xA9) || (c >= 0xB0 && c <= 0xF7)) && p < e) {
			unsigned char c2 = *p++;

			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int w = (c - 0x81)*192 + c2 - 0x40;
				ZEND_ASSERT(w < cp936_ucs_table_size);
				if (w == 0x1864) {
					w = 0x30FB;
				} else if (w == 0x186A) {
					w = 0x2015;
				} else if ((w >= 0x1921 && w <= 0x192A) || w == 0x1963 || (w >= 0x1C59 && w <= 0x1C7E) || (w >= 0x1DBB && w <= 0x1DC4)) {
					w = 0;
				} else {
					w = cp936_ucs_table[w];
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_euccn(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			if (w != 0xB7 && w != 0x144 && w != 0x148 && w != 0x251 && w != 0x261) {
				s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
			}
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			if (w == 0x2015) {
				s = 0xA1AA;
			} else if (w != 0x2014 && (w < 0x2170 || w > 0x2179)) {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			if (w == 0x30FB) {
				s = 0xA1A4;
			} else {
				s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
			}
		} else if (w >= ucs_i_cp936_table_min && w < ucs_i_cp936_table_max) {
			s = ucs_i_cp936_table[w - ucs_i_cp936_table_min];
		} else if (w >= ucs_hff_cp936_table_min && w < ucs_hff_cp936_table_max) {
			if (w == 0xFF04) {
				s = 0xA1E7;
			} else if (w == 0xFF5E) {
				s = 0xA1AB;
			} else if (w >= 0xFF01 && w <= 0xFF5D) {
				s = w - 0xFF01 + 0xA3A1;
			} else if (w >= 0xFFE0 && w <= 0xFFE5) {
				s = ucs_hff_s_cp936_table[w - 0xFFE0];
			}
		}

		/* Exclude CP936 extensions */
		if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
			s = 0;
		}

		if (!s) {
			if (w < 0x80) {
				out = mb_convert_buf_add(out, w);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_euccn);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			}
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_euctw_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (((c >= 0xA1 && c <= 0xA6) || (c >= 0xC2 && c <= 0xFD)) && c != 0xC3) { /* 2-byte character, first byte */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8E) { /* 4-byte character, first byte */
			filter->status = 2;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* 2-byte character, second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xA0 && c < 0xFF) {
			w = (c1 - 0xA1)*94 + (c - 0xA1);
			if (w >= 0 && w < cns11643_1_ucs_table_size) {
				w = cns11643_1_ucs_table[w];
			} else {
				w = 0;
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2: /* got 0x8e, second byte */
		if (c == 0xA1 || c == 0xA2 || c == 0xAE) {
			filter->status = 3;
			filter->cache = c - 0xA1;
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* got 0x8e, third byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0xA1 && ((c1 == 0 && ((c >= 0xA1 && c <= 0xA6) || (c >= 0xC2 && c <= 0xFD)) && c != 0xC3) ||
				(c1 == 1 && c <= 0xF2) || (c1 == 13 && c <= 0xE7))) {
			filter->status = 4;
			filter->cache = (c1 << 8) + c - 0xA1;
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 4:	/* multi-byte character, fourth byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c1 <= 0xDFF && c > 0xA0 && c < 0xFF) {
			int plane = (c1 & 0xF00) >> 8; /* This is actually the CNS-11643 plane minus one */
			s = (c1 & 0xFF)*94 + c - 0xA1;
			w = 0;
			if (s >= 0) {
				/* A later version of CNS-11643 moved all the characters in "plane 14" to "plane 3",
				 * and added tens of thousands more characters in planes 4, 5, 6, and 7
				 * We only support the older version of CNS-11643
				 * This is the same as iconv from glibc 2.2 */
				if (plane == 0 && s < cns11643_1_ucs_table_size) {
					w = cns11643_1_ucs_table[s];
				} else if (plane == 1 && s < cns11643_2_ucs_table_size) {
					w = cns11643_2_ucs_table[s];
				} else if (plane == 13 && s < cns11643_14_ucs_table_size) {
					w = cns11643_14_ucs_table[s];
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_euctw(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_cns11643_table_min && c < ucs_a1_cns11643_table_max) {
		s = ucs_a1_cns11643_table[c - ucs_a1_cns11643_table_min];
	} else if (c >= ucs_a2_cns11643_table_min && c < ucs_a2_cns11643_table_max) {
		s = ucs_a2_cns11643_table[c - ucs_a2_cns11643_table_min];
	} else if (c >= ucs_a3_cns11643_table_min && c < ucs_a3_cns11643_table_max) {
		s = ucs_a3_cns11643_table[c - ucs_a3_cns11643_table_min];
	} else if (c >= ucs_i_cns11643_table_min && c < ucs_i_cns11643_table_max) {
		s = ucs_i_cns11643_table[c - ucs_i_cns11643_table_min];
	} else if (c >= ucs_r_cns11643_table_min && c < ucs_r_cns11643_table_max) {
		s = ucs_r_cns11643_table[c - ucs_r_cns11643_table_min];
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		int plane = (s & 0x1F0000) >> 16;
		if (plane <= 1) {
			if (s < 0x80) { /* latin */
				CK((*filter->output_function)(s, filter->data));
			} else {
				s = (s & 0xFFFF) | 0x8080;
				CK((*filter->output_function)((s >> 8) & 0xFF, filter->data));
				CK((*filter->output_function)(s & 0xFF, filter->data));
			}
		} else {
			s = (0x8EA00000 + (plane << 16)) | ((s & 0xFFFF) | 0x8080);
			CK((*filter->output_function)(0x8e , filter->data));
			CK((*filter->output_function)((s >> 16) & 0xFF, filter->data));
			CK((*filter->output_function)((s >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s & 0xFF, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return 0;
}

static int mbfl_filt_conv_euctw_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* 2-byte or 4-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_euctw_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (((c >= 0xA1 && c <= 0xA6) || (c >= 0xC2 && c <= 0xFD)) && c != 0xC3 && p < e) {
			unsigned char c2 = *p++;

			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int w = (c - 0xA1)*94 + (c2 - 0xA1);
				if (w < cns11643_1_ucs_table_size) {
					w = cns11643_1_ucs_table[w];
				} else {
					w = 0;
				}
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			unsigned char c2 = *p++;

			if ((c2 == 0xA1 || c2 == 0xA2 || c2 == 0xAE) && p < e) {
				unsigned int plane = c2 - 0xA1; /* This is actually the CNS-11643 plane minus one */
				unsigned char c3 = *p++;

				if (c3 >= 0xA1 && ((plane == 0 && ((c3 >= 0xA1 && c3 <= 0xA6) || (c3 >= 0xC2 && c3 <= 0xFD)) && c3 != 0xC3) || (plane == 1 && c3 <= 0xF2) || (plane == 13 && c3 <= 0xE7)) && p < e) {
					unsigned char c4 = *p++;

					if (c2 <= 0xAE && c4 > 0xA0 && c4 < 0xFF) {
						unsigned int s = (c3 - 0xA1)*94 + c4 - 0xA1, w = 0;

						/* A later version of CNS-11643 moved all the characters in "plane 14" to "plane 3",
						 * and added tens of thousands more characters in planes 4, 5, 6, and 7
						 * We only support the older version of CNS-11643
						 * This is the same as iconv from glibc 2.2 */
						if (plane == 0 && s < cns11643_1_ucs_table_size) {
							w = cns11643_1_ucs_table[s];
						} else if (plane == 1 && s < cns11643_2_ucs_table_size) {
							w = cns11643_2_ucs_table[s];
						} else if (plane == 13 && s < cns11643_14_ucs_table_size) {
							w = cns11643_14_ucs_table[s];
						}

						if (!w)
							w = MBFL_BAD_INPUT;
						*out++ = w;
						continue;
					}
				}
			}

			*out++ = MBFL_BAD_INPUT;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_euctw(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cns11643_table_min && w < ucs_a1_cns11643_table_max) {
			s = ucs_a1_cns11643_table[w - ucs_a1_cns11643_table_min];
		} else if (w >= ucs_a2_cns11643_table_min && w < ucs_a2_cns11643_table_max) {
			s = ucs_a2_cns11643_table[w - ucs_a2_cns11643_table_min];
		} else if (w >= ucs_a3_cns11643_table_min && w < ucs_a3_cns11643_table_max) {
			s = ucs_a3_cns11643_table[w - ucs_a3_cns11643_table_min];
		} else if (w >= ucs_i_cns11643_table_min && w < ucs_i_cns11643_table_max) {
			s = ucs_i_cns11643_table[w - ucs_i_cns11643_table_min];
		} else if (w >= ucs_r_cns11643_table_min && w < ucs_r_cns11643_table_max) {
			s = ucs_r_cns11643_table[w - ucs_r_cns11643_table_min];
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_euctw);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			}
		} else {
			unsigned int plane = s >> 16;
			if (plane <= 1) {
				if (s < 0x80) {
					out = mb_convert_buf_add(out, s);
				} else {
					out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
				}
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add4(out, 0x8E, 0xA0 + plane, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
			}
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_euckr_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, w, flag;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (((c >= 0xA1 && c <= 0xAC) || (c >= 0xB0 && c <= 0xFD)) && c != 0xC9) { /* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		flag = 0;
		if (c1 >= 0xa1 && c1 <= 0xc6) {
			flag = 1;
		} else if (c1 >= 0xc7 && c1 <= 0xfe && c1 != 0xc9) {
			flag = 2;
		}
		if (flag > 0 && c >= 0xa1 && c <= 0xfe) {
			if (flag == 1) { /* 1st: 0xa1..0xc6, 2nd: 0x41..0x7a, 0x81..0xfe */
				w = (c1 - 0x81)*190 + c - 0x41;
				ZEND_ASSERT(w < uhc1_ucs_table_size);
				w = uhc1_ucs_table[w];
			} else { /* 1st: 0xc7..0xc8,0xca..0xfe, 2nd: 0xa1..0xfe */
				w = (c1 - 0xc7)*94 + c - 0xa1;
				ZEND_ASSERT(w < uhc3_ucs_table_size);
				w = uhc3_ucs_table[w];
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_euckr(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_uhc_table_min && c < ucs_a1_uhc_table_max) {
		s = ucs_a1_uhc_table[c - ucs_a1_uhc_table_min];
	} else if (c >= ucs_a2_uhc_table_min && c < ucs_a2_uhc_table_max) {
		s = ucs_a2_uhc_table[c - ucs_a2_uhc_table_min];
	} else if (c >= ucs_a3_uhc_table_min && c < ucs_a3_uhc_table_max) {
		s = ucs_a3_uhc_table[c - ucs_a3_uhc_table_min];
	} else if (c >= ucs_i_uhc_table_min && c < ucs_i_uhc_table_max) {
		s = ucs_i_uhc_table[c - ucs_i_uhc_table_min];
	} else if (c >= ucs_s_uhc_table_min && c < ucs_s_uhc_table_max) {
		s = ucs_s_uhc_table[c - ucs_s_uhc_table_min];
	} else if (c >= ucs_r1_uhc_table_min && c < ucs_r1_uhc_table_max) {
		s = ucs_r1_uhc_table[c - ucs_r1_uhc_table_min];
	} else if (c >= ucs_r2_uhc_table_min && c < ucs_r2_uhc_table_max) {
		s = ucs_r2_uhc_table[c - ucs_r2_uhc_table_min];
	}

	/* exclude UHC extension area (although we are using the UHC conversion tables) */
	if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
		s = 0;
	}

	if (s <= 0) {
		if (c < 0x80) {
			s = c;
		} else {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_euckr_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_euckr_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (((c >= 0xA1 && c <= 0xAC) || (c >= 0xB0 && c <= 0xFD)) && c != 0xC9 && p < e) {
			unsigned char c2 = *p++;
			if (c2 < 0xA1 || c2 == 0xFF) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			if (c <= 0xC6) {
				unsigned int w = (c - 0x81)*190 + c2 - 0x41;
				ZEND_ASSERT(w < uhc1_ucs_table_size);
				w = uhc1_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				unsigned int w = (c - 0xC7)*94 + c2 - 0xA1;
				ZEND_ASSERT(w < uhc3_ucs_table_size);
				w = uhc3_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_euckr(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_uhc_table_min && w < ucs_a1_uhc_table_max) {
			s = ucs_a1_uhc_table[w - ucs_a1_uhc_table_min];
		} else if (w >= ucs_a2_uhc_table_min && w < ucs_a2_uhc_table_max) {
			s = ucs_a2_uhc_table[w - ucs_a2_uhc_table_min];
		} else if (w >= ucs_a3_uhc_table_min && w < ucs_a3_uhc_table_max) {
			s = ucs_a3_uhc_table[w - ucs_a3_uhc_table_min];
		} else if (w >= ucs_i_uhc_table_min && w < ucs_i_uhc_table_max) {
			s = ucs_i_uhc_table[w - ucs_i_uhc_table_min];
		} else if (w >= ucs_s_uhc_table_min && w < ucs_s_uhc_table_max) {
			s = ucs_s_uhc_table[w - ucs_s_uhc_table_min];
		} else if (w >= ucs_r1_uhc_table_min && w < ucs_r1_uhc_table_max) {
			s = ucs_r1_uhc_table[w - ucs_r1_uhc_table_min];
		} else if (w >= ucs_r2_uhc_table_min && w < ucs_r2_uhc_table_max) {
			s = ucs_r2_uhc_table[w - ucs_r2_uhc_table_min];
		}

		/* Exclude UHC extension area (although we are using the UHC conversion tables) */
		if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
			s = 0;
		}

		if (!s) {
			if (w < 0x80) {
				out = mb_convert_buf_add(out, w);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_euckr);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_uhc_wchar(int c, mbfl_convert_filter *filter)
{
	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0x80 && c < 0xfe && c != 0xc9) { /* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		int c1 = filter->cache, w = 0;

		if (c1 >= 0x81 && c1 <= 0xc6 && c >= 0x41 && c <= 0xfe) {
			w = (c1 - 0x81)*190 + (c - 0x41);
			if (w >= 0 && w < uhc1_ucs_table_size) {
				w = uhc1_ucs_table[w];
			}
		} else if (c1 >= 0xc7 && c1 < 0xfe && c >= 0xa1 && c <= 0xfe) {
			w = (c1 - 0xc7)*94 + (c - 0xa1);
			if (w >= 0 && w < uhc3_ucs_table_size) {
				w = uhc3_ucs_table[w];
			}
		}

		if (w == 0) {
			w = MBFL_BAD_INPUT;
		}
		CK((*filter->output_function)(w, filter->data));
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_uhc_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_uhc(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_uhc_table_min && c < ucs_a1_uhc_table_max) {
		s = ucs_a1_uhc_table[c - ucs_a1_uhc_table_min];
	} else if (c >= ucs_a2_uhc_table_min && c < ucs_a2_uhc_table_max) {
		s = ucs_a2_uhc_table[c - ucs_a2_uhc_table_min];
	} else if (c >= ucs_a3_uhc_table_min && c < ucs_a3_uhc_table_max) {
		s = ucs_a3_uhc_table[c - ucs_a3_uhc_table_min];
	} else if (c >= ucs_i_uhc_table_min && c < ucs_i_uhc_table_max) {
		s = ucs_i_uhc_table[c - ucs_i_uhc_table_min];
	} else if (c >= ucs_s_uhc_table_min && c < ucs_s_uhc_table_max) {
		s = ucs_s_uhc_table[c - ucs_s_uhc_table_min];
	} else if (c >= ucs_r1_uhc_table_min && c < ucs_r1_uhc_table_max) {
		s = ucs_r1_uhc_table[c - ucs_r1_uhc_table_min];
	} else if (c >= ucs_r2_uhc_table_min && c < ucs_r2_uhc_table_max) {
		s = ucs_r2_uhc_table[c - ucs_r2_uhc_table_min];
	}

	if (s == 0 && c != 0) {
		s = -1;
	}

	if (s >= 0) {
		if (s < 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_uhc_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	e--; /* Stop the main loop 1 byte short of the end of the input */

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c > 0x80 && c < 0xFE) {
			/* We don't need to check p < e here; it's not possible that this pointer dereference
			 * will be outside the input string, because of e-- above */
			unsigned char c2 = *p++;
			if (c2 < 0x41 || c2 == 0xFF) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}
			unsigned int w = 0;

			if (c <= 0xC6) {
				w = (c - 0x81)*190 + c2 - 0x41;
				ZEND_ASSERT(w < uhc1_ucs_table_size);
				w = uhc1_ucs_table[w];
			} else if (c2 >= 0xA1) {
				w = (c - 0xC7)*94 + c2 - 0xA1;
				ZEND_ASSERT(w < uhc3_ucs_table_size);
				w = uhc3_ucs_table[w];
			}
			if (!w) {
				/* If c == 0xC9, we shouldn't have tried to read a 2-byte char at all... but it is faster
				 * to fix up that rare case here rather than include an extra check in the hot path */
				if (c == 0xC9) {
					p--;
				}
				w = MBFL_BAD_INPUT;
			}
			*out++ = w;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	/* Finish up last byte of input string if there is one */
	if (p == e && out < limit) {
		unsigned char c = *p++;
		*out++ = (c < 0x80) ? c : MBFL_BAD_INPUT;
	}

	*in_len = e - p + 1;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_uhc(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_uhc_table_min && w < ucs_a1_uhc_table_max) {
			s = ucs_a1_uhc_table[w - ucs_a1_uhc_table_min];
		} else if (w >= ucs_a2_uhc_table_min && w < ucs_a2_uhc_table_max) {
			s = ucs_a2_uhc_table[w - ucs_a2_uhc_table_min];
		} else if (w >= ucs_a3_uhc_table_min && w < ucs_a3_uhc_table_max) {
			s = ucs_a3_uhc_table[w - ucs_a3_uhc_table_min];
		} else if (w >= ucs_i_uhc_table_min && w < ucs_i_uhc_table_max) {
			s = ucs_i_uhc_table[w - ucs_i_uhc_table_min];
		} else if (w >= ucs_s_uhc_table_min && w < ucs_s_uhc_table_max) {
			s = ucs_s_uhc_table[w - ucs_s_uhc_table_min];
		} else if (w >= ucs_r1_uhc_table_min && w < ucs_r1_uhc_table_max) {
			s = ucs_r1_uhc_table[w - ucs_r1_uhc_table_min];
		} else if (w >= ucs_r2_uhc_table_min && w < ucs_r2_uhc_table_max) {
			s = ucs_r2_uhc_table[w - ucs_r2_uhc_table_min];
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_uhc);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static const unsigned char mblen_table_eucjp[] = { /* 0xA1-0xFE */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_euc_jp_aliases[] = {"EUC", "EUC_JP", "eucJP", "x-euc-jp", NULL};

static const struct mbfl_convert_vtbl vtbl_eucjp_wchar = {
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_eucjp_wchar,
	mbfl_filt_conv_eucjp_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_eucjp = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_jp,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_eucjp,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_euc_jp = {
	mbfl_no_encoding_euc_jp,
	"EUC-JP",
	"EUC-JP",
	mbfl_encoding_euc_jp_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_eucjp_wchar,
	&vtbl_wchar_eucjp,
	mb_eucjp_to_wchar,
	mb_wchar_to_eucjp,
	NULL
};

static const char *mbfl_encoding_eucjp2004_aliases[] = {"EUC_JP-2004", NULL};

static const struct mbfl_convert_vtbl vtbl_eucjp2004_wchar = {
	mbfl_no_encoding_eucjp2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_eucjp2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_eucjp2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_eucjp2004 = {
	mbfl_no_encoding_eucjp2004,
	"EUC-JP-2004",
	"EUC-JP",
	mbfl_encoding_eucjp2004_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_eucjp2004_wchar,
	&vtbl_wchar_eucjp2004,
	mb_eucjp2004_to_wchar,
	mb_wchar_to_eucjp2004,
	NULL
};

static const char *mbfl_encoding_eucjp_win_aliases[] = {"eucJP-open", "eucJP-ms", NULL};

static const struct mbfl_convert_vtbl vtbl_eucjpwin_wchar = {
	mbfl_no_encoding_eucjp_win,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_eucjpwin_wchar,
	mbfl_filt_conv_eucjpwin_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_eucjpwin = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_eucjp_win,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_eucjpwin,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_eucjp_win = {
	mbfl_no_encoding_eucjp_win,
	"eucJP-win",
	"EUC-JP",
	mbfl_encoding_eucjp_win_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_eucjpwin_wchar,
	&vtbl_wchar_eucjpwin,
	mb_eucjpwin_to_wchar,
	mb_wchar_to_eucjpwin,
	NULL
};

static const char *mbfl_encoding_cp51932_aliases[] = {"cp51932", NULL};

static const struct mbfl_convert_vtbl vtbl_cp51932_wchar = {
	mbfl_no_encoding_cp51932,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp51932_wchar,
	mbfl_filt_conv_cp51932_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp51932 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp51932,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp51932,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_cp51932 = {
	mbfl_no_encoding_cp51932,
	"CP51932",
	"CP51932",
	mbfl_encoding_cp51932_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_cp51932_wchar,
	&vtbl_wchar_cp51932,
	mb_cp51932_to_wchar,
	mb_wchar_to_cp51932,
	NULL
};

static const unsigned char mblen_table_euccn[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_euc_cn_aliases[] = {"CN-GB", "EUC_CN", "eucCN", "x-euc-cn", "gb2312", NULL};

static const struct mbfl_convert_vtbl vtbl_euccn_wchar = {
	mbfl_no_encoding_euc_cn,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_euccn_wchar,
	mbfl_filt_conv_euccn_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_euccn = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_cn,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_euccn,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_euc_cn = {
	mbfl_no_encoding_euc_cn,
	"EUC-CN",
	"CN-GB",
	mbfl_encoding_euc_cn_aliases,
	mblen_table_euccn,
	0,
	&vtbl_euccn_wchar,
	&vtbl_wchar_euccn,
	mb_euccn_to_wchar,
	mb_wchar_to_euccn,
	NULL
};

static const char *mbfl_encoding_euc_tw_aliases[] = {"EUC_TW", "eucTW", "x-euc-tw", NULL};

static const struct mbfl_convert_vtbl vtbl_euctw_wchar = {
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_euctw_wchar,
	mbfl_filt_conv_euctw_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_euctw = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_tw,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_euctw,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_euc_tw = {
	mbfl_no_encoding_euc_tw,
	"EUC-TW",
	"EUC-TW",
	mbfl_encoding_euc_tw_aliases,
	mblen_table_euccn,
	0,
	&vtbl_euctw_wchar,
	&vtbl_wchar_euctw,
	mb_euctw_to_wchar,
	mb_wchar_to_euctw,
	NULL
};

static const char *mbfl_encoding_euc_kr_aliases[] = {"EUC_KR", "eucKR", "x-euc-kr", NULL};

static const struct mbfl_convert_vtbl vtbl_euckr_wchar = {
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_euckr_wchar,
	mbfl_filt_conv_euckr_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_euckr = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_kr,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_euckr,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_euc_kr = {
	mbfl_no_encoding_euc_kr,
	"EUC-KR",
	"EUC-KR",
	mbfl_encoding_euc_kr_aliases,
	mblen_table_euccn,
	0,
	&vtbl_euckr_wchar,
	&vtbl_wchar_euckr,
	mb_euckr_to_wchar,
	mb_wchar_to_euckr,
	NULL
};

/* UHC was introduced by MicroSoft in Windows 95, and is also known as CP949.
 * It is the same as EUC-KR, but with 8,822 additional characters added to
 * complete all the characters in the Johab charset. */

static const unsigned char mblen_table_81_to_fe[] = { /* 0x81-0xFE */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_uhc_aliases[] = {"CP949", NULL};

static const struct mbfl_convert_vtbl vtbl_uhc_wchar = {
	mbfl_no_encoding_uhc,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_uhc_wchar,
	mbfl_filt_conv_uhc_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_uhc = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_uhc,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_uhc,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_uhc = {
	mbfl_no_encoding_uhc,
	"UHC",
	"UHC",
	mbfl_encoding_uhc_aliases,
	mblen_table_81_to_fe,
	0,
	&vtbl_uhc_wchar,
	&vtbl_wchar_uhc,
	mb_uhc_to_wchar,
	mb_wchar_to_uhc,
	NULL
};

/*
 * GB18030/CP936
 */

static int mbfl_filt_conv_gb18030_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, c3, w = -1;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0x80 && c < 0xff) { /* dbcs/qbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs/qbcs second byte */
		c1 = filter->cache;
		filter->status = 0;

		if (c1 >= 0x81 && c1 <= 0x84 && c >= 0x30 && c <= 0x39) {
			/* 4 byte range: Unicode BMP */
			filter->status = 2;
			filter->cache = (c1 << 8) | c;
			return 0;
		} else if (c1 >= 0x90 && c1 <= 0xe3 && c >= 0x30 && c <= 0x39) {
			/* 4 byte range: Unicode 16 planes */
			filter->status = 2;
			filter->cache = (c1 << 8) | c;
			return 0;
		} else if (((c1 >= 0xaa && c1 <= 0xaf) || (c1 >= 0xf8 && c1 <= 0xfe)) && (c >= 0xa1 && c <= 0xfe)) {
			/* UDA part 1,2: U+E000-U+E4C5 */
			w = 94*(c1 >= 0xf8 ? c1 - 0xf2 : c1 - 0xaa) + (c - 0xa1) + 0xe000;
			CK((*filter->output_function)(w, filter->data));
		} else if (c1 >= 0xa1 && c1 <= 0xa7 && c >= 0x40 && c < 0xa1 && c != 0x7f) {
			/* UDA part3 : U+E4C6-U+E765*/
			w = 96*(c1 - 0xa1) + c - (c >= 0x80 ? 0x41 : 0x40) + 0xe4c6;
			CK((*filter->output_function)(w, filter->data));
		}

		c2 = (c1 << 8) | c;

		if (w <= 0 &&
			((c2 >= 0xa2ab && c2 <= 0xa9f0 + (0xe80f-0xe801)) ||
			 (c2 >= 0xd7fa && c2 <= 0xd7fa + (0xe814-0xe810)) ||
			 (c2 >= 0xfe50 && c2 <= 0xfe80 + (0xe864-0xe844)))) {
			for (k = 0; k < mbfl_gb18030_pua_tbl_max; k++) {
				if (c2 >= mbfl_gb18030_pua_tbl[k][2] && c2 <= mbfl_gb18030_pua_tbl[k][2] + mbfl_gb18030_pua_tbl[k][1] - mbfl_gb18030_pua_tbl[k][0]) {
					w = c2 - mbfl_gb18030_pua_tbl[k][2] + mbfl_gb18030_pua_tbl[k][0];
					CK((*filter->output_function)(w, filter->data));
					break;
				}
			}
		}

		if (w <= 0) {
			if ((c1 >= 0xa1 && c1 <= 0xa9 && c >= 0xa1 && c <= 0xfe) ||
				(c1 >= 0xb0 && c1 <= 0xf7 && c >= 0xa1 && c <= 0xfe) ||
				(c1 >= 0x81 && c1 <= 0xa0 && c >= 0x40 && c <= 0xfe && c != 0x7f) ||
				(c1 >= 0xaa && c1 <= 0xfe && c >= 0x40 && c <= 0xa0 && c != 0x7f) ||
				(c1 >= 0xa8 && c1 <= 0xa9 && c >= 0x40 && c <= 0xa0 && c != 0x7f)) {
				w = (c1 - 0x81)*192 + c - 0x40;
				ZEND_ASSERT(w < cp936_ucs_table_size);
				CK((*filter->output_function)(cp936_ucs_table[w], filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

	case 2: /* qbcs third byte */
		c1 = (filter->cache >> 8) & 0xff;
		c2 = filter->cache & 0xff;
		filter->status = filter->cache = 0;
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) && c2 >= 0x30 && c2 <= 0x39 && c >= 0x81 && c <= 0xfe) {
			filter->cache = (c1 << 16) | (c2 << 8) | c;
			filter->status = 3;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* qbcs fourth byte */
		c1 = (filter->cache >> 16) & 0xff;
		c2 = (filter->cache >> 8) & 0xff;
		c3 = filter->cache & 0xff;
		filter->status = filter->cache = 0;
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) && c2 >= 0x30 && c2 <= 0x39 && c3 >= 0x81 && c3 <= 0xfe && c >= 0x30 && c <= 0x39) {
			if (c1 >= 0x90 && c1 <= 0xe3) {
				w = ((((c1 - 0x90)*10 + (c2 - 0x30))*126 + (c3 - 0x81)))*10 + (c - 0x30) + 0x10000;
				if (w > 0x10FFFF) {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
					return 0;
				}
			} else { /* Unicode BMP */
				w = (((c1 - 0x81)*10 + (c2 - 0x30))*126 + (c3 - 0x81))*10 + (c - 0x30);
				if (w >= 0 && w <= 39419) {
					k = mbfl_bisec_srch(w, mbfl_gb2uni_tbl, mbfl_gb_uni_max);
					w += mbfl_gb_uni_ofst[k];
				} else {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
					return 0;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_gb18030_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* multi-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_gb18030(int c, mbfl_convert_filter *filter)
{
	int k, k1, k2;
	int c1, s = 0, s1 = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		if (c == 0x01f9) {
			s = 0xa8bf;
		} else {
			s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
		}
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		if (c == 0x20ac) { /* euro-sign */
			s = 0xa2e3;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= ucs_ci_cp936_table_min && c < ucs_ci_cp936_table_max) {
		/* U+F900-FA2F CJK Compatibility Ideographs */
		if (c == 0xf92c) {
			s = 0xfd9c;
		} else if (c == 0xf979) {
			s = 0xfd9d;
		} else if (c == 0xf995) {
			s = 0xfd9e;
		} else if (c == 0xf9e7) {
			s = 0xfd9f;
		} else if (c == 0xf9f1) {
			s = 0xfda0;
		} else if (c >= 0xfa0c && c <= 0xfa29) {
			s = ucs_ci_s_cp936_table[c - 0xfa0c];
		}
	} else if (c >= ucs_cf_cp936_table_min && c < ucs_cf_cp936_table_max) {
		/* FE30h CJK Compatibility Forms  */
		s = ucs_cf_cp936_table[c - ucs_cf_cp936_table_min];
	} else if (c >= ucs_sfv_cp936_table_min && c < ucs_sfv_cp936_table_max) {
		/* U+FE50-FE6F Small Form Variants */
		s = ucs_sfv_cp936_table[c - ucs_sfv_cp936_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		/* U+FF00-FFFF HW/FW Forms */
		if (c == 0xff04) {
			s = 0xa1e7;
		} else if (c == 0xff5e) {
			s = 0xa1ab;
		} else if (c >= 0xff01 && c <= 0xff5d) {
			s = c - 0xff01 + 0xa3a1;
		} else if (c >= 0xffe0 && c <= 0xffe5) {
			s = ucs_hff_s_cp936_table[c-0xffe0];
		}
	}

	/* While GB18030 and CP936 are very similar, some mappings are different between these encodings;
	 * do a binary search in a table of differing codepoints to see if we have one */
	if (s <= 0 && c >= mbfl_gb18030_c_tbl_key[0] && c <= mbfl_gb18030_c_tbl_key[mbfl_gb18030_c_tbl_max-1]) {
		k1 = mbfl_bisec_srch2(c, mbfl_gb18030_c_tbl_key, mbfl_gb18030_c_tbl_max);
		if (k1 >= 0) {
			s = mbfl_gb18030_c_tbl_val[k1];
		}
	}

	if (c >= 0xe000 && c <= 0xe864) { /* PUA */
		if (c < 0xe766) {
			if (c < 0xe4c6) {
				c1 = c - 0xe000;
				s = (c1 % 94) + 0xa1;
				c1 /= 94;
				s |= (c1 < 0x06 ? c1 + 0xaa : c1 + 0xf2) << 8;
			} else {
				c1 = c - 0xe4c6;
				s = ((c1 / 96) + 0xa1) << 8;
				c1 %= 96;
				s |= c1 + (c1 >= 0x3f ? 0x41 : 0x40);
			}
		} else {
			/* U+E766..U+E864 */
			k1 = 0;
			k2 = mbfl_gb18030_pua_tbl_max;
			while (k1 < k2) {
				k = (k1 + k2) >> 1;
				if (c < mbfl_gb18030_pua_tbl[k][0]) {
					k2 = k;
				} else if (c > mbfl_gb18030_pua_tbl[k][1]) {
					k1 = k + 1;
				} else {
					s = c - mbfl_gb18030_pua_tbl[k][0] + mbfl_gb18030_pua_tbl[k][2];
					break;
				}
			}
		}
	}

	/* If we have not yet found a suitable mapping for this codepoint, it requires a 4-byte code */
	if (s <= 0 && c >= 0x0080 && c <= 0xffff) {
		/* BMP */
		s = mbfl_bisec_srch(c, mbfl_uni2gb_tbl, mbfl_gb_uni_max);
		if (s >= 0) {
			c1 = c - mbfl_gb_uni_ofst[s];
			s = (c1 % 10) + 0x30;
			c1 /= 10;
			s |= ((c1 % 126) + 0x81) << 8;
			c1 /= 126;
			s |= ((c1 % 10) + 0x30) << 16;
			c1 /= 10;
			s1 = c1 + 0x81;
		}
	} else if (c >= 0x10000 && c <= 0x10ffff) {
		/* Code set 3: Unicode U+10000..U+10FFFF */
		c1 = c - 0x10000;
		s = (c1 % 10) + 0x30;
		c1 /= 10;
		s |= ((c1 % 126) + 0x81) << 8;
		c1 /= 126;
		s |= ((c1 % 10) + 0x30) << 16;
		c1 /= 10;
		s1 = c1 + 0x90;
	}

	if (c == 0) {
		s = 0;
	} else if (s == 0) {
		s = -1;
	}

	if (s >= 0) {
		if (s <= 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else if (s1 > 0) { /* qbcs */
			CK((*filter->output_function)(s1 & 0xff, filter->data));
			CK((*filter->output_function)((s >> 16) & 0xff, filter->data));
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else { /* dbcs */
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static const unsigned short gb18030_pua_tbl3[] = {
	/* 0xFE50 */
	0x0000,0xE816,0xE817,0xE818,0x0000,0x0000,0x0000,0x0000,
	0x0000,0xE81E,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0xE826,0x0000,0x0000,0x0000,0x0000,0xE82B,0xE82C,
	0x0000,0x0000,0x0000,0x0000,0xE831,0xE832,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xE83B,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xE843,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0xE854,0xE855,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	/* 0xFEA0 */
	0xE864
};

static size_t mb_gb18030_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c == 0x80 || c == 0xFF) {
			*out++ = MBFL_BAD_INPUT;
		} else {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (((c >= 0x81 && c <= 0x84) || (c >= 0x90 && c <= 0xE3)) && c2 >= 0x30 && c2 <= 0x39) {
				if (p >= e) {
					*out++ = MBFL_BAD_INPUT;
					break;
				}
				unsigned char c3 = *p++;

				if (c3 >= 0x81 && c3 <= 0xFE && p < e) {
					unsigned char c4 = *p++;

					if (c4 >= 0x30 && c4 <= 0x39) {
						if (c >= 0x90 && c <= 0xE3) {
							unsigned int w = ((((c - 0x90)*10 + (c2 - 0x30))*126 + (c3 - 0x81)))*10 + (c4 - 0x30) + 0x10000;
							*out++ = (w > 0x10FFFF) ? MBFL_BAD_INPUT : w;
						} else {
							/* Unicode BMP */
							unsigned int w = (((c - 0x81)*10 + (c2 - 0x30))*126 + (c3 - 0x81))*10 + (c4 - 0x30);
							if (w <= 39419) {
								*out++ = w + mbfl_gb_uni_ofst[mbfl_bisec_srch(w, mbfl_gb2uni_tbl, mbfl_gb_uni_max)];
							} else {
								*out++ = MBFL_BAD_INPUT;
							}
						}
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (((c >= 0xAA && c <= 0xAF) || (c >= 0xF8 && c <= 0xFE)) && (c2 >= 0xA1 && c2 <= 0xFE)) {
				/* UDA part 1, 2: U+E000-U+E4C5 */
				*out++ = 94*(c >= 0xF8 ? c - 0xF2 : c - 0xAA) + (c2 - 0xA1) + 0xE000;
			} else if (c >= 0xA1 && c <= 0xA7 && c2 >= 0x40 && c2 < 0xA1 && c2 != 0x7F) {
				/* UDA part 3: U+E4C6-U+E765 */
				*out++ = 96*(c - 0xA1) + c2 - (c2 >= 0x80 ? 0x41 : 0x40) + 0xE4C6;
			} else if (c2 >= 0x40 && c2 != 0x7F && c2 != 0xFF) {
				unsigned int w = (c - 0x81)*192 + c2 - 0x40;

				if (w >= 0x192B) {
					if (w <= 0x1EBE) {
						if (w != 0x1963 && w != 0x1DBF && (w < 0x1E49 || w > 0x1E55) && w != 0x1E7F) {
							*out++ = cp936_pua_tbl1[w - 0x192B];
							continue;
						}
					} else if (w >= 0x413A) {
						if (w <= 0x413E) {
							*out++ = cp936_pua_tbl2[w - 0x413A];
							continue;
						} else if (w >= 0x5DD0 && w <= 0x5E20) {
							unsigned int c = gb18030_pua_tbl3[w - 0x5DD0];
							if (c) {
								*out++ = c;
								continue;
							}
						}
					}
				}

				if ((c >= 0x81 && c <= 0xA9) || (c >= 0xB0 && c <= 0xF7 && c2 >= 0xA1) || (c >= 0xAA && c <= 0xFE && c2 <= 0xA0)) {
					ZEND_ASSERT(w < cp936_ucs_table_size);
					*out++ = cp936_ucs_table[w];
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_gb18030(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		} else if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			if (w == 0x1F9) {
				s = 0xA8Bf;
			} else {
				s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
			}
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			if (w == 0x20AC) { /* Euro sign */
				s = 0xA2E3;
			} else {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
		} else if (w >= ucs_i_cp936_table_min && w < ucs_i_cp936_table_max) {
			s = ucs_i_cp936_table[w - ucs_i_cp936_table_min];
		} else if (w >= ucs_ci_cp936_table_min && w < ucs_ci_cp936_table_max) {
			/* U+F900-U+FA2F CJK Compatibility Ideographs */
			if (w == 0xF92C) {
				s = 0xFD9C;
			} else if (w == 0xF979) {
				s = 0xFD9D;
			} else if (w == 0xF995) {
				s = 0xFD9E;
			} else if (w == 0xF9E7) {
				s = 0xFD9F;
			} else if (w == 0xF9F1) {
				s = 0xFDA0;
			} else if (w >= 0xFA0C && w <= 0xFA29) {
				s = ucs_ci_s_cp936_table[w - 0xFA0C];
			}
		} else if (w >= ucs_cf_cp936_table_min && w < ucs_cf_cp936_table_max) {
			/* CJK Compatibility Forms  */
			s = ucs_cf_cp936_table[w - ucs_cf_cp936_table_min];
		} else if (w >= ucs_sfv_cp936_table_min && w < ucs_sfv_cp936_table_max) {
			/* U+FE50-U+FE6F Small Form Variants */
			s = ucs_sfv_cp936_table[w - ucs_sfv_cp936_table_min];
		} else if (w >= ucs_hff_cp936_table_min && w < ucs_hff_cp936_table_max) {
			/* U+FF00-U+FFFF HW/FW Forms */
			if (w == 0xFF04) {
				s = 0xA1E7;
			} else if (w == 0xFF5E) {
				s = 0xA1AB;
			} else if (w >= 0xFF01 && w <= 0xFF5D) {
				s = w - 0xFF01 + 0xA3A1;
			} else if (w >= 0xFFE0 && w <= 0xFFE5) {
				s = ucs_hff_s_cp936_table[w - 0xFFE0];
			}
		} else if (w >= 0xE000 && w <= 0xE864) {
			/* PUA */
			if (w < 0xE766) {
				if (w < 0xE4C6) {
					unsigned int c1 = w - 0xE000;
					s = (c1 % 94) + 0xA1;
					c1 /= 94;
					s |= (c1 + (c1 < 0x06 ? 0xAA : 0xF2)) << 8;
				} else {
					unsigned int c1 = w - 0xE4C6;
					s = ((c1 / 96) + 0xA1) << 8;
					c1 %= 96;
					s |= c1 + (c1 >= 0x3F ? 0x41 : 0x40);
				}
			} else {
				/* U+E766-U+E864 */
				unsigned int k1 = 0, k2 = mbfl_gb18030_pua_tbl_max;
				while (k1 < k2) {
					unsigned int k = (k1 + k2) >> 1;
					if (w < mbfl_gb18030_pua_tbl[k][0]) {
						k2 = k;
					} else if (w > mbfl_gb18030_pua_tbl[k][1]) {
						k1 = k + 1;
					} else {
						s = w - mbfl_gb18030_pua_tbl[k][0] + mbfl_gb18030_pua_tbl[k][2];
						break;
					}
				}
			}
		}

		/* While GB18030 and CP936 are very similar, some mappings are different between these encodings;
		 * do a binary search in a table of differing codepoints to see if we have one */
		if (!s && w >= mbfl_gb18030_c_tbl_key[0] && w <= mbfl_gb18030_c_tbl_key[mbfl_gb18030_c_tbl_max-1]) {
			int i = mbfl_bisec_srch2(w, mbfl_gb18030_c_tbl_key, mbfl_gb18030_c_tbl_max);
			if (i >= 0) {
				s = mbfl_gb18030_c_tbl_val[i];
			}
		}

		/* If we have not yet found a suitable mapping for this codepoint, it requires a 4-byte code */
		if (!s && w >= 0x80 && w <= 0xFFFF) {
			/* BMP */
			int i = mbfl_bisec_srch(w, mbfl_uni2gb_tbl, mbfl_gb_uni_max);
			if (i >= 0) {
				unsigned int c1 = w - mbfl_gb_uni_ofst[i];
				s = (c1 % 10) + 0x30;
				c1 /= 10;
				s |= ((c1 % 126) + 0x81) << 8;
				c1 /= 126;
				s |= ((c1 % 10) + 0x30) << 16;
				c1 /= 10;
				s |= (c1 + 0x81) << 24;
			}
		} else if (w >= 0x10000 && w <= 0x10FFFF) {
			/* Code set 3: Unicode U+10000-U+10FFFF */
			unsigned int c1 = w - 0x10000;
			s = (c1 % 10) + 0x30;
			c1 /= 10;
			s |= ((c1 % 126) + 0x81) << 8;
			c1 /= 126;
			s |= ((c1 % 10) + 0x30) << 16;
			c1 /= 10;
			s |= (c1 + 0x90) << 24;
		}

		if (!s) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_gb18030);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s > 0xFFFFFF) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			out = mb_convert_buf_add4(out, (s >> 24) & 0xFF, (s >> 16) & 0xFF, (s >> 8) & 0xFF, s & 0xFF);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static int mbfl_filt_conv_cp936_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, w = -1;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c == 0x80) {	/* euro sign */
			CK((*filter->output_function)(0x20ac, filter->data));
		} else if (c < 0xff) {	/* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else { /* 0xff */
			CK((*filter->output_function)(0xf8f5, filter->data));
		}
		break;

	case 1:		/* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;

		if (((c1 >= 0xaa && c1 <= 0xaf) || (c1 >= 0xf8 && c1 <= 0xfe)) &&
			(c >= 0xa1 && c <= 0xfe)) {
			/* UDA part1,2: U+E000-U+E4C5 */
			w = 94*(c1 >= 0xf8 ? c1 - 0xf2 : c1 - 0xaa) + (c - 0xa1) + 0xe000;
			CK((*filter->output_function)(w, filter->data));
		} else if (c1 >= 0xa1 && c1 <= 0xa7 && c >= 0x40 && c < 0xa1 && c != 0x7f) {
			/* UDA part3 : U+E4C6-U+E765*/
			w = 96*(c1 - 0xa1) + c - (c >= 0x80 ? 0x41 : 0x40) + 0xe4c6;
			CK((*filter->output_function)(w, filter->data));
		}

		c2 = (c1 << 8) | c;

		if (w <= 0 &&
			((c2 >= 0xa2ab && c2 <= 0xa9f0 + (0xe80f-0xe801)) ||
			 (c2 >= 0xd7fa && c2 <= 0xd7fa + (0xe814-0xe810)) ||
			 (c2 >= 0xfe50 && c2 <= 0xfe80 + (0xe864-0xe844)))) {
			for (k = 0; k < mbfl_cp936_pua_tbl_max; k++) {
				if (c2 >= mbfl_cp936_pua_tbl[k][2] &&
					c2 <= mbfl_cp936_pua_tbl[k][2] +
					mbfl_cp936_pua_tbl[k][1] -  mbfl_cp936_pua_tbl[k][0]) {
					w = c2 -  mbfl_cp936_pua_tbl[k][2] + mbfl_cp936_pua_tbl[k][0];
					CK((*filter->output_function)(w, filter->data));
					break;
				}
			}
		}

		if (w <= 0) {
			if (c1 < 0xff && c1 > 0x80 && c >= 0x40 && c < 0xff && c != 0x7f) {
				w = (c1 - 0x81)*192 + c - 0x40;
				ZEND_ASSERT(w < cp936_ucs_table_size);
				CK((*filter->output_function)(cp936_ucs_table[w], filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp936_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp936(int c, mbfl_convert_filter *filter)
{
	int k, k1, k2;
	int c1, s = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		/* U+0000 - U+0451 */
		s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		/* U+2000 - U+26FF */
		if (c == 0x203e) {
			s = 0xa3fe;
		} else if (c == 0x2218) {
			s = 0xa1e3;
		} else if (c == 0x223c) {
			s = 0xa1ab;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		/* U+2F00 - U+33FF */
		s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		/* U+4D00-9FFF CJK Unified Ideographs (+ Extension A) */
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= 0xe000 && c <= 0xe864) { /* PUA */
		if (c < 0xe766) {
			if (c < 0xe4c6) {
				c1 = c - 0xe000;
				s = (c1 % 94) + 0xa1; c1 /= 94;
				s |= (c1 < 0x06 ? c1 + 0xaa : c1 + 0xf2) << 8;
			} else {
				c1 = c - 0xe4c6;
				s = ((c1 / 96) + 0xa1) << 8; c1 %= 96;
				s |= c1 + (c1 >= 0x3f ? 0x41 : 0x40);
			}
		} else {
			/* U+E766..U+E864 */
			k1 = 0; k2 = mbfl_cp936_pua_tbl_max;
			while (k1 < k2) {
				k = (k1 + k2) >> 1;
				if (c < mbfl_cp936_pua_tbl[k][0]) {
					k2 = k;
				} else if (c > mbfl_cp936_pua_tbl[k][1]) {
					k1 = k + 1;
				} else {
					s = c - mbfl_cp936_pua_tbl[k][0] + mbfl_cp936_pua_tbl[k][2];
					break;
				}
			}
		}
	} else if (c == 0xf8f5) {
		s = 0xff;
	} else if (c >= ucs_ci_cp936_table_min && c < ucs_ci_cp936_table_max) {
		/* U+F900-FA2F CJK Compatibility Ideographs */
		s = ucs_ci_cp936_table[c - ucs_ci_cp936_table_min];
	} else if (c >= ucs_cf_cp936_table_min && c < ucs_cf_cp936_table_max) {
		s = ucs_cf_cp936_table[c - ucs_cf_cp936_table_min];
	} else if (c >= ucs_sfv_cp936_table_min && c < ucs_sfv_cp936_table_max) {
		s = ucs_sfv_cp936_table[c - ucs_sfv_cp936_table_min]; /* U+FE50-FE6F Small Form Variants */
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		/* U+FF00-FFFF HW/FW Forms */
		if (c == 0xff04) {
			s = 0xa1e7;
		} else if (c == 0xff5e) {
			s = 0xa1ab;
		} else if (c >= 0xff01 && c <= 0xff5d) {
			s = c - 0xff01 + 0xa3a1;
		} else if (c >= 0xffe0 && c <= 0xffe5) {
			s = ucs_hff_s_cp936_table[c-0xffe0];
		}
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s <= 0x80 || s == 0xff) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_cp936_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c == 0x80) {
			*out++ = 0x20AC; /* Euro sign */
		} else if (c < 0xFF) {
			if (p >= e) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			unsigned char c2 = *p++;
			if (c2 < 0x40 || c2 == 0x7F || c2 == 0xFF) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			if (((c >= 0xAA && c <= 0xAF) || (c >= 0xF8 && c <= 0xFE)) && c2 >= 0xA1) {
				/* UDA part 1, 2: U+E000-U+E4C5 */
				*out++ = 94*(c >= 0xF8 ? c - 0xF2 : c - 0xAA) + (c2 - 0xA1) + 0xE000;
			} else if (c >= 0xA1 && c <= 0xA7 && c2 < 0xA1) {
				/* UDA part 3: U+E4C6-U+E765*/
				*out++ = 96*(c - 0xA1) + c2 - (c2 >= 0x80 ? 0x41 : 0x40) + 0xE4C6;
			} else {
				unsigned int w = (c - 0x81)*192 + c2 - 0x40; /* Convert c, c2 into GB 2312 table lookup index */

				/* For CP936 and GB18030, certain GB 2312 byte combinations are mapped to PUA codepoints,
				 * whereas the same combinations aren't mapped to any codepoint for HZ and EUC-CN
				 * To avoid duplicating the entire GB 2312 -> Unicode lookup table, we have three
				 * auxiliary tables which are consulted instead for specific ranges of lookup indices */
				if (w >= 0x192B) {
					if (w <= 0x1EBE) {
						*out++ = cp936_pua_tbl1[w - 0x192B];
						continue;
					} else if (w >= 0x413A) {
						if (w <= 0x413E) {
							*out++ = cp936_pua_tbl2[w - 0x413A];
							continue;
						} else if (w >= 0x5DD0 && w <= 0x5E20) {
							*out++ = cp936_pua_tbl3[w - 0x5DD0];
							continue;
						}
					}
				}

				ZEND_ASSERT(w < cp936_ucs_table_size);
				*out++ = cp936_ucs_table[w];
			}
		} else {
			*out++ = 0xF8F5;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_cp936(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			/* U+0000-U+0451 */
			s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			/* U+2000-U+26FF */
			if (w == 0x203E) {
				s = 0xA3FE;
			} else if (w == 0x2218) {
				s = 0xA1E3;
			} else if (w == 0x223C) {
				s = 0xA1AB;
			} else {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			/* U+2F00-U+33FF */
			s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
		} else if (w >= ucs_i_cp936_table_min && w < ucs_i_cp936_table_max) {
			/* U+4D00-9FFF CJK Unified Ideographs (+ Extension A) */
			s = ucs_i_cp936_table[w - ucs_i_cp936_table_min];
		} else if (w >= 0xE000 && w <= 0xE864) {
			/* PUA */
			if (w < 0xe766) {
				if (w < 0xe4c6) {
					unsigned int c1 = w - 0xE000;
					s = (c1 % 94) + 0xA1;
					c1 /= 94;
					s |= (c1 < 0x6 ? c1 + 0xAA : c1 + 0xF2) << 8;
				} else {
					unsigned int c1 = w - 0xE4C6;
					s = ((c1 / 96) + 0xA1) << 8;
					c1 %= 96;
					s |= c1 + (c1 >= 0x3F ? 0x41 : 0x40);
				}
			} else {
				/* U+E766-U+E864 */
				unsigned int k1 = 0;
				unsigned int k2 = mbfl_cp936_pua_tbl_max;
				while (k1 < k2) {
					int k = (k1 + k2) >> 1;
					if (w < mbfl_cp936_pua_tbl[k][0]) {
						k2 = k;
					} else if (w > mbfl_cp936_pua_tbl[k][1]) {
						k1 = k + 1;
					} else {
						s = w - mbfl_cp936_pua_tbl[k][0] + mbfl_cp936_pua_tbl[k][2];
						break;
					}
				}
			}
		} else if (w == 0xF8F5) {
			s = 0xFF;
		} else if (w >= ucs_ci_cp936_table_min && w < ucs_ci_cp936_table_max) {
			/* U+F900-U+FA2F CJK Compatibility Ideographs */
			s = ucs_ci_cp936_table[w - ucs_ci_cp936_table_min];
		} else if (w >= ucs_cf_cp936_table_min && w < ucs_cf_cp936_table_max) {
			s = ucs_cf_cp936_table[w - ucs_cf_cp936_table_min];
		} else if (w >= ucs_sfv_cp936_table_min && w < ucs_sfv_cp936_table_max) {
			/* U+FE50-U+FE6F Small Form Variants */
			s = ucs_sfv_cp936_table[w - ucs_sfv_cp936_table_min];
		} else if (w >= ucs_hff_cp936_table_min && w < ucs_hff_cp936_table_max) {
			/* U+FF00-U+FFFF HW/FW Forms */
			if (w == 0xFF04) {
				s = 0xA1E7;
			} else if (w == 0xFF5E) {
				s = 0xA1AB;
			} else if (w >= 0xFF01 && w <= 0xFF5D) {
				s = w - 0xFF01 + 0xA3A1;
			} else if (w >= 0xFFE0 && w <= 0xFFE5) {
				s = ucs_hff_s_cp936_table[w - 0xFFE0];
			}
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp936);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			}
		} else if (s <= 0x80 || s == 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static const char *mbfl_encoding_gb18030_aliases[] = {"gb-18030", "gb-18030-2000", NULL};

static const struct mbfl_convert_vtbl vtbl_gb18030_wchar = {
	mbfl_no_encoding_gb18030,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_gb18030_wchar,
	mbfl_filt_conv_gb18030_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_gb18030 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_gb18030,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_gb18030,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_gb18030 = {
	mbfl_no_encoding_gb18030,
	"GB18030",
	"GB18030",
	mbfl_encoding_gb18030_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_gb18030_wchar,
	&vtbl_wchar_gb18030,
	mb_gb18030_to_wchar,
	mb_wchar_to_gb18030,
	NULL
};

static const char *mbfl_encoding_cp936_aliases[] = {"CP-936", "GBK", NULL};

static const struct mbfl_convert_vtbl vtbl_cp936_wchar = {
	mbfl_no_encoding_cp936,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp936_wchar,
	mbfl_filt_conv_cp936_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp936 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp936,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp936,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_cp936 = {
	mbfl_no_encoding_cp936,
	"CP936",
	"CP936",
	mbfl_encoding_cp936_aliases,
	mblen_table_81_to_fe,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp936_wchar,
	&vtbl_wchar_cp936,
	mb_cp936_to_wchar,
	mb_wchar_to_cp936,
	NULL
};

/*
 * BIG5/CP950
 */

/* 63 + 94 = 157 or 94 */
static unsigned short cp950_pua_tbl[][4] = {
	{0xe000, 0xe310, 0xfa40, 0xfefe},
	{0xe311, 0xeeb7, 0x8e40, 0xa0fe},
	{0xeeb8, 0xf6b0, 0x8140, 0x8dfe},
	{0xf6b1, 0xf70e, 0xc6a1, 0xc6fe},
	{0xf70f, 0xf848, 0xc740, 0xc8fe},
};

static inline int is_in_cp950_pua(int c1, int c)
{
	if ((c1 >= 0xfa && c1 <= 0xfe) || (c1 >= 0x8e && c1 <= 0xa0) || (c1 >= 0x81 && c1 <= 0x8d) || (c1 >= 0xc7 && c1 <= 0xc8)) {
		return (c >= 0x40 && c <= 0x7e) || (c >= 0xa1 && c <= 0xfe);
	} else if (c1 == 0xc6) {
		return c >= 0xa1 && c <= 0xfe;
	}
	return 0;
}

static int mbfl_filt_conv_big5_wchar(int c, mbfl_convert_filter *filter)
{
	int k, c1, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (filter->from->no_encoding != mbfl_no_encoding_cp950 && c > 0xA0 && c <= 0xF9 && c != 0xC8) {
			filter->status = 1;
			filter->cache = c;
		} else if (filter->from->no_encoding == mbfl_no_encoding_cp950 && c > 0x80 && c <= 0xFE) {
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		if ((c > 0x3f && c < 0x7f) || (c > 0xa0 && c < 0xff)) {
			if (c < 0x7f) {
				w = (c1 - 0xa1)*157 + (c - 0x40);
			} else {
				w = (c1 - 0xa1)*157 + (c - 0xa1) + 0x3f;
			}
			if (w >= 0 && w < big5_ucs_table_size) {
				w = big5_ucs_table[w];
			} else {
				w = 0;
			}

			if (filter->from->no_encoding == mbfl_no_encoding_cp950) {
				/* PUA for CP950 */
				if (is_in_cp950_pua(c1, c)) {
					int c2 = (c1 << 8) | c;

					for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
						if (c2 >= cp950_pua_tbl[k][2] && c2 <= cp950_pua_tbl[k][3]) {
							break;
						}
					}

					if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
						w = 157*(c1 - (cp950_pua_tbl[k][2]>>8)) + c - (c >= 0xa1 ? 0x62 : 0x40) + cp950_pua_tbl[k][0];
					} else {
						w = c2 - cp950_pua_tbl[k][2] + cp950_pua_tbl[k][0];
					}
				} else if (c1 == 0xA1) {
					if (c == 0x45) {
						w = 0x2027;
					} else if (c == 0x4E) {
						w = 0xFE51;
					} else if (c == 0x5A) {
						w = 0x2574;
					} else if (c == 0xC2) {
						w = 0x00AF;
					} else if (c == 0xC3) {
						w = 0xFFE3;
					} else if (c == 0xC5) {
						w = 0x02CD;
					} else if (c == 0xE3) {
						w = 0xFF5E;
					} else if (c == 0xF2) {
						w = 0x2295;
					} else if (c == 0xF3) {
						w = 0x2299;
					} else if (c == 0xFE) {
						w = 0xFF0F;
					}
				} else if (c1 == 0xA2) {
					if (c == 0x40) {
						w = 0xFF3C;
					} else if (c == 0x41) {
						w = 0x2215;
					} else if (c == 0x42) {
						w = 0xFE68;
					} else if (c == 0x46) {
						w = 0xFFE0;
					} else if (c == 0x47) {
						w = 0xFFE1;
					} else if (c == 0xCC) {
						w = 0x5341;
					} else if (c == 0xCE) {
						w = 0x5345;
					}
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_big5_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_big5(int c, mbfl_convert_filter *filter)
{
	int k, s = 0;

	if (c >= ucs_a1_big5_table_min && c < ucs_a1_big5_table_max) {
		s = ucs_a1_big5_table[c - ucs_a1_big5_table_min];
	} else if (c >= ucs_a2_big5_table_min && c < ucs_a2_big5_table_max) {
		s = ucs_a2_big5_table[c - ucs_a2_big5_table_min];
	} else if (c >= ucs_a3_big5_table_min && c < ucs_a3_big5_table_max) {
		s = ucs_a3_big5_table[c - ucs_a3_big5_table_min];
	} else if (c >= ucs_i_big5_table_min && c < ucs_i_big5_table_max) {
		s = ucs_i_big5_table[c - ucs_i_big5_table_min];
	} else if (c >= ucs_r1_big5_table_min && c < ucs_r1_big5_table_max) {
		s = ucs_r1_big5_table[c - ucs_r1_big5_table_min];
	} else if (c >= ucs_r2_big5_table_min && c < ucs_r2_big5_table_max) {
		s = ucs_r2_big5_table[c - ucs_r2_big5_table_min];
	}

	if (filter->to->no_encoding == mbfl_no_encoding_cp950) {
		if (c >= 0xe000 && c <= 0xf848) { /* PUA for CP950 */
			for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
				if (c <= cp950_pua_tbl[k][1]) {
					break;
				}
			}

			int c1 = c - cp950_pua_tbl[k][0];
			if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
				int c2 = cp950_pua_tbl[k][2] >> 8;
				s = ((c1 / 157) + c2) << 8;
				c1 %= 157;
				s |= c1 + (c1 >= 0x3f ? 0x62 : 0x40);
			} else {
				s = c1 + cp950_pua_tbl[k][2];
			}
		} else if (c == 0x00A2) {
			s = 0;
		} else if (c == 0x00A3) {
			s = 0;
		} else if (c == 0x00AF) {
			s = 0xA1C2;
		} else if (c == 0x02CD) {
			s = 0xA1C5;
		} else if (c == 0x0401) {
			s = 0;
		} else if (c >= 0x0414 && c <= 0x041C) {
			s = 0;
		} else if (c >= 0x0423 && c <= 0x044F) {
			s = 0;
		} else if (c == 0x0451) {
			s = 0;
		} else if (c == 0x2022) {
			s = 0;
		} else if (c == 0x2027) {
			s = 0xA145;
		} else if (c == 0x203E) {
			s = 0;
		} else if (c == 0x2215) {
			s = 0xA241;
		} else if (c == 0x223C) {
			s = 0;
		} else if (c == 0x2295) {
			s = 0xA1F2;
		} else if (c == 0x2299) {
			s = 0xA1F3;
		} else if (c >= 0x2460 && c <= 0x247D) {
			s = 0;
		} else if (c == 0x2574) {
			s = 0xA15A;
		} else if (c == 0x2609) {
			s = 0;
		} else if (c == 0x2641) {
			s = 0;
		} else if (c == 0x3005 || (c >= 0x302A && c <= 0x30FF)) {
			s = 0;
		} else if (c == 0xFE51) {
			s = 0xA14E;
		} else if (c == 0xFE68) {
			s = 0xA242;
		} else if (c == 0xFF3C) {
			s = 0xA240;
		} else if (c == 0xFF5E) {
			s = 0xA1E3;
		} else if (c == 0xFF64) {
			s = 0;
		} else if (c == 0xFFE0) {
			s = 0xA246;
		} else if (c == 0xFFE1) {
			s = 0xA247;
		} else if (c == 0xFFE3) {
			s = 0xA1C3;
		} else if (c == 0xFF0F) {
			s = 0xA1FE;
		}
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s <= 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_big5_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	e--; /* Stop the main loop 1 byte short of the end of the input */

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c > 0xA0 && c <= 0xF9) {
			/* We don't need to check p < e here; it's not possible that this pointer dereference
			 * will be outside the input string, because of e-- above */
			unsigned char c2 = *p++;

			if ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0xA1 && c2 <= 0xFE)) {
				unsigned int w = (c - 0xA1)*157 + c2 - ((c2 <= 0x7E) ? 0x40 : 0xA1 - 0x3F);
				ZEND_ASSERT(w < big5_ucs_table_size);
				w = big5_ucs_table[w];
				if (!w) {
					if (c == 0xC8) {
						p--;
					}
					w = MBFL_BAD_INPUT;
				}
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	/* Finish up last byte of input string if there is one */
	if (p == e && out < limit) {
		unsigned char c = *p++;
		*out++ = (c <= 0x7F) ? c : MBFL_BAD_INPUT;
	}

	*in_len = e - p + 1;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_big5(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_big5_table_min && w < ucs_a1_big5_table_max) {
			s = ucs_a1_big5_table[w - ucs_a1_big5_table_min];
		} else if (w >= ucs_a2_big5_table_min && w < ucs_a2_big5_table_max) {
			s = ucs_a2_big5_table[w - ucs_a2_big5_table_min];
		} else if (w >= ucs_a3_big5_table_min && w < ucs_a3_big5_table_max) {
			s = ucs_a3_big5_table[w - ucs_a3_big5_table_min];
		} else if (w >= ucs_i_big5_table_min && w < ucs_i_big5_table_max) {
			s = ucs_i_big5_table[w - ucs_i_big5_table_min];
		} else if (w >= ucs_r1_big5_table_min && w < ucs_r1_big5_table_max) {
			s = ucs_r1_big5_table[w - ucs_r1_big5_table_min];
		} else if (w >= ucs_r2_big5_table_min && w < ucs_r2_big5_table_max) {
			s = ucs_r2_big5_table[w - ucs_r2_big5_table_min];
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_big5);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s <= 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_cp950_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c > 0x80 && c <= 0xFE && p < e) {
			unsigned char c2 = *p++;

			if ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0xA1 && c2 <= 0xFE)) {
				unsigned int w = ((c - 0xA1)*157) + c2 - ((c2 <= 0x7E) ? 0x40 : 0xA1 - 0x3F);
				w = (w < big5_ucs_table_size) ? big5_ucs_table[w] : 0;

				/* PUA for CP950 */
				if (is_in_cp950_pua(c, c2)) {
					unsigned int s = (c << 8) | c2;

					int k;
					for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
						if (s >= cp950_pua_tbl[k][2] && s <= cp950_pua_tbl[k][3]) {
							break;
						}
					}

					if ((cp950_pua_tbl[k][2] & 0xFF) == 0x40) {
						w = 157*(c - (cp950_pua_tbl[k][2] >> 8)) + c2 - (c2 >= 0xA1 ? 0x62 : 0x40) + cp950_pua_tbl[k][0];
					} else {
						w = s - cp950_pua_tbl[k][2] + cp950_pua_tbl[k][0];
					}
				} else if (c == 0xA1) {
					if (c2 == 0x45) {
						w = 0x2027;
					} else if (c2 == 0x4E) {
						w = 0xFE51;
					} else if (c2 == 0x5A) {
						w = 0x2574;
					} else if (c2 == 0xC2) {
						w = 0x00AF;
					} else if (c2 == 0xC3) {
						w = 0xFFE3;
					} else if (c2 == 0xC5) {
						w = 0x02CD;
					} else if (c2 == 0xE3) {
						w = 0xFF5E;
					} else if (c2 == 0xF2) {
						w = 0x2295;
					} else if (c2 == 0xF3) {
						w = 0x2299;
					} else if (c2 == 0xFE) {
						w = 0xFF0F;
					}
				} else if (c == 0xA2) {
					if (c2 == 0x40) {
						w = 0xFF3C;
					} else if (c2 == 0x41) {
						w = 0x2215;
					} else if (c2 == 0x42) {
						w = 0xFE68;
					} else if (c2 == 0x46) {
						w = 0xFFE0;
					} else if (c2 == 0x47) {
						w = 0xFFE1;
					} else if (c2 == 0xCC) {
						w = 0x5341;
					} else if (c2 == 0xCE) {
						w = 0x5345;
					}
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_cp950(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_big5_table_min && w < ucs_a1_big5_table_max) {
			s = ucs_a1_big5_table[w - ucs_a1_big5_table_min];
		} else if (w >= ucs_a2_big5_table_min && w < ucs_a2_big5_table_max) {
			s = ucs_a2_big5_table[w - ucs_a2_big5_table_min];
		} else if (w >= ucs_a3_big5_table_min && w < ucs_a3_big5_table_max) {
			s = ucs_a3_big5_table[w - ucs_a3_big5_table_min];
		} else if (w >= ucs_i_big5_table_min && w < ucs_i_big5_table_max) {
			s = ucs_i_big5_table[w - ucs_i_big5_table_min];
		} else if (w >= ucs_r1_big5_table_min && w < ucs_r1_big5_table_max) {
			s = ucs_r1_big5_table[w - ucs_r1_big5_table_min];
		} else if (w >= ucs_r2_big5_table_min && w < ucs_r2_big5_table_max) {
			s = ucs_r2_big5_table[w - ucs_r2_big5_table_min];
		}

		if (w >= 0xE000 && w <= 0xF848) {
			int k;
			for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
				if (w <= cp950_pua_tbl[k][1]) {
					break;
				}
			}

			int c1 = w - cp950_pua_tbl[k][0];
			if ((cp950_pua_tbl[k][2] & 0xFF) == 0x40) {
				int c2 = cp950_pua_tbl[k][2] >> 8;
				s = ((c1 / 157) + c2) << 8;
				c1 %= 157;
				s |= c1 + (c1 >= 0x3F ? 0x62 : 0x40);
			} else {
				s = c1 + cp950_pua_tbl[k][2];
			}
		} else if (w == 0xA2 || w == 0xA3 || w == 0x401 || (w >= 0x414 && w <= 0x41C) || (w >= 0x423 && w <= 0x44F) || w == 0x451 || w == 0x2022 || w == 0x203E || w == 0x223C || (w >= 0x2460 && w <= 0x247D) || w == 0x2609 || w == 0x2641 || w == 0x3005 || (w >= 0x302A && w <= 0x30FF) || w == 0xFF64) {
			s = 0;
		} else if (w == 0xAF) {
			s = 0xA1C2;
		} else if (w == 0x2CD) {
			s = 0xA1C5;
		} else if (w == 0x2027) {
			s = 0xA145;
		} else if (w == 0x2215) {
			s = 0xA241;
		} else if (w == 0x2295) {
			s = 0xA1F2;
		} else if (w == 0x2299) {
			s = 0xA1F3;
		} else if (w == 0x2574) {
			s = 0xA15A;
		} else if (w == 0xFE51) {
			s = 0xA14E;
		} else if (w == 0xFE68) {
			s = 0xA242;
		} else if (w == 0xFF3C) {
			s = 0xA240;
		} else if (w == 0xFF5E) {
			s = 0xA1E3;
		} else if (w == 0xFFE0) {
			s = 0xA246;
		} else if (w == 0xFFE1) {
			s = 0xA247;
		} else if (w == 0xFFE3) {
			s = 0xA1C3;
		} else if (w == 0xFF0F) {
			s = 0xA1FE;
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_big5);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s <= 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static const char *mbfl_encoding_big5_aliases[] = {"CN-BIG5", "BIG-FIVE", "BIGFIVE", NULL};

static const struct mbfl_convert_vtbl vtbl_big5_wchar = {
	mbfl_no_encoding_big5,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_big5_wchar,
	mbfl_filt_conv_big5_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_big5 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_big5,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_big5,
	mbfl_filt_conv_common_flush,
	NULL
};

const mbfl_encoding mbfl_encoding_big5 = {
	mbfl_no_encoding_big5,
	"BIG-5",
	"BIG5",
	mbfl_encoding_big5_aliases,
	mblen_table_81_to_fe,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_big5_wchar,
	&vtbl_wchar_big5,
	mb_big5_to_wchar,
	mb_wchar_to_big5,
	NULL
};

static const struct mbfl_convert_vtbl vtbl_cp950_wchar = {
	mbfl_no_encoding_cp950,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_big5_wchar,
	mbfl_filt_conv_big5_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_cp950 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp950,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_big5,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_cp950 = {
	mbfl_no_encoding_cp950,
	"CP950",
	"BIG5",
	NULL,
	mblen_table_81_to_fe,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp950_wchar,
	&vtbl_wchar_cp950,
	mb_cp950_to_wchar,
	mb_wchar_to_cp950,
	NULL
};

/*
 * HZ
 */

static int mbfl_filt_conv_hz_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status & 0xf) {
	/* case 0x00: ASCII */
	/* case 0x10: GB2312 */
	case 0:
		if (c == '~') {
			filter->status += 2;
		} else if (filter->status == 0x10 && ((c > 0x20 && c <= 0x29) || (c >= 0x30 && c <= 0x77))) {
			/* DBCS first char */
			filter->cache = c;
			filter->status += 1;
		} else if (filter->status == 0 && c >= 0 && c < 0x80) { /* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* case 0x11: GB2312 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c1 > 0x20 && c1 < 0x7F && c > 0x20 && c < 0x7F) {
			s = (c1 - 1)*192 + c + 0x40; /* GB2312 */
			ZEND_ASSERT(s < cp936_ucs_table_size);
			if (s == 0x1864) {
				w = 0x30FB;
			} else if (s == 0x186A) {
				w = 0x2015;
			} else if (s == 0x186C) {
				w = 0x2225;
			} else if ((s >= 0x1920 && s <= 0x192A) || s == 0x1963 || (s >= 0x1C60 && s <= 0x1C7F) || (s >= 0x1DBB && s <= 0x1DC4)) {
				w = 0;
			} else {
				w = cp936_ucs_table[s];
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* '~' */
	case 2:
		if (c == '}' && filter->status == 0x12) {
			filter->status = 0;
		} else if (c == '{' && filter->status == 2) {
			filter->status = 0x10;
		} else if (c == '~' && filter->status == 2) {
			CK((*filter->output_function)('~', filter->data));
			filter->status -= 2;
		} else if (c == '\n') {
			/* "~\n" is a line continuation; no output is needed, nor should we shift modes */
			filter->status -= 2;
		} else {
			/* Invalid character after ~ */
			filter->status -= 2;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_hz_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 0x11) {
		/* 2-byte character was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_hz(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		if (c == 0xB7 || c == 0x144 || c == 0x148 || c == 0x251 || c == 0x261 || c == 0x2CA || c == 0x2CB || c == 0x2D9) {
			s = 0;
		} else {
			s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
		}
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		if (c == 0x2015) {
			s = 0xA1AA;
		} else if (c == 0x2010 || c == 0x2013 || c == 0x2014 || c == 0x2016 || c == 0x2025 || c == 0x2035 ||
				c == 0x2105 || c == 0x2109 || c == 0x2121 || (c >= 0x2170 && c <= 0x2179) || (c >= 0x2196 && c <= 0x2199) ||
				c == 0x2215 || c == 0x221F || c == 0x2223 || c == 0x2252 || c == 0x2266 || c == 0x2267 || c == 0x2295 ||
				(c >= 0x2550 && c <= 0x2573) || c == 0x22BF || c == 0x2609 || (c >= 0x2581 && c <= 0x258F) ||
				(c >= 0x2593 && c <= 0x2595) || c == 0x25BC || c == 0x25BD || (c >= 0x25E2 && c <= 0x25E5)) {
			s = 0;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		if (c == 0x30FB) {
			s = 0xA1A4;
		} else if (c == 0x3006 || c == 0x3007 || c == 0x3012 || c == 0x3231 || c == 0x32A3 || c >= 0x3300 ||
				(c >= 0x3018 && c <= 0x3040) || (c >= 0x309B && c <= 0x309E) || (c >= 0x30FC && c <= 0x30FE)) {
			s = 0;
		} else {
			s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
		}
	} else if (c >= ucs_i_gb2312_table_min && c < ucs_i_gb2312_table_max) {
		s = ucs_i_gb2312_table[c - ucs_i_gb2312_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		if (c == 0xFF04) {
			s = 0xA1E7;
		} else if (c == 0xFF5E) {
			s = 0xA1AB;
		} else if (c >= 0xFF01 && c <= 0xFF5D) {
			s = c - 0xFF01 + 0xA3A1;
		} else if (c == 0xFFE0 || c == 0xFFE1 || c == 0xFFE3 || c == 0xFFE5) {
			s = ucs_hff_s_cp936_table[c - 0xFFE0];
		}
	}

	if (s & 0x8000) {
		s -= 0x8080;
	}

	if (s <= 0) {
		s = (c == 0) ? 0 : -1;
	} else if ((s >= 0x80 && s < 0x2121) || s > 0x8080) {
		s = -1;
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)('~', filter->data));
				CK((*filter->output_function)('}', filter->data));
			}
			filter->status = 0;
			if (s == 0x7E) {
				CK((*filter->output_function)('~', filter->data));
			}
			CK((*filter->output_function)(s, filter->data));
		} else { /* GB 2312-80 */
			if ((filter->status & 0xFF00) != 0x200) {
				CK((*filter->output_function)('~', filter->data));
				CK((*filter->output_function)('{', filter->data));
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7F, filter->data));
			CK((*filter->output_function)(s & 0x7F, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_any_hz_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if (filter->status & 0xFF00) {
		CK((*filter->output_function)('~', filter->data));
		CK((*filter->output_function)('}', filter->data));
	}
	filter->status = 0;
	return 0;
}

#define ASCII 0
#define GB2312 1

static size_t mb_hz_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == '~') {
			if (p == e) {
				break;
			}
			unsigned char c2 = *p++;

			if (c2 == '}' && *state == GB2312) {
				*state = ASCII;
			} else if (c2 == '{' && *state == ASCII) {
				*state = GB2312;
			} else if (c2 == '~' && *state == ASCII) {
				*out++ = '~';
			} else if (c2 == '\n') {
				/* "~\n" is a line continuation; no output is needed, nor should we shift modes */
			} else {
				/* Invalid character after ~ */
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (((c > 0x20 && c <= 0x29) || (c >= 0x30 && c <= 0x77)) && p < e && *state == GB2312) {
			unsigned char c2 = *p++;

			if (c > 0x20 && c < 0x7F && c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 1)*192 + c2 + 0x40;
				ZEND_ASSERT(s < cp936_ucs_table_size);

				if (s == 0x1864) {
					s = 0x30FB;
				} else if (s == 0x186A) {
					s = 0x2015;
				} else if (s == 0x186C) {
					s = 0x2225;
				} else if ((s >= 0x1920 && s <= 0x192A) || s == 0x1963 || (s >= 0x1C60 && s <= 0x1C7F) || (s >= 0x1DBB && s <= 0x1DC4)) {
					s = 0;
				} else {
					s = cp936_ucs_table[s];
				}
				if (!s)
					s = MBFL_BAD_INPUT;
				*out++ = s;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c < 0x80 && *state == ASCII) {
			*out++ = c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_hz(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			if (w == 0xB7 || w == 0x144 || w == 0x148 || w == 0x251 || w == 0x261 || w == 0x2CA || w == 0x2CB || w == 0x2D9) {
				s = 0;
			} else {
				s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
			}
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			if (w == 0x2015) {
				s = 0xA1AA;
			} else if (w == 0x2010 || w == 0x2013 || w == 0x2014 || w == 0x2016 || w == 0x2025 || w == 0x2035 || w == 0x2105 || w == 0x2109 || w == 0x2121 || (w >= 0x2170 && w <= 0x2179) || (w >= 0x2196 && w <= 0x2199) || w == 0x2215 || w == 0x221F || w == 0x2223 || w == 0x2252 || w == 0x2266 || w == 0x2267 || w == 0x2295 || (w >= 0x2550 && w <= 0x2573) || w == 0x22BF || w == 0x2609 || (w >= 0x2581 && w <= 0x258F) || (w >= 0x2593 && w <= 0x2595) || w == 0x25BC || w == 0x25BD || (w >= 0x25E2 && w <= 0x25E5)) {
				s = 0;
			} else {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			if (w == 0x30FB) {
				s = 0xA1A4;
			} else if (w == 0x3006 || w == 0x3007 || w == 0x3012 || w == 0x3231 || w == 0x32A3 || w >= 0x3300 || (w >= 0x3018 && w <= 0x3040) || (w >= 0x309B && w <= 0x309E) || (w >= 0x30FC && w <= 0x30FE)) {
				s = 0;
			} else {
				s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
			}
		} else if (w >= ucs_i_gb2312_table_min && w < ucs_i_gb2312_table_max) {
			s = ucs_i_gb2312_table[w - ucs_i_gb2312_table_min];
		} else if (w >= ucs_hff_cp936_table_min && w < ucs_hff_cp936_table_max) {
			if (w == 0xFF04) {
				s = 0xA1E7;
			} else if (w == 0xFF5E) {
				s = 0xA1AB;
			} else if (w >= 0xFF01 && w <= 0xFF5D) {
				s = w - 0xFF01 + 0xA3A1;
			} else if (w == 0xFFE0 || w == 0xFFE1 || w == 0xFFE3 || w == 0xFFE5) {
				s = ucs_hff_s_cp936_table[w - 0xFFE0];
			}
		}

		s &= ~0x8080;

		if ((!s && w) || (s >= 0x80 && s < 0x2121)) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_hz);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s < 0x80) {
			/* ASCII */
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
				out = mb_convert_buf_add2(out, '~', '}');
				buf->state = ASCII;
			}
			if (s == '~') {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				out = mb_convert_buf_add2(out, '~', '~');
			} else {
				out = mb_convert_buf_add(out, s);
			}
		} else {
			/* GB 2312-80 */
			if (buf->state != GB2312) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add2(out, '~', '{');
				buf->state = GB2312;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		}
	}

	if (end && buf->state != ASCII) {
		/* If not in ASCII state, need to emit closing control chars */
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 2);
		out = mb_convert_buf_add2(out, '~', '}');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static const struct mbfl_convert_vtbl vtbl_hz_wchar = {
	mbfl_no_encoding_hz,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_hz_wchar,
	mbfl_filt_conv_hz_wchar_flush,
	NULL,
};

static const struct mbfl_convert_vtbl vtbl_wchar_hz = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_hz,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_hz,
	mbfl_filt_conv_any_hz_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_hz = {
	mbfl_no_encoding_hz,
	"HZ",
	"HZ-GB-2312",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_hz_wchar,
	&vtbl_wchar_hz,
	mb_hz_to_wchar,
	mb_wchar_to_hz,
	NULL
};
