/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "php_open_temporary_file.h"
#include "zend_globals.h"
#include "php_globals.h"
#include "php_variables.h"
#include "rfc1867.h"
#include "ext/standard/type.h"


#define NEW_BOUNDARY_CHECK 1
#define SAFE_RETURN { if (namebuf) efree(namebuf); if (filenamebuf) efree(filenamebuf); if (lbuf) efree(lbuf); if (abuf) efree(abuf); if(arr_index) efree(arr_index); zend_hash_destroy(&PG(rfc1867_protected_variables)); return; }

/* The longest property name we use in an uploaded file array */
#define MAX_SIZE_OF_INDEX sizeof("[tmp_name]")

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


static void safe_php_register_variable_ex(char *var, zval *val, pval *track_vars_array, zend_bool override_protection TSRMLS_DC)
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
 * Split raw mime stream up into appropriate components
 */
static void php_mime_split(char *buf, int cnt, char *boundary, int len, zval *array_ptr TSRMLS_DC)
{
	char *ptr, *loc, *loc2, *loc3, *s, *name, *filename, *u, *temp_filename, c;
	int state = 0, Done = 0, rem, urem;
	int eolsize;
	long bytes, max_file_size = 0;
	char *namebuf=NULL, *filenamebuf=NULL, *lbuf=NULL, 
		 *abuf=NULL, *start_arr=NULL, *end_arr=NULL, *arr_index=NULL;
	FILE *fp;
	int itype, is_arr_upload=0, arr_len=0;
	zval *http_post_files=NULL;
	zend_bool upload_successful;
	zend_bool magic_quotes_gpc;

	zend_hash_init(&PG(rfc1867_protected_variables), 5, NULL, NULL, 0);

	ALLOC_HASHTABLE(SG(rfc1867_uploaded_files));
	zend_hash_init(SG(rfc1867_uploaded_files), 5, NULL, (dtor_func_t) free_estring, 0);

	ALLOC_ZVAL(http_post_files);
	array_init(http_post_files);
	INIT_PZVAL(http_post_files);
	PG(http_globals)[TRACK_VARS_FILES] = http_post_files;

	ptr = buf;
	rem = cnt;

	while ((ptr - buf < cnt) && !Done) {
		switch (state) {
			case 0:			/* Looking for mime boundary */
				loc = memchr(ptr, *boundary, rem);  /* fixed */
				if (loc) {
					if (!strncmp(loc, boundary, len)) {

						state = 1;

						eolsize = 2;
						if(*(loc+len)=='\n') {
							eolsize = 1;
						}

						rem -= (loc - ptr) + len + eolsize;
						ptr = loc + len + eolsize;
					} else {
						rem -= (loc - ptr) + 1;
						ptr = loc + 1;
					}
				} else {
					Done = 1;
				}
				break;
			case 1:			/* Check content-disposition */
				while (strncasecmp(ptr, "Content-Disposition: form-data;", 31)) {
					if (rem < 31) {
						SAFE_RETURN;
					}
					if (ptr[1] == '\n') {
						/* empty line as end of header found */
						php_error(E_WARNING, "File Upload Mime headers garbled ptr: [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
						SAFE_RETURN;
					}
					/* some other headerfield found, skip it */
					loc = (char *) memchr(ptr, '\n', rem)+1;
					if (!loc) {
						/* broken */
						php_error(E_WARNING, "File Upload Mime headers garbled ptr: [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
						SAFE_RETURN;
					}
					while (*loc == ' ' || *loc == '\t') {
						/* other field is folded, skip it */
						loc = (char *) memchr(loc, '\n', rem-(loc-ptr))+1;
						if (!loc) {
							/* broken */
							php_error(E_WARNING, "File Upload Mime headers garbled ptr: [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
							SAFE_RETURN;
						}
					}
					rem -= (loc - ptr);
					ptr = loc;
				}
				loc = memchr(ptr, '\n', rem);
				if (!loc) {
					/* broken */
					php_error(E_WARNING, "File Upload Mime headers garbled ptr: [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
					SAFE_RETURN;
				}
				while (loc[1] == ' ' || loc[1] == '\t') {
					/* field is folded, look for end */
					loc = memchr(loc+1, '\n', rem-(loc-ptr)-1);
					if (!loc) {
						/* broken */
						php_error(E_WARNING, "File Upload Mime headers garbled ptr: [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
						SAFE_RETURN;
					}
				}
				name = strstr(ptr, " name=");
				if (name && name < loc) {
					name += 6;
					if ( *name == '\"' ) { 
						name++;
						s = memchr(name, '\"', loc - name);
					} else {
						s = strpbrk(name, " \t()<>@,;:\\\"/[]?=\r\n");
					}
					if (!s) {
						php_error(E_WARNING, "File Upload Mime headers garbled name: [%c%c%c%c%c]", *name, *(name + 1), *(name + 2), *(name + 3), *(name + 4));
						SAFE_RETURN;
					}
					
					if (namebuf) {
						efree(namebuf);
					}
					namebuf = estrndup(name, s-name);
					if (lbuf) {
						efree(lbuf);
					}
					lbuf = emalloc(s-name + MAX_SIZE_OF_INDEX + 1);
					state = 2;
					
					/* the fix at this position was wrong
					 * the end of headers search was broken
					 * below. fix moved there and restored
					 * pre 4.0.6 code here
					 */
					loc2 = memchr(loc + 1, '\n', rem);
					rem -= (loc2 - ptr) + 1;
					ptr = loc2 + 1;
					/* is_arr_upload is true when name of file upload field
					 * ends in [.*]
					 * start_arr is set to point to 1st [
					 * end_arr points to last ]
					 */
					is_arr_upload = (start_arr = strchr(namebuf,'[')) && 
									(end_arr = strrchr(namebuf,']')) && 
									(end_arr == namebuf+strlen(namebuf)-1);
					if(is_arr_upload) {
						arr_len = strlen(start_arr); /* is NOW >=2 */
						if(arr_index) efree(arr_index);
						arr_index = estrndup(start_arr+1, arr_len-2);	
					}
				} else {
					php_error(E_WARNING, "File upload error - no name component in content disposition");
					SAFE_RETURN;
				}
				filename = strstr(s, "filename=\"");
				if (filename && filename < loc) {
					filename += 10;
					s = memchr(filename, '\"', loc - filename);
					if (!s) {
						php_error(E_WARNING, "File Upload Mime headers garbled filename: [%c%c%c%c%c]", *filename, *(filename + 1), *(filename + 2), *(filename + 3), *(filename + 4));
						SAFE_RETURN;
					}
					if (filenamebuf) {
						efree(filenamebuf);
					}
					filenamebuf = estrndup(filename, s-filename);

					/* Add $foo_name */
					if (is_arr_upload) {
						if (abuf) {
							efree(abuf);
						}
						abuf = estrndup(namebuf, strlen(namebuf)-arr_len);
						sprintf(lbuf, "%s_name[%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s_name", namebuf);
					}
					s = strrchr(filenamebuf, '\\');
					if (s && s > filenamebuf) {
						safe_php_register_variable(lbuf, s+1, NULL, 0 TSRMLS_CC);
					} else {
						safe_php_register_variable(lbuf, filenamebuf, NULL, 0 TSRMLS_CC);
					}

					/* Add $foo[name] */
					if (is_arr_upload) {
						sprintf(lbuf, "%s[name][%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s[name]", namebuf);
					}
					if (s && s > filenamebuf) {
						register_http_post_files_variable(lbuf, s+1, http_post_files, 0 TSRMLS_CC);
					} else {
						register_http_post_files_variable(lbuf, filenamebuf, http_post_files, 0 TSRMLS_CC);
					}

					state = 3;
					s = "";
					if ((loc2 - loc) > 2) {
						if (!strncasecmp(loc + 1, "Content-Type:", 13)) {
							c = *(loc2 - 1);
							*(loc2 - 1) = '\0';
							s = loc+15;
						}
						/* end of header fix fixed and moved here
						 * find the double newline that marks the
						 * end of the headers
						 */
						loc3 = loc2;
						while (loc3[2] != '\n') {
							
							/* empty line as end of headers not yet found */
							loc3 = memchr(loc3 + 1, '\n', rem-(loc3-ptr)-1);
							if (loc3==NULL) {
								php_error(E_WARNING, "File Upload Mime headers garbled header3: [%c%c%c%c%c]", *loc2, *(loc2 + 1), *(loc2 + 2), *(loc2 + 3), *(loc2 + 4));
								SAFE_RETURN;
							}
						}
						rem -= (loc3 - ptr) + 3;
						ptr = loc3 + 3;
					}

					/* Add $foo_type */
					if (is_arr_upload) {
						sprintf(lbuf, "%s_type[%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s_type", namebuf);
					}
					safe_php_register_variable(lbuf, s, NULL, 0 TSRMLS_CC);
					
					/* Add $foo[type] */
					if (is_arr_upload) {
						sprintf(lbuf, "%s[type][%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s[type]", namebuf);
					}
					register_http_post_files_variable(lbuf, s, http_post_files, 0 TSRMLS_CC);
					if(*s != '\0') {
						/* write old char back
						 * most probably it is '\r'
						 * and not '\n'
						 */
						*(loc2 - 1) = c;  
					}
				}
				break;

			case 2:			/* handle form-data fields */
				loc = memchr(ptr, *boundary, rem);
				u = ptr;
				while (loc) {
					if (!strncmp(loc, boundary, len))
						break;
					u = loc + 1;
					urem = rem - (loc - ptr) - 1;
					loc = memchr(u, *boundary, urem);
				}
				if (!loc) {
					php_error(E_WARNING, "File Upload Field Data garbled");
					SAFE_RETURN;
				}
				*(loc - 4) = '\0';

				/* Check to make sure we are not overwriting special file
				 * upload variables */
				safe_php_register_variable(namebuf, ptr, array_ptr, 0 TSRMLS_CC);

				/* And a little kludge to pick out special MAX_FILE_SIZE */
				itype = php_check_ident_type(namebuf);
				if (itype) {
					u = strchr(namebuf, '[');
					if (u)
						*u = '\0';
				}
				if (!strcmp(namebuf, "MAX_FILE_SIZE")) {
					max_file_size = atol(ptr);
				}
				if (itype) {
					if (u)
						*u = '[';
				}
				rem	-= (loc - ptr);
				ptr = loc;
				state = 0;
				break;

			case 3:			/* Handle file */
				loc = memchr(ptr, *boundary, rem);
				u = ptr;
				while (loc) {
					if (!strncmp(loc, boundary, len)
#if NEW_BOUNDARY_CHECK
						&& (loc-2>buf && *(loc-2)=='-' && *(loc-1)=='-') /* ensure boundary is prefixed with -- */
						&& (loc-2==buf || *(loc-3)=='\n') /* ensure beginning of line */
#endif
						) {
						break;
					}
					u = loc + 1;
					urem = rem - (loc - ptr) - 1;
					loc = memchr(u, *boundary, urem);
				}
				if (!loc) {
					php_error(E_WARNING, "File Upload Error - No Mime boundary found after start of file header");
					SAFE_RETURN;
				}
				bytes = 0;

				fp = php_open_temporary_file(PG(upload_tmp_dir), "php", &temp_filename TSRMLS_CC);
				if (!fp) {
					php_error(E_WARNING, "File upload error - unable to create a temporary file");
					SAFE_RETURN;
				}
				if ((loc - ptr - 4) > PG(upload_max_filesize)) {
					php_error(E_WARNING, "Max file size of %ld bytes exceeded - file [%s] not saved", PG(upload_max_filesize), namebuf);
					upload_successful = 0;
				} else if (max_file_size && ((loc - ptr - 4) > max_file_size)) {
					php_error(E_WARNING, "Max file size exceeded - file [%s] not saved", namebuf);
					upload_successful = 0;
				} else if ((loc - ptr - 4) <= 0) {
					upload_successful = 0;
				} else {
					bytes = fwrite(ptr, 1, loc - ptr - 4, fp);
					if (bytes < (loc - ptr - 4)) {
						php_error(E_WARNING, "Only %d bytes were written, expected to write %ld", bytes, loc - ptr - 4);
					}
					upload_successful = 1;
				}
				fclose(fp);
				add_protected_variable(namebuf TSRMLS_CC);
				if (!upload_successful) {
					if(temp_filename) {
						unlink(temp_filename);
						efree(temp_filename);
					}
					temp_filename = "none";
				} else {
					zend_hash_add(SG(rfc1867_uploaded_files), temp_filename, strlen(temp_filename)+1, &temp_filename, sizeof(char *), NULL);
				}

				magic_quotes_gpc = PG(magic_quotes_gpc);
				PG(magic_quotes_gpc) = 0;
				safe_php_register_variable(namebuf, temp_filename, NULL, 1 TSRMLS_CC);
				/* Add $foo[tmp_name] */
				if(is_arr_upload) {
					sprintf(lbuf, "%s[tmp_name][%s]", abuf, arr_index);
				} else {
					sprintf(lbuf, "%s[tmp_name]", namebuf);
				}
				add_protected_variable(lbuf TSRMLS_CC);
				register_http_post_files_variable(lbuf, temp_filename, http_post_files, 1 TSRMLS_CC);
				PG(magic_quotes_gpc) = magic_quotes_gpc;

				{
					zval file_size;

					file_size.value.lval = bytes;
					file_size.type = IS_LONG;

					/* Add $foo_size */
					if(is_arr_upload) {
						sprintf(lbuf, "%s_size[%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s_size", namebuf);
					}
					safe_php_register_variable_ex(lbuf, &file_size, NULL, 0 TSRMLS_CC);

					/* Add $foo[size] */
					if(is_arr_upload) {
						sprintf(lbuf, "%s[size][%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s[size]", namebuf);
					}
					register_http_post_files_variable_ex(lbuf, &file_size, http_post_files, 0 TSRMLS_CC);
				}
				state = 0;
				rem -= (loc - ptr);
				ptr = loc;
				break;
		}
	}
	SAFE_RETURN;
}


/*
 * Reads post data chunk
 *
 */
static int read_post_data_chunk(char *buf TSRMLS_DC)
{
	int read_bytes;
	
	read_bytes = sapi_module.read_post(buf, SAPI_POST_BLOCK_SIZE TSRMLS_CC);
		
	SG(read_post_bytes) += read_bytes;
		
	return read_bytes;
}


SAPI_API SAPI_POST_HANDLER_FUNC(rfc1867_post_handler)
{
	char *boundary;
	uint boundary_len;
	zval *array_ptr = (zval *) arg;

	if (!PG(file_uploads)) {
		php_error(E_WARNING, "File uploads are disabled");
		return;
	}

	if (SG(request_info).content_length > SG(post_max_size)) {
		sapi_module.sapi_error(E_COMPILE_ERROR, "POST Content-Length of %d bytes exceeds the limit of %d bytes", SG(request_info).content_length, SG(post_max_size));
		return;
	}

	boundary = strstr(content_type_dup, "boundary");
	if (!boundary || !(boundary=strchr(boundary, '='))) {
		sapi_module.sapi_error(E_COMPILE_ERROR, "Missing boundary in multipart/form-data POST data");
		return;
	}
	boundary++;
	boundary_len = strlen(boundary);

	if (boundary[0] == '"' && boundary[boundary_len-1] == '"') {
		boundary++;
		boundary_len -= 2;
		boundary[boundary_len] = '\0';
	}

	/* <FIXME> Temporary. Should be done same time as parsing. Maybe that Apache stuff.. */
	{
		int allocated_bytes=SAPI_POST_BLOCK_SIZE+1, read_bytes;
		
		SG(request_info).post_data = emalloc(allocated_bytes);

		for (;;) {
			read_bytes = read_post_data_chunk(SG(request_info).post_data+SG(read_post_bytes) TSRMLS_CC);

			if(read_bytes <= 0 || read_bytes < SAPI_POST_BLOCK_SIZE) {
				break; 
			}

			if (SG(read_post_bytes) > SG(post_max_size)) {
				php_error(E_WARNING, "Actual POST length does not match Content-Length, and exceeds %d bytes", SG(post_max_size));
				return;
			}

			if (SG(read_post_bytes) + SAPI_POST_BLOCK_SIZE >= allocated_bytes) {
				allocated_bytes = SG(read_post_bytes)+SAPI_POST_BLOCK_SIZE+1;
				SG(request_info).post_data = erealloc(SG(request_info).post_data, allocated_bytes);
			}
		}

		SG(request_info).post_data[SG(read_post_bytes)] = 0;  /* terminating NULL */
		SG(request_info).post_data_length = SG(read_post_bytes);
	}
	/* </FIXME> */

	if (SG(request_info).post_data) {
		php_mime_split(SG(request_info).post_data, SG(request_info).post_data_length, boundary, boundary_len, array_ptr TSRMLS_CC);
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
