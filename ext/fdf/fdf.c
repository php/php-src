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
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
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
#include "php_open_temporary_file.h"

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
	PHP_FE(fdf_add_template,						NULL)
	PHP_FE(fdf_close,								NULL)
	PHP_FE(fdf_create,								NULL)
	PHP_FE(fdf_enum_values,                         NULL)
	PHP_FE(fdf_errno,        						NULL)
	PHP_FE(fdf_error,        						NULL)
	PHP_FE(fdf_get_ap,								NULL)
	PHP_FE(fdf_get_encoding,						NULL)
	PHP_FE(fdf_get_file,							NULL)
	PHP_FE(fdf_get_flags,							NULL)
	PHP_FE(fdf_get_opt,								NULL)
	PHP_FE(fdf_get_status,							NULL)
	PHP_FE(fdf_get_value,							NULL)
	PHP_FE(fdf_get_version,                         NULL)
	PHP_FE(fdf_next_field_name,						NULL)
	PHP_FE(fdf_open,								NULL)
	PHP_FE(fdf_open_string,							NULL)
	PHP_FE(fdf_remove_item,                         NULL)
	PHP_FE(fdf_save,								NULL)
	PHP_FE(fdf_save_string,	        				NULL)
	PHP_FE(fdf_set_ap,								NULL)
	PHP_FE(fdf_set_encoding,						NULL)
	PHP_FE(fdf_set_file,							NULL)
	PHP_FE(fdf_set_flags,							NULL)
	PHP_FE(fdf_set_javascript_action,				NULL)
	PHP_FE(fdf_set_opt,								NULL)
	PHP_FE(fdf_set_status,							NULL)
	PHP_FE(fdf_set_submit_form_action,				NULL)
	PHP_FE(fdf_set_value,							NULL)
	PHP_FE(fdf_header,                              NULL)
#ifdef HAVE_FDFTK_5
	PHP_FE(fdf_add_doc_javascript,                  NULL)
	PHP_FE(fdf_get_attachment,                      NULL)
	PHP_FE(fdf_set_on_import_javascript,            NULL)
	PHP_FE(fdf_set_target_frame,                    NULL)
	PHP_FE(fdf_set_version,                         NULL)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry fdf_module_entry = {
    STANDARD_MODULE_HEADER,
	"fdf", 
	fdf_functions, 
	PHP_MINIT(fdf), 
	PHP_MSHUTDOWN(fdf), 
	PHP_RINIT(fdf), 
	NULL,
	PHP_MINFO(fdf), 
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FDF
ZEND_GET_MODULE(fdf)
#endif

ZEND_DECLARE_MODULE_GLOBALS(fdf)

#define FDF_SUCCESS do { FDF_G(error)=FDFErcOK; RETURN_TRUE;} while(0)
#define FDF_FAILURE(err)  do { FDF_G(error)=err; RETURN_FALSE;} while(0)

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

	/* Constants used by fdf_(set|get)_ap */
	REGISTER_LONG_CONSTANT("FDFNormalAP", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFRolloverAP", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FDFDownAP", 3, CONST_CS | CONST_PERSISTENT);
	
#ifdef PHP_WIN32
	return SUCCESS;
#endif
	if((err = FDFInitialize()) == FDFErcOK) return SUCCESS;
	return FAILURE;
}
/* }}} */

/* {{{ RINIT */
PHP_RINIT_FUNCTION(fdf) {
	FDF_G(error) = FDFErcOK;
	return SUCCESS;
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
		if(err == FDFErcOK) err= FDFErcInternalError;
		FDF_FAILURE(err);
	}

	ZEND_REGISTER_RESOURCE(return_value, fdf, le_fdf);
} 
/* }}} */

/* {{{ proto int fdf_open_string(string fdf_data)
   Opens a new FDF document from string */
