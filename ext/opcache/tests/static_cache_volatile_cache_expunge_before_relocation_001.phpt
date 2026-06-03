--TEST--
OPcache stable cache expunges expired entries before relocating fragmented payload blocks
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=8
--FILE--
<?php

class StableExpungeBeforeRelocationProbe
{
    public function __construct(public string $name) {}
}

OPcache\StableCache::getInstance('default')->clear();

var_dump(OPcache\StableCache::getInstance('default')->store('probe', new StableExpungeBeforeRelocationProbe('probe')));
var_dump(OPcache\StableCache::getInstance('default')->store('first', str_repeat('A', 1200000)));
var_dump(OPcache\StableCache::getInstance('default')->store('second', str_repeat('B', 1200000)));
var_dump(OPcache\StableCache::getInstance('default')->store('third', str_repeat('C', 1200000)));
var_dump(OPcache\StableCache::getInstance('default')->storeWithTtl('expired-tail', str_repeat('D', 1200000), 1));

OPcache\StableCache::getInstance('default')->delete('second');

sleep(2);

var_dump(OPcache\StableCache::getInstance('default')->store('after-expired', str_repeat('E', 2400000)));
var_dump(OPcache\StableCache::getInstance('default')->fetch('probe')->name);
var_dump(OPcache\StableCache::getInstance('default')->fetch('expired-tail', 'expired'));
var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('after-expired')));
var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('first')));
var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('third')));

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
