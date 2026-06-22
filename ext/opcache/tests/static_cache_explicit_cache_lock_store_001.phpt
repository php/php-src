--TEST--
OPcache explicit cache locks can reserve keys until store
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

function cache_store(string $backend, string $key, mixed $value): mixed
{
	if ($backend === 'volatile') {
		return OPcache\VolatileCache::getInstance('default')->store($key, $value);
	}

	OPcache\StableCache::getInstance('default')->store($key, $value);
	return null;
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->fetch($key, $default)
		: OPcache\StableCache::getInstance('default')->fetch($key, $default);
}

function cache_exists(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->has($key)
		: OPcache\StableCache::getInstance('default')->has($key);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->lock($key)
		: OPcache\StableCache::getInstance('default')->lock($key);
}

foreach (['volatile', 'stable'] as $backend) {
	$key = $backend . '_exists_lock_store';

	echo $backend, "\n";
	cache_clear($backend);

	var_dump(cache_lock($backend, $key));
	cache_store($backend, $key, 'built');
	var_dump(cache_exists($backend, $key));
	var_dump(cache_lock($backend, $key));
	var_dump(cache_fetch($backend, $key));
}

?>
--EXPECT--
volatile
bool(true)
bool(true)
bool(true)
string(5) "built"
stable
bool(true)
bool(true)
bool(true)
string(5) "built"
