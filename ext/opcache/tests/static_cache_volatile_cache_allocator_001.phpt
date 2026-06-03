--TEST--
OPcache volatile cache reuses freed payload memory
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

$payloadA = str_repeat('A', 2400000);
$payloadB = str_repeat('B', 2400000);
$payloadC = str_repeat('C', 2400000);

var_dump(OPcache\VolatileCache::getInstance('default')->store('first', $payloadA));
OPcache\VolatileCache::getInstance('default')->delete('first');
var_dump(OPcache\VolatileCache::getInstance('default')->store('second', $payloadB));
OPcache\VolatileCache::getInstance('default')->delete('second');
var_dump(OPcache\VolatileCache::getInstance('default')->store('third', $payloadC));
var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('third')));

opcache_static_cache_volatile_reset();

var_dump(OPcache\VolatileCache::getInstance('default')->store('same', $payloadA));
var_dump(OPcache\VolatileCache::getInstance('default')->store('same', $payloadB));
var_dump(OPcache\VolatileCache::getInstance('default')->store('same', $payloadC));
var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('same')));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(2400000)
bool(true)
bool(true)
bool(true)
int(2400000)
