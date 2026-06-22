--TEST--
OPcache StableCache::store returns false when stable cache memory is exhausted
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=8
--FILE--
<?php

OPcache\StableCache::getInstance('default')->store('small', 'ok');

var_dump(OPcache\StableCache::getInstance('default')->store('huge', str_repeat('H', 12 * 1024 * 1024)));

var_dump(OPcache\StableCache::getInstance('default')->fetch('small', 'fallback'));
var_dump(OPcache\StableCache::getInstance('default')->fetch('missing', 'fallback'));

?>
--EXPECT--
bool(false)
string(2) "ok"
string(8) "fallback"
