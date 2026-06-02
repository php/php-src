--TEST--
OPcache VolatileCache::get default value and VolatileCache::has
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::has('missing'));
var_dump(OPcache\VolatileCache::get('missing'));
var_dump(OPcache\VolatileCache::get('missing', 'fallback'));

var_dump(OPcache\VolatileCache::set('null', null));
var_dump(OPcache\VolatileCache::get('null', 'fallback'));
var_dump(OPcache\VolatileCache::has('null'));

var_dump(OPcache\VolatileCache::set('false', false));
var_dump(OPcache\VolatileCache::get('false', 'fallback'));
var_dump(OPcache\VolatileCache::has('false'));

OPcache\VolatileCache::delete('false');
var_dump(OPcache\VolatileCache::has('false'));

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
