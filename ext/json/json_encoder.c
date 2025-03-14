/*
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
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "ext/standard/html.h"
#include "zend_smart_str.h"
#include "php_json.h"
#include "php_json_encoder.h"
#include "zend_portability.h"
#include <zend_exceptions.h>
#include "zend_enum.h"
#include "zend_property_hooks.h"
#include "zend_lazy_objects.h"
#include "zend_bitset.h"

#if defined(ZEND_INTRIN_SSE4_2_NATIVE) || defined(ZEND_INTRIN_SSE4_2_FUNC_PROTO)
# include <nmmintrin.h>
#endif
#ifdef ZEND_INTRIN_SSE4_2_FUNC_PROTO
# include "zend_cpuinfo.h"
#endif

#ifdef __SSE2__
# define JSON_USE_SIMD
#endif

typedef enum php_json_simd_result {
	PHP_JSON_STOP,
	PHP_JSON_SLOW,
	PHP_JSON_NON_ASCII,
} php_json_simd_result;

/* Specialization of smart_str_appendl() to avoid performance loss due to code bloat */
static zend_always_inline void php_json_append(smart_str *dest, const char *src, size_t len)
{
	/* dest has a minimum size of the input length,
	 * this avoids generating initial allocation code */
	ZEND_ASSERT(dest->s);

	smart_str_appendl(dest, src, len);
}

static zend_always_inline bool php_json_printable_ascii_escape(smart_str *buf, unsigned char us, int options)
{
	ZEND_ASSERT(buf->s);

	switch (us) {
		case '"':
			if (options & PHP_JSON_HEX_QUOT) {
				php_json_append(buf, "\\u0022", 6);
			} else {
				php_json_append(buf, "\\\"", 2);
			}
			break;

		case '\\':
			php_json_append(buf, "\\\\", 2);
			break;

		case '/':
			if (options & PHP_JSON_UNESCAPED_SLASHES) {
				smart_str_appendc(buf, '/');
			} else {
				php_json_append(buf, "\\/", 2);
			}
			break;

		case '<':
			if (options & PHP_JSON_HEX_TAG) {
				php_json_append(buf, "\\u003C", 6);
			} else {
				smart_str_appendc(buf, '<');
			}
			break;

		case '>':
			if (options & PHP_JSON_HEX_TAG) {
				php_json_append(buf, "\\u003E", 6);
			} else {
				smart_str_appendc(buf, '>');
			}
			break;

		case '&':
			if (options & PHP_JSON_HEX_AMP) {
				php_json_append(buf, "\\u0026", 6);
			} else {
				smart_str_appendc(buf, '&');
			}
			break;

		case '\'':
			if (options & PHP_JSON_HEX_APOS) {
				php_json_append(buf, "\\u0027", 6);
			} else {
				smart_str_appendc(buf, '\'');
			}
			break;

		default:
			return false;
	}

	return true;
}

#ifdef JSON_USE_SIMD
static zend_always_inline int php_json_sse2_compute_escape_intersection(const __m128i mask, const __m128i input)
{
	(void) mask;

	const __m128i result_34 = _mm_cmpeq_epi8(input, _mm_set1_epi8('"'));
	const __m128i result_38 = _mm_cmpeq_epi8(input, _mm_set1_epi8('&'));
	const __m128i result_39 = _mm_cmpeq_epi8(input, _mm_set1_epi8('\''));
	const __m128i result_47 = _mm_cmpeq_epi8(input, _mm_set1_epi8('/'));
	const __m128i result_60 = _mm_cmpeq_epi8(input, _mm_set1_epi8('<'));
	const __m128i result_62 = _mm_cmpeq_epi8(input, _mm_set1_epi8('>'));
	const __m128i result_92 = _mm_cmpeq_epi8(input, _mm_set1_epi8('\\'));

	const __m128i result_34_38 = _mm_or_si128(result_34, result_38);
	const __m128i result_39_47 = _mm_or_si128(result_39, result_47);
	const __m128i result_60_62 = _mm_or_si128(result_60, result_62);

	const __m128i result_34_38_39_47 = _mm_or_si128(result_34_38, result_39_47);
	const __m128i result_60_62_92 = _mm_or_si128(result_60_62, result_92);

	const __m128i result_individual_bytes = _mm_or_si128(result_34_38_39_47, result_60_62_92);
	return _mm_movemask_epi8(result_individual_bytes);
}

