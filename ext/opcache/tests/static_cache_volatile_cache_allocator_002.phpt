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

OPcache\volatile_clear();

$payloadA = str_repeat('A', 2000000);
$payloadB = str_repeat('B', 2000000);
$payloadC = str_repeat('C', 2000000);
$payloadD = str_repeat('D', 3500000);

var_dump(OPcache\volatile_store('first', $payloadA));
var_dump(OPcache\volatile_store('second', $payloadB));
var_dump(OPcache\volatile_store('third', $payloadC));

OPcache\volatile_delete('first');
OPcache\volatile_delete('second');

var_dump(OPcache\volatile_store('merged', $payloadD));
var_dump(strlen(OPcache\volatile_fetch('merged')));

OPcache\volatile_clear();

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
int(3500000)
