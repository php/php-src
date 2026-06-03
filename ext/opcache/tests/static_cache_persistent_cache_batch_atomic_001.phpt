--TEST--
OPcache stable cache batch and atomic public API
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

OPcache\StableCache::getInstance('default')->storeMultiple([
	'count' => 10,
	'name' => 'php',
	'null' => null,
]);

$fallback = ['fallback'];

var_dump(OPcache\StableCache::getInstance('default')->fetchMultiple(['count', 'name', 'missing'], $fallback));
var_dump(OPcache\StableCache::getInstance('default')->increment('count'));
var_dump(OPcache\StableCache::getInstance('default')->decrement('count', 3));

OPcache\StableCache::getInstance('default')->deleteMultiple(['name', 'missing']);
var_dump(OPcache\StableCache::getInstance('default')->fetchMultiple(['count', 'name', 'null'], $fallback));

OPcache\StableCache::getInstance('default')->clear();
var_dump(OPcache\StableCache::getInstance('default')->fetchMultiple(['count'], $fallback));

?>
--EXPECT--
array(3) {
  ["count"]=>
  int(10)
  ["name"]=>
  string(3) "php"
  ["missing"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
}
int(11)
int(8)
array(3) {
  ["count"]=>
  int(8)
  ["name"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
  ["null"]=>
  NULL
}
array(1) {
  ["count"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
}
