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
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tidy.h"
#include "Zend/zend_API.h"
#include "Zend/zend_hash.h"
#include "safe_mode.h"

#ifdef ZEND_ENGINE_2
#include "zend_objects_API.h"
#include "zend_objects.h"
#include "zend_operators.h"
#endif

ZEND_DECLARE_MODULE_GLOBALS(tidy);

static int le_tidydoc;
#define le_tidydoc_name "Tidy Document"

#ifdef ZEND_ENGINE_2

zend_class_entry *php_tidy_ce;

static zend_object_handlers php_tidy_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	tidy_property_read,
	tidy_property_write,
	tidy_read_dim,
	tidy_write_dim,
	tidy_property_get_ptr,
	tidy_property_get_ptr,
	NULL,
	NULL,
	tidy_property_exists,
	tidy_property_delete,
	tidy_del_dim,
	tidy_get_properties,
	tidy_get_method,
	tidy_call_method,
	tidy_get_constructor,
	tidy_get_class_entry,
	tidy_get_class_name,
	tidy_objects_compare,
	tidy_object_cast
};

#endif

function_entry tidy_functions[] = {
	PHP_FE(tidy_create,	        	NULL)
	PHP_FE(tidy_setopt,             NULL)
	PHP_FE(tidy_getopt,             NULL)
	PHP_FE(tidy_parse_string,       NULL)
	PHP_FE(tidy_parse_file,         NULL)
	PHP_FE(tidy_get_output,         NULL)
	PHP_FE(tidy_get_error_buffer,   NULL)
	PHP_FE(tidy_clean_repair,       NULL)
	PHP_FE(tidy_diagnose,           NULL)
	PHP_FE(tidy_get_release,		NULL)
	PHP_FE(tidy_get_status,		  	NULL)
	PHP_FE(tidy_get_html_ver,	  	NULL)
	PHP_FE(tidy_is_xhtml,		  	NULL)
	PHP_FE(tidy_is_xml,		       	NULL)
	PHP_FE(tidy_error_count,		NULL)
	PHP_FE(tidy_warning_count,	  	NULL)
	PHP_FE(tidy_access_count,	  	NULL)
	PHP_FE(tidy_config_count,	  	NULL)
	PHP_FE(tidy_load_config,		NULL)
	PHP_FE(tidy_load_config_enc,	NULL)
	PHP_FE(tidy_set_encoding,	  	NULL)
	PHP_FE(tidy_save_config,		NULL)

#ifdef ZEND_ENGINE_2
	PHP_FE(tidy_get_root,		  	NULL)
	PHP_FE(tidy_get_head,		  	NULL)
	PHP_FE(tidy_get_html,		  	NULL)
	PHP_FE(tidy_get_body,		  	NULL)
#endif

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
	NULL,	
	NULL,
	PHP_MINFO(tidy),
#if ZEND_MODULE_API_NO >= 20010901
	"0.6b",
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_TIDY
ZEND_GET_MODULE(tidy)
#endif

#ifdef ZEND_ENGINE_2

static inline PHPTidyObj *php_tidy_fetch_object(zval *object TSRMLS_DC)
{
	
	return (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);
}

PHPTidyObj *php_tidy_new()
{

	PHPTidyObj *intern;

	intern = emalloc(sizeof(PHPTidyObj));
	intern->obj.ce = php_tidy_ce;
	intern->obj.in_get = 0;
	intern->obj.in_set = 0;
	intern->node = NULL;
	intern->attr = NULL;
	intern->type = PHP_IS_TIDYUNDEF;
	intern->tdoc = NULL;
	intern->parent = NULL;
	
	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	return intern;	
	
}

zval *_php_tidy_create_obj_zval(unsigned int objtype,
					PHPTidyObj *parent,
					void  *data
					TSRMLS_DC)
{
	zval *return_value;
	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);
	
	_php_tidy_create_obj(return_value, objtype, parent, data TSRMLS_CC);
	return return_value;
}

void _php_tidy_create_obj(zval *return_value,
				unsigned int objtype,
				PHPTidyObj *parent,
				void *data
				TSRMLS_DC)
{
	
	PHPTidyObj *retobj, *t;
	
	retobj = php_tidy_new();
	retobj->tdoc = parent->tdoc;
	retobj->type = objtype;
	retobj->refcount = 1;
	retobj->parent = parent;
	
	t = retobj;
	while((t = t->parent)) {
		t->refcount++;
	}
	
	parent->refcount++;
	
	switch(objtype) {
		case PHP_IS_TIDYNODE:
			retobj->node = (TidyNode)data;
			break;
		case PHP_IS_TIDYATTR:
			retobj->node = parent->node;
			retobj->attr = (TidyAttr)data;
			break;
		default:
			retobj->node = NULL;
			retobj->attr = NULL;
	}

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_tidy_register_object(retobj TSRMLS_CC);

}

