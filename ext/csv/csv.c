/*
   +----------------------------------------------------------------------+
   | Copyright © 2020-2026, Gina Peter Banyard and Contributors.          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Gina Peter Banyard <girgias@php.net>                        |
   |          Damian Jóźwiak <damian.jozwiak.lodz@gmail.com>              |
   +----------------------------------------------------------------------+
*/
/* csv extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_streams.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h" /* For the default stream context */
#include "ext/standard/php_string.h" /* For php_str_to_str() */
#include "php_csv.h"
#include "csv_arginfo.h"

#include <stdbool.h>
#include "zend_smart_str.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

PHP_MINFO_FUNCTION(csv)
{
	php_info_print_table_start();
		php_info_print_table_row(2, "CSV support", "enabled");
	php_info_print_table_end();
}

#define EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(delimiter_arg_num, enclosure_arg_num, eol_arg_num) {	\
	if (eol_sequence) {																							\
		/* Make sure that there is at least one character in string */											\
		if (UNEXPECTED(ZSTR_LEN(eol_sequence) == 0)) {															\
			zend_argument_must_not_be_empty_error(eol_arg_num);													\
			RETURN_THROWS();																					\
		}																										\
		zend_string_addref(eol_sequence);                                                                       \
	} else {																									\
		eol_sequence = zend_string_init(ZEND_STRL("\r\n"), 0);													\
	}																											\
																												\
	if (delimiter) {																							\
		/* Make sure that there is at least one character in string */											\
		if (UNEXPECTED(ZSTR_LEN(delimiter) == 0)) {																\
			zend_argument_must_not_be_empty_error(delimiter_arg_num);											\
			zend_string_release(eol_sequence);																	\
			RETURN_THROWS();																					\
		}																										\
		if (UNEXPECTED(zend_string_equals(delimiter, eol_sequence))) {											\
			zend_argument_value_error(																			\
				eol_arg_num,																					\
				"must not be identical to argument #%"PRIu32" ($delimiter)",									\
				delimiter_arg_num);																				\
			zend_string_release(eol_sequence);																	\
			RETURN_THROWS();																					\
		}																										\
		zend_string_addref(delimiter);                                                                          \
	} else {																									\
		delimiter = ZSTR_CHAR(',');																				\
	}																											\
																												\
	if (enclosure) {																							\
		if (UNEXPECTED(ZSTR_LEN(enclosure) == 0)) {																\
			zend_argument_must_not_be_empty_error(enclosure_arg_num);											\
			zend_string_release(delimiter);																		\
			zend_string_release(eol_sequence);																	\
			RETURN_THROWS();																					\
		}																										\
		if (UNEXPECTED(zend_string_equals(enclosure, eol_sequence))) {											\
			zend_argument_value_error(																			\
				eol_arg_num,																					\
				"must not be identical to argument #%"PRIu32" ($enclosure)",									\
				enclosure_arg_num);																				\
			zend_string_release(eol_sequence);																	\
			zend_string_release(delimiter);																		\
			RETURN_THROWS();																					\
		}																										\
		zend_string_addref(enclosure);                                                                          \
	} else {																									\
		enclosure = ZSTR_CHAR('"');																				\
	}																											\
																												\
	/* Ensure delimiter and enclosure are different */															\
	if (UNEXPECTED(zend_string_equals(delimiter, enclosure))) {													\
		zend_argument_value_error(																				\
			enclosure_arg_num,																					\
			"must not be identical to argument #%"PRIu32" ($delimiter)",										\
			delimiter_arg_num);																					\
		zend_string_release(eol_sequence);																		\
		zend_string_release(delimiter);																			\
		zend_string_release(enclosure);																			\
		RETURN_THROWS();																						\
	}																											\
}

static bool zend_string_contains(const zend_string *haystack, const zend_string *needle) {
	return php_memnstr(ZSTR_VAL(haystack), ZSTR_VAL(needle), ZSTR_LEN(needle), ZSTR_VAL(haystack) + ZSTR_LEN(haystack));
}

static zend_always_inline bool buffer_starts_with_zend_string(const char *buffer, const char *end, const zend_string *needle) {
	size_t buffer_len = end-buffer;
	size_t needle_len = ZSTR_LEN(needle);
	/* Tokens are not empty; comparing the first byte inline avoids a memcmp() call per byte */
	return buffer_len >= needle_len
		&& buffer[0] == ZSTR_VAL(needle)[0]
		&& (needle_len == 1 || !memcmp(buffer + 1, ZSTR_VAL(needle) + 1, needle_len - 1));
}

/**
 * Whether a proper prefix of the enclosure sequence is also a suffix of it (e.g. "aa", "--").
 * With such an enclosure, the bytes before the end of an enclosed field are ambiguous: the field
 * "a" enclosed with "aa" is written as "aaaaa", where the enclosure sequence also occurs one byte
 * before the closing one. The closing enclosure is then the one that is followed by the delimiter,
 * the EOL sequence or the end of the input.
 */
static bool csv_enclosure_overlaps_itself(const zend_string *enclosure) {
	for (size_t k = 1; k < ZSTR_LEN(enclosure); k++) {
		if (!memcmp(ZSTR_VAL(enclosure), ZSTR_VAL(enclosure) + ZSTR_LEN(enclosure) - k, k)) {
			return true;
		}
	}
	return false;
}

static bool csv_is_at_end_of_field(
	const char *position,
	const char *end,
	const zend_string *delimiter,
	const zend_string *eol_sequence
) {
	return position == end
		|| buffer_starts_with_zend_string(position, end, delimiter)
		|| buffer_starts_with_zend_string(position, end, eol_sequence);
}

/* The tokens of a CSV dialect, with what the parser and the row boundary scanner derive from them */
typedef struct csv_dialect {
	const zend_string *delimiter;
	const zend_string *enclosure;
	const zend_string *eol_sequence;
	bool enclosure_overlaps_itself;
	char enclosure_first_byte;
	/* Outside of an escaped field, the bytes that may start a token or must be rejected (CR and
	 * LF); all other bytes are data and are skipped without matching any token */
	bool is_special_byte[256];
} csv_dialect;

static void csv_dialect_init(
	csv_dialect *dialect,
	const zend_string *delimiter,
	const zend_string *enclosure,
	const zend_string *eol_sequence
) {
	dialect->delimiter = delimiter;
	dialect->enclosure = enclosure;
	dialect->eol_sequence = eol_sequence;
	dialect->enclosure_overlaps_itself = csv_enclosure_overlaps_itself(enclosure);
	dialect->enclosure_first_byte = ZSTR_VAL(enclosure)[0];
	memset(dialect->is_special_byte, 0, sizeof(dialect->is_special_byte));
	dialect->is_special_byte[(unsigned char) ZSTR_VAL(delimiter)[0]] = true;
	dialect->is_special_byte[(unsigned char) ZSTR_VAL(enclosure)[0]] = true;
	dialect->is_special_byte[(unsigned char) ZSTR_VAL(eol_sequence)[0]] = true;
	dialect->is_special_byte['\r'] = true;
	dialect->is_special_byte['\n'] = true;
}

