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
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_DBASE_H
#define PHP_DBASE_H
#if DBASE
extern zend_module_entry dbase_module_entry;
#define dbase_module_ptr &dbase_module_entry

PHP_MINIT_FUNCTION(dbase);
PHP_FUNCTION(dbase_open);
PHP_FUNCTION(dbase_create);
PHP_FUNCTION(dbase_close);
PHP_FUNCTION(dbase_numrecords);
PHP_FUNCTION(dbase_numfields);
PHP_FUNCTION(dbase_add_record);
PHP_FUNCTION(dbase_get_record);
PHP_FUNCTION(dbase_delete_record);
PHP_FUNCTION(dbase_pack);
PHP_FUNCTION(dbase_get_record_with_names);
#else
#define dbase_module_ptr NULL
#endif

#define phpext_dbase_ptr dbase_module_ptr

#endif /* PHP_DBASE_H */
