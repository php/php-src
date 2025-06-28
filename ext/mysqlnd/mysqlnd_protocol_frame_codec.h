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

#ifndef MYSQLND_PROTOCOL_FRAME_CODEC_H
#define MYSQLND_PROTOCOL_FRAME_CODEC_H

PHPAPI MYSQLND_PFC * mysqlnd_pfc_init(const bool persistent, MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *object_factory, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);
PHPAPI void mysqlnd_pfc_free(MYSQLND_PFC * const pfc, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);

#endif /* MYSQLND_PROTOCOL_FRAME_CODEC_H */
