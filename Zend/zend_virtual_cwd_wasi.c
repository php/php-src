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
*/

#include "zend.h"
#include "zend_virtual_cwd.h"

CWD_API int virtual_chmod(const char *filename, mode_t mode) /* {{{ */
{
  return -1;
}
/* }}} */

CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group, int link) /* {{{ */
{
  return -1;
}
/* }}} */

CWD_API FILE *virtual_popen(const char *command, const char *type) /* {{{ */
{
  return NULL;
}
/* }}} */
