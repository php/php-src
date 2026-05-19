--TEST--
OPcache explicit cache locks can reserve keys until store
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
		OPcache\volatile_clear();
	} else {
		OPcache\pinned_clear();
	}
}

function cache_store(string $backend, string $key, mixed $value): mixed
{
	if ($backend === 'volatile') {
		return OPcache\volatile_store($key, $value);
	}

	OPcache\pinned_store($key, $value);
	return null;
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\volatile_fetch($key, $default)
		: OPcache\pinned_fetch($key, $default);
}

function cache_exists(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_exists($key)
		: OPcache\pinned_exists($key);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_lock($key)
		: OPcache\pinned_lock($key);
}

foreach (['volatile', 'pinned'] as $backend) {
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
pinned
bool(true)
bool(true)
bool(true)
string(5) "built"
