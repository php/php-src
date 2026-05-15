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

var_dump(function_exists('OPcache\\volatile_atomic_increment'));
var_dump(function_exists('OPcache\\volatile_atomic_decrement'));
var_dump(function_exists('OPcache\\persistent_atomic_increment'));
var_dump(function_exists('OPcache\\persistent_atomic_decrement'));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