#if defined(ZEND_INTRIN_SSE4_2_NATIVE) || defined(ZEND_INTRIN_SSE4_2_FUNC_PROTO)
static const char php_json_escape_noslashes_lut[2][8][16] = {
	/* !PHP_JSON_UNESCAPED_SLASHES */
	{
		[0] = {'"', '\\', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP] = {'"', '\\', '&', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_APOS] = {'"', '\\', '\'', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP|PHP_JSON_HEX_APOS] = {'"', '\\', '&', '\'', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_TAG] = {'"', '\\', '<', '>', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP|PHP_JSON_HEX_TAG] = {'"', '\\', '&', '<', '>', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_APOS|PHP_JSON_HEX_TAG] = {'"', '\\', '\'', '<', '>', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP|PHP_JSON_HEX_APOS|PHP_JSON_HEX_TAG] = {'"', '\\', '&', '\'', '<', '>', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},

	/* PHP_JSON_UNESCAPED_SLASHES */
	{
		[0] = {'"', '\\', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP] = {'"', '\\', '&', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_APOS] = {'"', '\\', '\'', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP|PHP_JSON_HEX_APOS] = {'"', '\\', '&', '\'', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_TAG] = {'"', '\\', '<', '>', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP|PHP_JSON_HEX_TAG] = {'"', '\\', '&', '<', '>', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_APOS|PHP_JSON_HEX_TAG] = {'"', '\\', '\'', '<', '>', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		[PHP_JSON_HEX_AMP|PHP_JSON_HEX_APOS|PHP_JSON_HEX_TAG] = {'"', '\\', '&', '\'', '<', '>', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	}
};

static zend_always_inline __m128i php_json_create_sse_escape_mask(int options)
{
	const int slashes = (options & PHP_JSON_UNESCAPED_SLASHES) ? 1 : 0;
	const int masked = options & (PHP_JSON_HEX_AMP|PHP_JSON_HEX_APOS|PHP_JSON_HEX_TAG);
	return *(const __m128i *) &php_json_escape_noslashes_lut[slashes][masked];
}

ZEND_INTRIN_SSE4_2_FUNC_DECL(int php_json_sse42_compute_escape_intersection_real(const __m128i mask, const __m128i input));
zend_always_inline int php_json_sse42_compute_escape_intersection_real(const __m128i mask, const __m128i input)
{
	const __m128i result_individual_bytes = _mm_cmpistrm(mask, input, _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_BIT_MASK);
	return _mm_cvtsi128_si32(result_individual_bytes);
}
#endif

#ifdef ZEND_INTRIN_SSE4_2_FUNC_PROTO
static int php_json_sse42_compute_escape_intersection(const __m128i mask, const __m128i input) __attribute__((ifunc("php_json_resolve_escape_intersection")));

typedef int (*php_json_compute_escape_intersection_t)(const __m128i mask, const __m128i input);

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED /* clang mistakenly warns about this */
static php_json_compute_escape_intersection_t php_json_resolve_escape_intersection(void) {
	if (zend_cpu_supports_sse42()) {
		return php_json_sse42_compute_escape_intersection_real;
	}
	return php_json_sse2_compute_escape_intersection;
}
#endif

static zend_always_inline php_json_simd_result php_json_process_simd_block(
	smart_str *buf,
	const __m128i sse_escape_mask,
	const char **restrict s,
	size_t *restrict pos,
	size_t *restrict len,
	int options
)
{
	while (*len >= sizeof(__m128i)) {
		const __m128i input = _mm_loadu_si128((const __m128i *) (*s + *pos));
		/* signed compare, so checks for unsigned bytes >= 0x80 as well */
		const __m128i input_range = _mm_cmplt_epi8(input, _mm_set1_epi8(32));

		int max_shift = sizeof(__m128i);

		int input_range_mask = _mm_movemask_epi8(input_range);
		if (input_range_mask != 0) {
			if (UNEXPECTED(input_range_mask & 1)) {
				/* not worth it */
				return PHP_JSON_NON_ASCII;
			}
			max_shift = zend_ulong_ntz(input_range_mask);
		}

#ifdef ZEND_INTRIN_SSE4_2_NATIVE
		int mask = php_json_sse42_compute_escape_intersection_real(sse_escape_mask, input);
#elif defined(ZEND_INTRIN_SSE4_2_FUNC_PROTO)
		int mask = php_json_sse42_compute_escape_intersection(sse_escape_mask, input);
#else
		int mask = php_json_sse2_compute_escape_intersection(_mm_setzero_si128(), input);
#endif
		if (mask != 0) {
			if (UNEXPECTED(max_shift < sizeof(__m128i))) {
				int shift = zend_ulong_ntz(mask); /* first offending character */
				*pos += MIN(max_shift, shift);
				*len -= MIN(max_shift, shift);
				return PHP_JSON_SLOW;
			}

			php_json_append(buf, *s, *pos);
			*s += *pos;
			const char *s_backup = *s;

			/* It's more important to keep this loop tight than to optimize this with
			 * a trailing zero count. */
			for (; mask; mask >>= 1, *s += 1) {
				if (UNEXPECTED(mask & 1)) {
					bool handled = php_json_printable_ascii_escape(buf, (*s)[0], options);
					ZEND_ASSERT(handled);
				} else {
					ZEND_ASSERT(buf->s);
					smart_str_appendc(buf, (*s)[0]);
				}
			}

			*pos = sizeof(__m128i) - (*s - s_backup);
		} else {
			if (max_shift < sizeof(__m128i)) {
				*pos += max_shift;
				*len -= max_shift;
				return PHP_JSON_SLOW;
			}
			*pos += sizeof(__m128i);
		}

		*len -= sizeof(__m128i);
	}

	return UNEXPECTED(!*len) ? PHP_JSON_STOP : PHP_JSON_SLOW;
}

# if defined(ZEND_INTRIN_SSE4_2_NATIVE) || defined(ZEND_INTRIN_SSE4_2_FUNC_PROTO)
#  define JSON_DEFINE_ESCAPE_MASK(name, options) const __m128i name = php_json_create_sse_escape_mask(options)
# else
#  define JSON_DEFINE_ESCAPE_MASK(name, options) const __m128i name = _mm_setzero_si128()
# endif
#else
# define JSON_DEFINE_ESCAPE_MASK(name, options)
#endif

static const char digits[] = "0123456789abcdef";

static zend_always_inline bool php_json_check_stack_limit(void)
{
#ifdef ZEND_CHECK_STACK_LIMIT
	return zend_call_stack_overflowed(EG(stack_limit));
#else
	return false;
#endif
}

static int php_json_determine_array_type(zval *val) /* {{{ */
{
	zend_array *myht = Z_ARRVAL_P(val);

	if (myht) {
		return zend_array_is_list(myht) ? PHP_JSON_OUTPUT_ARRAY : PHP_JSON_OUTPUT_OBJECT;
	}

	return PHP_JSON_OUTPUT_ARRAY;
}
/* }}} */

/* {{{ Pretty printing support functions */

static inline void php_json_pretty_print_char(smart_str *buf, int options, char c) /* {{{ */
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		smart_str_appendc(buf, c);
	}
}
/* }}} */

static inline void php_json_pretty_print_indent(smart_str *buf, int options, php_json_encoder *encoder) /* {{{ */
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < encoder->depth; ++i) {
			smart_str_appendl(buf, "    ", 4);
		}
	}
}
/* }}} */

