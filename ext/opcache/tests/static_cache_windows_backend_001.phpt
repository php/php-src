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

$volatileInfo = OPcache\volatile_cache_info();
$pinnedInfo = OPcache\pinned_cache_info();

var_dump($volatileInfo->available);
var_dump($pinnedInfo->available);
var_dump($volatileInfo->shared_model);
var_dump($pinnedInfo->shared_model);
var_dump($volatileInfo->shared_memory);
var_dump($pinnedInfo->shared_memory);

OPcache\volatile_clear();
OPcache\pinned_clear();

$key = 'windows-backend';

var_dump(OPcache\volatile_lock($key));
var_dump(OPcache\volatile_store($key, ['backend' => 'volatile']));
var_dump(OPcache\pinned_lock($key));
OPcache\pinned_store($key, ['backend' => 'pinned']);

var_dump(OPcache\volatile_fetch($key));
var_dump(OPcache\pinned_fetch($key));

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
  string(10) "pinned"
}
