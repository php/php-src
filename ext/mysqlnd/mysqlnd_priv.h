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

#ifndef MYSQLND_PRIV_H
#define MYSQLND_PRIV_H
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_object_factory);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_conn);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_conn_data);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_res);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_result_unbuffered);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_result_buffered);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_protocol_payload_decoder_factory);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_protocol_packet_frame_codec);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_vio);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_upsert_status);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_error_info);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_command);

enum_func_status mysqlnd_handle_local_infile(MYSQLND_CONN_DATA * conn, const char * const filename, bool * is_warning);
#endif	/* MYSQLND_PRIV_H */
