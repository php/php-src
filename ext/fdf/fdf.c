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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   |          Hartmut Holzgraefe <hartmut@six.de>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* FdfTk lib 2.0 is a Complete C/C++ FDF Toolkit available from
   http://beta1.adobe.com/ada/acrosdk/forms.html. */

/* Note that there is no code from the FdfTk lib in this file */

#if !PHP_31 && defined(THREAD_SAFE)
#undef THREAD_SAFE
#endif

#include "php.h"
#include "SAPI.h"
#include "ext/standard/head.h"
#include "php_open_temporary_file.h"
#include <math.h>
#include "php_fdf.h"

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_FDFLIB

#include "ext/standard/info.h"

#ifdef THREAD_SAFE
DWORD FDFlibTls;
static int numthreads=0;

typedef struct fdflib_global_struct{
	int le_fdf;
} fdflib_global_struct;

# define FDF_GLOBAL(a) fdflib_globals->a
# define FDF_TLS_VARS fdflib_global_struct *fdflib_globals=TlsGetValue(FDFlibTls)

#else
#  define FDF_GLOBAL(a) a
#  define FDF_TLS_VARS
int le_fdf_info;
int le_fdf;
#endif

SAPI_POST_HANDLER_FUNC(fdf_post_handler);

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
	PHP_FE(fdf_add_template,							NULL)
	PHP_FE(fdf_set_flags,							NULL)
	PHP_FE(fdf_set_opt,								NULL)
	PHP_FE(fdf_set_submit_form_action,				NULL)
	PHP_FE(fdf_set_javascript_action,				NULL)
	{NULL, NULL, NULL}
};

