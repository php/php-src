--TEST--
OPcache explicit store_array APIs reject non-string keys before storing any entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

foreach ([
	'volatile' => [OPcache\VolatileCache::getInstance('default')->storeMultiple(...), OPcache\VolatileCache::getInstance('default')->fetch(...)],
	'stable' => [OPcache\StableCache::getInstance('default')->storeMultiple(...), OPcache\StableCache::getInstance('default')->fetch(...)],
] as $label => [$storeArray, $fetch]) {
	$key = $label . '_first';

	try {
		$storeArray([
			$key => 'stored',
			0 => 'bad',
		]);
	} catch (ValueError $exception) {
		echo $label, ': ', $exception->getMessage(), "\n";
	}

	var_dump($fetch($key, 'missing'));
}

?>
--EXPECT--
volatile: OPcache\VolatileCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
string(7) "missing"
stable: OPcache\StableCache::storeMultiple(): Argument #1 ($values) must be an array with non-empty string keys that do not contain ":" and are not reserved static-cache class keys or loaded class names
string(7) "missing"
