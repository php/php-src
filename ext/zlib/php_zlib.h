/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stefan Röhrich <sr@linux.de>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_ZLIB_H
#define PHP_ZLIB_H

#if HAVE_ZLIB

typedef struct {
	int gzgetss_state;
} php_zlib_globals;

extern zend_module_entry php_zlib_module_entry;
#define zlib_module_ptr &php_zlib_module_entry

PHP_MINIT_FUNCTION(zlib);
PHP_MSHUTDOWN_FUNCTION(zlib);
PHP_MINFO_FUNCTION(zlib);
PHP_FUNCTION(gzopen);
PHP_FUNCTION(gzclose);
PHP_FUNCTION(gzeof);
PHP_FUNCTION(gzread);
PHP_FUNCTION(gzgetc);
PHP_FUNCTION(gzgets);
PHP_FUNCTION(gzgetss);
PHP_FUNCTION(gzwrite);
PHP_FUNCTION(gzrewind);
PHP_FUNCTION(gztell);
PHP_FUNCTION(gzseek);
PHP_FUNCTION(gzpassthru);
PHP_FUNCTION(readgzfile);
PHP_FUNCTION(gzfile);
PHP_FUNCTION(gzcompress);
PHP_FUNCTION(gzuncompress);

#ifdef ZTS
#define ZLIBLS_D php_zlib_globals *zlib_globals
#define ZLIBG(v) (zlib_globals->v)
#define ZLIBLS_FETCH() php_zlib_globals *zlib_globals = ts_resource(zlib_globals_id)
#else
#define ZLIBLS_D
#define ZLIBG(v) (zlib_globals.v)
#define ZLIBLS_FETCH()
#endif

#else
#define zlib_module_ptr NULL
#endif /* HAVE_ZLIB */

#define phpext_zlib_ptr zlib_module_ptr

#endif /* PHP_ZLIB_H */
