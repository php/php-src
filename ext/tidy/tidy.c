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

#ifdef ZEND_ENGINE_2
	PHP_FE(tidy_get_root,		NULL)
	PHP_FE(tidy_get_head,		NULL)
	PHP_FE(tidy_get_html,		NULL)
	PHP_FE(tidy_get_body,		NULL)
#endif

	{NULL, NULL, NULL}
};

#ifdef ZEND_ENGINE_2
#include "zend_default_classes.h"

static void tidy_object_dtor(void *object, zend_object_handle handle TSRMLS_DC);
static void tidy_object_new(zend_class_entry *class_type, zend_object_handlers *handlers, zend_object_value *retval TSRMLS_DC);

static zend_object_value tidy_object_new_node(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value tidy_object_new_attr(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value tidy_object_new_exception(zend_class_entry *class_type TSRMLS_DC);

static zend_class_entry *tidy_get_ce_node(zval *object TSRMLS_DC);
static zend_class_entry *tidy_get_ce_attr(zval *object TSRMLS_DC);

static zval * tidy_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC);

zend_class_entry *tidy_ce_node, *tidy_ce_attr,
				 *tidy_ce_exception;
                 
static zend_object_handlers tidy_object_handlers_node;
static zend_object_handlers tidy_object_handlers_attr;
static zend_object_handlers tidy_object_handlers_exception;

function_entry tidy_funcs_node[] = {
	
	PHP_NODE_ME(__construct, NULL)
	PHP_NODE_ME(attributes, NULL)
	PHP_NODE_ME(children, NULL)
	
	PHP_NODE_ME(has_children, NULL)
	PHP_NODE_ME(has_siblings, NULL)
	PHP_NODE_ME(is_comment, NULL)
	PHP_NODE_ME(is_html, NULL)
	PHP_NODE_ME(is_text, NULL)
	PHP_NODE_ME(is_jste, NULL)
	PHP_NODE_ME(is_asp, NULL)
	PHP_NODE_ME(is_php, NULL)
	
	PHP_NODE_ME(next, NULL)
	PHP_NODE_ME(prev, NULL)
	PHP_NODE_ME(get_attr, NULL)
	/*PHP_NODE_ME(get_nodes, NULL) TODO */
	{NULL, NULL, NULL}
};

function_entry tidy_funcs_attr[] = {
	{NULL, NULL, NULL}
};

function_entry tidy_funcs_exception[] = {
	{NULL, NULL, NULL}
};

#endif

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
	char *data=NULL, *cfg_file=NULL, *arg1;
	int cfg_file_len, arg1_len;
	zend_bool use_include_path = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sb", &arg1, &arg1_len, &cfg_file, &cfg_file_len, &use_include_path) == FAILURE) {
		RETURN_FALSE;
	}

	if (is_file) {
		if (!(data = php_tidy_file_to_mem(arg1, use_include_path TSRMLS_CC))) {
			RETURN_FALSE;
		}
	} else {
		data = arg1;
	}

	if (cfg_file && cfg_file[0]) {
		TIDY_SAFE_MODE_CHECK(cfg_file);
		if(tidyLoadConfig(TG(tdoc)->doc, cfg_file) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s'", cfg_file);
			RETVAL_FALSE;
		}
		TG(used) = 1;
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
	REGISTER_INI_ENTRIES();

#ifdef ZEND_ENGINE_2
	REGISTER_TIDY_CLASS(node, NULL);
	REGISTER_TIDY_CLASS(attr, NULL);
	REGISTER_TIDY_CLASS(exception, zend_exception_get_default());

	tidy_object_handlers_node.get_class_entry = tidy_get_ce_node;
	tidy_object_handlers_attr.get_class_entry = tidy_get_ce_attr;
#endif

	_php_tidy_register_tags(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_attributes(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_nodetypes(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_INIT_MODULE_GLOBALS(tidy, tidy_globals_ctor, tidy_globals_dtor);

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
	TidyOption opt;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &optname, &optname_len, &value) == FAILURE) {
		RETURN_FALSE;
	}

	opt = tidyGetOptionByName(TG(tdoc)->doc, optname);
	if (!opt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		RETURN_FALSE;
	}
	    
	switch(tidyOptGetType(opt)) {
		case TidyString:
			convert_to_string_ex(&value);
			if(tidyOptSetValue(TG(tdoc)->doc, tidyOptGetId(opt), Z_STRVAL_P(value))) {
				TG(used) = 1;
				RETURN_TRUE;
			} 
			break;
			
		case TidyInteger:
			convert_to_long_ex(&value);
			if(tidyOptSetInt(TG(tdoc)->doc, tidyOptGetId(opt), Z_LVAL_P(value))) {
				TG(used) = 1;
				RETURN_TRUE;
			} 
			break;
			
		case TidyBoolean:
			convert_to_long_ex(&value);
			if(tidyOptSetBool(TG(tdoc)->doc,  tidyOptGetId(opt), Z_LVAL_P(value))) {
				TG(used) = 1;
				RETURN_TRUE;
			} 
			break;
			
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of Tidy configuration constant to set");
			break;
	}
	RETURN_FALSE;
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
   
#ifdef ZEND_ENGINE_2
static void tidy_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	PHPTidyObj *intern = (PHPTidyObj *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);
	
	efree(object);
}

static void tidy_object_new(zend_class_entry *class_type, zend_object_handlers *handlers, zend_object_value *retval TSRMLS_DC)
{
	PHPTidyObj *intern;
	zval *tmp;

	intern = emalloc(sizeof(PHPTidyObj));
	memset(intern, 0, sizeof(PHPTidyObj));
	intern->std.ce = class_type;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval->handle = zend_objects_store_put(intern, tidy_object_dtor, NULL TSRMLS_CC);
	retval->handlers = handlers;
}

static zend_object_value tidy_object_new_node(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_node, &retval TSRMLS_CC);
	return retval;
}

