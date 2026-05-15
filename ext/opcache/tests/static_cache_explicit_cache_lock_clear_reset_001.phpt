--TEST--
OPcache explicit cache clear and reset release cache locks
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_clear();
	} else {
		OPcache\persistent_clear();
	}
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_lock($key)
		: OPcache\persistent_lock($key);
}

function cache_delete(string $backend, string $key): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_delete($key);
	} else {
		OPcache\persistent_delete($key);
	}
}

foreach (['volatile', 'persistent'] as $backend) {
	$key = $backend . '_lock_clear_reset';

	echo $backend, "\n";
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	cache_delete($backend, $key);
	var_dump(cache_lock($backend, $key));
}

OPcache\volatile_clear();
OPcache\persistent_clear();

var_dump(OPcache\volatile_lock('opcache_reset_volatile_lock'));
var_dump(OPcache\persistent_lock('opcache_reset_persistent_lock'));
var_dump(opcache_reset());
var_dump(OPcache\volatile_lock('opcache_reset_volatile_lock'));
var_dump(OPcache\persistent_lock('opcache_reset_persistent_lock'));

?>
--EXPECT--
volatile
bool(true)
bool(true)
bool(true)
persistent
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
