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

var_dump(OPcache\volatile_store('n', 41));
var_dump(OPcache\volatile_fetch('n'));

var_dump(OPcache\volatile_store('s', 'php'));
var_dump(OPcache\volatile_fetch('s'));

var_dump(OPcache\volatile_store('a', ['x' => 1, 'y' => true]));
var_dump(OPcache\volatile_fetch('a'));

var_dump(OPcache\volatile_store_array(['foo' => 'bar', 'baz' => true]));
var_dump(OPcache\volatile_fetch('foo'));
var_dump(OPcache\volatile_fetch('baz'));

OPcache\volatile_delete_array(['foo', 'baz']);
var_dump(OPcache\volatile_fetch('foo', 'fallback'));

OPcache\volatile_clear();
var_dump(OPcache\volatile_fetch('n', 'fallback'));

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