/* Append the field made of field_value followed by [start, end) to the row */
static zend_always_inline void csv_add_field(HashTable *row, smart_str *field_value, const char *start, const char *end) {
	zval tmp;
	if (field_value->s == NULL) {
		ZVAL_STR(&tmp, zend_string_init_fast(start, end - start));
	} else {
		smart_str_appendl(field_value, start, end - start);
		ZVAL_STR(&tmp, smart_str_extract(field_value));
	}
	zend_hash_next_index_insert_new(row, &tmp);
}

/**
 * Follows RFC4180 https://tools.ietf.org/html/rfc4180
 * The terminology can be slightly confusing here as what PHP considers the 'enclosure' is what is used
 * to escape a field in the RFC.
 *
 * This only formats ONE row of a CSV file.
 *
 * Returns a NULL pointer on error
 */
static zend_string* hashtable_to_rfc4180_string(
	HashTable *fields,
	const zend_string *delimiter,
	const zend_string *enclosure,
	const zend_string *eol_sequence
) {
	uint32_t nb_fields;
	uint32_t fields_iterated = 0;
	zval *tmp_field_zval;
	smart_str row = {0};

	nb_fields = zend_hash_num_elements(fields);
	ZEND_HASH_FOREACH_VAL(fields, tmp_field_zval) {
		bool escape_field = false;
		bool enclosure_within_filed = false;
		zend_string *tmp_field_str;
		zend_string *field_str = zval_try_get_tmp_string(tmp_field_zval, &tmp_field_str);
		if (UNEXPECTED(field_str == NULL)) {
			smart_str_free(&row);
			return NULL;
		}

		/*
		 * A field must be escaped (enclosed) if it contains the delimiter OR a Carriage Return (\r)
		 * OR a Line Feed (\n) OR the field escape sequence (i.e. enclosure parameter) OR the custom EOL sequence
		 */
		if (
			memchr(ZSTR_VAL(field_str), '\n', ZSTR_LEN(field_str))
			|| memchr(ZSTR_VAL(field_str), '\r', ZSTR_LEN(field_str))
			|| zend_string_contains(field_str, delimiter)
			|| zend_string_contains(field_str, eol_sequence)
		) {
			escape_field = true;
		}
		/* If the field escape sequence (i.e. enclosure parameter) is within the field it needs to be
		 * duplicated within the field. */
		if (zend_string_contains(field_str, enclosure)) {
			escape_field = true;
			enclosure_within_filed = true;
		}

		if (escape_field) {
			smart_str_append(&row, enclosure);

			if (enclosure_within_filed) {
				/* Create replace string (twice the field escape sequence) */
				smart_str escaped_enclosure = {0};
				smart_str_append(&escaped_enclosure, enclosure);
				smart_str_append(&escaped_enclosure, enclosure);
				smart_str_0(&escaped_enclosure);

				zend_string *replace = php_str_to_str(ZSTR_VAL(field_str), ZSTR_LEN(field_str), ZSTR_VAL(enclosure),
						ZSTR_LEN(enclosure), ZSTR_VAL(escaped_enclosure.s), ZSTR_LEN(escaped_enclosure.s));

				smart_str_append(&row, replace);

				smart_str_free(&escaped_enclosure);
				zend_string_release(replace);
			} else {
				smart_str_append(&row, field_str);
			}

			smart_str_append(&row, enclosure);
		} else {
			smart_str_append(&row, field_str);
		}

		/* Only add the delimiter in between fields on the same row. */
		if (++fields_iterated != nb_fields) {
			smart_str_append(&row, delimiter);
		}

		/* Clear temporary variable */
		zend_tmp_string_release(tmp_field_str);
	} ZEND_HASH_FOREACH_END();
	/* Add the EOL sequence to indicate the end of the row. */
	smart_str_append(&row, eol_sequence);

	return smart_str_extract(&row);
}

static zend_object_iterator* csv_init_iterator_foreach(zval *iterator_zval, zval **first_iteration_value) {
	ZEND_ASSERT(Z_TYPE_P(iterator_zval) == IS_OBJECT);
	ZEND_ASSERT(first_iteration_value != NULL);

	zend_class_entry *ce = Z_OBJCE_P(iterator_zval);
	ZEND_ASSERT(ce->iterator_funcs_ptr != NULL);
	ZEND_ASSERT(ce->get_iterator != NULL);

	zend_object_iterator *it = ce->get_iterator(ce, iterator_zval, 0);
	if (UNEXPECTED(EG(exception))) {
		if (UNEXPECTED(it)) {
			zend_iterator_dtor(it);
		}
		return NULL;
	}
	ZEND_ASSERT(it != NULL);

	/* Rewind iterator */
	it->index = 0;
	if (it->funcs->rewind) {
		it->funcs->rewind(it);
		if (UNEXPECTED(EG(exception))) {
			zend_iterator_dtor(it);
			return NULL;
		}
	}

	if (it->funcs->valid(it) != SUCCESS || UNEXPECTED(EG(exception))) {
		zend_iterator_dtor(it);
		return NULL;
	}

	*first_iteration_value = it->funcs->get_current_data(it);
	if (*first_iteration_value == NULL || UNEXPECTED(EG(exception))) {
		zend_iterator_dtor(it);
		return NULL;
	}

	return it;
}

static zval* csv_advance_iterator_foreach(zend_object_iterator *it) {
	ZEND_ASSERT(it != NULL);

	/* Move iterator forward */
	it->index++;
	it->funcs->move_forward(it);
	if (UNEXPECTED(EG(exception))) {
		zend_iterator_dtor(it);
		return NULL;
	}

	if (it->funcs->valid(it) != SUCCESS || UNEXPECTED(EG(exception))) {
		zend_iterator_dtor(it);
		return NULL;
	}

	zval *next_value = it->funcs->get_current_data(it);
	if (next_value == NULL || UNEXPECTED(EG(exception))) {
		zend_iterator_dtor(it);
		return NULL;
	}

	return next_value;
}

#define CSV_ITERABLE_FOREACH_VAL(____iterable_zval, __value, __iterator_error_label) \
	do { \
		bool __is_array = true; \
		bool __is_foreach_loop_valid = true; \
		zval *__loop_value = NULL; \
		/* Needed for iterators */ \
		zend_object_iterator *__zend_iterator = NULL; \
		/* Copied from _ZEND_HASH_FOREACH_VAL */ \
		const HashTable *__ht = NULL; \
		uint32_t _count = 0; \
		size_t _size = 0; \
		if (Z_TYPE_P(____iterable_zval) == IS_ARRAY) { \
			__ht = Z_ARRVAL_P(____iterable_zval); \
			_count = __ht->nNumUsed; \
			_size = ZEND_HASH_ELEMENT_SIZE(__ht); \
			__loop_value = __ht->arPacked; \
			__is_foreach_loop_valid = _count > 0; \
		} else { \
			__is_array = false; \
			__zend_iterator = csv_init_iterator_foreach(____iterable_zval, &__loop_value); \
			if (UNEXPECTED(__zend_iterator == NULL)) { \
				goto __iterator_error_label; \
			} \
		} \
		while (__is_foreach_loop_valid) { \
			if (__is_array) { \
				/* Skip holes (e.g. after unset()) while advancing, otherwise the loop never progresses */ \
				while (__is_foreach_loop_valid && UNEXPECTED(Z_TYPE_P(__loop_value) == IS_UNDEF)) { \
					_count--; \
					__loop_value = ZEND_HASH_NEXT_ELEMENT(__loop_value, _size); \
					__is_foreach_loop_valid = _count > 0; \
				} \
				if (!__is_foreach_loop_valid) { \
					break; \
				} \
			} \
			__value = __loop_value;

#define CSV_ITERABLE_FOREACH_END() \
			do { \
				if (__is_array) { \
					_count--; \
					__loop_value = ZEND_HASH_NEXT_ELEMENT(__loop_value, _size); \
					__is_foreach_loop_valid = _count > 0; \
				} else { \
					__loop_value = csv_advance_iterator_foreach(__zend_iterator); \
					if (__loop_value == NULL) { \
						__is_foreach_loop_valid = false; \
					} \
				} \
			} while (false); \
		} /* end while (__is_foreach_loop_valid) */ \
		if (!__is_array && __is_foreach_loop_valid) { \
			/* The loop was aborted with break: the iterator has not been destroyed yet */ \
			zend_iterator_dtor(__zend_iterator); \
		} \
	} while (false)

