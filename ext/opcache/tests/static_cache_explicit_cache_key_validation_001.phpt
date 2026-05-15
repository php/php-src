--TEST--
OPcache explicit cache APIs reject empty and non-scalar cache keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
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

foreach (['volatile', 'persistent'] as $backend) {
	echo $backend, "\n";

	$store = $backend === 'volatile' ? OPcache\volatile_store(...) : OPcache\persistent_store(...);
	$storeArray = $backend === 'volatile' ? OPcache\volatile_store_array(...) : OPcache\persistent_store_array(...);
	$fetch = $backend === 'volatile' ? OPcache\volatile_fetch(...) : OPcache\persistent_fetch(...);
	$fetchArray = $backend === 'volatile' ? OPcache\volatile_fetch_array(...) : OPcache\persistent_fetch_array(...);
	$exists = $backend === 'volatile' ? OPcache\volatile_exists(...) : OPcache\persistent_exists(...);
	$lock = $backend === 'volatile' ? OPcache\volatile_lock(...) : OPcache\persistent_lock(...);
	$delete = $backend === 'volatile' ? OPcache\volatile_delete(...) : OPcache\persistent_delete(...);
	$deleteArray = $backend === 'volatile' ? OPcache\volatile_delete_array(...) : OPcache\persistent_delete_array(...);

	dump_error('store-empty', static fn () => $store('', 'value'));
	dump_error('store-array-empty', static fn () => $storeArray(['' => 'value']));
	dump_error('fetch-empty', static fn () => $fetch(''));
	dump_error('fetch-array-empty', static fn () => $fetchArray(['']));
	dump_error('fetch-array-object', static fn () => $fetchArray([new StringableKey()]));
	dump_error('exists-empty', static fn () => $exists(''));
	dump_error('lock-empty', static fn () => $lock(''));
	dump_error('delete-empty', static fn () => $delete(''));
	dump_error('delete-array-empty', static fn () => $deleteArray(['']));
	dump_error('delete-array-object', static fn () => $deleteArray([new StringableKey()]));
}

?>
--EXPECTF--
volatile
store-empty: ValueError: OPcache\volatile_store(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\volatile_store_array(): Argument #1 ($values) must be an array with non-empty string keys
fetch-empty: ValueError: OPcache\volatile_fetch(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\volatile_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
fetch-array-object: ValueError: OPcache\volatile_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
exists-empty: ValueError: OPcache\volatile_exists(): Argument #1 ($key) must be a non-empty string
lock-empty: ValueError: OPcache\volatile_lock(): Argument #1 ($key) must be a non-empty string
delete-empty: ValueError: OPcache\volatile_delete(): Argument #1 ($key) must be a non-empty string
delete-array-empty: ValueError: OPcache\volatile_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
delete-array-object: ValueError: OPcache\volatile_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
persistent
store-empty: ValueError: OPcache\persistent_store(): Argument #1 ($key) must be a non-empty string
store-array-empty: ValueError: OPcache\persistent_store_array(): Argument #1 ($values) must be an array with non-empty string keys
fetch-empty: ValueError: OPcache\persistent_fetch(): Argument #1 ($key) must be a non-empty string
fetch-array-empty: ValueError: OPcache\persistent_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
fetch-array-object: ValueError: OPcache\persistent_fetch_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
exists-empty: ValueError: OPcache\persistent_exists(): Argument #1 ($key) must be a non-empty string
lock-empty: ValueError: OPcache\persistent_lock(): Argument #1 ($key) must be a non-empty string
delete-empty: ValueError: OPcache\persistent_delete(): Argument #1 ($key) must be a non-empty string
delete-array-empty: ValueError: OPcache\persistent_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
delete-array-object: ValueError: OPcache\persistent_delete_array(): Argument #1 ($keys) must contain only non-empty string or int cache keys
