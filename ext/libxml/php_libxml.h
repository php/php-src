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
   | Authors: Shane Caraveo <shane@php.net>                               |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LIBXML_H
#define PHP_LIBXML_H

#ifdef HAVE_LIBXML
extern zend_module_entry libxml_module_entry;
#define libxml_module_ptr &libxml_module_entry
#else
#define libxml_module_ptr NULL
#endif

#ifdef HAVE_LIBXML 

#ifdef PHP_WIN32
#define PHP_LIBXML_API __declspec(dllexport)
#else
#define PHP_LIBXML_API
#endif



typedef struct {
	zval *stream_context;
} php_libxml_globals;

	
PHP_FUNCTION(libxml_set_streams_context);

#endif /* HAVE_LIBXML */

#define phpext_libxml_ptr libxml_module_ptr

#ifdef ZTS
#define LIBXML(v) TSRMG(libxml_globals_id, php_libxml_globals *, v)
#else
#define LIBXML(v) (libxml_globals.v)
#endif

PHP_LIBXML_API void php_libxml_initialize();
PHP_LIBXML_API void php_libxml_shutdown();

#endif /* PHP_LIBXML_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
