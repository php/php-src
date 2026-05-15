--TEST--
OPcache PersistentStatic storage failures throw StaticCacheException
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=8
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

class PersistentStaticStoreFailureProperty
{
	#[OPcache\PersistentStatic]
	public static mixed $value = null;
}

#[OPcache\PersistentStatic]
class PersistentStaticStoreFailureClass
{
	public static mixed $value = null;
}

class PersistentStaticStoreFailureMethod
{
	#[OPcache\PersistentStatic]
	public static function assign(mixed $value): void
	{
		static $state = null;

		$state = $value;
	}
}

class PersistentStaticStoreFailureArray
{
	#[OPcache\PersistentStatic]
	public static array $value = [];
}

class PersistentStaticStoreFailureUnsupportedValueBox
{
	public function __construct(public mixed $value)
	{
	}
}

OPcache\persistent_clear();

dump_static_cache_exception('property-resource', function (): void {
	$resource = fopen('/dev/null', 'r');
	try {
		PersistentStaticStoreFailureProperty::$value = $resource;
	} finally {
		if (is_resource($resource)) {
			fclose($resource);
		}
	}
});

dump_static_cache_exception('property-closure', function (): void {
	PersistentStaticStoreFailureProperty::$value = static fn () => null;
});

dump_static_cache_exception('property-object-resource', function (): void {
	$resource = fopen('/dev/null', 'r');
	try {
		PersistentStaticStoreFailureProperty::$value = new PersistentStaticStoreFailureUnsupportedValueBox($resource);
	} finally {
		if (is_resource($resource)) {
			fclose($resource);
		}
	}
});

dump_static_cache_exception('property-object-closure', function (): void {
	PersistentStaticStoreFailureProperty::$value = new PersistentStaticStoreFailureUnsupportedValueBox(static fn () => null);
});

$unused = PersistentStaticStoreFailureClass::$value;
dump_static_cache_exception('class-closure', function (): void {
	PersistentStaticStoreFailureClass::$value = static fn () => null;
});

dump_static_cache_exception('method-closure', function (): void {
	PersistentStaticStoreFailureMethod::assign(static fn () => null);
});

PersistentStaticStoreFailureArray::$value = [];
dump_static_cache_exception('array-mutation-overflow', function (): void {
	PersistentStaticStoreFailureArray::$value[] = str_repeat('X', 12 * 1024 * 1024);
});

OPcache\persistent_clear();

?>
--EXPECT--
property-resource: OPcache\StaticCacheException: resources cannot be stored in the static cache
property-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
property-object-resource: OPcache\StaticCacheException: resources cannot be stored in the static cache
property-object-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
class-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
method-closure: OPcache\StaticCacheException: Closure objects cannot be stored in the static cache
array-mutation-overflow: OPcache\StaticCacheException: not enough shared memory left