/* }}} */

static
#if defined(_MSC_VER) && defined(_M_ARM64)
// MSVC bug: https://developercommunity.visualstudio.com/t/corrupt-optimization-on-arm64-with-Ox-/10102551
zend_never_inline
#else
inline
#endif
bool php_json_is_valid_double(double d) /* {{{ */
{
	return !zend_isinf(d) && !zend_isnan(d);
}
/* }}} */

static inline void php_json_encode_double(smart_str *buf, double d, int options) /* {{{ */
{
	size_t len;
	char num[ZEND_DOUBLE_MAX_LENGTH];

	zend_gcvt(d, (int)PG(serialize_precision), '.', 'e', num);
	len = strlen(num);
	if (options & PHP_JSON_PRESERVE_ZERO_FRACTION && strchr(num, '.') == NULL && len < ZEND_DOUBLE_MAX_LENGTH - 2) {
		num[len++] = '.';
		num[len++] = '0';
		num[len] = '\0';
	}
	smart_str_appendl(buf, num, len);
}
/* }}} */

#define PHP_JSON_HASH_PROTECT_RECURSION(_tmp_ht) \
	do { \
		if (_tmp_ht) { \
			GC_TRY_PROTECT_RECURSION(_tmp_ht); \
		} \
	} while (0)

