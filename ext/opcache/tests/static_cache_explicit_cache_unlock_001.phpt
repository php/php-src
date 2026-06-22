--TEST--
OPcache explicit cache locks can be manually unlocked
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

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::getInstance('default')->store($key, $value);
	} else {
		OPcache\StableCache::getInstance('default')->store($key, $value);
	}
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->fetch($key, $default)
		: OPcache\StableCache::getInstance('default')->fetch($key, $default);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->lock($key)
		: OPcache\StableCache::getInstance('default')->lock($key);
}

function cache_unlock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->unlock($key)
		: OPcache\StableCache::getInstance('default')->unlock($key);
}

foreach (['volatile', 'stable'] as $backend) {
	$key = $backend . '_manual_unlock';

	echo $backend, "\n";
	cache_clear($backend);

	var_dump(cache_unlock($backend, $key));
	var_dump(cache_lock($backend, $key));
	var_dump(cache_lock($backend, $key));
	var_dump(cache_unlock($backend, $key));
	var_dump(cache_unlock($backend, $key));
	var_dump(cache_lock($backend, $key));
	cache_store($backend, $key, 'published');
	var_dump(cache_unlock($backend, $key));
	var_dump(cache_fetch($backend, $key));
}

?>
--EXPECT--
volatile
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
string(9) "published"
stable
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
string(9) "published"
