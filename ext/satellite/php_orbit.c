/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

/*
 * Interface to PHP
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>	 /* for DISPLAY_INI_ENTRIES() */
#include <ext/standard/info.h>	/* for php_info_print_table_*() */
#include "php_orbit.h"
#include "corba.h"
#include "typemanager.h"
#include "namedvalue_to_zval.h"

/* classes */
#include "enum.h"
#include "object.h"
#include "struct.h"


#include "php_config.h"	/* for COMPILE_DL_ORBIT */

/* see php4/README.SELF-CONTAINED-EXTENSIONS */
#if COMPILE_DL_SATELLITE
ZEND_GET_MODULE(satellite)
#endif

PHP_MINFO_FUNCTION(satellite);

PHP_INI_BEGIN()
	PHP_INI_ENTRY1("idl_directory", NULL, 0, NULL, NULL)
PHP_INI_END()

/*
 * functions in module
 */
static function_entry satellite_functions[] = {
	PHP_FE(satellite_load_idl,					NULL)
	PHP_FE(satellite_get_repository_id, NULL)
	PHP_FE(satellite_caught_exception, 	NULL)
	PHP_FE(satellite_exception_id, 			NULL)
	PHP_FE(satellite_exception_value, 	NULL)
	PHP_FE(satellite_object_to_string, 	NULL)

	/* support the old prefix orbit_ */
 	PHP_FALIAS(orbit_load_idl, 					satellite_load_idl, 					NULL)
	PHP_FALIAS(orbit_get_repository_id,	satellite_get_repository_id,	NULL)
	PHP_FALIAS(orbit_caught_exception, 	satellite_caught_exception, 	NULL)
	PHP_FALIAS(orbit_exception_id, 			satellite_exception_id, 			NULL)
	PHP_FALIAS(orbit_exception_value, 	satellite_exception_value, 		NULL)

 {NULL, NULL, NULL}
};


/*
 * module entry
 */
zend_module_entry satellite_module_entry = {
	STANDARD_MODULE_HEADER,
	"satellite",
	satellite_functions,
	PHP_MINIT(satellite),			/* module startup */
	PHP_MSHUTDOWN(satellite),	/* module shutdown */
	NULL,											/* request startup */
	NULL,											/* request shutdown */
	PHP_MINFO(satellite),			/* module info */
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

/*
 * module initialization
 */
PHP_MINIT_FUNCTION(satellite)
{
	zend_bool success;
	
	REGISTER_INI_ENTRIES();
	
	success = 
		orbit_corba_init() &&
		TypeManager_Init(INI_STR("idl_directory")) &&
		OrbitEnum_Init(module_number) &&
		OrbitObject_Init(module_number) &&
		OrbitStruct_Init(module_number);

	return success ? SUCCESS : FAILURE;
}

/*
 * shutdown module!
*/
PHP_MSHUTDOWN_FUNCTION(satellite)
{
	TypeManager_Shutdown();
	orbit_corba_shutdown();	
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

/*
 * some function
 */
PHP_MINFO_FUNCTION(satellite) 
{

	php_info_print_table_start();
	php_info_print_table_header(2, "CORBA support via Satellite", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/* instruct the type manager to load an IDL file if not already loaded */
PHP_FUNCTION(satellite_load_idl)
{
	zval * p_parameter;
	
	if (ZEND_NUM_ARGS() != 1)
	{
		WRONG_PARAM_COUNT;
		RETURN_NULL();
	}

	getParameters(ht, 1, &p_parameter);

	if (Z_TYPE_P(p_parameter) != IS_STRING)
	{
		RETURN_NULL();
	}

	RETURN_BOOL(TypeManager_LoadFile(Z_STRVAL_P(p_parameter)));
}

/* 
 * NOT IMPLEMENTED
 *
 * get the repository id for an Orbit* object (nice for debugging...)
 */
PHP_FUNCTION(satellite_get_repository_id)
{
}

PHP_FUNCTION(satellite_caught_exception)
{
	RETURN_BOOL(orbit_caught_exception());
}

PHP_FUNCTION(satellite_exception_id)
{
	CORBA_char * p_id = CORBA_exception_id(orbit_get_environment());
	
	RETURN_STRING(p_id, TRUE);
}

/* real name: php_if_orbit_exception_value */
PHP_FUNCTION(satellite_exception_value)
{
	CORBA_NamedValue source;
	ExceptionType * p_exception = NULL;
	
	memset(&source, 0, sizeof(CORBA_NamedValue));
	
	/* get exception type info */
	p_exception = TypeManager_FindException(
			CORBA_exception_id(orbit_get_environment()));
	
	/* get exception typecode */
	source.argument._type = ExceptionType_GetTypeCode(p_exception);
	/* get exception value */
	source.argument._value = CORBA_exception_value(orbit_get_environment());
	
	/* create structure with exception data */
	orbit_namedvalue_to_zval(&source, return_value);
}

