--TEST--
OPcache static cache attributes work with preloaded classes
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
opcache.preload={PWD}/static_cache_preload_001.inc
--FILE--
<?php

var_dump(StaticCachePreloadGlobalState::$values);
var_dump(StaticCachePreloadGlobalState::next());
var_dump(StaticCachePreloadMethodState::value());

?>
--EXPECT--
array(1) {
  ["preload"]=>
  bool(true)
}
int(1)
array(1) {
  ["preload"]=>
  bool(true)
}
