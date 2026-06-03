--TEST--
OPcache stable atomic operations reject non-integer entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

OPcache\StableCache::getInstance('default')->store('text', 'php');

var_dump(OPcache\StableCache::getInstance('default')->increment('text'));
var_dump(OPcache\StableCache::getInstance('default')->decrement('text'));

?>
--EXPECT--
bool(false)
bool(false)
