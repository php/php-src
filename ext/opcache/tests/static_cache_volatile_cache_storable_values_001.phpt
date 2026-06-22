--TEST--
OPcache static cache rejects resource and Closure values
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

dump_type_error(static fn () => OPcache\VolatileCache::getInstance('default')->store('resource', $resource));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('resource', 'missing'));
dump_type_error(static fn () => OPcache\VolatileCache::getInstance('default')->store('closure-value', $closure));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('closure-value', 'missing'));
dump_type_error(static fn () => OPcache\VolatileCache::getInstance('default')->storeMultiple(['resource-value' => $resource]));
dump_type_error(static fn () => OPcache\VolatileCache::getInstance('default')->storeMultiple(['closure-value' => $closure]));

var_dump(OPcache\VolatileCache::getInstance('default')->storeMultiple(['nested-resource' => ['value' => $resource]]));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('nested-resource', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->storeMultiple(['nested-closure' => ['value' => $closure]]));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('nested-closure', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('object-resource', $resource_object));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('object-resource', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('object-closure', $closure_object));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('object-closure', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('spl-resource', $resource_fixed_array));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('spl-resource', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('spl-closure', $closure_fixed_array));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('spl-closure', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('array-object-resource', $resource_array_object));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('array-object-resource', 'missing'));
var_dump(OPcache\VolatileCache::getInstance('default')->store('array-object-closure', $closure_array_object));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('array-object-closure', 'missing'));
StaticCacheUnsupportedSerializedPayload::$value = $resource;
var_dump(OPcache\VolatileCache::getInstance('default')->store('serialized-resource', $serialized_payload));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('serialized-resource', 'missing'));
StaticCacheUnsupportedSerializedPayload::$value = $closure;
var_dump(OPcache\VolatileCache::getInstance('default')->store('serialized-closure', $serialized_payload));
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('serialized-closure', 'missing'));

dump_type_error(static fn () => OPcache\VolatileCache::getInstance('default')->fetch('missing', $resource));
dump_type_error(static fn () => OPcache\VolatileCache::getInstance('default')->fetch('missing', $closure));

dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->store('resource', $resource));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->store('closure-value', $closure));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->storeMultiple(['resource-value' => $resource]));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->storeMultiple(['closure-value' => $closure]));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->storeMultipleWithTtl(['resource-value' => $resource], 1));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->storeMultipleWithTtl(['closure-value' => $closure], 1));
var_dump(OPcache\StableCache::getInstance('default')->storeMultiple(['nested-resource' => ['value' => $resource]]));
var_dump(OPcache\StableCache::getInstance('default')->storeMultiple(['nested-closure' => ['value' => $closure]]));
var_dump(OPcache\StableCache::getInstance('default')->store('object-resource', $resource_object));
var_dump(OPcache\StableCache::getInstance('default')->store('object-closure', $closure_object));
var_dump(OPcache\StableCache::getInstance('default')->store('spl-resource', $resource_fixed_array));
var_dump(OPcache\StableCache::getInstance('default')->store('spl-closure', $closure_fixed_array));
var_dump(OPcache\StableCache::getInstance('default')->store('array-object-resource', $resource_array_object));
var_dump(OPcache\StableCache::getInstance('default')->store('array-object-closure', $closure_array_object));
StaticCacheUnsupportedSerializedPayload::$value = $resource;
var_dump(OPcache\StableCache::getInstance('default')->store('serialized-resource', $serialized_payload));
StaticCacheUnsupportedSerializedPayload::$value = $closure;
var_dump(OPcache\StableCache::getInstance('default')->store('serialized-closure', $serialized_payload));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->fetch('missing', $resource));
dump_type_error(static fn () => OPcache\StableCache::getInstance('default')->fetch('missing', $closure));

fclose($resource);

?>
--EXPECT--
OPcache\VolatileCache::store(): Argument #2 ($value) must be of type object|array|string|int|float|bool|null, resource given
string(7) "missing"
OPcache\VolatileCache::store(): Argument #2 ($value) must not be a Closure object
string(7) "missing"
OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must contain only values of type object|array|string|int|float|bool|null, resource given
OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must not contain Closure objects
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
OPcache\VolatileCache::fetch(): Argument #2 ($default) must be of type object|array|string|int|float|bool|null, resource given
OPcache\VolatileCache::fetch(): Argument #2 ($default) must not be a Closure object
OPcache\StableCache::store(): Argument #2 ($value) must be of type object|array|string|int|float|bool|null, resource given
OPcache\StableCache::store(): Argument #2 ($value) must not be a Closure object
OPcache\StableCache::storeMultiple(): Argument #1 ($values) must contain only values of type object|array|string|int|float|bool|null, resource given
OPcache\StableCache::storeMultiple(): Argument #1 ($values) must not contain Closure objects
OPcache\StableCache::storeMultipleWithTtl(): Argument #1 ($values) must contain only values of type object|array|string|int|float|bool|null, resource given
OPcache\StableCache::storeMultipleWithTtl(): Argument #1 ($values) must not contain Closure objects
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
OPcache\StableCache::fetch(): Argument #2 ($default) must be of type object|array|string|int|float|bool|null, resource given
OPcache\StableCache::fetch(): Argument #2 ($default) must not be a Closure object
