/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: John Coggeshall <john@php.net>                               |
  |         Ilia Alshanetsky <ilia@php.net>				 				 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_tidy.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_API.h"
#include "Zend/zend_hash.h"
#include "safe_mode.h"

ZEND_DECLARE_MODULE_GLOBALS(tidy);

#define TIDY_PARSED_CHECK() \
if(!TG(tdoc)->parsed) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function."); \
	RETURN_FALSE; \
} \

#define TIDY_SAFE_MODE_CHECK(filename) \
if ((PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(filename TSRMLS_CC)) { \
	RETURN_FALSE; \
} \

#define TIDY_CLEAR_ERROR  if (TG(tdoc)->errbuf && TG(tdoc)->errbuf->bp) { tidyBufClear(TG(tdoc)->errbuf); }

function_entry tidy_functions[] = {
	PHP_FE(tidy_setopt,             NULL)
	PHP_FE(tidy_getopt,             NULL)
	PHP_FE(tidy_parse_string,       NULL)
	PHP_FE(tidy_parse_file,         NULL)
	PHP_FE(tidy_get_output,         NULL)
	PHP_FE(tidy_get_error_buffer,   NULL)
	PHP_FE(tidy_clean_repair,       NULL)
	PHP_FE(tidy_repair_string,	NULL)
	PHP_FE(tidy_repair_file,	NULL)
	PHP_FE(tidy_diagnose,           NULL)
	PHP_FE(tidy_get_release,	NULL)
	PHP_FE(tidy_get_config,		NULL)
	PHP_FE(tidy_get_status,		NULL)
	PHP_FE(tidy_get_html_ver,	NULL)
	PHP_FE(tidy_is_xhtml,		NULL)
	PHP_FE(tidy_is_xml,		NULL)
	PHP_FE(tidy_error_count,	NULL)
	PHP_FE(tidy_warning_count,	NULL)
	PHP_FE(tidy_access_count,	NULL)
	PHP_FE(tidy_config_count,	NULL)
	PHP_FE(tidy_load_config,	NULL)
	PHP_FE(tidy_load_config_enc,	NULL)
	PHP_FE(tidy_set_encoding,	NULL)
	PHP_FE(tidy_save_config,	NULL)
	{NULL, NULL, NULL}
};


zend_module_entry tidy_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"tidy",
	tidy_functions,
	PHP_MINIT(tidy),
	PHP_MSHUTDOWN(tidy),
	PHP_RINIT(tidy),
	NULL,
	PHP_MINFO(tidy),
#if ZEND_MODULE_API_NO >= 20010901
	"1.0",
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TIDY
ZEND_GET_MODULE(tidy)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("tidy.default_config",	"",	PHP_INI_SYSTEM,		OnUpdateString,		default_config,		zend_tidy_globals,	tidy_globals)
PHP_INI_END()
/* }}} */

static void tidy_globals_ctor(zend_tidy_globals *g TSRMLS_DC)
{
	g->used = 0;
	g->tdoc = pemalloc(sizeof(PHPTidyDoc), 1);
	g->tdoc->doc = tidyCreate();
	g->tdoc->parsed = 0;
	g->tdoc->errbuf = pemalloc(sizeof(TidyBuffer), 1);
	tidyBufInit(g->tdoc->errbuf);

	if(tidySetErrorBuffer(g->tdoc->doc, g->tdoc->errbuf) != 0) {
		zend_error(E_ERROR, "Could not set Tidy error buffer");
	}

	tidyOptSetBool(g->tdoc->doc, TidyForceOutput, yes);
	tidyOptSetBool(g->tdoc->doc, TidyMark, no);

	/* remember settings so that we can restore them */
	tidyOptSnapshot(g->tdoc->doc);
}

static void tidy_globals_dtor(zend_tidy_globals *g TSRMLS_DC)
{
	tidyBufFree(g->tdoc->errbuf);
	pefree(g->tdoc->errbuf, 1);
	tidyRelease(g->tdoc->doc);
	pefree(g->tdoc, 1);
	g->used = 0;
}

static void *php_tidy_get_opt_val(TidyOption opt, TidyOptionType *type TSRMLS_DC)
{
	*type = tidyOptGetType(opt);

	switch (*type) {
		case TidyString: {
			char *val = (char *) tidyOptGetValue(TG(tdoc)->doc, tidyOptGetId(opt));
			if (val) {
				return (void *) estrdup(val);
			} else {
				return (void *) estrdup("");
			}
		}
			break;

		case TidyInteger:
			return (void *) tidyOptGetInt(TG(tdoc)->doc, tidyOptGetId(opt));
			break;

		case TidyBoolean:
			return (void *) tidyOptGetBool(TG(tdoc)->doc, tidyOptGetId(opt));
			break;
	}

	/* should not happen */
	return NULL;
}

static int _php_tidy_set_tidy_opt(char *optname, zval *value TSRMLS_DC)
{
	TidyOption opt;
	zval conv = *value;

	if (!(opt = tidyGetOptionByName(TG(tdoc)->doc, optname))) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown Tidy Configuration Option '%s'", optname);
		return FAILURE;
	}

	if (tidyOptIsReadOnly(opt)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Attempting to set read-only option '%s'", optname);
		return FAILURE;
	}

	switch(tidyOptGetType(opt)) {
		case TidyString:
			if (Z_TYPE(conv) != IS_STRING) {
 				zval_copy_ctor(&conv);
 				convert_to_string(&conv);
 			}
 			if (tidyOptSetValue(TG(tdoc)->doc, tidyOptGetId(opt), Z_STRVAL(conv))) {
 				if (Z_TYPE(conv) != Z_TYPE_P(value)) {
 					zval_dtor(&conv);
 				}
  				return SUCCESS;
  			}
 			if (Z_TYPE(conv) != Z_TYPE_P(value)) {
 				zval_dtor(&conv);
 			}
			break;

		case TidyInteger:
 			if (Z_TYPE(conv) != IS_LONG) {
 				zval_copy_ctor(&conv);
 				convert_to_long(&conv);
 			}
 			if (tidyOptSetInt(TG(tdoc)->doc, tidyOptGetId(opt), Z_LVAL(conv))) {
				return SUCCESS;
			}
			break;

		case TidyBoolean:
			if (Z_TYPE(conv) != IS_LONG) {
 				zval_copy_ctor(&conv);
 				convert_to_long(&conv);
 			}
 			if (tidyOptSetBool(TG(tdoc)->doc, tidyOptGetId(opt), Z_LVAL(conv))) {
				return SUCCESS;
			}
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of configuration option");
			break;
	}	

	return FAILURE;
}

