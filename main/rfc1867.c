/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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
#include "ext/standard/php_standard.h"
#include "ext/standard/file.h" /* for php_file_le_uploads() */
#include "zend_globals.h"
#include "php_globals.h"
#include "php_variables.h"
#include "rfc1867.h"


#define NEW_BOUNDARY_CHECK 1
#define SAFE_RETURN { if (namebuf) efree(namebuf); if (filenamebuf) efree(filenamebuf); if (lbuf) efree(lbuf); if (abuf) efree(abuf); if(arr_index) efree(arr_index); zend_hash_destroy(&PG(rfc1867_protected_variables)); return; }

/* The longest property name we use in an uploaded file array */
#define MAX_SIZE_OF_INDEX sizeof("[tmp_name]")

static void add_protected_variable(char *varname PLS_DC)
{
	int dummy=1;

	zend_hash_add(&PG(rfc1867_protected_variables), varname, strlen(varname)+1, &dummy, sizeof(int), NULL);
}


static zend_bool is_protected_variable(char *varname PLS_DC)
{
	return zend_hash_exists(&PG(rfc1867_protected_variables), varname, strlen(varname)+1);
}


static void safe_php_register_variable(char *var, char *strval, zval *track_vars_array, zend_bool override_protection ELS_DC PLS_DC)
{
	if (override_protection || !is_protected_variable(var PLS_CC)) {
		php_register_variable(var, strval, track_vars_array ELS_CC PLS_CC);
	}
}


static void safe_php_register_variable_ex(char *var, zval *val, pval *track_vars_array, zend_bool override_protection ELS_DC PLS_DC)
{
	if (override_protection || !is_protected_variable(var PLS_CC)) {
		php_register_variable_ex(var, val, track_vars_array ELS_CC PLS_CC);
	}
}


static void register_http_post_files_variable(char *strvar, char *val, zval *http_post_files, zend_bool override_protection ELS_DC PLS_DC)
{
	int register_globals = PG(register_globals);

	PG(register_globals) = 0;
	safe_php_register_variable(strvar, val, http_post_files, override_protection ELS_CC PLS_CC);
	PG(register_globals) = register_globals;
}


static void register_http_post_files_variable_ex(char *var, zval *val, zval *http_post_files, zend_bool override_protection ELS_DC PLS_DC)
{
	int register_globals = PG(register_globals);

	PG(register_globals) = 0;
	safe_php_register_variable_ex(var, val, http_post_files, override_protection ELS_CC PLS_CC);
	PG(register_globals) = register_globals;
}


/*
 * Split raw mime stream up into appropriate components
 */
