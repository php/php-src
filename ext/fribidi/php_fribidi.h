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
   | Author: Onn Ben-Zvi <onn@zend.com>, onnb@mercury.co.il               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_FRIBIDI_H
#define PHP_FRIBIDI_H

#if HAVE_FRIBIDI

#include "fribidi.h"
#include "../standard/info.h"

extern zend_module_entry fribidi_module_entry;
#define phpext_fribidi_ptr &fribidi_module_entry


typedef struct _php_fribidi_rsrc{
	guint16 *position_L_to_V_list;	
	guint16 *position_V_to_L_list;
	guint8  *embedding_level_list;
	guchar *out_string;
} php_fribidi_rsrc;


#ifdef PHP_WIN32
#define PHP_FRIBIDI_API __declspec(dllexport)
#else
#define PHP_FRIBIDI_API
#endif

PHP_MINIT_FUNCTION(fribidi);
PHP_MSHUTDOWN_FUNCTION(fribidi);
PHP_RINIT_FUNCTION(fribidi);
PHP_RSHUTDOWN_FUNCTION(fribidi);
PHP_MINFO_FUNCTION(fribidi);

ZEND_FUNCTION(fribidi_log2vis);
/*void php_fribidi_free_rsrc(php_fribidi_rsrc* p_rsrc);*/
/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(fribidi)
	int global_variable;
ZEND_END_MODULE_GLOBALS(fribidi)
*/

/* In every function that needs to use variables in php_fribidi_globals,
   do call FRIBIDILS_FETCH(); after declaring other variables used by
   that function, and always refer to them as FRIBIDIG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define FRIBIDIG(v) TSRMG(fribidi_globals_id, php_fribidi_globals *, v)
#else
#define FRIBIDIG(v) (fribidi_globals.v)
#endif

#else

#define phpext_fribidi_ptr NULL

#endif

#endif	/* PHP_FRIBIDI_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