#define PHP_JSON_HASH_UNPROTECT_RECURSION(_tmp_ht) \
	do { \
		if (_tmp_ht) { \
			GC_TRY_UNPROTECT_RECURSION(_tmp_ht); \
		} \
	} while (0)

static zend_result php_json_encode_array(smart_str *buf, zval *val, int options, php_json_encoder *encoder) /* {{{ */
{
	int r, need_comma = 0;
	HashTable *myht, *prop_ht;
	zend_refcounted *recursion_rc;

	if (php_json_check_stack_limit()) {
		encoder->error_code = PHP_JSON_ERROR_DEPTH;
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			smart_str_appendl(buf, "null", 4);
		}
		return FAILURE;
	}

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
		recursion_rc = (zend_refcounted *)myht;
		prop_ht = NULL;
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : php_json_determine_array_type(val);
	} else if (Z_OBJ_P(val)->properties == NULL
	 && Z_OBJ_HT_P(val)->get_properties_for == NULL
	 && Z_OBJ_HT_P(val)->get_properties == zend_std_get_properties
	 && Z_OBJ_P(val)->ce->num_hooked_props == 0
	 && !zend_object_is_lazy(Z_OBJ_P(val))) {
		/* Optimized version without rebuilding properties HashTable */
		zend_object *obj = Z_OBJ_P(val);
		zend_class_entry *ce = obj->ce;
		zend_property_info *prop_info;
		zval *prop;

		if (GC_IS_RECURSIVE(obj)) {
			encoder->error_code = PHP_JSON_ERROR_RECURSION;
			smart_str_appendl(buf, "null", 4);
			return FAILURE;
		}

		PHP_JSON_HASH_PROTECT_RECURSION(obj);

		smart_str_appendc(buf, '{');

		++encoder->depth;

		for (int i = 0; i < ce->default_properties_count; i++) {
			prop_info = ce->properties_info_table[i];
			if (!prop_info) {
				continue;
			}
			if (ZSTR_VAL(prop_info->name)[0] == '\0' && ZSTR_LEN(prop_info->name) > 0) {
				/* Skip protected and private members. */
				continue;
			}
			prop = OBJ_PROP(obj, prop_info->offset);
			if (Z_TYPE_P(prop) == IS_UNDEF) {
				continue;
			}

			if (need_comma) {
				smart_str_appendc(buf, ',');
			} else {
				need_comma = 1;
			}

			php_json_pretty_print_char(buf, options, '\n');
			php_json_pretty_print_indent(buf, options, encoder);

			if (php_json_escape_string(buf, ZSTR_VAL(prop_info->name), ZSTR_LEN(prop_info->name),
					options & ~PHP_JSON_NUMERIC_CHECK, encoder) == FAILURE &&
					(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) &&
					buf->s) {
				ZSTR_LEN(buf->s) -= 4;
				smart_str_appendl(buf, "\"\"", 2);
			}

			smart_str_appendc(buf, ':');
			php_json_pretty_print_char(buf, options, ' ');

			if (php_json_encode_zval(buf, prop, options, encoder) == FAILURE &&
					!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				PHP_JSON_HASH_UNPROTECT_RECURSION(obj);
				return FAILURE;
			}
		}

		PHP_JSON_HASH_UNPROTECT_RECURSION(obj);
		if (encoder->depth > encoder->max_depth) {
			encoder->error_code = PHP_JSON_ERROR_DEPTH;
			if (!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				return FAILURE;
			}
		}
		--encoder->depth;

		if (need_comma) {
			php_json_pretty_print_char(buf, options, '\n');
			php_json_pretty_print_indent(buf, options, encoder);
		}
		smart_str_appendc(buf, '}');
		return SUCCESS;
	} else {
		zend_object *obj = Z_OBJ_P(val);
		prop_ht = myht = zend_get_properties_for(val, ZEND_PROP_PURPOSE_JSON);
		if (obj->ce->num_hooked_props == 0) {
			recursion_rc = (zend_refcounted *)prop_ht;
		} else {
			/* Protecting the object itself is fine here because myht is temporary and can't be
			 * referenced from a different place in the object graph. */
			recursion_rc = (zend_refcounted *)obj;
		}
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (recursion_rc && GC_IS_RECURSIVE(recursion_rc)) {
		encoder->error_code = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		zend_release_properties(prop_ht);
		return FAILURE;
	}

	PHP_JSON_HASH_PROTECT_RECURSION(recursion_rc);

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, '[');
	} else {
		smart_str_appendc(buf, '{');
	}

	++encoder->depth;

	uint32_t i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;

		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {
			zval tmp;
			ZVAL_UNDEF(&tmp);

			if (r == PHP_JSON_OUTPUT_ARRAY) {
				ZEND_ASSERT(Z_TYPE_P(data) != IS_PTR);

				if (need_comma) {
					smart_str_appendc(buf, ',');
				} else {
					need_comma = 1;
				}

				php_json_pretty_print_char(buf, options, '\n');
				php_json_pretty_print_indent(buf, options, encoder);
			} else if (r == PHP_JSON_OUTPUT_OBJECT) {
				if (key) {
					if (ZSTR_VAL(key)[0] == '\0' && ZSTR_LEN(key) > 0 && Z_TYPE_P(val) == IS_OBJECT) {
						/* Skip protected and private members. */
						continue;
					}

					/* data is IS_PTR for properties with hooks. */
					if (UNEXPECTED(Z_TYPE_P(data) == IS_PTR)) {
						zend_property_info *prop_info = Z_PTR_P(data);
						if ((prop_info->flags & ZEND_ACC_VIRTUAL) && !prop_info->hooks[ZEND_PROPERTY_HOOK_GET]) {
							continue;
						}
						data = zend_read_property_ex(prop_info->ce, Z_OBJ_P(val), prop_info->name, /* silent */ true, &tmp);
						if (EG(exception)) {
							PHP_JSON_HASH_UNPROTECT_RECURSION(recursion_rc);
							zend_release_properties(prop_ht);
							return FAILURE;
						}
					}

					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options, encoder);

					if (php_json_escape_string(buf, ZSTR_VAL(key), ZSTR_LEN(key),
								options & ~PHP_JSON_NUMERIC_CHECK, encoder) == FAILURE &&
							(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) &&
							buf->s) {
						ZSTR_LEN(buf->s) -= 4;
						smart_str_appendl(buf, "\"\"", 2);
					}
				} else {
					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options, encoder);

					smart_str_appendc(buf, '"');
					smart_str_append_long(buf, (zend_long) index);
					smart_str_appendc(buf, '"');
				}

				smart_str_appendc(buf, ':');
				php_json_pretty_print_char(buf, options, ' ');
			}

			if (php_json_encode_zval(buf, data, options, encoder) == FAILURE &&
					!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				PHP_JSON_HASH_UNPROTECT_RECURSION(recursion_rc);
				zend_release_properties(prop_ht);
				zval_ptr_dtor(&tmp);
				return FAILURE;
			}
			zval_ptr_dtor(&tmp);
		} ZEND_HASH_FOREACH_END();
	}

	PHP_JSON_HASH_UNPROTECT_RECURSION(recursion_rc);

	if (encoder->depth > encoder->max_depth) {
		encoder->error_code = PHP_JSON_ERROR_DEPTH;
		if (!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
			zend_release_properties(prop_ht);
			return FAILURE;
		}
	}
	--encoder->depth;

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		php_json_pretty_print_char(buf, options, '\n');
		php_json_pretty_print_indent(buf, options, encoder);
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}

	zend_release_properties(prop_ht);
	return SUCCESS;
}
/* }}} */

