/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rui Hirokawa <hirokawa@php.net>                              |
   |         Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_variables.h"
#include "mbstring.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_mail.h"
#include "ext/standard/url.h"
#include "main/php_output.h"
#include "ext/standard/info.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "TSRM.h"

#include "mb_gpc.h"
/* }}} */

#if HAVE_MBSTRING

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

/* {{{ static sapi_post_entry mbstr_post_entries[] */
static sapi_post_entry mbstr_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data, php_mb_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

/* {{{ static sapi_post_entry php_post_entries[] */
static sapi_post_entry php_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data,	php_std_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

/* {{{ int _php_mb_enable_encoding_translation(int flag) */
int _php_mb_enable_encoding_translation(int flag)
{
	if (flag) {
		sapi_unregister_post_entry(php_post_entries);
		sapi_register_post_entries(mbstr_post_entries);
		sapi_register_treat_data(mbstr_treat_data);	   			   
	} else {
		sapi_unregister_post_entry(mbstr_post_entries);
		sapi_register_post_entries(php_post_entries);
		sapi_register_treat_data(php_default_treat_data);	   			   
	}
	return SUCCESS;
}
/* }}} */ 

/* {{{ MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data)
 * http input processing */
MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data)
{
	char *res = NULL, *separator=NULL;
	const char *c_var;
	pval *array_ptr;
	int free_buffer=0;

	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			ALLOC_ZVAL(array_ptr);
			array_init(array_ptr);
			INIT_PZVAL(array_ptr);
			switch (arg) {
				case PARSE_POST:
					PG(http_globals)[TRACK_VARS_POST] = array_ptr;
					break;
				case PARSE_GET:
					PG(http_globals)[TRACK_VARS_GET] = array_ptr;
					break;
				case PARSE_COOKIE:
					PG(http_globals)[TRACK_VARS_COOKIE] = array_ptr;
					break;
			}
			break;
		default:
			array_ptr=destArray;
			break;
	}

	if (arg==PARSE_POST) { 
		sapi_handle_post(array_ptr TSRMLS_CC);
		return;
	}

	if (arg == PARSE_GET) {		/* GET data */
		c_var = SG(request_info).query_string;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		c_var = SG(request_info).cookie_data;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
		free_buffer = 1;
	}

	if (!res) {
		return;
	}

	switch (arg) {
	case PARSE_POST:
	case PARSE_GET:
	case PARSE_STRING:
		separator = (char *) estrdup(PG(arg_separator).input);
		break;
	case PARSE_COOKIE:
		separator = ";\0";
		break;
	}
	
	switch(arg) {
	case PARSE_POST:
		MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
		break;
	case PARSE_GET:
		MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
		break;
	case PARSE_COOKIE:
		MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
		break;
	case PARSE_STRING:
		MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;
		break;
	}

	_php_mb_encoding_handler_ex(array_ptr, res, separator, 0, 0 TSRMLS_CC);

	if (MBSTRG(http_input_identify) != mbfl_no_encoding_invalid) {
		switch(arg){
		case PARSE_POST:
			MBSTRG(http_input_identify_post) = MBSTRG(http_input_identify);
			break;
		case PARSE_GET:
			MBSTRG(http_input_identify_get) = MBSTRG(http_input_identify);
			break;
		case PARSE_COOKIE:
			MBSTRG(http_input_identify_cookie) = MBSTRG(http_input_identify);
			break;
		case PARSE_STRING:
			MBSTRG(http_input_identify_string) = MBSTRG(http_input_identify);
			break;
		}
	}

	if (arg != PARSE_COOKIE) {
		efree(separator);
	}

	if (free_buffer) {
		efree(res);
	}
}
/* }}} */

