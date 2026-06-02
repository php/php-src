--TEST--
OPcache Static Cache stays unavailable for a SAPI that does not opt in (cgi-fcgi)
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

$volatile = OPcache\VolatileCache::info();
$pinned = OPcache\PinnedCache::info();

echo "volatile\n";
var_dump($volatile->enabled);
var_dump($volatile->available);
var_dump($volatile->failure_reason);
var_dump(OPcache\VolatileCache::set('key', 'volatile'));

echo "pinned\n";
var_dump($pinned->enabled);
var_dump($pinned->available);
var_dump($pinned->failure_reason);
var_dump(OPcache\PinnedCache::set('key', 'pinned'));

?>
--EXPECT--
cgi-fcgi
volatile
bool(true)
bool(false)
string(70) "Static Cache is not enabled for this SAPI (the runtime did not opt in)"
bool(false)
pinned
bool(true)
bool(false)
string(70) "Static Cache is not enabled for this SAPI (the runtime did not opt in)"
bool(false)