static char *php_tidy_file_to_mem(char *filename, zend_bool use_include_path TSRMLS_DC)
{
	php_stream *stream;
	int len;
	char *data = NULL;

	if (!(stream = php_stream_open_wrapper(filename, "rb", (use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL))) {
		return NULL;
	}
	if ((len = php_stream_copy_to_mem(stream, &data, PHP_STREAM_COPY_ALL, 0)) > 0) {
		/* noop */
	} else if (len == 0) {
		data = estrdup("");
	}
	php_stream_close(stream);

	return data;
}

static void php_tidy_quick_repair(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_file)
{
	char *data=NULL, *arg1;
	int arg1_len;
	zend_bool use_include_path = 0;
	zval *cfg=NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|zb", &arg1, &arg1_len, &cfg, &use_include_path) == FAILURE) {
		RETURN_FALSE;
	}

	if (is_file) {
		if (!(data = php_tidy_file_to_mem(arg1, use_include_path TSRMLS_CC))) {
			RETURN_FALSE;
		}
	} else {
		data = arg1;
	}

	TIDY_CLEAR_ERROR;

	if (cfg) {
		if(Z_TYPE_P(cfg) == IS_ARRAY) {
			char *opt_name = NULL;
			zval **opt_val;
			ulong opt_indx;

			for (zend_hash_internal_pointer_reset(HASH_OF(cfg)); 
					zend_hash_get_current_data(HASH_OF(cfg), (void **)&opt_val) == SUCCESS;
			 		zend_hash_move_forward(HASH_OF(cfg))) {
		
				if(zend_hash_get_current_key(HASH_OF(cfg), &opt_name, &opt_indx, FALSE) == FAILURE) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not retrieve key from option array");
				}
        
				if(opt_name) {
					if (_php_tidy_set_tidy_opt(opt_name, *opt_val TSRMLS_CC) != FAILURE) {
						TG(used) = 1;	
					}
					opt_name = NULL;
				}
			}
		} else {
			convert_to_string_ex(&cfg);
			if (Z_STRVAL_P(cfg) && Z_STRVAL_P(cfg)[0]) {
				TIDY_SAFE_MODE_CHECK(Z_STRVAL_P(cfg));
				if(tidyLoadConfig(TG(tdoc)->doc, Z_STRVAL_P(cfg)) < 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s'", Z_STRVAL_P(cfg));
					RETVAL_FALSE;
				}
				TG(used) = 1;
			}
		}
	}

	if (data) {
		if(tidyParseString(TG(tdoc)->doc, data) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "[Tidy error] %s", TG(tdoc)->errbuf->bp);
			RETVAL_FALSE;
		} else {
			TG(tdoc)->parsed = TRUE;
			if (tidyCleanAndRepair(TG(tdoc)->doc) >= 0) {
				TidyBuffer output = {0};

				tidySaveBuffer (TG(tdoc)->doc, &output);
				RETVAL_STRING(output.bp, 1);
				tidyBufFree(&output);
			} else {
				RETVAL_FALSE;
			}
		}
	}

	if (is_file) {
		efree(data);
	}
}

