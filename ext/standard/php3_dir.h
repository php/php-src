/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | PHP 4.0 patches by Thies C. Arntzen (thies@digicol.de)               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP3_DIR_H
#define _PHP3_DIR_H
extern php3_module_entry php3_dir_module_entry;
#define php3_dir_module_ptr &php3_dir_module_entry

/* directory functions */
extern PHP_MINIT_FUNCTION(dir);
PHP_FUNCTION(opendir);
PHP_FUNCTION(closedir);
PHP_FUNCTION(chdir);
PHP_FUNCTION(rewinddir);
PHP_FUNCTION(readdir);
PHP_FUNCTION(getdir);

#define phpext_dir_ptr php3_dir_module_ptr

#endif /* _PHP3_DIR_H */