static zend_object_value tidy_object_new_attr(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_attr, &retval TSRMLS_CC);
	return retval;
	
}

static zend_object_value tidy_object_new_exception(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_exception, &retval TSRMLS_CC);
	return retval;
	
}

static zend_class_entry *tidy_get_ce_node(zval *object TSRMLS_DC)
{
	return tidy_ce_node;
}

static zend_class_entry *tidy_get_ce_attr(zval *object TSRMLS_DC)
{
	return tidy_ce_attr;
}

static zval * tidy_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}
	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	object->refcount = 1;
	object->is_ref = 1;
	return object;
}

#define ADD_PROPERITY_STRING(_table, _key, _string) \
	{ \
		zval *tmp; \
		MAKE_STD_ZVAL(tmp); \
		if(_string) { \
			ZVAL_STRING(tmp, (char *)_string, 1); \
		} else { \
			ZVAL_EMPTY_STRING(tmp); \
		} \
		zend_hash_update(_table, #_key, sizeof(#_key), (void *)&tmp, sizeof(zval *), NULL); \
	}

#define ADD_PROPERITY_LONG(_table, _key, _long) \
	{ \
		zval *tmp; \
		MAKE_STD_ZVAL(tmp); \
		ZVAL_LONG(tmp, _long); \
		zend_hash_update(_table, #_key, sizeof(#_key), (void *)&tmp, sizeof(zval *), NULL); \
	}

static void tidy_add_default_properities(PHPTidyObj *obj, tidy_obj_type type TSRMLS_DC) {
	
	TidyBuffer buf;
	
	switch(type) {
		
		case is_node:
			memset(&buf, 0, sizeof(buf));
			tidyNodeGetText(TG(tdoc)->doc, obj->node, &buf);
			ADD_PROPERITY_STRING(obj->std.properties, value, buf.bp);
			tidyBufFree(&buf);
			
			fprintf(stderr, "type: %d\n",tidyNodeGetType(obj->node)); 
			ADD_PROPERITY_STRING(obj->std.properties, name, tidyNodeGetName(obj->node));
			ADD_PROPERITY_LONG(obj->std.properties, type, tidyNodeGetType(obj->node));
			switch(tidyNodeGetType(obj->node)) {
				
				case TidyNode_Root:
				case TidyNode_DocType:
				case TidyNode_Text:
				case TidyNode_Comment:
					break;
				default:
					ADD_PROPERITY_LONG(obj->std.properties, id, tidyNodeGetId(obj->node));
			}
			
            break;
		case is_attr:
			ADD_PROPERITY_STRING(obj->std.properties, name, tidyAttrName(obj->attr));
			ADD_PROPERITY_STRING(obj->std.properties, value, tidyAttrValue(obj->attr));
			ADD_PROPERITY_LONG(obj->std.properties, id, tidyAttrGetId(obj->attr));
			break;
	}
	
}
/* {{{ proto TidyNode tidy_get_root()
    Returns a TidyNode Object representing the root of the tidy parse tree */
PHP_FUNCTION(tidy_get_root)
{
	PHPTidyObj	*obj;
	
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();
	
	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj->node = tidyGetRoot(TG(tdoc)->doc);
	obj->attr = NULL;
	obj->type = is_node;
	
	tidy_add_default_properities(obj, is_node TSRMLS_CC);
		
}
/* }}} */

/* {{{ proto TidyNode tidy_get_html()
    Returns a TidyNode Object starting from the <HTML> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_html)
{
	PHPTidyObj	*obj;
	
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();
	
	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj->node = tidyGetHtml(TG(tdoc)->doc);
	obj->attr = NULL;
	obj->type = is_node;
	
	tidy_add_default_properities(obj, is_node TSRMLS_CC);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_head()
    Returns a TidyNode Object starting from the <HEAD> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_head)
{
	PHPTidyObj	*obj;
	
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();
	
	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj->node = tidyGetHead(TG(tdoc)->doc);
	obj->attr = NULL;
	obj->type = is_node;
	
	tidy_add_default_properities(obj, is_node TSRMLS_CC);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_body(resource tidy)
    Returns a TidyNode Object starting from the <BODY> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_body)
{
	PHPTidyObj	*obj;
	
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();
	
	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj->node = tidyGetBody(TG(tdoc)->doc);
	obj->attr = NULL;
	obj->type = is_node;
	
	tidy_add_default_properities(obj, is_node TSRMLS_CC);
}
/* }}} */

/* {{{ proto void tidy_node::tidy_node()
   Constructor. */
PHP_NODE_METHOD(__construct)
{
}
/* }}} */

/* {{{ proto tidy_attr tidy_node::attributes()
   Returns an array of attribute objects for node */
PHP_NODE_METHOD(attributes)
{
	TidyAttr	tempattr;
	zval *object;
	PHPTidyObj	*objtemp;
	GET_THIS_CONTAINER();
	
	tempattr = tidyAttrFirst(obj->node);
			
	if(tempattr) {
		array_init(return_value);
		
		do {
			
			MAKE_STD_ZVAL(object);
			INSTANCIATE_ATTR(object, objtemp, tempattr);
			add_next_index_zval(return_value, object);
					
		} while((tempattr = tidyAttrNext(tempattr)));
	}
}
/* }}} */



/* {{{ proto tidy_node tidy_node::children()
   Returns an array of child nodes */
PHP_NODE_METHOD(children)
{
	TidyNode tempnode;
	zval *object;
	PHPTidyObj	*objtemp;
	GET_THIS_CONTAINER();
	
	tempnode = tidyGetChild(obj->node);
			
	if(tempnode) {
		array_init(return_value);
		do {
		
			MAKE_STD_ZVAL(object);
			INSTANCIATE_NODE(object, objtemp, tempnode);
			add_next_index_zval(return_value, object);
							
		} while((tempnode = tidyGetNext(tempnode)));
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::has_children()
   Returns true if this node has children */
PHP_NODE_METHOD(has_children)
{
	GET_THIS_CONTAINER();
	
	if(tidyGetChild(obj->node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
	
}
/* }}} */

/* {{{ proto boolean tidy_node::has_siblings()
   Returns true if this node has siblings */
PHP_NODE_METHOD(has_siblings)
{
	GET_THIS_CONTAINER();
	
	if(tidyGetNext(obj->node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
	
}
/* }}} */
   
/* {{{ proto boolean tidy_node::is_comment()
   Returns true if this node represents a comment */
PHP_NODE_METHOD(is_comment)
{
	GET_THIS_CONTAINER();
	if(tidyNodeGetType(obj->node) == TidyNode_Comment) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}

}
/* }}} */
   
/* {{{ proto boolean tidy_node::is_html()
   Returns true if this node is part of a HTML document */
PHP_NODE_METHOD(is_html)
{
	GET_THIS_CONTAINER();
	
	if(tidyNodeGetType(obj->node) & (TidyNode_Start | TidyNode_End | TidyNode_StartEnd)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
	
}
/* }}} */
   
/* {{{ proto boolean tidy_node::is_xhtml()
   Returns true if this node is part of a XHTML document */ 
PHP_NODE_METHOD(is_xhtml)
{
	GET_THIS_CONTAINER();
	if(tidyDetectedXhtml(TG(tdoc)->doc)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
   
/* {{{ proto boolean tidy_node::is_xml()
   Returns true if this node is part of a XML document */
PHP_NODE_METHOD(is_xml)
{
	GET_THIS_CONTAINER();
	if(tidyDetectedGenericXml(TG(tdoc)->doc)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_text()
   Returns true if this node represents text (no markup) */
PHP_NODE_METHOD(is_text)
{
	GET_THIS_CONTAINER();
	if(tidyNodeGetType(obj->node) == TidyNode_Text) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
	
}
/* }}} */
   
/* {{{ proto boolean tidy_node::is_jste()
   Returns true if this node is JSTE */
PHP_NODE_METHOD(is_jste)
{
	GET_THIS_CONTAINER();
	if(tidyNodeGetType(obj->node) == TidyNode_Jste) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_asp()
   Returns true if this node is ASP */
PHP_NODE_METHOD(is_asp)
{
	GET_THIS_CONTAINER();
	if(tidyNodeGetType(obj->node) == TidyNode_Asp) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
   
/* {{{ proto boolean tidy_node::is_jsp()
   Returns true if this node is JSP */
PHP_NODE_METHOD(is_php)
{
	GET_THIS_CONTAINER();
	if(tidyNodeGetType(obj->node) == TidyNode_Php) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto tidy_node tidy_node::next()
   Returns the next sibling to this node */
PHP_NODE_METHOD(next)
{
	PHPTidyObj	*obj;
	
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();
	
	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj->node = tidyGetNext(obj->node);
	obj->attr = NULL;
	obj->type = is_node;
	
	tidy_add_default_properities(obj, is_node TSRMLS_CC);
}
/* }}} */
   
/* {{{ proto tidy_node tidy_node::prev()
   Returns the previous sibiling to this node */
PHP_NODE_METHOD(prev)
{
	PHPTidyObj	*obj;
	
	if(ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();
	
	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj->node = tidyGetPrev(obj->node);
	obj->attr = NULL;
	obj->type = is_node;
	
	tidy_add_default_properities(obj, is_node TSRMLS_CC);
}
/* }}} */
   
/* {{{ proto tidy_attr tidy_node::get_attr(int attrib_id)
   Return the attribute with the provided attribute id */
PHP_NODE_METHOD(get_attr)
{
	TidyAttr tempattr;
	long param;
	GET_THIS_CONTAINER();
	
	if(ZEND_NUM_ARGS() != 1) {
		WRONG_PARAM_COUNT;
	}

	TIDY_PARSED_CHECK();

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &param) == FAILURE) {
		RETURN_FALSE;
	}
	
	for(tempattr = tidyAttrFirst(obj->node); tempattr; tempattr = tidyAttrNext(tempattr)) {
		
		if(tidyAttrGetId(tempattr) == param) {
	
			tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	
			obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
			obj->node = NULL;
			obj->attr = tempattr;
			obj->type = is_attr;
	
			tidy_add_default_properities(obj, is_attr TSRMLS_CC);
		}
	}
		
}
/* }}} */

/* {{{ proto tidy_node tidy_node::get_nodes(int node_id)
   Return an array of nodes under this node with the specified id */
PHP_NODE_METHOD(get_nodes)
{
	/* TODO */
}
/* }}} */

#endif	/* ZEND_ENGINE_2 */
   
void _php_tidy_register_nodetypes(INIT_FUNC_ARGS)
{
    
    TIDY_NODE_CONST(ROOT, Root);
    TIDY_NODE_CONST(DOCTYPE, DocType);
    TIDY_NODE_CONST(COMMENT, Comment);
    TIDY_NODE_CONST(PROCINS, ProcIns);
    TIDY_NODE_CONST(TEXT, Text);
    TIDY_NODE_CONST(START, Start);
    TIDY_NODE_CONST(END, End);
    TIDY_NODE_CONST(STARTEND, StartEnd);
    TIDY_NODE_CONST(CDATA, CDATA);
    TIDY_NODE_CONST(SECTION, Section);
    TIDY_NODE_CONST(ASP, Asp);
    TIDY_NODE_CONST(JSTE, Jste);
    TIDY_NODE_CONST(PHP, Php);
    TIDY_NODE_CONST(XMLDECL, XmlDecl);
    
}

void _php_tidy_register_tags(INIT_FUNC_ARGS)
{

    TIDY_TAG_CONST(UNKNOWN);
    TIDY_TAG_CONST(A);
    TIDY_TAG_CONST(ABBR);
    TIDY_TAG_CONST(ACRONYM);
    TIDY_TAG_CONST(ADDRESS);
    TIDY_TAG_CONST(ALIGN);
    TIDY_TAG_CONST(APPLET);
    TIDY_TAG_CONST(AREA);
    TIDY_TAG_CONST(B);
    TIDY_TAG_CONST(BASE);
    TIDY_TAG_CONST(BASEFONT);
    TIDY_TAG_CONST(BDO);
    TIDY_TAG_CONST(BGSOUND);
    TIDY_TAG_CONST(BIG);
    TIDY_TAG_CONST(BLINK);
    TIDY_TAG_CONST(BLOCKQUOTE);
    TIDY_TAG_CONST(BODY);
    TIDY_TAG_CONST(BR);
    TIDY_TAG_CONST(BUTTON);
    TIDY_TAG_CONST(CAPTION);
    TIDY_TAG_CONST(CENTER);
    TIDY_TAG_CONST(CITE);
    TIDY_TAG_CONST(CODE);
    TIDY_TAG_CONST(COL);
    TIDY_TAG_CONST(COLGROUP);
    TIDY_TAG_CONST(COMMENT);
    TIDY_TAG_CONST(DD);
    TIDY_TAG_CONST(DEL);
    TIDY_TAG_CONST(DFN);
    TIDY_TAG_CONST(DIR);
    TIDY_TAG_CONST(DIV);
    TIDY_TAG_CONST(DL);
    TIDY_TAG_CONST(DT);
    TIDY_TAG_CONST(EM);
    TIDY_TAG_CONST(EMBED);
    TIDY_TAG_CONST(FIELDSET);
    TIDY_TAG_CONST(FONT);
    TIDY_TAG_CONST(FORM);
    TIDY_TAG_CONST(FRAME);
    TIDY_TAG_CONST(FRAMESET);
    TIDY_TAG_CONST(H1);
    TIDY_TAG_CONST(H2);
    TIDY_TAG_CONST(H3);
    TIDY_TAG_CONST(H4);
    TIDY_TAG_CONST(H5);
    TIDY_TAG_CONST(H6);
    TIDY_TAG_CONST(HEAD);
    TIDY_TAG_CONST(HR);
    TIDY_TAG_CONST(HTML);
    TIDY_TAG_CONST(I);
    TIDY_TAG_CONST(IFRAME);
    TIDY_TAG_CONST(ILAYER);
    TIDY_TAG_CONST(IMG);
    TIDY_TAG_CONST(INPUT);
    TIDY_TAG_CONST(INS);
    TIDY_TAG_CONST(ISINDEX);
    TIDY_TAG_CONST(KBD);
    TIDY_TAG_CONST(KEYGEN);
    TIDY_TAG_CONST(LABEL);
    TIDY_TAG_CONST(LAYER);
    TIDY_TAG_CONST(LEGEND);
    TIDY_TAG_CONST(LI);
    TIDY_TAG_CONST(LINK);
    TIDY_TAG_CONST(LISTING);
    TIDY_TAG_CONST(MAP);
    TIDY_TAG_CONST(MARQUEE);
    TIDY_TAG_CONST(MENU);
    TIDY_TAG_CONST(META);
    TIDY_TAG_CONST(MULTICOL);
    TIDY_TAG_CONST(NOBR);
    TIDY_TAG_CONST(NOEMBED);
    TIDY_TAG_CONST(NOFRAMES);
    TIDY_TAG_CONST(NOLAYER);
    TIDY_TAG_CONST(NOSAVE);
    TIDY_TAG_CONST(NOSCRIPT);
    TIDY_TAG_CONST(OBJECT);
    TIDY_TAG_CONST(OL);
    TIDY_TAG_CONST(OPTGROUP);
    TIDY_TAG_CONST(OPTION);
    TIDY_TAG_CONST(P);
    TIDY_TAG_CONST(PARAM);
    TIDY_TAG_CONST(PLAINTEXT);
    TIDY_TAG_CONST(PRE);
    TIDY_TAG_CONST(Q);
    TIDY_TAG_CONST(RB);
    TIDY_TAG_CONST(RBC);
    TIDY_TAG_CONST(RP);
    TIDY_TAG_CONST(RT);
    TIDY_TAG_CONST(RTC);
    TIDY_TAG_CONST(RUBY);
    TIDY_TAG_CONST(S);
    TIDY_TAG_CONST(SAMP);
    TIDY_TAG_CONST(SCRIPT);
    TIDY_TAG_CONST(SELECT);
    TIDY_TAG_CONST(SERVER);
    TIDY_TAG_CONST(SERVLET);
    TIDY_TAG_CONST(SMALL);
    TIDY_TAG_CONST(SPACER);
    TIDY_TAG_CONST(SPAN);
    TIDY_TAG_CONST(STRIKE);
    TIDY_TAG_CONST(STRONG);
    TIDY_TAG_CONST(STYLE);
    TIDY_TAG_CONST(SUB);
    TIDY_TAG_CONST(SUP);
    TIDY_TAG_CONST(TABLE);
    TIDY_TAG_CONST(TBODY);
    TIDY_TAG_CONST(TD);
    TIDY_TAG_CONST(TEXTAREA);
    TIDY_TAG_CONST(TFOOT);
    TIDY_TAG_CONST(TH);
    TIDY_TAG_CONST(THEAD);
    TIDY_TAG_CONST(TITLE);
    TIDY_TAG_CONST(TR);
    TIDY_TAG_CONST(TT);
    TIDY_TAG_CONST(U);
    TIDY_TAG_CONST(UL);
    TIDY_TAG_CONST(VAR);
    TIDY_TAG_CONST(WBR);
    TIDY_TAG_CONST(XMP);

}

void _php_tidy_register_attributes(INIT_FUNC_ARGS)
{

    TIDY_ATTR_CONST(UNKNOWN);
    TIDY_ATTR_CONST(ABBR);
    TIDY_ATTR_CONST(ACCEPT);
    TIDY_ATTR_CONST(ACCEPT_CHARSET);
    TIDY_ATTR_CONST(ACCESSKEY);
    TIDY_ATTR_CONST(ACTION);
    TIDY_ATTR_CONST(ADD_DATE);
    TIDY_ATTR_CONST(ALIGN);
    TIDY_ATTR_CONST(ALINK);
    TIDY_ATTR_CONST(ALT);
    TIDY_ATTR_CONST(ARCHIVE);
    TIDY_ATTR_CONST(AXIS);
    TIDY_ATTR_CONST(BACKGROUND);
    TIDY_ATTR_CONST(BGCOLOR);
    TIDY_ATTR_CONST(BGPROPERTIES);
    TIDY_ATTR_CONST(BORDER);
    TIDY_ATTR_CONST(BORDERCOLOR);
    TIDY_ATTR_CONST(BOTTOMMARGIN);
    TIDY_ATTR_CONST(CELLPADDING);
    TIDY_ATTR_CONST(CELLSPACING);
    TIDY_ATTR_CONST(CHAR);
    TIDY_ATTR_CONST(CHAROFF);
    TIDY_ATTR_CONST(CHARSET);
    TIDY_ATTR_CONST(CHECKED);
    TIDY_ATTR_CONST(CITE);
    TIDY_ATTR_CONST(CLASS);
    TIDY_ATTR_CONST(CLASSID);
    TIDY_ATTR_CONST(CLEAR);
    TIDY_ATTR_CONST(CODE);
    TIDY_ATTR_CONST(CODEBASE);
    TIDY_ATTR_CONST(CODETYPE);
    TIDY_ATTR_CONST(COLOR);
    TIDY_ATTR_CONST(COLS);
    TIDY_ATTR_CONST(COLSPAN);
    TIDY_ATTR_CONST(COMPACT);
    TIDY_ATTR_CONST(CONTENT);
    TIDY_ATTR_CONST(COORDS);
    TIDY_ATTR_CONST(DATA);
    TIDY_ATTR_CONST(DATAFLD);
 /* TIDY_ATTR_CONST(DATAFORMATSAS); */
    TIDY_ATTR_CONST(DATAPAGESIZE);
    TIDY_ATTR_CONST(DATASRC);
    TIDY_ATTR_CONST(DATETIME);
    TIDY_ATTR_CONST(DECLARE);
    TIDY_ATTR_CONST(DEFER);
    TIDY_ATTR_CONST(DIR);
    TIDY_ATTR_CONST(DISABLED);
    TIDY_ATTR_CONST(ENCODING);
    TIDY_ATTR_CONST(ENCTYPE);
    TIDY_ATTR_CONST(FACE);
    TIDY_ATTR_CONST(FOR);
    TIDY_ATTR_CONST(FRAME);
    TIDY_ATTR_CONST(FRAMEBORDER);
    TIDY_ATTR_CONST(FRAMESPACING);
    TIDY_ATTR_CONST(GRIDX);
    TIDY_ATTR_CONST(GRIDY);
    TIDY_ATTR_CONST(HEADERS);
    TIDY_ATTR_CONST(HEIGHT);
    TIDY_ATTR_CONST(HREF);
    TIDY_ATTR_CONST(HREFLANG);
    TIDY_ATTR_CONST(HSPACE);
    TIDY_ATTR_CONST(HTTP_EQUIV);
    TIDY_ATTR_CONST(ID);
    TIDY_ATTR_CONST(ISMAP);
    TIDY_ATTR_CONST(LABEL);
    TIDY_ATTR_CONST(LANG);
    TIDY_ATTR_CONST(LANGUAGE);
    TIDY_ATTR_CONST(LAST_MODIFIED);
    TIDY_ATTR_CONST(LAST_VISIT);
    TIDY_ATTR_CONST(LEFTMARGIN);
    TIDY_ATTR_CONST(LINK);
    TIDY_ATTR_CONST(LONGDESC);
    TIDY_ATTR_CONST(LOWSRC);
    TIDY_ATTR_CONST(MARGINHEIGHT);
    TIDY_ATTR_CONST(MARGINWIDTH);
    TIDY_ATTR_CONST(MAXLENGTH);
    TIDY_ATTR_CONST(MEDIA);
    TIDY_ATTR_CONST(METHOD);
    TIDY_ATTR_CONST(MULTIPLE);
    TIDY_ATTR_CONST(NAME);
    TIDY_ATTR_CONST(NOHREF);
    TIDY_ATTR_CONST(NORESIZE);
    TIDY_ATTR_CONST(NOSHADE);
    TIDY_ATTR_CONST(NOWRAP);
    TIDY_ATTR_CONST(OBJECT);
    TIDY_ATTR_CONST(OnAFTERUPDATE);
    TIDY_ATTR_CONST(OnBEFOREUNLOAD);
    TIDY_ATTR_CONST(OnBEFOREUPDATE);
    TIDY_ATTR_CONST(OnBLUR);
    TIDY_ATTR_CONST(OnCHANGE);
    TIDY_ATTR_CONST(OnCLICK);
    TIDY_ATTR_CONST(OnDATAAVAILABLE);
    TIDY_ATTR_CONST(OnDATASETCHANGED);
    TIDY_ATTR_CONST(OnDATASETCOMPLETE);
    TIDY_ATTR_CONST(OnDBLCLICK);
    TIDY_ATTR_CONST(OnERRORUPDATE);
    TIDY_ATTR_CONST(OnFOCUS);
    TIDY_ATTR_CONST(OnKEYDOWN);
    TIDY_ATTR_CONST(OnKEYPRESS);
    TIDY_ATTR_CONST(OnKEYUP);
    TIDY_ATTR_CONST(OnLOAD);
    TIDY_ATTR_CONST(OnMOUSEDOWN);
    TIDY_ATTR_CONST(OnMOUSEMOVE);
    TIDY_ATTR_CONST(OnMOUSEOUT);
    TIDY_ATTR_CONST(OnMOUSEOVER);
    TIDY_ATTR_CONST(OnMOUSEUP);
    TIDY_ATTR_CONST(OnRESET);
    TIDY_ATTR_CONST(OnROWENTER);
    TIDY_ATTR_CONST(OnROWEXIT);
    TIDY_ATTR_CONST(OnSELECT);
    TIDY_ATTR_CONST(OnSUBMIT);
    TIDY_ATTR_CONST(OnUNLOAD);
    TIDY_ATTR_CONST(PROFILE);
    TIDY_ATTR_CONST(PROMPT);
    TIDY_ATTR_CONST(RBSPAN);
    TIDY_ATTR_CONST(READONLY);
    TIDY_ATTR_CONST(REL);
    TIDY_ATTR_CONST(REV);
    TIDY_ATTR_CONST(RIGHTMARGIN);
    TIDY_ATTR_CONST(ROWS);
    TIDY_ATTR_CONST(ROWSPAN);
    TIDY_ATTR_CONST(RULES);
    TIDY_ATTR_CONST(SCHEME);
    TIDY_ATTR_CONST(SCOPE);
    TIDY_ATTR_CONST(SCROLLING);
    TIDY_ATTR_CONST(SELECTED);
    TIDY_ATTR_CONST(SHAPE);
    TIDY_ATTR_CONST(SHOWGRID);
    TIDY_ATTR_CONST(SHOWGRIDX);
    TIDY_ATTR_CONST(SHOWGRIDY);
    TIDY_ATTR_CONST(SIZE);
    TIDY_ATTR_CONST(SPAN);
    TIDY_ATTR_CONST(SRC);
    TIDY_ATTR_CONST(STANDBY);
    TIDY_ATTR_CONST(START);
    TIDY_ATTR_CONST(STYLE);
    TIDY_ATTR_CONST(SUMMARY);
    TIDY_ATTR_CONST(TABINDEX);
    TIDY_ATTR_CONST(TARGET);
    TIDY_ATTR_CONST(TEXT);
    TIDY_ATTR_CONST(TITLE);
    TIDY_ATTR_CONST(TOPMARGIN);
    TIDY_ATTR_CONST(TYPE);
    TIDY_ATTR_CONST(USEMAP);
    TIDY_ATTR_CONST(VALIGN);
    TIDY_ATTR_CONST(VALUE);
    TIDY_ATTR_CONST(VALUETYPE);
    TIDY_ATTR_CONST(VERSION);
    TIDY_ATTR_CONST(VLINK);
    TIDY_ATTR_CONST(VSPACE);
    TIDY_ATTR_CONST(WIDTH);
    TIDY_ATTR_CONST(WRAP);
    TIDY_ATTR_CONST(XML_LANG);
    TIDY_ATTR_CONST(XML_SPACE);
    TIDY_ATTR_CONST(XMLNS);
        
}