zend_object_value php_tidy_register_object(PHPTidyObj *intern TSRMLS_DC)
{
	
	zend_object_value retval;
	
	retval.handle = zend_objects_store_put(intern,
						php_tidy_obj_dtor,
						php_tidy_obj_clone TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &php_tidy_object_handlers;

	return retval;
}

 void php_tidy_obj_dtor(void *object, zend_object_handle handle TSRMLS_DC)
 {
		
	PHPTidyObj *o = (PHPTidyObj *)object;
	
	do {
		o->refcount--;
	} while((o = o->parent));
	
	o = (PHPTidyObj *)object;
	
	if(o->refcount <= 0) {
		/* We don't free anything else here from
		   PHPTidyObj, they are all pointers
		   to internal TidyNode structs, which
		   get freed when the tidy resource is
		   destroied by TidyRelease()
		*/
		
/*		zend_hash_destroy(o->obj.properties);
		FREE_HASHTABLE(o->obj.properties); */
		
		zend_objects_destroy_object(&o->obj, handle TSRMLS_CC);
		
		o->node = NULL;
		o->attr = NULL;
		o->tdoc = NULL;
		o->type = PHP_IS_TIDYUNDEF;
		o->parent = NULL;
		efree(o);
	}
	
}

 void php_tidy_obj_clone(void *object, void **object_clone TSRMLS_DC)
 {
	
	PHPTidyObj *intern = (PHPTidyObj *) object;
	PHPTidyObj **intern_clone = (PHPTidyObj **) object_clone;

	*intern_clone = emalloc(sizeof(PHPTidyObj));
	(*intern_clone)->obj.ce = intern->obj.ce;
	(*intern_clone)->obj.in_get = 0;
	(*intern_clone)->obj.in_set = 0;
	ALLOC_HASHTABLE((*intern_clone)->obj.properties);
	
	/* memcopy these.. */
	memcpy((*intern_clone)->node, intern->node, sizeof(TidyNode));
	memcpy((*intern_clone)->attr, intern->attr, sizeof(TidyAttr));
	memcpy((*intern_clone)->tdoc, intern->tdoc, sizeof(PHPTidyDoc));
	(*intern_clone)->type = intern->type;
	
}

#endif

void * _php_tidy_mem_alloc(size_t size)
{
	return emalloc(size);
}

void * _php_tidy_mem_realloc(void *mem, size_t newsize)
{
	return erealloc(mem, newsize);
}

void _php_tidy_mem_free(void *mem)
{
	efree(mem);
}

void _php_tidy_mem_panic(ctmbstr errmsg)
{
	
	TSRMLS_FETCH();
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not allocate memory for Tidy: %s", (char *)errmsg);
}

 
void dtor_TidyDoc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    
	PHPTidyDoc *tdoc = (PHPTidyDoc *)rsrc->ptr;
	    
	if(tdoc->doc) {
		    tidyRelease(tdoc->doc);
	}
	if(tdoc->errbuf) {
		    tidyBufFree(tdoc->errbuf);
		    efree(tdoc->errbuf);
	
	}
	
	efree(tdoc);
    
}

void php_tidy_init_globals(zend_tidy_globals *tidy_globals)
{
	
	/* No globals for now */
}

