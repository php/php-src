--TEST--
OPcache explicit cache locks can be manually unlocked
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

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_store($key, $value);
	} else {
		OPcache\persistent_store($key, $value);
	}
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\volatile_fetch($key, $default)
		: OPcache\persistent_fetch($key, $default);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_lock($key)
		: OPcache\persistent_lock($key);
}

function cache_unlock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_unlock($key)
		: OPcache\persistent_unlock($key);
}

foreach (['volatile', 'persistent'] as $backend) {
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
persistent
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
string(9) "published"
