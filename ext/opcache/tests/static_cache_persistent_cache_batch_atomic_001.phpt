--TEST--
OPcache pinned cache batch and atomic public API
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

OPcache\PinnedCache::setMultiple([
	'count' => 10,
	'name' => 'php',
	'null' => null,
]);

$fallback = ['fallback'];

var_dump(OPcache\PinnedCache::getMultiple(['count', 'name', 'missing'], $fallback));
var_dump(OPcache\PinnedCache::increment('count'));
var_dump(OPcache\PinnedCache::decrement('count', 3));

OPcache\PinnedCache::deleteMultiple(['name', 'missing']);
var_dump(OPcache\PinnedCache::getMultiple(['count', 'name', 'null'], $fallback));

OPcache\PinnedCache::clear();
var_dump(OPcache\PinnedCache::getMultiple(['count'], $fallback));

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
