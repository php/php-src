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

dump_info('volatile', OPcache\VolatileCache::info());
dump_info('pinned', OPcache\PinnedCache::info());

dump_result('VolatileCache::set', OPcache\VolatileCache::set('key', 'value'));
dump_result('VolatileCache::setMultiple', OPcache\VolatileCache::setMultiple(['array-key' => 'value']));
dump_result('VolatileCache::get', OPcache\VolatileCache::get('key', 'default'));
dump_result('VolatileCache::getMultiple', OPcache\VolatileCache::getMultiple(['key'], ['key' => 'default']));
dump_result('VolatileCache::has', OPcache\VolatileCache::has('key'));
dump_result('VolatileCache::lock', OPcache\VolatileCache::lock('key'));
dump_result('VolatileCache::unlock', OPcache\VolatileCache::unlock('key'));
dump_result('VolatileCache::delete', OPcache\VolatileCache::delete('key'));
dump_result('VolatileCache::deleteMultiple', OPcache\VolatileCache::deleteMultiple(['key']));
dump_result('VolatileCache::clear', OPcache\VolatileCache::clear());
dump_result('PinnedCache::set', OPcache\PinnedCache::set('key', 'value'));
dump_result('PinnedCache::setMultiple', OPcache\PinnedCache::setMultiple(['array-key' => 'value']));
dump_result('PinnedCache::get', OPcache\PinnedCache::get('key', 'default'));
dump_result('PinnedCache::getMultiple', OPcache\PinnedCache::getMultiple(['key'], ['key' => 'default']));
dump_result('PinnedCache::has', OPcache\PinnedCache::has('key'));
dump_result('PinnedCache::lock', OPcache\PinnedCache::lock('key'));
dump_result('PinnedCache::unlock', OPcache\PinnedCache::unlock('key'));
dump_result('PinnedCache::delete', OPcache\PinnedCache::delete('key'));
dump_result('PinnedCache::deleteMultiple', OPcache\PinnedCache::deleteMultiple(['key']));
dump_result('PinnedCache::clear', OPcache\PinnedCache::clear());
dump_result('PinnedCache::increment', OPcache\PinnedCache::increment('key'));
dump_result('PinnedCache::decrement', OPcache\PinnedCache::decrement('key'));

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
VolatileCache::set: bool(false)
VolatileCache::setMultiple: bool(false)
VolatileCache::get: bool(false)
VolatileCache::getMultiple: bool(false)
VolatileCache::has: bool(false)
VolatileCache::lock: bool(false)
VolatileCache::unlock: bool(false)
VolatileCache::delete: bool(false)
VolatileCache::deleteMultiple: bool(false)
VolatileCache::clear: bool(false)
PinnedCache::set: bool(false)
PinnedCache::setMultiple: bool(false)
PinnedCache::get: bool(false)
PinnedCache::getMultiple: bool(false)
PinnedCache::has: bool(false)
PinnedCache::lock: bool(false)
PinnedCache::unlock: bool(false)
PinnedCache::delete: bool(false)
PinnedCache::deleteMultiple: bool(false)
PinnedCache::clear: bool(false)
PinnedCache::increment: bool(false)
PinnedCache::decrement: bool(false)
