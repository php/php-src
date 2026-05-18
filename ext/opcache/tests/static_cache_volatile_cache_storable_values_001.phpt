--TEST--
OPcache static cache rejects resource and Closure values
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

use OPcache\StaticCacheException;

class StaticCacheUnsupportedValueBox
{
	public function __construct(public mixed $value)
	{
	}
}

$resource = fopen(__FILE__, 'r');
$closure = static fn () => true;
$resource_object = new StaticCacheUnsupportedValueBox($resource);
$closure_object = new StaticCacheUnsupportedValueBox($closure);
$resource_fixed_array = new SplFixedArray(1);
$resource_fixed_array[0] = $resource;
$closure_fixed_array = new SplFixedArray(1);
$closure_fixed_array[0] = $closure;
$resource_array_object = new ArrayObject(['value' => $resource]);
$closure_array_object = new ArrayObject(['value' => $closure]);

class StaticCacheUnsupportedSerializedPayload
{
	public static mixed $value = null;

	public function __serialize(): array
	{
		return ['value' => self::$value];
	}
}

$serialized_payload = new StaticCacheUnsupportedSerializedPayload();

function dump_type_error(Closure $callback): void
{
	try {
		$callback();
	} catch (TypeError $e) {
		echo $e->getMessage(), "\n";
	}
}

function dump_static_cache_exception(Closure $callback): void
{
	try {
		$callback();
	} catch (StaticCacheException $e) {
		echo get_class($e), ': ', $e->getMessage(), "\n";
	}
}

dump_type_error(static fn () => OPcache\volatile_store('resource', $resource));
var_dump(OPcache\volatile_fetch('resource', 'missing'));
dump_type_error(static fn () => OPcache\volatile_store('closure-value', $closure));
var_dump(OPcache\volatile_fetch('closure-value', 'missing'));

var_dump(OPcache\volatile_store_array(['nested-resource' => ['value' => $resource]]));
var_dump(OPcache\volatile_fetch('nested-resource', 'missing'));
var_dump(OPcache\volatile_store_array(['nested-closure' => ['value' => $closure]]));
var_dump(OPcache\volatile_fetch('nested-closure', 'missing'));
var_dump(OPcache\volatile_store('object-resource', $resource_object));
var_dump(OPcache\volatile_fetch('object-resource', 'missing'));
var_dump(OPcache\volatile_store('object-closure', $closure_object));
var_dump(OPcache\volatile_fetch('object-closure', 'missing'));
var_dump(OPcache\volatile_store('spl-resource', $resource_fixed_array));
var_dump(OPcache\volatile_fetch('spl-resource', 'missing'));
var_dump(OPcache\volatile_store('spl-closure', $closure_fixed_array));
var_dump(OPcache\volatile_fetch('spl-closure', 'missing'));
var_dump(OPcache\volatile_store('array-object-resource', $resource_array_object));
var_dump(OPcache\volatile_fetch('array-object-resource', 'missing'));
var_dump(OPcache\volatile_store('array-object-closure', $closure_array_object));
var_dump(OPcache\volatile_fetch('array-object-closure', 'missing'));
StaticCacheUnsupportedSerializedPayload::$value = $resource;
var_dump(OPcache\volatile_store('serialized-resource', $serialized_payload));
var_dump(OPcache\volatile_fetch('serialized-resource', 'missing'));
StaticCacheUnsupportedSerializedPayload::$value = $closure;
var_dump(OPcache\volatile_store('serialized-closure', $serialized_payload));
var_dump(OPcache\volatile_fetch('serialized-closure', 'missing'));

dump_type_error(static fn () => OPcache\volatile_fetch('missing', $resource));
dump_type_error(static fn () => OPcache\volatile_fetch('missing', $closure));

dump_type_error(static fn () => OPcache\persistent_store('resource', $resource));
dump_type_error(static fn () => OPcache\persistent_store('closure-value', $closure));
dump_static_cache_exception(static fn () => OPcache\persistent_store_array(['nested-resource' => ['value' => $resource]]));
dump_static_cache_exception(static fn () => OPcache\persistent_store_array(['nested-closure' => ['value' => $closure]]));
dump_static_cache_exception(static fn () => OPcache\persistent_store('object-resource', $resource_object));
dump_static_cache_exception(static fn () => OPcache\persistent_store('object-closure', $closure_object));
dump_static_cache_exception(static fn () => OPcache\persistent_store('spl-resource', $resource_fixed_array));
dump_static_cache_exception(static fn () => OPcache\persistent_store('spl-closure', $closure_fixed_array));
dump_static_cache_exception(static fn () => OPcache\persistent_store('array-object-resource', $resource_array_object));
dump_static_cache_exception(static fn () => OPcache\persistent_store('array-object-closure', $closure_array_object));
StaticCacheUnsupportedSerializedPayload::$value = $resource;
dump_static_cache_exception(static fn () => OPcache\persistent_store('serialized-resource', $serialized_payload));
StaticCacheUnsupportedSerializedPayload::$value = $closure;
dump_static_cache_exception(static fn () => OPcache\persistent_store('serialized-closure', $serialized_payload));
dump_type_error(static fn () => OPcache\persistent_fetch('missing', $resource));
dump_type_error(static fn () => OPcache\persistent_fetch('missing', $closure));

fclose($resource);

?>
--EXPECT--
OPcache\volatile_store(): Argument #2 ($value) must be of type object|array|string|int|float|bool|null, resource given
string(7) "missing"
OPcache\volatile_store(): Argument #2 ($value) must not be a Closure object
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
bool(false)
string(7) "missing"
OPcache\volatile_fetch(): Argument #2 ($default) must be of type object|array|string|int|float|bool|null, resource given
OPcache\volatile_fetch(): Argument #2 ($default) must not be a Closure object
OPcache\persistent_store(): Argument #2 ($value) must be of type object|array|string|int|float|bool|null, resource given
OPcache\persistent_store(): Argument #2 ($value) must not be a Closure object
OPcache\StaticCacheException: resources cannot be stored in the static cache
OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources cannot be stored in the static cache
OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources and Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources and Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources and Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources and Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources and Closure objects cannot be stored in the static cache
OPcache\StaticCacheException: resources and Closure objects cannot be stored in the static cache
OPcache\persistent_fetch(): Argument #2 ($default) must be of type object|array|string|int|float|bool|null, resource given
OPcache\persistent_fetch(): Argument #2 ($default) must not be a Closure object
