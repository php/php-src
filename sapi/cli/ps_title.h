/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Keyur Govande <kgovande@gmail.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef	PS_TITLE_HEADER
#define	PS_TITLE_HEADER

typedef enum {
	PS_TITLE_SUCCESS = 0,
	PS_TITLE_NOT_AVAILABLE = 1,
	PS_TITLE_NOT_INITIALIZED = 2,
	PS_TITLE_BUFFER_NOT_AVAILABLE = 3,
	PS_TITLE_WINDOWS_ERROR = 4,
	PS_TITLE_TOO_LONG = 5,
} ps_title_status;

extern char** save_ps_args(int argc, char** argv);

extern ps_title_status set_ps_title(const char *title, size_t title_len);

extern ps_title_status get_ps_title(size_t* displen, const char** string);

extern const char* ps_title_errno(ps_title_status rc);

extern ps_title_status is_ps_title_available(void);

extern void cleanup_ps_args(char **argv);

#endif // PS_TITLE_HEADER