PHP_MINIT_FUNCTION(tidy)
{

#ifdef ZEND_ENGINE_2

	zend_class_entry _tidy_entry;
    
	INIT_CLASS_ENTRY(_tidy_entry, "TidyObject", NULL);
	php_tidy_ce = zend_register_internal_class(&_tidy_entry TSRMLS_CC);

#endif

	ZEND_INIT_MODULE_GLOBALS(tidy, php_tidy_init_globals, NULL);
	le_tidydoc = zend_register_list_destructors_ex(dtor_TidyDoc, NULL, le_tidydoc_name, module_number);
    
	_php_tidy_register_tags(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_attributes(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_nodetypes(INIT_FUNC_ARGS_PASSTHRU);
	
	tidySetMallocCall(_php_tidy_mem_alloc);
	tidySetReallocCall(_php_tidy_mem_realloc);
	tidySetFreeCall(_php_tidy_mem_free);
	tidySetPanicCall(_php_tidy_mem_panic);
	
	return SUCCESS;

}


PHP_MSHUTDOWN_FUNCTION(tidy)
{
    return SUCCESS;
}



PHP_MINFO_FUNCTION(tidy)
{
    
	php_info_print_table_start();
	php_info_print_table_header(2, "Tidy support", "enabled");
	php_info_print_table_row(2, "Tidy Build Date", (char *)tidyReleaseDate());
	php_info_print_table_end();

}

/* {{{ proto resource tidy_create()
   Initialize a new tidy document */
PHP_FUNCTION(tidy_create)
{
    
	PHPTidyDoc *tdoc;
	tdoc = emalloc(sizeof(PHPTidyDoc));
	tdoc->doc = tidyCreate();
	tdoc->parsed = 0;
	
	tdoc->errbuf = emalloc(sizeof(TidyBuffer));
	tidyBufInit(tdoc->errbuf);
	
	if(tidySetErrorBuffer(tdoc->doc, tdoc->errbuf) != 0) 
	    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not set Tidy error buffer");
	
	tidyOptSetBool(tdoc->doc, TidyForceOutput, yes);
	    tidyOptSetBool(tdoc->doc, TidyMark, no);
		    
	ZEND_REGISTER_RESOURCE(return_value, tdoc, le_tidydoc);
    
}
/* }}} */

/* {{{ proto void tidy_parse_string(resource tidy, string input)
   Parse a document stored in a string */
PHP_FUNCTION(tidy_parse_string)
{
    
	char *input;
	int input_len;
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rs", &res, &input, &input_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(tidyParseString(tdoc->doc, input) < 0) {
	    php_error_docref(NULL TSRMLS_CC, E_ERROR, "[Tidy error] %s", tdoc->errbuf->bp);
	}
	
	tdoc->parsed = 1;
    
}
/* }}} */

/* {{{ proto string tidy_get_error_buffer(resource tidy [, boolean detailed])
   Return warnings and errors which occured parsing the specified document*/
PHP_FUNCTION(tidy_get_error_buffer)
{

	zval *res;
	PHPTidyDoc *tdoc;
	zend_bool detailed;
	
	if((ZEND_NUM_ARGS() == 0) || (ZEND_NUM_ARGS() > 2)) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r|b", &res, &detailed) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	if(detailed) {
	    tidyErrorSummary(tdoc->doc);
	}
	
	RETVAL_STRING(tdoc->errbuf->bp, 1);
	
	tidyBufClear(tdoc->errbuf); 
    
}
/* }}} */

/* {{{ proto string tidy_get_output(resource tidy) 
   Return a string representing the parsed tidy markup */
PHP_FUNCTION(tidy_get_output)
{
    
	zval *res;
	PHPTidyDoc *tdoc;
	TidyBuffer output = {0};
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	tidySaveBuffer( tdoc->doc, &output );
	
	RETVAL_STRING(output.bp, 1);
	
	tidyBufFree(&output);
    
}
/* }}} */

/* {{{ proto boolean tidy_parse_file(resource tidy, string file)
   Parse markup in file or URI */
PHP_FUNCTION(tidy_parse_file)
{
    
	char *inputfile;
	int input_len;
	zval *res;
	PHPTidyDoc *tdoc;
	php_stream *stream;
	char *contents;
	
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rs", &res, &inputfile, &input_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	stream = php_stream_open_wrapper(inputfile, "rb",
									     ENFORCE_SAFE_MODE | REPORT_ERRORS,
									     NULL);
	
	if(!stream) {
		    RETURN_FALSE;
	}
	
	    if (php_stream_copy_to_mem(stream, &contents, PHP_STREAM_COPY_ALL, 0) > 0) {
		    
		    if(tidyParseString(tdoc->doc, contents) < 0) {
			    php_error_docref(NULL TSRMLS_CC, E_ERROR, "[Tidy error] %s", tdoc->errbuf->bp);
		    }
		    tdoc->parsed = TRUE;
		    efree(contents);
	    }
	
	    php_stream_close(stream);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_clean_repair(resource tidy)
   Execute configured cleanup and repair operations on parsed markup */
PHP_FUNCTION(tidy_clean_repair)
{
    
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	   WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	if(tidyCleanAndRepair(tdoc->doc) >= 0) RETURN_TRUE;
	
	RETURN_FALSE;
        
}
/* }}} */

/* {{{ proto boolean tidy_diagnose(resource tidy)
    Run configured diagnostics on parsed and repaired markup. */
PHP_FUNCTION(tidy_diagnose)
{
    
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	if(tidyRunDiagnostics(tdoc->doc) >= 0) RETURN_TRUE;
	
	RETURN_FALSE;    
}

/* }}} */

/* {{{ proto string tidy_get_release()
    Get release date (version) for Tidy library */
PHP_FUNCTION(tidy_get_release)
{

	RETURN_STRING((char *)tidyReleaseDate(), 1);    
}
/* }}} */

