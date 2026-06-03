--TEST--
OPcache static cache disabled backends report unavailable and explicit APIs return false by default
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=0
opcache.static_cache.stable_size_mb=0
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

dump_info('volatile', OPcache\VolatileCache::info());
dump_info('stable', OPcache\StableCache::info());

dump_result('VolatileCache::store', OPcache\VolatileCache::getInstance('default')->store('key', 'value'));
dump_result('VolatileCache::storeMultiple', OPcache\VolatileCache::getInstance('default')->storeMultiple(['array-key' => 'value']));
dump_result('VolatileCache::fetch', OPcache\VolatileCache::getInstance('default')->fetch('key', 'default'));
dump_result('VolatileCache::fetchMultiple', OPcache\VolatileCache::getInstance('default')->fetchMultiple(['key'], ['key' => 'default']));
dump_result('VolatileCache::has', OPcache\VolatileCache::getInstance('default')->has('key'));
dump_result('VolatileCache::lock', OPcache\VolatileCache::getInstance('default')->lock('key'));
dump_result('VolatileCache::unlock', OPcache\VolatileCache::getInstance('default')->unlock('key'));
dump_result('VolatileCache::delete', OPcache\VolatileCache::getInstance('default')->delete('key'));
dump_result('VolatileCache::deleteMultiple', OPcache\VolatileCache::getInstance('default')->deleteMultiple(['key']));
dump_result('opcache_static_cache_volatile_reset', opcache_static_cache_volatile_reset());
dump_result('StableCache::store', OPcache\StableCache::getInstance('default')->store('key', 'value'));
dump_result('StableCache::storeMultiple', OPcache\StableCache::getInstance('default')->storeMultiple(['array-key' => 'value']));
dump_result('StableCache::fetch', OPcache\StableCache::getInstance('default')->fetch('key', 'default'));
dump_result('StableCache::fetchMultiple', OPcache\StableCache::getInstance('default')->fetchMultiple(['key'], ['key' => 'default']));
dump_result('StableCache::has', OPcache\StableCache::getInstance('default')->has('key'));
dump_result('StableCache::lock', OPcache\StableCache::getInstance('default')->lock('key'));
dump_result('StableCache::unlock', OPcache\StableCache::getInstance('default')->unlock('key'));
dump_result('StableCache::delete', OPcache\StableCache::getInstance('default')->delete('key'));
dump_result('StableCache::deleteMultiple', OPcache\StableCache::getInstance('default')->deleteMultiple(['key']));
dump_result('StableCache::clear', OPcache\StableCache::getInstance('default')->clear());
dump_result('StableCache::increment', OPcache\StableCache::getInstance('default')->increment('key'));
dump_result('StableCache::decrement', OPcache\StableCache::getInstance('default')->decrement('key'));

?>
--EXPECT--
volatile
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
NULL
stable
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
NULL
VolatileCache::store: bool(false)
VolatileCache::storeMultiple: bool(false)
VolatileCache::fetch: bool(false)
VolatileCache::fetchMultiple: bool(false)
VolatileCache::has: bool(false)
VolatileCache::lock: bool(false)
VolatileCache::unlock: bool(false)
VolatileCache::delete: bool(false)
VolatileCache::deleteMultiple: bool(false)
opcache_static_cache_volatile_reset: bool(false)
StableCache::store: bool(false)
StableCache::storeMultiple: bool(false)
StableCache::fetch: bool(false)
StableCache::fetchMultiple: bool(false)
StableCache::has: bool(false)
StableCache::lock: bool(false)
StableCache::unlock: bool(false)
StableCache::delete: bool(false)
StableCache::deleteMultiple: bool(false)
StableCache::clear: bool(false)
StableCache::increment: bool(false)
StableCache::decrement: bool(false)
