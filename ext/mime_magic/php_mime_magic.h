/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifndef PHP_MIME_MAGIC_H
#define PHP_MIME_MAGIC_H

extern zend_module_entry mime_magic_module_entry;
#define phpext_mime_magic_ptr &mime_magic_module_entry

#ifdef PHP_WIN32
#define PHP_MIME_MAGIC_API __declspec(dllexport)
#else
#define PHP_MIME_MAGIC_API
#endif

PHP_MINIT_FUNCTION(mime_magic);
PHP_MSHUTDOWN_FUNCTION(mime_magic);
PHP_RINIT_FUNCTION(mime_magic);
PHP_RSHUTDOWN_FUNCTION(mime_magic);
PHP_MINFO_FUNCTION(mime_magic);

PHP_FUNCTION(mime_content_type);	

#endif	/* PHP_MIME_MAGIC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
