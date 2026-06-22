--TEST--
OPcache VolatileCache::fetch default value and VolatileCache::has
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

opcache_static_cache_volatile_reset();

var_dump(OPcache\VolatileCache::getInstance('default')->has('missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('missing', 'fallback'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('null', null));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('null', 'fallback'));
var_dump(OPcache\VolatileCache::getInstance('default')->has('null'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('false', false));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('false', 'fallback'));
var_dump(OPcache\VolatileCache::getInstance('default')->has('false'));

OPcache\VolatileCache::getInstance('default')->delete('false');
var_dump(OPcache\VolatileCache::getInstance('default')->has('false'));

?>
--EXPECT--
bool(false)
NULL
string(8) "fallback"
bool(true)
NULL
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
