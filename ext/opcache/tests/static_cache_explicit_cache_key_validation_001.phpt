--TEST--
OPcache explicit cache APIs reject empty and non-scalar cache keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

function dump_error(string $label, callable $callback): void
{
	try {
		$callback();
		echo $label, ": no error\n";
	} catch (Throwable $e) {
		echo $label, ": ", get_class($e), ": ", $e->getMessage(), "\n";
	}
}

class StringableKey
{
	public function __toString(): string
	{
		echo "__toString called\n";

		return "stringable";
	}
}

class LoadedClassKey
{
}

foreach (['volatile', 'stable'] as $backend) {
	echo $backend, "\n";

	$reservedClassKey = $backend . '_static_class:LoadedClassKey';
	$loadedClassKey = LoadedClassKey::class;
	$delimiterKey = 'key:with-delimiter';
	$store = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->store(...) : OPcache\StableCache::getInstance('default')->store(...);
	$storeArray = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->storeMultiple(...) : OPcache\StableCache::getInstance('default')->storeMultiple(...);
	$fetch = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->fetch(...) : OPcache\StableCache::getInstance('default')->fetch(...);
	$fetchArray = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->fetchMultiple(...) : OPcache\StableCache::getInstance('default')->fetchMultiple(...);
	$exists = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->has(...) : OPcache\StableCache::getInstance('default')->has(...);
	$lock = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->lock(...) : OPcache\StableCache::getInstance('default')->lock(...);
	$unlock = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->unlock(...) : OPcache\StableCache::getInstance('default')->unlock(...);
	$delete = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->delete(...) : OPcache\StableCache::getInstance('default')->delete(...);
	$deleteArray = $backend === 'volatile' ? OPcache\VolatileCache::getInstance('default')->deleteMultiple(...) : OPcache\StableCache::getInstance('default')->deleteMultiple(...);

	dump_error('store-empty', static fn () => $store('', 'value'));
	dump_error('store-array-empty', static fn () => $storeArray(['' => 'value']));
	dump_error('store-reserved-class', static fn () => $store($reservedClassKey, 'value'));
	dump_error('store-array-reserved-class', static fn () => $storeArray([$reservedClassKey => 'value']));
	dump_error('store-loaded-class', static fn () => $store($loadedClassKey, 'value'));
	dump_error('store-array-loaded-class', static fn () => $storeArray([$loadedClassKey => 'value']));
	dump_error('store-delimiter', static fn () => $store($delimiterKey, 'value'));
	dump_error('store-array-delimiter', static fn () => $storeArray([$delimiterKey => 'value']));
	dump_error('fetch-empty', static fn () => $fetch(''));
	dump_error('fetch-array-empty', static fn () => $fetchArray(['']));
	dump_error('fetch-array-object', static fn () => $fetchArray([new StringableKey()]));
	dump_error('fetch-reserved-class', static fn () => $fetch($reservedClassKey));
	dump_error('fetch-array-reserved-class', static fn () => $fetchArray([$reservedClassKey]));
	dump_error('fetch-loaded-class', static fn () => $fetch($loadedClassKey));
	dump_error('fetch-array-loaded-class', static fn () => $fetchArray([$loadedClassKey]));
	dump_error('fetch-delimiter', static fn () => $fetch($delimiterKey));
	dump_error('fetch-array-delimiter', static fn () => $fetchArray([$delimiterKey]));
	dump_error('exists-empty', static fn () => $exists(''));
	dump_error('exists-reserved-class', static fn () => $exists($reservedClassKey));
	dump_error('exists-loaded-class', static fn () => $exists($loadedClassKey));
	dump_error('exists-delimiter', static fn () => $exists($delimiterKey));
	dump_error('lock-empty', static fn () => $lock(''));
	dump_error('lock-negative-lease', static fn () => $lock('key', -1));
	dump_error('lock-reserved-class', static fn () => $lock($reservedClassKey));
	dump_error('lock-loaded-class', static fn () => $lock($loadedClassKey));
	dump_error('lock-delimiter', static fn () => $lock($delimiterKey));
	dump_error('unlock-empty', static fn () => $unlock(''));
	dump_error('unlock-reserved-class', static fn () => $unlock($reservedClassKey));
	dump_error('unlock-loaded-class', static fn () => $unlock($loadedClassKey));
	dump_error('unlock-delimiter', static fn () => $unlock($delimiterKey));
	dump_error('delete-empty', static fn () => $delete(''));
	dump_error('delete-reserved-class', static fn () => $delete($reservedClassKey));
	dump_error('delete-delimiter', static fn () => $delete($delimiterKey));
	dump_error('delete-array-empty', static fn () => $deleteArray(['']));
	dump_error('delete-array-object', static fn () => $deleteArray([new StringableKey()]));
	dump_error('delete-array-reserved-class', static fn () => $deleteArray([$reservedClassKey]));
	dump_error('delete-array-loaded-class', static fn () => $deleteArray([$loadedClassKey]));
	dump_error('delete-array-delimiter', static fn () => $deleteArray([$delimiterKey]));

	if ($backend === 'stable') {
		dump_error('atomic-increment-reserved-class', static fn () => OPcache\StableCache::getInstance('default')->increment($reservedClassKey));
		dump_error('atomic-decrement-reserved-class', static fn () => OPcache\StableCache::getInstance('default')->decrement($reservedClassKey));
		dump_error('atomic-increment-loaded-class', static fn () => OPcache\StableCache::getInstance('default')->increment($loadedClassKey));
		dump_error('atomic-decrement-loaded-class', static fn () => OPcache\StableCache::getInstance('default')->decrement($loadedClassKey));
		dump_error('atomic-increment-delimiter', static fn () => OPcache\StableCache::getInstance('default')->increment($delimiterKey));
		dump_error('atomic-decrement-delimiter', static fn () => OPcache\StableCache::getInstance('default')->decrement($delimiterKey));
	}
}

