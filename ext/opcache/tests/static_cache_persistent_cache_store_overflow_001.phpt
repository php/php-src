--TEST--
OPcache PinnedCache::set returns false when pinned cache memory is exhausted
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=8
--FILE--
<?php

OPcache\PinnedCache::set('small', 'ok');

var_dump(OPcache\PinnedCache::set('huge', str_repeat('H', 12 * 1024 * 1024)));

var_dump(OPcache\PinnedCache::get('small', 'fallback'));
var_dump(OPcache\PinnedCache::get('missing', 'fallback'));

?>
--EXPECT--
bool(false)
string(2) "ok"
string(8) "fallback"
