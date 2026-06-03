--TEST--
OPcache explicit cache clear and reset release cache locks
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

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->lock($key)
		: OPcache\StableCache::getInstance('default')->lock($key);
}

function cache_delete(string $backend, string $key): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::getInstance('default')->delete($key);
	} else {
		OPcache\StableCache::getInstance('default')->delete($key);
	}
}

foreach (['volatile', 'stable'] as $backend) {
	$key = $backend . '_lock_clear_reset';

	echo $backend, "\n";
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	cache_delete($backend, $key);
	var_dump(cache_lock($backend, $key));
}

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

var_dump(OPcache\VolatileCache::getInstance('default')->lock('opcache_reset_volatile_lock'));
var_dump(OPcache\StableCache::getInstance('default')->lock('opcache_reset_stable_lock'));
var_dump(opcache_reset());
var_dump(OPcache\VolatileCache::getInstance('default')->lock('opcache_reset_volatile_lock'));
var_dump(OPcache\StableCache::getInstance('default')->lock('opcache_reset_stable_lock'));

?>
--EXPECT--
volatile
bool(true)
bool(true)
bool(true)
stable
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
