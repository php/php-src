--TEST--
OPcache getCacheStoreType reports how explicit and attribute-backed values are stored
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

use OPcache\VolatileCache;
use OPcache\StableCache;
use OPcache\CacheStoreType;

class StoreTypePlainBox
{
	public int $a = 1;
	public string $b = "x";
}

class StoreTypeMetadata
{
	#[OPcache\StableStatic]
	public static array $routes = [];

	#[OPcache\StableStatic]
	public static int $count = 0;

	#[OPcache\StableStatic]
	public static function next(): int
	{
		static $counter = 0;

		return ++$counter;
	}
}

class StoreTypeVolatileMetadata
{
	#[OPcache\VolatileStatic]
	public static array $routes = [];

	#[OPcache\VolatileStatic]
	public static function next(): int
	{
		static $counter = 0;

		return ++$counter;
	}
}

/* Enum surface. */
echo implode(',', array_map(static fn (CacheStoreType $c): string => $c->name, CacheStoreType::cases())), "\n";

/* Explicit volatile keys: each storage strategy is surfaced without decoding. */
VolatileCache::getInstance('default')->store('scalar_int', 123);
VolatileCache::getInstance('default')->store('scalar_str', 'hello');
VolatileCache::getInstance('default')->store('scalar_array', [1, 2, 3]);
VolatileCache::getInstance('default')->store('plain_object', new StoreTypePlainBox());
VolatileCache::getInstance('default')->store('dynamic_object', (object) ['a' => 1]);

foreach (['scalar_int', 'scalar_str', 'scalar_array', 'plain_object', 'dynamic_object', 'absent'] as $key) {
	printf("volatile %-15s => %s\n", $key, VolatileCache::getInstance('default')->getCacheStoreType($key)->name);
}

/* The stable backend uses the same machinery. */
StableCache::getInstance('default')->store('stable_scalar', 7);
StableCache::getInstance('default')->store('stable_graph', ['x' => 1]);
printf("stable   %-15s => %s\n", 'stable_scalar', StableCache::getInstance('default')->getCacheStoreType('stable_scalar')->name);
printf("stable   %-15s => %s\n", 'stable_graph', StableCache::getInstance('default')->getCacheStoreType('stable_graph')->name);

/* A volatile key is not visible on the stable backend and vice versa. */
printf("cross    %-15s => %s\n", 'scalar_int', StableCache::getInstance('default')->getCacheStoreType('scalar_int')->name);

/* Attribute-backed static-property and method-static storage: use the dedicated static introspection APIs. */
StoreTypeMetadata::$routes = ['a' => 1, 'b' => 2];
StoreTypeMetadata::$count = 5;
StoreTypeMetadata::next();
StoreTypeVolatileMetadata::$routes = ['a' => 1];
StoreTypeVolatileMetadata::next();
printf("attr     %-15s => %s\n", 'routes', StableCache::getCacheStoreTypeByProperty(StoreTypeMetadata::class, 'routes')->name);
printf("attr     %-15s => %s\n", 'count', StableCache::getCacheStoreTypeByProperty(StoreTypeMetadata::class, 'count')->name);
printf("attr     %-15s => %s\n", 'method', StableCache::getCacheStoreTypeByMethod(StoreTypeMetadata::class, 'next', 'counter')->name);
printf("attr     %-15s => %s\n", 'missing', StableCache::getCacheStoreTypeByProperty(StoreTypeMetadata::class, 'missing')->name);
printf("attr     %-15s => %s\n", '\\-prefixed', StableCache::getCacheStoreTypeByProperty('\\' . StoreTypeMetadata::class, 'routes')->name);
printf("volatile attr %-8s => %s\n", 'routes', VolatileCache::getCacheStoreTypeByProperty(StoreTypeVolatileMetadata::class, 'routes')->name);
printf("volatile attr %-8s => %s\n", 'method', VolatileCache::getCacheStoreTypeByMethod(StoreTypeVolatileMetadata::class, 'next', 'counter')->name);

/* An empty key is an argument error. */
try {
	VolatileCache::getInstance('default')->getCacheStoreType('');
} catch (ValueError $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
NotFound,Scalar,SharedGraph,OPcacheSerialized,PHPSerialized
volatile scalar_int      => Scalar
volatile scalar_str      => Scalar
volatile scalar_array    => SharedGraph
volatile plain_object    => SharedGraph
volatile dynamic_object  => OPcacheSerialized
volatile absent          => NotFound
stable   stable_scalar   => Scalar
stable   stable_graph    => SharedGraph
cross    scalar_int      => NotFound
attr     routes          => SharedGraph
attr     count           => Scalar
attr     method          => NotFound
attr     missing         => NotFound
attr     \-prefixed      => SharedGraph
volatile attr routes   => SharedGraph
volatile attr method   => NotFound
OPcache\VolatileCache::getCacheStoreType(): Argument #1 ($key) must be a non-empty string
