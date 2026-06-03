--TEST--
OPcache explicit fetch_array APIs handle hits, missing keys, and default values
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		opcache_static_cache_volatile_reset();
	} else {
		OPcache\StableCache::getInstance('default')->clear();
	}
}

function cache_store_array(string $backend, array $values): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::getInstance('default')->storeMultiple($values);
	} else {
		OPcache\StableCache::getInstance('default')->storeMultiple($values);
	}
}

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::getInstance('default')->store($key, $value);
	} else {
		OPcache\StableCache::getInstance('default')->store($key, $value);
	}
}

function cache_fetch_array(string $backend, array $keys, ?array $default = null): ?array
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->fetchMultiple($keys, $default)
		: OPcache\StableCache::getInstance('default')->fetchMultiple($keys, $default);
}

foreach (['volatile', 'stable'] as $backend) {
	echo $backend, "\n";
	cache_clear($backend);
	cache_store_array($backend, [
		'hit' => 'value',
		'null' => null,
	]);
	cache_store($backend, '42', 'numeric');

	var_dump(cache_fetch_array($backend, ['hit', 'missing', 'null']));
	var_dump(cache_fetch_array($backend, ['hit', 'missing', 'null'], ['fallback']));
	var_dump(cache_fetch_array($backend, [42]));
	var_dump(cache_fetch_array($backend, []));
}

?>
--EXPECT--
volatile
array(3) {
  ["hit"]=>
  string(5) "value"
  ["missing"]=>
  NULL
  ["null"]=>
  NULL
}
array(3) {
  ["hit"]=>
  string(5) "value"
  ["missing"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
  ["null"]=>
  NULL
}
array(1) {
  ["42"]=>
  string(7) "numeric"
}
array(0) {
}
stable
array(3) {
  ["hit"]=>
  string(5) "value"
  ["missing"]=>
  NULL
  ["null"]=>
  NULL
}
array(3) {
  ["hit"]=>
  string(5) "value"
  ["missing"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
  ["null"]=>
  NULL
}
array(1) {
  ["42"]=>
  string(7) "numeric"
}
array(0) {
}