PHP_FUNCTION(fdf_open_string) 
{
	char *fdf_data;
	int fdf_data_len;
	FDFDoc fdf;
	FDFErc err;
	char *temp_filename;
	FILE *fp;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							 &fdf_data, &fdf_data_len) 
	   == FAILURE) {
		return;
	}

	fp = php_open_temporary_file(PG(upload_tmp_dir), "php", &temp_filename TSRMLS_CC);
	if(!fp) {
		RETURN_FALSE;
	}
	fwrite(fdf_data, fdf_data_len, 1, fp);
	fclose(fp);

	err = FDFOpen(temp_filename, 0, &fdf);
	unlink(temp_filename);
	efree(temp_filename);

	if(err != FDFErcOK || !fdf) {
		if(err == FDFErcOK) err= FDFErcInternalError;
		FDF_FAILURE(err);
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
		if(err == FDFErcOK) err= FDFErcInternalError;
		FDF_FAILURE(err);
	}

	ZEND_REGISTER_RESOURCE(return_value, fdf, le_fdf);
}
/* }}} */

/* {{{ proto bool fdf_close(resource fdfdoc)
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

/* {{{ proto string fdf_get_value(resource fdfdoc, string fieldname [, int which])
   Gets the value of a field as string */
PHP_FUNCTION(fdf_get_value) 
{
	zval *r_fdf;
	char *fieldname;
	int fieldname_len;
	long which = -1;
	FDFDoc fdf;
	FDFErc err;
	ASInt32 nr, size = 256;
	char *buffer;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l",
							 &r_fdf, &fieldname, &fieldname_len,
							 &which) 
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	buffer = emalloc(size);
	if(which >= 0) {
#if HAVE_FDFTK_5
		err = FDFGetNthValue(fdf, fieldname, which, buffer, size-2, &nr);
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "the optional 'which' parameter requires FDF toolkit 5.0 or above, it will be ignored for now");
		which = -1;
#endif
	} else {
		err = FDFGetValue(fdf, fieldname, buffer, size-2, &nr);
	}
	if(err == FDFErcBufTooShort && nr > 0 ) {
		buffer = erealloc(buffer, nr+2); 
		if(which >= 0) {
#if HAVE_FDFTK_5
			err = FDFGetNthValue(fdf, fieldname, which, buffer, nr, &nr);
#endif
		} else {
			err = FDFGetValue(fdf, fieldname, buffer, nr, &nr);
		} 
#if HAVE_FDFTK_5
	} else if((err == FDFErcValueIsArray) && (which == -1)) {
		if (array_init(return_value) == FAILURE) {
			efree(buffer);
			FDF_FAILURE(FDFErcInternalError);
		}
		which = 0;
		do {
			err = FDFGetNthValue(fdf, fieldname, which, buffer, size-2, &nr); 
			if(err == FDFErcBufTooShort && nr > 0 ) {
				buffer = erealloc(buffer, nr+2); 
				err = FDFGetNthValue(fdf, fieldname, which, buffer, nr, &nr);
			} 
			if (err == FDFErcOK) {
				add_next_index_string(return_value, buffer, 1);
			} 
			which++;
		} while (err == FDFErcOK);
		if(err == FDFErcNoValue) err = FDFErcOK;
		efree(buffer); 
		buffer = NULL;
#endif
	}

	if(err != FDFErcOK) {
		if(buffer) efree(buffer);
		FDF_FAILURE(err);
	}

	if(buffer) {
		RETVAL_STRING(buffer, 1);
		efree(buffer);
	}

	return;
}
/* }}} */

/* {{{ proto bool fdf_set_value(resource fdfdoc, string fieldname, mixed value [, int isname])
   Sets the value of a field */
PHP_FUNCTION(fdf_set_value) 
{
	zval **fdfp, **fieldname, **value, **dummy;
	FDFDoc fdf;
	FDFErc err;

	switch(ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &fdfp, &fieldname, &value) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (zend_get_parameters_ex(4, &fdfp, &fieldname, &value, &dummy) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, fdfp, -1, "fdf", le_fdf);

	convert_to_string_ex(fieldname);

	if (Z_TYPE_PP(value) == IS_ARRAY) {
#ifdef HAVE_FDFTK_5
		ASInt32 nValues = zend_hash_num_elements(Z_ARRVAL_PP(value));
		char **newValues = ecalloc(nValues, sizeof(char *)), **next;
		HashPosition   pos;
		zval         **tmp;

		next = newValues;
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(value), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(value), 
											 (void **) &tmp,
											 &pos) == SUCCESS) {
			convert_to_string_ex(tmp);
			*next++ = estrdup(Z_STRVAL_PP(tmp));
			zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &pos);
		}

		err = FDFSetValues(fdf, Z_STRVAL_PP(fieldname), nValues, (const char **)newValues);
		
		for(next = newValues; nValues; nValues--) {
			efree(*next++);
		}
		efree(newValues);
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "setting array values is only possible with FDF toolkit 5.0 and above");
		RETURN_FALSE;
