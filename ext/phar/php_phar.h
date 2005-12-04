/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PHAR_H
#define PHP_PHAR_H

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#include "zend_hash.h"
#include "ext/standard/basic_functions.h"
extern zend_module_entry phar_module_entry;
#define phpext_phar_ptr &phar_module_entry

#ifdef PHP_WIN32
#define PHP_PHAR_API __declspec(dllexport)
#else
#define PHP_PHAR_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(phar);
PHP_MSHUTDOWN_FUNCTION(phar);
PHP_RINIT_FUNCTION(phar);
PHP_RSHUTDOWN_FUNCTION(phar);
PHP_MINFO_FUNCTION(phar);

typedef struct _phar_manifest_entry {
	php_uint32	filename_len;
	char		*filename;
	php_uint32	uncompressed_filesize;
	php_uint32	timestamp;
	php_uint32	offset_within_phar;
	php_uint32	compressed_filesize;
} phar_manifest_entry;

typedef struct _phar_file_data {
	char		*file;
	HashTable	*manifest;
	char		*alias;
	int		alias_len;
	size_t		internal_file_start;
	zend_bool	is_compressed;
} phar_file_data;

typedef struct _phar_dir_entry {
	char		*entry;
	struct _phar_dir_entry	*next;
} phar_dir_entry;

typedef struct _phar_dir_data {
	phar_dir_entry *current;
} phar_dir_data;

ZEND_BEGIN_MODULE_GLOBALS(phar)
	HashTable	phar_data;
ZEND_END_MODULE_GLOBALS(phar)

/* In every utility function you add that needs to use variables 
   in php_phar_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PHAR_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#	include "TSRM.h"
#	define PHAR_G(v) TSRMG(phar_globals_id, zend_phar_globals *, v)
#	define PHAR_GLOBALS ((zend_phar_globals *) (*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(phar_globals_id)])
#else
#	define PHAR_G(v) (phar_globals.v)
#	define PHAR_GLOBALS (&phar_globals)
#endif

#endif	/* PHP_PHAR_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
