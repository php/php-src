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

#include "php.h"
#include "php_ini.h"

#include "fpm.h"
#include "fpm_worker_pool.h"
#include "fpm_user_cache.h"
#include "zlog.h"

#include "ext/user_cache/php_user_cache.h"

typedef struct {
	const char *name;
	zend_ini_entry *entry;
	zend_string *pool_value;
} fpm_ucache_ini;

static void fpm_ucache_apply_pool_values(
		struct key_value_s *values,
		fpm_ucache_ini *settings,
		size_t count)
{
	struct key_value_s *kv;
	zend_ini_entry *entry;
	zend_string *value;
	size_t i;

	for (kv = values; kv != NULL; kv = kv->next) {
		for (i = 0; i < count; i++) {
			if (strcmp(kv->key, settings[i].name) != 0) {
				continue;
			}

			entry = settings[i].entry;
			if (entry == NULL || entry->on_modify == NULL) {
				break;
			}

			value = zend_string_init(kv->value, strlen(kv->value), 1);
			GC_MAKE_PERSISTENT_LOCAL(value);
			if (entry->on_modify(entry, value, entry->mh_arg1, entry->mh_arg2,
					entry->mh_arg3, PHP_INI_STAGE_ACTIVATE) == SUCCESS) {
				if (settings[i].pool_value != NULL) {
					zend_string_release_ex(settings[i].pool_value, 1);
				}
				settings[i].pool_value = value;
			} else {
				zend_string_release_ex(value, 1);
			}
			break;
		}
	}
}

static void fpm_ucache_startup_pool(struct fpm_worker_pool_s *wp, bool opted_in)
{
	fpm_ucache_ini settings[] = {
		{ "user_cache.enable", NULL, NULL },
		{ "user_cache.shm_size", NULL, NULL },
		{ "user_cache.entries_hint", NULL, NULL },
		{ "user_cache.preferred_memory_model", NULL, NULL },
		{ "user_cache.lockfile_path", NULL, NULL },
	};
	const size_t count = sizeof(settings) / sizeof(settings[0]);
	zend_string *master_value;
	zend_ini_entry *entry;
	size_t i;
	bool pool_enabled, restored;

	for (i = 0; i < count; i++) {
		settings[i].entry = zend_hash_str_find_ptr(
			EG(ini_directives), settings[i].name, strlen(settings[i].name));
	}

	fpm_ucache_apply_pool_values(wp->config->php_values, settings, count);
	fpm_ucache_apply_pool_values(wp->config->php_admin_values, settings, count);

	pool_enabled = php_ucache_is_enabled_by_ini();
	if (!opted_in) {
		if (pool_enabled) {
			zlog(ZLOG_WARNING, "[pool %s] unable to register UserCache request mode; UserCache will be unavailable", wp->config->name);
		}
	} else {
		wp->ucache_partition = php_ucache_partition_create(wp->config->name);
		if (wp->ucache_partition == NULL) {
			if (pool_enabled) {
				zlog(ZLOG_WARNING, "[pool %s] unable to allocate UserCache partition; UserCache will be unavailable", wp->config->name);
			}
		} else {
			php_ucache_partition_set_max_procs(wp->ucache_partition, (uint32_t) wp->config->pm_max_children);

			if (!php_ucache_partition_startup_storage(wp->ucache_partition) && pool_enabled) {
				zlog(ZLOG_WARNING, "[pool %s] UserCache partition startup failed; UserCache will be unavailable", wp->config->name);
			}
		}
	}

	for (i = 0; i < count; i++) {
		if (settings[i].pool_value == NULL) {
			continue;
		}

		entry = settings[i].entry;
		master_value = entry->value;

		restored = entry->on_modify(
			entry,
			master_value,
			entry->mh_arg1,
			entry->mh_arg2,
			entry->mh_arg3,
			PHP_INI_STAGE_DEACTIVATE
		) == SUCCESS;
		ZEND_ASSERT(restored);

		zend_string_release_ex(settings[i].pool_value, 1);
	}
}

int fpm_ucache_init_main(void)
{
	struct fpm_worker_pool_s *wp;
	bool opted_in = php_ucache_opt_in(PHP_UCACHE_MODE_REQ) == SUCCESS;

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		fpm_ucache_startup_pool(wp, opted_in);
	}

	return 0;
}
