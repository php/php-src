/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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

#include "zend_accelerator_api.h"
#include "ZendAccelerator.h"

ZEND_API bool opcache_preloading(void)
{
	return ZCG(preloading);
}

ZEND_API bool opcache_shm_read_locked(void)
{
	return ZCG(counted);
}

ZEND_API time_t opcache_last_restart_time(void)
{
	if (!accel_startup_ok) {
		return 0;
	}

	return ZCSG(last_restart_time);
}
