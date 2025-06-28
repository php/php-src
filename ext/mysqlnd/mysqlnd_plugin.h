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

#ifndef MYSQLND_PLUGIN_H
#define MYSQLND_PLUGIN_H


void mysqlnd_plugin_subsystem_init(void);
void mysqlnd_plugin_subsystem_end(void);

void mysqlnd_register_builtin_authentication_plugins(void);

void mysqlnd_example_plugin_register(void);

#endif	/* MYSQLND_PLUGIN_H */
