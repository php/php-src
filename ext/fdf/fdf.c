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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   |          Hartmut Holzgraefe <hartmut@six.de>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* FdfTk lib 2.0 is a Complete C/C++ FDF Toolkit available from
   http://beta1.adobe.com/ada/acrosdk/forms.html. */

/* Note that there is no code from the FdfTk lib in this file */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_FDFLIB

#include "SAPI.h"
#include "ext/standard/info.h"
#include "php_open_temporary_file.h"
#include "php_variables.h"
#include "php_fdf.h"

static int le_fdf;

SAPI_POST_HANDLER_FUNC(fdf_post_handler);

/* {{{ fdf_functions[]
 */
function_entry fdf_functions[] = {
	PHP_FE(fdf_open,								NULL)
	PHP_FE(fdf_create,								NULL)
	PHP_FE(fdf_close,								NULL)
	PHP_FE(fdf_save,								NULL)
	PHP_FE(fdf_get_value,							NULL)
	PHP_FE(fdf_set_value,							NULL)
	PHP_FE(fdf_next_field_name,						NULL)
	PHP_FE(fdf_set_ap,								NULL)
	PHP_FE(fdf_set_status,							NULL)
	PHP_FE(fdf_get_status,							NULL)
	PHP_FE(fdf_set_file,							NULL)
	PHP_FE(fdf_get_file,							NULL)
	PHP_FE(fdf_add_template,						NULL)
	PHP_FE(fdf_set_flags,							NULL)
	PHP_FE(fdf_set_opt,								NULL)
	PHP_FE(fdf_set_submit_form_action,				NULL)
	PHP_FE(fdf_set_javascript_action,				NULL)
	PHP_FE(fdf_set_encoding,						NULL)
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry fdf_module_entry = {
    STANDARD_MODULE_HEADER,
	"fdf", 
	fdf_functions, 
	PHP_MINIT(fdf), 
	PHP_MSHUTDOWN(fdf), 
	NULL, 
	NULL,
	PHP_MINFO(fdf), 
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FDF
ZEND_GET_MODULE(fdf)
#endif


static void phpi_FDFClose(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	FDFDoc fdf = (FDFDoc)rsrc->ptr;

	(void) FDFClose(fdf);
}


#define FDF_POST_CONTENT_TYPE	"application/vnd.fdf"

static sapi_post_entry php_fdf_post_entry =	{
	FDF_POST_CONTENT_TYPE,
	sizeof(FDF_POST_CONTENT_TYPE)-1,
	sapi_read_standard_form_data,
	fdf_post_handler
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fdf)
{
	FDFErc err;
	
	le_fdf = zend_register_list_destructors_ex(phpi_FDFClose, NULL, "fdf", module_number);

	/* add handler for Acrobat FDF form post requests */
	sapi_register_post_entry(&php_fdf_post_entry);


	/* Constants used by fdf_set_opt() */
	REGISTER_LONG_CONSTANT("FDFValue", FDFValue, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFStatus", FDFStatus, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFFile", FDFFile, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFID", FDFID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFFf", FDFFf, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFSetFf", FDFSetFf, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFClearFf", FDFClearFf, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFFlags", FDFFlags, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFSetF", FDFSetF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFClrF", FDFClrF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFAP", FDFAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFAS", FDFAS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFAction", FDFAction, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFAA", FDFAA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFAPRef", FDFAPRef, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFIF", FDFIF, CONST_CS | CONST_PERSISTENT);

	/* Constants used by fdf_set_javascript_action() */
	REGISTER_LONG_CONSTANT("FDFEnter", FDFEnter, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFExit", FDFExit, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFDown", FDFDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFUp", FDFUp, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFFormat", FDFFormat, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFValidate", FDFValidate, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFKeystroke", FDFKeystroke, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFCalculate", FDFCalculate, CONST_CS | CONST_PERSISTENT);

#ifdef PHP_WIN32
	return SUCCESS;
#endif
	if((err = FDFInitialize()) == FDFErcOK) return SUCCESS;
	return FAILURE;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fdf)
{
	/* need to use a PHPAPI function here because it is external module in windows */
	php_info_print_table_start();
	php_info_print_table_row(2, "FDF Support", "enabled");
	php_info_print_table_row(2, "FdfTk Version", FDFGetVersion() );
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(fdf)
{
	FDFErc err;

	/* remove handler for Acrobat FDF form post requests */
	sapi_unregister_post_entry(&php_fdf_post_entry); 

#ifdef PHP_WIN32
	return SUCCESS;
#endif
	if((err = FDFFinalize()) == FDFErcOK) return SUCCESS;
	return FAILURE;
}
/* }}} */

/* {{{ proto int fdf_open(string filename)
   Opens a new FDF document */
PHP_FUNCTION(fdf_open) 
{
	zval **file;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

	err = FDFOpen(Z_STRVAL_PP(file), 0, &fdf);

	if(err != FDFErcOK || !fdf) {
		php_error(E_WARNING, "%s(): Could not open fdf document: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(file));
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, fdf, le_fdf);
} 
/* }}} */

/* {{{ proto int fdf_create(void)
   Creates a new FDF document */
PHP_FUNCTION(fdf_create) 
{
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	err = FDFCreate(&fdf);

	if(err != FDFErcOK || !fdf) {
		php_error(E_WARNING, "%s(): Error creating new fdf document!", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, fdf, le_fdf);
}
/* }}} */

/* {{{ proto bool fdf_close(int fdfdoc)
   Closes the FDF document */
PHP_FUNCTION(fdf_close) 
{
	zval **fdfp;
	FDFDoc fdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fdfp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);
	zend_list_delete(Z_RESVAL_PP(fdfp));
} 
/* }}} */

/* {{{ proto string fdf_get_value(int fdfdoc, string fieldname)
   Gets the value of a field as string */
PHP_FUNCTION(fdf_get_value) 
{
	zval **fdfp, **fieldname;
	ASInt32 nr, size = 256;
	char *buffer;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fdfp, &fieldname) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);

	buffer = emalloc(size);
	err = FDFGetValue(fdf, Z_STRVAL_PP(fieldname), buffer, size-1, &nr);
	if(err == FDFErcBufTooShort && nr > 0 ) {
		buffer = erealloc(buffer, nr+1); 
		err = FDFGetValue(fdf, Z_STRVAL_PP(fieldname), buffer, nr, &nr);
	} 

	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error getting value of %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname));
		efree(buffer);
		RETURN_FALSE;
	}

	RETVAL_STRING(buffer, 1);
	efree(buffer);
}
/* }}} */

/* {{{ proto bool fdf_set_value(int fdfdoc, string fieldname, string value, int isname)
   Sets the value of a field */
PHP_FUNCTION(fdf_set_value) 
{
	zval **fdfp, **fieldname, **value, **isname;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fdfp, &fieldname, &value, &isname) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);
	convert_to_string_ex(value);
	convert_to_long_ex(isname);

	err = FDFSetValue(fdf, Z_STRVAL_PP(fieldname), Z_STRVAL_PP(value), (ASBool) Z_LVAL_PP(isname));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting field: %s to value: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname), Z_STRVAL_PP(value));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fdf_next_field_name(int fdfdoc [, string fieldname])
   Gets the name of the next field name or the first field name */