zend_result php_json_escape_string(
		smart_str *buf, const char *s, size_t len,
		int options, php_json_encoder *encoder) /* {{{ */
{
	unsigned int us;
	size_t pos, checkpoint;
	char *dst;

	if (len == 0) {
		smart_str_appendl(buf, "\"\"", 2);
		return SUCCESS;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		zend_long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				smart_str_append_long(buf, p);
				return SUCCESS;
			} else if (type == IS_DOUBLE && php_json_is_valid_double(d)) {
				php_json_encode_double(buf, d, options);
				return SUCCESS;
			}
		}

	}

	/* pre-allocate for string length plus 2 quotes */
	smart_str_alloc(buf, len+2, 0);
	checkpoint = ZSTR_LEN(buf->s);
	smart_str_appendc(buf, '"');

	pos = 0;

	JSON_DEFINE_ESCAPE_MASK(sse_escape_mask, options);

	do {
		static const uint32_t charmap[8] = {
			0xffffffff, 0x500080c4, 0x10000000, 0x00000000,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

		php_json_simd_result result = PHP_JSON_SLOW;
#ifdef JSON_USE_SIMD
		result = php_json_process_simd_block(buf, sse_escape_mask, &s, &pos, &len, options);
		if (UNEXPECTED(result == PHP_JSON_STOP)) {
			break;
		}
#endif

		us = (unsigned char)s[pos];
		if (EXPECTED(result != PHP_JSON_NON_ASCII && !ZEND_BIT_TEST(charmap, us))) {
			pos++;
			len--;
		} else {
			if (UNEXPECTED(us >= 0x80)) {
				size_t pos_old = pos;
				const char *cur = s + pos;
				pos = 0;
				us = php_next_utf8_char_mb((unsigned char *)cur, us, len, &pos);
				len -= pos;
				pos += pos_old;

				/* check whether UTF8 character is correct */
				if (UNEXPECTED(!us)) {
					if (pos_old && (options & (PHP_JSON_INVALID_UTF8_IGNORE|PHP_JSON_INVALID_UTF8_SUBSTITUTE))) {
						php_json_append(buf, s, pos_old);
					}
					s += pos;
					pos = 0;

					if (options & PHP_JSON_INVALID_UTF8_IGNORE) {
						/* ignore invalid UTF8 character */
					} else if (options & PHP_JSON_INVALID_UTF8_SUBSTITUTE) {
						/* Use Unicode character 'REPLACEMENT CHARACTER' (U+FFFD) */
						if (options & PHP_JSON_UNESCAPED_UNICODE) {
							php_json_append(buf, "\xef\xbf\xbd", 3);
						} else {
							php_json_append(buf, "\\ufffd", 6);
						}
					} else {
						ZSTR_LEN(buf->s) = checkpoint;
						encoder->error_code = PHP_JSON_ERROR_UTF8;
						if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
							php_json_append(buf, "null", 4);
						}
						return FAILURE;
					}

				/* Escape U+2028/U+2029 line terminators, UNLESS both
				   JSON_UNESCAPED_UNICODE and
				   JSON_UNESCAPED_LINE_TERMINATORS were provided */
				} else if ((options & PHP_JSON_UNESCAPED_UNICODE)
				    && ((options & PHP_JSON_UNESCAPED_LINE_TERMINATORS)
						|| us < 0x2028 || us > 0x2029)) {
					/* No need to emit any bytes, just move the cursor. */
				} else {
					php_json_append(buf, s, pos_old);
					s += pos;
					pos = 0;

					ZEND_ASSERT(buf->s);

					/* From http://en.wikipedia.org/wiki/UTF16 */
					dst = smart_str_extend(buf, 6 + ((us >= 0x10000) ? 6 : 0));
					if (us >= 0x10000) {
						unsigned int next_us;

						us -= 0x10000;
						next_us = (unsigned short)((us & 0x3ff) | 0xdc00);
						us = (unsigned short)((us >> 10) | 0xd800);
						dst[0] = '\\';
						dst[1] = 'u';
						dst[2] = digits[(us >> 12) & 0xf];
						dst[3] = digits[(us >> 8) & 0xf];
						dst[4] = digits[(us >> 4) & 0xf];
						dst[5] = digits[us & 0xf];
						us = next_us;
						dst += 6;
					}
					dst[0] = '\\';
					dst[1] = 'u';
					dst[2] = digits[(us >> 12) & 0xf];
					dst[3] = digits[(us >> 8) & 0xf];
					dst[4] = digits[(us >> 4) & 0xf];
					dst[5] = digits[us & 0xf];
				}
			} else {
				if (pos) {
					php_json_append(buf, s, pos);
					s += pos;
					pos = 0;
				}
				s++;
				switch (us) {
					case '\b':
						php_json_append(buf, "\\b", 2);
						break;

					case '\f':
						php_json_append(buf, "\\f", 2);
						break;

					case '\n':
						php_json_append(buf, "\\n", 2);
						break;

					case '\r':
						php_json_append(buf, "\\r", 2);
						break;

					case '\t':
						php_json_append(buf, "\\t", 2);
						break;

					default:
						if (!php_json_printable_ascii_escape(buf, us, options)) {
							ZEND_ASSERT(us < ' ');
							dst = smart_str_extend(buf, 6);
							dst[0] = '\\';
							dst[1] = 'u';
							dst[2] = '0';
							dst[3] = '0';
							dst[4] = digits[(us >> 4) & 0xf];
							dst[5] = digits[us & 0xf];
						}
						break;
				}
				len--;
			}
		}
	} while (len);

	php_json_append(buf, s, pos);

	ZEND_ASSERT(buf->s);
	smart_str_appendc(buf, '"');

	return SUCCESS;
}
/* }}} */

