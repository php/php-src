--TEST--
OPcache explicit cache APIs reject empty and non-scalar cache keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
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

foreach (['volatile', 'pinned'] as $backend) {
	echo $backend, "\n";

	$reservedClassKey = $backend . '_static_class:LoadedClassKey';
	$loadedClassKey = LoadedClassKey::class;
	$store = $backend === 'volatile' ? OPcache\VolatileCache::set(...) : OPcache\PinnedCache::set(...);
	$storeArray = $backend === 'volatile' ? OPcache\VolatileCache::setMultiple(...) : OPcache\PinnedCache::setMultiple(...);
	$fetch = $backend === 'volatile' ? OPcache\VolatileCache::get(...) : OPcache\PinnedCache::get(...);
	$fetchArray = $backend === 'volatile' ? OPcache\VolatileCache::getMultiple(...) : OPcache\PinnedCache::getMultiple(...);
	$exists = $backend === 'volatile' ? OPcache\VolatileCache::has(...) : OPcache\PinnedCache::has(...);
	$lock = $backend === 'volatile' ? OPcache\VolatileCache::lock(...) : OPcache\PinnedCache::lock(...);
	$unlock = $backend === 'volatile' ? OPcache\VolatileCache::unlock(...) : OPcache\PinnedCache::unlock(...);
	$delete = $backend === 'volatile' ? OPcache\VolatileCache::delete(...) : OPcache\PinnedCache::delete(...);
	$deleteArray = $backend === 'volatile' ? OPcache\VolatileCache::deleteMultiple(...) : OPcache\PinnedCache::deleteMultiple(...);

	dump_error('store-empty', static fn () => $store('', 'value'));
	dump_error('store-array-empty', static fn () => $storeArray(['' => 'value']));
	dump_error('store-reserved-class', static fn () => $store($reservedClassKey, 'value'));
	dump_error('store-array-reserved-class', static fn () => $storeArray([$reservedClassKey => 'value']));
	dump_error('store-loaded-class', static fn () => $store($loadedClassKey, 'value'));
	dump_error('store-array-loaded-class', static fn () => $storeArray([$loadedClassKey => 'value']));
	dump_error('fetch-empty', static fn () => $fetch(''));
	dump_error('fetch-array-empty', static fn () => $fetchArray(['']));
	dump_error('fetch-array-object', static fn () => $fetchArray([new StringableKey()]));
	dump_error('fetch-reserved-class', static fn () => $fetch($reservedClassKey));
	dump_error('fetch-array-reserved-class', static fn () => $fetchArray([$reservedClassKey]));
	dump_error('fetch-loaded-class', static fn () => $fetch($loadedClassKey));
	dump_error('fetch-array-loaded-class', static fn () => $fetchArray([$loadedClassKey]));
	dump_error('exists-empty', static fn () => $exists(''));
	dump_error('exists-reserved-class', static fn () => $exists($reservedClassKey));
	dump_error('exists-loaded-class', static fn () => $exists($loadedClassKey));
	dump_error('lock-empty', static fn () => $lock(''));
	dump_error('lock-negative-lease', static fn () => $lock('key', -1));
	dump_error('lock-reserved-class', static fn () => $lock($reservedClassKey));
	dump_error('lock-loaded-class', static fn () => $lock($loadedClassKey));
	dump_error('unlock-empty', static fn () => $unlock(''));
	dump_error('unlock-reserved-class', static fn () => $unlock($reservedClassKey));
	dump_error('unlock-loaded-class', static fn () => $unlock($loadedClassKey));
	dump_error('delete-empty', static fn () => $delete(''));
	dump_error('delete-reserved-class', static fn () => $delete($reservedClassKey));
	dump_error('delete-array-empty', static fn () => $deleteArray(['']));
	dump_error('delete-array-object', static fn () => $deleteArray([new StringableKey()]));
	dump_error('delete-array-reserved-class', static fn () => $deleteArray([$reservedClassKey]));
	dump_error('delete-array-loaded-class', static fn () => $deleteArray([$loadedClassKey]));

	if ($backend === 'pinned') {
		dump_error('atomic-increment-reserved-class', static fn () => OPcache\PinnedCache::increment($reservedClassKey));
		dump_error('atomic-decrement-reserved-class', static fn () => OPcache\PinnedCache::decrement($reservedClassKey));
		dump_error('atomic-increment-loaded-class', static fn () => OPcache\PinnedCache::increment($loadedClassKey));
		dump_error('atomic-decrement-loaded-class', static fn () => OPcache\PinnedCache::decrement($loadedClassKey));
	}
}

