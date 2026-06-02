--TEST--
OPcache getCacheStoreType reports how explicit and attribute-backed values are stored
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

use OPcache\VolatileCache;
use OPcache\PinnedCache;
use OPcache\CacheStoreType;

class StoreTypePlainBox
{
	public int $a = 1;
	public string $b = "x";
}

class StoreTypeMetadata
{
	#[OPcache\PinnedStatic]
	public static array $routes = [];

	#[OPcache\PinnedStatic]
	public static int $count = 0;
}

/* Enum surface. */
echo implode(',', array_map(static fn (CacheStoreType $c): string => $c->name, CacheStoreType::cases())), "\n";

/* Explicit volatile keys: each storage strategy is surfaced without decoding. */
VolatileCache::set('scalar_int', 123);
VolatileCache::set('scalar_str', 'hello');
VolatileCache::set('scalar_array', [1, 2, 3]);
VolatileCache::set('plain_object', new StoreTypePlainBox());
VolatileCache::set('dynamic_object', (object) ['a' => 1]);

foreach (['scalar_int', 'scalar_str', 'scalar_array', 'plain_object', 'dynamic_object', 'absent'] as $key) {
	printf("volatile %-15s => %s\n", $key, VolatileCache::getCacheStoreType($key)->name);
}

/* The pinned backend uses the same machinery. */
PinnedCache::set('pinned_scalar', 7);
PinnedCache::set('pinned_graph', ['x' => 1]);
printf("pinned   %-15s => %s\n", 'pinned_scalar', PinnedCache::getCacheStoreType('pinned_scalar')->name);
printf("pinned   %-15s => %s\n", 'pinned_graph', PinnedCache::getCacheStoreType('pinned_graph')->name);

/* A volatile key is not visible on the pinned backend and vice versa. */
printf("cross    %-15s => %s\n", 'scalar_int', PinnedCache::getCacheStoreType('scalar_int')->name);

/* Attribute-backed static-property storage: pass the property name plus class. */
StoreTypeMetadata::$routes = ['a' => 1, 'b' => 2];
StoreTypeMetadata::$count = 5;
printf("attr     %-15s => %s\n", 'routes', PinnedCache::getCacheStoreType('routes', StoreTypeMetadata::class)->name);
printf("attr     %-15s => %s\n", 'count', PinnedCache::getCacheStoreType('count', StoreTypeMetadata::class)->name);
printf("attr     %-15s => %s\n", 'missing', PinnedCache::getCacheStoreType('missing', StoreTypeMetadata::class)->name);
printf("attr     %-15s => %s\n", '\\-prefixed', PinnedCache::getCacheStoreType('routes', '\\' . StoreTypeMetadata::class)->name);

/* An empty key is an argument error. */
try {
	VolatileCache::getCacheStoreType('');
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
pinned   pinned_scalar   => Scalar
pinned   pinned_graph    => SharedGraph
cross    scalar_int      => NotFound
attr     routes          => SharedGraph
attr     count           => Scalar
attr     missing         => NotFound
attr     \-prefixed      => SharedGraph
OPcache\VolatileCache::getCacheStoreType(): Argument #1 ($key_or_property) must be a non-empty string
