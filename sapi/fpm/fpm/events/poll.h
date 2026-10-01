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
   | Authors: Jerome Loyet <jerome@loyet.net>                             |
   +----------------------------------------------------------------------+
*/

#ifndef FPM_EVENTS_POLL_H
#define FPM_EVENTS_POLL_H

#include "../fpm_config.h"
#include "../fpm_events.h"

struct fpm_event_module_s *fpm_event_poll_module(void);

#endif /* FPM_EVENTS_POLL_H */
