--TEST--
OPcache volatile cache status and info surface
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

$config = opcache_get_configuration();
$status = opcache_get_status();
$info = OPcache\volatile_cache_info();

var_dump($config['directives']['opcache.static_cache.volatile_size_mb']);
var_dump($status['volatile_cache']->enabled);
var_dump($status['volatile_cache']->available);
var_dump($status['volatile_cache']->startup_failed);
var_dump($status['volatile_cache']->backend_initialized);
var_dump($status['volatile_cache']->configured_memory);
var_dump($status['volatile_cache']->shared_memory);
var_dump($status['volatile_cache']->segment_count > 0);
var_dump(is_string($status['volatile_cache']->shared_model));
var_dump($info == $status['volatile_cache']);
var_dump($info instanceof OPcache\StaticCacheInfo);
var_dump((new ReflectionClass($info))->isReadOnly());
var_dump(property_exists($info, 'failure_reason'));
var_dump($info->failure_reason);
try {
	$info->entry_count = 0;
} catch (Error $e) {
	echo $e->getMessage(), "\n";
}
try {
	new OPcache\StaticCacheInfo();
} catch (Error $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
int(33554432)
bool(true)
bool(true)
bool(false)
bool(true)
int(33554432)
int(33554432)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
NULL
Cannot modify readonly property OPcache\StaticCacheInfo::$entry_count
Call to private OPcache\StaticCacheInfo::__construct() from global scope