#endif
	} else {
		convert_to_string_ex(value);
		
		err = FDFSetValue(fdf, Z_STRVAL_PP(fieldname), Z_STRVAL_PP(value), (ASBool)0 /*dummy*/);
	}
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto string fdf_next_field_name(resource fdfdoc [, string fieldname])
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
		FDF_FAILURE(err);
	} 

	RETVAL_STRING(buffer, 1);
	efree(buffer);
}
/* }}} */

/* {{{ proto bool fdf_set_ap(resource fdfdoc, string fieldname, int face, string filename, int pagenr)
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
			break;
	}

	err = FDFSetAP(fdf, Z_STRVAL_PP(fieldname), facenr, NULL, Z_STRVAL_PP(filename), (ASInt32) Z_LVAL_PP(pagenr));

	/* This should be made more intelligent, ie. use switch() with the 
	   possible errors this function can return. Or create global error handler function.
	 */
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}

	FDF_SUCCESS;

}
/* }}} */

/* {{{ proto bool fdf_get_ap(resource fdfdoc, string fieldname, int face, string filename) 
	 Gets the appearance of a field and creates a PDF document out of it. */
PHP_FUNCTION(fdf_get_ap) {
	zval *r_fdf;
	char *fieldname, *filename;
	int fieldname_len, filename_len, face;
	FDFDoc fdf;
	FDFErc err;
	FDFAppFace facenr;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsls",
							 &r_fdf, &fieldname, &fieldname_len,
							 &face, &filename, &filename_len) 
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	switch(face) {
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
			break;
	}

	err = FDFGetAP(fdf, fieldname, facenr, filename);

	/* This should be made more intelligent, ie. use switch() with the 
	   possible errors this function can return. Or create global error handler function.
	 */
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}

	FDF_SUCCESS;

	
}
/* }}} */

/* {{{ proto string fdf_get_encoding(resource fdf) 
   Gets FDF file encoding scheme */
PHP_FUNCTION(fdf_get_encoding) {
	zval *r_fdf;
	FDFDoc fdf;
	FDFErc err;
	char buffer[32];
	ASInt32 len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							 &r_fdf) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	err = FDFGetEncoding(fdf, buffer, 32, &len);

	/* This should be made more intelligent, ie. use switch() with the 
	   possible errors this function can return. Or create global error handler function.
	 */
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	
	FDF_G(error) = FDFErcOK;
	RETURN_STRINGL(buffer, (size_t)len, 1);
}
/* }}} */

/* {{{ proto bool fdf_set_status(resource fdfdoc, string status)
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
		FDF_FAILURE(err);
	}

	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto string fdf_get_status(resource fdfdoc)
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
		efree(buf);
		FDF_FAILURE(err);
	}

	RETVAL_STRING(buf, 1);
	efree(buf);
}
/* }}} */

/* {{{ proto bool fdf_set_file(resource fdfdoc, string filename [, string target_frame])
   Sets the value of /F key */
PHP_FUNCTION(fdf_set_file) 
{
	zval *r_fdf;
	char *filename, *target_frame= NULL;
	int filename_len, target_frame_len;
	FDFDoc fdf;
	FDFErc err;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s", &r_fdf, 
							 &filename, &filename_len,
							 &target_frame, &target_frame_len)
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	err = FDFSetFile(fdf, filename);
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	if(target_frame) {
#ifdef HAVE_FDFTK_5
		err = FDFSetTargetFrame(fdf, target_frame);
		if(err != FDFErcOK) {
			FDF_FAILURE(err);
		}
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "setting the target frame is only possible with FDF toolkit 5.0 and above, ignoring it for now");
#endif
	}

	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto string fdf_get_file(resource fdfdoc)
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
		efree(buf);
		FDF_FAILURE(err);
	}

	RETVAL_STRING(buf, 1);
	efree(buf);
}
/* }}} */

