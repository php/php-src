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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_VIO_H
#define MYSQLND_VIO_H

PHPAPI MYSQLND_VIO * mysqlnd_vio_init(bool persistent, MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *object_factory, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);
PHPAPI void mysqlnd_vio_free(MYSQLND_VIO * const vio, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);

#endif /* MYSQLND_VIO_H */