/* {{{ int _php_mb_encoding_handler_ex() */
int _php_mb_encoding_handler_ex(zval *arg, char *res, char *separator, int force_register_globals, int report_errors TSRMLS_DC)
{
	char *var, *val, *s1, *s2;
	char *strtok_buf = NULL, **val_list = NULL;
	zval *array_ptr = (zval *) arg;
	int n, num, val_len, *len_list = NULL, *elist, elistsz;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_string string, resvar, resval;
	mbfl_encoding_detector *identd = NULL; 
	mbfl_buffer_converter *convd = NULL;
	int prev_rg_state = 0;
	int retval = 0;

	mbfl_string_init_set(&string, MBSTRG(current_language), MBSTRG(current_internal_encoding));
	mbfl_string_init_set(&resvar, MBSTRG(current_language), MBSTRG(current_internal_encoding));
	mbfl_string_init_set(&resval, MBSTRG(current_language), MBSTRG(current_internal_encoding));

	/* register_globals stuff
	 * XXX: this feature is going to be deprecated? */

	if (force_register_globals) {
		prev_rg_state = PG(register_globals);
		PG(register_globals) = 1;
	}

	if (!res || *res == '\0') {
		goto out;
	}
	
	/* count the variables(separators) contained in the "res".
	 * separator may contain multiple separator chars.
	 * separaror chars are set in php.ini (arg_separator.input)
	 */
	num = 1;
	for (s1=res; *s1 != '\0'; s1++) {
		for (s2=separator; *s2 != '\0'; s2++) {
			if (*s1 == *s2) {
				num++;
			}	
		}
	}
	num *= 2; /* need space for variable name and value */
	
	val_list = (char **)ecalloc(num, sizeof(char *));
	len_list = (int *)ecalloc(num, sizeof(int));

	/* split and decode the query */
	n = 0;
	strtok_buf = NULL;
	var = php_strtok_r(res, separator, &strtok_buf);
	while (var)  {
		val = strchr(var, '=');
		if (val) { /* have a value */
			len_list[n] = php_url_decode(var, val-var);
			val_list[n] = var;
			n++;
			
			*val++ = '\0';
			val_list[n] = val;
			len_list[n] = php_url_decode(val, strlen(val));
		} else {
			len_list[n] = php_url_decode(var, strlen(var));
			val_list[n] = var;
			n++;
			
			val_list[n] = "";
			len_list[n] = 0;
		}
		n++;
		var = php_strtok_r(NULL, separator, &strtok_buf);
	} 
	num = n; /* make sure to process initilized vars only */
	
	/* initialize converter */
	to_encoding = MBSTRG(current_internal_encoding);
	elist = MBSTRG(http_input_list);
	elistsz = MBSTRG(http_input_list_size);
	if (elistsz <= 0) {
		from_encoding = mbfl_no_encoding_pass;
	} else if (elistsz == 1) {
		from_encoding = *elist;
	} else {
		/* auto detect */
		from_encoding = mbfl_no_encoding_invalid;
		identd = mbfl_encoding_detector_new(elist, elistsz TSRMLS_CC);
		if (identd) {
			n = 0;
			while (n < num) {
				string.val = (unsigned char *)val_list[n];
				string.len = len_list[n];
				if (mbfl_encoding_detector_feed(identd, &string TSRMLS_CC)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge(identd TSRMLS_CC);
			mbfl_encoding_detector_delete(identd TSRMLS_CC);
		}
		if (from_encoding == mbfl_no_encoding_invalid) {
			if (report_errors) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to detect encoding");
			}
			from_encoding = mbfl_no_encoding_pass;
		}
	}

	convd = NULL;
	if (from_encoding != mbfl_no_encoding_pass) {
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0 TSRMLS_CC);
		if (convd != NULL) {
			mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode) TSRMLS_CC);
			mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar) TSRMLS_CC);
		} else {
			if (report_errors) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create converter");
			}
			goto out;
		}
	}

	/* convert encoding */
	string.no_encoding = from_encoding;

	n = 0;
	while (n < num) {
		string.val = (unsigned char *)val_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resvar TSRMLS_CC) != NULL) {
			var = (char *)resvar.val;
		} else {
			var = val_list[n];
		}
		n++;
		string.val = val_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resval TSRMLS_CC) != NULL) {
			val = resval.val;
			val_len = resval.len;
		} else {
			val = val_list[n];
			val_len = len_list[n];
		}
		n++;
		/* add variable to symbol table */
		php_register_variable_safe(var, val, val_len, array_ptr TSRMLS_CC);
		if (convd != NULL){
			mbfl_string_clear(&resvar);
			mbfl_string_clear(&resval);
		}
	}

	MBSTRG(http_input_identify) = from_encoding;
	retval = 1;

out:
	/* register_global stuff */
	if (force_register_globals) {
		PG(register_globals) = prev_rg_state;
	}

	if (convd != NULL) {
		mbfl_buffer_converter_delete(convd TSRMLS_CC);
	}
	if (val_list != NULL) {
		efree((void *)val_list);
	}
	if (len_list != NULL) {
		efree((void *)len_list);
	}

	return retval;
}
/* }}} */

/* {{{ SAPI_POST_HANDLER_FUNC(php_mb_post_handler) */
SAPI_POST_HANDLER_FUNC(php_mb_post_handler)
{
	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;

	_php_mb_encoding_handler_ex(arg, SG(request_info).post_data, "&", 0, 0 TSRMLS_CC);

	if (MBSTRG(http_input_identify) != mbfl_no_encoding_invalid) {
		MBSTRG(http_input_identify_post) = MBSTRG(http_input_identify);
	}
}
/* }}} */

#endif /* HAVE_MBSTRING */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