/* {{{ proto mixed fdf_save(resource fdfdoc [, string filename])
   Writes out the FDF file */
PHP_FUNCTION(fdf_save) 
{
	zval *r_fdf;
	char *filename = NULL;
	int filename_len;
	FDFDoc fdf;
	FDFErc err;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &r_fdf, &filename, &filename_len)
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	if(filename) {
		err = FDFSave(fdf, filename);	
	} else {
		FILE *fp;
		char *temp_filename;

		fp = php_open_temporary_file(PG(upload_tmp_dir), "php", &temp_filename TSRMLS_CC);
		if(!fp) {
			err = FDFErcFileSysErr;
		} else {
			fclose(fp);
			err = FDFSave(fdf, temp_filename);

			if(err == FDFErcOK) {
				php_stream *stream = php_stream_open_wrapper(temp_filename, "rb", 0, NULL);
				if (stream)	{
					php_stream_passthru(stream);
					php_stream_close(stream);
				} else {
					err = FDFErcFileSysErr;
				}	
			}
		}
		if(temp_filename) {
			unlink(temp_filename);
			efree(temp_filename);
		}
	}

	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	
	FDF_SUCCESS;
} 
/* }}} */

/* {{{ proto mixed fdf_save_string(resource fdfdoc)
   Returns the FDF file as a string */
PHP_FUNCTION(fdf_save_string) 
{
	zval *r_fdf;
	FDFDoc fdf;
	FDFErc err;
	FILE *fp;
	char *temp_filename = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &r_fdf)
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	fp = php_open_temporary_file(PG(upload_tmp_dir), "php", &temp_filename TSRMLS_CC);
	if(!fp) {
		err = FDFErcFileSysErr;
	} else {
		fclose(fp);
		err = FDFSave(fdf, temp_filename);

		if(err == FDFErcOK) {
			fp = fopen(temp_filename, "rb");
			if (fp)	{
				struct stat stat;
				char *buf;

				fstat(fileno(fp), &stat);
				buf = emalloc(stat.st_size +1);
				fread(buf, stat.st_size, 1, fp);
				buf[stat.st_size] = '\0';
				fclose(fp);

				unlink(temp_filename);
				efree(temp_filename);
				RETURN_STRINGL(buf, stat.st_size, 0);
			} else {
				err = FDFErcFileSysErr;
			}	
		}
	}

	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}

	if(temp_filename) {
		unlink(temp_filename);
		efree(temp_filename);
	}

	return;
} 
/* }}} */

/* {{{ proto bool fdf_add_template(resource fdfdoc, int newpage, string filename, string template, int rename)
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
		FDF_FAILURE(err);
	}

	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto bool fdf_set_flags(resource fdfdoc, string fieldname, int whichflags, int newflags)
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
		FDF_FAILURE(err);
	}

	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto int fdf_get_flags(resorce fdfdoc, string fieldname, int whichflags) 
   Gets the flags of a field */
PHP_FUNCTION(fdf_get_flags) {
 	zval *r_fdf;
	char *fieldname;
	int fieldname_len, whichflags;
	FDFDoc fdf;
	FDFErc err;
	ASUns32 flags;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl",
							 &r_fdf, &fieldname, &fieldname_len,
							 &whichflags) 
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	err = FDFGetFlags(fdf, fieldname, (FDFItem)whichflags, &flags);

	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}

	RETURN_LONG((long)flags);
}
/* }}} */

/* {{{ proto bool fdf_set_opt(resource fdfdoc, string fieldname, int element, string value, string name)
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
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto mixed fdf_get_option(resource fdfdof, string fieldname [, int element])
   Gets a value from the opt array of a field */