static zend_result php_json_encode_serializable_object(smart_str *buf, zend_object *obj, int options, php_json_encoder *encoder)
{
	zend_class_entry *ce = obj->ce;
	uint32_t *guard = zend_get_recursion_guard(obj);
	zval retval;
	zend_result return_code;

	ZEND_ASSERT(guard != NULL);

	if (ZEND_GUARD_IS_RECURSIVE(guard, JSON)) {
		encoder->error_code = PHP_JSON_ERROR_RECURSION;
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			smart_str_appendl(buf, "null", 4);
		}
		return FAILURE;
	}

	ZEND_GUARD_PROTECT_RECURSION(guard, JSON);

	zend_function *json_serialize_method = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("jsonserialize"));
	ZEND_ASSERT(json_serialize_method != NULL && "This should be guaranteed prior to calling this function");
	zend_call_known_function(json_serialize_method, obj, ce, &retval, 0, NULL, NULL);
	/* An exception has occurred */
	if (Z_TYPE(retval) == IS_UNDEF) {
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			smart_str_appendl(buf, "null", 4);
		}
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
		return FAILURE;
	}

	if (Z_TYPE(retval) == IS_OBJECT && Z_OBJ(retval) == obj) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
		return_code = php_json_encode_array(buf, &retval, options, encoder);
	} else {
		/* All other types, encode as normal */
		return_code = php_json_encode_zval(buf, &retval, options, encoder);
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
	}

	zval_ptr_dtor(&retval);

	return return_code;
}

