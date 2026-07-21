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
*/

#ifndef PHP_IO_POLL_H
#define PHP_IO_POLL_H

#include "php.h"

BEGIN_EXTERN_C()

PHPAPI void php_io_poll_stream_notify_close(struct _php_stream *stream);

END_EXTERN_C()

#endif /* PHP_IO_POLL_H */
