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
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_ZIP_H
#define PHP_ZIP_H

#if HAVE_ZZIPLIB

#include <zziplib.h>

extern zend_module_entry zip_module_entry;
#define phpext_zip_ptr &zip_module_entry

#define PHP_ZZIPLIB_API
#ifdef PHP_WIN32
#undef PHP_ZZIPLIB_API
#ifdef ZIP_EXPORTS
#define PHP_ZZIPLIB_API __declspec(dllexport)
#else
#define PHP_ZZIPLIB_API __declspec(dllimport)
#endif
#endif

PHP_MINIT_FUNCTION(zip);
PHP_MINFO_FUNCTION(zip);

PHP_FUNCTION(zip_open);
PHP_FUNCTION(zip_read);
PHP_FUNCTION(zip_close);
PHP_FUNCTION(zip_entry_name);
PHP_FUNCTION(zip_entry_compressedsize);
PHP_FUNCTION(zip_entry_filesize);
PHP_FUNCTION(zip_entry_compressionmethod);
PHP_FUNCTION(zip_entry_open);
PHP_FUNCTION(zip_entry_read);
PHP_FUNCTION(zip_entry_close);

typedef struct {
    ZZIP_FILE   *fp;
    ZZIP_DIRENT  dirent;
} php_zzip_dirent;

#else
#define phpext_zziplib_ptr NULL
#endif

#endif	/* PHP_ZZIPLIB_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
