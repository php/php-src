/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jani Taskinen <sniper@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

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

#if HAVE_MBSTRING && !defined(COMPILE_DL_MBSTRING)
#include "ext/mbstring/mbstring.h"
#endif

#undef DEBUG_FILE_UPLOAD

#define SAFE_RETURN { \
	if (lbuf) efree(lbuf); \
	if (abuf) efree(abuf); \
	if (array_index) efree(array_index); \
	zend_hash_destroy(&PG(rfc1867_protected_variables)); \
	zend_llist_destroy(&header); \
	if (mbuff->boundary_next) efree(mbuff->boundary_next); \
	if (mbuff->boundary) efree(mbuff->boundary); \
	if (mbuff->buffer) efree(mbuff->buffer); \
	if (mbuff) efree(mbuff); \
	return; }


/* The longest property name we use in an uploaded file array */
#define MAX_SIZE_OF_INDEX sizeof("[tmp_name]")

/* Errors */
#define UPLOAD_ERROR_OK   0  /* File upload succesful */
#define UPLOAD_ERROR_A    1  /* Uploaded file exceeded upload_max_filesize */
#define UPLOAD_ERROR_B    2  /* Uploaded file exceeded MAX_FILE_SIZE */
#define UPLOAD_ERROR_C    3  /* Partially uploaded */
#define UPLOAD_ERROR_D    4  /* No file uploaded */

