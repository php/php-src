/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Keyur Govande <kgovande@gmail.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef	PS_TITLE_HEADER
#define	PS_TITLE_HEADER

#define PS_TITLE_SUCCESS 0
#define PS_TITLE_NOT_AVAILABLE 1
#define PS_TITLE_NOT_INITIALIZED 2
#define PS_TITLE_BUFFER_NOT_AVAILABLE 3
#define PS_TITLE_WINDOWS_ERROR 4

extern char** save_ps_args(int argc, char** argv);

extern int set_ps_title(const char* new_str);

extern int get_ps_title(int* displen, const char** string);

extern const char* ps_title_errno(int rc);

extern int is_ps_title_available();

extern void cleanup_ps_args(char **argv);

#endif // PS_TITLE_HEADER