/* {{{ proto int tidy_get_status(resource tidy)
    Get status of specfied document. */
PHP_FUNCTION(tidy_get_status)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	RETURN_LONG(tidyStatus(tdoc->doc));
    
}
/* }}} */

/* {{{ proto int tidy_get_html_ver(resource tidy)
    Get the Detected HTML version for the specified document. */
PHP_FUNCTION(tidy_get_html_ver)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	
	RETURN_LONG(tidyDetectedHtmlVersion(tdoc->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml(resource tidy)
    Indicates if the document is a XHTML document. */
PHP_FUNCTION(tidy_is_xhtml)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	RETURN_BOOL(tidyDetectedXhtml(tdoc->doc));	
	
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml(resource tidy)
    Indicates if the document is a generic (non HTML/XHTML) XML document. */
PHP_FUNCTION(tidy_is_xml)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	RETURN_BOOL(tidyDetectedGenericXml(tdoc->doc));	
}
/* }}} */

/* {{{ proto int tidy_error_count(resource tidy)
    Returns the Number of Tidy errors encountered for specified document. */
PHP_FUNCTION(tidy_error_count)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	RETURN_LONG(tidyErrorCount(tdoc->doc));	
}
/* }}} */

/* {{{ proto int tidy_warning_count(resource tidy)
    Returns the Number of Tidy warnings encountered for specified document. */
PHP_FUNCTION(tidy_warning_count)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	RETURN_LONG(tidyWarningCount(tdoc->doc));	
}
/* }}} */

/* {{{ proto int tidy_access_count(resource tidy)
    Returns the Number of Tidy accessibility warnings encountered for specified document. */
PHP_FUNCTION(tidy_access_count)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	RETURN_LONG(tidyAccessWarningCount(tdoc->doc));	
		
}
/* }}} */

/* {{{ proto int tidy_config_count(resource tidy)
    Returns the Number of Tidy configuration errors encountered for specified document. */
PHP_FUNCTION(tidy_config_count)
{
	zval *res;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	
	RETURN_LONG(tidyConfigErrorCount(tdoc->doc));	
	
}
/* }}} */

/* {{{ proto void tidy_load_config(resource tidy, string filename)
    Load an ASCII Tidy configuration file */
PHP_FUNCTION(tidy_load_config)
{
	zval *res;
	char *filename;
	int filename_len;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rs", &res, &filename, &filename_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	if(tidyLoadConfig(tdoc->doc, filename) < 0) {
	     php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not load configuration file '%s'", filename);
	}
    
}
/* }}} */

/* {{{ proto void tidy_load_config(resource tidy, string filename, string encoding)
    Load an ASCII Tidy configuration file with the specified encoding */
