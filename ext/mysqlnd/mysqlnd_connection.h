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

#ifndef MYSQLND_CONNECTION_H
#define MYSQLND_CONNECTION_H

PHPAPI extern const char * const mysqlnd_out_of_sync;
PHPAPI extern const char * const mysqlnd_server_gone;
PHPAPI extern const char * const mysqlnd_out_of_memory;


void mysqlnd_upsert_status_init(MYSQLND_UPSERT_STATUS * const upsert_status);

#define UPSERT_STATUS_RESET(status)							(status)->m->reset((status))

#define UPSERT_STATUS_GET_SERVER_STATUS(status)				(status)->server_status
#define UPSERT_STATUS_SET_SERVER_STATUS(status, server_st)	(status)->server_status = (server_st)

#define UPSERT_STATUS_GET_WARNINGS(status)					(status)->warning_count
#define UPSERT_STATUS_SET_WARNINGS(status, warnings)		(status)->warning_count = (warnings)

#define UPSERT_STATUS_GET_AFFECTED_ROWS(status)				(status)->affected_rows
#define UPSERT_STATUS_SET_AFFECTED_ROWS(status, rows)		(status)->affected_rows = (rows)
#define UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(status)	(status)->m->set_affected_rows_to_error((status))

#define UPSERT_STATUS_GET_LAST_INSERT_ID(status)			(status)->last_insert_id
#define UPSERT_STATUS_SET_LAST_INSERT_ID(status, id)		(status)->last_insert_id = (id)

PHPAPI void mysqlnd_error_info_init(MYSQLND_ERROR_INFO * const info, const bool persistent);
PHPAPI void	mysqlnd_error_info_free_contents(MYSQLND_ERROR_INFO * const info);

#define GET_CONNECTION_STATE(state_struct)		(state_struct)->m->get((state_struct))
#define SET_CONNECTION_STATE(state_struct, s)	(state_struct)->m->set((state_struct), (s))

PHPAPI void mysqlnd_connection_state_init(struct st_mysqlnd_connection_state * const state);

#endif /* MYSQLND_CONNECTION_H */
