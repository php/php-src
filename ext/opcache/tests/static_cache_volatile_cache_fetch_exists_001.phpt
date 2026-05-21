--TEST--
OPcache volatile_fetch default value and volatile_exists
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

OPcache\volatile_clear();

var_dump(OPcache\volatile_exists('missing'));
var_dump(OPcache\volatile_fetch('missing'));
var_dump(OPcache\volatile_fetch('missing', 'fallback'));

var_dump(OPcache\volatile_store('null', null));
var_dump(OPcache\volatile_fetch('null', 'fallback'));
var_dump(OPcache\volatile_exists('null'));

var_dump(OPcache\volatile_store('false', false));
var_dump(OPcache\volatile_fetch('false', 'fallback'));
var_dump(OPcache\volatile_exists('false'));

OPcache\volatile_delete('false');
var_dump(OPcache\volatile_exists('false'));

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
