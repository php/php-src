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
$volatileInfo = OPcache\volatile_cache_info();
$pinnedInfo = OPcache\pinned_cache_info();

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

try {
	OPcache\volatile_store('key', 'value');
} catch (Throwable $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	OPcache\pinned_store('key', 'value');
} catch (Throwable $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(false)
int(33554432)
bool(false)
bool(false)
bool(true)
bool(false)
int(33554432)
bool(true)
bool(true)
string(42) "Unable to initialize shared memory backend"
string(42) "Unable to initialize shared memory backend"
OPcache\StaticCacheException: Unable to initialize shared memory backend
OPcache\StaticCacheException: Unable to initialize shared memory backend