/* Returns a NULL pointer on error */
static HashTable* rfc4180_string_to_hashtable(
	const char **buffer,
	const char *end_buffer,
	const csv_dialect *dialect
) {
	HashTable *return_value = zend_new_array(8);
	const zend_string *delimiter = dialect->delimiter;
	const zend_string *enclosure = dialect->enclosure;
	const zend_string *eol_sequence = dialect->eol_sequence;

	bool in_escaped_field = false;
	bool at_start_of_field = true;
	/* A closing enclosure must be followed by a delimiter, an EOL sequence or the end of input */
	bool after_closing_enclosure = false;

	/* Dereference buffer */
	const char *row = *buffer;
	/* row_to_array('') passes an empty buffer, which is a single empty field */
	ZEND_ASSERT(row <= end_buffer);

	/* The value of the current field is field_value followed by the bytes [segment_start,
	 * segment_end) of the buffer. field_value is only used once a doubled enclosure sequence
	 * splits an escaped field into several segments; otherwise the field is created from the
	 * buffer directly. */
	smart_str field_value = {0};
	const char *segment_start = row;
	const char *segment_end = row;

	/* Main loop to "tokenize" the row */
	while (row < end_buffer) {
		if (in_escaped_field) {
			/* Only an enclosure sequence ends or interrupts an escaped field */
			const char *next = memchr(row, dialect->enclosure_first_byte, end_buffer - row);
			if (next == NULL) {
				row = end_buffer;
				break;
			}
			row = next;
			if (!buffer_starts_with_zend_string(row, end_buffer, enclosure)) {
				row++;
				continue;
			}
			const char *enclosure_start = row;
			row += ZSTR_LEN(enclosure);

			/* A doubled enclosure sequence stays in the field once. Consume the whole sequence
			 * at once, otherwise a multibyte enclosure would be re-matched against its own tail
			 * bytes. */
			if (buffer_starts_with_zend_string(row, end_buffer, enclosure)) {
				smart_str_appendl(&field_value, segment_start, row - segment_start);
				row += ZSTR_LEN(enclosure);
				segment_start = row;
				continue;
			}

			/* See csv_enclosure_overlaps_itself(): the enclosure sequence starting one byte later
			 * may be the closing one, so this byte is data. */
			if (dialect->enclosure_overlaps_itself
				&& !csv_is_at_end_of_field(row, end_buffer, delimiter, eol_sequence)) {
				row = enclosure_start + 1;
				continue;
			}

			in_escaped_field = false;
			after_closing_enclosure = true;
			segment_end = enclosure_start;
			continue;
		}

		/* Bytes that cannot start a token are data */
		if (!dialect->is_special_byte[(unsigned char) *row]) {
			if (UNEXPECTED(after_closing_enclosure)) {
				zend_value_error("Closing enclosure sequence must be followed by the delimiter or the EOL sequence");
				goto error;
			}
			do {
				row++;
			} while (row < end_buffer && !dialect->is_special_byte[(unsigned char) *row]);
			at_start_of_field = false;
			segment_end = row;
			continue;
		}

		/* Check for field escape sequence (i.e. enclosure) */
		if (buffer_starts_with_zend_string(row, end_buffer, enclosure)) {
			if (UNEXPECTED(after_closing_enclosure)) {
				zend_value_error("Closing enclosure sequence must be followed by the delimiter or the EOL sequence");
				goto error;
			}
			if (UNEXPECTED(!at_start_of_field)) {
				zend_value_error("Enclosure sequence is used in a non escaped field");
				goto error;
			}
			row += ZSTR_LEN(enclosure);
			in_escaped_field = true;
			at_start_of_field = false;
			segment_start = row;
			continue;
		}

		/* Check for delimiter */
		if (buffer_starts_with_zend_string(row, end_buffer, delimiter)) {
			csv_add_field(return_value, &field_value, segment_start, segment_end);
			row += ZSTR_LEN(delimiter);
			at_start_of_field = true;
			after_closing_enclosure = false;
			segment_start = row;
			segment_end = row;
			continue;
		}

		/* Check for End Of Line sequence */
		if (buffer_starts_with_zend_string(row, end_buffer, eol_sequence)) {
			row += ZSTR_LEN(eol_sequence);
			goto eol;
		}

		/* A byte that may start a token but does not */
		if (UNEXPECTED(after_closing_enclosure)) {
			zend_value_error("Closing enclosure sequence must be followed by the delimiter or the EOL sequence");
			goto error;
		}
		/* RFC 4180 only allows CR and LF inside enclosed fields; outside of them they are
		 * either part of the EOL sequence or a sign that the input uses a different one */
		if (UNEXPECTED(*row == '\r' || *row == '\n')) {
			zend_value_error("A non escaped field must not contain CR or LF characters that are not part of the EOL sequence");
			goto error;
		}
		at_start_of_field = false;
		row++;
		segment_end = row;
	}

	if (UNEXPECTED(in_escaped_field)) {
		zend_value_error("Enclosure sequence is not closed");
		goto error;
	}

	eol:;
	csv_add_field(return_value, &field_value, segment_start, segment_end);

	/* Update outer buffer position */
	*buffer = row;

	return return_value;

error:
	smart_str_free(&field_value);
	zend_array_destroy(return_value);
	return NULL;
}

