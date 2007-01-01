/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Chad Robinson <chadr@brttech.com>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
  filePro 4.x support developed by Chad Robinson, chadr@brttech.com
  Contact Chad Robinson at BRT Technical Services Corp. for details.
  filePro is a registered trademark by Fiserv, Inc.  This file contains
  no code or information that is not freely available from the filePro
  web site at http://www.fileproplus.com/
*/

#ifndef PHP_FILEPRO_H
#define PHP_FILEPRO_H
#if HAVE_FILEPRO
extern zend_module_entry filepro_module_entry;
#define phpext_filepro_ptr &filepro_module_entry

PHP_FUNCTION(filepro);
PHP_FUNCTION(filepro_rowcount);
PHP_FUNCTION(filepro_fieldname);
PHP_FUNCTION(filepro_fieldtype);
PHP_FUNCTION(filepro_fieldwidth);
PHP_FUNCTION(filepro_fieldcount);
PHP_FUNCTION(filepro_retrieve);

PHP_MINIT_FUNCTION(filepro);
PHP_RINIT_FUNCTION(filepro);
PHP_RSHUTDOWN_FUNCTION(filepro);
PHP_MSHUTDOWN_FUNCTION(filepro);
#else
#define phpext_filepro_ptr NULL
#endif
#endif /* PHP_FILEPRO_H */
