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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jani Taskinen <jani@php.net>                                |
   +----------------------------------------------------------------------+
 */

/*
 *  This product includes software developed by the Apache Group
 *  for use in the Apache HTTP server project (http://www.apache.org/).
 *
 */

#include <stdio.h>
#include "php.h"
#include "php_open_temporary_file.h"
#include "zend_globals.h"
#include "php_globals.h"
#include "php_variables.h"
#include "rfc1867.h"
#include "zend_smart_string.h"
#include "zend_exceptions.h"

#ifndef DEBUG_FILE_UPLOAD
# define DEBUG_FILE_UPLOAD 0
#endif

static int dummy_encoding_translation(void)
{
	return 0;
}

static char *php_ap_getword(const zend_encoding *encoding, char **line, char stop);
static char *php_ap_getword_conf(const zend_encoding *encoding, char *str);

static php_rfc1867_encoding_translation_t php_rfc1867_encoding_translation = dummy_encoding_translation;
static php_rfc1867_get_detect_order_t php_rfc1867_get_detect_order = NULL;
static php_rfc1867_set_input_encoding_t php_rfc1867_set_input_encoding = NULL;
static php_rfc1867_getword_t php_rfc1867_getword = php_ap_getword;
static php_rfc1867_getword_conf_t php_rfc1867_getword_conf = php_ap_getword_conf;
static php_rfc1867_basename_t php_rfc1867_basename = NULL;

PHPAPI zend_result (*php_rfc1867_callback)(unsigned int event, void *event_data, void **extra) = NULL;

static void safe_php_register_variable(char *var, char *strval, size_t val_len, zval *track_vars_array, bool override_protection);

/* The longest property name we use in an uploaded file array */
#define MAX_SIZE_OF_INDEX sizeof("[full_path]")

/* The longest anonymous name */
#define MAX_SIZE_ANONNAME 33

static void normalize_protected_variable(char *varname) /* {{{ */
{
	char *s = varname, *index = NULL, *indexend = NULL, *p;

	/* skip leading space */
	while (*s == ' ') {
		s++;
	}

	/* and remove it */
	if (s != varname) {
		memmove(varname, s, strlen(s)+1);
	}

	for (p = varname; *p && *p != '['; p++) {
		switch(*p) {
			case ' ':
			case '.':
				*p = '_';
				break;
		}
	}

	/* find index */
	index = strchr(varname, '[');
	if (index) {
		index++;
		s = index;
	} else {
		return;
	}

	/* done? */
	while (index) {
		while (*index == ' ' || *index == '\r' || *index == '\n' || *index=='\t') {
			index++;
		}
		indexend = strchr(index, ']');
		indexend = indexend ? indexend + 1 : index + strlen(index);

		if (s != index) {
			memmove(s, index, strlen(index)+1);
			s += indexend-index;
		} else {
			s = indexend;
		}

		if (*s == '[') {
			s++;
			index = s;
		} else {
			index = NULL;
		}
	}
	*s = '\0';
}
/* }}} */

static void add_protected_variable(char *varname) /* {{{ */
{
	normalize_protected_variable(varname);
	zend_hash_str_add_empty_element(&PG(rfc1867_protected_variables), varname, strlen(varname));
}
/* }}} */

static bool is_protected_variable(char *varname) /* {{{ */
{
	normalize_protected_variable(varname);
	return zend_hash_str_exists(&PG(rfc1867_protected_variables), varname, strlen(varname));
}
/* }}} */

static void safe_php_register_variable(char *var, char *strval, size_t val_len, zval *track_vars_array, bool override_protection) /* {{{ */
{
	if (override_protection || !is_protected_variable(var)) {
		php_register_variable_safe(var, strval, val_len, track_vars_array);
	}
}
/* }}} */

static void safe_php_register_variable_ex(char *var, zval *val, zval *track_vars_array, bool override_protection) /* {{{ */
{
	if (override_protection || !is_protected_variable(var)) {
		php_register_variable_ex(var, val, track_vars_array);
	}
}
/* }}} */

static void register_http_post_files_variable(char *strvar, char *val, zval *http_post_files, bool override_protection) /* {{{ */
{
	safe_php_register_variable(strvar, val, strlen(val), http_post_files, override_protection);
}
/* }}} */

static void register_http_post_files_variable_ex(char *var, zval *val, zval *http_post_files, bool override_protection) /* {{{ */
{
	safe_php_register_variable_ex(var, val, http_post_files, override_protection);
}
/* }}} */

static void free_filename(zval *el) {
	zend_string *filename = Z_STR_P(el);
	zend_string_release_ex(filename, 0);
}