/* Returns a NULL pointer on error */
static HashTable* rfc4180_buffer_to_hashtable_collection(
	const zend_string *buffer,
	const zend_string *delimiter,
	const zend_string *enclosure,
	const zend_string *eol_sequence,
	bool is_lax
) {
	HashTable *return_value = zend_new_array(8);
	const char *start_position = ZSTR_VAL(buffer);
	const char *current_position = ZSTR_VAL(buffer);
	const char *end_buffer = ZSTR_VAL(buffer) + ZSTR_LEN(buffer);
	size_t length = ZSTR_LEN(buffer);
	size_t buffer_row_nb = 1;
	uint32_t nb_fields = 0;
	csv_dialect dialect;
	csv_dialect_init(&dialect, delimiter, enclosure, eol_sequence);

	while (current_position - start_position < (ptrdiff_t) length) {
		uint32_t new_nb_fields = 0;
		HashTable *row = rfc4180_string_to_hashtable(&current_position, end_buffer, &dialect);

		/* Issue with parsing row */
		if (row == NULL) {
			zend_array_destroy(return_value);
			return NULL;
		}

		/* used to check if the number of fields is equal in each iteration */
		new_nb_fields = zend_hash_num_elements(row);
		/* buffer_row_nb == 1 means we are at the first iteration so don't check */
		if (new_nb_fields != nb_fields && buffer_row_nb != 1 && !is_lax) {
			zend_value_error("Buffer row %zu contains %"PRIu32" fields compared to %"PRIu32" fields on previous rows",
				buffer_row_nb, new_nb_fields, nb_fields);
			zend_array_destroy(row);
			zend_array_destroy(return_value);
			return NULL;
		}
		nb_fields = new_nb_fields;
		buffer_row_nb++;

		zval tmp;
		ZVAL_ARR(&tmp, row);
		zend_hash_next_index_insert(return_value, &tmp);
	}

	return return_value;
}

PHP_FUNCTION(Csv_array_to_row)
{
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;
	HashTable *fields;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h|SSS", &fields, &delimiter, &enclosure, &eol_sequence) == FAILURE) {
		RETURN_THROWS();
	}

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(2, 3, 4);

	zend_string *result = hashtable_to_rfc4180_string(fields, delimiter, enclosure, eol_sequence);
	zend_string_release(eol_sequence);
	zend_string_release(delimiter);
	zend_string_release(enclosure);

	if (UNEXPECTED(result == NULL)) {
		RETURN_THROWS();
	}

	RETURN_STR(result);
}

PHP_FUNCTION(Csv_collection_to_buffer)
{
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;
	zval *collection;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_ITERABLE(collection)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(delimiter)
		Z_PARAM_STR(enclosure)
		Z_PARAM_STR(eol_sequence)
	ZEND_PARSE_PARAMETERS_END();

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(2, 3, 4);

	zval *fields;
	size_t collection_index = 0;
	uint32_t nb_fields = 0;
	smart_str buffer = {0};
	bool has_errors = false;
	CSV_ITERABLE_FOREACH_VAL(collection, fields, end) {
		/* Check that fields is an array */
		ZVAL_DEREF(fields);
		if (UNEXPECTED(Z_TYPE_P(fields) != IS_ARRAY)) {
			zend_type_error("Element %zu of the collection must be an array", collection_index);
			has_errors = true;
			break;
		}

		/* used to check if the number of fields is equal in each iteration */
		uint32_t new_nb_fields = zend_hash_num_elements(Z_ARRVAL_P(fields));
		/* collection_index == 0 means we are at the first iteration so don't check */
		if (nb_fields != new_nb_fields && collection_index != 0) {
			zend_value_error("Element %zu of the collection contains %"PRIu32
			" fields compared to %"PRIu32" fields on previous rows",
				collection_index, new_nb_fields, nb_fields);
			has_errors = true;
			break;
		}
		nb_fields = new_nb_fields;
		collection_index++;

		/* Hold a reference to the row while formatting it: converting a field to string can run
		 * userland code (__toString()) that resumes a generator or overwrites the element of the
		 * collection, which would otherwise free the row while it is being iterated. */
		zval row;
		ZVAL_COPY(&row, fields);
		zend_string *result = hashtable_to_rfc4180_string(Z_ARRVAL(row), delimiter, enclosure, eol_sequence);
		zval_ptr_dtor(&row);
		if (UNEXPECTED(result == NULL)) {
			has_errors = true;
			break;
		}
		smart_str_append(&buffer, result);
		zend_string_release(result);
	} CSV_ITERABLE_FOREACH_END();

	end:
	/* Release strings */
	zend_string_release(eol_sequence);
	zend_string_release(delimiter);
	zend_string_release(enclosure);
	/* If an Error/Exception has been thrown */
	if (has_errors || UNEXPECTED(EG(exception))) {
		smart_str_free(&buffer);
		RETURN_THROWS();
	}

	RETURN_STR(smart_str_extract(&buffer));
}

/* Open a stream the way SplFileObject does: through the default stream context (so that
 * stream_context_set_default() applies), with the reason of a failure (e.g. "No such file or
 * directory") reported by the wrapper turned into the message of the thrown Error.
 *
 * The stream stays registered as a resource, so the resource list closes it at shutdown if the
 * owner never got to, but userland must not close it behind the owner's back (it can reach it
 * through get_resources()): PHP_STREAM_FLAG_NO_FCLOSE makes fclose() refuse to do that. */
static php_stream *php_csv_stream_open(const zend_string *file, const char *mode, const char *purpose)
{
	zend_error_handling error_handling;
	zend_replace_error_handling(EH_THROW, zend_ce_error, &error_handling);
	php_stream *stream = php_stream_open_wrapper_ex(ZSTR_VAL(file), mode, REPORT_ERRORS, NULL,
		php_stream_context_from_zval(NULL, 0));
	zend_restore_error_handling(&error_handling);

	if (UNEXPECTED(stream == NULL)) {
		if (!EG(exception)) {
			zend_throw_error(NULL, "Failed to open \"%s\" for %s", ZSTR_VAL(file), purpose);
		}
		return NULL;
	}
	if (UNEXPECTED(EG(exception))) {
		/* E.g. a userspace wrapper opened the stream but raised a warning while doing so */
		php_stream_close(stream);
		return NULL;
	}

	stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;
	return stream;
}

