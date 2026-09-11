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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "fpm_config.h"

#include "fpm.h"
#include "fpm_worker_pool.h"
#include "fpm_user_cache.h"
#include "zlog.h"

#include "ext/user_cache/php_user_cache.h"

int fpm_user_cache_init_main(void)
{
	struct fpm_worker_pool_s *wp;

	php_user_cache_opt_in();

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		wp->user_cache_partition = php_user_cache_partition_create(wp->config->name);
		if (wp->user_cache_partition == NULL) {
			zlog(ZLOG_ERROR, "[pool %s] unable to allocate UserCache partition", wp->config->name);
			return -1;
		}

		if (!php_user_cache_partition_startup_storage(wp->user_cache_partition)) {
			zlog(ZLOG_WARNING, "[pool %s] UserCache partition startup failed; UserCache will be unavailable", wp->config->name);
		}
	}

	return 0;
}
