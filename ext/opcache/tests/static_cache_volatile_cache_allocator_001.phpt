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

var_dump(OPcache\volatile_store('first', $payloadA));
OPcache\volatile_delete('first');
var_dump(OPcache\volatile_store('second', $payloadB));
OPcache\volatile_delete('second');
var_dump(OPcache\volatile_store('third', $payloadC));
var_dump(strlen(OPcache\volatile_fetch('third')));

OPcache\volatile_clear();

var_dump(OPcache\volatile_store('same', $payloadA));
var_dump(OPcache\volatile_store('same', $payloadB));
var_dump(OPcache\volatile_store('same', $payloadC));
var_dump(strlen(OPcache\volatile_fetch('same')));

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