PHP_MINIT_FUNCTION(tidy)
{
	ZEND_INIT_MODULE_GLOBALS(tidy, tidy_globals_ctor, tidy_globals_dtor);
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(tidy)
{
#ifndef ZTS
        tidy_globals_dtor(&tidy_globals TSRMLS_CC);
#endif
	return SUCCESS;
}

PHP_RINIT_FUNCTION(tidy)
{
	if (TG(used) && tidyOptDiffThanSnapshot((TG(tdoc))->doc)) {
		tidyOptResetToSnapshot((TG(tdoc))->doc);
		TG(used) = 0;
	}
	/* if a user provided a default configuration file, use it */
	if (TG(default_config) && TG(default_config)[0]) {
		if (tidyLoadConfig((TG(tdoc))->doc, TG(default_config)) < 0) {
			zend_error(E_ERROR, "Unable to load Tidy configuration file at '%s'.", TG(default_config));
		}
		TG(used) = 1;
	}

	TIDY_CLEAR_ERROR;

	return SUCCESS;
}

PHP_MINFO_FUNCTION(tidy)
{
	TidyIterator itOpt = tidyGetOptionList(TG(tdoc)->doc);
	void *opt_value;
	TidyOptionType optt;
	char buf[255];

	php_info_print_table_start();
	php_info_print_table_header(2, "Tidy support", "enabled");
	php_info_print_table_row(2, "libTidy Build Date", (char *)tidyReleaseDate());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

	php_info_print_table_start();
	php_info_print_table_header(2, "Tidy Configuration Directive", "Value");
	while (itOpt) {
		TidyOption opt = tidyGetNextOption(TG(tdoc)->doc, &itOpt);

		opt_value = php_tidy_get_opt_val(opt, &optt TSRMLS_CC);
		switch (optt) {
			case TidyString:
				php_info_print_table_row(2, (char *)tidyOptGetName(opt), (char*)opt_value);
				efree(opt_value);
				break;

			case TidyInteger:
				sprintf(buf, "%d", (int)opt_value);
				php_info_print_table_row(2, (char *)tidyOptGetName(opt), (char*)buf);
				break;

			case TidyBoolean:
				php_info_print_table_row(2, (char *)tidyOptGetName(opt), (opt_value ? "TRUE" : "FALSE"));
				break;
		}
	}
	php_info_print_table_end();
}

/* {{{ proto bool tidy_parse_string(string input)
   Parse a document stored in a string */
PHP_FUNCTION(tidy_parse_string)
{
	char *input;
	int input_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE) {
		RETURN_FALSE;
	}

	TIDY_CLEAR_ERROR;

	if(tidyParseString(TG(tdoc)->doc, input) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[Tidy error] %s", TG(tdoc)->errbuf->bp);
		RETURN_FALSE;
	}

	TG(tdoc)->parsed = TRUE;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string tidy_get_error_buffer([boolean detailed])
   Return warnings and errors which occured parsing the specified document*/
PHP_FUNCTION(tidy_get_error_buffer)
{
	zend_bool detailed = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &detailed) == FAILURE) {
		RETURN_FALSE;
	}

	TIDY_PARSED_CHECK();

	if (detailed) {
		tidyErrorSummary(TG(tdoc)->doc);
	}
	if (!TG(tdoc)->errbuf || !TG(tdoc)->errbuf->bp) {
		RETURN_FALSE;
	}
	RETVAL_STRING(TG(tdoc)->errbuf->bp, 1);
	tidyBufClear(TG(tdoc)->errbuf); 
}
/* }}} */

