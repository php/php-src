--TEST--
OPcache pinned atomic operations reject non-integer entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

OPcache\PinnedCache::set('text', 'php');

var_dump(OPcache\PinnedCache::increment('text'));
var_dump(OPcache\PinnedCache::decrement('text'));

?>
--EXPECT--
bool(false)
bool(false)
