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

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::set('probe', new VolatileExpungeBeforeRelocationProbe('probe')));
var_dump(OPcache\VolatileCache::set('first', str_repeat('A', 1200000)));
var_dump(OPcache\VolatileCache::set('second', str_repeat('B', 1200000)));
var_dump(OPcache\VolatileCache::set('third', str_repeat('C', 1200000)));
var_dump(OPcache\VolatileCache::set('expired-tail', str_repeat('D', 1200000), 1));

OPcache\VolatileCache::delete('second');

sleep(2);

var_dump(OPcache\VolatileCache::set('after-expired', str_repeat('E', 2400000)));
var_dump(OPcache\VolatileCache::get('probe')->name);
var_dump(OPcache\VolatileCache::get('expired-tail', 'expired'));
var_dump(strlen(OPcache\VolatileCache::get('after-expired')));
var_dump(strlen(OPcache\VolatileCache::get('first')));
var_dump(strlen(OPcache\VolatileCache::get('third')));

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