PHP_FUNCTION(Csv_collection_to_file)
{
	zend_string *file = NULL;
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;
	zval *collection;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_PATH_STR(file)
		Z_PARAM_ITERABLE(collection)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(delimiter)
		Z_PARAM_STR(enclosure)
		Z_PARAM_STR(eol_sequence)
	ZEND_PARSE_PARAMETERS_END();

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(3, 4, 5);

	php_stream *stream = php_csv_stream_open(file, "wb", "writing");
	if (UNEXPECTED(stream == NULL)) {
		zend_string_release(eol_sequence);
		zend_string_release(delimiter);
		zend_string_release(enclosure);
		RETURN_THROWS();
	}

	zval *fields;
	size_t collection_index = 0;
	uint32_t nb_fields = 0;
	bool has_errors = false;
	CSV_ITERABLE_FOREACH_VAL(collection, fields, end) {
		/* Check that fields is an array */
		ZVAL_DEREF(fields);
		if (UNEXPECTED(Z_TYPE_P(fields) != IS_ARRAY)) {
			zend_type_error("Element %zu of the collection must be an array", collection_index);
			has_errors = true;
			break;
		}

		/* used to check if the number of fields is equal in each iteration */
		uint32_t new_nb_fields = zend_hash_num_elements(Z_ARRVAL_P(fields));
		/* collection_index == 0 means we are at the first iteration so don't check */
		if (nb_fields != new_nb_fields && collection_index != 0) {
			zend_value_error("Element %zu of the collection contains %"PRIu32
			" fields compared to %"PRIu32" fields on previous rows",
				collection_index, new_nb_fields, nb_fields);
			has_errors = true;
			break;
		}
		nb_fields = new_nb_fields;
		collection_index++;

		/* Hold a reference to the row while formatting it: converting a field to string can run
		 * userland code (__toString()) that resumes a generator or overwrites the element of the
		 * collection, which would otherwise free the row while it is being iterated. */
		zval row;
		ZVAL_COPY(&row, fields);
		zend_string *result = hashtable_to_rfc4180_string(Z_ARRVAL(row), delimiter, enclosure, eol_sequence);
		zval_ptr_dtor(&row);
		if (UNEXPECTED(result == NULL)) {
			has_errors = true;
			break;
		}

		ssize_t bytes_written = php_stream_write(stream, ZSTR_VAL(result), ZSTR_LEN(result));
		bool is_short_write = bytes_written < 0 || (size_t) bytes_written != ZSTR_LEN(result);
		zend_string_release(result);
		if (UNEXPECTED(is_short_write)) {
			zend_throw_error(NULL, "Failed to write element %zu of the collection to \"%s\"",
				collection_index - 1, ZSTR_VAL(file));
			has_errors = true;
			break;
		}
	} CSV_ITERABLE_FOREACH_END();

	end:;
	/* Flush explicitly: php_stream_free() flushes too but discards the result, so e.g. a
	 * userspace wrapper returning false from stream_flush() would go unnoticed. Closing
	 * can also fail on its own (e.g. a compressed stream finishing its deflate buffer);
	 * either failure means the file is incomplete even though every write succeeded.
	 * Known limitation, shared with userland fclose(): a failure signalled by a write
	 * filter only during its final ($closing) flush is not observable through the
	 * streams API and is reported as success. */
	int flush_status = has_errors || EG(exception) ? 0 : php_stream_flush(stream);
	int close_status = php_stream_close(stream);
	/* Release strings */
	zend_string_release(eol_sequence);
	zend_string_release(delimiter);
	zend_string_release(enclosure);
	/* If an Error/Exception has been thrown */
	if (has_errors || UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}
	if (UNEXPECTED(flush_status != 0 || close_status != 0)) {
		zend_throw_error(NULL, "Failed to finish writing to \"%s\"", ZSTR_VAL(file));
		RETURN_THROWS();
	}
}

PHP_FUNCTION(Csv_row_to_array)
{
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;
	zend_string *row;
	HashTable *fields = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|SSS", &row, &delimiter, &enclosure, &eol_sequence) == FAILURE) {
		RETURN_THROWS();
	}

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(2, 3, 4);

	const char *row_c = ZSTR_VAL(row);
	const char *end_row = ZSTR_VAL(row) + ZSTR_LEN(row);
	csv_dialect dialect;
	csv_dialect_init(&dialect, delimiter, enclosure, eol_sequence);
	fields = rfc4180_string_to_hashtable(&row_c, end_row, &dialect);
	zend_string_release(eol_sequence);
	zend_string_release(delimiter);
	zend_string_release(enclosure);

	if (UNEXPECTED(fields == NULL)) {
		RETURN_THROWS();
	}

	/* The row may end with the EOL sequence, but must not be followed by another one */
	if (UNEXPECTED(row_c != end_row)) {
		zend_array_destroy(fields);
		zend_argument_value_error(1, "must contain a single row");
		RETURN_THROWS();
	}

	RETURN_ARR(fields);
}

static void buffer_to_collection_generic(INTERNAL_FUNCTION_PARAMETERS, bool is_lax)
{
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;
	zend_string *buffer;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|SSS", &buffer, &delimiter, &enclosure, &eol_sequence) == FAILURE) {
		RETURN_THROWS();
	}

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(2, 3, 4);

	HashTable *collection = rfc4180_buffer_to_hashtable_collection(buffer, delimiter, enclosure, eol_sequence, is_lax);

	/* Release strings */
	zend_string_release(eol_sequence);
	zend_string_release(delimiter);
	zend_string_release(enclosure);
	/* If an Error has been thrown */
	if (UNEXPECTED(collection == NULL)) {
		RETURN_THROWS();
	}
	RETURN_ARR(collection);
}