static void php_mime_split(char *buf, int cnt, char *boundary, zval *array_ptr SLS_DC)
{
	char *ptr, *loc, *loc2, *loc3, *s, *name, *filename, *u, *fn;
	int len, state = 0, Done = 0, rem, urem;
	int eolsize;
	long bytes, max_file_size = 0;
	char *namebuf=NULL, *filenamebuf=NULL, *lbuf=NULL, 
		 *abuf=NULL, *start_arr=NULL, *end_arr=NULL, *arr_index=NULL;
	FILE *fp;
	int itype, is_arr_upload=0, arr_len=0;
	zval *http_post_files=NULL;
	ELS_FETCH();
	PLS_FETCH();

	zend_hash_init(&PG(rfc1867_protected_variables), 5, NULL, NULL, 0);

	ALLOC_HASHTABLE(SG(rfc1867_uploaded_files));
	zend_hash_init(SG(rfc1867_uploaded_files), 5, NULL, NULL, 0);

	ALLOC_ZVAL(http_post_files);
	array_init(http_post_files);
	INIT_PZVAL(http_post_files);
	PG(http_globals)[TRACK_VARS_FILES] = http_post_files;

	ptr = buf;
	rem = cnt;
	len = strlen(boundary);
	while ((ptr - buf < cnt) && !Done) {
		switch (state) {
			case 0:			/* Looking for mime boundary */
				loc = memchr(ptr, *boundary, cnt);
				if (loc) {
					if (!strncmp(loc, boundary, len)) {

						state = 1;

						eolsize = 2;
						if(*(loc+len)==0x0a) {
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
				if (strncasecmp(ptr, "Content-Disposition: form-data;", 31)) {
					if (rem < 31) {
						SAFE_RETURN;
					}
					php_error(E_WARNING, "File Upload Mime headers garbled ptr: [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
					SAFE_RETURN;
				}
				loc = memchr(ptr, '\n', rem);
				name = strstr(ptr, " name=\"");
				if (name && name < loc) {
					name += 7;
					s = memchr(name, '\"', loc - name);
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
					lbuf = emalloc(s-name + MAX_SIZE_OF_INDEX);
					state = 2;
					loc2 = memchr(loc + 1, '\n', rem);
					rem -= (loc2 - ptr) + 1;
					ptr = loc2 + 1;
					/* is_arr_upload is true when name of file upload field
					 * ends in [.*]
					 * start_arr is set to point to 1st [
					 * end_arr points to last ]
					 */
					is_arr_upload = (start_arr = strrchr(namebuf,'[')) && 
									(end_arr = strrchr(namebuf,']')) && 
									(end_arr = namebuf+strlen(namebuf)-1);
					if(is_arr_upload) {
						arr_len = strlen(start_arr);
						if(arr_index) efree(arr_index);
						arr_index = estrndup(start_arr+1,arr_len-1);	
					}
				} else {
					php_error(E_WARNING, "File upload error - no name component in content disposition");
					SAFE_RETURN;
				}
				filename = strstr(s, " filename=\"");
				if (filename && filename < loc) {
					filename += 11;
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
						safe_php_register_variable(lbuf, s+1, NULL, 0 ELS_CC PLS_CC);
					} else {
						safe_php_register_variable(lbuf, filenamebuf, NULL, 0 ELS_CC PLS_CC);
					}

					/* Add $foo[name] */
                    if (is_arr_upload) {
                        sprintf(lbuf, "%s[name][%s]", abuf, arr_index);
                    } else {
                        sprintf(lbuf, "%s[name]", namebuf);
                    }
					if (s && s > filenamebuf) {
						register_http_post_files_variable(lbuf, s+1, http_post_files, 0 ELS_CC PLS_CC);
					} else {
						register_http_post_files_variable(lbuf, filenamebuf, http_post_files, 0 ELS_CC PLS_CC);
					}

					state = 3;
					s = "";
					if ((loc2 - loc) > 2) {
						if (!strncasecmp(loc + 1, "Content-Type:", 13)) {
							*(loc2 - 1) = '\0';
							s = loc+15;
						}
						loc3=memchr(loc2+1, '\n', rem-1);
						if (loc3==NULL) {
						    php_error(E_WARNING, "File Upload Mime headers garbled header3: [%c%c%c%c%c]", *loc2, *(loc2 + 1), *(loc2 + 2), *(loc2 + 3), *(loc2 + 4));
						    SAFE_RETURN;
						}
						if (loc3 - loc2 > 2) { /* we have a third header */
						    rem -= (ptr-loc3)+3;
						    ptr = loc3+3;
						} else {
							rem -= (ptr-loc3)+1;
							ptr = loc3+1;
						}
					}

					/* Add $foo_type */
					if (is_arr_upload) {
						sprintf(lbuf, "%s_type[%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s_type", namebuf);
					}
					safe_php_register_variable(lbuf, s, NULL, 0 ELS_CC PLS_CC);
					
					/* Add $foo[type] */
					if (is_arr_upload) {
						sprintf(lbuf, "%s[type][%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s[type]", namebuf);
					}
					register_http_post_files_variable(lbuf, s, http_post_files, 0 ELS_CC PLS_CC);
					if(*s != '\0') {
						*(loc2 - 1) = '\n';
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
				safe_php_register_variable(namebuf, ptr, array_ptr, 0 ELS_CC PLS_CC);

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
				fn = tempnam(PG(upload_tmp_dir), "php");
				if ((loc - ptr - 4) > PG(upload_max_filesize)) {
					php_error(E_WARNING, "Max file size of %ld bytes exceeded - file [%s] not saved", PG(upload_max_filesize),namebuf);
					fn = "none";
				} else if (max_file_size && ((loc - ptr - 4) > max_file_size)) {
					php_error(E_WARNING, "Max file size exceeded - file [%s] not saved", namebuf);
					fn = "none";
				} else if ((loc - ptr - 4) <= 0) {
					fn = "none";
				} else {
					fp = V_FOPEN(fn, "wb");
					if (!fp) {
						php_error(E_WARNING, "File Upload Error - Unable to open temporary file [%s]", fn);
						SAFE_RETURN;
					}
					bytes = fwrite(ptr, 1, loc - ptr - 4, fp);
					fclose(fp);
					zend_list_insert(fn,php_file_le_uploads());  /* Tell PHP about the file so the destructor can unlink it later */
					if (bytes < (loc - ptr - 4)) {
						php_error(E_WARNING, "Only %d bytes were written, expected to write %ld", bytes, loc - ptr - 4);
					}
				}
				add_protected_variable(namebuf PLS_CC);
				safe_php_register_variable(namebuf, fn, NULL, 1 ELS_CC PLS_CC);
				{
					int dummy=1;

					zend_hash_add(SG(rfc1867_uploaded_files), namebuf, strlen(namebuf)+1, &dummy, sizeof(int), NULL);
				}

				/* Add $foo[tmp_name] */
				if(is_arr_upload) {
					sprintf(lbuf, "%s[tmp_name][%s]", abuf, arr_index);
				} else {
					sprintf(lbuf, "%s[tmp_name]", namebuf);
				}
				add_protected_variable(lbuf PLS_CC);
				register_http_post_files_variable(lbuf, fn, http_post_files, 1 ELS_CC PLS_CC);
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
					safe_php_register_variable_ex(lbuf, &file_size, NULL, 0 ELS_CC PLS_CC);

					/* Add $foo[size] */
					if(is_arr_upload) {
						sprintf(lbuf, "%s[size][%s]", abuf, arr_index);
					} else {
						sprintf(lbuf, "%s[size]", namebuf);
					}
					register_http_post_files_variable_ex(lbuf, &file_size, http_post_files, 0 ELS_CC PLS_CC);
				}
				state = 0;
				rem -= (loc - ptr);
				ptr = loc;
				break;
		}
	}
	SAFE_RETURN;
}


SAPI_POST_HANDLER_FUNC(rfc1867_post_handler)
{
	char *boundary;
	uint boundary_len;
	zval *array_ptr = (zval *) arg;

	boundary = strstr(content_type_dup, "boundary");
	if (!boundary || !(boundary=strchr(boundary, '='))) {
		sapi_module.sapi_error(E_COMPILE_ERROR, "Missing boundary in multipart/form-data POST data");
		return;
	}
	boundary++;
	boundary_len = strlen(boundary);

	if (SG(request_info).post_data) {
		php_mime_split(SG(request_info).post_data, SG(request_info).post_data_length, boundary, array_ptr SLS_CC);
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