/* {{{ proto string tidy_get_output() 
   Return a string representing the parsed tidy markup */
PHP_FUNCTION(tidy_get_output)
{
	TidyBuffer output = {0};

	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	tidySaveBuffer (TG(tdoc)->doc, &output);

	RETVAL_STRING(output.bp, 1);

	tidyBufFree(&output);
}
/* }}} */

/* {{{ proto boolean tidy_parse_file(string file [, bool use_include_path])
   Parse markup in file or URI */
PHP_FUNCTION(tidy_parse_file)
{
	char *inputfile;
	int input_len;
	zend_bool use_include_path = 0;
	char *contents;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &inputfile, &input_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!(contents = php_tidy_file_to_mem(inputfile, use_include_path TSRMLS_CC))) {
		RETURN_FALSE;
	}

	TIDY_CLEAR_ERROR;

	if(tidyParseString(TG(tdoc)->doc, contents) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[Tidy error] %s", TG(tdoc)->errbuf->bp);
		RETVAL_FALSE;
	} else {
		TG(tdoc)->parsed = TRUE;
		RETVAL_TRUE;
	}

	efree(contents);
}
/* }}} */

/* {{{ proto boolean tidy_clean_repair()
   Execute configured cleanup and repair operations on parsed markup */
PHP_FUNCTION(tidy_clean_repair)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	if (tidyCleanAndRepair(TG(tdoc)->doc) >= 0) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean tidy_repair_string(string data [, string config_file])
   Repair a string using an optionally provided configuration file */
