--TEST--
OPcache static cache disables the subsystem after startup failure
--EXTENSIONS--
opcache
--ENV--
OPCACHE_STATIC_CACHE_FORCE_STARTUP_FAILURE=1
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

$status = opcache_get_status();
$volatileInfo = OPcache\VolatileCache::info();
$pinnedInfo = OPcache\PinnedCache::info();

var_dump($status['volatile_cache']->enabled);
var_dump($status['volatile_cache']->available);
var_dump($status['volatile_cache']->startup_failed);
var_dump($status['volatile_cache']->backend_initialized);
var_dump($status['volatile_cache']->configured_memory);
var_dump($status['pinned_cache']->enabled);
var_dump($status['pinned_cache']->available);
var_dump($status['pinned_cache']->startup_failed);
var_dump($status['pinned_cache']->backend_initialized);
var_dump($status['pinned_cache']->configured_memory);
var_dump($volatileInfo == $status['volatile_cache']);
var_dump($pinnedInfo == $status['pinned_cache']);
var_dump($volatileInfo->failure_reason);
var_dump($pinnedInfo->failure_reason);
var_dump(OPcache\VolatileCache::set('key', 'value'));
var_dump(OPcache\PinnedCache::set('key', 'value'));

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
int(33554432)
bool(true)
bool(false)
bool(true)
bool(false)
int(33554432)
bool(true)
bool(true)
string(42) "Unable to initialize shared memory backend"
string(42) "Unable to initialize shared memory backend"
bool(false)
bool(false)