PHP_FUNCTION(tidy_load_config_enc)
{
	zval *res;
	char *filename;
	char *encoding;
	int enc_len, file_len;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rss", &res, &filename, &file_len, &encoding, &enc_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	if(tidyLoadConfigEnc(tdoc->doc, filename, encoding) < 0) {
	     php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s' using encoding '%s'", filename, encoding);
	     RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_set_encoding(resource tidy, string encoding)
    Set the input/output character encoding for parsing markup.
    Values include: ascii, latin1, raw, utf8, iso2022, mac, win1252, utf16le,
    utf16be, utf16, big5 and shiftjis. */
PHP_FUNCTION(tidy_set_encoding)
{
	zval *res;
	char *encoding;
	int enc_len;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rs", &res, &encoding, &enc_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	if(tidySetCharEncoding(tdoc->doc, encoding) < 0) {
	     php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not set encoding '%s'", encoding);
	     RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_save_config(resource tidy, string filename)
    Save current settings to named file. Only non-default values are written. */
PHP_FUNCTION(tidy_save_config)
{
	zval *res;
	char *filename;
	int file_len;
	PHPTidyDoc *tdoc;
	
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rs", &res, &filename, &file_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	if(tidyOptSaveFile(tdoc->doc, filename) < 0) {
	     php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not write tidy configuration file '%s'", filename);
	     RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_setopt(resource tidy, string option, mixed newvalue)
    Updates the configuration settings for the specified tidy document. */
PHP_FUNCTION(tidy_setopt)
{
	
	zval *res, *value;
	char *optname;
	int optname_len;
	PHPTidyDoc *tdoc;
	TidyOption opt;
	
	
	if(ZEND_NUM_ARGS() != 3) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rsz", &res, &optname, &optname_len, &value) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->doc) {
		    php_error_docref(NULL TSRMLS_CC,E_ERROR, "Invalid Tidy Resource Specified");
		    RETURN_FALSE;
	}
	
	opt = tidyGetOptionByName(tdoc->doc, optname);
	    if(!opt) {
		    php_error_docref(NULL TSRMLS_CC,E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		    RETURN_FALSE;
	    }
	    
	switch(tidyOptGetType(opt)) {
		    
		case TidyString:
			if(tidyOptSetValue(tdoc->doc, tidyOptGetId(opt), Z_STRVAL_P(value))) {
				RETURN_TRUE;
			} 
			break;
			
		case TidyInteger:
			if(tidyOptSetInt(tdoc->doc, tidyOptGetId(opt), Z_LVAL_P(value))) {
				RETURN_TRUE;
			} 
			break;
			
		case TidyBoolean:
			if(tidyOptSetBool(tdoc->doc,  tidyOptGetId(opt), Z_LVAL_P(value))) {
				RETURN_TRUE;
			} 
			break;
			
		default:
			
			php_error_docref(NULL TSRMLS_CC,E_WARNING, "Unable to determine type of Tidy configuration constant to set");
			    
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed tidy_getopt(resource tidy, string option)
    Returns the value of the specified configuration option for the tidy document. */
PHP_FUNCTION(tidy_getopt)
{
	
	zval *res;
	char *optname, *strval;
	int optname_len;
	PHPTidyDoc *tdoc;
	    TidyOption opt;
	    
	if(ZEND_NUM_ARGS() != 2) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "rs", &res, &optname, &optname_len) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->doc) {
		    php_error_docref(NULL TSRMLS_CC,E_ERROR, "Invalid Tidy Resource Specified");
		    RETURN_FALSE;
	}
	
	opt = tidyGetOptionByName(tdoc->doc, optname);
	    if(!opt) {
		    php_error_docref(NULL TSRMLS_CC,E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		    RETURN_FALSE;
	    }
	    
	switch(tidyOptGetType(opt)) {
		    
		    case TidyString:
			    strval = (char *)tidyOptGetValue(tdoc->doc, tidyOptGetId(opt));
			    
			    if(strval) {
				    RETURN_STRING(strval, 0);
			    } else {
				    RETURN_EMPTY_STRING();
			    }
			    
			    break;
		    case TidyInteger:
			    RETURN_LONG(tidyOptGetInt(tdoc->doc, tidyOptGetId(opt)))
			    break;
		    case TidyBoolean:	
						    
			    if(tidyOptGetBool(tdoc->doc, tidyOptGetId(opt))) {
				    RETURN_TRUE;
			    }
			    
			    break;
			    
		    default:
			    
			    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of Tidy configuration constant to get");
			    
	}
	
	RETURN_FALSE;
	
}
/* }}} */

#ifdef ZEND_ENGINE_2

/* {{{ proto TidyNode tidy_get_root(resource tidy)
    Returns a TidyNode Object representing the root of the tidy parse tree */
PHP_FUNCTION(tidy_get_root)
{
    
	zval *res;
	PHPTidyDoc *tdoc;
	    PHPTidyObj *obj;
	    
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	obj = php_tidy_new();
	obj->node = tidyGetRoot(tdoc->doc);
	obj->tdoc = tdoc;
	obj->attr = NULL;
	obj->type = PHP_IS_TIDYNODE;
	
	return_value->type = IS_OBJECT;
	return_value->value.obj = php_tidy_register_object(obj TSRMLS_CC);	
	
}
/* }}} */

/* {{{ proto TidyNode tidy_get_html(resource tidy)
    Returns a TidyNode Object starting from the <HTML> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_html)
{
    
	zval *res;
	PHPTidyDoc *tdoc;
	    PHPTidyObj *obj;
	    
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	obj = php_tidy_new();
	obj->node = tidyGetHtml(tdoc->doc);
	obj->tdoc = tdoc;
	obj->attr = NULL;
	obj->type = PHP_IS_TIDYNODE;
	
	return_value->type = IS_OBJECT;
	return_value->value.obj = php_tidy_register_object(obj TSRMLS_CC);
	
}
/* }}} */

/* {{{ proto TidyNode tidy_get_head(resource tidy)
    Returns a TidyNode Object starting from the <HEAD> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_head)
{
    
	zval *res;
	PHPTidyDoc *tdoc;
	    PHPTidyObj *obj;
	    
	if(ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				 "r", &res) == FAILURE) {
	    return;
	}
	
	ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);
	
	if(!tdoc->parsed) {
		    php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		    RETURN_FALSE;
	}
	
	obj = php_tidy_new();
	obj->node = tidyGetHead(tdoc->doc);
	obj->tdoc = tdoc;
	obj->attr = NULL;
	obj->type = PHP_IS_TIDYNODE;
	
	return_value->type = IS_OBJECT;
	return_value->value.obj = php_tidy_register_object(obj TSRMLS_CC);	
	
}
/* }}} */

/* {{{ proto TidyNode tidy_get_body(resource tidy)
    Returns a TidyNode Object starting from the <BODY> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_body)
{
    
    zval *res;
    PHPTidyDoc *tdoc;
	PHPTidyObj *obj;
	
    if(ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }
    
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                             "r", &res) == FAILURE) {
        return;
    }
    
    ZEND_FETCH_RESOURCE(tdoc, PHPTidyDoc *, &res, -1, le_tidydoc_name, le_tidydoc);

    if(!tdoc->parsed) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function.");
		RETURN_FALSE;
    }
    
    obj = php_tidy_new();
    obj->node = tidyGetBody(tdoc->doc);
    obj->tdoc = tdoc;
    obj->attr = NULL;
    obj->type = PHP_IS_TIDYNODE;
    
    return_value->type = IS_OBJECT;
	return_value->value.obj = php_tidy_register_object(obj TSRMLS_CC);	
	
}
/* }}} */

void tidy_property_delete(zval *obj, zval *member TSRMLS_DC) {}
void tidy_property_write(zval *obj, zval *member, zval *value TSRMLS_DC) {}
void tidy_object_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC) {}
void tidy_write_dim(zval *object, zval *offset, zval *value TSRMLS_DC) {}
void tidy_del_dim(zval *object, zval *offset TSRMLS_DC) {}

union _zend_function * tidy_get_constructor(zval *obj TSRMLS_DC)
{
	return NULL;
}

zval *tidy_read_dim(zval *object, zval *offset TSRMLS_DC)
{
	
	return EG(uninitialized_zval_ptr);
}

zend_class_entry * tidy_get_class_entry(zval *obj TSRMLS_DC)
{
	
	return php_tidy_ce;
}


zval ** tidy_property_get_ptr(zval *obj, zval *member TSRMLS_DC)
{
	zval **p_ptr = NULL;
	zval  *p;
	
	p = tidy_property_read(obj, member, 0 TSRMLS_CC);

	*p_ptr = p;
	
	return p_ptr;
	
}

zval * tidy_property_read(zval *object, zval *member, zend_bool silent TSRMLS_DC)
{
	
	PHPTidyObj *obj = php_tidy_fetch_object(object TSRMLS_CC);
	zval *return_value, *temp;
	TidyBuffer buf;
	TidyNode tempnode;
	TidyAttr tempattr;
	char *temp_str;
	
	char *name = Z_STRVAL_P(member);
	   
	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	/* Seems to me the engine expects to simply recieve a pointer to
	   an already-existing zval, not for one to be created and returned..
	   
	   Thus, it doesn't feel compelled to free the return value once it's
	   done with it... this seems to compell it appropiately. */
	return_value->refcount--;
	
	switch(obj->type) {
		
		case PHP_IS_TIDYNODE:
					
			if(!strcmp(name, "name")) {
				temp_str = (char *)tidyNodeGetName(obj->node);
				if(temp_str) {
					ZVAL_STRING(return_value, temp_str, 1);
				} 
				
			} else if(!strcmp(name, "value")) {
				
				memset(&buf, 0, sizeof(buf));
				tidyNodeGetText(obj->tdoc->doc, obj->node, &buf);
				ZVAL_STRING(return_value, (char *)buf.bp, 1);
				
				/* The buffer adds a newline at the end of the string */
				REMOVE_NEWLINE(return_value);
				
				tidyBufFree(&buf);
						
			} else if(!strcmp(name, "type")) {
				
				ZVAL_LONG(return_value, tidyNodeGetType(obj->node));
						  
			} else if(!strcmp(name, "id")) {
				
				if(tidyNodeGetName(obj->node)) {
					ZVAL_LONG(return_value, tidyNodeGetId(obj->node));
				}
						  
			} else if(!strcmp(name, "attribs")) {
				
				array_init(return_value);
				
				tempattr = tidyAttrFirst(obj->node);
				
				if(tempattr) {
					
					temp = _php_tidy_create_obj_zval(PHP_IS_TIDYATTR, obj, tempattr TSRMLS_CC);
					add_next_index_zval(return_value, temp);
					while((tempattr = tidyAttrNext(tempattr))) {
						
						temp = _php_tidy_create_obj_zval(PHP_IS_TIDYATTR, obj, tempattr TSRMLS_CC);
						add_next_index_zval(return_value, temp);
					}
				}
				
			} else if(!strcmp(name, "children")) {
				
				array_init(return_value);
				tempnode = tidyGetChild(obj->node);
				if(tempnode) {
					
					temp = _php_tidy_create_obj_zval(PHP_IS_TIDYNODE, obj, tempnode TSRMLS_CC);
					add_next_index_zval(return_value, temp);
					while((tempnode = tidyGetNext(tempnode))) {
						
						temp = _php_tidy_create_obj_zval(PHP_IS_TIDYNODE, obj, tempnode TSRMLS_CC);					
						add_next_index_zval(return_value, temp);
					}
				}
				
			} else if(!strcmp(name, "line")) {
				ZVAL_LONG(return_value, tidyNodeLine(obj->node));
			} else if(!strcmp(name, "column")) {
				ZVAL_LONG(return_value, tidyNodeColumn(obj->node));
			} else if(!strcmp(name, "html_ver")) {
				ZVAL_LONG(return_value, tidyDetectedHtmlVersion(obj->tdoc->doc));
			}
			
			break;
			
		case PHP_IS_TIDYATTR:
			
			if(!strcmp(name, "name")) {
				temp_str = (char *)tidyAttrName(obj->attr);
				if(temp_str) {
					ZVAL_STRING(return_value, temp_str , 1);
				}
			} else if(!strcmp(name, "value")) {
				temp_str = (char *)tidyAttrValue(obj->attr);
				if(temp_str) {
					ZVAL_STRING(return_value, temp_str , 1);
					efree(temp_str);
				}
			} else if(!strcmp(name, "id")) {
				ZVAL_LONG(return_value,	tidyAttrGetId(obj->attr));
			}
			
			break;
		default:
			php_error_docref(NULL TSRMLS_CC,E_ERROR, "Undefined Tidy object type.");
			break;
	}
	
	return return_value;
			
}

int tidy_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
		
	return TRUE;
}

HashTable * tidy_get_properties(zval *object TSRMLS_DC)
{
		
	zend_object *zobj;
	zobj = zend_objects_get_address(object TSRMLS_CC);
	return zobj->properties;
	
}

union _zend_function * tidy_get_method(zval *obj, char *method, int method_len TSRMLS_DC)
{
	
	zend_internal_function *f;

	f = emalloc(sizeof(zend_internal_function));
	f->type = ZEND_OVERLOADED_FUNCTION;
	/* f->arg_types = NULL; */
	f->scope = php_tidy_ce;
	f->fn_flags = 0;
	f->function_name = estrndup(method, method_len);

	return (union _zend_function *) f;
}

zend_bool _php_tidy_node_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{

	PHPTidyObj *obj = php_tidy_fetch_object(getThis() TSRMLS_CC);
	zend_internal_function *func = (zend_internal_function *)EG(function_state_ptr)->function;
	PHPTidyObj *newobj;
	TidyNode tempnode;
	TidyAttr tempattr;
	zend_bool retval = TRUE;
	
	int param;
	
	if(strstr(method, "has_")) {
	
		if(!strcmp(method, "has_siblings")) {
			
			if(tidyGetNext(obj->node) || tidyGetPrev(obj->node)) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
					
		} else if(!strcmp(method, "has_children")) {
		
			if(tidyGetChild(obj->node)) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
				
		} else if(!strcmp(method, "has_parent")) {
			
			if(tidyGetParent(obj->node)) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		}		
	
	} else if(strstr(method, "is_")) {

		if(!strcmp(method, "is_comment")) {
		
			if(tidyNodeGetType(obj->node) == TidyNode_Comment) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_xhtml")) {
		
			if(tidyDetectedXhtml(obj->tdoc->doc)) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_xml")) {
		
			if(tidyDetectedGenericXml(obj->tdoc->doc)) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_text")) {
		
			if(tidyNodeGetType(obj->node) == TidyNode_Text) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_jste")) {
		
			if(tidyNodeGetType(obj->node) == TidyNode_Jste) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_asp")) {
		
			if(tidyNodeGetType(obj->node) == TidyNode_Asp) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_php")) {
		
			if(tidyNodeGetType(obj->node) == TidyNode_Php) {
				TIDY_RV_TRUE(return_value);
			} else {
				TIDY_RV_FALSE(return_value);
			}
		
		} else if(!strcmp(method, "is_html")) {
				
			switch(tidyNodeGetType(obj->node)) {
				
				case TidyNode_Start:
				case TidyNode_End:
				case TidyNode_StartEnd:
					TIDY_RV_TRUE(return_value);
					break;
				default:
					TIDY_RV_FALSE(return_value);
					break;
			}
	
		}
				
	} else {
		
		if(!strcmp(method, "next")) {
				
				tempnode = tidyGetNext(obj->node);
				if(tempnode) {
					_php_tidy_create_obj(return_value,
										PHP_IS_TIDYNODE,
										obj,
										tempnode TSRMLS_CC);
				} 
							
		} else if(!strcmp(method, "prev")) {

				tempnode = tidyGetPrev(obj->node);
				if(tempnode) {
					_php_tidy_create_obj(return_value,
										 PHP_IS_TIDYNODE,
										 obj,
										 tempnode TSRMLS_CC);
				} 
				
		} else if(!strcmp(method, "parent")) {

				tempnode = tidyGetParent(obj->node);
				if(tempnode) {
					_php_tidy_create_obj(return_value,
											  PHP_IS_TIDYNODE,
											  obj,
											  tempnode TSRMLS_CC);
				} 
				
		} else if(!strcmp(method, "child")) {

				tempnode = tidyGetChild(obj->node);
				if(tempnode) {
					_php_tidy_create_obj(return_value,
											  PHP_IS_TIDYNODE,
											  obj,
											  tempnode TSRMLS_CC);
				} 
		
		} else if(!strcmp(method, "get_attr_type")) {
			
			if(ZEND_NUM_ARGS() != 1) {
				zend_wrong_param_count(TSRMLS_C);
				return TRUE;
			}
			
			if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                             "l", &param) == FAILURE) {
				return FALSE;
			}
			
			
			newobj = php_tidy_new();
			newobj->tdoc = obj->tdoc;
			
			for(tempattr = tidyAttrFirst(obj->node);
				tempattr;
				tempattr = tidyAttrNext(tempattr)) {
				
				if(tidyAttrGetId(tempattr) == param) {
					
					newobj->attr = tempattr;
					newobj->type = PHP_IS_TIDYATTR;
					obj->refcount++;
					
					return_value->type = IS_OBJECT;
					return_value->value.obj = php_tidy_register_object(newobj TSRMLS_CC);
					
					break;
			
				}
				
			}
						
		} else {
			retval = FALSE;

		}
	}
	efree(func->function_name);
	efree(func);
	return retval;
	
}

