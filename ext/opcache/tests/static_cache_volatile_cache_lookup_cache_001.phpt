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

var_dump(OPcache\VolatileCache::get('transient', 'MISS'));

var_dump(OPcache\VolatileCache::set('transient', 'value'));
var_dump(OPcache\VolatileCache::get('transient'));

var_dump(OPcache\VolatileCache::set('victim', 'gone'));
var_dump(OPcache\VolatileCache::get('victim'));
OPcache\VolatileCache::delete('victim');
var_dump(OPcache\VolatileCache::get('victim', 'MISS'));

var_dump(OPcache\VolatileCache::set('clear_victim', 'clear me'));
var_dump(OPcache\VolatileCache::get('clear_victim'));
OPcache\VolatileCache::clear();
var_dump(OPcache\VolatileCache::get('clear_victim', 'MISS'));

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