PHP_FUNCTION(Csv_buffer_to_collection)
{
	buffer_to_collection_generic(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
PHP_FUNCTION(Csv_buffer_to_collection_lax)
{
	buffer_to_collection_generic(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static zend_class_entry *php_csv_lazy_collection_ce = NULL;
static zend_object_handlers php_csv_lazy_collection_object_handlers;

/* How much data is read from the stream at once while looking for the end of a row */
#define PHP_CSV_STREAM_CHUNK_SIZE 8192
/* Once this many consumed bytes accumulate at the front of the stream buffer it is compacted,
 * which is what keeps memory usage bounded for createFromFile() */
#define PHP_CSV_STREAM_BUFFER_COMPACT_THRESHOLD (64 * 1024)

typedef struct php_csv_lazy_collection_object {
	/* Buffer mode (createFromBuffer): the whole CSV document; NULL in stream mode */
	zend_string *buffer;
	/* Stream mode (createFromFile): the stream and a sliding window of not yet parsed data. The
	 * stream is closed (and set to NULL) when the object is destroyed. As a stream can only be
	 * read from one place, at most one iterator may be active at a time. */
	bool is_stream_mode;
	bool stream_has_active_iterator;
	bool stream_iteration_started;
	php_stream *stream;
	smart_str stream_buffer;
	size_t stream_buffer_position;
	/* Where csv_find_end_of_row() resumes scanning the row starting at stream_buffer_position
	 * after more data has been read, so that a long row is not rescanned from its start */
	size_t stream_scan_position;
	bool stream_scan_in_escaped_field;
	/* Common */
	zend_string *delimiter;
	zend_string *enclosure;
	zend_string *eol_sequence;
	csv_dialect dialect;
	zend_object std;
} php_csv_lazy_collection_object;

static php_csv_lazy_collection_object *php_csv_lazy_collection_object_from(zend_object *object) {
	return (php_csv_lazy_collection_object *)(((char*) object) - offsetof(php_csv_lazy_collection_object, std));
}

static php_csv_lazy_collection_object *php_csv_lazy_collection_object_fetch(zval *obj) {
	return php_csv_lazy_collection_object_from(Z_OBJ_P(obj));
}

static zend_object *php_csv_lazy_collection_object_new(zend_class_entry *ce)
{
	php_csv_lazy_collection_object *lazy_collection = zend_object_alloc(sizeof(php_csv_lazy_collection_object), ce);
	zend_object_std_init(&lazy_collection->std, ce);
	object_properties_init(&lazy_collection->std, ce);

	return &lazy_collection->std;
}

/* The stream is closed here rather than in free_obj: destructors run while the executor is still
 * fully functional, so closing a userspace-wrapper or filtered stream can safely call back into
 * userland (stream_close(), onClose()). If destructors are skipped (e.g. after a fatal error), the
 * stream is left to the resource list, which closes every stream that is still open. */
static void php_csv_lazy_collection_object_dtor(zend_object *std)
{
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_from(std);

	zend_objects_destroy_object(std);

	if (lazy_collection->stream != NULL) {
		php_stream *stream = lazy_collection->stream;
		lazy_collection->stream = NULL;
		php_stream_close(stream);
	}
}

static void php_csv_lazy_collection_object_free(zend_object *std)
{
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_from(std);

	/* PHP Will create an object even if php_csv_lazy_collection_get_constructor() throws,
	 * So we cannot assume the buffers and various CSV settings are set */
	if (lazy_collection->buffer != NULL) {
		zend_string_release(lazy_collection->buffer);
		lazy_collection->buffer = NULL;
	}

	/* A stream that is still set was not closed by the destructor; it belongs to the resource
	 * list then (see php_csv_lazy_collection_object_dtor()) and may already have been freed. */
	lazy_collection->stream = NULL;
	smart_str_free(&lazy_collection->stream_buffer);

	if (lazy_collection->delimiter != NULL) {
		zend_string_release(lazy_collection->delimiter);
		lazy_collection->delimiter = NULL;
	}

	if (lazy_collection->enclosure != NULL) {
		zend_string_release(lazy_collection->enclosure);
		lazy_collection->enclosure = NULL;
	}

	if (lazy_collection->eol_sequence != NULL) {
		zend_string_release(lazy_collection->eol_sequence);
		lazy_collection->eol_sequence = NULL;
	}

	zend_object_std_dtor(&lazy_collection->std);
}

static zend_function *php_csv_lazy_collection_get_constructor(zend_object *obj) {
	zend_throw_error(NULL, "Cannot instantiate Csv\\LazyLaxCollection class");
	return NULL;
}

/**
 * Find the end of the first complete row in [row_start, end), honouring the enclosure rules of
 * rfc4180_string_to_hashtable(): an EOL sequence inside an escaped (enclosed) field does not
 * terminate the row, and a doubled enclosure sequence stays inside the field.
 *
 * Returns a pointer one past the row's EOL sequence, or NULL when no complete row is available
 * yet (i.e. more data must be read from the stream).
 *
 * Scanning starts at *resume_position in the *resume_in_escaped_field state (the start of the row
 * and false for a new row), and the furthest position up to which every decision was final is
 * stored back with the state at that point, so that after reading more data the caller resumes
 * there and a row is scanned in linear time however many reads it takes. Skipping bytes that
 * cannot start a token is always final; a decision on a token depends on at most stable_len bytes,
 * so it is final when that many bytes are available. Decisions closer to the end (e.g. a multibyte
 * EOL split by a read boundary) are made again once more data is available.
 *
 * The scanner does not reproduce the parser's errors on malformed input; on such input it may
 * over-approximate the row length, and the parser then reports the error.
 */
static const char *csv_find_end_of_row(
	const char **resume_position,
	bool *resume_in_escaped_field,
	const char *end,
	const csv_dialect *dialect
) {
	const zend_string *delimiter = dialect->delimiter;
	const zend_string *enclosure = dialect->enclosure;
	const zend_string *eol_sequence = dialect->eol_sequence;
	/* Deciding whether an enclosure closes the field reads it, a possible doubled one, and for a
	 * self-overlapping enclosure the delimiter or EOL sequence following it */
	const size_t stable_len = ZSTR_LEN(enclosure)
		+ MAX(ZSTR_LEN(enclosure), MAX(ZSTR_LEN(delimiter), ZSTR_LEN(eol_sequence)));
	const char *position = *resume_position;
	bool in_escaped_field = *resume_in_escaped_field;
	bool decisions_are_final = true;

	while (position < end) {
		/* Skip the bytes that cannot start a token, in the same way as the parser */
		if (in_escaped_field) {
			const char *next = memchr(position, dialect->enclosure_first_byte, end - position);
			position = next != NULL ? next : end;
		} else {
			while (position < end && !dialect->is_special_byte[(unsigned char) *position]) {
				position++;
			}
		}

		if (decisions_are_final) {
			*resume_position = position;
			*resume_in_escaped_field = in_escaped_field;
			decisions_are_final = (size_t) (end - position) >= stable_len;
		}
		if (position == end) {
			break;
		}

		if (buffer_starts_with_zend_string(position, end, enclosure)) {
			position += ZSTR_LEN(enclosure);
			if (in_escaped_field) {
				if (buffer_starts_with_zend_string(position, end, enclosure)) {
					/* Doubled enclosure: still inside the field */
					position += ZSTR_LEN(enclosure);
				} else if (dialect->enclosure_overlaps_itself
					&& !csv_is_at_end_of_field(position, end, delimiter, eol_sequence)) {
					/* Same rule as the parser: the first byte is data */
					position -= ZSTR_LEN(enclosure) - 1;
				} else {
					in_escaped_field = false;
				}
			} else {
				in_escaped_field = true;
			}
			continue;
		}

		if (!in_escaped_field) {
			/* Tokens must be matched in the same order as the parser does: a delimiter is
			 * consumed as a whole so that an EOL sequence occurring inside it (e.g.
			 * delimiter "||" with EOL "|") does not split the row. */
			if (buffer_starts_with_zend_string(position, end, delimiter)) {
				position += ZSTR_LEN(delimiter);
				continue;
			}
			if (buffer_starts_with_zend_string(position, end, eol_sequence)) {
				return position + ZSTR_LEN(eol_sequence);
			}
		}

		position++;
	}

	return NULL;
}

/* Parse one row from the stream buffer window [start, end) into current_row and advance the
 * consumed position. */
static void php_csv_lazy_collection_parse_buffered_row(
	php_csv_lazy_collection_object *lazy_collection,
	zval *current_row,
	const char *start,
	const char *end
) {
	const char *position = start;
	HashTable *row_ht = rfc4180_string_to_hashtable(&position, end, &lazy_collection->dialect);
	if (UNEXPECTED(row_ht == NULL)) {
		/* An Error has been thrown; leaving current_row undef ends the iteration */
		return;
	}

	lazy_collection->stream_buffer_position += (size_t) (position - start);
	ZVAL_ARR(current_row, row_ht);

	/* Compact the buffer so memory stays proportional to the longest row, not the file */
	if (lazy_collection->stream_buffer_position >= PHP_CSV_STREAM_BUFFER_COMPACT_THRESHOLD) {
		zend_string *s = lazy_collection->stream_buffer.s;
		size_t remaining = ZSTR_LEN(s) - lazy_collection->stream_buffer_position;
		memmove(ZSTR_VAL(s), ZSTR_VAL(s) + lazy_collection->stream_buffer_position, remaining);
		ZSTR_LEN(s) = remaining;
		lazy_collection->stream_buffer_position = 0;
	}

	/* The next row is scanned from its start */
	lazy_collection->stream_scan_position = lazy_collection->stream_buffer_position;
	lazy_collection->stream_scan_in_escaped_field = false;
}

/* How many bytes of lookahead past a candidate row end the scanner needs before the row
 * boundary can be trusted. Two situations make a boundary decision depend on bytes that may
 * not have been read yet:
 *  - a token occurring inside a longer token (as its prefix, like EOL "\n" in enclosure
 *    "\nX", or in its interior, like EOL "\n" in delimiter "x\ny"): the shorter token could
 *    be matched where the longer one was cut by the read boundary. Every decision the
 *    scanner makes at a position q consumes a token ending at or before the row end, and
 *    the longer token it could have missed extends at most (len(longer) - offset -
 *    len(shorter)) bytes past it, so that many bytes of lookahead disambiguate all of them;
 *  - a closing enclosure right before the EOL: its "doubled enclosure?" check looks
 *    ZSTR_LEN(enclosure) bytes past the enclosure, which can reach past the EOL.
 * For the common single-byte dialects this returns 0, so a complete row buffered from a
 * blocking stream (e.g. a FIFO) is delivered without waiting for further data. */
static size_t csv_scanner_lookahead_needed(
	const zend_string *delimiter,
	const zend_string *enclosure,
	const zend_string *eol_sequence
) {
	const zend_string *tokens[3] = {delimiter, enclosure, eol_sequence};
	size_t needed = 0;

	for (int longer = 0; longer < 3; longer++) {
		for (int shorter = 0; shorter < 3; shorter++) {
			size_t longer_len = ZSTR_LEN(tokens[longer]);
			size_t shorter_len = ZSTR_LEN(tokens[shorter]);
			if (shorter_len > longer_len) {
				continue;
			}
			for (size_t offset = 0; offset + shorter_len <= longer_len; offset++) {
				if (longer == shorter && offset == 0) {
					continue;
				}
				if (memcmp(ZSTR_VAL(tokens[longer]) + offset, ZSTR_VAL(tokens[shorter]), shorter_len) == 0) {
					needed = MAX(needed, longer_len - offset - shorter_len);
				}
			}
		}
	}

	if (ZSTR_LEN(enclosure) > ZSTR_LEN(eol_sequence)) {
		needed = MAX(needed, ZSTR_LEN(enclosure) - ZSTR_LEN(eol_sequence));
	}

	return needed;
}

static void php_csv_lazy_collection_stream_next(php_csv_lazy_collection_object *lazy_collection, zval *current_row)
{
	smart_str *stream_buffer = &lazy_collection->stream_buffer;
	const size_t lookahead_needed = csv_scanner_lookahead_needed(
		lazy_collection->delimiter, lazy_collection->enclosure, lazy_collection->eol_sequence);

	for (;;) {
		const char *start = NULL;
		const char *end = NULL;
		if (stream_buffer->s != NULL) {
			start = ZSTR_VAL(stream_buffer->s) + lazy_collection->stream_buffer_position;
			end = ZSTR_VAL(stream_buffer->s) + ZSTR_LEN(stream_buffer->s);
		}
		bool has_buffered_data = start != NULL && start < end;

		const char *end_of_row = NULL;
		if (has_buffered_data) {
			const char *resume_position = ZSTR_VAL(stream_buffer->s) + lazy_collection->stream_scan_position;
			end_of_row = csv_find_end_of_row(&resume_position, &lazy_collection->stream_scan_in_escaped_field,
				end, &lazy_collection->dialect);
			lazy_collection->stream_scan_position = (size_t) (resume_position - ZSTR_VAL(stream_buffer->s));
		}
		bool at_eof = php_stream_eof(lazy_collection->stream);

		/* Only trust a row boundary when the dialect-specific lookahead is available past
		 * it, or when no further bytes can arrive; re-reading and re-scanning resolves
		 * boundary ambiguities (see csv_scanner_lookahead_needed()). */
		if (end_of_row != NULL && (at_eof || (size_t) (end - end_of_row) >= lookahead_needed)) {
			php_csv_lazy_collection_parse_buffered_row(lazy_collection, current_row, start, end_of_row);
			return;
		}

		if (at_eof) {
			if (!has_buffered_data) {
				/* End of iteration; current_row stays undef */
				return;
			}
			/* Final row without a terminating EOL sequence: hand the whole remainder to the
			 * parser, mirroring the end-of-buffer behaviour of createFromBuffer(). */
			php_csv_lazy_collection_parse_buffered_row(lazy_collection, current_row, start, end);
			return;
		}

		/* Read through the stream's read buffer the way php_stream_get_line() does:
		 * php_stream_fill_read_buffer() issues a single read against the underlying
		 * stream, so a partial read (e.g. from a FIFO whose writer keeps it open) is
		 * processed as soon as it arrives. php_stream_read() must not be used here, as
		 * it keeps reading plain-file streams until the full requested size is filled.
		 * A stream with read filters attached can still block across multiple underlying
		 * reads inside the fill; that matches what php_stream_get_line() based readers
		 * (e.g. fgetcsv()) do on such streams. */
		php_stream *stream = lazy_collection->stream;
		if (stream->writepos <= stream->readpos
			&& UNEXPECTED(php_stream_fill_read_buffer(stream, PHP_CSV_STREAM_CHUNK_SIZE) != SUCCESS)) {
			zend_throw_error(NULL, "Failed to read from the CSV file");
			return;
		}
		if (stream->writepos > stream->readpos) {
			size_t available = (size_t) (stream->writepos - stream->readpos);
			smart_str_appendl(stream_buffer, (const char *) stream->readbuf + stream->readpos, available);
			stream->readpos += available;
			stream->position += available;
			continue;
		}
		/* The buffer could not be filled although the EOF flag was not set at the start
		 * of this iteration: no more data can be obtained, so treat the stream exactly
		 * like EOF instead of spinning on it. */
		if (end_of_row != NULL) {
			php_csv_lazy_collection_parse_buffered_row(lazy_collection, current_row, start, end_of_row);
			return;
		}
		if (!has_buffered_data) {
			return;
		}
		php_csv_lazy_collection_parse_buffered_row(lazy_collection, current_row, start, end);
		return;
	}
}

/**
 * Csv\LazyLaxCollection internal iterator
 * Copied from zend_test/iterator.c
 *
 * The iteration state lives in the iterator, so that nested loops over the same collection are
 * independent. In stream mode the position in the stream is shared, so get_iterator() allows a
 * single active iterator.
 */
typedef struct php_csv_lazy_collection_it {
	zend_object_iterator intern;
	/* Buffer mode: where the next row starts in the collection's buffer */
	const char *buffer_position;
	/* Cannot use a HashTable as we need to be able to return a zval for current() */
	zval current_row;
} php_csv_lazy_collection_it;

static php_csv_lazy_collection_it *php_csv_lazy_collection_it_fetch(zend_object_iterator *obj_iter) {
	return (php_csv_lazy_collection_it *)obj_iter;
}

static void php_csv_lazy_collection_it_dtor(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);

	zval_ptr_dtor(&iterator->current_row);
	if (lazy_collection->is_stream_mode) {
		lazy_collection->stream_has_active_iterator = false;
	}
	zval_ptr_dtor(&iterator->intern.data);
}

static void php_csv_lazy_collection_it_next(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);

	zval_ptr_dtor(&iterator->current_row);
	ZVAL_UNDEF(&iterator->current_row);

	if (lazy_collection->is_stream_mode) {
		/* The stream is gone once the collection has been destroyed (e.g. at shutdown) */
		if (lazy_collection->stream != NULL) {
			php_csv_lazy_collection_stream_next(lazy_collection, &iterator->current_row);
		}
		return;
	}

	/* Do not move past eof */
	const char *end_of_buffer = ZSTR_VAL(lazy_collection->buffer) + ZSTR_LEN(lazy_collection->buffer);
	if (iterator->buffer_position == end_of_buffer) {
		return;
	}

	HashTable *row_ht = rfc4180_string_to_hashtable(&iterator->buffer_position, end_of_buffer, &lazy_collection->dialect);
	if (UNEXPECTED(row_ht == NULL)) {
		return;
	}

	ZVAL_ARR(&iterator->current_row, row_ht);
}

static void php_csv_lazy_collection_it_rewind(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);
	zval_ptr_dtor(&iterator->current_row);
	ZVAL_UNDEF(&iterator->current_row);

	if (lazy_collection->is_stream_mode) {
		if (lazy_collection->stream_iteration_started && lazy_collection->stream != NULL) {
			if (UNEXPECTED(php_stream_rewind(lazy_collection->stream) != 0)) {
				zend_throw_error(NULL, "Cannot rewind the CSV file stream");
				return;
			}
			smart_str_free(&lazy_collection->stream_buffer);
			lazy_collection->stream_buffer_position = 0;
			lazy_collection->stream_scan_position = 0;
			lazy_collection->stream_scan_in_escaped_field = false;
		}
		lazy_collection->stream_iteration_started = true;
	} else {
		iterator->buffer_position = ZSTR_VAL(lazy_collection->buffer);
	}

	/* Fetch first row as this is what is expected */
	php_csv_lazy_collection_it_next(obj_iter);
}

static zend_result php_csv_lazy_collection_it_valid(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);

	/* Upon reaching EOF the current row is freed and set to undef */
	return Z_ISUNDEF(iterator->current_row) ? FAILURE : SUCCESS;
}