PHP_FUNCTION(fdf_next_field_name) 
{
	zval **fdfp, **field;
	int argc=ZEND_NUM_ARGS();
	ASInt32 length=256, nr;
	char *buffer=NULL, *fieldname=NULL;
	FDFDoc fdf;
	FDFErc err;

	if (argc > 2 || argc < 1 || zend_get_parameters_ex(argc, &fdfp, &field) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	if(argc == 2) {
		convert_to_string_ex(field);
		fieldname = Z_STRVAL_PP(field);
	}

	buffer = emalloc(length);
	err = FDFNextFieldName(fdf, fieldname, buffer, length-1, &nr);

	if(err == FDFErcBufTooShort && nr > 0 ) {
		buffer = erealloc(buffer, nr+1); 
		err = FDFNextFieldName(fdf, fieldname, buffer, length-1, &nr);
	} 

	if(err != FDFErcOK) {
		efree(buffer);
		php_error(E_WARNING, "%s(): Error getting next fieldname!", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	} 

	RETVAL_STRING(buffer, 1);
	efree(buffer);
}
/* }}} */

/* {{{ proto bool fdf_set_ap(int fdfdoc, string fieldname, int face, string filename, int pagenr)
   Sets the appearence of a field */
PHP_FUNCTION(fdf_set_ap) 
{
	zval **fdfp, **fieldname, **face, **filename, **pagenr;
	FDFDoc fdf;
	FDFErc err;
	FDFAppFace facenr;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &fdfp, &fieldname, &face, &filename, &pagenr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);
	convert_to_long_ex(face);
	convert_to_string_ex(filename);
	convert_to_long_ex(pagenr);

	switch(Z_LVAL_PP(face)) {
		case 1:
			facenr = FDFNormalAP;
			break;
		case 2:
			facenr = FDFRolloverAP;
			break;
		case 3:
			facenr = FDFDownAP;
			break;
		default:
			facenr = FDFNormalAP;
	}

	err = FDFSetAP(fdf, Z_STRVAL_PP(fieldname), facenr, NULL, Z_STRVAL_PP(filename), (ASInt32) Z_LVAL_PP(pagenr));

	/* This should be made more intelligent, ie. use switch() with the 
	   possible errors this function can return. Or create global error handler function.
	 */
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting appearence of field: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname));
		RETURN_FALSE;
	}

	RETURN_TRUE;

}
/* }}} */

