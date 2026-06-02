--TEST--
OPcache explicit cache clear and reset release cache locks
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

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::lock($key)
		: OPcache\PinnedCache::lock($key);
}

function cache_delete(string $backend, string $key): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::delete($key);
	} else {
		OPcache\PinnedCache::delete($key);
	}
}

foreach (['volatile', 'pinned'] as $backend) {
	$key = $backend . '_lock_clear_reset';

	echo $backend, "\n";
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	cache_delete($backend, $key);
	var_dump(cache_lock($backend, $key));
}

OPcache\VolatileCache::clear();
OPcache\PinnedCache::clear();

var_dump(OPcache\VolatileCache::lock('opcache_reset_volatile_lock'));
var_dump(OPcache\PinnedCache::lock('opcache_reset_pinned_lock'));
var_dump(opcache_reset());
var_dump(OPcache\VolatileCache::lock('opcache_reset_volatile_lock'));
var_dump(OPcache\PinnedCache::lock('opcache_reset_pinned_lock'));

?>
--EXPECT--
volatile
bool(true)
bool(true)
bool(true)
pinned
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
