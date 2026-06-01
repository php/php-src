--TEST--
OPcache Static Cache is disabled by default for unsafe cgi-fcgi runtime
--EXTENSIONS--
opcache
--CGI--
--INI--
opcache.enable=1
opcache.static_cache.volatile_size_mb=8
opcache.static_cache.pinned_size_mb=8
--FILE--
<?php

echo PHP_SAPI, "\n";

$configuration = opcache_get_configuration();
var_dump($configuration['directives']['opcache.static_cache.allow_unsafe_runtime']);

$volatile = OPcache\volatile_cache_info();
$pinned = OPcache\pinned_cache_info();

echo "volatile\n";
var_dump($volatile->enabled);
var_dump($volatile->available);
var_dump($volatile->failure_reason);
var_dump(OPcache\volatile_store('key', 'volatile'));

echo "pinned\n";
var_dump($pinned->enabled);
var_dump($pinned->available);
var_dump($pinned->failure_reason);
var_dump(OPcache\pinned_store('key', 'pinned'));

?>
--EXPECT--
cgi-fcgi
bool(false)
volatile
bool(true)
bool(false)
string(96) "Static Cache is disabled for this SAPI unless opcache.static_cache.allow_unsafe_runtime=1 is set"
bool(false)
pinned
bool(true)
bool(false)
string(96) "Static Cache is disabled for this SAPI unless opcache.static_cache.allow_unsafe_runtime=1 is set"
bool(false)