/* {{{ proto bool fdf_set_status(int fdfdoc, string status)
   Sets the value of /Status key */
PHP_FUNCTION(fdf_set_status) 
{
	zval **fdfp, **status;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fdfp, &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(status);

	err = FDFSetStatus(fdf, Z_STRVAL_PP(status));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting fdf document status key to: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(status));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fdf_get_status(int fdfdoc)
   Gets the value of /Status key */
PHP_FUNCTION(fdf_get_status) 
{
	zval **fdfp;
	ASInt32 nr, size = 256;
	char *buf;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fdfp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	buf = emalloc(size);
	err = FDFGetStatus(fdf, buf, size-1,  &nr);

	if(err == FDFErcBufTooShort && nr > 0 ) {
		buf = erealloc(buf, nr+1); 
		err = FDFGetStatus(fdf, buf, size-1,  &nr);
	}
	
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error getting fdf document status key!", get_active_function_name(TSRMLS_C));
		efree(buf);
		RETURN_FALSE;
	}

	RETVAL_STRING(buf, 1);
	efree(buf);
}
/* }}} */

/* {{{ proto bool fdf_set_file(int fdfdoc, string filename)
   Sets the value of /F key */
PHP_FUNCTION(fdf_set_file) 
{
	zval **fdfp, **filename;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fdfp, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(filename);

	err = FDFSetFile(fdf, Z_STRVAL_PP(filename));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting filename key to: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(filename));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fdf_get_file(int fdfdoc)
   Gets the value of /F key */
PHP_FUNCTION(fdf_get_file) 
{
	zval **fdfp;
	ASInt32 nr, size = 256;
	char *buf;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fdfp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	buf = emalloc(size);
	err = FDFGetFile(fdf, buf, size-1,  &nr);

	if(err == FDFErcBufTooShort && nr > 0 ) {
		buf = erealloc(buf, nr+1); 
		err = FDFGetFile(fdf, buf, size-1,  &nr);
	}
	
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error getting fdf document filename key!", get_active_function_name(TSRMLS_C));
		efree(buf);
		RETURN_FALSE;
	}

	RETVAL_STRING(buf, 1);
	efree(buf);
}
/* }}} */

/* {{{ proto bool fdf_save(int fdfdoc, string filename)
   Writes out the FDF file */
PHP_FUNCTION(fdf_save) 
{
	zval **fdfp, **filename;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fdfp, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(filename);
	err = FDFSave(fdf, Z_STRVAL_PP(filename));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error saving fdf document into filename: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(filename));
		RETURN_FALSE;
	}

	RETURN_TRUE;

} 
/* }}} */

/* {{{ proto bool fdf_add_template(int fdfdoc, int newpage, string filename, string template, int rename)
   Adds a template into the FDF document */
PHP_FUNCTION(fdf_add_template) 
{
	zval **fdfp, **newpage, **filename, **template, **rename;
	FDFDoc fdf;
	FDFErc err;
	pdfFileSpecRec filespec;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &fdfp, &newpage, &filename, &template, &rename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_long_ex(newpage);
	convert_to_string_ex(filename);
	convert_to_string_ex(template);
	convert_to_long_ex(rename);

	filespec.FS = NULL;
	filespec.F = Z_STRVAL_PP(filename);
	filespec.Mac = NULL;
	filespec.DOS = NULL;
	filespec.Unix = NULL;
	filespec.ID[0] = NULL;
	filespec.ID[1] = NULL;
	filespec.bVolatile = false;

	err = FDFAddTemplate(fdf, Z_LVAL_PP(newpage), &filespec, Z_STRVAL_PP(template), Z_LVAL_PP(rename));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error adding template: %s into fdf document", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(template));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fdf_set_flags(int fdfdoc, string fieldname, int whichflags, int newflags)
   Sets flags for a field in the FDF document */
PHP_FUNCTION(fdf_set_flags) 
{
	zval **fdfp, **fieldname, **flags, **newflags;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fdfp, &fieldname, &flags, &newflags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);
	convert_to_long_ex(flags);
	convert_to_long_ex(newflags);	

	err=FDFSetFlags(fdf, Z_STRVAL_PP(fieldname), Z_LVAL_PP(flags), Z_LVAL_PP(newflags));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting flags for field: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fdf_set_opt(int fdfdoc, string fieldname, int element, string value, string name)
   Sets a value in the opt array for a field */
PHP_FUNCTION(fdf_set_opt)
{
	zval **fdfp, **fieldname, **element, **value, **name;
	FDFDoc fdf;
	FDFErc err;	

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &fdfp, &fieldname, &element, &value, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);
	convert_to_long_ex(element);
	convert_to_string_ex(value);
	convert_to_string_ex(name);

	err = FDFSetOpt(fdf, Z_STRVAL_PP(fieldname), Z_LVAL_PP(element), Z_STRVAL_PP(value), Z_STRVAL_PP(name));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting FDF option for field: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fdf_set_submit_form_action(int fdfdoc, string fieldname, int whichtrigger, string url, int flags)
   Sets the submit form action for a field */
PHP_FUNCTION(fdf_set_submit_form_action) 
{
	zval **fdfp, **fieldname, **trigger, **url, **flags;
	FDFDoc fdf;
	FDFErc err;	

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &fdfp, &fieldname, &trigger, &url, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);
	convert_to_long_ex(trigger);
	convert_to_string_ex(url);
	convert_to_long_ex(flags);

	err = FDFSetSubmitFormAction(fdf, Z_STRVAL_PP(fieldname), Z_LVAL_PP(trigger), Z_STRVAL_PP(url), Z_LVAL_PP(flags));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting FDF submit action for field: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fdf_set_javascript_action(int fdfdoc, string fieldname, int whichtrigger, string script)
   Sets the javascript action for a field */
PHP_FUNCTION(fdf_set_javascript_action) 
{
	zval **fdfp, **fieldname, **trigger, **script;
	FDFDoc fdf;
	FDFErc err;	

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fdfp, &fieldname, &trigger, &script) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);
	convert_to_long_ex(trigger);
	convert_to_string_ex(script);
	
	err = FDFSetJavaScriptAction(fdf, Z_STRVAL_PP(fieldname), Z_LVAL_PP(trigger), Z_STRVAL_PP(script));
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting FDF javascript action for field: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(fieldname));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fdf_set_encoding(int fdf_document, string encoding)
   Sets FDF encoding (either "Shift-JIS" or "Unicode") */  