void php_rfc1867_register_constants(TSRMLS_D)
{
	REGISTER_MAIN_LONG_CONSTANT("UPLOAD_ERR_OK",         UPLOAD_ERROR_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("UPLOAD_ERR_INI_SIZE",   UPLOAD_ERROR_A,  CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("UPLOAD_ERR_FORM_SIZE",  UPLOAD_ERROR_B,  CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("UPLOAD_ERR_PARTIAL",    UPLOAD_ERROR_C,  CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("UPLOAD_ERR_NO_FILE",    UPLOAD_ERROR_D,  CONST_CS | CONST_PERSISTENT);
}

static void add_protected_variable(char *varname TSRMLS_DC)
{
	int dummy=1;

	zend_hash_add(&PG(rfc1867_protected_variables), varname, strlen(varname)+1, &dummy, sizeof(int), NULL);
}


static zend_bool is_protected_variable(char *varname TSRMLS_DC)
{
	return zend_hash_exists(&PG(rfc1867_protected_variables), varname, strlen(varname)+1);
}


static void safe_php_register_variable(char *var, char *strval, zval *track_vars_array, zend_bool override_protection TSRMLS_DC)
{
	if (override_protection || !is_protected_variable(var TSRMLS_CC)) {
		php_register_variable(var, strval, track_vars_array TSRMLS_CC);
	}
}


static void safe_php_register_variable_ex(char *var, zval *val, zval *track_vars_array, zend_bool override_protection TSRMLS_DC)
{
	if (override_protection || !is_protected_variable(var TSRMLS_CC)) {
		php_register_variable_ex(var, val, track_vars_array TSRMLS_CC);
	}
}


static void register_http_post_files_variable(char *strvar, char *val, zval *http_post_files, zend_bool override_protection TSRMLS_DC)
{
	int register_globals = PG(register_globals);

	PG(register_globals) = 0;
	safe_php_register_variable(strvar, val, http_post_files, override_protection TSRMLS_CC);
	PG(register_globals) = register_globals;
}


static void register_http_post_files_variable_ex(char *var, zval *val, zval *http_post_files, zend_bool override_protection TSRMLS_DC)
{
	int register_globals = PG(register_globals);

	PG(register_globals) = 0;
	safe_php_register_variable_ex(var, val, http_post_files, override_protection TSRMLS_CC);
	PG(register_globals) = register_globals;
}


static int unlink_filename(char **filename TSRMLS_DC)
{
	VCWD_UNLINK(*filename);
	return 0;
}


void destroy_uploaded_files_hash(TSRMLS_D)
{
	zend_hash_apply(SG(rfc1867_uploaded_files), (apply_func_t) unlink_filename TSRMLS_CC);
	zend_hash_destroy(SG(rfc1867_uploaded_files));
	FREE_HASHTABLE(SG(rfc1867_uploaded_files));
}


/*
 *  Following code is based on apache_multipart_buffer.c from libapreq-0.33 package.
 *
 */

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

} multipart_buffer;


typedef struct {
	char *key;
	char *value;
} mime_header_entry;


/*
  fill up the buffer with client data.
  returns number of bytes added to buffer.
*/
static int fill_buffer(multipart_buffer *self TSRMLS_DC)
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

		actual_read = sapi_module.read_post(buf, bytes_to_read TSRMLS_CC);

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
static int multipart_buffer_eof(multipart_buffer *self TSRMLS_DC)
{
	if ( (self->bytes_in_buffer == 0 && fill_buffer(self TSRMLS_CC) < 1) ) {
		return 1;
	} else {
		return 0;
	}
}


/* create new multipart_buffer structure */
static multipart_buffer *multipart_buffer_new(char *boundary, int boundary_len)
{
	multipart_buffer *self = (multipart_buffer *) ecalloc(1, sizeof(multipart_buffer));

	int minsize = boundary_len + 6;
	if (minsize < FILLUNIT) minsize = FILLUNIT;

	self->buffer = (char *) ecalloc(1, minsize + 1);
	self->bufsize = minsize;

	self->boundary = (char *) ecalloc(1, boundary_len + 3); 
	sprintf(self->boundary, "--%s", boundary);
	
	self->boundary_next = (char *) ecalloc(1, boundary_len + 4);
	sprintf(self->boundary_next, "\n--%s", boundary);
	self->boundary_next_len = boundary_len + 3;

	self->buf_begin = self->buffer;
	self->bytes_in_buffer = 0;

	return self;
}


/*
  gets the next CRLF terminated line from the input buffer.
  if it doesn't find a CRLF, and the buffer isn't completely full, returns
  NULL; otherwise, returns the beginning of the null-terminated line,
  minus the CRLF.

  note that we really just look for LF terminated lines. this works
  around a bug in internet explorer for the macintosh which sends mime
  boundaries that are only LF terminated when you use an image submit
  button in a multipart/form-data form.
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


/* returns the next CRLF terminated line from the client */
static char *get_line(multipart_buffer *self TSRMLS_DC)
{
	char* ptr = next_line(self);

	if (!ptr) {
		fill_buffer(self TSRMLS_CC);
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
static int find_boundary(multipart_buffer *self, char *boundary TSRMLS_DC)
{
	char *line;

	/* loop thru lines */
	while( (line = get_line(self TSRMLS_CC)) )
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
static int multipart_buffer_headers(multipart_buffer *self, zend_llist *header TSRMLS_DC)
{
	char *line;
	mime_header_entry prev_entry, entry;
	int prev_len, cur_len;
	
	/* didn't find boundary, abort */
	if (!find_boundary(self, self->boundary TSRMLS_CC)) {
		return 0;
	}

	/* get lines of text, or CRLF_CRLF */

	while( (line = get_line(self TSRMLS_CC)) && strlen(line) > 0 )
	{
		/* add header to table */
		
		char *key = line;
		char *value = NULL;
		
		/* space in the beginning means same header */
		if (!isspace(line[0])) {
			value = strchr(line, ':');
		}

		if (value) {
			*value = 0;
			do { value++; } while(isspace(*value));

			entry.value = estrdup(value);
			entry.key = estrdup(key);

		} else if (zend_llist_count(header)) { /* If no ':' on the line, add to previous line */

			prev_len = strlen(prev_entry.value);
			cur_len = strlen(line);

			entry.value = emalloc(prev_len + cur_len + 1);
			memcpy(entry.value, prev_entry.value, prev_len);
			memcpy(entry.value + prev_len, line, cur_len);
			entry.value[cur_len + prev_len] = '\0';

			entry.key = estrdup(prev_entry.key);
			
			zend_llist_remove_tail(header);
		} else {
			continue;
		}

		zend_llist_add_element(header, &entry);
		prev_entry = entry;
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


static char *php_ap_getword(char **line, char stop)
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


static char *substring_conf(char *start, int len, char quote TSRMLS_DC)
{
	char *result = emalloc(len + 2);
	char *resp = result;
	int i;

	for (i = 0; i < len; ++i) {
		if (start[i] == '\\' && (start[i + 1] == '\\' || (quote && start[i + 1] == quote))) {
			*resp++ = start[++i];
		} else {
			*resp++ = start[i];
#if HAVE_MBSTRING && !defined(COMPILE_DL_MBSTRING)
			if (mbstr_encoding_translation(TSRMLS_C) && 
				mbstr_is_mb_leadbyte(start+i TSRMLS_CC)) {
				*resp++ = start[++i];
			}
#endif
		}
	}

	*resp++ = '\0';
	return result;
}


static char *php_ap_getword_conf(char **line TSRMLS_DC)
{
	char *str = *line, *strend, *res, quote;

	while (*str && isspace(*str)) {
		++str;
	}

	if (!*str) {
		*line = str;
		return estrdup("");
	}

	if ((quote = *str) == '"' || quote == '\'') {
		strend = str + 1;
		while (*strend && *strend != quote) {
			if (*strend == '\\' && strend[1] && strend[1] == quote) {
				strend += 2;
			} else {
				++strend;
			}
		}
		res = substring_conf(str + 1, strend - str - 1, quote TSRMLS_CC);

		if (*strend == quote) {
			++strend;
		}

	} else {

		strend = str;
		while (*strend && !isspace(*strend)) {
			++strend;
		}
		res = substring_conf(str, strend - str, 0 TSRMLS_CC);
	}

	while (*strend && isspace(*strend)) {
		++strend;
	}

	*line = strend;
	return res;
}


/*
  search for a string in a fixed-length byte string.
  if partial is true, partial matches are allowed at the end of the buffer.
  returns NULL if not found, or a pointer to the start of the first match.
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
static int multipart_buffer_read(multipart_buffer *self, char *buf, int bytes TSRMLS_DC)
{
	int len, max;
	char *bound;

	/* fill buffer if needed */
	if (bytes > self->bytes_in_buffer) {
		fill_buffer(self TSRMLS_CC);
	}

	/* look for a potential boundary match, only read data up to that point */
	if ((bound = php_ap_memstr(self->buf_begin, self->bytes_in_buffer, self->boundary_next, self->boundary_next_len, 1))) {
		max = bound - self->buf_begin;
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
		self->bytes_in_buffer -= len;
		self->buf_begin += len;
	}

	return len;
}


/*
  XXX: this is horrible memory-usage-wise, but we only expect
  to do this on small pieces of form data.
*/
static char *multipart_buffer_read_body(multipart_buffer *self TSRMLS_DC)
{
	char buf[FILLUNIT], *out=NULL;
	int total_bytes=0, read_bytes=0;

	while((read_bytes = multipart_buffer_read(self, buf, sizeof(buf) TSRMLS_CC))) {
		out = erealloc(out, total_bytes + read_bytes + 1);
		memcpy(out + total_bytes, buf, read_bytes);
		total_bytes += read_bytes;
	}

	if (out) out[total_bytes] = '\0';

	return out;
}


/*
 * The combined READER/HANDLER
 *
 */

SAPI_API SAPI_POST_HANDLER_FUNC(rfc1867_post_handler)
{
	char *boundary, *s=NULL, *boundary_end = NULL, *start_arr=NULL, *array_index=NULL;
	char *temp_filename=NULL, *lbuf=NULL, *abuf=NULL;
	int boundary_len=0, total_bytes=0, cancel_upload=0, is_arr_upload=0, array_len=0, max_file_size=0;
	zval *http_post_files=NULL;
	zend_bool magic_quotes_gpc;
	multipart_buffer *mbuff;
	zval *array_ptr = (zval *) arg;
	FILE *fp;
	zend_llist header;

	if (SG(request_info).content_length > SG(post_max_size)) {
		sapi_module.sapi_error(E_WARNING, "POST Content-Length of %d bytes exceeds the limit of %d bytes", SG(request_info).content_length, SG(post_max_size));
		return;
	}

	/* Get the boundary */
	boundary = strstr(content_type_dup, "boundary");
	if (!boundary || !(boundary=strchr(boundary, '='))) {
		sapi_module.sapi_error(E_WARNING, "Missing boundary in multipart/form-data POST data");
		return;
	}

	/* search for the end of the boundary */
	boundary_end = strchr(boundary, ',');
	if (boundary_end ) *boundary_end = 0;

	boundary++;
	boundary_len = strlen(boundary);

	if (boundary[0] == '"' && boundary[boundary_len-1] == '"') {
		if (boundary_len < 2) { /* otherwise a single " passes */
			sapi_module.sapi_error(E_WARNING, "Invalid boundary in multipart/form-data POST data");
			return;
		}
		boundary++;
		boundary_len -= 2;
		boundary[boundary_len] = '\0';
	}

	/* Initialize the buffer */
	if (!(mbuff = multipart_buffer_new(boundary, boundary_len))) {
		sapi_module.sapi_error(E_WARNING, "Unable to initialize the input buffer");
		return;
	}

	/* Initialize $_FILES[] */
	zend_hash_init(&PG(rfc1867_protected_variables), 5, NULL, NULL, 0);

	ALLOC_HASHTABLE(SG(rfc1867_uploaded_files));
	zend_hash_init(SG(rfc1867_uploaded_files), 5, NULL, (dtor_func_t) free_estring, 0);

	ALLOC_ZVAL(http_post_files);
	array_init(http_post_files);
	INIT_PZVAL(http_post_files);
	PG(http_globals)[TRACK_VARS_FILES] = http_post_files;

	zend_llist_init(&header, sizeof(mime_header_entry), (llist_dtor_func_t) php_free_hdr_entry, 0);

	while (!multipart_buffer_eof(mbuff TSRMLS_CC))
	{
		char buff[FILLUNIT];
		char *cd=NULL,*param=NULL,*filename=NULL;
		int blen=0, wlen=0;

		zend_llist_clean(&header);

		if (!multipart_buffer_headers(mbuff, &header TSRMLS_CC)) {
			SAFE_RETURN;
		}

		if ((cd = php_mime_get_hdr_value(header, "Content-Disposition"))) {
			char *pair=NULL;
			
			while (isspace(*cd)) {
				++cd;
			}

			while (*cd && (pair = php_ap_getword(&cd, ';')))
			{
				char *key=NULL, *word = pair;

				while (isspace(*cd)) {
					++cd;
				}

				if (strchr(pair, '=')) {
					key = php_ap_getword(&pair, '=');
					
					if (!strcmp(key, "name")) {
						if (param) {
							efree(param);
						}
						param = php_ap_getword_conf(&pair TSRMLS_CC);
					} else if (!strcmp(key, "filename")) {
						if (filename) {
							efree(filename);
						}
						filename = php_ap_getword_conf(&pair TSRMLS_CC);
					}
				}
				if (key) {
					efree(key);
				}
				efree(word);
			}

			/* Normal form variable, safe to read all data into memory */
			if (!filename && param) {

				char *value = multipart_buffer_read_body(mbuff TSRMLS_CC);

				if (!value) {
					value = estrdup("");
				}

				safe_php_register_variable(param, value, array_ptr, 0 TSRMLS_CC);
				if (!strcmp(param, "MAX_FILE_SIZE")) {
					max_file_size = atol(value);
				}

				efree(param);
				efree(value);
				continue;
			}

			/* If file_uploads=off, skip the file part */
			if (!PG(file_uploads)) {
				if (filename) {
					efree(filename);
				}
				if (param) {
					efree(param);
				}
				continue;
			}

			/* Return with an error if the posted data is garbled */
			if (!param) {
				sapi_module.sapi_error(E_WARNING, "File Upload Mime headers garbled");
				if (filename) {
					efree(filename);
				}
				SAFE_RETURN;
			}

			/* Handle file */
			fp = php_open_temporary_file(PG(upload_tmp_dir), "php", &temp_filename TSRMLS_CC);
			if (!fp) {
				efree(param);
				efree(filename);
				sapi_module.sapi_error(E_WARNING, "File upload error - unable to create a temporary file");
				SAFE_RETURN;
			}

			total_bytes = 0;
			cancel_upload = 0;

			if(strlen(filename) == 0) {
#ifdef DEBUG_FILE_UPLOAD
				sapi_module.sapi_error(E_NOTICE, "No file uploaded");
#endif
				cancel_upload = UPLOAD_ERROR_D;
			}

			while (!cancel_upload && (blen = multipart_buffer_read(mbuff, buff, sizeof(buff) TSRMLS_CC)))
			{
				if (total_bytes > PG(upload_max_filesize)) {
					sapi_module.sapi_error(E_WARNING, "upload_max_filesize of %ld bytes exceeded - file [%s=%s] not saved", PG(upload_max_filesize), param, filename);
					cancel_upload = UPLOAD_ERROR_A;
				} else if (max_file_size && (total_bytes > max_file_size)) {
					sapi_module.sapi_error(E_WARNING, "MAX_FILE_SIZE of %ld bytes exceeded - file [%s=%s] not saved", max_file_size, param, filename);
					cancel_upload = UPLOAD_ERROR_B;
				} else if (blen > 0) {
					wlen = fwrite(buff, 1, blen, fp);
			
					if (wlen < blen) {
						sapi_module.sapi_error(E_WARNING, "Only %d bytes were written, expected to write %ld", wlen, blen);
						cancel_upload = UPLOAD_ERROR_C;
					} else {
						total_bytes += wlen;
					}
				} 
			} 
			fclose(fp);

#ifdef DEBUG_FILE_UPLOAD
			if(strlen(filename) > 0 && total_bytes == 0) {
				sapi_module.sapi_error(E_WARNING, "Uploaded file size 0 - file [%s=%s] not saved", param, filename);
				cancel_upload = 5;
			}
#endif		

			if (cancel_upload) {
				if (temp_filename) {
					unlink(temp_filename);
					efree(temp_filename);
				}
				temp_filename="";
			} else {
				zend_hash_add(SG(rfc1867_uploaded_files), temp_filename, strlen(temp_filename) + 1, &temp_filename, sizeof(char *), NULL);
			}

			/* is_arr_upload is true when name of file upload field
			 * ends in [.*]
			 * start_arr is set to point to 1st [
			 */
			is_arr_upload =	(start_arr = strchr(param,'[')) &&
							(param[strlen(param)-1] == ']');

			if (is_arr_upload) {
				array_len = strlen(start_arr);
				if (array_index) {
					efree(array_index);
				}
				array_index = estrndup(start_arr+1, array_len-2);   
			}
			
			/* Add $foo_name */
			if (lbuf) {
				efree(lbuf);
			}
			lbuf = (char *) emalloc(strlen(param) + MAX_SIZE_OF_INDEX + 1);
			
			if (is_arr_upload) {
				if (abuf) efree(abuf);
				abuf = estrndup(param, strlen(param)-array_len);
				sprintf(lbuf, "%s_name[%s]", abuf, array_index);
			} else {
				sprintf(lbuf, "%s_name", param);
			}

#if HAVE_MBSTRING && !defined(COMPILE_DL_MBSTRING)
			if (mbstr_encoding_translation(TSRMLS_C)) {
				s = mbstr_strrchr(filename, '\\' TSRMLS_CC);
			} else {
				s = strrchr(filename, '\\');
			}
#else
			s = strrchr(filename, '\\');
#endif
			if (s && s > filename) {
				safe_php_register_variable(lbuf, s+1, NULL, 0 TSRMLS_CC);
			} else {
				safe_php_register_variable(lbuf, filename, NULL, 0 TSRMLS_CC);
			}

			/* Add $foo[name] */
			if (is_arr_upload) {
				sprintf(lbuf, "%s[name][%s]", abuf, array_index);
			} else {
				sprintf(lbuf, "%s[name]", param);
			}
			if (s && s > filename) {
				register_http_post_files_variable(lbuf, s+1, http_post_files, 0 TSRMLS_CC);
			} else {
				register_http_post_files_variable(lbuf, filename, http_post_files, 0 TSRMLS_CC);
			}
			efree(filename);
			s = NULL;
	
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

			/* Add $foo_type */
			if (is_arr_upload) {
				sprintf(lbuf, "%s_type[%s]", abuf, array_index);
			} else {
				sprintf(lbuf, "%s_type", param);
			}
			safe_php_register_variable(lbuf, cd, NULL, 0 TSRMLS_CC);

			/* Add $foo[type] */
			if (is_arr_upload) {
				sprintf(lbuf, "%s[type][%s]", abuf, array_index);
			} else {
				sprintf(lbuf, "%s[type]", param);
			}
			register_http_post_files_variable(lbuf, cd, http_post_files, 0 TSRMLS_CC);

			/* Restore Content-Type Header */
			if (s != NULL) {
				*s = ';';
			}
			s = "";

			/* Initialize variables */
			add_protected_variable(param TSRMLS_CC);

			magic_quotes_gpc = PG(magic_quotes_gpc);
			PG(magic_quotes_gpc) = 0;
			/* if param is of form xxx[.*] this will cut it to xxx */
			safe_php_register_variable(param, temp_filename, NULL, 1 TSRMLS_CC);
	
			/* Add $foo[tmp_name] */
			if (is_arr_upload) {
				sprintf(lbuf, "%s[tmp_name][%s]", abuf, array_index);
			} else {
				sprintf(lbuf, "%s[tmp_name]", param);
			}
			add_protected_variable(lbuf TSRMLS_CC);
			register_http_post_files_variable(lbuf, temp_filename, http_post_files, 1 TSRMLS_CC);

			PG(magic_quotes_gpc) = magic_quotes_gpc;

			{
				zval file_size, error_type;

				error_type.value.lval = cancel_upload;
				error_type.type = IS_LONG;

				/* Add $foo[error] */
				if (cancel_upload) {
					file_size.value.lval = 0;
					file_size.type = IS_LONG;
				} else {
					file_size.value.lval = total_bytes;
					file_size.type = IS_LONG;
				}	
	
				if (is_arr_upload) {
					sprintf(lbuf, "%s[error][%s]", abuf, array_index);
				} else {
					sprintf(lbuf, "%s[error]", param);
				}
				register_http_post_files_variable_ex(lbuf, &error_type, http_post_files, 0 TSRMLS_CC);

				/* Add $foo_size */
				if (is_arr_upload) {
					sprintf(lbuf, "%s_size[%s]", abuf, array_index);
				} else {
					sprintf(lbuf, "%s_size", param);
				}
				safe_php_register_variable_ex(lbuf, &file_size, NULL, 0 TSRMLS_CC);

				/* Add $foo[size] */
				if (is_arr_upload) {
					sprintf(lbuf, "%s[size][%s]", abuf, array_index);
				} else {
					sprintf(lbuf, "%s[size]", param);
				}
				register_http_post_files_variable_ex(lbuf, &file_size, http_post_files, 0 TSRMLS_CC);
			}
			efree(param);
		}
	}

	SAFE_RETURN;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
