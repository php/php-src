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

var_dump(OPcache\VolatileCache::set('first', $payloadA));
OPcache\VolatileCache::delete('first');
var_dump(OPcache\VolatileCache::set('second', $payloadB));
OPcache\VolatileCache::delete('second');
var_dump(OPcache\VolatileCache::set('third', $payloadC));
var_dump(strlen(OPcache\VolatileCache::get('third')));

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::set('same', $payloadA));
var_dump(OPcache\VolatileCache::set('same', $payloadB));
var_dump(OPcache\VolatileCache::set('same', $payloadC));
var_dump(strlen(OPcache\VolatileCache::get('same')));

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