PHP_FUNCTION(fdf_get_opt) {
 	zval *r_fdf;
	char *fieldname;
	int fieldname_len, element = -1;
	FDFDoc fdf;
	FDFErc err;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l",
							 &r_fdf, &fieldname, &fieldname_len,
							 &element) 
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	if(element == -1) {
		ASInt32 elements;
		err = FDFGetOpt(fdf, fieldname, (ASInt32)-1, NULL, NULL, 0, &elements);
		if(err != FDFErcOK) {
			FDF_FAILURE(err);
		}
		RETURN_LONG((long)elements);
	} else {
		ASInt32 bufSize, nRet;
		char *buf1, *buf2;

		bufSize = 1024; 
		buf1 = emalloc(bufSize);
		buf2 = emalloc(bufSize);

		err = FDFGetOpt(fdf, fieldname, (ASInt32)element, buf1, buf2, bufSize, &nRet);
		if(err == FDFErcBufTooShort) {
			efree(buf1);
			efree(buf2);
			buf1 = emalloc(nRet);
			buf2 = emalloc(nRet);
			bufSize = nRet;
			err = FDFGetOpt(fdf, fieldname, (ASInt32)element, buf1, buf2, bufSize, &nRet);
		}
		if(err != FDFErcOK) {
			FDF_FAILURE(err);
		}
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_next_index_stringl(return_value, buf1, strlen(buf1), 1);
		add_next_index_stringl(return_value, buf2, strlen(buf2), 1);
		efree(buf1);
		efree(buf2);
	}
}
/* }}} */

/* {{{ proto bool fdf_set_submit_form_action(resource fdfdoc, string fieldname, int whichtrigger, string url, int flags)
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
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto bool fdf_set_javascript_action(resource fdfdoc, string fieldname, int whichtrigger, string script)
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
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
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
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
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

/* {{{ int fdf_errno(void) 
   Gets error code for last operation */
PHP_FUNCTION(fdf_errno) {
	RETURN_LONG((long)FDF_G(error));
}
/* }}} */

/* {{{ string fdf_error([int errno]) 
   Gets error description for error code */
PHP_FUNCTION(fdf_error) {
	FDFErc err;
	long p_err = -1;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &p_err)
	   == FAILURE) {
		return;
	}

	err = (p_err >= 0) ? (FDFErc)p_err : FDF_G(error);

	switch(err) {
	case FDFErcOK: 
		RETURN_STRING("no error", 1);
	case FDFErcInternalError: 
		RETURN_STRING("An internal FDF Library error occurred", 1);
	case FDFErcBadParameter: 
		RETURN_STRING("One or more of the parameters passed were invalid. ", 1);
	case FDFErcFileSysErr: 
		RETURN_STRING("A file system error occurred or the file was not found", 1);
	case FDFErcBadFDF: 
		RETURN_STRING("The FDF file being opened or parsed was invalid", 1);
	case FDFErcFieldNotFound: 
		RETURN_STRING("The field whose name was passed in the parameter fieldName does not exist in the FDF file", 1);
	case FDFErcNoValue: 
		RETURN_STRING("The field whose value was requested has no value", 1);
	case FDFErcEnumStopped: 
		RETURN_STRING("Enumeration was stopped by FDFEnumValues by returning FALSE", 1);
	case FDFErcCantInsertField: 
		RETURN_STRING("The field whose name was passed in the parameter fieldName cannot be inserted into the FDF file", 1);
	case FDFErcNoOption: 
		RETURN_STRING("The requested element in a fields /Opt key does not exist, or the field has no /Opt key. ", 1);
	case FDFErcNoFlags: 
		RETURN_STRING("The field has no /F or /Ff keys", 1);
	case FDFErcBadPDF:
		RETURN_STRING("The PDF file passed as the parameter to FDFSetAP was invalid, or did not contain the requested page ", 1);
	case FDFErcBufTooShort: 
		RETURN_STRING("The buffer passed as a parameter was too short", 1);
	case FDFErcNoAP: 
		RETURN_STRING("The field has no /AP key", 1);
	case FDFErcIncompatibleFDF: 
		RETURN_STRING("An attempt to mix classic and template-based FDF files was made", 1);
#ifdef HAVE_FDFTK_5
	case FDFErcNoAppendSaves: 
		RETURN_STRING("The FDF does not include a /Difference key", 1);
	case FDFErcValueIsArray: 
		RETURN_STRING("The value of this field is an array. Use FDFGetNthValue. ", 1);
	case FDFErcEmbeddedFDFs: 
		RETURN_STRING("The FDF you passed as a parameter is a container for one or more FDFs embedded within it. Use FDFOpenFromEmbedded to gain access to each embedded FDF", 1);
	case FDFErcNoMoreFDFs: 
		RETURN_STRING("Returned by FDFOpenFromEmbedded when parameter iWhich >= the number of embedded FDFs (including the case when the passed FDF does not contain any embedded FDFs)", 1);
	case FDFErcInvalidPassword: 
		RETURN_STRING("Returned by FDFOpenFromEmbedded when the embedded FDF is encrypted, and you did not provide the correct password", 1);
#endif
	case FDFErcLast: 
		RETURN_STRING("Reserved for future use", 1);
	default: 
		RETURN_STRING("unknown error", 1);
	}
}
/* }}} */

