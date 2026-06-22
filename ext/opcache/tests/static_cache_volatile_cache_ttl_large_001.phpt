--TEST--
OPcache stable cache stores large TTL values without 32-bit expiry truncation
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

OPcache\StableCache::getInstance('default')->clear();

var_dump(OPcache\StableCache::getInstance('default')->storeWithTtl('large_ttl', 'value', PHP_INT_MAX));
var_dump(OPcache\StableCache::getInstance('default')->fetch('large_ttl', 'expired'));

?>
--EXPECT--
bool(true)
string(5) "value"
