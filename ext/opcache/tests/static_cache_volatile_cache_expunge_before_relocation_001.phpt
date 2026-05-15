--TEST--
OPcache volatile cache expunges expired entries before relocating fragmented payload blocks
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

class VolatileExpungeBeforeRelocationProbe
{
    public function __construct(public string $name) {}
}

OPcache\volatile_clear();

var_dump(OPcache\volatile_store('probe', new VolatileExpungeBeforeRelocationProbe('probe')));
var_dump(OPcache\volatile_store('first', str_repeat('A', 1200000)));
var_dump(OPcache\volatile_store('second', str_repeat('B', 1200000)));
var_dump(OPcache\volatile_store('third', str_repeat('C', 1200000)));
var_dump(OPcache\volatile_store('expired-tail', str_repeat('D', 1200000), 1));

OPcache\volatile_delete('second');

sleep(2);

var_dump(OPcache\volatile_store('after-expired', str_repeat('E', 2400000)));
var_dump(OPcache\volatile_fetch('probe')->name);
var_dump(OPcache\volatile_fetch('expired-tail', 'expired'));
var_dump(strlen(OPcache\volatile_fetch('after-expired')));
var_dump(strlen(OPcache\volatile_fetch('first')));
var_dump(strlen(OPcache\volatile_fetch('third')));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(5) "probe"
string(7) "expired"
int(2400000)
int(1200000)
int(1200000)
