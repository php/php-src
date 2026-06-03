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
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

$status = opcache_get_status();
$volatileInfo = OPcache\VolatileCache::info();
$stableInfo = OPcache\StableCache::info();

var_dump($status['volatile_cache']->enabled);
var_dump($status['volatile_cache']->available);
var_dump($status['volatile_cache']->startup_failed);
var_dump($status['volatile_cache']->backend_initialized);
var_dump($status['volatile_cache']->configured_memory);
var_dump($status['stable_cache']->enabled);
var_dump($status['stable_cache']->available);
var_dump($status['stable_cache']->startup_failed);
var_dump($status['stable_cache']->backend_initialized);
var_dump($status['stable_cache']->configured_memory);
var_dump($volatileInfo == $status['volatile_cache']);
var_dump($stableInfo == $status['stable_cache']);
var_dump($volatileInfo->failure_reason);
var_dump($stableInfo->failure_reason);
var_dump(OPcache\VolatileCache::getInstance('default')->store('key', 'value'));
var_dump(OPcache\StableCache::getInstance('default')->store('key', 'value'));

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
