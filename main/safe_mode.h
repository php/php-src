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
*/

/* $Id$ */

#ifndef SAFE_MODE_H
#define SAFE_MODE_H

/* mode's for php_checkuid() */
#define CHECKUID_DISALLOW_FILE_NOT_EXISTS 0
#define CHECKUID_ALLOW_FILE_NOT_EXISTS 1
#define CHECKUID_CHECK_FILE_AND_DIR 2
#define CHECKUID_ALLOW_ONLY_DIR 3
#define CHECKUID_CHECK_MODE_PARAM 4
#define CHECKUID_ALLOW_ONLY_FILE 5

/* flags for php_checkuid_ex() */
#define CHECKUID_NO_ERRORS	0x01

extern PHPAPI int php_checkuid(const char *filename, char *fopen_mode, int mode);
extern PHPAPI int php_checkuid_ex(const char *filename, char *fopen_mode, int mode, int flags);
extern PHPAPI char *php_get_current_user(void);

#endif