static zval *php_csv_lazy_collection_it_current(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);

	if (UNEXPECTED(Z_ISUNDEF(iterator->current_row))) {
		return NULL;
	}
	return &iterator->current_row;
}

static const zend_object_iterator_funcs php_csv_lazy_collection_it_vtable = {
	php_csv_lazy_collection_it_dtor,
	php_csv_lazy_collection_it_valid,
	php_csv_lazy_collection_it_current,
	NULL, // get_current_key
	php_csv_lazy_collection_it_next,
	php_csv_lazy_collection_it_rewind,
	NULL, // invalidate_current
	NULL, // get_gc
};

static zend_object_iterator *php_csv_lazy_collection_get_iterator(
	zend_class_entry *ce,
	zval *object,
	int by_ref
) {
	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(object);
	if (lazy_collection->is_stream_mode) {
		if (UNEXPECTED(lazy_collection->stream_has_active_iterator)) {
			zend_throw_error(NULL, "A Csv\\LazyLaxCollection created from a file cannot be iterated by more than one loop at a time");
			return NULL;
		}
		lazy_collection->stream_has_active_iterator = true;
	}

	php_csv_lazy_collection_it *iterator = emalloc(sizeof(php_csv_lazy_collection_it));
	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &php_csv_lazy_collection_it_vtable;
	iterator->buffer_position = NULL;
	ZVAL_UNDEF(&iterator->current_row);

	return (zend_object_iterator*)iterator;
}

