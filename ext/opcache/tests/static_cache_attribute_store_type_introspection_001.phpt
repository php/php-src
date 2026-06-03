--TEST--
OPcache attribute store type introspection reports property and method static entries after publication
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_attribute_store_type_introspection_001.php', <<<'PHP'
<?php

class StoreTypeStableAttributeState
{
	#[OPcache\StableStatic]
	public static array $routes = [];

	#[OPcache\StableStatic]
	public static function next(): int
	{
		static $counter = 0;

		return ++$counter;
	}
}

class StoreTypeVolatileAttributeState
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

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	StoreTypeStableAttributeState::$routes = ['stable' => true];
	StoreTypeStableAttributeState::next();
	StoreTypeVolatileAttributeState::$routes = ['volatile' => true];
	StoreTypeVolatileAttributeState::next();
	echo "seed\n";
	return;
}

printf("stable property => %s\n", OPcache\StableCache::getCacheStoreTypeByProperty(StoreTypeStableAttributeState::class, 'routes')->name);
printf("stable method   => %s\n", OPcache\StableCache::getCacheStoreTypeByMethod(StoreTypeStableAttributeState::class, 'next', 'counter')->name);
printf("volatile property => %s\n", OPcache\VolatileCache::getCacheStoreTypeByProperty(StoreTypeVolatileAttributeState::class, 'routes')->name);
printf("volatile method   => %s\n", OPcache\VolatileCache::getCacheStoreTypeByMethod(StoreTypeVolatileAttributeState::class, 'next', 'counter')->name);
printf("missing property => %s\n", OPcache\StableCache::getCacheStoreTypeByProperty(StoreTypeStableAttributeState::class, 'missing')->name);
printf("missing method   => %s\n", OPcache\StableCache::getCacheStoreTypeByMethod(StoreTypeStableAttributeState::class, 'next', 'missing')->name);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_attribute_store_type_introspection_001.php?action=seed');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_attribute_store_type_introspection_001.php?action=inspect');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/static_cache_attribute_store_type_introspection_001.php');
?>
--EXPECT--
seed
stable property => SharedGraph
stable method   => Scalar
volatile property => SharedGraph
volatile method   => Scalar
missing property => NotFound
missing method   => NotFound
