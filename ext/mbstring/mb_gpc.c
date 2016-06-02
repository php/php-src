/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include "libmbfl/mbfl/mbfilter_pass.h"
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

/* {{{ MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data)
 * http input processing */
MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data)
{
	char *res = NULL, *separator=NULL;
	const char *c_var;
	zval v_array;
	int free_buffer=0;
	const mbfl_encoding *detected;
	php_mb_encoding_handler_info_t info;

	if (arg != PARSE_STRING) {
		char *value = MBSTRG(internal_encoding_name);
		_php_mb_ini_mbstring_internal_encoding_set(value, value ? strlen(value): 0);
	}

	if (!MBSTRG(encoding_translation)) {
		php_default_treat_data(arg, str, destArray);
		return;
	}

	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			array_init(&v_array);
			switch (arg) {
				case PARSE_POST:
					ZVAL_COPY_VALUE(&PG(http_globals)[TRACK_VARS_POST], &v_array);
					break;
				case PARSE_GET:
					ZVAL_COPY_VALUE(&PG(http_globals)[TRACK_VARS_GET], &v_array);
					break;
				case PARSE_COOKIE:
					ZVAL_COPY_VALUE(&PG(http_globals)[TRACK_VARS_COOKIE], &v_array);
					break;
			}
			break;
		default:
			ZVAL_COPY_VALUE(&v_array, destArray);
			break;
	}

	if (arg == PARSE_POST) {
		sapi_handle_post(&v_array);
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

	switch (arg) {
		case PARSE_POST:
			MBSTRG(http_input_identify_post) = NULL;
			break;
		case PARSE_GET:
			MBSTRG(http_input_identify_get) = NULL;
			break;
		case PARSE_COOKIE:
			MBSTRG(http_input_identify_cookie) = NULL;
			break;
		case PARSE_STRING:
			MBSTRG(http_input_identify_string) = NULL;
			break;
	}

	info.data_type              = arg;
	info.separator              = separator;
	info.report_errors          = 0;
	info.to_encoding            = MBSTRG(internal_encoding);
	info.to_language            = MBSTRG(language);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size);
	info.from_language          = MBSTRG(language);

	MBSTRG(illegalchars) = 0;

	detected = _php_mb_encoding_handler_ex(&info, &v_array, res);
	MBSTRG(http_input_identify) = detected;

	if (detected) {
		switch(arg){
		case PARSE_POST:
			MBSTRG(http_input_identify_post) = detected;
			break;
		case PARSE_GET:
			MBSTRG(http_input_identify_get) = detected;
			break;
		case PARSE_COOKIE:
			MBSTRG(http_input_identify_cookie) = detected;
			break;
		case PARSE_STRING:
			MBSTRG(http_input_identify_string) = detected;
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

/* {{{ mbfl_no_encoding _php_mb_encoding_handler_ex() */
const mbfl_encoding *_php_mb_encoding_handler_ex(const php_mb_encoding_handler_info_t *info, zval *arg, char *res)
{
	char *var, *val;
	const char *s1, *s2;
	char *strtok_buf = NULL, **val_list = NULL;
	zval *array_ptr = (zval *) arg;
	int n, num, *len_list = NULL;
	size_t val_len, new_val_len;
	mbfl_string string, resvar, resval;
	const mbfl_encoding *from_encoding = NULL;
	mbfl_encoding_detector *identd = NULL;
	mbfl_buffer_converter *convd = NULL;

	mbfl_string_init_set(&string, info->to_language, info->to_encoding->no_encoding);
	mbfl_string_init_set(&resvar, info->to_language, info->to_encoding->no_encoding);
	mbfl_string_init_set(&resval, info->to_language, info->to_encoding->no_encoding);

	if (!res || *res == '\0') {
		goto out;
	}

	/* count the variables(separators) contained in the "res".
	 * separator may contain multiple separator chars.
	 */
	num = 1;
	for (s1=res; *s1 != '\0'; s1++) {
		for (s2=info->separator; *s2 != '\0'; s2++) {
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
	var = php_strtok_r(res, info->separator, &strtok_buf);
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
		var = php_strtok_r(NULL, info->separator, &strtok_buf);
	}

	if (n > (PG(max_input_vars) * 2)) {
		php_error_docref(NULL, E_WARNING, "Input variables exceeded %pd. To increase the limit change max_input_vars in php.ini.", PG(max_input_vars));
		goto out;
	}

	num = n; /* make sure to process initialized vars only */

	/* initialize converter */
	if (info->num_from_encodings <= 0) {
		from_encoding = &mbfl_encoding_pass;
	} else if (info->num_from_encodings == 1) {
		from_encoding = info->from_encodings[0];
	} else {
		/* auto detect */
		from_encoding = NULL;
		identd = mbfl_encoding_detector_new2(info->from_encodings, info->num_from_encodings, MBSTRG(strict_detection));
		if (identd != NULL) {
			n = 0;
			while (n < num) {
				string.val = (unsigned char *)val_list[n];
				string.len = len_list[n];
				if (mbfl_encoding_detector_feed(identd, &string)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge2(identd);
			mbfl_encoding_detector_delete(identd);
		}
		if (!from_encoding) {
			if (info->report_errors) {
				php_error_docref(NULL, E_WARNING, "Unable to detect encoding");
			}
			from_encoding = &mbfl_encoding_pass;
		}
	}

	convd = NULL;
	if (from_encoding != &mbfl_encoding_pass) {
		convd = mbfl_buffer_converter_new2(from_encoding, info->to_encoding, 0);
		if (convd != NULL) {
			mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
			mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
		} else {
			if (info->report_errors) {
				php_error_docref(NULL, E_WARNING, "Unable to create converter");
			}
			goto out;
		}
	}

	/* convert encoding */
	string.no_encoding = from_encoding->no_encoding;

	n = 0;
	while (n < num) {
		string.val = (unsigned char *)val_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resvar) != NULL) {
			var = (char *)resvar.val;
		} else {
			var = val_list[n];
		}
		n++;
		string.val = (unsigned char *)val_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resval) != NULL) {
			val = (char *)resval.val;
			val_len = resval.len;
		} else {
			val = val_list[n];
			val_len = len_list[n];
		}
		n++;
		/* we need val to be emalloc()ed */
		val = estrndup(val, val_len);
		if (sapi_module.input_filter(info->data_type, var, &val, val_len, &new_val_len)) {
			/* add variable to symbol table */
			php_register_variable_safe(var, val, new_val_len, array_ptr);
		}
		efree(val);

		if (convd != NULL){
			mbfl_string_clear(&resvar);
			mbfl_string_clear(&resval);
		}
	}

out:
	if (convd != NULL) {
		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
		mbfl_buffer_converter_delete(convd);
	}
	if (val_list != NULL) {
		efree((void *)val_list);
	}
	if (len_list != NULL) {
		efree((void *)len_list);
	}

	return from_encoding;
}
/* }}} */

/* {{{ SAPI_POST_HANDLER_FUNC(php_mb_post_handler) */
SAPI_POST_HANDLER_FUNC(php_mb_post_handler)
{
	const mbfl_encoding *detected;
	php_mb_encoding_handler_info_t info;
	zend_string *post_data_str = NULL;

	MBSTRG(http_input_identify_post) = NULL;

	info.data_type              = PARSE_POST;
	info.separator              = "&";
	info.report_errors          = 0;
	info.to_encoding            = MBSTRG(internal_encoding);
	info.to_language            = MBSTRG(language);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size);
	info.from_language          = MBSTRG(language);

	php_stream_rewind(SG(request_info).request_body);
	post_data_str = php_stream_copy_to_mem(SG(request_info).request_body, PHP_STREAM_COPY_ALL, 0);
	detected = _php_mb_encoding_handler_ex(&info, arg, post_data_str ? ZSTR_VAL(post_data_str) : NULL);
	if (post_data_str) {
		zend_string_release(post_data_str);
	}

	MBSTRG(http_input_identify) = detected;
	if (detected) {
		MBSTRG(http_input_identify_post) = detected;
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

