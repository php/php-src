--TEST--
OPcache direct cache handlers reject unstorable values in SPL subclass state
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

class SafeDirectUnstorableArrayObject extends ArrayObject
{
}

class SafeDirectUnstorableFixedArray extends SplFixedArray
{
}

$resource = fopen(__FILE__, 'r');
$fixed_array = new SafeDirectUnstorableFixedArray(1);
$fixed_array[0] = $resource;
$array_object = new SafeDirectUnstorableArrayObject(['value' => static fn () => true]);

var_dump(OPcache\VolatileCache::getInstance('default')->store('fixed-resource', $fixed_array));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('fixed-resource', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('array-object-closure', $array_object));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('array-object-closure', 'missing'));

var_dump(OPcache\StableCache::getInstance('default')->store('fixed-resource', $fixed_array));
var_dump(OPcache\StableCache::getInstance('default')->store('array-object-closure', $array_object));

fclose($resource);

?>
--EXPECT--
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
bool(false)
