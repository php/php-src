--TEST--
OPcache StableStatic storage failures throw StaticCacheException
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=8
opcache.optimization_level=0
opcache.file_update_protection=0
opcache.jit=0
--FILE--
<?php

function dump_static_cache_exception(string $label, Closure $callback): void
{
	try {
		$callback();
		echo $label, ": no exception\n";
	} catch (OPcache\StaticCacheException $exception) {
		echo $label, ': ', get_class($exception), ': ', $exception->getMessage(), "\n";
	}
}

class StableStaticStoreFailureProperty
{
	#[OPcache\StableStatic]
	public static mixed $value = null;
}

#[OPcache\StableStatic]
class StableStaticStoreFailureClass
{
	public static mixed $value = null;
}

class StableStaticStoreFailureMethod
{
	#[OPcache\StableStatic]
	public static function assign(mixed $value): void
	{
		static $state = null;

		$state = $value;
	}
}

class StableStaticStoreFailureArray
{
	#[OPcache\StableStatic]
	public static array $value = [];
}

class StableStaticStoreFailureUnsupportedValueBox
{
	public function __construct(public mixed $value)
	{
	}
}

OPcache\StableCache::getInstance('default')->clear();
$nullDevice = PHP_OS_FAMILY === 'Windows' ? 'NUL' : '/dev/null';

dump_static_cache_exception('property-resource', function (): void {
	global $nullDevice;

	$resource = fopen($nullDevice, 'r');
	try {
		StableStaticStoreFailureProperty::$value = $resource;
	} finally {
		if (is_resource($resource)) {
			fclose($resource);
		}
	}
});

dump_static_cache_exception('property-closure', function (): void {
	StableStaticStoreFailureProperty::$value = static fn () => null;
});

dump_static_cache_exception('property-object-resource', function (): void {
	global $nullDevice;

	$resource = fopen($nullDevice, 'r');
	try {
		StableStaticStoreFailureProperty::$value = new StableStaticStoreFailureUnsupportedValueBox($resource);
	} finally {
		if (is_resource($resource)) {
			fclose($resource);
		}
	}
});

dump_static_cache_exception('property-object-closure', function (): void {
	StableStaticStoreFailureProperty::$value = new StableStaticStoreFailureUnsupportedValueBox(static fn () => null);
});

$unused = StableStaticStoreFailureClass::$value;
dump_static_cache_exception('class-closure', function (): void {
	StableStaticStoreFailureClass::$value = static fn () => null;
});

dump_static_cache_exception('method-closure', function (): void {
	StableStaticStoreFailureMethod::assign(static fn () => null);
});

StableStaticStoreFailureArray::$value = [];
dump_static_cache_exception('array-mutation-overflow', function (): void {
	StableStaticStoreFailureArray::$value[] = str_repeat('X', 12 * 1024 * 1024);
});

OPcache\StableCache::getInstance('default')->clear();

?>
--EXPECT--
property-resource: OPcache\StaticCacheException: resources cannot be stored in the static cache
property-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
property-object-resource: OPcache\StaticCacheException: resources cannot be stored in the static cache
property-object-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
class-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
method-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
array-mutation-overflow: OPcache\StaticCacheException: not enough shared memory left
