/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Pratik Bhujel <prateekbhujelpb@gmail.com>                    |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IO_TERMINAL_H
#define PHP_IO_TERMINAL_H

#include "php.h"

PHP_MINIT_FUNCTION(terminal);
PHP_MSHUTDOWN_FUNCTION(terminal);
PHP_RINIT_FUNCTION(terminal);
PHP_RSHUTDOWN_FUNCTION(terminal);

#endif /* PHP_IO_TERMINAL_H */