static zend_result php_json_encode_serializable_enum(smart_str *buf, zval *val, int options, php_json_encoder *encoder)
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	if (ce->enum_backing_type == IS_UNDEF) {
		encoder->error_code = PHP_JSON_ERROR_NON_BACKED_ENUM;
		smart_str_appendc(buf, '0');
		return FAILURE;
	}

	zval *value_zv = zend_enum_fetch_case_value(Z_OBJ_P(val));
	return php_json_encode_zval(buf, value_zv, options, encoder);
}

zend_result php_json_encode_zval(smart_str *buf, zval *val, int options, php_json_encoder *encoder) /* {{{ */
{
again:
	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			smart_str_appendl(buf, "null", 4);
			break;

		case IS_TRUE:
			smart_str_appendl(buf, "true", 4);
			break;
		case IS_FALSE:
			smart_str_appendl(buf, "false", 5);
			break;

		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			if (php_json_is_valid_double(Z_DVAL_P(val))) {
				php_json_encode_double(buf, Z_DVAL_P(val), options);
			} else {
				encoder->error_code = PHP_JSON_ERROR_INF_OR_NAN;
				smart_str_appendc(buf, '0');
			}
			break;

		case IS_STRING:
			return php_json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options, encoder);

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_json_serializable_ce)) {
				return php_json_encode_serializable_object(buf, Z_OBJ_P(val), options, encoder);
			}
			if (Z_OBJCE_P(val)->ce_flags & ZEND_ACC_ENUM) {
				return php_json_encode_serializable_enum(buf, val, options, encoder);
			}
			/* fallthrough -- Non-serializable object */
			ZEND_FALLTHROUGH;
		case IS_ARRAY: {
			/* Avoid modifications (and potential freeing) of the array through a reference when a
			 * jsonSerialize() method is invoked. */
			zval zv;
			zend_result res;
			ZVAL_COPY(&zv, val);
			res = php_json_encode_array(buf, &zv, options, encoder);
			zval_ptr_dtor_nogc(&zv);
			return res;
		}

		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;

		default:
			encoder->error_code = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
				smart_str_appendl(buf, "null", 4);
			}
			return FAILURE;
	}

	return SUCCESS;
}
/* }}} */