PHP_METHOD(Csv_LazyLaxCollection, __construct) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(NULL, "Cannot manually instantiate Csv\\LazyLaxCollection");
}

PHP_METHOD(Csv_LazyLaxCollection, getIterator) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

/* See static void buffer_to_collection_generic */
PHP_METHOD(Csv_LazyLaxCollection, createFromBuffer) {
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;
	zend_string *buffer;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|SSS", &buffer, &delimiter, &enclosure, &eol_sequence) == FAILURE) {
		RETURN_THROWS();
	}

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(2, 3, 4);

	zend_string_addref(buffer);

	object_init_ex(return_value, php_csv_lazy_collection_ce);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(return_value);

	/* We have copies from the macro */
	lazy_collection->eol_sequence = eol_sequence;
	lazy_collection->enclosure = enclosure;
	lazy_collection->delimiter = delimiter;
	csv_dialect_init(&lazy_collection->dialect, delimiter, enclosure, eol_sequence);
	lazy_collection->buffer = buffer;
}

PHP_METHOD(Csv_LazyLaxCollection, createFromFile) {
	zend_string *file = NULL;
	zend_string *delimiter = NULL;
	zend_string *enclosure = NULL;
	zend_string *eol_sequence = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_PATH_STR(file)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(delimiter)
		Z_PARAM_STR(enclosure)
		Z_PARAM_STR(eol_sequence)
	ZEND_PARSE_PARAMETERS_END();

	EOL_SEQUENCE_AND_DELIMITER_AND_ENCLOSURE_CHECKS(2, 3, 4);

	php_stream *stream = php_csv_stream_open(file, "rb", "reading");
	if (UNEXPECTED(stream == NULL)) {
		zend_string_release(eol_sequence);
		zend_string_release(delimiter);
		zend_string_release(enclosure);
		RETURN_THROWS();
	}

	object_init_ex(return_value, php_csv_lazy_collection_ce);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(return_value);

	/* We have copies from the macro */
	lazy_collection->eol_sequence = eol_sequence;
	lazy_collection->enclosure = enclosure;
	lazy_collection->delimiter = delimiter;
	csv_dialect_init(&lazy_collection->dialect, delimiter, enclosure, eol_sequence);
	lazy_collection->is_stream_mode = true;
	lazy_collection->stream = stream;
}

PHP_MINIT_FUNCTION(csv)
{
	php_csv_lazy_collection_ce = register_class_Csv_LazyLaxCollection(zend_ce_aggregate);
	php_csv_lazy_collection_ce->create_object = php_csv_lazy_collection_object_new;
	php_csv_lazy_collection_ce->get_iterator = php_csv_lazy_collection_get_iterator;
	php_csv_lazy_collection_ce->default_object_handlers = &php_csv_lazy_collection_object_handlers;

	memcpy(&php_csv_lazy_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_csv_lazy_collection_object_handlers.offset = offsetof(php_csv_lazy_collection_object, std);
	php_csv_lazy_collection_object_handlers.dtor_obj = php_csv_lazy_collection_object_dtor;
	php_csv_lazy_collection_object_handlers.free_obj = php_csv_lazy_collection_object_free;
	php_csv_lazy_collection_object_handlers.get_constructor = php_csv_lazy_collection_get_constructor;
	php_csv_lazy_collection_object_handlers.compare = zend_objects_not_comparable;
	php_csv_lazy_collection_object_handlers.clone_obj = NULL;

	return SUCCESS;
}

zend_module_entry csv_module_entry = {
	STANDARD_MODULE_HEADER,
	"csv",					/* Extension name */
	ext_functions,			/* zend_function_entry */
	PHP_MINIT(csv),			/* PHP_MINIT - Module initialization */
	NULL,					/* PHP_MSHUTDOWN - Module shutdown */
	NULL,					/* PHP_RINIT - Request initialization */
	NULL,					/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(csv),			/* PHP_MINFO - Module info */
	PHP_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CSV
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(csv)
#endif
