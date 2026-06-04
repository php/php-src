--TEST--
OPcache explicit object fetch keeps array properties isolated on request-local clone
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

final class ExplicitObjectArrayPropertyChild
{
	public function __construct(public string $label) {}
}

final class ExplicitObjectArrayPropertyPayload
{
	public array $scalarTree;
	public array $objectTree;

	public function __construct(string $prefix)
	{
		$child = new ExplicitObjectArrayPropertyChild($prefix . '-child');
		$this->scalarTree = [
			'routes' => [
				['name' => $prefix . '-route-0', 'score' => 10],
				['name' => $prefix . '-route-1', 'score' => 20],
			],
			'meta' => ['owner' => $prefix, 'enabled' => true],
		];
		$this->objectTree = [
			'primary' => $child,
			'alias' => $child,
		];
	}
}

function check_array_property_clone(string $name, object $cache): void
{
	$cache->store('object_array_property', new ExplicitObjectArrayPropertyPayload($name));

	$first = $cache->fetch('object_array_property');
	$warm = $cache->fetch('object_array_property');
	var_dump($first === $warm);
	var_dump($first->objectTree['primary'] === $first->objectTree['alias']);
	var_dump($warm->objectTree['primary'] === $warm->objectTree['alias']);
	var_dump($first->objectTree['primary'] === $warm->objectTree['primary']);

	$first->scalarTree['routes'][0]['name'] = $name . '-first-mutated-route';
	$first->objectTree['primary']->label = $name . '-first-mutated-child';

	$second = $cache->fetch('object_array_property');
	var_dump($first === $second);
	var_dump($second->objectTree['primary'] === $second->objectTree['alias']);
	var_dump($first->objectTree['primary'] === $second->objectTree['primary']);
	echo $name, '-second-route=', $second->scalarTree['routes'][0]['name'], "\n";
	echo $name, '-second-child=', $second->objectTree['primary']->label, "\n";

	$second->scalarTree['meta']['owner'] = $name . '-second-mutated-owner';
	echo $name, '-first-owner=', $first->scalarTree['meta']['owner'], "\n";

	$third = $cache->fetch('object_array_property');
	echo $name, '-third-owner=', $third->scalarTree['meta']['owner'], "\n";
	echo $name, '-third-child=', $third->objectTree['primary']->label, "\n";
}

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

check_array_property_clone('volatile', OPcache\VolatileCache::getInstance('default'));
check_array_property_clone('stable', OPcache\StableCache::getInstance('default'));

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
volatile-second-route=volatile-route-0
volatile-second-child=volatile-child
volatile-first-owner=volatile
volatile-third-owner=volatile
volatile-third-child=volatile-child
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
stable-second-route=stable-route-0
stable-second-child=stable-child
stable-first-owner=stable
stable-third-owner=stable
stable-third-child=stable-child
