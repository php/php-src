--TEST--
OPcache explicit cache locks can be manually unlocked
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::clear();
	} else {
		OPcache\PinnedCache::clear();
	}
}

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::set($key, $value);
	} else {
		OPcache\PinnedCache::set($key, $value);
	}
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::get($key, $default)
		: OPcache\PinnedCache::get($key, $default);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::lock($key)
		: OPcache\PinnedCache::lock($key);
}

function cache_unlock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::unlock($key)
		: OPcache\PinnedCache::unlock($key);
}

foreach (['volatile', 'pinned'] as $backend) {
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
pinned
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
string(9) "published"