/* {{{ proto string fdf_get_version([resource fdfdoc]) 
   Gets version number for FDF api or file */
PHP_FUNCTION(fdf_get_version) {
	zval *r_fdf = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &r_fdf)
	   == FAILURE) {
		return;
	}

	if(r_fdf) {
#if HAVE_FDFTK_5
		const char *fdf_version; 
		FDFDoc fdf;

		ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);
		fdf_version = FDFGetFDFVersion(fdf);
		RETURN_STRING((char *)fdf_version, 1);
#else
		RETURN_STRING("1.2",1);
#endif
	} else {
		const char *api_version = FDFGetVersion();
		RETURN_STRING((char *)api_version, 1);
	}
}
/* }}} */

#ifdef HAVE_FDFTK_5
/* {{{ proto bool fdf_set_version(resourece fdfdoc, string version)
   Sets FDF version for a file*/
PHP_FUNCTION(fdf_set_version) {
	zval *r_fdf;
	char *version;
	int version_len;
	FDFDoc fdf;
	FDFErc err;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &r_fdf, &version, &version_len)
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	err = FDFSetFDFVersion(fdf, version);

	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto bool fdf_add_doc_javascript(resource fdfdoc, string scriptname, string script) 
	 Add javascript code to the fdf file */
PHP_FUNCTION(fdf_add_doc_javascript) {
	zval *r_fdf;
	char *name, *script;
	int name_len, script_len;
	FDFDoc fdf;
	FDFErc err;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &r_fdf, 
							 &name, &name_len,
							 &script, &script_len
							 )
	   == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);
	
	err = FDFAddDocJavaScript(fdf, name, script);
	
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto bool fdf_set_on_import_javascript(resource fdfdoc, string script [, bool before_data_import])
   Adds javascript code to be executed when Acrobat opens the FDF */
PHP_FUNCTION(fdf_set_on_import_javascript) {
	zval *r_fdf;
	char *script;
	int script_len;
	zend_bool before;
	FDFDoc fdf;
	FDFErc err;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsb", &r_fdf, 
							 &script, &script_len, &before
							 )
	   == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);
	
	err = FDFSetOnImportJavaScript(fdf, script, before);
	
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
} 
/* }}} */

/* {{{ proto bool fdf_set_target_frame(resource fdfdoc, string target)
   Sets target frame for form */
PHP_FUNCTION(fdf_set_target_frame) {
	zval *r_fdf;
	char *target;
	int target_len;
	FDFDoc fdf;
	FDFErc err;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &r_fdf, 
							 &target, &target_len
							 )
	   == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);
	
	err = FDFSetTargetFrame(fdf, target);
	
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
} 
/* }}} */
#endif

/* {{{ proto bool fdf_remove_item(resource fdfdoc, string fieldname, int item)
   Sets target frame for form */
PHP_FUNCTION(fdf_remove_item) {
	zval *r_fdf;
	char *fieldname;
	int fieldname_len;
	long item;
	FDFDoc fdf;
	FDFErc err;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &r_fdf, 
							 &fieldname, &fieldname_len,
							 item
							 )
	   == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);
	
	err = FDFRemoveItem(fdf, *fieldname ? fieldname : NULL, item);
	
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
} 
/* }}} */

#ifdef HAVE_FDFTK_5
/* {{{ proto array fdf_get_attachment(resource fdfdoc, string fieldname, string savepath)
   Get attached uploaded file */