?>
--EXPECTF--
volatile
store-empty: ValueError: OPcache\VolatileCache::set(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\VolatileCache::setMultiple(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-reserved-class: ValueError: OPcache\VolatileCache::set(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
store-array-reserved-class: ValueError: OPcache\VolatileCache::setMultiple(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-loaded-class: ValueError: OPcache\VolatileCache::set(): Argument #1 ($key) must not be a loaded class name
store-array-loaded-class: ValueError: OPcache\VolatileCache::setMultiple(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
fetch-empty: ValueError: OPcache\VolatileCache::get(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\VolatileCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-array-object: ValueError: OPcache\VolatileCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-reserved-class: ValueError: OPcache\VolatileCache::get(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
fetch-array-reserved-class: ValueError: OPcache\VolatileCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-loaded-class: ValueError: OPcache\VolatileCache::get(): Argument #1 ($key) must not be a loaded class name
fetch-array-loaded-class: ValueError: OPcache\VolatileCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
exists-empty: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must be a non-empty string
exists-reserved-class: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
exists-loaded-class: ValueError: OPcache\VolatileCache::has(): Argument #1 ($key) must not be a loaded class name
lock-empty: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must be a non-empty string
lock-negative-lease: ValueError: OPcache\VolatileCache::lock(): Argument #2 ($lease) must be greater than or equal to 0
lock-reserved-class: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
lock-loaded-class: ValueError: OPcache\VolatileCache::lock(): Argument #1 ($key) must not be a loaded class name
unlock-empty: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must be a non-empty string
unlock-reserved-class: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
unlock-loaded-class: ValueError: OPcache\VolatileCache::unlock(): Argument #1 ($key) must not be a loaded class name
delete-empty: ValueError: OPcache\VolatileCache::delete(): Argument #1 ($key_or_class) must be a non-empty string
delete-reserved-class: ValueError: OPcache\VolatileCache::delete(): Argument #1 ($key_or_class) must not start with a reserved static-cache class key prefix
delete-array-empty: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-object: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-reserved-class: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-loaded-class: ValueError: OPcache\VolatileCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
pinned
store-empty: ValueError: OPcache\PinnedCache::set(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\PinnedCache::setMultiple(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-reserved-class: ValueError: OPcache\PinnedCache::set(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
store-array-reserved-class: ValueError: OPcache\PinnedCache::setMultiple(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-loaded-class: ValueError: OPcache\PinnedCache::set(): Argument #1 ($key) must not be a loaded class name
store-array-loaded-class: ValueError: OPcache\PinnedCache::setMultiple(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
fetch-empty: ValueError: OPcache\PinnedCache::get(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\PinnedCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-array-object: ValueError: OPcache\PinnedCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-reserved-class: ValueError: OPcache\PinnedCache::get(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
fetch-array-reserved-class: ValueError: OPcache\PinnedCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-loaded-class: ValueError: OPcache\PinnedCache::get(): Argument #1 ($key) must not be a loaded class name
fetch-array-loaded-class: ValueError: OPcache\PinnedCache::getMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
exists-empty: ValueError: OPcache\PinnedCache::has(): Argument #1 ($key) must be a non-empty string
exists-reserved-class: ValueError: OPcache\PinnedCache::has(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
exists-loaded-class: ValueError: OPcache\PinnedCache::has(): Argument #1 ($key) must not be a loaded class name
lock-empty: ValueError: OPcache\PinnedCache::lock(): Argument #1 ($key) must be a non-empty string
lock-negative-lease: ValueError: OPcache\PinnedCache::lock(): Argument #2 ($lease) must be greater than or equal to 0
lock-reserved-class: ValueError: OPcache\PinnedCache::lock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
lock-loaded-class: ValueError: OPcache\PinnedCache::lock(): Argument #1 ($key) must not be a loaded class name
unlock-empty: ValueError: OPcache\PinnedCache::unlock(): Argument #1 ($key) must be a non-empty string
unlock-reserved-class: ValueError: OPcache\PinnedCache::unlock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
unlock-loaded-class: ValueError: OPcache\PinnedCache::unlock(): Argument #1 ($key) must not be a loaded class name
delete-empty: ValueError: OPcache\PinnedCache::delete(): Argument #1 ($key_or_class) must be a non-empty string
delete-reserved-class: ValueError: OPcache\PinnedCache::delete(): Argument #1 ($key_or_class) must not start with a reserved static-cache class key prefix
delete-array-empty: ValueError: OPcache\PinnedCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-object: ValueError: OPcache\PinnedCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-reserved-class: ValueError: OPcache\PinnedCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-loaded-class: ValueError: OPcache\PinnedCache::deleteMultiple(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
atomic-increment-reserved-class: ValueError: OPcache\PinnedCache::increment(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
atomic-decrement-reserved-class: ValueError: OPcache\PinnedCache::decrement(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
atomic-increment-loaded-class: ValueError: OPcache\PinnedCache::increment(): Argument #1 ($key) must not be a loaded class name
atomic-decrement-loaded-class: ValueError: OPcache\PinnedCache::decrement(): Argument #1 ($key) must not be a loaded class name