PHP_FUNCTION(tidy_repair_string)
{
	php_tidy_quick_repair(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto boolean tidy_repair_file(string filename [, string config_file [, bool use_include_path]])
   Repair a file using an optionally provided configuration file */
PHP_FUNCTION(tidy_repair_file)
{
	php_tidy_quick_repair(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto boolean tidy_diagnose()
    Run configured diagnostics on parsed and repaired markup. */
PHP_FUNCTION(tidy_diagnose)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	if (tidyRunDiagnostics(TG(tdoc)->doc) >= 0) {
		RETURN_TRUE;
	}

	RETURN_FALSE;    
}
/* }}} */

/* {{{ proto string tidy_get_release()
    Get release date (version) for Tidy library */
PHP_FUNCTION(tidy_get_release)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING((char *)tidyReleaseDate(), 1);    
}
/* }}} */

/* {{{ proto string tidy_reset_config()
    Restore Tidy configuration to default values */
PHP_FUNCTION(tidy_reset_config)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	RETURN_BOOL(tidyOptResetToSnapshot(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto array tidy_get_config()
    Get current Tidy configuarion */
PHP_FUNCTION(tidy_get_config)
{
	TidyIterator itOpt = tidyGetOptionList(TG(tdoc)->doc);
	char *opt_name;
	void *opt_value;
	TidyOptionType optt;

	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	array_init(return_value);

	while (itOpt) {
		TidyOption opt = tidyGetNextOption(TG(tdoc)->doc, &itOpt);

		opt_name = (char *)tidyOptGetName(opt);
		opt_value = php_tidy_get_opt_val(opt, &optt TSRMLS_CC);
		switch (optt) {
			case TidyString:
				add_assoc_string(return_value, opt_name, (char*)opt_value, 0);
				break;

			case TidyInteger:
				add_assoc_long(return_value, opt_name, (long)opt_value);
				break;

			case TidyBoolean:
				add_assoc_bool(return_value, opt_name, (long)opt_value);
				break;
		}
		
	}

	return;
}
/* }}} */


/* {{{ proto int tidy_get_status()
    Get status of specfied document. */
PHP_FUNCTION(tidy_get_status)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(tidyStatus(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto int tidy_get_html_ver()
    Get the Detected HTML version for the specified document. */
PHP_FUNCTION(tidy_get_html_ver)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_LONG(tidyDetectedHtmlVersion(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml()
    Indicates if the document is a XHTML document. */
PHP_FUNCTION(tidy_is_xhtml)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_BOOL(tidyDetectedXhtml(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml()
    Indicates if the document is a generic (non HTML/XHTML) XML document. */
PHP_FUNCTION(tidy_is_xml)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_BOOL(tidyDetectedGenericXml(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto int tidy_error_count()
    Returns the Number of Tidy errors encountered for specified document. */
PHP_FUNCTION(tidy_error_count)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_LONG(tidyErrorCount(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto int tidy_warning_count()
    Returns the Number of Tidy warnings encountered for specified document. */
PHP_FUNCTION(tidy_warning_count)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_LONG(tidyWarningCount(TG(tdoc)->doc));
}
/* }}} */

/* {{{ proto int tidy_access_count()
    Returns the Number of Tidy accessibility warnings encountered for specified document. */
PHP_FUNCTION(tidy_access_count)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_LONG(tidyAccessWarningCount(TG(tdoc)->doc));	
}
/* }}} */

/* {{{ proto int tidy_config_count()
    Returns the Number of Tidy configuration errors encountered for specified document. */
PHP_FUNCTION(tidy_config_count)
{
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	RETURN_LONG(tidyConfigErrorCount(TG(tdoc)->doc));	
}
/* }}} */

/* {{{ proto void tidy_load_config(string filename)
    Load an ASCII Tidy configuration file */
PHP_FUNCTION(tidy_load_config)
{
	char *filename;
	int filename_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		RETURN_FALSE;
	}

	TIDY_SAFE_MODE_CHECK(filename);

	if(tidyLoadConfig(TG(tdoc)->doc, filename) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s'", filename);
		RETURN_FALSE;
	}

	TG(used) = 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void tidy_load_config(string filename, string encoding)
    Load an ASCII Tidy configuration file with the specified encoding */
PHP_FUNCTION(tidy_load_config_enc)
{
	char *filename, *encoding;
	int enc_len, file_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filename, &file_len, &encoding, &enc_len) == FAILURE) {
		RETURN_FALSE;
	}

	TIDY_SAFE_MODE_CHECK(filename);

	if(tidyLoadConfigEnc(TG(tdoc)->doc, filename, encoding) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s' using encoding '%s'", filename, encoding);
		RETURN_FALSE;
	}

	TG(used) = 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_set_encoding(string encoding)
    Set the input/output character encoding for parsing markup.
    Values include: ascii, latin1, raw, utf8, iso2022, mac, win1252, utf16le,
    utf16be, utf16, big5 and shiftjis. */
PHP_FUNCTION(tidy_set_encoding)
{
	char *encoding;
	int enc_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &encoding, &enc_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(tidySetCharEncoding(TG(tdoc)->doc, encoding) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not set encoding '%s'", encoding);
		RETURN_FALSE;
	}

	TG(used) = 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_save_config(string filename)
    Save current settings to named file. Only non-default values are written. */
PHP_FUNCTION(tidy_save_config)
{
	char *filename;
	int file_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &file_len) == FAILURE) {
		RETURN_FALSE;
	}

	TIDY_SAFE_MODE_CHECK(filename);

	if(tidyOptSaveFile(TG(tdoc)->doc, filename) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not write tidy configuration file '%s'", filename);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto boolean tidy_setopt(string option, mixed newvalue)
    Updates the configuration settings for the specified tidy document. */
PHP_FUNCTION(tidy_setopt)
{
	zval *value;
	char *optname;
	int optname_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &optname, &optname_len, &value) == FAILURE) {
		RETURN_FALSE;
	}

	if (_php_tidy_set_tidy_opt(optname, value TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	} else {
		TG(used) = 1;
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto mixed tidy_getopt(string option)
    Returns the value of the specified configuration option for the tidy document. */
PHP_FUNCTION(tidy_getopt)
{
	char *optname;
	void *optval;
	int optname_len;
	TidyOption opt;
	TidyOptionType optt;
	    
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &optname, &optname_len) == FAILURE) {
		RETURN_FALSE;
	}

	opt = tidyGetOptionByName(TG(tdoc)->doc, optname);
	if (!opt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		RETURN_FALSE;
	}
	    
	optval = php_tidy_get_opt_val(opt, &optt TSRMLS_CC);
	switch (optt) {
		case TidyString:
			RETVAL_STRING((char *)optval, 0);
			break;

		case TidyInteger:
			RETURN_LONG((long)optval);
			break;

		case TidyBoolean:
			if (optval) {
				RETURN_TRUE;
			} else {
				RETURN_NULL();
			}
			break;
			    
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of Tidy configuration constant to get");
			break;
	}

	RETURN_FALSE;
}
/* }}} */