?>
--EXPECTF--
volatile
store-empty: ValueError: OPcache\VolatileCache::store(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
store-reserved-class: ValueError: OPcache\VolatileCache::store(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
store-array-reserved-class: ValueError: OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
store-loaded-class: ValueError: OPcache\VolatileCache::store(): Argument #1 ($key) must not be a loaded class name
store-array-loaded-class: ValueError: OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
store-delimiter: ValueError: OPcache\VolatileCache::store(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
store-array-delimiter: ValueError: OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-empty: ValueError: OPcache\VolatileCache::fetch(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\VolatileCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-array-object: ValueError: OPcache\VolatileCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-reserved-class: ValueError: OPcache\VolatileCache::fetch(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
fetch-array-reserved-class: ValueError: OPcache\VolatileCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-loaded-class: ValueError: OPcache\VolatileCache::fetch(): Argument #1 ($key) must not be a loaded class name
fetch-array-loaded-class: ValueError: OPcache\VolatileCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-delimiter: ValueError: OPcache\VolatileCache::fetch(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
fetch-array-delimiter: ValueError: OPcache\VolatileCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
exists-empty: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must be a non-empty string
exists-reserved-class: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
exists-loaded-class: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must not be a loaded class name
exists-delimiter: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
lock-empty: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must be a non-empty string
lock-negative-lease: ValueError: OPcache\VolatileCache::lock(): Argument #2 ($lease) must be greater than or equal to 0
lock-reserved-class: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
lock-loaded-class: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must not be a loaded class name
lock-delimiter: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
unlock-empty: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must be a non-empty string
unlock-reserved-class: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
unlock-loaded-class: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must not be a loaded class name
unlock-delimiter: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
delete-empty: ValueError: OPcache\VolatileCache::delete(): Argument #1 ($key) must be a non-empty string
delete-reserved-class: ValueError: OPcache\VolatileCache::delete(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
delete-delimiter: ValueError: OPcache\VolatileCache::delete(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
delete-array-empty: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-object: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-reserved-class: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-loaded-class: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-delimiter: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
stable
store-empty: ValueError: OPcache\StableCache::store(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\StableCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
store-reserved-class: ValueError: OPcache\StableCache::store(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
store-array-reserved-class: ValueError: OPcache\StableCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
store-loaded-class: ValueError: OPcache\StableCache::store(): Argument #1 ($key) must not be a loaded class name
store-array-loaded-class: ValueError: OPcache\StableCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
store-delimiter: ValueError: OPcache\StableCache::store(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
store-array-delimiter: ValueError: OPcache\StableCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-empty: ValueError: OPcache\StableCache::fetch(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\StableCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-array-object: ValueError: OPcache\StableCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-reserved-class: ValueError: OPcache\StableCache::fetch(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
fetch-array-reserved-class: ValueError: OPcache\StableCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-loaded-class: ValueError: OPcache\StableCache::fetch(): Argument #1 ($key) must not be a loaded class name
fetch-array-loaded-class: ValueError: OPcache\StableCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
fetch-delimiter: ValueError: OPcache\StableCache::fetch(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
fetch-array-delimiter: ValueError: OPcache\StableCache::fetchMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
exists-empty: ValueError: OPcache\StableCache::has(): Argument #1 ($key) must be a non-empty string
exists-reserved-class: ValueError: OPcache\StableCache::has(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
exists-loaded-class: ValueError: OPcache\StableCache::has(): Argument #1 ($key) must not be a loaded class name
exists-delimiter: ValueError: OPcache\StableCache::has(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
lock-empty: ValueError: OPcache\StableCache::lock(): Argument #1 ($key) must be a non-empty string
lock-negative-lease: ValueError: OPcache\StableCache::lock(): Argument #2 ($lease) must be greater than or equal to 0
lock-reserved-class: ValueError: OPcache\StableCache::lock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
lock-loaded-class: ValueError: OPcache\StableCache::lock(): Argument #1 ($key) must not be a loaded class name
lock-delimiter: ValueError: OPcache\StableCache::lock(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
unlock-empty: ValueError: OPcache\StableCache::unlock(): Argument #1 ($key) must be a non-empty string
unlock-reserved-class: ValueError: OPcache\StableCache::unlock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
unlock-loaded-class: ValueError: OPcache\StableCache::unlock(): Argument #1 ($key) must not be a loaded class name
unlock-delimiter: ValueError: OPcache\StableCache::unlock(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
delete-empty: ValueError: OPcache\StableCache::delete(): Argument #1 ($key) must be a non-empty string
delete-reserved-class: ValueError: OPcache\StableCache::delete(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
delete-delimiter: ValueError: OPcache\StableCache::delete(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
delete-array-empty: ValueError: OPcache\StableCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-object: ValueError: OPcache\StableCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-reserved-class: ValueError: OPcache\StableCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-loaded-class: ValueError: OPcache\StableCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
delete-array-delimiter: ValueError: OPcache\StableCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
atomic-increment-reserved-class: ValueError: OPcache\StableCache::increment(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
atomic-decrement-reserved-class: ValueError: OPcache\StableCache::decrement(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
atomic-increment-loaded-class: ValueError: OPcache\StableCache::increment(): Argument #1 ($key) must not be a loaded class name
atomic-decrement-loaded-class: ValueError: OPcache\StableCache::decrement(): Argument #1 ($key) must not be a loaded class name
atomic-increment-delimiter: ValueError: OPcache\StableCache::increment(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
atomic-decrement-delimiter: ValueError: OPcache\StableCache::decrement(): Argument #1 ($key) must not contain the static-cache key delimiter ":"
