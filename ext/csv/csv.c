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
#include "ext/standard/php_string.h" /* For php_str_to_str() */
#include "php_csv.h"
#include "csv_arginfo.h"

#include <stdbool.h>
#include "zend_smart_str.h"
#include "zend_interfaces.h"

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

static bool buffer_starts_with_zend_string(const char *buffer, const char *end, const zend_string *needle) {
	size_t buffer_len = end-buffer;
	return buffer_len >= ZSTR_LEN(needle) && !memcmp(buffer, ZSTR_VAL(needle), ZSTR_LEN(needle));
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
	const zend_string *delimiter,
	const zend_string *enclosure,
	const zend_string *eol_sequence
) {
	HashTable *return_value = zend_new_array(8);

	bool in_escaped_field = false;
	bool at_start_of_field = true;

	/* Dereference buffer */
	const char *row = *buffer;
	/* row_to_array('') passes an empty buffer, which is a single empty field */
	ZEND_ASSERT(row <= end_buffer);

	smart_str field_value = {0};

	/* Main loop to "tokenize" the row */
	while (row < end_buffer) {
		/* Check for field escape sequence (i.e. enclosure) */
		if (buffer_starts_with_zend_string(row, end_buffer, enclosure)) {
			row += ZSTR_LEN(enclosure);

			if (!in_escaped_field) {
				if (UNEXPECTED(!at_start_of_field)) {
					zend_value_error("Enclosure sequence is used in a non escaped field");
					smart_str_free(&field_value);
					zend_array_destroy(return_value);
					return NULL;
				}
				in_escaped_field = true;
				continue;
			}

			/* In an escaped field: a doubled enclosure sequence stays in the field.
			 * Consume the whole sequence at once, otherwise a multibyte enclosure would
			 * be re-matched against its own tail bytes. */
			if (buffer_starts_with_zend_string(row, end_buffer, enclosure)) {
				smart_str_appendl(&field_value, ZSTR_VAL(enclosure), ZSTR_LEN(enclosure));
				row += ZSTR_LEN(enclosure);
				continue;
			}

			in_escaped_field = false;
			continue;
		}

		/* Check for delimiter if not in an escaped field */
		if (
			!in_escaped_field
			&& buffer_starts_with_zend_string(row, end_buffer, delimiter)
		) {
			row += ZSTR_LEN(delimiter);

			/* Add nul terminating byte */
			zend_string *value = smart_str_extract(&field_value);
			zval tmp;
			ZVAL_STR(&tmp, value);
			zend_hash_next_index_insert(return_value, &tmp);

			at_start_of_field = true;
			continue;
		}

		/* Check for End Of Line sequence when not in an escaped field */
		if (
			!in_escaped_field
			&& buffer_starts_with_zend_string(row, end_buffer, eol_sequence)
		) {
			row += ZSTR_LEN(eol_sequence);
			goto eol;
		}

		at_start_of_field = false;
		smart_str_appendc(&field_value, row[0]);
		row++;
	}

	eol:;
	/* Add nul terminating byte */
	zend_string *value = smart_str_extract(&field_value);
	zval tmp;
	ZVAL_STR(&tmp, value);
	zend_hash_next_index_insert(return_value, &tmp);

	smart_str_free(&field_value);
	/* Update outer buffer position */
	*buffer = row;

	return return_value;
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

	while (current_position - start_position < (ptrdiff_t) length) {
		uint32_t new_nb_fields = 0;
		HashTable *row = rfc4180_string_to_hashtable(&current_position, end_buffer, delimiter, enclosure, eol_sequence);

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

		zend_string *result = hashtable_to_rfc4180_string(Z_ARRVAL_P(fields), delimiter, enclosure, eol_sequence);
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

/* Streams opened by this extension are internal implementation details. Every opened stream
 * is also registered in EG(regular_list), where userland can reach it (e.g. via
 * get_resources()) and close it while we still hold a pointer to it. Detach the stream from
 * its resource so the extension is the sole owner of the stream and php_stream_close() in
 * the owning code path is the only way it is freed. */
static void php_csv_stream_make_private(php_stream *stream)
{
	zend_resource *res = stream->res;

	stream->res = NULL;

	/* Turn the resource into a closed one, the same representation zend_resource_dtor()
	 * leaves behind, so that references userland retained while the stream was being opened
	 * (e.g. a stream filter's onCreate() calling get_resources()) see an invalid stream
	 * instead of freed memory. Then drop the stream's own reference: the regular-list entry
	 * is removed now if it was the last one, or by zend_list_free() once userland releases
	 * its references. As the type is negative, the list destructor only frees the
	 * zend_resource itself. */
	res->ptr = NULL;
	res->type = -1;
	zend_list_delete(res);
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

	php_stream *stream = php_stream_open_wrapper_ex(ZSTR_VAL(file), "wb", 0, NULL, NULL);
	if (UNEXPECTED(stream == NULL)) {
		zend_string_release(eol_sequence);
		zend_string_release(delimiter);
		zend_string_release(enclosure);
		zend_throw_error(NULL, "Failed to open \"%s\" for writing", ZSTR_VAL(file));
		RETURN_THROWS();
	}
	php_csv_stream_make_private(stream);

	zval *fields;
	size_t collection_index = 0;
	uint32_t nb_fields = 0;
	bool has_errors = false;
	CSV_ITERABLE_FOREACH_VAL(collection, fields, end) {
		/* Check that fields is an array */
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

		zend_string *result = hashtable_to_rfc4180_string(Z_ARRVAL_P(fields), delimiter, enclosure, eol_sequence);
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
	fields = rfc4180_string_to_hashtable(&row_c, end_row, delimiter, enclosure, eol_sequence);
	zend_string_release(eol_sequence);
	zend_string_release(delimiter);
	zend_string_release(enclosure);

	if (UNEXPECTED(fields == NULL)) {
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
	const char *buffer_current_position;
	/* Stream mode (createFromFile): a private stream and a sliding window of not yet parsed data */
	php_stream *stream;
	smart_str stream_buffer;
	size_t stream_buffer_position;
	bool stream_iteration_started;
	/* Common */
	zend_string *delimiter;
	zend_string *enclosure;
	zend_string *eol_sequence;
	/* Cannot use a HashTable as we need to be able to return a zval for current() */
	zval current_row;
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

	ZVAL_UNDEF(&lazy_collection->current_row);
	return &lazy_collection->std;
}

static void php_csv_lazy_collection_object_free(zend_object *std)
{
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_from(std);
	zval_ptr_dtor(&lazy_collection->current_row);

	/* PHP Will create an object even if php_csv_lazy_collection_get_constructor() throws,
	 * So we cannot assume the buffers and various CSV settings are set */
	if (lazy_collection->buffer != NULL) {
		zend_string_release(lazy_collection->buffer);
		lazy_collection->buffer = NULL;
	}

	if (lazy_collection->stream != NULL) {
		if (!(EG(flags) & EG_FLAGS_IN_RESOURCE_SHUTDOWN)) {
			php_stream_close(lazy_collection->stream);
		} else if (php_stream_is(lazy_collection->stream, PHP_STREAM_IS_STDIO)
			&& lazy_collection->stream->readfilters.head == NULL
			&& lazy_collection->stream->writefilters.head == NULL) {
			/* The stream's resource entry was detached in php_csv_stream_make_private(),
			 * so resource shutdown will not close it; force-closing a plain, unfiltered
			 * file here is safe and releases the descriptor (same as ext/mysqlnd does
			 * for its socket). Everything else is deliberately left to the memory
			 * manager: closing a userspace-wrapper or filtered stream now could call
			 * back into the already shut down executor (e.g. a user filter's onClose()),
			 * and closing other wrapper streams could touch freed wrapper state. The
			 * known cost is that a native descriptor duplicated by such a wrapper (e.g.
			 * compress.zlib://) is not released when the collection survives until
			 * resource shutdown. */
			php_stream_free(lazy_collection->stream, PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_RSRC_DTOR);
		}
		lazy_collection->stream = NULL;
	}
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

static const char* php_csv_lazy_collection_object_get_end_of_buffer(const php_csv_lazy_collection_object *lazy_collection) {
	return ZSTR_VAL(lazy_collection->buffer) + ZSTR_LEN(lazy_collection->buffer);
}

static bool php_csv_lazy_collection_is_buffer_at_eof(const php_csv_lazy_collection_object *lazy_collection) {
	return lazy_collection->buffer_current_position == php_csv_lazy_collection_object_get_end_of_buffer(lazy_collection);
}

/**
 * Find the end of the first complete row in [position, end), honouring the enclosure rules of
 * rfc4180_string_to_hashtable(): an EOL sequence inside an escaped (enclosed) field does not
 * terminate the row, and a doubled enclosure sequence stays inside the field.
 *
 * Returns a pointer one past the row's EOL sequence, or NULL when no complete row is available
 * yet (i.e. more data must be read from the stream).
 *
 * The scanner does not reproduce the parser's "enclosure in a non escaped field" error; on such
 * input it may over-approximate the row length, and the parser then reports the error. A doubled
 * enclosure or a multibyte EOL split across a read boundary can make a single scan come up empty
 * or short, which is harmless: the caller re-scans from the start of the row after every read.
 */
static const char *csv_find_end_of_row(
	const char *position,
	const char *end,
	const zend_string *delimiter,
	const zend_string *enclosure,
	const zend_string *eol_sequence
) {
	bool in_escaped_field = false;

	while (position < end) {
		if (buffer_starts_with_zend_string(position, end, enclosure)) {
			position += ZSTR_LEN(enclosure);
			if (in_escaped_field) {
				if (buffer_starts_with_zend_string(position, end, enclosure)) {
					/* Doubled enclosure: still inside the field */
					position += ZSTR_LEN(enclosure);
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

/* Parse one row from the stream buffer window [start, end) and advance the consumed position. */
static void php_csv_lazy_collection_parse_buffered_row(
	php_csv_lazy_collection_object *lazy_collection,
	const char *start,
	const char *end
) {
	const char *position = start;
	HashTable *row_ht = rfc4180_string_to_hashtable(
		&position,
		end,
		lazy_collection->delimiter,
		lazy_collection->enclosure,
		lazy_collection->eol_sequence
	);
	if (UNEXPECTED(row_ht == NULL)) {
		/* An Error has been thrown; leaving current_row undef ends the iteration */
		return;
	}

	lazy_collection->stream_buffer_position += (size_t) (position - start);
	ZVAL_ARR(&lazy_collection->current_row, row_ht);

	/* Compact the buffer so memory stays proportional to the longest row, not the file */
	if (lazy_collection->stream_buffer_position >= PHP_CSV_STREAM_BUFFER_COMPACT_THRESHOLD) {
		zend_string *s = lazy_collection->stream_buffer.s;
		size_t remaining = ZSTR_LEN(s) - lazy_collection->stream_buffer_position;
		memmove(ZSTR_VAL(s), ZSTR_VAL(s) + lazy_collection->stream_buffer_position, remaining);
		ZSTR_LEN(s) = remaining;
		lazy_collection->stream_buffer_position = 0;
	}
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

static void php_csv_lazy_collection_stream_next(php_csv_lazy_collection_object *lazy_collection)
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
			end_of_row = csv_find_end_of_row(start, end,
				lazy_collection->delimiter, lazy_collection->enclosure, lazy_collection->eol_sequence);
		}
		bool at_eof = php_stream_eof(lazy_collection->stream);

		/* Only trust a row boundary when the dialect-specific lookahead is available past
		 * it, or when no further bytes can arrive; re-reading and re-scanning resolves
		 * boundary ambiguities (see csv_scanner_lookahead_needed()). */
		if (end_of_row != NULL && (at_eof || (size_t) (end - end_of_row) >= lookahead_needed)) {
			php_csv_lazy_collection_parse_buffered_row(lazy_collection, start, end_of_row);
			return;
		}

		if (at_eof) {
			if (!has_buffered_data) {
				/* End of iteration; current_row stays undef */
				return;
			}
			/* Final row without a terminating EOL sequence: hand the whole remainder to the
			 * parser, mirroring the end-of-buffer behaviour of createFromBuffer(). */
			php_csv_lazy_collection_parse_buffered_row(lazy_collection, start, end);
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
			php_csv_lazy_collection_parse_buffered_row(lazy_collection, start, end_of_row);
			return;
		}
		if (!has_buffered_data) {
			return;
		}
		php_csv_lazy_collection_parse_buffered_row(lazy_collection, start, end);
		return;
	}
}

/**
 * Csv\LazyLaxCollection internal iterator
 * Copied from zend_test/iterator.c
 */
typedef struct php_csv_lazy_collection_it {
	zend_object_iterator intern;
} php_csv_lazy_collection_it;

static php_csv_lazy_collection_it *php_csv_lazy_collection_it_fetch(zend_object_iterator *obj_iter) {
	return (php_csv_lazy_collection_it *)obj_iter;
}

static void php_csv_lazy_collection_it_dtor(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	zval_ptr_dtor(&iterator->intern.data);
}

static void php_csv_lazy_collection_it_next(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);

	zval_ptr_dtor(&lazy_collection->current_row);
	ZVAL_UNDEF(&lazy_collection->current_row);

	if (lazy_collection->stream != NULL) {
		php_csv_lazy_collection_stream_next(lazy_collection);
		return;
	}

	/* Do not move past eof */
	if (php_csv_lazy_collection_is_buffer_at_eof(lazy_collection)) {
		return;
	}

	HashTable *row_ht = rfc4180_string_to_hashtable(
		&lazy_collection->buffer_current_position,
		php_csv_lazy_collection_object_get_end_of_buffer(lazy_collection),
		lazy_collection->delimiter,
		lazy_collection->enclosure,
		lazy_collection->eol_sequence
	);
	if (UNEXPECTED(row_ht == NULL)) {
		return;
	}

	ZVAL_ARR(&lazy_collection->current_row, row_ht);
}

static void php_csv_lazy_collection_it_rewind(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);
	zval_ptr_dtor(&lazy_collection->current_row);
	ZVAL_UNDEF(&lazy_collection->current_row);

	if (lazy_collection->stream != NULL) {
		if (lazy_collection->stream_iteration_started) {
			if (UNEXPECTED(php_stream_rewind(lazy_collection->stream) != 0)) {
				zend_throw_error(NULL, "Cannot rewind the CSV file stream");
				return;
			}
			smart_str_free(&lazy_collection->stream_buffer);
			lazy_collection->stream_buffer_position = 0;
		}
		lazy_collection->stream_iteration_started = true;
	} else {
		lazy_collection->buffer_current_position = ZSTR_VAL(lazy_collection->buffer);
	}

	/* Fetch first row as this is what is expected */
	php_csv_lazy_collection_it_next(obj_iter);
}

static zend_result php_csv_lazy_collection_it_valid(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	const php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);

	/* Upon reaching EOF the current row is freed and set to undef */
	return Z_ISUNDEF(lazy_collection->current_row) ? FAILURE : SUCCESS;
}

static zval *php_csv_lazy_collection_it_current(zend_object_iterator *obj_iter) {
	php_csv_lazy_collection_it *iterator = php_csv_lazy_collection_it_fetch(obj_iter);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(&iterator->intern.data);

	if (UNEXPECTED(Z_ISUNDEF(lazy_collection->current_row))) {
		return NULL;
	}
	return &lazy_collection->current_row;
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

	php_csv_lazy_collection_it *iterator = emalloc(sizeof(php_csv_lazy_collection_it));
	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &php_csv_lazy_collection_it_vtable;

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

	php_stream *stream = php_stream_open_wrapper_ex(ZSTR_VAL(file), "rb", 0, NULL, NULL);
	if (UNEXPECTED(stream == NULL)) {
		zend_string_release(eol_sequence);
		zend_string_release(delimiter);
		zend_string_release(enclosure);
		zend_throw_error(NULL, "Failed to open \"%s\" for reading", ZSTR_VAL(file));
		RETURN_THROWS();
	}
	php_csv_stream_make_private(stream);

	object_init_ex(return_value, php_csv_lazy_collection_ce);
	php_csv_lazy_collection_object *lazy_collection = php_csv_lazy_collection_object_fetch(return_value);

	/* We have copies from the macro */
	lazy_collection->eol_sequence = eol_sequence;
	lazy_collection->enclosure = enclosure;
	lazy_collection->delimiter = delimiter;
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
