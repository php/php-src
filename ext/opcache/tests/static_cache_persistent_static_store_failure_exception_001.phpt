--TEST--
OPcache PinnedStatic storage failures throw StaticCacheException
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=8
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

class PinnedStaticStoreFailureProperty
{
	#[OPcache\PinnedStatic]
	public static mixed $value = null;
}

#[OPcache\PinnedStatic]
class PinnedStaticStoreFailureClass
{
	public static mixed $value = null;
}

class PinnedStaticStoreFailureMethod
{
	#[OPcache\PinnedStatic]
	public static function assign(mixed $value): void
	{
		static $state = null;

		$state = $value;
	}
}

class PinnedStaticStoreFailureArray
{
	#[OPcache\PinnedStatic]
	public static array $value = [];
}

class PinnedStaticStoreFailureUnsupportedValueBox
{
	public function __construct(public mixed $value)
	{
	}
}

OPcache\pinned_clear();

dump_static_cache_exception('property-resource', function (): void {
	$resource = fopen('/dev/null', 'r');
	try {
		PinnedStaticStoreFailureProperty::$value = $resource;
	} finally {
		if (is_resource($resource)) {
			fclose($resource);
		}
	}
});

dump_static_cache_exception('property-closure', function (): void {
	PinnedStaticStoreFailureProperty::$value = static fn () => null;
});

dump_static_cache_exception('property-object-resource', function (): void {
	$resource = fopen('/dev/null', 'r');
	try {
		PinnedStaticStoreFailureProperty::$value = new PinnedStaticStoreFailureUnsupportedValueBox($resource);
	} finally {
		if (is_resource($resource)) {
			fclose($resource);
		}
	}
});

dump_static_cache_exception('property-object-closure', function (): void {
	PinnedStaticStoreFailureProperty::$value = new PinnedStaticStoreFailureUnsupportedValueBox(static fn () => null);
});

$unused = PinnedStaticStoreFailureClass::$value;
dump_static_cache_exception('class-closure', function (): void {
	PinnedStaticStoreFailureClass::$value = static fn () => null;
});

dump_static_cache_exception('method-closure', function (): void {
	PinnedStaticStoreFailureMethod::assign(static fn () => null);
});

PinnedStaticStoreFailureArray::$value = [];
dump_static_cache_exception('array-mutation-overflow', function (): void {
	PinnedStaticStoreFailureArray::$value[] = str_repeat('X', 12 * 1024 * 1024);
});

OPcache\pinned_clear();

?>
--EXPECT--
property-resource: OPcache\StaticCacheException: resources cannot be stored in the static cache
property-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
property-object-resource: OPcache\StaticCacheException: resources cannot be stored in the static cache
property-object-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
class-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
method-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
array-mutation-overflow: OPcache\StaticCacheException: not enough shared memory left
