--TEST--
OPcache volatile cache stores large TTL values without 32-bit expiry truncation
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::set('large_ttl', 'value', PHP_INT_MAX));
var_dump(OPcache\VolatileCache::get('large_ttl', 'expired'));

?>
--EXPECT--
bool(true)
string(5) "value"
