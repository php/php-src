--TEST--
OPcache static cache rejects resource and Closure values
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

dump_type_error(static fn () => OPcache\VolatileCache::set('resource', $resource));
var_dump(OPcache\VolatileCache::get('resource', 'missing'));
dump_type_error(static fn () => OPcache\VolatileCache::set('closure-value', $closure));
var_dump(OPcache\VolatileCache::get('closure-value', 'missing'));

var_dump(OPcache\VolatileCache::setMultiple(['nested-resource' => ['value' => $resource]]));
var_dump(OPcache\VolatileCache::get('nested-resource', 'missing'));
var_dump(OPcache\VolatileCache::setMultiple(['nested-closure' => ['value' => $closure]]));
var_dump(OPcache\VolatileCache::get('nested-closure', 'missing'));
var_dump(OPcache\VolatileCache::set('object-resource', $resource_object));
var_dump(OPcache\VolatileCache::get('object-resource', 'missing'));
var_dump(OPcache\VolatileCache::set('object-closure', $closure_object));
var_dump(OPcache\VolatileCache::get('object-closure', 'missing'));
var_dump(OPcache\VolatileCache::set('spl-resource', $resource_fixed_array));
var_dump(OPcache\VolatileCache::get('spl-resource', 'missing'));
var_dump(OPcache\VolatileCache::set('spl-closure', $closure_fixed_array));
var_dump(OPcache\VolatileCache::get('spl-closure', 'missing'));
var_dump(OPcache\VolatileCache::set('array-object-resource', $resource_array_object));
var_dump(OPcache\VolatileCache::get('array-object-resource', 'missing'));
var_dump(OPcache\VolatileCache::set('array-object-closure', $closure_array_object));
var_dump(OPcache\VolatileCache::get('array-object-closure', 'missing'));
StaticCacheUnsupportedSerializedPayload::$value = $resource;
var_dump(OPcache\VolatileCache::set('serialized-resource', $serialized_payload));
var_dump(OPcache\VolatileCache::get('serialized-resource', 'missing'));
StaticCacheUnsupportedSerializedPayload::$value = $closure;
var_dump(OPcache\VolatileCache::set('serialized-closure', $serialized_payload));
var_dump(OPcache\VolatileCache::get('serialized-closure', 'missing'));

dump_type_error(static fn () => OPcache\VolatileCache::get('missing', $resource));
dump_type_error(static fn () => OPcache\VolatileCache::get('missing', $closure));

dump_type_error(static fn () => OPcache\PinnedCache::set('resource', $resource));
dump_type_error(static fn () => OPcache\PinnedCache::set('closure-value', $closure));
var_dump(OPcache\PinnedCache::setMultiple(['nested-resource' => ['value' => $resource]]));
var_dump(OPcache\PinnedCache::setMultiple(['nested-closure' => ['value' => $closure]]));
var_dump(OPcache\PinnedCache::set('object-resource', $resource_object));
var_dump(OPcache\PinnedCache::set('object-closure', $closure_object));
var_dump(OPcache\PinnedCache::set('spl-resource', $resource_fixed_array));
var_dump(OPcache\PinnedCache::set('spl-closure', $closure_fixed_array));
var_dump(OPcache\PinnedCache::set('array-object-resource', $resource_array_object));
var_dump(OPcache\PinnedCache::set('array-object-closure', $closure_array_object));
StaticCacheUnsupportedSerializedPayload::$value = $resource;
var_dump(OPcache\PinnedCache::set('serialized-resource', $serialized_payload));
StaticCacheUnsupportedSerializedPayload::$value = $closure;
var_dump(OPcache\PinnedCache::set('serialized-closure', $serialized_payload));
dump_type_error(static fn () => OPcache\PinnedCache::get('missing', $resource));
dump_type_error(static fn () => OPcache\PinnedCache::get('missing', $closure));

fclose($resource);

?>
--EXPECT--
OPcache\VolatileCache::set(): Argument #2 ($value) must be of type object|array|string|int|float|bool|null, resource given
string(7) "missing"
OPcache\VolatileCache::set(): Argument #2 ($value) must not be a Closure object
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
OPcache\VolatileCache::get(): Argument #2 ($default) must be of type object|array|string|int|float|bool|null, resource given
OPcache\VolatileCache::get(): Argument #2 ($default) must not be a Closure object
OPcache\PinnedCache::set(): Argument #2 ($value) must be of type object|array|string|int|float|bool|null, resource given
OPcache\PinnedCache::set(): Argument #2 ($value) must not be a Closure object
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
OPcache\PinnedCache::get(): Argument #2 ($default) must be of type object|array|string|int|float|bool|null, resource given
OPcache\PinnedCache::get(): Argument #2 ($default) must not be a Closure object