zend_module_entry fdf_module_entry = {
	"fdf", fdf_functions, PHP_MINIT(fdf), PHP_MSHUTDOWN(fdf), NULL, NULL,
	PHP_MINFO(fdf), STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FDF
ZEND_GET_MODULE(fdf)
#endif

static void phpi_FDFClose(zend_rsrc_list_entry *rsrc)
{
	FDFDoc fdf = (FDFDoc)rsrc->ptr;
	(void)FDFClose(fdf);
}

static sapi_post_entry supported_post_entries[] = {
#if HAVE_FDFLIB
	{ "application/vnd.fdf",	sizeof("application/vnd.fdf")-1,	php_default_post_reader, fdf_post_handler},
#endif
	{ NULL, 0, NULL }
};



PHP_MINIT_FUNCTION(fdf)
{
	FDFErc err;
	FDF_GLOBAL(le_fdf) = register_list_destructors(phpi_FDFClose, NULL, "fdf");

	/* add handler for Acrobat FDF form post requests */
	sapi_add_post_entry("application/vnd.fdf",	php_default_post_reader, fdf_post_handler);

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
	err = FDFInitialize();
	if(err == FDFErcOK)
		return SUCCESS;
	return FAILURE;
}

PHP_MINFO_FUNCTION(fdf)
{
	/* need to use a PHPAPI function here because it is external module in windows */
	php_info_print_table_start();
	php_info_print_table_row(2, "FDF Support", "enabled");
	php_info_print_table_row(2, "FdfTk Version", FDFGetVersion() );
	php_info_print_table_end();
}

PHP_MSHUTDOWN_FUNCTION(fdf)
{
	FDFErc err;

	/* remove handler for Acrobat FDF form post requests */
	sapi_remove_post_entry("application/vnd.fdf"); 

#ifdef PHP_WIN32
	return SUCCESS;
#endif
	err = FDFFinalize();
	if(err == FDFErcOK)
		return SUCCESS;
	return FAILURE;
}

/* {{{ proto int fdf_open(string filename)
   Opens a new fdf document */
PHP_FUNCTION(fdf_open) {
	pval **file;
	int id;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

	err = FDFOpen((*file)->value.str.val, 0, &fdf);
	if(err != FDFErcOK)
		printf("Aiii, error\n");
	if(!fdf)
		RETURN_FALSE;

	id = zend_list_insert(fdf,FDF_GLOBAL(le_fdf));
	RETURN_LONG(id);
} /* }}} */

/* {{{ proto boolean fdf_close(int fdfdoc)
   Closes the fdf document */
PHP_FUNCTION(fdf_close) {
	pval **arg1;
	int id, type;
	FDFDoc fdf;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

/*	FDFClose(fdf); */
	zend_list_delete(id);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto int fdf_create(void)
   Creates a new fdf document */
PHP_FUNCTION(fdf_create) {
	int id;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	err = FDFCreate(&fdf);
	if(err != FDFErcOK)
		printf("Aiii, error\n");
	if(!fdf)
		RETURN_FALSE;

	id = zend_list_insert(fdf,FDF_GLOBAL(le_fdf));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto string fdf_get_value(int fdfdoc, string fieldname)
   Gets the value of a field as string */
PHP_FUNCTION(fdf_get_value) {
	pval **arg1, **arg2;
	int id, type;
	ASInt32 nr;
	char *buffer;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFGetValue(fdf, (*arg2)->value.str.val, NULL, 0, &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");
  /* In the inofficial version of FdfTK 4.0 (as FDFGetVersion says. The
     library has a name with version 3.0, don't know what adobe has in
     mind) the number of bytes of the value doesn't include the trailing
     '\0'. This was not the case in 2.0
  */
	if(strcmp(FDFGetVersion(), "2.0"))
		nr++;
	buffer = emalloc(nr);
	err = FDFGetValue(fdf, (*arg2)->value.str.val, buffer, nr, &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_STRING(buffer, 0);
}
/* }}} */

/* {{{ proto boolean fdf_set_value(int fdfdoc, string fieldname, string value, int isName)
   Sets the value of a field */
PHP_FUNCTION(fdf_set_value) {
	pval **arg1, **arg2, **arg3, **arg4;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2,&arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFSetValue(fdf, (*arg2)->value.str.val,(*arg3)->value.str.val, (ASBool) (*arg4)->value.lval);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fdf_next_field_name(int fdfdoc [, string fieldname])
   Gets the name of the next field name or the first field name */
PHP_FUNCTION(fdf_next_field_name) {
	pval **argv[2];
	int id, type, argc;
	ASInt32 nr;
	char *buffer, *fieldname;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	argc = ZEND_NUM_ARGS();
	if((argc > 2) || (argc < 1))
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(argv[0]);
	if(argc == 2) {
		convert_to_string_ex(argv[1]);
		fieldname = (*argv[1])->value.str.val;
	} else {
		fieldname = NULL;
	}
	id=(*argv[0])->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFNextFieldName(fdf, fieldname, NULL, 0, &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");
	if(nr == 0)
		RETURN_STRING(empty_string, 1);
		
	buffer = emalloc(nr);
	err = FDFNextFieldName(fdf, fieldname, buffer, nr, &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_STRING(buffer, 0);
}
/* }}} */

/* {{{ proto boolean fdf_set_ap(int fdfdoc, string fieldname, int face, string filename, int pagenr)
   Sets the value of a field */
PHP_FUNCTION(fdf_set_ap) {
	pval **arg1, **arg2, **arg3, **arg4, **arg5;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	FDFAppFace face;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2,&arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_long_ex(arg5);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	switch((*arg3)->value.lval) {
		case 1:
			face = FDFNormalAP;
			break;
		case 2:
			face = FDFRolloverAP;
			break;
		case 3:
			face = FDFDownAP;
			break;
		default:
			face = FDFNormalAP;
	}

	err = FDFSetAP(fdf, (*arg2)->value.str.val, face, NULL,
(*arg4)->value.str.val, (ASInt32) (*arg5)->value.lval);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean fdf_set_status(int fdfdoc, string status)
   Sets the value in the /Status key. */
PHP_FUNCTION(fdf_set_status) {
	pval **arg1, **arg2;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFSetStatus(fdf, (*arg2)->value.str.val);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fdf_get_status(int fdfdoc)
   Gets the value in the /Status key. */
PHP_FUNCTION(fdf_get_status) {
	pval **arg1;
	int id, type;
	ASInt32 nr;
	char *buf;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFGetStatus(fdf, NULL, 0,  &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");
	if(nr == 0)
		RETURN_STRING(empty_string, 1);
	buf = emalloc(nr);
	err = FDFGetStatus(fdf, buf, nr,  &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_STRING(buf, 0);
}
/* }}} */

/* {{{ proto boolean fdf_set_file(int fdfdoc, string filename)
   Sets the value of the FDF's /F key */
PHP_FUNCTION(fdf_set_file) {
	pval **arg1, **arg2;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFSetFile(fdf, (*arg2)->value.str.val);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fdf_get_file(int fdfdoc)
   Gets the value in the /F key. */
PHP_FUNCTION(fdf_get_file) {
	pval **arg1;
	int id, type;
	ASInt32 nr;
	char *buf;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFGetFile(fdf, NULL, 0,  &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");
	if(nr == 0)
		RETURN_STRING(empty_string, 1);
	buf = emalloc(nr);
	err = FDFGetFile(fdf, buf, nr,  &nr);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_STRING(buf, 0);
}
/* }}} */

/* {{{ proto boolean fdf_save(int fdfdoc, string filename)
   Writes out an FDF file. */
PHP_FUNCTION(fdf_save) {
	pval **arg1, **arg2;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err = FDFSave(fdf, (*arg2)->value.str.val);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_TRUE;
} /* }}} */

/* {{{ proto boolean fdf_add_template(int fdfdoc, int newpage, string filename, string template, int rename)
   Adds a template to the FDF*/
PHP_FUNCTION(fdf_add_template) {
	pval **arg1, **arg2, **arg3, **arg4, **arg5;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	pdfFileSpecRec filespec;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2,&arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_long_ex(arg5);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	filespec.FS = NULL;
	filespec.F = (*arg3)->value.str.val;
	filespec.Mac = NULL;
	filespec.DOS = NULL;
	filespec.Unix = NULL;
	filespec.ID[0] = NULL;
	filespec.ID[1] = NULL;
	filespec.bVolatile = false;
	err = FDFAddTemplate(fdf, (*arg2)->value.lval, &filespec,(*arg4)->value.str.val, (*arg5)->value.lval);
	if(err != FDFErcOK)
		printf("Aiii, error\n");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean fdf_set_flags(int fdfdoc, string fieldname, int whichFlags, int newFlags)
   modifies a flag for a field in the fdf*/
PHP_FUNCTION(fdf_set_flags) {
	pval **arg1, **arg2, **arg3, **arg4;
	int id, type;
	FDFDoc fdf;
	FDFErc err;
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2,&arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);	
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	err=FDFSetFlags(fdf,(*arg2)->value.str.val,(*arg3)->value.lval,(*arg4)->value.lval);
	if(err != FDFErcOK)
		printf("Error setting FDF flags: %d\n",err);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean fdf_set_opt(int fdfdoc, string fieldname, int element, string value, string name)
   Sets a value in the opt array for a field in the FDF*/
PHP_FUNCTION(fdf_set_opt) {
	pval **arg1, **arg2, **arg3, **arg4, **arg5;
	int id, type;
	FDFDoc fdf;
	FDFErc err;	
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2,&arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	err = FDFSetOpt(fdf,(*arg2)->value.str.val,(*arg3)->value.lval,(*arg4)->value.str.val,(*arg5)->value.str.val);
	if(err != FDFErcOK)
		printf("Error setting FDF option: %d",err);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto booelan fdf_set_submit_form_action(int fdfdoc, string fieldname, int whichTrigger, string url, int flags)
   sets the submit form action for a field in the fdf*/
PHP_FUNCTION(fdf_set_submit_form_action) {
	pval **arg1, **arg2, **arg3, **arg4, **arg5;
	int id, type;
	FDFDoc fdf;
	FDFErc err;	
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2,&arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_long_ex(arg5);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	err = FDFSetSubmitFormAction(fdf,(*arg2)->value.str.val,(*arg3)->value.lval,(*arg4)->value.str.val,(*arg5)->value.lval);
	if(err != FDFErcOK)
		printf("Error setting FDF submit action: %d",err);

	RETURN_TRUE;
}

/* {{{ proto boolean fdf_set_javascript_action(int fdfdoc, string fieldname, int whichTrigger, string script)
   sets the javascript action for a field in the fdf*/
PHP_FUNCTION(fdf_set_javascript_action) {
	pval **arg1, **arg2, **arg3, **arg4;
	int id, type;
	FDFDoc fdf;
	FDFErc err;	
	FDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(4, &arg1, &arg2,&arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_string_ex(arg4);
	id=(*arg1)->value.lval;
	fdf = zend_list_find(id,&type);
	if(!fdf || type!=FDF_GLOBAL(le_fdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	err = FDFSetJavaScriptAction(fdf,(*arg2)->value.str.val,(*arg3)->value.lval,(*arg4)->value.str.val);
	if(err != FDFErcOK)
		printf("Error setting FDF JavaScript action: %d",err);

	RETURN_TRUE;
}


SAPI_POST_HANDLER_FUNC(fdf_post_handler)
{
	FILE *fp;
	FDFDoc theFDF;
	char *name=NULL,*value=NULL,*p;
	int name_len=0,value_len=0;
	char *lastfieldname =NULL;
	char *strtok_buf = NULL;
	char *filename = NULL;
	FDFErc err;
	ASInt32 nBytes, datalen;
	zval *array_ptr = (zval *) arg;
	ELS_FETCH();
	PLS_FETCH();
	
	fp=php_open_temporary_file(NULL,"fdfdata.",&filename);
	if(!fp) {
		if(filename) efree(filename);
		return;
	}
	fwrite(SG(request_info).post_data,SG(request_info).post_data_length,1,fp);
	fclose(fp);

 	err = FDFOpen(filename,0,&theFDF);

	if(err==FDFErcOK){	
		name = emalloc(name_len=256);
		value= emalloc(value_len=256);
		while (1) {
			err = FDFNextFieldName(theFDF,lastfieldname,name,name_len-1,&nBytes);
			if(err == FDFErcBufTooShort && nBytes >0 ) {
				name = erealloc(name,name_len=(nBytes+1)); 
				err = FDFNextFieldName(theFDF,lastfieldname,name,name_len-1,&nBytes);
			} 
			
			if(err != FDFErcOK || nBytes == 0) break; 
			
			if(lastfieldname) efree(lastfieldname);
			lastfieldname = estrdup(name);		

			err = FDFGetValue(theFDF,name,NULL,0,&nBytes);			
			if(err != FDFErcOK && err != FDFErcNoValue ) break; 

			if(value_len<nBytes+1) value = erealloc(value,value_len=(nBytes+1));
			
			if(nBytes>0) {
				err = FDFGetValue(theFDF,name,value,value_len-1,&nBytes);
				if(err == FDFErcOK && nBytes != 0) {
					for(p=value;*p;p++) if(*p=='\r') *p='\n';
					lastfieldname = estrdup(name);		
					php_register_variable(name, value, array_ptr ELS_CC PLS_CC);
				} 
			}
		}   
		
		FDFClose(theFDF);
		V_UNLINK((const char *)filename);
		efree(filename);

		if(name)          efree(name);
		if(value)         efree(value);
		if(lastfieldname) efree(lastfieldname);
	} 
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
