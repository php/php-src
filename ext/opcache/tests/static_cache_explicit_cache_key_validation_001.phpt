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
	$store = $backend === 'volatile' ? OPcache\volatile_store(...) : OPcache\pinned_store(...);
	$storeArray = $backend === 'volatile' ? OPcache\volatile_store_array(...) : OPcache\pinned_store_array(...);
	$fetch = $backend === 'volatile' ? OPcache\volatile_fetch(...) : OPcache\pinned_fetch(...);
	$fetchArray = $backend === 'volatile' ? OPcache\volatile_fetch_array(...) : OPcache\pinned_fetch_array(...);
	$exists = $backend === 'volatile' ? OPcache\volatile_exists(...) : OPcache\pinned_exists(...);
	$lock = $backend === 'volatile' ? OPcache\volatile_lock(...) : OPcache\pinned_lock(...);
	$unlock = $backend === 'volatile' ? OPcache\volatile_unlock(...) : OPcache\pinned_unlock(...);
	$delete = $backend === 'volatile' ? OPcache\volatile_delete(...) : OPcache\pinned_delete(...);
	$deleteArray = $backend === 'volatile' ? OPcache\volatile_delete_array(...) : OPcache\pinned_delete_array(...);

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
		dump_error('atomic-increment-reserved-class', static fn () => OPcache\pinned_atomic_increment($reservedClassKey));
		dump_error('atomic-decrement-reserved-class', static fn () => OPcache\pinned_atomic_decrement($reservedClassKey));
		dump_error('atomic-increment-loaded-class', static fn () => OPcache\pinned_atomic_increment($loadedClassKey));
		dump_error('atomic-decrement-loaded-class', static fn () => OPcache\pinned_atomic_decrement($loadedClassKey));
	}
}

?>
--EXPECTF--
volatile
store-empty: ValueError: OPcache\volatile_store(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\volatile_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-reserved-class: ValueError: OPcache\volatile_store(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
store-array-reserved-class: ValueError: OPcache\volatile_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-loaded-class: ValueError: OPcache\volatile_store(): Argument #1 ($key) must not be a loaded class name
store-array-loaded-class: ValueError: OPcache\volatile_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
fetch-empty: ValueError: OPcache\volatile_fetch(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\volatile_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-array-object: ValueError: OPcache\volatile_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-reserved-class: ValueError: OPcache\volatile_fetch(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
fetch-array-reserved-class: ValueError: OPcache\volatile_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-loaded-class: ValueError: OPcache\volatile_fetch(): Argument #1 ($key) must not be a loaded class name
fetch-array-loaded-class: ValueError: OPcache\volatile_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
exists-empty: ValueError: OPcache\volatile_exists(): Argument #1 ($key) must be a non-empty string
exists-reserved-class: ValueError: OPcache\volatile_exists(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
exists-loaded-class: ValueError: OPcache\volatile_exists(): Argument #1 ($key) must not be a loaded class name
lock-empty: ValueError: OPcache\volatile_lock(): Argument #1 ($key) must be a non-empty string
lock-negative-lease: ValueError: OPcache\volatile_lock(): Argument #2 ($lease) must be greater than or equal to 0
lock-reserved-class: ValueError: OPcache\volatile_lock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
lock-loaded-class: ValueError: OPcache\volatile_lock(): Argument #1 ($key) must not be a loaded class name
unlock-empty: ValueError: OPcache\volatile_unlock(): Argument #1 ($key) must be a non-empty string
unlock-reserved-class: ValueError: OPcache\volatile_unlock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
unlock-loaded-class: ValueError: OPcache\volatile_unlock(): Argument #1 ($key) must not be a loaded class name
delete-empty: ValueError: OPcache\volatile_delete(): Argument #1 ($key_or_class) must be a non-empty string
delete-reserved-class: ValueError: OPcache\volatile_delete(): Argument #1 ($key_or_class) must not start with a reserved static-cache class key prefix
delete-array-empty: ValueError: OPcache\volatile_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-object: ValueError: OPcache\volatile_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-reserved-class: ValueError: OPcache\volatile_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-loaded-class: ValueError: OPcache\volatile_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
pinned
store-empty: ValueError: OPcache\pinned_store(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\pinned_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-reserved-class: ValueError: OPcache\pinned_store(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
store-array-reserved-class: ValueError: OPcache\pinned_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
store-loaded-class: ValueError: OPcache\pinned_store(): Argument #1 ($key) must not be a loaded class name
store-array-loaded-class: ValueError: OPcache\pinned_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
fetch-empty: ValueError: OPcache\pinned_fetch(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\pinned_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-array-object: ValueError: OPcache\pinned_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-reserved-class: ValueError: OPcache\pinned_fetch(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
fetch-array-reserved-class: ValueError: OPcache\pinned_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
fetch-loaded-class: ValueError: OPcache\pinned_fetch(): Argument #1 ($key) must not be a loaded class name
fetch-array-loaded-class: ValueError: OPcache\pinned_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
exists-empty: ValueError: OPcache\pinned_exists(): Argument #1 ($key) must be a non-empty string
exists-reserved-class: ValueError: OPcache\pinned_exists(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
exists-loaded-class: ValueError: OPcache\pinned_exists(): Argument #1 ($key) must not be a loaded class name
lock-empty: ValueError: OPcache\pinned_lock(): Argument #1 ($key) must be a non-empty string
lock-negative-lease: ValueError: OPcache\pinned_lock(): Argument #2 ($lease) must be greater than or equal to 0
lock-reserved-class: ValueError: OPcache\pinned_lock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
lock-loaded-class: ValueError: OPcache\pinned_lock(): Argument #1 ($key) must not be a loaded class name
unlock-empty: ValueError: OPcache\pinned_unlock(): Argument #1 ($key) must be a non-empty string
unlock-reserved-class: ValueError: OPcache\pinned_unlock(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
unlock-loaded-class: ValueError: OPcache\pinned_unlock(): Argument #1 ($key) must not be a loaded class name
delete-empty: ValueError: OPcache\pinned_delete(): Argument #1 ($key_or_class) must be a non-empty string
delete-reserved-class: ValueError: OPcache\pinned_delete(): Argument #1 ($key_or_class) must not start with a reserved static-cache class key prefix
delete-array-empty: ValueError: OPcache\pinned_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-object: ValueError: OPcache\pinned_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-reserved-class: ValueError: OPcache\pinned_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
delete-array-loaded-class: ValueError: OPcache\pinned_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names
atomic-increment-reserved-class: ValueError: OPcache\pinned_atomic_increment(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
atomic-decrement-reserved-class: ValueError: OPcache\pinned_atomic_decrement(): Argument #1 ($key) must not start with a reserved static-cache class key prefix
atomic-increment-loaded-class: ValueError: OPcache\pinned_atomic_increment(): Argument #1 ($key) must not be a loaded class name
atomic-decrement-loaded-class: ValueError: OPcache\pinned_atomic_decrement(): Argument #1 ($key) must not be a loaded class name
