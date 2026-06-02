--TEST--
OPcache direct cache handlers reject unstorable values in SPL subclass state
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
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

var_dump(OPcache\VolatileCache::set('fixed-resource', $fixed_array));
var_dump(OPcache\VolatileCache::get('fixed-resource', 'missing'));
var_dump(OPcache\VolatileCache::set('array-object-closure', $array_object));
var_dump(OPcache\VolatileCache::get('array-object-closure', 'missing'));

var_dump(OPcache\PinnedCache::set('fixed-resource', $fixed_array));
var_dump(OPcache\PinnedCache::set('array-object-closure', $array_object));

fclose($resource);

?>
--EXPECT--
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
bool(false)