PHPAPI void destroy_uploaded_files_hash(void) /* {{{ */
{
	zval *el;

	ZEND_HASH_MAP_FOREACH_VAL(SG(rfc1867_uploaded_files), el) {
		zend_string *filename = Z_STR_P(el);
		VCWD_UNLINK(ZSTR_VAL(filename));
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(SG(rfc1867_uploaded_files));
	FREE_HASHTABLE(SG(rfc1867_uploaded_files));
}
/* }}} */

/* {{{ Following code is based on apache_multipart_buffer.c from libapreq-0.33 package. */

#define FILLUNIT (1024 * 5)

typedef struct {

	/* read buffer */
	char *buffer;
	char *buf_begin;
	int  bufsize;
	int  bytes_in_buffer;

	/* boundary info */
	char *boundary;
	char *boundary_next;
	int  boundary_next_len;

	const zend_encoding *input_encoding;
	const zend_encoding **detect_order;
	size_t detect_order_size;
} multipart_buffer;

typedef struct {
	char *key;
	char *value;
} mime_header_entry;

/*
 * Fill up the buffer with client data.
 * Returns number of bytes added to buffer.
 */
static int fill_buffer(multipart_buffer *self)
{
	int bytes_to_read, total_read = 0, actual_read = 0;

	/* shift the existing data if necessary */
	if (self->bytes_in_buffer > 0 && self->buf_begin != self->buffer) {
		memmove(self->buffer, self->buf_begin, self->bytes_in_buffer);
	}

	self->buf_begin = self->buffer;

	/* calculate the free space in the buffer */
	bytes_to_read = self->bufsize - self->bytes_in_buffer;

	/* read the required number of bytes */
	while (bytes_to_read > 0) {

		char *buf = self->buffer + self->bytes_in_buffer;
		actual_read = (int)sapi_module.read_post(buf, bytes_to_read);

		/* update the buffer length */
		if (actual_read > 0) {
			self->bytes_in_buffer += actual_read;
			SG(read_post_bytes) += actual_read;
			total_read += actual_read;
			bytes_to_read -= actual_read;
		} else {
			break;
		}
	}

	return total_read;
}

/* eof if we are out of bytes, or if we hit the final boundary */
static int multipart_buffer_eof(multipart_buffer *self)
{
	return self->bytes_in_buffer == 0 && fill_buffer(self) < 1;
}

/* create new multipart_buffer structure */
static multipart_buffer *multipart_buffer_new(char *boundary, int boundary_len)
{
	multipart_buffer *self = (multipart_buffer *) ecalloc(1, sizeof(multipart_buffer));

	int minsize = boundary_len + 6;
	if (minsize < FILLUNIT) minsize = FILLUNIT;

	self->buffer = (char *) ecalloc(1, minsize + 1);
	self->bufsize = minsize;

	spprintf(&self->boundary, 0, "--%s", boundary);

	self->boundary_next_len = (int)spprintf(&self->boundary_next, 0, "\n--%s", boundary);

	self->buf_begin = self->buffer;
	self->bytes_in_buffer = 0;

	if (php_rfc1867_encoding_translation()) {
		php_rfc1867_get_detect_order(&self->detect_order, &self->detect_order_size);
	} else {
		self->detect_order = NULL;
		self->detect_order_size = 0;
	}

	self->input_encoding = NULL;

	return self;
}

/*
 * Gets the next CRLF terminated line from the input buffer.
 * If it doesn't find a CRLF, and the buffer isn't completely full, returns
 * NULL; otherwise, returns the beginning of the null-terminated line,
 * minus the CRLF.
 *
 * Note that we really just look for LF terminated lines. This works
 * around a bug in internet explorer for the macintosh which sends mime
 * boundaries that are only LF terminated when you use an image submit
 * button in a multipart/form-data form.
 */
static char *next_line(multipart_buffer *self)
{
	/* look for LF in the data */
	char* line = self->buf_begin;
	char* ptr = memchr(self->buf_begin, '\n', self->bytes_in_buffer);

	if (ptr) {	/* LF found */

		/* terminate the string, remove CRLF */
		if ((ptr - line) > 0 && *(ptr-1) == '\r') {
			*(ptr-1) = 0;
		} else {
			*ptr = 0;
		}

		/* bump the pointer */
		self->buf_begin = ptr + 1;
		self->bytes_in_buffer -= (self->buf_begin - line);

	} else {	/* no LF found */

		/* buffer isn't completely full, fail */
		if (self->bytes_in_buffer < self->bufsize) {
			return NULL;
		}
		/* return entire buffer as a partial line */
		line[self->bufsize] = 0;
		self->buf_begin = ptr;
		self->bytes_in_buffer = 0;
	}

	return line;
}

/* Returns the next CRLF terminated line from the client */
static char *get_line(multipart_buffer *self)
{
	char* ptr = next_line(self);

	if (!ptr) {
		fill_buffer(self);
		ptr = next_line(self);
	}

	return ptr;
}

/* Free header entry */
static void php_free_hdr_entry(mime_header_entry *h)
{
	if (h->key) {
		efree(h->key);
	}
	if (h->value) {
		efree(h->value);
	}
}

/* finds a boundary */
static int find_boundary(multipart_buffer *self, char *boundary)
{
	char *line;

	/* loop through lines */
	while( (line = get_line(self)) )
	{
		/* finished if we found the boundary */
		if (!strcmp(line, boundary)) {
			return 1;
		}
	}

	/* didn't find the boundary */
	return 0;
}

/* parse headers */
static int multipart_buffer_headers(multipart_buffer *self, zend_llist *header)
{
	char *line;
	mime_header_entry entry = {0};
	smart_string buf_value = {0};
	char *key = NULL;

	/* didn't find boundary, abort */
	if (!find_boundary(self, self->boundary)) {
		return 0;
	}

	/* get lines of text, or CRLF_CRLF */

	while ((line = get_line(self)) && line[0] != '\0') {
		/* add header to table */
		char *value = NULL;

		if (php_rfc1867_encoding_translation()) {
			self->input_encoding = zend_multibyte_encoding_detector((const unsigned char *) line, strlen(line), self->detect_order, self->detect_order_size);
		}

		/* space in the beginning means same header */
		if (!isspace(line[0])) {
			value = strchr(line, ':');
		}

		if (value) {
			if (buf_value.c && key) {
				/* new entry, add the old one to the list */
				smart_string_0(&buf_value);
				entry.key = key;
				entry.value = buf_value.c;
				zend_llist_add_element(header, &entry);
				buf_value.c = NULL;
				key = NULL;
			}

			*value = '\0';
			do { value++; } while (isspace(*value));

			key = estrdup(line);
			smart_string_appends(&buf_value, value);
		} else if (buf_value.c) { /* If no ':' on the line, add to previous line */
			smart_string_appends(&buf_value, line);
		} else {
			continue;
		}
	}

	if (buf_value.c && key) {
		/* add the last one to the list */
		smart_string_0(&buf_value);
		entry.key = key;
		entry.value = buf_value.c;
		zend_llist_add_element(header, &entry);
	}

	return 1;
}

static char *php_mime_get_hdr_value(zend_llist header, char *key)
{
	mime_header_entry *entry;

	if (key == NULL) {
		return NULL;
	}

	entry = zend_llist_get_first(&header);
	while (entry) {
		if (!strcasecmp(entry->key, key)) {
			return entry->value;
		}
		entry = zend_llist_get_next(&header);
	}

	return NULL;
}

static char *php_ap_getword(const zend_encoding *encoding, char **line, char stop)
{
	char *pos = *line, quote;
	char *res;

	while (*pos && *pos != stop) {
		if ((quote = *pos) == '"' || quote == '\'') {
			++pos;
			while (*pos && *pos != quote) {
				if (*pos == '\\' && pos[1] && pos[1] == quote) {
					pos += 2;
				} else {
					++pos;
				}
			}
			if (*pos) {
				++pos;
			}
		} else ++pos;
	}
	if (*pos == '\0') {
		res = estrdup(*line);
		*line += strlen(*line);
		return res;
	}

	res = estrndup(*line, pos - *line);

	while (*pos == stop) {
		++pos;
	}

	*line = pos;
	return res;
}

static char *substring_conf(char *start, int len, char quote)
{
	char *result = emalloc(len + 1);
	char *resp = result;
	int i;

	for (i = 0; i < len && start[i] != quote; ++i) {
		if (start[i] == '\\' && (start[i + 1] == '\\' || (quote && start[i + 1] == quote))) {
			*resp++ = start[++i];
		} else {
			*resp++ = start[i];
		}
	}

	*resp = '\0';
	return result;
}

static char *php_ap_getword_conf(const zend_encoding *encoding, char *str)
{
	while (*str && isspace(*str)) {
		++str;
	}

	if (!*str) {
		return estrdup("");
	}

	if (*str == '"' || *str == '\'') {
		char quote = *str;

		str++;
		return substring_conf(str, (int)strlen(str), quote);
	} else {
		char *strend = str;

		while (*strend && !isspace(*strend)) {
			++strend;
		}
		return substring_conf(str, strend - str, 0);
	}
}

static char *php_ap_basename(const zend_encoding *encoding, char *path)
{
	char *s = strrchr(path, '\\');
	char *s2 = strrchr(path, '/');

	if (s && s2) {
		if (s > s2) {
			++s;
		} else {
			s = ++s2;
		}
		return s;
	} else if (s) {
		return ++s;
	} else if (s2) {
		return ++s2;
	}
	return path;
}

/*
 * Search for a string in a fixed-length byte string.
 * If partial is true, partial matches are allowed at the end of the buffer.
 * Returns NULL if not found, or a pointer to the start of the first match.
 */
static void *php_ap_memstr(char *haystack, int haystacklen, char *needle, int needlen, int partial)
{
	int len = haystacklen;
	char *ptr = haystack;

	/* iterate through first character matches */
	while( (ptr = memchr(ptr, needle[0], len)) ) {

		/* calculate length after match */
		len = haystacklen - (ptr - (char *)haystack);

		/* done if matches up to capacity of buffer */
		if (memcmp(needle, ptr, needlen < len ? needlen : len) == 0 && (partial || len >= needlen)) {
			break;
		}

		/* next character */
		ptr++; len--;
	}

	return ptr;
}

/* read until a boundary condition */
static size_t multipart_buffer_read(multipart_buffer *self, char *buf, size_t bytes, int *end)
{
	size_t len, max;
	char *bound;

	/* fill buffer if needed */
	if (bytes > (size_t)self->bytes_in_buffer) {
		fill_buffer(self);
	}

	/* look for a potential boundary match, only read data up to that point */
	if ((bound = php_ap_memstr(self->buf_begin, self->bytes_in_buffer, self->boundary_next, self->boundary_next_len, 1))) {
		max = bound - self->buf_begin;
		if (end && php_ap_memstr(self->buf_begin, self->bytes_in_buffer, self->boundary_next, self->boundary_next_len, 0)) {
			*end = 1;
		}
	} else {
		max = self->bytes_in_buffer;
	}

	/* maximum number of bytes we are reading */
	len = max < bytes-1 ? max : bytes-1;

	/* if we read any data... */
	if (len > 0) {

		/* copy the data */
		memcpy(buf, self->buf_begin, len);
		buf[len] = 0;

		if (bound && len > 0 && buf[len-1] == '\r') {
			buf[--len] = 0;
		}

		/* update the buffer */
		self->bytes_in_buffer -= (int)len;
		self->buf_begin += len;
	}

	return len;
}

/*
  XXX: this is horrible memory-usage-wise, but we only expect
  to do this on small pieces of form data.
*/
static char *multipart_buffer_read_body(multipart_buffer *self, size_t *len)
{
	char buf[FILLUNIT], *out=NULL;
	size_t total_bytes=0, read_bytes=0;

	while((read_bytes = multipart_buffer_read(self, buf, sizeof(buf), NULL))) {
		out = erealloc(out, total_bytes + read_bytes + 1);
		memcpy(out + total_bytes, buf, read_bytes);
		total_bytes += read_bytes;
	}

	if (out) {
		out[total_bytes] = '\0';
	}
	*len = total_bytes;

	return out;
}
/* }}} */

/*
 * The combined READER/HANDLER
 *
 */

SAPI_API SAPI_POST_HANDLER_FUNC(rfc1867_post_handler)
{
	char *boundary, *s = NULL, *boundary_end = NULL, *start_arr = NULL, *array_index = NULL;
	char *lbuf = NULL, *abuf = NULL;
	zend_string *temp_filename = NULL;
	int boundary_len = 0, cancel_upload = 0, is_arr_upload = 0;
	size_t array_len = 0;
	int64_t total_bytes = 0, max_file_size = 0;
	int skip_upload = 0, anonymous_index = 0;
	HashTable *uploaded_files = NULL;
	multipart_buffer *mbuff;
	zval *array_ptr = (zval *) arg;
	bool throw_exceptions = SG(request_parse_body_context).throw_exceptions;
	int fd = -1;
	zend_llist header;
	void *event_extra_data = NULL;
	unsigned int llen = 0;
	zend_long upload_cnt = REQUEST_PARSE_BODY_OPTION_GET(max_file_uploads, INI_INT("max_file_uploads"));
	zend_long body_parts_cnt = REQUEST_PARSE_BODY_OPTION_GET(max_multipart_body_parts, INI_INT("max_multipart_body_parts"));
	zend_long post_max_size = REQUEST_PARSE_BODY_OPTION_GET(post_max_size, SG(post_max_size));
	zend_long max_input_vars = REQUEST_PARSE_BODY_OPTION_GET(max_input_vars, PG(max_input_vars));
	zend_long upload_max_filesize = REQUEST_PARSE_BODY_OPTION_GET(upload_max_filesize, PG(upload_max_filesize));
	const zend_encoding *internal_encoding = zend_multibyte_get_internal_encoding();
	php_rfc1867_getword_t getword;
	php_rfc1867_getword_conf_t getword_conf;
	php_rfc1867_basename_t _basename;
	zend_long count = 0;

#define EMIT_WARNING_OR_ERROR(...) do { \
		if (throw_exceptions) { \
			zend_throw_exception_ex(zend_ce_request_parse_body_exception, 0, __VA_ARGS__); \
		} else { \
			php_error_docref(NULL, E_WARNING, __VA_ARGS__); \
		} \
	} while (0)

	if (php_rfc1867_encoding_translation() && internal_encoding) {
		getword = php_rfc1867_getword;
		getword_conf = php_rfc1867_getword_conf;
		_basename = php_rfc1867_basename;
	} else {
		getword = php_ap_getword;
		getword_conf = php_ap_getword_conf;
		_basename = php_ap_basename;
	}

	if (post_max_size > 0 && SG(request_info).content_length > post_max_size) {
		EMIT_WARNING_OR_ERROR("POST Content-Length of " ZEND_LONG_FMT " bytes exceeds the limit of " ZEND_LONG_FMT " bytes", SG(request_info).content_length, post_max_size);
		return;
	}

	if (body_parts_cnt < 0) {
		body_parts_cnt = max_input_vars + upload_cnt;
	}
	int body_parts_limit = body_parts_cnt;

	/* Get the boundary */
	boundary = strstr(content_type_dup, "boundary");
	if (!boundary) {
		int content_type_len = (int)strlen(content_type_dup);
		char *content_type_lcase = estrndup(content_type_dup, content_type_len);

		zend_str_tolower(content_type_lcase, content_type_len);
		boundary = strstr(content_type_lcase, "boundary");
		if (boundary) {
			boundary = content_type_dup + (boundary - content_type_lcase);
		}
		efree(content_type_lcase);
	}

	if (!boundary || !(boundary = strchr(boundary, '='))) {
		EMIT_WARNING_OR_ERROR("Missing boundary in multipart/form-data POST data");
		return;
	}

	boundary++;
	boundary_len = (int)strlen(boundary);

	if (boundary[0] == '"') {
		boundary++;
		boundary_end = strchr(boundary, '"');
		if (!boundary_end) {
			EMIT_WARNING_OR_ERROR("Invalid boundary in multipart/form-data POST data");
			return;
		}
	} else {
		/* search for the end of the boundary */
		boundary_end = strpbrk(boundary, ",;");
	}
	if (boundary_end) {
		boundary_end[0] = '\0';
		boundary_len = boundary_end-boundary;
	}

	/* Initialize the buffer */
	mbuff = multipart_buffer_new(boundary, boundary_len);

	/* Initialize $_FILES[] */
	zend_hash_init(&PG(rfc1867_protected_variables), 8, NULL, NULL, 0);

	ALLOC_HASHTABLE(uploaded_files);
	zend_hash_init(uploaded_files, 8, NULL, free_filename, 0);
	SG(rfc1867_uploaded_files) = uploaded_files;

	if (Z_TYPE(PG(http_globals)[TRACK_VARS_FILES]) != IS_ARRAY) {
		/* php_auto_globals_create_files() might have already done that */
		array_init(&PG(http_globals)[TRACK_VARS_FILES]);
	}

	zend_llist_init(&header, sizeof(mime_header_entry), (llist_dtor_func_t) php_free_hdr_entry, 0);

	if (php_rfc1867_callback != NULL) {
		multipart_event_start event_start;

		event_start.content_length = SG(request_info).content_length;
		if (php_rfc1867_callback(MULTIPART_EVENT_START, &event_start, &event_extra_data) == FAILURE) {
			goto fileupload_done;
		}
	}

	while (!multipart_buffer_eof(mbuff))
	{
		char buff[FILLUNIT];
		char *cd = NULL, *param = NULL, *filename = NULL, *tmp = NULL;
		size_t blen = 0, wlen = 0;
		zend_off_t offset;

		zend_llist_clean(&header);

		if (!multipart_buffer_headers(mbuff, &header)) {
			goto fileupload_done;
		}

		if ((cd = php_mime_get_hdr_value(header, "Content-Disposition"))) {
			char *pair = NULL;
			int end = 0;

			if (--body_parts_cnt < 0) {
				EMIT_WARNING_OR_ERROR("Multipart body parts limit exceeded %d. To increase the limit change max_multipart_body_parts in php.ini.", body_parts_limit);
				goto fileupload_done;
			}

			while (isspace(*cd)) {
				++cd;
			}

			while (*cd && (pair = getword(mbuff->input_encoding, &cd, ';')))
			{
				char *key = NULL, *word = pair;

				while (isspace(*cd)) {
					++cd;
				}

				if (strchr(pair, '=')) {
					key = getword(mbuff->input_encoding, &pair, '=');

					if (!strcasecmp(key, "name")) {
						if (param) {
							efree(param);
						}
						param = getword_conf(mbuff->input_encoding, pair);
						if (mbuff->input_encoding && internal_encoding) {
							unsigned char *new_param;
							size_t new_param_len;
							if ((size_t)-1 != zend_multibyte_encoding_converter(&new_param, &new_param_len, (unsigned char *)param, strlen(param), internal_encoding, mbuff->input_encoding)) {
								efree(param);
								param = (char *)new_param;
							}
						}
					} else if (!strcasecmp(key, "filename")) {
						if (filename) {
							efree(filename);
						}
						filename = getword_conf(mbuff->input_encoding, pair);
						if (mbuff->input_encoding && internal_encoding) {
							unsigned char *new_filename;
							size_t new_filename_len;
							if ((size_t)-1 != zend_multibyte_encoding_converter(&new_filename, &new_filename_len, (unsigned char *)filename, strlen(filename), internal_encoding, mbuff->input_encoding)) {
								efree(filename);
								filename = (char *)new_filename;
							}
						}
					}
				}
				if (key) {
					efree(key);
				}
				efree(word);
			}

			/* Normal form variable, safe to read all data into memory */
			if (!filename && param) {
				size_t value_len;
				char *value = multipart_buffer_read_body(mbuff, &value_len);
				size_t new_val_len; /* Dummy variable */

				if (!value) {
					value = estrdup("");
					value_len = 0;
				}

				if (mbuff->input_encoding && internal_encoding) {
					unsigned char *new_value;
					size_t new_value_len;
					if ((size_t)-1 != zend_multibyte_encoding_converter(&new_value, &new_value_len, (unsigned char *)value, value_len, internal_encoding, mbuff->input_encoding)) {
						efree(value);
						value = (char *)new_value;
						value_len = new_value_len;
					}
				}

				if (++count <= max_input_vars && sapi_module.input_filter(PARSE_POST, param, &value, value_len, &new_val_len)) {
					if (php_rfc1867_callback != NULL) {
						multipart_event_formdata event_formdata;
						size_t newlength = new_val_len;

						event_formdata.post_bytes_processed = SG(read_post_bytes);
						event_formdata.name = param;
						event_formdata.value = &value;
						event_formdata.length = new_val_len;
						event_formdata.newlength = &newlength;
						if (php_rfc1867_callback(MULTIPART_EVENT_FORMDATA, &event_formdata, &event_extra_data) == FAILURE) {
							efree(param);
							efree(value);
							continue;
						}
						new_val_len = newlength;
					}
					safe_php_register_variable(param, value, new_val_len, array_ptr, 0);
				} else {
					if (count == max_input_vars + 1) {
						EMIT_WARNING_OR_ERROR("Input variables exceeded " ZEND_LONG_FMT ". To increase the limit change max_input_vars in php.ini.", max_input_vars);
					}

					if (php_rfc1867_callback != NULL) {
						multipart_event_formdata event_formdata;

						event_formdata.post_bytes_processed = SG(read_post_bytes);
						event_formdata.name = param;
						event_formdata.value = &value;
						event_formdata.length = value_len;
						event_formdata.newlength = NULL;
						php_rfc1867_callback(MULTIPART_EVENT_FORMDATA, &event_formdata, &event_extra_data);
					}
				}

				if (!strcasecmp(param, "MAX_FILE_SIZE")) {
					max_file_size = strtoll(value, NULL, 10);
				}

				efree(param);
				efree(value);
				continue;
			}

			/* If file_uploads=off, skip the file part */
			if (!PG(file_uploads)) {
				skip_upload = 1;
			} else if (upload_cnt <= 0) {
				skip_upload = 1;
				if (upload_cnt == 0) {
					--upload_cnt;
					EMIT_WARNING_OR_ERROR("Maximum number of allowable file uploads has been exceeded");
				}
			}

			/* Return with an error if the posted data is garbled */
			if (!param && !filename) {
				EMIT_WARNING_OR_ERROR("File Upload Mime headers garbled");
				goto fileupload_done;
			}

			if (!param) {
				param = emalloc(MAX_SIZE_ANONNAME);
				snprintf(param, MAX_SIZE_ANONNAME, "%u", anonymous_index++);
			}

			/* New Rule: never repair potential malicious user input */
			if (!skip_upload) {
				long c = 0;
				tmp = param;

				while (*tmp) {
					if (*tmp == '[') {
						c++;
					} else if (*tmp == ']') {
						c--;
						if (tmp[1] && tmp[1] != '[') {
							skip_upload = 1;
							break;
						}
					}
					if (c < 0) {
						skip_upload = 1;
						break;
					}
					tmp++;
				}
				/* Brackets should always be closed */
				if(c != 0) {
					skip_upload = 1;
				}
			}

			total_bytes = cancel_upload = 0;
			temp_filename = NULL;
			fd = -1;

			if (!skip_upload && php_rfc1867_callback != NULL) {
				multipart_event_file_start event_file_start;

				event_file_start.post_bytes_processed = SG(read_post_bytes);
				event_file_start.name = param;
				event_file_start.filename = &filename;
				if (php_rfc1867_callback(MULTIPART_EVENT_FILE_START, &event_file_start, &event_extra_data) == FAILURE) {
					temp_filename = NULL;
					efree(param);
					efree(filename);
					continue;
				}
			}

			if (skip_upload) {
				efree(param);
				efree(filename);
				continue;
			}

			if (filename[0] == '\0') {
#if DEBUG_FILE_UPLOAD
				sapi_module.sapi_error(E_NOTICE, "No file uploaded");
#endif
				cancel_upload = PHP_UPLOAD_ERROR_D;
			}

			offset = 0;
			end = 0;

			if (!cancel_upload) {
				/* only bother to open temp file if we have data */
				blen = multipart_buffer_read(mbuff, buff, sizeof(buff), &end);
#if DEBUG_FILE_UPLOAD
				if (blen > 0) {
#else
				/* in non-debug mode we have no problem with 0-length files */
				{
#endif
					fd = php_open_temporary_fd_ex(PG(upload_tmp_dir), "php", &temp_filename, PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ON_FALLBACK);
					upload_cnt--;
					if (fd == -1) {
						EMIT_WARNING_OR_ERROR("File upload error - unable to create a temporary file");
						cancel_upload = PHP_UPLOAD_ERROR_E;
					}
				}
			}

			while (!cancel_upload && (blen > 0))
			{
				if (php_rfc1867_callback != NULL) {
					multipart_event_file_data event_file_data;

					event_file_data.post_bytes_processed = SG(read_post_bytes);
					event_file_data.offset = offset;
					event_file_data.data = buff;
					event_file_data.length = blen;
					event_file_data.newlength = &blen;
					if (php_rfc1867_callback(MULTIPART_EVENT_FILE_DATA, &event_file_data, &event_extra_data) == FAILURE) {
						cancel_upload = PHP_UPLOAD_ERROR_X;
						continue;
					}
				}

				if (upload_max_filesize > 0 && (zend_long)(total_bytes+blen) > upload_max_filesize) {
#if DEBUG_FILE_UPLOAD
					sapi_module.sapi_error(E_NOTICE, "upload_max_filesize of " ZEND_LONG_FMT " bytes exceeded - file [%s=%s] not saved", upload_max_filesize, param, filename);
#endif
					cancel_upload = PHP_UPLOAD_ERROR_A;
				} else if (max_file_size && ((zend_long)(total_bytes+blen) > max_file_size)) {
#if DEBUG_FILE_UPLOAD
					sapi_module.sapi_error(E_NOTICE, "MAX_FILE_SIZE of %" PRId64 " bytes exceeded - file [%s=%s] not saved", max_file_size, param, filename);
#endif
					cancel_upload = PHP_UPLOAD_ERROR_B;
				} else if (blen > 0) {
#ifdef PHP_WIN32
					wlen = write(fd, buff, (unsigned int)blen);
#else
					wlen = write(fd, buff, blen);
#endif

					if (wlen == (size_t)-1) {
						/* write failed */
#if DEBUG_FILE_UPLOAD
						sapi_module.sapi_error(E_NOTICE, "write() failed - %s", strerror(errno));
#endif
						cancel_upload = PHP_UPLOAD_ERROR_F;
					} else if (wlen < blen) {
#if DEBUG_FILE_UPLOAD
						sapi_module.sapi_error(E_NOTICE, "Only %zd bytes were written, expected to write %zd", wlen, blen);
#endif
						cancel_upload = PHP_UPLOAD_ERROR_F;
					} else {
						total_bytes += wlen;
					}
					offset += wlen;
				}

				/* read data for next iteration */
				blen = multipart_buffer_read(mbuff, buff, sizeof(buff), &end);
			}

			if (fd != -1) { /* may not be initialized if file could not be created */
				close(fd);
			}

			if (!cancel_upload && !end) {
#if DEBUG_FILE_UPLOAD
				sapi_module.sapi_error(E_NOTICE, "Missing mime boundary at the end of the data for file %s", filename[0] != '\0' ? filename : "");
#endif
				cancel_upload = PHP_UPLOAD_ERROR_C;
			}
#if DEBUG_FILE_UPLOAD
			if (filename[0] != '\0' && total_bytes == 0 && !cancel_upload) {
				EMIT_WARNING_OR_ERROR("Uploaded file size 0 - file [%s=%s] not saved", param, filename);
				cancel_upload = 5;
			}
#endif
			if (php_rfc1867_callback != NULL) {
				multipart_event_file_end event_file_end;

				event_file_end.post_bytes_processed = SG(read_post_bytes);
				event_file_end.temp_filename = temp_filename ? ZSTR_VAL(temp_filename) : NULL;
				event_file_end.cancel_upload = cancel_upload;
				if (php_rfc1867_callback(MULTIPART_EVENT_FILE_END, &event_file_end, &event_extra_data) == FAILURE) {
					cancel_upload = PHP_UPLOAD_ERROR_X;
				}
			}

			if (cancel_upload) {
				if (temp_filename) {
					if (cancel_upload != PHP_UPLOAD_ERROR_E) { /* file creation failed */
						unlink(ZSTR_VAL(temp_filename));
					}
					zend_string_release_ex(temp_filename, 0);
				}
				temp_filename = NULL;
			} else {
				zend_hash_add_ptr(SG(rfc1867_uploaded_files), temp_filename, temp_filename);
			}

			/* is_arr_upload is true when name of file upload field
			 * ends in [.*]
			 * start_arr is set to point to 1st [ */
			is_arr_upload =	(start_arr = strchr(param,'[')) && (param[strlen(param)-1] == ']');

			if (is_arr_upload) {
				array_len = strlen(start_arr);
				if (array_index) {
					efree(array_index);
				}
				array_index = estrndup(start_arr + 1, array_len - 2);
			}

			/* Add $foo_name */
			if (llen < strlen(param) + MAX_SIZE_OF_INDEX + 1) {
				llen = (int)strlen(param);
				lbuf = (char *) safe_erealloc(lbuf, llen, 1, MAX_SIZE_OF_INDEX + 1);
				llen += MAX_SIZE_OF_INDEX + 1;
			}

			if (is_arr_upload) {
				if (abuf) efree(abuf);
				abuf = estrndup(param, strlen(param)-array_len);
				snprintf(lbuf, llen, "%s_name[%s]", abuf, array_index);
			} else {
				snprintf(lbuf, llen, "%s_name", param);
			}

			/* Pursuant to RFC 7578, strip any path components in the
			 * user-supplied file name:
			 *  > If a "filename" parameter is supplied ... do not use
			 *  > directory path information that may be present."
			 */
			s = _basename(internal_encoding, filename);
			if (!s) {
				s = filename;
			}

			/* Add $foo[name] */
			if (is_arr_upload) {
				snprintf(lbuf, llen, "%s[name][%s]", abuf, array_index);
			} else {
				snprintf(lbuf, llen, "%s[name]", param);
			}
			register_http_post_files_variable(lbuf, s, &PG(http_globals)[TRACK_VARS_FILES], 0);
			s = NULL;

			/* Add full path of supplied file for folder uploads via
			 * <input type="file" name="files" multiple webkitdirectory>
			 */
			/* Add $foo[full_path] */
			if (is_arr_upload) {
				snprintf(lbuf, llen, "%s[full_path][%s]", abuf, array_index);
			} else {
				snprintf(lbuf, llen, "%s[full_path]", param);
			}
			register_http_post_files_variable(lbuf, filename, &PG(http_globals)[TRACK_VARS_FILES], 0);
			efree(filename);

			/* Possible Content-Type: */
			if (cancel_upload || !(cd = php_mime_get_hdr_value(header, "Content-Type"))) {
				cd = "";
			} else {
				/* fix for Opera 6.01 */
				s = strchr(cd, ';');
				if (s != NULL) {
					*s = '\0';
				}
			}

			/* Add $foo[type] */
			if (is_arr_upload) {
				snprintf(lbuf, llen, "%s[type][%s]", abuf, array_index);
			} else {
				snprintf(lbuf, llen, "%s[type]", param);
			}
			register_http_post_files_variable(lbuf, cd, &PG(http_globals)[TRACK_VARS_FILES], 0);

			/* Restore Content-Type Header */
			if (s != NULL) {
				*s = ';';
			}
			s = "";

			{
				/* store temp_filename as-is (in case upload_tmp_dir
				 * contains escapable characters. escape only the variable name.) */
				zval zfilename;

				/* Initialize variables */
				add_protected_variable(param);

				/* Add $foo[tmp_name] */
				if (is_arr_upload) {
					snprintf(lbuf, llen, "%s[tmp_name][%s]", abuf, array_index);
				} else {
					snprintf(lbuf, llen, "%s[tmp_name]", param);
				}
				add_protected_variable(lbuf);
				if (temp_filename) {
					ZVAL_STR_COPY(&zfilename, temp_filename);
				} else {
					ZVAL_EMPTY_STRING(&zfilename);
				}
				register_http_post_files_variable_ex(lbuf, &zfilename, &PG(http_globals)[TRACK_VARS_FILES], 1);
			}

			{
				zval file_size, error_type;
				int size_overflow = 0;
				char file_size_buf[65];

				ZVAL_LONG(&error_type, cancel_upload);

				/* Add $foo[error] */
				if (cancel_upload) {
					ZVAL_LONG(&file_size, 0);
				} else {
					if (total_bytes > ZEND_LONG_MAX) {
#ifdef PHP_WIN32
						if (_i64toa_s(total_bytes, file_size_buf, 65, 10)) {
							file_size_buf[0] = '0';
							file_size_buf[1] = '\0';
						}
#else
						{
							int __len = snprintf(file_size_buf, 65, "%" PRId64, total_bytes);
							file_size_buf[__len] = '\0';
						}
#endif
						size_overflow = 1;

					} else {
						ZVAL_LONG(&file_size, total_bytes);
					}
				}

				if (is_arr_upload) {
					snprintf(lbuf, llen, "%s[error][%s]", abuf, array_index);
				} else {
					snprintf(lbuf, llen, "%s[error]", param);
				}
				register_http_post_files_variable_ex(lbuf, &error_type, &PG(http_globals)[TRACK_VARS_FILES], 0);

				/* Add $foo[size] */
				if (is_arr_upload) {
					snprintf(lbuf, llen, "%s[size][%s]", abuf, array_index);
				} else {
					snprintf(lbuf, llen, "%s[size]", param);
				}
				if (size_overflow) {
					ZVAL_STRING(&file_size, file_size_buf);
				}
				register_http_post_files_variable_ex(lbuf, &file_size, &PG(http_globals)[TRACK_VARS_FILES], size_overflow);
			}
			efree(param);
		}
	}

fileupload_done:
	if (php_rfc1867_callback != NULL) {
		multipart_event_end event_end;

		event_end.post_bytes_processed = SG(read_post_bytes);
		php_rfc1867_callback(MULTIPART_EVENT_END, &event_end, &event_extra_data);
	}

	if (lbuf) efree(lbuf);
	if (abuf) efree(abuf);
	if (array_index) efree(array_index);
	zend_hash_destroy(&PG(rfc1867_protected_variables));
	zend_llist_destroy(&header);
	if (mbuff->boundary_next) efree(mbuff->boundary_next);
	if (mbuff->boundary) efree(mbuff->boundary);
	if (mbuff->buffer) efree(mbuff->buffer);
	if (mbuff) efree(mbuff);

#undef EMIT_WARNING_OR_ERROR
}
/* }}} */

SAPI_API void php_rfc1867_set_multibyte_callbacks(
					php_rfc1867_encoding_translation_t encoding_translation,
					php_rfc1867_get_detect_order_t get_detect_order,
					php_rfc1867_set_input_encoding_t set_input_encoding,
					php_rfc1867_getword_t getword,
					php_rfc1867_getword_conf_t getword_conf,
					php_rfc1867_basename_t basename) /* {{{ */
{
	php_rfc1867_encoding_translation = encoding_translation;
	php_rfc1867_get_detect_order = get_detect_order;
	php_rfc1867_set_input_encoding = set_input_encoding;
	php_rfc1867_getword = getword;
	php_rfc1867_getword_conf = getword_conf;
	php_rfc1867_basename = basename;
}
/* }}} */