PHP_FUNCTION(fdf_set_encoding) 
{
	zval **fdfp, **enc;
	FDFDoc fdf;
	FDFErc err;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fdfp, &enc) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(enc);

	err = FDFSetEncoding(fdf, Z_STRVAL_PP(enc));
    
	if(err != FDFErcOK) {
		php_error(E_WARNING, "%s(): Error setting FDF encoding: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(enc));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ SAPI_POST_HANDLER_FUNC
 * SAPI post handler for FDF forms */
SAPI_POST_HANDLER_FUNC(fdf_post_handler)
{
	FILE *fp;
	FDFDoc theFDF;
	char *name=NULL, *value=NULL, *p, *data;
	int name_len=0, value_len=0;
	char *lastfieldname =NULL;
	char *filename = NULL;
	FDFErc err;
	ASInt32 nBytes;
	zval *array_ptr = (zval *) arg;
	
	fp=php_open_temporary_file(NULL, "fdfdata.", &filename TSRMLS_CC);
	if(!fp) {
		if(filename) efree(filename);
		return;
	}
	fwrite(SG(request_info).post_data, SG(request_info).post_data_length, 1, fp);
	fclose(fp);

	/* Set HTTP_FDF_DATA variable */
	data = estrndup(SG(request_info).post_data, SG(request_info).post_data_length);
	SET_VAR_STRINGL("HTTP_FDF_DATA", data, SG(request_info).post_data_length);

 	err = FDFOpen(filename, 0, &theFDF);

	if(err==FDFErcOK){	
		name = emalloc(name_len=256);
		value= emalloc(value_len=256);
		while (1) {
			err = FDFNextFieldName(theFDF, lastfieldname, name, name_len-1, &nBytes);
			if(err == FDFErcBufTooShort && nBytes >0 ) {
				name = erealloc(name, name_len=(nBytes+1)); 
				err = FDFNextFieldName(theFDF, lastfieldname, name, name_len-1, &nBytes);
			} 
			
			if(err != FDFErcOK || nBytes == 0) break; 
			
			if(lastfieldname) efree(lastfieldname);
			lastfieldname = estrdup(name);		

			err = FDFGetValue(theFDF, name, NULL, 0, &nBytes);			
			if(err != FDFErcOK && err != FDFErcNoValue ) break; 

			if(value_len<nBytes+1) value = erealloc(value, value_len=(nBytes+1));
			
			if(nBytes>0) {
				err = FDFGetValue(theFDF, name, value, value_len-1, &nBytes);
				if(err == FDFErcOK && nBytes != 0) {
					for(p=value;*p;p++) if(*p=='\r') *p='\n';
					if(lastfieldname) efree(lastfieldname);
					lastfieldname = estrdup(name);		
					php_register_variable(name, value, array_ptr TSRMLS_CC);
				} 
			}
		}   
		
		FDFClose(theFDF);
		VCWD_UNLINK((const char *)filename);
		efree(filename);

		if(name)          efree(name);
		if(value)         efree(value);
		if(lastfieldname) efree(lastfieldname);
	} 
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
