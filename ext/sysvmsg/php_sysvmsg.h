/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
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
