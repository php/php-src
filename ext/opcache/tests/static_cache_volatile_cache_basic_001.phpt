--TEST--
OPcache volatile cache basic KV operations
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(OPcache\VolatileCache::getInstance('default')->store('n', 41));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('n'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('s', 'php'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('s'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('a', ['x' => 1, 'y' => true]));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('a'));

var_dump(OPcache\VolatileCache::getInstance('default')->storeMultiple(['foo' => 'bar', 'baz' => true]));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('foo'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('baz'));

OPcache\VolatileCache::getInstance('default')->deleteMultiple(['foo', 'baz']);
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('foo', 'fallback'));

opcache_static_cache_volatile_reset();
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('n', 'fallback'));

?>
--EXPECT--
bool(true)
int(41)
bool(true)
string(3) "php"
bool(true)
array(2) {
  ["x"]=>
  int(1)
  ["y"]=>
  bool(true)
}
bool(true)
string(3) "bar"
bool(true)
string(8) "fallback"
string(8) "fallback"
