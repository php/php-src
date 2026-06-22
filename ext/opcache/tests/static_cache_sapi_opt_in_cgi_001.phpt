--TEST--
OPcache Static Cache stays unavailable for a SAPI that does not opt in (cgi-fcgi)
--EXTENSIONS--
opcache
--CGI--
--INI--
opcache.enable=1
opcache.static_cache.volatile_size_mb=8
opcache.static_cache.stable_size_mb=8
--FILE--
<?php

echo PHP_SAPI, "\n";

$volatile = OPcache\VolatileCache::info();
$stable = OPcache\StableCache::info();

echo "volatile\n";
var_dump($volatile->enabled);
var_dump($volatile->available);
var_dump($volatile->failure_reason);
var_dump(OPcache\VolatileCache::getInstance('default')->store('key', 'volatile'));

echo "stable\n";
var_dump($stable->enabled);
var_dump($stable->available);
var_dump($stable->failure_reason);
var_dump(OPcache\StableCache::getInstance('default')->store('key', 'stable'));

?>
--EXPECT--
cgi-fcgi
volatile
bool(true)
bool(false)
string(70) "Static Cache is not enabled for this SAPI (the runtime did not opt in)"
bool(false)
stable
bool(true)
bool(false)
string(70) "Static Cache is not enabled for this SAPI (the runtime did not opt in)"
bool(false)
