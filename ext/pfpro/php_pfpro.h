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
   | Author: David Croft <david@infotrek.co.uk>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef _PHP_PFPRO_H
#define _PHP_PFPRO_H

#if HAVE_PFPRO

extern zend_module_entry pfpro_module_entry;
#define phpext_pfpro_ptr &pfpro_module_entry

#ifdef PHP_WIN32
#define PHP_PFPRO_API __declspec(dllexport)
#else
#define PHP_PFPRO_API
#endif

PHP_MINIT_FUNCTION(pfpro);
PHP_MSHUTDOWN_FUNCTION(pfpro);
PHP_RINIT_FUNCTION(pfpro);
PHP_RSHUTDOWN_FUNCTION(pfpro);
PHP_MINFO_FUNCTION(pfpro);

PHP_FUNCTION(pfpro_version);	        /* Return library version     */
PHP_FUNCTION(pfpro_init);               /* Initialise pfpro gateway   */
PHP_FUNCTION(pfpro_cleanup);            /* Shut down cleanly          */
PHP_FUNCTION(pfpro_process_raw);        /* Raw transaction processing */
PHP_FUNCTION(pfpro_process);            /* Transaction processing     */

typedef struct {
	int le_pfpro;
	int initialised;
} php_pfpro_globals;

#ifdef ZTS
#define PFPROG(v) (pfpro_globals->v)
#define PFPROLS_FETCH() php_pfpro_globals *pfpro_globals = ts_resource(gd_pfpro_id)
#else
#define PFPROG(v) (pfpro_globals.v)
#define PFPROLS_FETCH()
#endif

#else

#define phpext_pfpro_ptr NULL

#endif

#endif	/* _PHP_PFPRO_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
