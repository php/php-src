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
   | Authors: James Moore <jmoore@php.net>                                |
   +----------------------------------------------------------------------+
 */

 /* $Id$ */

#if HAVE_W32API

#ifndef PHP_W32API_H
#define PHP_W32API_H

extern zend_module_entry w32api_module_entry;
#define phpext_w32api_ptr &w32api_module_entry

#ifdef PHP_WIN32
#define PHP_W32API_API __declspec(dllexport)
#else
#define PHP_W32API_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(w32api)
	HashTable *regfuncs;					// HashTable of Registered function handles
	HashTable *libraries;					// HashTable holding pointers to the libariers
	HashTable *types;						// Handles for users' types
	int le_dynaparm;						// Resource handle
	DWORD call_type;						// Type of call we use when calling a DLL.
ZEND_END_MODULE_GLOBALS(w32api)

#ifdef ZTS
#define W32_G(v) TSRMG(w32api_globals_id, zend_w32api_globals *, v)
#else
#define W32_G(v) (w32api_globals.v)
#endif

#define W32_REG_CONST(cname) REGISTER_LONG_CONSTANT(#cname, cname, CONST_CS | CONST_PERSISTENT);

#define  DC_MICROSOFT           0x0000      // Default
#define  DC_BORLAND             0x0001      // Borland compat
#define  DC_CALL_CDECL          0x0010      // __cdecl
#define  DC_CALL_STD            0x0020      // __stdcall
#define  DC_RETVAL_MATH4        0x0100      // Return value in ST
#define  DC_RETVAL_MATH8        0x0200      // Return value in ST

#define  DC_CALL_STD_BO         (DC_CALL_STD | DC_BORLAND)
#define  DC_CALL_STD_MS         (DC_CALL_STD | DC_MICROSOFT)
#define  DC_CALL_STD_M8         (DC_CALL_STD | DC_RETVAL_MATH8)

#define  DC_FLAG_ARGPTR         0x00000002

typedef struct W32APIFE {
	FARPROC fp;								// Pointer to the function
	char *rettype;							// return value type
	int retvaltype;							// if complex = 1 if simple = 0
} W32APIFE;

typedef struct _field {
	char *fname;							// Fields name
	char *type;								// Type of field
	int fsize;								// size of field
} field;

typedef struct runtime_struct {
	char *name;								// structs name
	long size;								// structs size
	void *fields;							// pointer to an array of fields
} runtime_struct;

#pragma pack(1)                 // Set struct packing to one byte

typedef union RESULT {          // Various result types
    int     Int;                // Generic four-byte type
    long    Long;               // Four-byte long
    void   *Pointer;            // 32-bit pointer
    float   Float;              // Four byte real
    double  Double;             // 8-byte real
    __int64 int64;              // big int (64-bit)
} RESULT;


typedef struct DYNAPARM {
    DWORD       dwFlags;        // Parameter flags
    int         nWidth;         // Byte width
    union {                     // 
        DWORD   dwArg;          // 4-byte argument
        void   *pArg;           // Pointer to argument
    };
} DYNAPARM;


PHP_W32API_API RESULT php_w32api_dynamic_dll_call(
    int      Flags,
    DWORD    lpFunction,
    int      nArgs,
    DYNAPARM Param[],
    LPVOID   pRet,
    int      nRetSize
    );

static void php_w32api_init_globals(zend_w32api_globals *w32api_globals);
static void php_w32api_dtor_libary(void *data);
static void php_w32api_unload_libraries();

PHP_MINFO_FUNCTION(w32api);
PHP_MINIT_FUNCTION(w32api);
PHP_MSHUTDOWN_FUNCTION(w32api);

PHP_FUNCTION(w32api_register_function);
PHP_FUNCTION(w32api_invoke_function);
PHP_FUNCTION(w32api_deftype);
PHP_FUNCTION(w32api_init_dtype);
PHP_FUNCTION(w32api_set_call_method);

static void register_constants(int module_number);
static void w32api_free_dynaparm(zend_rsrc_list_entry *rsrc TSRMLS_DC);
void get_arg_pointer(zval **value, void ** argument);
DYNAPARM w32api_convert_zval_to_dynparam(zval ** carg TSRMLS_DC);

#endif /* PHP_W32API_H */
#endif /* HAVE_W32API */