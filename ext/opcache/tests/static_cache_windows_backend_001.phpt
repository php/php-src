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
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

$volatileInfo = OPcache\volatile_cache_info();
$persistentInfo = OPcache\persistent_cache_info();

var_dump($volatileInfo->available);
var_dump($persistentInfo->available);
var_dump($volatileInfo->shared_model);
var_dump($persistentInfo->shared_model);
var_dump($volatileInfo->shared_memory);
var_dump($persistentInfo->shared_memory);

OPcache\volatile_clear();
OPcache\persistent_clear();

$key = 'windows-backend';

var_dump(OPcache\volatile_lock($key));
var_dump(OPcache\volatile_store($key, ['backend' => 'volatile']));
var_dump(OPcache\persistent_lock($key));
OPcache\persistent_store($key, ['backend' => 'persistent']);

var_dump(OPcache\volatile_fetch($key));
var_dump(OPcache\persistent_fetch($key));

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
  string(10) "persistent"
}