zend_bool _php_tidy_attr_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	
	PHPTidyObj *obj = php_tidy_fetch_object(getThis() TSRMLS_CC);
	TidyAttr tempattr;
	
	if(!strcmp(method, "next")) {
		
		tempattr = tidyAttrNext(obj->attr);
		
		if(tempattr) {
			_php_tidy_create_obj(return_value, PHP_IS_TIDYATTR, obj, tempattr TSRMLS_CC);
		} else {
			
			TIDY_RV_FALSE(return_value);
		
		}
		
	} else if(!strcmp(method, "tag")) {
		_php_tidy_create_obj(return_value, PHP_IS_TIDYNODE, obj, obj->node TSRMLS_CC);
	} else {
		return FALSE;
	}
	
	return TRUE;
}

int tidy_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	
	PHPTidyObj *obj = php_tidy_fetch_object(getThis() TSRMLS_CC);

	switch(obj->type) {
		
		case PHP_IS_TIDYNODE:
			return _php_tidy_node_call_method(method, INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case PHP_IS_TIDYATTR:
			return _php_tidy_attr_call_method(method, INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Undefined Tidy object type.");
			break;
	}
	
	return FALSE;
}

int tidy_get_class_name(zval *obj, char **class_name, zend_uint *name_len, int parent TSRMLS_DC)
{
	
	PHPTidyObj *object = php_tidy_fetch_object(obj TSRMLS_CC);
	
	switch(object->type) {
		
		case PHP_IS_TIDYNODE:
	
			*class_name = estrdup("Tidy_Node");
			*name_len = sizeof("Tidy_Node");
	
			break;
		case PHP_IS_TIDYATTR:
	
			*class_name = estrdup("Tidy_Attribute");
			*name_len = sizeof("Tidy_Attribute");
	
			break;
		default:
			
			*class_name = estrdup("Tidy_Unknown");
			*name_len = sizeof("Tidy_Unknown");
			break;
	}
	
	return TRUE;
}

int tidy_objects_compare(zval *obj_one, zval *obj_two TSRMLS_DC)
{

	PHPTidyObj *obj1, *obj2;
	
	obj1 = php_tidy_fetch_object(obj_one TSRMLS_CC);
	obj2 = php_tidy_fetch_object(obj_two TSRMLS_CC);
	
	if( (obj1->tdoc == obj2->tdoc) &&
		(obj1->node == obj2->node) &&
		(obj1->attr == obj2->attr) &&
		(obj1->type == obj2->type)) {
		
		return TRUE;
		
	}
	
	return FALSE;
	
}

#endif

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
