/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Ignace Nyamagana Butera <nyamsprod@gmail.com>                |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"
#include "encoding_arginfo.h"
#include "encoding_decl.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Woverride-init"
#endif

/* Class entries */
static zend_class_entry *encoding_ce_EncodingError;
static zend_class_entry *encoding_ce_EncodingException;
static zend_class_entry *encoding_ce_UnableToDecodeException;
static zend_class_entry *encoding_ce_UnableToEncodeException;

static zend_class_entry *encoding_ce_Base16;
static zend_class_entry *encoding_ce_Base32;
static zend_class_entry *encoding_ce_Base58;
static zend_class_entry *encoding_ce_Base64;
static zend_class_entry *encoding_ce_Base85;
static zend_class_entry *encoding_ce_PaddingMode;
static zend_class_entry *encoding_ce_DecodingMode;
static zend_class_entry *encoding_ce_TimingMode;

/* Exception throwing macros */
#define THROW_UNABLE_TO_DECODE(msg) \
	zend_throw_exception(encoding_ce_UnableToDecodeException, msg, 0)

#define THROW_UNABLE_TO_ENCODE(msg) \
	zend_throw_exception(encoding_ce_UnableToEncodeException, msg, 0)

/* ===================== Base16 ===================== */

/* Lookup tables for Base16 encoding */
static const uint8_t base16_encode_upper[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static const uint8_t base16_encode_lower[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

/* Reverse lookup for decoding: maps ASCII char to 0-15 or 0xFF if invalid */
static const uint8_t base16_reverse_upper[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4, ['5'] = 5,
	['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15
};

static const uint8_t base16_reverse_lower[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4, ['5'] = 5,
	['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9,
	['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15
};

static const uint8_t base16_reverse_forgiving[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4, ['5'] = 5,
	['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15,
	['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15
};

static inline bool is_base16_whitespace(uint8_t c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static zend_string *base16_encode_impl(const char *data, size_t len, zend_enum_Encoding_Base16 variant, bool constant_time) {
	const uint8_t *table = (variant == ZEND_ENUM_Encoding_Base16_Upper)
		? base16_encode_upper : base16_encode_lower;

	zend_string *result = zend_string_alloc(len * 2, 0);
	char *dst = ZSTR_VAL(result);

	for (size_t i = 0; i < len; i++) {
		uint8_t byte = (uint8_t)data[i];
		*dst++ = table[byte >> 4];
		*dst++ = table[byte & 0x0F];
	}

	ZSTR_LEN(result) = len * 2;
	ZSTR_VAL(result)[len * 2] = '\0';
	return result;
}

static zend_string *base16_decode_impl(const char *data, size_t len, zend_enum_Encoding_Base16 variant, zend_enum_Encoding_DecodingMode mode, bool constant_time) {
	const uint8_t *reverse_table;

	if (mode == ZEND_ENUM_Encoding_DecodingMode_Forgiving) {
		reverse_table = base16_reverse_forgiving;
	} else if (variant == ZEND_ENUM_Encoding_Base16_Upper) {
		reverse_table = base16_reverse_upper;
	} else {
		reverse_table = base16_reverse_lower;
	}

	/* Count non-whitespace characters to determine output length */
	size_t content_len = 0;
	for (size_t i = 0; i < len; i++) {
		if (!is_base16_whitespace((uint8_t)data[i])) {
			content_len++;
		}
	}

	/* Must have even number of hex digits */
	if (content_len % 2 != 0) {
		THROW_UNABLE_TO_DECODE("Hex data must have an even length");
		return NULL;
	}

	zend_string *result = zend_string_alloc(content_len / 2, 0);
	char *dst = ZSTR_VAL(result);
	size_t dst_idx = 0;
	uint8_t high = 0;
	bool have_high = false;

	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];

		if (is_base16_whitespace(c)) {
			continue;
		}

		uint8_t val = reverse_table[c];

		if (val == 0xFF) {
			zend_string_efree(result);
			THROW_UNABLE_TO_DECODE("Invalid hex character");
			return NULL;
		}

		if (!have_high) {
			high = val;
			have_high = true;
		} else {
			dst[dst_idx++] = (char)(high << 4 | val);
			have_high = false;
		}
	}

	ZSTR_LEN(result) = dst_idx;
	ZSTR_VAL(result)[dst_idx] = '\0';
	return result;
}

 PHP_FUNCTION(Encoding_base16_encode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base16)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base16 variant = ZEND_ENUM_Encoding_Base16_Upper;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base16)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	bool constant_time = false;
	if (timing_obj) {
		constant_time = (zend_enum_fetch_case_id(Z_OBJ_P(timing_obj)) == ZEND_ENUM_Encoding_TimingMode_Constant);
	}

	RETURN_STR(base16_encode_impl(data, data_len, variant, constant_time));
}

 PHP_FUNCTION(Encoding_base16_decode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *decoding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base16)
		Z_PARAM_OBJECT_OF_CLASS(decoding_obj, encoding_ce_DecodingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base16 variant = ZEND_ENUM_Encoding_Base16_Upper;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base16)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	zend_enum_Encoding_DecodingMode mode = ZEND_ENUM_Encoding_DecodingMode_Strict;
	if (decoding_obj) {
		mode = (zend_enum_Encoding_DecodingMode)zend_enum_fetch_case_id(Z_OBJ_P(decoding_obj));
	}

	bool constant_time = false;
	if (timing_obj) {
		constant_time = (zend_enum_fetch_case_id(Z_OBJ_P(timing_obj)) == ZEND_ENUM_Encoding_TimingMode_Constant);
	}

	zend_string *result = base16_decode_impl(data, data_len, variant, mode, constant_time);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

/* ===================== Base32 ===================== */

static const char base32_ascii_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static const char base32_hex_table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
static const char base32_crockford_table[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
static const char base32_z_table[] = "pebrv9tkngxm4zywsiu1oca6fjh5q23780ld";

static const uint8_t base32_ascii_reverse[256] = {
	[0 ... 255] = 0xFF,
	['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,
	['F'] = 5,  ['G'] = 6,  ['H'] = 7,  ['I'] = 8,  ['J'] = 9,
	['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14,
	['P'] = 15, ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
	['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, ['Y'] = 24,
	['Z'] = 25, ['2'] = 26, ['3'] = 27, ['4'] = 28, ['5'] = 29,
	['6'] = 30, ['7'] = 31
};

static const uint8_t base32_hex_reverse[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,  ['4'] = 4,
	['5'] = 5,  ['6'] = 6,  ['7'] = 7,  ['8'] = 8,  ['9'] = 9,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14,
	['F'] = 15, ['G'] = 16, ['H'] = 17, ['I'] = 18, ['J'] = 19,
	['K'] = 20, ['L'] = 21, ['M'] = 22, ['N'] = 23, ['O'] = 24,
	['P'] = 25, ['Q'] = 26, ['R'] = 27, ['S'] = 28, ['T'] = 29,
	['U'] = 30, ['V'] = 31
};

static const uint8_t base32_crockford_reverse[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,  ['4'] = 4,
	['5'] = 5,  ['6'] = 6,  ['7'] = 7,  ['8'] = 8,  ['9'] = 9,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14,
	['F'] = 15, ['G'] = 16, ['H'] = 17,
	/* Crockford: I and L decode as 1, O and o decode as 0 */
	['I'] = 1,  ['i'] = 1,  ['L'] = 1,  ['l'] = 1,
	['O'] = 0,  ['o'] = 0,
	['J'] = 18, ['K'] = 19,
	['M'] = 20, ['N'] = 21, ['P'] = 22,
	['Q'] = 23, ['R'] = 24, ['S'] = 25, ['T'] = 26,
	['V'] = 27, ['W'] = 28, ['X'] = 29, ['Y'] = 30, ['Z'] = 31,
	/* Lowercase for case-insensitive Crockford */
	['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14,
	['f'] = 15, ['g'] = 16, ['h'] = 17,
	['j'] = 18, ['k'] = 19,
	['m'] = 20, ['n'] = 21, ['p'] = 22,
	['q'] = 23, ['r'] = 24, ['s'] = 25, ['t'] = 26,
	['v'] = 27, ['w'] = 28, ['x'] = 29, ['y'] = 30, ['z'] = 31
};

static const uint8_t base32_z_reverse[256] = {
	[0 ... 255] = 0xFF,
	['p'] = 0,  ['e'] = 1,  ['b'] = 2,  ['r'] = 3,  ['v'] = 4,
	['9'] = 5,  ['t'] = 6,  ['k'] = 7,  ['n'] = 8,  ['g'] = 9,
	['x'] = 10, ['m'] = 11, ['4'] = 12, ['z'] = 13, ['y'] = 14,
	['w'] = 15, ['s'] = 16, ['i'] = 17, ['u'] = 18, ['1'] = 19,
	['o'] = 20, ['c'] = 21, ['a'] = 22, ['6'] = 23, ['f'] = 24,
	['j'] = 25, ['h'] = 26, ['5'] = 27, ['q'] = 28, ['2'] = 29,
	['3'] = 30, ['7'] = 31, ['8'] = 32, ['0'] = 33, ['l'] = 34, ['d'] = 35
};

static inline bool is_base32_whitespace(uint8_t c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static zend_string *base32_encode_impl(const char *data, size_t len, zend_enum_Encoding_Base32 variant, zend_enum_Encoding_PaddingMode padding) {
	const char *table;
	bool uses_padding;
	bool strip_padding;

	switch (variant) {
		case ZEND_ENUM_Encoding_Base32_Ascii:
			table = base32_ascii_table;
			uses_padding = true;
			break;
		case ZEND_ENUM_Encoding_Base32_Hex:
			table = base32_hex_table;
			uses_padding = true;
			break;
		case ZEND_ENUM_Encoding_Base32_Crockford:
			table = base32_crockford_table;
			uses_padding = false;
			break;
		case ZEND_ENUM_Encoding_Base32_Z:
			table = base32_z_table;
			uses_padding = false;
			break;
		default:
			table = base32_ascii_table;
			uses_padding = true;
			break;
	}

	switch (padding) {
		case ZEND_ENUM_Encoding_PaddingMode_VariantControlled:
			strip_padding = false;
			break;
		case ZEND_ENUM_Encoding_PaddingMode_StripPadding:
			strip_padding = true;
			if (!uses_padding) {
				zend_value_error("PaddingMode::StripPadding is not supported for variant %s",
					variant == ZEND_ENUM_Encoding_Base32_Crockford ? "Base32::Crockford" :
					variant == ZEND_ENUM_Encoding_Base32_Z ? "Base32::Z" : "Base32");
				return NULL;
			}
			break;
		case ZEND_ENUM_Encoding_PaddingMode_PreservePadding:
			strip_padding = false;
			if (!uses_padding) {
				zend_value_error("PaddingMode::PreservePadding is not supported for variant %s",
					variant == ZEND_ENUM_Encoding_Base32_Crockford ? "Base32::Crockford" :
					variant == ZEND_ENUM_Encoding_Base32_Z ? "Base32::Z" : "Base32");
				return NULL;
			}
			break;
		default:
			strip_padding = false;
			break;
	}

	size_t encoded_len = (len + 4) / 5 * 8;
	if (strip_padding) {
		size_t remainder = len % 5;
		if (remainder) {
			size_t pad = 8 - ((remainder * 8 + 4) / 5);
			encoded_len -= pad;
		}
	}

	zend_string *result = zend_string_alloc(encoded_len, 0);
	char *dst = ZSTR_VAL(result);

	size_t i = 0;
	while (i < len) {
		uint64_t nbits = 0;
		size_t bytes_left = len - i;
		size_t chars_to_read = bytes_left < 5 ? bytes_left : 5;

		for (size_t j = 0; j < chars_to_read; j++) {
			nbits = (nbits << 8) | (uint8_t)data[i + j];
		}

		size_t total_chars = (chars_to_read * 8 + 4) / 5;
		nbits <<= (uint64_t)(total_chars * 5 - chars_to_read * 8);
		for (size_t c = 0; c < total_chars; c++) {
			*dst++ = table[(nbits >> ((total_chars - 1 - c) * 5)) & 0x1F];
		}

		if (uses_padding && !strip_padding && chars_to_read < 5) {
			size_t pad = 8 - total_chars;
			for (size_t p = 0; p < pad; p++) {
				*dst++ = '=';
			}
		}

		i += chars_to_read;
	}

	size_t result_len = (size_t)(dst - ZSTR_VAL(result));
	ZSTR_LEN(result) = result_len;
	ZSTR_VAL(result)[result_len] = '\0';
	return result;
}

static zend_string *base32_decode_impl(const char *data, size_t len, zend_enum_Encoding_Base32 variant, zend_enum_Encoding_DecodingMode mode, bool constant_time) {
	const uint8_t *reverse_table;
	bool uses_padding;

	switch (variant) {
		case ZEND_ENUM_Encoding_Base32_Ascii:
			reverse_table = base32_ascii_reverse;
			uses_padding = true;
			break;
		case ZEND_ENUM_Encoding_Base32_Hex:
			reverse_table = base32_hex_reverse;
			uses_padding = true;
			break;
		case ZEND_ENUM_Encoding_Base32_Crockford:
			reverse_table = base32_crockford_reverse;
			uses_padding = false;
			break;
		case ZEND_ENUM_Encoding_Base32_Z:
			reverse_table = base32_z_reverse;
			uses_padding = false;
			break;
		default:
			reverse_table = base32_ascii_reverse;
			uses_padding = true;
			break;
	}

	/* Count non-whitespace, non-padding characters */
	size_t content_len = 0;
	size_t padding_count = 0;
	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];
		if (is_base32_whitespace(c)) {
			continue;
		}
		if (c == '=') {
			padding_count++;
			continue;
		}
		content_len++;
	}

	bool forgiving = (mode == ZEND_ENUM_Encoding_DecodingMode_Forgiving);

	if (uses_padding) {
		if (forgiving) {
			/* In forgiving mode, we can handle missing or incorrect padding */
			/* Calculate expected padding and fix it */
			size_t remainder = content_len % 8;
			if (remainder) {
				size_t expected_pad = 8 - remainder;
				if (padding_count != expected_pad && padding_count == 0) {
					/* Missing padding - forgive it */
				} else if (padding_count != expected_pad) {
					THROW_UNABLE_TO_DECODE("Invalid padding length");
					return NULL;
				}
			} else if (padding_count != 0) {
				THROW_UNABLE_TO_DECODE("Invalid padding length");
				return NULL;
			}
		} else {
			/* Strict mode: padding is required */
			size_t remainder = content_len % 8;
			if (remainder) {
				size_t expected_pad = 8 - remainder;
				if (padding_count != expected_pad) {
					THROW_UNABLE_TO_DECODE("Invalid padding length");
					return NULL;
				}
			} else if (padding_count != 0) {
				THROW_UNABLE_TO_DECODE("Invalid padding length");
				return NULL;
			}
		}
	}

	size_t out_len = (content_len * 5) / 8;
	zend_string *result = zend_string_alloc(out_len, 0);
	char *dst = ZSTR_VAL(result);
	size_t dst_idx = 0;

	uint64_t nbits = 0;
	int bits = 0;

	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];

		if (is_base32_whitespace(c) || c == '=') {
			continue;
		}

		uint8_t val = reverse_table[c];

		if (val == 0xFF) {
			zend_string_efree(result);
			THROW_UNABLE_TO_DECODE("Invalid Base32 character");
			return NULL;
		}

		nbits = (nbits << 5) | val;
		bits += 5;

		while (bits >= 8) {
			bits -= 8;
			if (dst_idx < out_len) {
				dst[dst_idx++] = (char)((nbits >> bits) & 0xFF);
			}
		}
	}

	ZSTR_LEN(result) = dst_idx;
	ZSTR_VAL(result)[dst_idx] = '\0';
	return result;
}

PHP_FUNCTION(Encoding_base32_encode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *padding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base32)
		Z_PARAM_OBJECT_OF_CLASS(padding_obj, encoding_ce_PaddingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base32 variant = ZEND_ENUM_Encoding_Base32_Ascii;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base32)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	zend_enum_Encoding_PaddingMode padding = ZEND_ENUM_Encoding_PaddingMode_VariantControlled;
	if (padding_obj) {
		padding = (zend_enum_Encoding_PaddingMode)zend_enum_fetch_case_id(Z_OBJ_P(padding_obj));
	}

	zend_string *result = base32_encode_impl(data, data_len, variant, padding);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

PHP_FUNCTION(Encoding_base32_decode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *decoding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base32)
		Z_PARAM_OBJECT_OF_CLASS(decoding_obj, encoding_ce_DecodingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base32 variant = ZEND_ENUM_Encoding_Base32_Ascii;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base32)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	zend_enum_Encoding_DecodingMode mode = ZEND_ENUM_Encoding_DecodingMode_Strict;
	if (decoding_obj) {
		mode = (zend_enum_Encoding_DecodingMode)zend_enum_fetch_case_id(Z_OBJ_P(decoding_obj));
	}

	bool constant_time = false;
	if (timing_obj) {
		constant_time = (zend_enum_fetch_case_id(Z_OBJ_P(timing_obj)) == ZEND_ENUM_Encoding_TimingMode_Constant);
	}

	zend_string *result = base32_decode_impl(data, data_len, variant, mode, constant_time);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

/* ===================== Base58 ===================== */

static const char base58_bitcoin_table[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const char base58_flickr_table[] = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";

static const uint8_t base58_bitcoin_reverse[256] = {
	[0 ... 255] = 0xFF,
	['1'] = 0,  ['2'] = 1,  ['3'] = 2,  ['4'] = 3,  ['5'] = 4,
	['6'] = 5,  ['7'] = 6,  ['8'] = 7,  ['9'] = 8,
	['A'] = 9,  ['B'] = 10, ['C'] = 11, ['D'] = 12, ['E'] = 13, ['F'] = 14, ['G'] = 15,
	['H'] = 16, ['J'] = 17, ['K'] = 18, ['L'] = 19,
	['M'] = 20, ['N'] = 21,
	['P'] = 22, ['Q'] = 23, ['R'] = 24, ['S'] = 25, ['T'] = 26, ['U'] = 27,
	['V'] = 28, ['W'] = 29, ['X'] = 30, ['Y'] = 31, ['Z'] = 32,
	['a'] = 33, ['b'] = 34, ['c'] = 35, ['d'] = 36, ['e'] = 37, ['f'] = 38,
	['g'] = 39, ['h'] = 40, ['i'] = 41, ['j'] = 42, ['k'] = 43, ['m'] = 44,
	['n'] = 45, ['o'] = 46, ['p'] = 47, ['q'] = 48, ['r'] = 49, ['s'] = 50, ['t'] = 51,
	['u'] = 52, ['v'] = 53, ['w'] = 54, ['x'] = 55, ['y'] = 56, ['z'] = 57
};

static const uint8_t base58_flickr_reverse[256] = {
	[0 ... 255] = 0xFF,
	['1'] = 0,  ['2'] = 1,  ['3'] = 2,  ['4'] = 3,  ['5'] = 4,
	['6'] = 5,  ['7'] = 6,  ['8'] = 7,  ['9'] = 8,
	['a'] = 9,  ['b'] = 10, ['c'] = 11, ['d'] = 12, ['e'] = 13, ['f'] = 14, ['g'] = 15,
	['h'] = 16, ['i'] = 17, ['j'] = 18, ['k'] = 19,
	['m'] = 20, ['n'] = 21, ['o'] = 22,
	['p'] = 23, ['q'] = 24, ['r'] = 25, ['s'] = 26, ['t'] = 27, ['u'] = 28,
	['v'] = 29, ['w'] = 30, ['x'] = 31, ['y'] = 32, ['z'] = 33,
	['A'] = 34, ['B'] = 35, ['C'] = 36, ['D'] = 37, ['E'] = 38, ['F'] = 39,
	['G'] = 40, ['H'] = 41, ['J'] = 42, ['K'] = 43, ['L'] = 44, ['M'] = 45,
	['N'] = 46, ['P'] = 47, ['Q'] = 48, ['R'] = 49, ['S'] = 50, ['T'] = 51,
	['U'] = 52, ['V'] = 53, ['W'] = 54, ['X'] = 55, ['Y'] = 56, ['Z'] = 57
};

static zend_string *base58_encode_impl(const char *data, size_t len, zend_enum_Encoding_Base58 variant) {
	const char *table = (variant == ZEND_ENUM_Encoding_Base58_Bitcoin)
		? base58_bitcoin_table : base58_flickr_table;

	/* Max output size: ceil(len * 8 / log2(58)) ≈ len * 1.4 + margin */
	size_t max_out_len = (len * 138 + 100) / 100 + 16;

	/* Count leading zeros */
	size_t zeros = 0;
	while (zeros < len && (uint8_t)data[zeros] == 0) {
		zeros++;
	}

	/* Output buffer, zero-initialized */
	uint8_t *output = ecalloc(max_out_len, sizeof(uint8_t));
	size_t out_begin = max_out_len;
	size_t out_end = max_out_len;

	/* Process each input byte: multiply accumulated number by 256, add byte */
	for (size_t i = zeros; i < len; i++) {
		uint32_t carry = (uint8_t)data[i];
		size_t j = out_end;
		while (j > out_begin) {
			j--;
			carry += (uint32_t)output[j] * 256;
			output[j] = (uint8_t)(carry % 58);
			carry /= 58;
		}
		while (carry > 0) {
			if (out_begin == 0) break;
			out_begin--;
			output[out_begin] = (uint8_t)(carry % 58);
			carry /= 58;
		}
	}

	/* Calculate output length */
	size_t out_len = zeros + (out_end - out_begin);
	zend_string *result = zend_string_alloc(out_len, 0);
	char *dst = ZSTR_VAL(result);

	/* Leading zeros become '1' characters */
	for (size_t i = 0; i < zeros; i++) {
		*dst++ = table[0];
	}

	/* Encode the digits */
	for (size_t i = out_begin; i < out_end; i++) {
		*dst++ = table[output[i]];
	}

	size_t result_len = (size_t)(dst - ZSTR_VAL(result));
	ZSTR_LEN(result) = result_len;
	ZSTR_VAL(result)[result_len] = '\0';
	efree(output);

	return result;
}

static zend_string *base58_decode_impl(const char *data, size_t len, zend_enum_Encoding_Base58 variant) {
	const uint8_t *reverse_table = (variant == ZEND_ENUM_Encoding_Base58_Bitcoin)
		? base58_bitcoin_reverse : base58_flickr_reverse;

	/* Count leading '1' characters (zeros in base58) and validate all chars */
	size_t zeros = 0;
	size_t content_len = 0;
	bool past_leading = false;
	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
		uint8_t val = reverse_table[c];
		if (val == 0xFF) {
			THROW_UNABLE_TO_DECODE("Invalid Base58 character");
			return NULL;
		}
		if (!past_leading) {
			if (val == 0) zeros++;
			else past_leading = true;
		}
		content_len++;
	}

	/* Parse non-zero digits into base58 digit array (LSB first) */
	size_t num_digits = content_len - zeros;
	if (num_digits == 0) {
		zend_string *result = zend_string_alloc(zeros, 0);
		memset(ZSTR_VAL(result), 0, zeros);
		ZSTR_VAL(result)[zeros] = '\0';
		return result;
	}

	uint8_t *digits = emalloc(num_digits);
	size_t di = 0;
	past_leading = false;
	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
		if (!past_leading) {
			if (reverse_table[c] == 0) continue;
			past_leading = true;
		}
		digits[di++] = reverse_table[c];
	}

	/* digits[] holds the base58 number with digits[0] = MSD, digits[num_digits-1] = LSD */
	/* Reverse to get LSD first */
	for (size_t i = 0; i < num_digits / 2; i++) {
		uint8_t tmp = digits[i];
		digits[i] = digits[num_digits - 1 - i];
		digits[num_digits - 1 - i] = tmp;
	}

	/* Repeatedly divide by 256, collecting remainders as output bytes */
	size_t max_out_len = (content_len * 74 + 100) / 100 + 1;
	uint8_t *output = ecalloc(max_out_len, sizeof(uint8_t));
	size_t out_idx = 0;

	size_t d_begin = 0;
	size_t d_end = num_digits;

		while (d_begin < d_end) {
		uint32_t carry = 0;
		for (size_t j = d_end; j > d_begin; ) {
			j--;
			uint32_t temp = carry * 58 + digits[j];
			digits[j] = (uint8_t)(temp / 256);
			carry = temp % 256;
		}
		if (out_idx < max_out_len) {
			output[out_idx++] = (uint8_t)carry;
		}
		/* Skip trailing zeros (MSD side) in digit array */
		while (d_begin < d_end && digits[d_end - 1] == 0) {
			d_end--;
		}
	}

	/* output[] is LSB first, reverse to get big-endian */
	for (size_t i = 0; i < out_idx / 2; i++) {
		uint8_t tmp = output[i];
		output[i] = output[out_idx - 1 - i];
		output[out_idx - 1 - i] = tmp;
	}

	size_t decoded_len = zeros + out_idx;
	zend_string *result = zend_string_alloc(decoded_len, 0);
	char *dst = ZSTR_VAL(result);

	for (size_t i = 0; i < zeros; i++) {
		*dst++ = 0;
	}
	for (size_t i = 0; i < out_idx; i++) {
		*dst++ = output[i];
	}

	size_t result_len = (size_t)(dst - ZSTR_VAL(result));
	ZSTR_LEN(result) = result_len;
	ZSTR_VAL(result)[result_len] = '\0';
	efree(output);
	efree(digits);

	return result;
}

PHP_FUNCTION(Encoding_base58_encode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base58)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base58 variant = ZEND_ENUM_Encoding_Base58_Bitcoin;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base58)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	RETURN_STR(base58_encode_impl(data, data_len, variant));
}

PHP_FUNCTION(Encoding_base58_decode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base58)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base58 variant = ZEND_ENUM_Encoding_Base58_Bitcoin;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base58)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	zend_string *result = base58_decode_impl(data, data_len, variant);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

/* ===================== Base64 ===================== */

static const char base64_standard_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base64_urlsafe_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static const char base64_imap_table[] = "-_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

static const uint8_t base64_standard_reverse[256] = {
	[0 ... 255] = 0xFF,
	['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,  ['F'] = 5,  ['G'] = 6,  ['H'] = 7,
	['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
	['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
	['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
	['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
	['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
	['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
	['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63
};

static const uint8_t base64_urlsafe_reverse[256] = {
	[0 ... 255] = 0xFF,
	['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,  ['F'] = 5,  ['G'] = 6,  ['H'] = 7,
	['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
	['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
	['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
	['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
	['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
	['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
	['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61, ['-'] = 62, ['_'] = 63
};

static const uint8_t base64_imap_reverse[256] = {
	[0 ... 255] = 0xFF,
	['-'] = 0,  ['_'] = 1,
	['A'] = 2,  ['B'] = 3,  ['C'] = 4,  ['D'] = 5,  ['E'] = 6,  ['F'] = 7,
	['G'] = 8,  ['H'] = 9,  ['I'] = 10, ['J'] = 11, ['K'] = 12, ['L'] = 13, ['O'] = 14, ['P'] = 15,
	['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
	['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
	['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
	['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
	['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
	['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61, ['N'] = 62, ['/'] = 63
};

static const uint8_t base64_forgiving_reverse[256] = {
	[0 ... 255] = 0xFF,
	['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,  ['F'] = 5,  ['G'] = 6,  ['H'] = 7,
	['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
	['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
	['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
	['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
	['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
	['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
	['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61,
	['+'] = 62, ['/'] = 62, ['-'] = 62, ['_'] = 62,
	['='] = 63, ['~'] = 63
};

static inline bool is_base64_whitespace(uint8_t c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static zend_string *base64_encode_impl(const char *data, size_t len, zend_enum_Encoding_Base64 variant, zend_enum_Encoding_PaddingMode padding) {
	const char *table;
	bool uses_padding;
	bool strip_padding;

	switch (variant) {
		case ZEND_ENUM_Encoding_Base64_Standard:
			table = base64_standard_table;
			uses_padding = true;
			break;
		case ZEND_ENUM_Encoding_Base64_UrlSafe:
			table = base64_urlsafe_table;
			uses_padding = false;
			break;
		case ZEND_ENUM_Encoding_Base64_Imap:
			table = base64_imap_table;
			uses_padding = true;
			break;
		default:
			table = base64_standard_table;
			uses_padding = true;
			break;
	}

	switch (padding) {
		case ZEND_ENUM_Encoding_PaddingMode_VariantControlled:
			strip_padding = !uses_padding;
			break;
		case ZEND_ENUM_Encoding_PaddingMode_StripPadding:
			strip_padding = true;
			break;
		case ZEND_ENUM_Encoding_PaddingMode_PreservePadding:
			strip_padding = false;
			break;
		default:
			strip_padding = false;
			break;
	}

	size_t encoded_len = ((len + 2) / 3) * 4;
	if (strip_padding) {
		size_t remainder = len % 3;
		if (remainder) {
			encoded_len -= 3 - remainder;
		}
	}

	zend_string *result = zend_string_alloc(encoded_len, 0);
	char *dst = ZSTR_VAL(result);

	size_t i = 0;
	while (i < len) {
		uint32_t nbits;
		size_t bytes_left = len - i;

		if (bytes_left >= 3) {
			nbits = ((uint8_t)data[i] << 16) | ((uint8_t)data[i+1] << 8) | (uint8_t)data[i+2];
			*dst++ = table[(nbits >> 18) & 0x3F];
			*dst++ = table[(nbits >> 12) & 0x3F];
			*dst++ = table[(nbits >> 6) & 0x3F];
			*dst++ = table[nbits & 0x3F];
			i += 3;
		} else {
			if (bytes_left == 2) {
				nbits = ((uint8_t)data[i] << 16) | ((uint8_t)data[i+1] << 8);
			} else {
				nbits = (uint8_t)data[i] << 16;
			}
			*dst++ = table[(nbits >> 18) & 0x3F];
			*dst++ = table[(nbits >> 12) & 0x3F];
			if (bytes_left == 2) {
				*dst++ = table[(nbits >> 6) & 0x3F];
			}
			if (!strip_padding) {
				*dst++ = '=';
				if (bytes_left == 1) {
					*dst++ = '=';
				}
			}
			i += bytes_left;
		}
	}

	ZSTR_LEN(result) = dst - ZSTR_VAL(result);
	ZSTR_VAL(result)[dst - ZSTR_VAL(result)] = '\0';
	return result;
}

static zend_string *base64_decode_impl(const char *data, size_t len, zend_enum_Encoding_Base64 variant, zend_enum_Encoding_DecodingMode mode) {
	const uint8_t *reverse_table;

	if (mode == ZEND_ENUM_Encoding_DecodingMode_Forgiving) {
		reverse_table = base64_forgiving_reverse;
	} else {
		switch (variant) {
			case ZEND_ENUM_Encoding_Base64_Standard:
				reverse_table = base64_standard_reverse;
				break;
			case ZEND_ENUM_Encoding_Base64_UrlSafe:
				reverse_table = base64_urlsafe_reverse;
				break;
			case ZEND_ENUM_Encoding_Base64_Imap:
				reverse_table = base64_imap_reverse;
				break;
			default:
				reverse_table = base64_standard_reverse;
				break;
		}
	}

	bool forgiving = (mode == ZEND_ENUM_Encoding_DecodingMode_Forgiving);

	/* Count content and padding */
	size_t content_len = 0;
	size_t padding_count = 0;
	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];
		if (is_base64_whitespace(c)) {
			continue;
		}
		if (c == '=') {
			padding_count++;
			continue;
		}
		content_len++;
	}

	/* Validate padding */
	if (forgiving) {
		if (padding_count > 2) {
			THROW_UNABLE_TO_DECODE("Invalid padding length");
			return NULL;
		}
	} else {
		size_t remainder = content_len % 4;
		if (remainder == 0) {
			if (padding_count != 0) {
				THROW_UNABLE_TO_DECODE("Invalid padding length");
				return NULL;
			}
		} else if (remainder == 2) {
			if (padding_count != 2) {
				THROW_UNABLE_TO_DECODE("Invalid padding length");
				return NULL;
			}
		} else if (remainder == 3) {
			if (padding_count != 1) {
				THROW_UNABLE_TO_DECODE("Invalid padding length");
				return NULL;
			}
		} else if (remainder == 1) {
			THROW_UNABLE_TO_DECODE("Invalid Base64 data");
			return NULL;
		}
	}

	size_t out_len = (content_len * 3) / 4;
	if (padding_count) {
		out_len -= padding_count;
	}
	zend_string *result = zend_string_alloc(out_len, 0);
	char *dst = ZSTR_VAL(result);
	size_t dst_idx = 0;

	uint32_t nbits = 0;
	int bits = 0;

	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];

		if (is_base64_whitespace(c)) {
			continue;
		}

		if (c == '=') {
			continue;
		}

		uint8_t val = reverse_table[c];

		if (val == 0xFF) {
			zend_string_efree(result);
			THROW_UNABLE_TO_DECODE("Invalid Base64 character");
			return NULL;
		}

		nbits = (nbits << 6) | val;
		bits += 6;

		while (bits >= 8) {
			bits -= 8;
			if (dst_idx < out_len) {
				dst[dst_idx++] = (char)((nbits >> bits) & 0xFF);
			}
		}
	}

	ZSTR_LEN(result) = dst_idx;
	ZSTR_VAL(result)[dst_idx] = '\0';
	return result;
}

PHP_FUNCTION(Encoding_base64_encode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *padding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base64)
		Z_PARAM_OBJECT_OF_CLASS(padding_obj, encoding_ce_PaddingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base64 variant = ZEND_ENUM_Encoding_Base64_Standard;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base64)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	zend_enum_Encoding_PaddingMode padding = ZEND_ENUM_Encoding_PaddingMode_VariantControlled;
	if (padding_obj) {
		padding = (zend_enum_Encoding_PaddingMode)zend_enum_fetch_case_id(Z_OBJ_P(padding_obj));
	}

	RETURN_STR(base64_encode_impl(data, data_len, variant, padding));
}

PHP_FUNCTION(Encoding_base64_decode) {
	char *data;
	size_t data_len;
	zval *variant_obj = NULL;
	zval *decoding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base64)
		Z_PARAM_OBJECT_OF_CLASS(decoding_obj, encoding_ce_DecodingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base64 variant = ZEND_ENUM_Encoding_Base64_Standard;
	if (variant_obj) {
		variant = (zend_enum_Encoding_Base64)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));
	}

	zend_enum_Encoding_DecodingMode mode = ZEND_ENUM_Encoding_DecodingMode_Strict;
	if (decoding_obj) {
		mode = (zend_enum_Encoding_DecodingMode)zend_enum_fetch_case_id(Z_OBJ_P(decoding_obj));
	}

	zend_string *result = base64_decode_impl(data, data_len, variant, mode);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

/* ===================== Base85 ===================== */

static const char base85_adobe_table[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstu";
static const char base85_z85_table[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@%$#";
static const char base85_git_table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!@$%^&*()-=+;:<>|~.#";

static const uint8_t base85_adobe_reverse[256] = {
	[0 ... 255] = 0xFF,
	['!'] = 0,  ['"'] = 1,  ['#'] = 2,  ['$'] = 3,  ['%'] = 4,  ['&'] = 5,  ['\''] = 6,  ['('] = 7,
	[')'] = 8,  ['*'] = 9,  ['+'] = 10,  [','] = 11,  ['-'] = 12,  ['.'] = 13,  ['/'] = 14,  ['0'] = 15,
	['1'] = 16,  ['2'] = 17,  ['3'] = 18,  ['4'] = 19,  ['5'] = 20,  ['6'] = 21,  ['7'] = 22,  ['8'] = 23,
	['9'] = 24,  [':'] = 25,  [';'] = 26,  ['<'] = 27,  ['='] = 28,  ['>'] = 29,  ['?'] = 30,  ['@'] = 31,
	['A'] = 32,  ['B'] = 33,  ['C'] = 34,  ['D'] = 35,  ['E'] = 36,  ['F'] = 37,  ['G'] = 38,  ['H'] = 39,
	['I'] = 40,  ['J'] = 41,  ['K'] = 42,  ['L'] = 43,  ['M'] = 44,  ['N'] = 45,  ['O'] = 46,  ['P'] = 47,
	['Q'] = 48,  ['R'] = 49,  ['S'] = 50,  ['T'] = 51,  ['U'] = 52,  ['V'] = 53,  ['W'] = 54,  ['X'] = 55,
	['Y'] = 56,  ['Z'] = 57,  ['['] = 58,  ['\\'] = 59,  [']'] = 60,  ['^'] = 61,  ['_'] = 62,  ['`'] = 63,
	['a'] = 64,  ['b'] = 65,  ['c'] = 66,  ['d'] = 67,  ['e'] = 68,  ['f'] = 69,  ['g'] = 70,  ['h'] = 71,
	['i'] = 72,  ['j'] = 73,  ['k'] = 74,  ['l'] = 75,  ['m'] = 76,  ['n'] = 77,  ['o'] = 78,  ['p'] = 79,
	['q'] = 80,  ['r'] = 81,  ['s'] = 82,  ['t'] = 83,  ['u'] = 84
};

static const uint8_t base85_z85_reverse[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,  ['4'] = 4,  ['5'] = 5,  ['6'] = 6,  ['7'] = 7,
	['8'] = 8,  ['9'] = 9,  ['a'] = 10,  ['b'] = 11,  ['c'] = 12,  ['d'] = 13,  ['e'] = 14,  ['f'] = 15,
	['g'] = 16,  ['h'] = 17,  ['i'] = 18,  ['j'] = 19,  ['k'] = 20,  ['l'] = 21,  ['m'] = 22,  ['n'] = 23,
	['o'] = 24,  ['p'] = 25,  ['q'] = 26,  ['r'] = 27,  ['s'] = 28,  ['t'] = 29,  ['u'] = 30,  ['v'] = 31,
	['w'] = 32,  ['x'] = 33,  ['y'] = 34,  ['z'] = 35,  ['A'] = 36,  ['B'] = 37,  ['C'] = 38,  ['D'] = 39,
	['E'] = 40,  ['F'] = 41,  ['G'] = 42,  ['H'] = 43,  ['I'] = 44,  ['J'] = 45,  ['K'] = 46,  ['L'] = 47,
	['M'] = 48,  ['N'] = 49,  ['O'] = 50,  ['P'] = 51,  ['Q'] = 52,  ['R'] = 53,  ['S'] = 54,  ['T'] = 55,
	['U'] = 56,  ['V'] = 57,  ['W'] = 58,  ['X'] = 59,  ['Y'] = 60,  ['Z'] = 61,  ['.'] = 62,  ['-'] = 63,
	[':'] = 64,  ['+'] = 65,  ['='] = 66,  ['^'] = 67,  ['!'] = 68,  ['/'] = 69,  ['*'] = 70,  ['?'] = 71,
	['&'] = 72,  ['<'] = 73,  ['>'] = 74,  ['('] = 75,  [')'] = 76,  ['['] = 77,  [']'] = 78,  ['{'] = 79,
	['}'] = 80,  ['@'] = 81,  ['%'] = 82,  ['$'] = 83,  ['#'] = 84
};

static const uint8_t base85_git_reverse[256] = {
	[0 ... 255] = 0xFF,
	['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,  ['4'] = 4,  ['5'] = 5,  ['6'] = 6,  ['7'] = 7,
	['8'] = 8,  ['9'] = 9,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15, ['G'] = 16, ['H'] = 17,
	['I'] = 18, ['J'] = 19, ['K'] = 20, ['L'] = 21, ['M'] = 22, ['N'] = 23, ['O'] = 24, ['P'] = 25,
	['Q'] = 26, ['R'] = 27, ['S'] = 28, ['T'] = 29, ['U'] = 30, ['V'] = 31, ['W'] = 32, ['X'] = 33,
	['Y'] = 34, ['Z'] = 35,
	['!'] = 36, ['@'] = 37, ['$'] = 38, ['%'] = 39, ['^'] = 40, ['&'] = 41,
	['*'] = 42, ['('] = 43, [')'] = 44, ['-'] = 45, ['='] = 46, ['+'] = 47,
	[';'] = 48, [':'] = 49, ['<'] = 50, ['>'] = 51, ['|'] = 52, ['~'] = 53, ['.'] = 54, ['#'] = 55
};

static zend_string *base85_encode_impl(const char *data, size_t len, zend_enum_Encoding_Base85 variant, zend_enum_Encoding_PaddingMode padding) {
	const char *table;
	bool uses_padding;

	switch (variant) {
		case ZEND_ENUM_Encoding_Base85_Adobe:
			table = base85_adobe_table;
			uses_padding = true;
			break;
		case ZEND_ENUM_Encoding_Base85_Z85:
			table = base85_z85_table;
			uses_padding = false;
			break;
		case ZEND_ENUM_Encoding_Base85_Git:
			table = base85_git_table;
			uses_padding = false;
			break;
		default:
			table = base85_adobe_table;
			uses_padding = true;
			break;
	}

	if (padding == ZEND_ENUM_Encoding_PaddingMode_PreservePadding && !uses_padding) {
		zend_value_error("PaddingMode::PreservePadding is not supported for variant %s",
			variant == ZEND_ENUM_Encoding_Base85_Z85 ? "Base85::Z85" : "Base85::Git");
		return NULL;
	}

	if (padding == ZEND_ENUM_Encoding_PaddingMode_StripPadding && !uses_padding) {
		zend_value_error("PaddingMode::StripPadding is not supported for variant %s",
			variant == ZEND_ENUM_Encoding_Base85_Z85 ? "Base85::Z85" : "Base85::Git");
		return NULL;
	}

	bool strip_padding = (padding == ZEND_ENUM_Encoding_PaddingMode_StripPadding);
	bool add_markers = uses_padding && !strip_padding;

	if (variant == ZEND_ENUM_Encoding_Base85_Z85 && len % 4 != 0) {
		zend_value_error("Z85 encoding requires input length to be a multiple of 4");
		return NULL;
	}

	/* Calculate total output length */
	size_t full_blocks = len / 4;
	size_t remainder = len % 4;
	size_t total_chars = full_blocks * 5;
	if (remainder) {
		if (strip_padding) {
			total_chars += remainder;
		} else {
			total_chars += remainder + 1;
		}
	}
	if (add_markers) total_chars += 4;

	zend_string *result = zend_string_alloc(total_chars, 0);
	char *dst = ZSTR_VAL(result);

	if (add_markers) {
		*dst++ = '<';
		*dst++ = '~';
	}

	for (size_t i = 0; i < len; i += 4) {
		size_t bytes_left = len - i;
		size_t bytes_in_block = bytes_left < 4 ? bytes_left : 4;

		uint32_t n = 0;
		for (size_t j = 0; j < bytes_in_block; j++) {
			n = (n << 8) | (uint8_t)data[i + j];
		}

		if (bytes_in_block == 4) {
			/* Full block: encode as 5 base-85 digits, MSD first */
			if (n == 0 && variant == ZEND_ENUM_Encoding_Base85_Adobe) {
				/* Adobe: all-zero block encoded as single 'z' */
				*dst++ = 'z';
			} else {
				uint32_t t = n;
				uint8_t digits[5];
				for (int d = 4; d >= 0; d--) {
					digits[d] = (uint8_t)(t % 85);
					t /= 85;
				}
				for (int d = 0; d < 5; d++) {
					*dst++ = table[digits[d]];
				}
			}
		} else {
			/* Partial block: left-shift to align, then divide by 85 */
			n <<= (4 - bytes_in_block) * 8;

			if (strip_padding) {
				/* Output only bytes_in_block chars */
				uint32_t t = n;
				uint8_t digits[5];
				for (int d = 4; d >= 0; d--) {
					digits[d] = (uint8_t)(t % 85);
					t /= 85;
				}
				for (int d = 0; d < (int)bytes_in_block; d++) {
					*dst++ = table[digits[d]];
				}
			} else {
				/* Output bytes_in_block + 1 chars */
				uint32_t t = n;
				uint8_t digits[5];
				for (int d = 4; d >= 0; d--) {
					digits[d] = (uint8_t)(t % 85);
					t /= 85;
				}
				for (int d = 0; d <= (int)bytes_in_block; d++) {
					*dst++ = table[digits[d]];
				}
			}
		}
	}

	if (add_markers) {
		*dst++ = '~';
		*dst++ = '>';
	}

	ZSTR_LEN(result) = dst - ZSTR_VAL(result);
	ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';
	return result;
}

static zend_string *base85_decode_impl(const char *data, size_t len, zend_enum_Encoding_Base85 variant, zend_enum_Encoding_DecodingMode mode) {
	const uint8_t *reverse_table;

	switch (variant) {
		case ZEND_ENUM_Encoding_Base85_Adobe:
			reverse_table = base85_adobe_reverse;
			break;
		case ZEND_ENUM_Encoding_Base85_Z85:
			reverse_table = base85_z85_reverse;
			break;
		case ZEND_ENUM_Encoding_Base85_Git:
			reverse_table = base85_git_reverse;
			break;
		default:
			reverse_table = base85_adobe_reverse;
			break;
	}

	bool forgiving = (mode == ZEND_ENUM_Encoding_DecodingMode_Forgiving);

	/* For Adobe variant, strip <~ and ~> markers if present */
	if (variant == ZEND_ENUM_Encoding_Base85_Adobe && len >= 4 && data[0] == '<' && data[1] == '~') {
		if (data[len - 2] == '~' && data[len - 1] == '>') {
			data += 2;
			len -= 2;
		} else if (forgiving) {
			data += 2;
			len -= 2;
		} else {
			THROW_UNABLE_TO_DECODE("Invalid Adobe Base85 format: missing closing marker");
			return NULL;
		}
	}

	/* Collect non-whitespace characters */
	uint8_t *content = emalloc(len);
	size_t content_len = 0;
	for (size_t i = 0; i < len; i++) {
		uint8_t c = (uint8_t)data[i];
		if (is_base64_whitespace(c)) {
			continue;
		}
		content[content_len++] = c;
	}

	/* Z85 requires multiple of 5 */
	if (variant == ZEND_ENUM_Encoding_Base85_Z85 && content_len % 5 != 0) {
		efree(content);
		THROW_UNABLE_TO_DECODE("Z85 encoded data must have length multiple of 5");
		return NULL;
	}

	/* Calculate output length and allocate */
	size_t full_blocks = 0;
	size_t z_blocks = 0;
	size_t remainder = content_len;

	for (size_t i = 0; i < remainder; ) {
		if (content[i] == 'z' && variant == ZEND_ENUM_Encoding_Base85_Adobe) {
			z_blocks++;
			i++;
			remainder--;
		} else {
			if (remainder >= 5) {
				full_blocks++;
				i += 5;
				remainder -= 5;
			} else {
				break;
			}
		}
	}

	size_t out_len = full_blocks * 4 + z_blocks * 4;
	if (remainder) {
		out_len += remainder - 1;
	}

	zend_string *result = zend_string_alloc(out_len, 0);
	char *dst = ZSTR_VAL(result);

	/* Decode content */
	size_t i = 0;
	while (i < content_len) {
		if (content[i] == 'z' && variant == ZEND_ENUM_Encoding_Base85_Adobe) {
			/* Adobe 'z' shortcut: 4 zero bytes */
			*dst++ = '\0';
			*dst++ = '\0';
			*dst++ = '\0';
			*dst++ = '\0';
			i++;
			continue;
		}

		/* Count characters in this block (non-'z') */
		size_t block_start = i;
		size_t chars_in_block = 0;
		while (i < content_len && chars_in_block < 5) {
			if (content[i] == 'z' && variant == ZEND_ENUM_Encoding_Base85_Adobe) {
				break; /* 'z' starts a new block */
			}
			i++;
			chars_in_block++;
		}

		/* Decode this block */
		uint32_t n = 0;
		size_t pad_count = 5 - chars_in_block;
		for (size_t j = block_start; j < i; j++) {
			uint8_t val = reverse_table[content[j]];
			if (val == 0xFF || val >= 85) {
				efree(content);
				zend_string_efree(result);
				THROW_UNABLE_TO_DECODE("Invalid Base85 character");
				return NULL;
			}
			n = n * 85 + val;
		}
		/* Pad with 84 ('u') for partial blocks */
		for (size_t j = 0; j < pad_count; j++) {
			n = n * 85 + 84;
		}

		/* Extract bytes */
		size_t bytes_to_output = (chars_in_block == 5) ? 4 : (chars_in_block - 1);
		for (size_t j = 0; j < bytes_to_output; j++) {
			*dst++ = (char)((n >> ((4 - j - 1) * 8)) & 0xFF);
		}
	}

	efree(content);
	ZSTR_LEN(result) = dst - ZSTR_VAL(result);
	ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';
	return result;
}

PHP_FUNCTION(Encoding_base85_encode) {
	char *data;
	size_t data_len;
	zval *variant_obj;
	zval *padding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base85)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(padding_obj, encoding_ce_PaddingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base85 variant = (zend_enum_Encoding_Base85)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));

	zend_enum_Encoding_PaddingMode padding = ZEND_ENUM_Encoding_PaddingMode_VariantControlled;
	if (padding_obj) {
		padding = (zend_enum_Encoding_PaddingMode)zend_enum_fetch_case_id(Z_OBJ_P(padding_obj));
	}

	zend_string *result = base85_encode_impl(data, data_len, variant, padding);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

PHP_FUNCTION(Encoding_base85_decode) {
	char *data;
	size_t data_len;
	zval *variant_obj;
	zval *decoding_obj = NULL;
	zval *timing_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OBJECT_OF_CLASS(variant_obj, encoding_ce_Base85)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(decoding_obj, encoding_ce_DecodingMode)
		Z_PARAM_OBJECT_OF_CLASS(timing_obj, encoding_ce_TimingMode)
	ZEND_PARSE_PARAMETERS_END();

	zend_enum_Encoding_Base85 variant = (zend_enum_Encoding_Base85)zend_enum_fetch_case_id(Z_OBJ_P(variant_obj));

	zend_enum_Encoding_DecodingMode mode = ZEND_ENUM_Encoding_DecodingMode_Strict;
	if (decoding_obj) {
		mode = (zend_enum_Encoding_DecodingMode)zend_enum_fetch_case_id(Z_OBJ_P(decoding_obj));
	}

	zend_string *result = base85_decode_impl(data, data_len, variant, mode);
	if (!result) {
		return;
	}
	RETURN_STR(result);
}

/* ===================== Module Registration ===================== */

static PHP_MINIT_FUNCTION(encoding)
{
	encoding_ce_EncodingError = register_class_Encoding_EncodingError(zend_ce_error);
	encoding_ce_EncodingException = register_class_Encoding_EncodingException(zend_ce_exception);
	encoding_ce_UnableToDecodeException = register_class_Encoding_UnableToDecodeException(encoding_ce_EncodingException);
	encoding_ce_UnableToEncodeException = register_class_Encoding_UnableToEncodeException(encoding_ce_EncodingException);

	encoding_ce_Base16 = register_class_Encoding_Base16();
	encoding_ce_Base32 = register_class_Encoding_Base32();
	encoding_ce_Base58 = register_class_Encoding_Base58();
	encoding_ce_Base64 = register_class_Encoding_Base64();
	encoding_ce_Base85 = register_class_Encoding_Base85();
	encoding_ce_PaddingMode = register_class_Encoding_PaddingMode();
	encoding_ce_DecodingMode = register_class_Encoding_DecodingMode();
	encoding_ce_TimingMode = register_class_Encoding_TimingMode();

	return SUCCESS;
}

zend_module_entry encoding_module_entry = {
	STANDARD_MODULE_HEADER,
	"encoding",
	ext_functions,
	PHP_MINIT(encoding),
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ENCODING
ZEND_GET_MODULE(encoding)
#endif
