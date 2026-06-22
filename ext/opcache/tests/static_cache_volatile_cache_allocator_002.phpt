--TEST--
OPcache volatile cache coalesces adjacent freed payload blocks
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

opcache_static_cache_volatile_reset();

$payloadA = str_repeat('A', 2000000);
$payloadB = str_repeat('B', 2000000);
$payloadC = str_repeat('C', 2000000);
$payloadD = str_repeat('D', 3500000);

var_dump(OPcache\VolatileCache::getInstance('default')->store('first', $payloadA));
var_dump(OPcache\VolatileCache::getInstance('default')->store('second', $payloadB));
var_dump(OPcache\VolatileCache::getInstance('default')->store('third', $payloadC));

OPcache\VolatileCache::getInstance('default')->delete('first');
OPcache\VolatileCache::getInstance('default')->delete('second');

var_dump(OPcache\VolatileCache::getInstance('default')->store('merged', $payloadD));
var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('merged')));

opcache_static_cache_volatile_reset();

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
int(3500000)
