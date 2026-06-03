--TEST--
OPcache volatile cache request-local lookup cache invalidates on writes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(OPcache\VolatileCache::getInstance('default')->fetch('transient', 'MISS'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('transient', 'value'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('transient'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('victim', 'gone'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('victim'));
OPcache\VolatileCache::getInstance('default')->delete('victim');
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('victim', 'MISS'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('clear_victim', 'clear me'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('clear_victim'));
opcache_static_cache_volatile_reset();
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('clear_victim', 'MISS'));

?>
--EXPECT--
string(4) "MISS"
bool(true)
string(5) "value"
bool(true)
string(4) "gone"
string(4) "MISS"
bool(true)
string(8) "clear me"
string(4) "MISS"
