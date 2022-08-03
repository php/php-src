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
   | Author: Rui Hirokawa <hirokawa@php.net>                              |
   |         Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
 */

/* {{{ includes */
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

#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "TSRM.h"

#include "mb_gpc.h"
/* }}} */

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

	switch (arg) {
		case PARSE_POST:
			sapi_handle_post(&v_array);
			return;
		case PARSE_GET: /* GET data */
			c_var = SG(request_info).query_string;
			if (c_var && *c_var) {
				res = (char *) estrdup(c_var);
				free_buffer = 1;
			}
			break;
		case PARSE_COOKIE: /* Cookie data */
			c_var = SG(request_info).cookie_data;
			if (c_var && *c_var) {
				res = (char *) estrdup(c_var);
				free_buffer = 1;
			}
			break;
		case PARSE_STRING: /* String data */
			res = str;
			free_buffer = 1;
			break;
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
	info.report_errors          = false;
	info.to_encoding            = MBSTRG(internal_encoding);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size);

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
const mbfl_encoding *_php_mb_encoding_handler_ex(const php_mb_encoding_handler_info_t *info, zval *array_ptr, char *res)
{
	char *var, *val;
	char *strtok_buf = NULL, **val_list = NULL;
	size_t n, num = 1, *len_list = NULL;
	size_t new_val_len;
	const mbfl_encoding *from_encoding = NULL;
	mbfl_encoding_detector *identd = NULL;

	if (!res || *res == '\0') {
		goto out;
	}

	/* count variables contained in `res`.
	 * separator may contain multiple separator chars; ANY of them demarcate variables */
	for (char *s1 = res; *s1; s1++) {
		for (const char *s2 = info->separator; *s2; s2++) {
			if (*s1 == *s2) {
				num++;
			}
		}
	}
	num *= 2; /* need space for variable name and value */

	val_list = (char **)ecalloc(num, sizeof(char *));
	len_list = (size_t *)ecalloc(num, sizeof(size_t));

	/* split and decode the query */
	n = 0;
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

	if (ZEND_SIZE_T_GT_ZEND_LONG(n, (PG(max_input_vars) * 2))) {
		php_error_docref(NULL, E_WARNING, "Input variables exceeded " ZEND_LONG_FMT ". To increase the limit change max_input_vars in php.ini.", PG(max_input_vars));
		goto out;
	}

	num = n; /* make sure to process initialized vars only */

	/* initialize converter */
	if (info->num_from_encodings == 0) {
		from_encoding = &mbfl_encoding_pass;
	} else if (info->num_from_encodings == 1) {
		from_encoding = info->from_encodings[0];
	} else {
		/* auto detect */
		from_encoding = NULL;
		identd = mbfl_encoding_detector_new(info->from_encodings, info->num_from_encodings, MBSTRG(strict_detection));
		if (identd != NULL) {
			n = 0;
			while (n < num) {
				mbfl_string string;
				string.val = (unsigned char *)val_list[n];
				string.len = len_list[n];
				if (mbfl_encoding_detector_feed(identd, &string)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge(identd);
			mbfl_encoding_detector_delete(identd);
		}
		if (!from_encoding) {
			if (info->report_errors) {
				php_error_docref(NULL, E_WARNING, "Unable to detect encoding");
			}
			from_encoding = &mbfl_encoding_pass;
		}
	}

	/* convert encoding */
	n = 0;
	while (n < num) {
		if (from_encoding != &mbfl_encoding_pass && info->to_encoding != &mbfl_encoding_pass) {
			unsigned int num_errors = 0;
			zend_string *converted_var = mb_fast_convert((unsigned char*)val_list[n], len_list[n], from_encoding, info->to_encoding, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode), &num_errors);
			MBSTRG(illegalchars) += num_errors;
			n++;

			num_errors = 0;
			zend_string *converted_val = mb_fast_convert((unsigned char*)val_list[n], len_list[n], from_encoding, info->to_encoding, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode), &num_errors);
			MBSTRG(illegalchars) += num_errors;
			n++;

			/* `val` must be a pointer returned by `emalloc` */
			val = estrndup(ZSTR_VAL(converted_val), ZSTR_LEN(converted_val));
			if (sapi_module.input_filter(info->data_type, ZSTR_VAL(converted_var), &val, ZSTR_LEN(converted_val), &new_val_len)) {
				/* add variable to symbol table */
				php_register_variable_safe(ZSTR_VAL(converted_var), val, new_val_len, array_ptr);
			}
			zend_string_free(converted_var);
			zend_string_free(converted_val);
		} else {
			var = val_list[n++];
			val = estrndup(val_list[n], len_list[n]);
			if (sapi_module.input_filter(info->data_type, var, &val, len_list[n], &new_val_len)) {
				php_register_variable_safe(var, val, new_val_len, array_ptr);
			}
			n++;
		}
		efree(val);
	}

out:
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
	info.report_errors          = false;
	info.to_encoding            = MBSTRG(internal_encoding);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size);

	php_stream_rewind(SG(request_info).request_body);
	post_data_str = php_stream_copy_to_mem(SG(request_info).request_body, PHP_STREAM_COPY_ALL, 0);
	detected = _php_mb_encoding_handler_ex(&info, arg, post_data_str ? ZSTR_VAL(post_data_str) : NULL);
	if (post_data_str) {
		zend_string_release_ex(post_data_str, 0);
	}

	MBSTRG(http_input_identify) = detected;
	if (detected) {
		MBSTRG(http_input_identify_post) = detected;
	}
}
/* }}} */
