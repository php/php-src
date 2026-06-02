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
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

$volatileInfo = OPcache\VolatileCache::info();
$pinnedInfo = OPcache\PinnedCache::info();

var_dump($volatileInfo->available);
var_dump($pinnedInfo->available);
var_dump($volatileInfo->shared_model);
var_dump($pinnedInfo->shared_model);
var_dump($volatileInfo->shared_memory);
var_dump($pinnedInfo->shared_memory);

OPcache\VolatileCache::clear();
OPcache\PinnedCache::clear();

$key = 'windows-backend';

var_dump(OPcache\VolatileCache::lock($key));
var_dump(OPcache\VolatileCache::set($key, ['backend' => 'volatile']));
var_dump(OPcache\PinnedCache::lock($key));
OPcache\PinnedCache::set($key, ['backend' => 'pinned']);

var_dump(OPcache\VolatileCache::get($key));
var_dump(OPcache\PinnedCache::get($key));

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
  string(6) "pinned"
}
