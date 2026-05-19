--TEST--
OPcache __DirectCacheSafe rejects unstorable values in SPL subclass state
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

use OPcache\StaticCacheException;

class SafeDirectUnstorableArrayObject extends ArrayObject
{
}

class SafeDirectUnstorableFixedArray extends SplFixedArray
{
}

function dump_pinned_exception(Closure $callback): void
{
	try {
		$callback();
	} catch (StaticCacheException $e) {
		echo $e->getMessage(), "\n";
	}
}

$resource = fopen(__FILE__, 'r');
$fixed_array = new SafeDirectUnstorableFixedArray(1);
$fixed_array[0] = $resource;
$array_object = new SafeDirectUnstorableArrayObject(['value' => static fn () => true]);

var_dump(OPcache\volatile_store('fixed-resource', $fixed_array));
var_dump(OPcache\volatile_fetch('fixed-resource', 'missing'));
var_dump(OPcache\volatile_store('array-object-closure', $array_object));
var_dump(OPcache\volatile_fetch('array-object-closure', 'missing'));

dump_pinned_exception(static fn () => OPcache\pinned_store('fixed-resource', $fixed_array));
dump_pinned_exception(static fn () => OPcache\pinned_store('array-object-closure', $array_object));

fclose($resource);

?>
--EXPECT--
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
resources and Closure objects cannot be stored in the static cache
resources and Closure objects cannot be stored in the static cache
