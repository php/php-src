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

var_dump(OPcache\VolatileCache::set('n', 41));
var_dump(OPcache\VolatileCache::get('n'));

var_dump(OPcache\VolatileCache::set('s', 'php'));
var_dump(OPcache\VolatileCache::get('s'));

var_dump(OPcache\VolatileCache::set('a', ['x' => 1, 'y' => true]));
var_dump(OPcache\VolatileCache::get('a'));

var_dump(OPcache\VolatileCache::setMultiple(['foo' => 'bar', 'baz' => true]));
var_dump(OPcache\VolatileCache::get('foo'));
var_dump(OPcache\VolatileCache::get('baz'));

OPcache\VolatileCache::deleteMultiple(['foo', 'baz']);
var_dump(OPcache\VolatileCache::get('foo', 'fallback'));

OPcache\VolatileCache::clear();
var_dump(OPcache\VolatileCache::get('n', 'fallback'));

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
