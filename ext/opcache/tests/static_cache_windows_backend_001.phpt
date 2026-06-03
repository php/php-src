--TEST--
OPcache static cache uses the Win32 shared memory and lock backend
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') {
	die('skip Windows only');
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

$volatileInfo = OPcache\VolatileCache::info();
$stableInfo = OPcache\StableCache::info();

var_dump($volatileInfo->available);
var_dump($stableInfo->available);
var_dump($volatileInfo->shared_model);
var_dump($stableInfo->shared_model);
var_dump($volatileInfo->shared_memory);
var_dump($stableInfo->shared_memory);

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

$key = 'windows-backend';

var_dump(OPcache\VolatileCache::getInstance('default')->lock($key));
var_dump(OPcache\VolatileCache::getInstance('default')->store($key, ['backend' => 'volatile']));
var_dump(OPcache\StableCache::getInstance('default')->lock($key));
OPcache\StableCache::getInstance('default')->store($key, ['backend' => 'stable']);

var_dump(OPcache\VolatileCache::getInstance('default')->fetch($key));
var_dump(OPcache\StableCache::getInstance('default')->fetch($key));

?>
--EXPECT--
bool(true)
bool(true)
string(5) "win32"
string(5) "win32"
int(33554432)
int(33554432)
bool(true)
bool(true)
bool(true)
array(1) {
  ["backend"]=>
  string(8) "volatile"
}
array(1) {
  ["backend"]=>
  string(6) "stable"
}