PHP_FUNCTION(fdf_get_attachment) {
	zval *r_fdf;
	char *fieldname, *savepath;
	int fieldname_len, savepath_len;
	int is_dir=0;
	FDFDoc fdf;
	FDFErc err;
	char pathbuf[MAXPATHLEN], mimebuf[1024];
    struct stat statBuf;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &r_fdf, 
							 &fieldname, &fieldname_len,
							 &savepath, &savepath_len
							 )
	   == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	strncpy(pathbuf	, savepath, MAXPATHLEN-1);
	pathbuf[MAXPATHLEN-1] = '\0';

	if(0 == stat(pathbuf, &statBuf)) {
		is_dir = S_ISDIR(statBuf.st_mode);
	}

	err = FDFExtractAttachment(fdf, fieldname, pathbuf, sizeof(pathbuf), is_dir, mimebuf, sizeof(mimebuf)); 
	
	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "path", pathbuf, 1);
    add_assoc_string(return_value, "type", mimebuf, 1);
	stat(pathbuf, &statBuf);
	add_assoc_long(return_value, "size", statBuf.st_size);
}
/* }}} */
#endif 

/* {{{ enum_values_callback */
  static ASBool enum_values_callback(char *name, char *value, void *userdata) {
	  zval *retval_ptr, *z_name, *z_value, **args[3];
	  long retval = 0;
	  int numargs = 2;
	  TSRMLS_FETCH();

	  MAKE_STD_ZVAL(z_name);
	  ZVAL_STRING(z_name, name, 1);
	  args[0] = &z_name;

	  if(*value) { // simple value 
		  MAKE_STD_ZVAL(z_value);
		  ZVAL_STRING(z_value, value, 1);
		  args[1] = &z_value;
	  } else { // empty value *might* be an array
		  // todo do it like fdf_get_value (or re-implement yourself?)
	  }
	  
	  if(userdata) {
		  args[2] = (zval **)userdata;
		  numargs++;
	  }

	  if (call_user_function_ex(EG(function_table), 
								NULL, 
								FDF_G(enum_callback), 
								&retval_ptr, 
								numargs, args, 
								0, NULL TSRMLS_CC)==SUCCESS
		  && retval_ptr) {
		  
		  convert_to_long_ex(&retval_ptr);
		  retval = Z_LVAL_P(retval_ptr);
		  zval_ptr_dtor(&retval_ptr);
	  } else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "callback failed");
	  }

	  zval_ptr_dtor(&z_name);
	  zval_ptr_dtor(&z_value);

	  return retval;
  }
/* }}} */

/* {{{ proto bool fdf_enum_values(resource fdfdoc, mixed callback [, mixed userdata])
 */
PHP_FUNCTION(fdf_enum_values) {
	zval *r_fdf;
	zval *callback;
	zval *userdata = NULL;
	FDFDoc fdf;
	FDFErc err;
	char *name;
	char namebuf[1024], valbuf[1024];

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|z", &r_fdf, 
							 &callback, &userdata
							 )
	   == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(fdf, FDFDoc *, &r_fdf, -1, "fdf", le_fdf);

	if (Z_TYPE_P(callback) != IS_ARRAY && 
		Z_TYPE_P(callback) != IS_STRING) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong syntax for function name");
		RETURN_FALSE;
	}

	convert_to_string_ex(&callback);

	if (!zend_is_callable(callback, 0, &name)) {
		php_error_docref1(NULL TSRMLS_CC, name, E_WARNING, "Second argument is expected to be a valid callback");
		efree(name);
		RETURN_FALSE;
	}
	efree(name);
	FDF_G(enum_callback) = callback;
	FDF_G(enum_fdf) = fdf;

	err = FDFEnumValues(fdf, enum_values_callback, 
						namebuf, sizeof(namebuf), 
						valbuf, sizeof(valbuf), 
						userdata ? &userdata : NULL, 0); 

	if(err != FDFErcOK) {
		FDF_FAILURE(err);
	}
	FDF_SUCCESS;
}
/* }}} */

/* {{{ proto fdf_header() 
 Set FDF specific HTTP headers */
PHP_FUNCTION(fdf_header) {
	sapi_header_line ctr = {0};
	
	ctr.line = "Content-type: application/vnd.fdf";
	ctr.line_len = strlen(ctr.line);
	ctr.response_code = 200;

	sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
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
