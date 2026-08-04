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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SYSVMSG_H
#define PHP_SYSVMSG_H

#ifdef HAVE_SYSVMSG

extern zend_module_entry sysvmsg_module_entry;
#define phpext_sysvmsg_ptr &sysvmsg_module_entry

#include "php_version.h"
#define PHP_SYSVMSG_VERSION PHP_VERSION

#endif /* HAVE_SYSVMSG */

/* In order to detect MSG_EXCEPT use at run time; we have no way
 * of knowing what the bit definitions are, so we can't just define
 * our own MSG_EXCEPT value. */
#define PHP_MSG_IPC_NOWAIT  1
#define PHP_MSG_NOERROR     2
#define PHP_MSG_EXCEPT      4

#endif	/* PHP_SYSVMSG_H */
