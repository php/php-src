/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifndef PHP_FAM_H
#define PHP_FAM_H

extern zend_module_entry fam_module_entry;
#define phpext_fam_ptr &fam_module_entry

#ifdef PHP_WIN32
#define PHP_FAM_API __declspec(dllexport)
#else
#define PHP_FAM_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(fam);
PHP_MSHUTDOWN_FUNCTION(fam);
PHP_RINIT_FUNCTION(fam);
PHP_RSHUTDOWN_FUNCTION(fam);
PHP_MINFO_FUNCTION(fam);

PHP_FUNCTION(fam_open);
PHP_FUNCTION(fam_close);
PHP_FUNCTION(fam_monitor_directory);
PHP_FUNCTION(fam_monitor_file);
PHP_FUNCTION(fam_monitor_collection);
PHP_FUNCTION(fam_suspend_monitor);
PHP_FUNCTION(fam_resume_monitor);
PHP_FUNCTION(fam_cancel_monitor);
PHP_FUNCTION(fam_pending);
PHP_FUNCTION(fam_next_event);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(fam)
	int   global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(fam)
*/

/* In every utility function you add that needs to use variables 
   in php_fam_globals, call TSRM_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as FAM_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define FAM_G(v) TSRMG(fam_globals_id, zend_fam_globals *, v)
#else
#define FAM_G(v) (fam_globals.v)
#endif

#endif	/* PHP_FAM_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
