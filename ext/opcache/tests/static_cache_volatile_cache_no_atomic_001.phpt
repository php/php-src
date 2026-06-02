--TEST--
OPcache volatile cache does not expose atomic public API
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(method_exists('OPcache\\VolatileCache', 'increment'));
var_dump(method_exists('OPcache\\VolatileCache', 'decrement'));
var_dump(method_exists('OPcache\\PinnedCache', 'increment'));
var_dump(method_exists('OPcache\\PinnedCache', 'decrement'));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
