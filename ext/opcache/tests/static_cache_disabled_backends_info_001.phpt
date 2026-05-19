--TEST--
OPcache static cache disabled backends report unavailable and explicit APIs return false by default
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=0
opcache.static_cache.pinned_size_mb=0
--FILE--
<?php

function dump_info(string $label, OPcache\StaticCacheInfo $info): void
{
	echo $label, "\n";
	var_dump($info->enabled);
	var_dump($info->available);
	var_dump($info->startup_failed);
	var_dump($info->backend_initialized);
	var_dump($info->configured_memory);
	var_dump($info->failure_reason);
}

function dump_result(string $label, mixed $value): void
{
	echo $label, ': ';
	var_dump($value);
}

function dump_static_cache_exception(string $label, Closure $callback): void
{
	try {
		$callback();
		echo $label, ": no exception\n";
	} catch (OPcache\StaticCacheException $exception) {
		echo $label, ': ', get_class($exception), ': ', $exception->getMessage(), "\n";
	}
}

dump_info('volatile', OPcache\volatile_cache_info());
dump_info('pinned', OPcache\pinned_cache_info());

dump_result('volatile_store', OPcache\volatile_store('key', 'value'));
dump_result('volatile_store_array', OPcache\volatile_store_array(['array-key' => 'value']));
dump_result('volatile_fetch', OPcache\volatile_fetch('key', 'default'));
dump_result('volatile_fetch_array', OPcache\volatile_fetch_array(['key'], ['key' => 'default']));
dump_result('volatile_exists', OPcache\volatile_exists('key'));
dump_result('volatile_lock', OPcache\volatile_lock('key'));
dump_result('volatile_unlock', OPcache\volatile_unlock('key'));
dump_result('volatile_delete', OPcache\volatile_delete('key'));
dump_result('volatile_delete_array', OPcache\volatile_delete_array(['key']));
dump_result('volatile_clear', OPcache\volatile_clear());
dump_result('pinned_store', OPcache\pinned_store('key', 'value'));
dump_result('pinned_store_array', OPcache\pinned_store_array(['array-key' => 'value']));
dump_result('pinned_fetch', OPcache\pinned_fetch('key', 'default'));
dump_result('pinned_fetch_array', OPcache\pinned_fetch_array(['key'], ['key' => 'default']));
dump_result('pinned_exists', OPcache\pinned_exists('key'));
dump_result('pinned_lock', OPcache\pinned_lock('key'));
dump_result('pinned_unlock', OPcache\pinned_unlock('key'));
dump_result('pinned_delete', OPcache\pinned_delete('key'));
dump_result('pinned_delete_array', OPcache\pinned_delete_array(['key']));
dump_result('pinned_clear', OPcache\pinned_clear());
dump_result('pinned_atomic_increment', OPcache\pinned_atomic_increment('key'));
dump_result('pinned_atomic_decrement', OPcache\pinned_atomic_decrement('key'));

dump_static_cache_exception('volatile_store_throw', static fn () => OPcache\volatile_store('key', 'value', 0, true));
dump_static_cache_exception('volatile_store_array_throw', static fn () => OPcache\volatile_store_array(['array-key' => 'value'], 0, true));
dump_static_cache_exception('volatile_fetch_throw', static fn () => OPcache\volatile_fetch('key', 'default', true));
dump_static_cache_exception('volatile_fetch_array_throw', static fn () => OPcache\volatile_fetch_array(['key'], ['key' => 'default'], true));
dump_static_cache_exception('volatile_exists_throw', static fn () => OPcache\volatile_exists('key', true));
dump_static_cache_exception('volatile_lock_throw', static fn () => OPcache\volatile_lock('key', 0, true));
dump_static_cache_exception('volatile_unlock_throw', static fn () => OPcache\volatile_unlock('key', true));
dump_static_cache_exception('volatile_delete_throw', static fn () => OPcache\volatile_delete('key', true));
dump_static_cache_exception('volatile_delete_array_throw', static fn () => OPcache\volatile_delete_array(['key'], true));
dump_static_cache_exception('volatile_clear_throw', static fn () => OPcache\volatile_clear(true));
dump_static_cache_exception('pinned_store_throw', static fn () => OPcache\pinned_store('key', 'value', true));
dump_static_cache_exception('pinned_store_array_throw', static fn () => OPcache\pinned_store_array(['array-key' => 'value'], true));
dump_static_cache_exception('pinned_fetch_throw', static fn () => OPcache\pinned_fetch('key', 'default', true));
dump_static_cache_exception('pinned_fetch_array_throw', static fn () => OPcache\pinned_fetch_array(['key'], ['key' => 'default'], true));
dump_static_cache_exception('pinned_exists_throw', static fn () => OPcache\pinned_exists('key', true));
dump_static_cache_exception('pinned_lock_throw', static fn () => OPcache\pinned_lock('key', 0, true));
dump_static_cache_exception('pinned_unlock_throw', static fn () => OPcache\pinned_unlock('key', true));
dump_static_cache_exception('pinned_delete_throw', static fn () => OPcache\pinned_delete('key', true));
dump_static_cache_exception('pinned_delete_array_throw', static fn () => OPcache\pinned_delete_array(['key'], true));
dump_static_cache_exception('pinned_clear_throw', static fn () => OPcache\pinned_clear(true));
dump_static_cache_exception('pinned_atomic_increment_throw', static fn () => OPcache\pinned_atomic_increment('key', 1, true));
dump_static_cache_exception('pinned_atomic_decrement_throw', static fn () => OPcache\pinned_atomic_decrement('key', 1, true));

?>
--EXPECT--
volatile
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
NULL
pinned
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
NULL
volatile_store: bool(false)
volatile_store_array: bool(false)
volatile_fetch: bool(false)
volatile_fetch_array: bool(false)
volatile_exists: bool(false)
volatile_lock: bool(false)
volatile_unlock: bool(false)
volatile_delete: bool(false)
volatile_delete_array: bool(false)
volatile_clear: bool(false)
pinned_store: bool(false)
pinned_store_array: bool(false)
pinned_fetch: bool(false)
pinned_fetch_array: bool(false)
pinned_exists: bool(false)
pinned_lock: bool(false)
pinned_unlock: bool(false)
pinned_delete: bool(false)
pinned_delete_array: bool(false)
pinned_clear: bool(false)
pinned_atomic_increment: bool(false)
pinned_atomic_decrement: bool(false)
volatile_store_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_store_array_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_fetch_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_fetch_array_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_exists_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_lock_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_unlock_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_delete_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_delete_array_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
volatile_clear_throw: OPcache\StaticCacheException: OPcache volatile cache is disabled
pinned_store_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_store_array_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_fetch_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_fetch_array_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_exists_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_lock_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_unlock_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_delete_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_delete_array_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_clear_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_atomic_increment_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
pinned_atomic_decrement_throw: OPcache\StaticCacheException: OPcache pinned cache is disabled
